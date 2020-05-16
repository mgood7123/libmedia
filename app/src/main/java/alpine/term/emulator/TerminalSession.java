/*
*************************************************************************
Alpine Term - a VM-based terminal emulator.
Copyright (C) 2019-2020  Leonid Plyushch <leonid.plyushch@gmail.com>

Originally was part of Termux.
Copyright (C) 2019  Fredrik Fornwall <fredrik@fornwall.net>

This program is free software: you can redistribute it and/or modify
it under the terms of the GNU General Public License as published by
the Free Software Foundation, either version 3 of the License, or
(at your option) any later version.

This program is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
GNU General Public License for more details.

You should have received a copy of the GNU General Public License
along with this program.  If not, see <http://www.gnu.org/licenses/>.
*************************************************************************
*/
package alpine.term.emulator;

import android.annotation.SuppressLint;
import android.content.Context;
import android.os.Handler;
import android.os.Message;
import android.system.ErrnoException;
import android.system.Os;
import android.system.OsConstants;

import libclient_service.InfiniteLoop;

import java.io.File;
import java.io.FileDescriptor;
import java.io.FileInputStream;
import java.io.FileOutputStream;
import java.nio.charset.StandardCharsets;
import java.util.UUID;
import java.util.concurrent.atomic.AtomicBoolean;

import alpine.term.Config;
import alpine.term.LogUtils;
import alpine.term.TrackedActivity;

import static alpine.term.TrackedActivity.wrapFileDescriptor;

/**
 * A terminal session, consisting of a process coupled to a terminal interface.
 * <p>
 * The subprocess will be executed by the constructor, and when the size is made known by a call to
 * {@link #updateSize(int, int, Context)} terminal emulation will begin and threads will be spawned to handle the subprocess I/O.
 * All terminal emulation and callback methods will be performed on the main thread.
 * <p>
 * The child process may be exited forcefully by using the {@link #finishIfRunning()} method.
 * <p>
 * NOTE: The terminal session may outlive the EmulatorView, so be careful with callbacks!
 */
public final class TerminalSession extends TerminalOutput {

    LogUtils logUtils = new LogUtils("Terminal Session");

    /**
     * true if this is associated with a client, otherwise false
     */
    public boolean isTrackedActivity;
    /**
     * the pid of the associated client, -1 if {@link #isTrackedActivity} is false
     */
    public int trackedActivityPid;

    /** Callback to be invoked when a {@link TerminalSession} changes. */
    public interface SessionChangedCallback {
        void onTextChanged(TerminalSession changedSession);

        void onTitleChanged(TerminalSession changedSession);

        void onSessionFinished(TerminalSession finishedSession);

        void onClipboardText(TerminalSession session, String text);

        void onBell(TerminalSession session);

        void onColorsChanged(TerminalSession session);

    }

    private static final int MSG_NEW_INPUT = 1;
    private static final int MSG_PROCESS_EXITED = 4;

    public final String mHandle = UUID.randomUUID().toString();

    TerminalEmulator mEmulator;

    /**
     * A queue written to from a separate thread when the process outputs, and read by main thread to process by
     * terminal emulator.
     */
    final ByteQueue mProcessToTerminalIOQueue = new ByteQueue(4096);
    /**
     * A queue written to from the main thread due to user interaction, and read by another thread which forwards by
     * writing to the {@link #mTerminalFileDescriptor}.
     */
    final ByteQueue mTerminalToProcessIOQueue = new ByteQueue(4096);
    /** Buffer to write translate code points into utf8 before writing to mTerminalToProcessIOQueue */
    private final byte[] mUtf8InputBuffer = new byte[5];

    /** Callback which gets notified when a session finishes or changes title. */
    final SessionChangedCallback mChangeCallback;

    /** The pid of the shell process. 0 if not started and -1 if finished running. */
    int mShellPid;

    /** The exit status of the shell process. Only valid if ${@link #mShellPid} is -1. */
    int mShellExitStatus;

    /**
     * The file descriptor referencing the master half of a pseudo-terminal pair, resulting from calling
     * {@link JNI#createSubprocess(String, String, String[], String[], int[], int, int)}.
     */
    public int mTerminalFileDescriptor;

    /** Set by the application for user identification of session, not by terminal. */
    public String mSessionName;

    @SuppressLint("HandlerLeak")
    final Handler mMainThreadHandler = new Handler() {
        final byte[] mReceiveBuffer = new byte[4 * 1024];

        @Override
        public void handleMessage(Message msg) {
            if (msg.what == MSG_NEW_INPUT && isRunning()) {
                int bytesRead = mProcessToTerminalIOQueue.read(mReceiveBuffer, false);
                if (bytesRead > 0) {
                    mEmulator.append(mReceiveBuffer, bytesRead);
                    notifyScreenUpdate();
                }
            } else if (msg.what == MSG_PROCESS_EXITED) {
                int exitCode = (Integer) msg.obj;
                cleanupResources(exitCode);
                mChangeCallback.onSessionFinished(TerminalSession.this);

                String exitDescription = "\r\n[Process completed";
                if (exitCode > 0) {
                    // Non-zero process exit.
                    exitDescription += " (code " + exitCode + ")";
                } else if (exitCode < 0) {
                    // Negated signal.
                    exitDescription += " (signal " + (-exitCode) + ")";
                }
                exitDescription += "]";

                byte[] bytesToWrite = exitDescription.getBytes(StandardCharsets.UTF_8);
                mEmulator.append(bytesToWrite, bytesToWrite.length);
                notifyScreenUpdate();
            }
        }
    };

    private final String mShellPath;
    private final String[] mArgs;
    private final String[] mEnv;
    private final String mCwd;
    private final boolean isLogView;
    private final TrackedActivity trackedActivity;
    private final int pseudoTerminal;
    private final boolean printWelcomeMessage;

    public TerminalSession(boolean isLogView, String shellPath, String[] args, String[] env, String cwd, SessionChangedCallback changeCallback, TrackedActivity trackedActivity, int[] pseudoTerminal, boolean printWelcomeMessage, Context context) {
        mChangeCallback = changeCallback;

        this.trackedActivity = trackedActivity;
        if (pseudoTerminal == null) this.pseudoTerminal = -1;
        else this.pseudoTerminal = pseudoTerminal[0];
        if (trackedActivity != null) {
            this.isTrackedActivity = true;
            this.trackedActivityPid = trackedActivity.pid;
        } else {
            this.isTrackedActivity = false;
            this.trackedActivityPid = -1;
        }
        this.isLogView = isLogView;
        this.mShellPath = shellPath;
        this.mArgs = args;
        this.mEnv = env;
        this.mCwd = cwd;
        this.printWelcomeMessage = printWelcomeMessage;
        updateSize(80,80, context);
    }

    /** Inform the attached pty of the new size and reflow or initialize the emulator. */
    public void updateSize(int columns, int rows, Context context) {
        if (mEmulator == null) {
            initializeEmulator(columns, rows, context);
        } else {
            JNI.setPtyWindowSize(mTerminalFileDescriptor, rows, columns);
            mEmulator.resize(columns, rows);
        }
    }

    /** The terminal title as set through escape sequences or null if none set. */
    public String getTitle() {
        return (mEmulator == null) ? null : mEmulator.getTitle();
    }

    AtomicBoolean TermSessionInputReader___shell__running = new AtomicBoolean(false);
    AtomicBoolean TermSessionOutputWriter__shell__running = new AtomicBoolean(false);
    AtomicBoolean TermSessionInputReader___logcat_running = new AtomicBoolean(false);
    AtomicBoolean logcat_restarter_running = new AtomicBoolean(false);
    AtomicBoolean TermSessionInputReader___log____running = new AtomicBoolean(false);
    public AtomicBoolean sessionIsAlive = new AtomicBoolean(true);

    public void waitForExit() {
        if (TermSessionInputReader___shell__running.get() || TermSessionOutputWriter__shell__running.get()) {
            JNI.puts("waiting for shell to stop...");
            boolean stopped = false;
            while (!stopped)
                if (
                    !TermSessionInputReader___shell__running.get() &&
                    !TermSessionOutputWriter__shell__running.get()
                ) {
                    JNI.puts("shell has stopped");
                    stopped = true;
                }
        }
        if (TermSessionInputReader___logcat_running.get() || logcat_restarter_running.get()) {
            JNI.puts("waiting for logcat to stop...");
            boolean stopped = false;
            while (!stopped)
                if (
                    !TermSessionInputReader___logcat_running.get() &&
                    !logcat_restarter_running.get()
                ) {
                    JNI.puts("logcat has stopped");
                    stopped = true;
                }
        }
        if (TermSessionInputReader___log____running.get()) {
            JNI.puts("waiting for log to stop...");
            boolean stopped = false;
            while (!stopped)
                if (!TermSessionInputReader___log____running.get()) {
                    JNI.puts("log has stopped");
                    stopped = true;
                }
        }
    }

    public void createShellSession(int columns, int rows) {
        logUtils.log_Info("creating shell");
        logUtils.errorAndThrowIfNull(mArgs);
        logUtils.errorAndThrowIfNull(mEnv);
        int[] processId = new int[1];
        mTerminalFileDescriptor = JNI.createSubprocess(mShellPath, mCwd, mArgs, mEnv, processId, rows, columns);
        mShellPid = processId[0];
        logUtils.log_Info("created shell");

        final FileDescriptor terminalFileDescriptorWrapped = wrapFileDescriptor(mTerminalFileDescriptor);
        if (terminalFileDescriptorWrapped == null) throw new NullPointerException();

        new Thread("TermSessionInputReader (stdout/stderr) [pid=" + mShellPid + "]") {
            @Override
            public void run() {
                try (FileInputStream termIn = new FileInputStream(terminalFileDescriptorWrapped)) {
                    final byte[] buffer = new byte[4096];
                    TermSessionInputReader___shell__running.set(true);
                    InfiniteLoop infiniteLoop = new InfiniteLoop();
                    infiniteLoop.setSleepTimeInMicroseconds(500);
                    infiniteLoop.loopMayThrowException(() -> sessionIsAlive.get(), () -> {
                        if (termIn.available() != 0) {
                            int read = termIn.read(buffer);
                            if (read == -1) {
                                logUtils.log_Error("stdout/stderr reader return -1");
                                TermSessionInputReader___shell__running.set(false);
                                return;
                            }
                            if (!mProcessToTerminalIOQueue.write(buffer, 0, read)) {
                                logUtils.log_Error("stdout/stderr reader [write] returned false (closed)");
                                TermSessionInputReader___shell__running.set(false);
                                return;
                            }
                            mMainThreadHandler.sendEmptyMessage(MSG_NEW_INPUT);
                        }
                    });
                    TermSessionInputReader___shell__running.set(false);
                } catch (Exception e) {
                    // Ignore, just shutting down.
                    TermSessionInputReader___shell__running.set(false);
                }
            }
        }.start();

        new Thread("TermSessionOutputWriter (stdin) [pid=" + mShellPid + "]") {
            @Override
            public void run() {
                final byte[] buffer = new byte[4096];
                try (FileOutputStream termOut = new FileOutputStream(terminalFileDescriptorWrapped)) {
                    TermSessionOutputWriter__shell__running.set(true);
                    InfiniteLoop infiniteLoop = new InfiniteLoop();
                    infiniteLoop.setSleepTimeInMicroseconds(500);
                    infiniteLoop.loopMayThrowException(() -> sessionIsAlive.get(), () -> {
                        int bytesToWrite = mTerminalToProcessIOQueue.read(buffer, true);
                        if (bytesToWrite == -1) {
                            logUtils.log_Error("stdin writer return -1");
                            TermSessionOutputWriter__shell__running.set(false);
                            return;
                        }
                        termOut.write(buffer, 0, bytesToWrite);
                    });
                    TermSessionOutputWriter__shell__running.set(false);
                } catch (Exception e) {
                    // Ignore.
                    TermSessionOutputWriter__shell__running.set(false);
                }
            }
        }.start();
    }

    public void createLogSession(int columns, int rows, Context context) {
        logUtils.errorAndThrowIfNull(context, "a context is required");
        logUtils.log_Info("creating log");
        final FileDescriptor terminalFileDescriptorWrapped;
        if (isTrackedActivity || pseudoTerminal != -1) {
            logUtils.log_Info("using existing FileDescriptor");
            if (isTrackedActivity) {
                logUtils.errorAndThrowIfNull(
                        trackedActivity.pseudoTerminalMaster, "trackedActivity.pseudoTerminalMaster IS NULL"
                );
                terminalFileDescriptorWrapped = trackedActivity.pseudoTerminalMaster.getFileDescriptor();
                mShellPid = trackedActivityPid;
            } else {
                logUtils.errorAndThrowIfNull(
                        pseudoTerminal, "pseudoTerminal IS NULL"
                );
                terminalFileDescriptorWrapped = wrapFileDescriptor(pseudoTerminal);
                mShellPid = JNI.getPid();
            }
        } else {
            logUtils.log_Info("creating new FileDescriptor");
            int fd = JNI.createPseudoTerminal(printWelcomeMessage)[0];
            terminalFileDescriptorWrapped = wrapFileDescriptor(fd);
            mShellPid = JNI.getPid();
        }
        logUtils.errorAndThrowIfNull(terminalFileDescriptorWrapped);
        JNI.puts("created log");
        logUtils.log_Info("created log");

        new Thread("TermSessionInputReader (stdout/stderr) [log (pid=" + mShellPid + ")]") {
            @Override
            public void run() {
                try (FileInputStream termIn = new FileInputStream(terminalFileDescriptorWrapped)) {
                    final byte[] buffer = new byte[4096];
                    File log = new File(Config.getDataDirectory(context) + "/native_log.txt");
                    if (!log.exists()) log.createNewFile();
                    if (!log.canRead()) log.setReadable(true);
                    if (!log.canWrite()) log.setWritable(true);
                    FileOutputStream logFile = new FileOutputStream(log);
                    FileDescriptor logFileFileDescriptor = logFile.getFD();
                    InfiniteLoop infiniteLoop = new InfiniteLoop();
                    infiniteLoop.setSleepTimeInMicroseconds(500);
                    infiniteLoop.loopMayThrowException(() -> sessionIsAlive.get(), () -> {
                        if (termIn.available() != 0) {
                            int read = termIn.read(buffer);
                            if (read == -1) {
                                logUtils.log_Error("stdout/stderr reader return -1");
                                TermSessionInputReader___log____running.set(false);
                                return;
                            }
                            logFile.write(buffer, 0, read);
                            logFileFileDescriptor.sync();
                            if (!mProcessToTerminalIOQueue.write(buffer, 0, read)) {
                                logUtils.log_Error("stdout/stderr reader [write] returned false (closed)");
                                TermSessionInputReader___log____running.set(false);
                                return;
                            }
                            mMainThreadHandler.sendEmptyMessage(MSG_NEW_INPUT);
                        }
                    });
                    TermSessionInputReader___log____running.set(false);
                } catch (Exception e) {
                    // Ignore, just shutting down.
                    TermSessionInputReader___log____running.set(false);
                }
            }
        }.start();
    }

    public void createLogcatSession(int columns, int rows, Context context) {
        logUtils.errorAndThrowIfNull(context, "a context is required");
        logUtils.log_Info("creating Logcat");
        logUtils.errorAndThrowIfNull(mArgs);
        logUtils.errorAndThrowIfNull(mEnv);
        int[] processId = new int[1];
        mTerminalFileDescriptor = JNI.createSubprocess(mShellPath, mCwd, mArgs, mEnv, processId, rows, columns);
        mShellPid = processId[0];
        JNI.puts("created Logcat");
        logUtils.log_Info("created Logcat");

        final FileDescriptor terminalFileDescriptorWrapped = wrapFileDescriptor(mTerminalFileDescriptor);
        logUtils.errorAndThrowIfNull(terminalFileDescriptorWrapped);

        new Thread("TermSessionInputReader (stdout/stderr) [Logcat (pid=" + mShellPid + ")]") {
            @Override
            public void run() {
                try (FileInputStream termIn = new FileInputStream(terminalFileDescriptorWrapped)) {
                    final byte[] buffer = new byte[4096];
                    File log = new File(Config.getDataDirectory(context) + "/logcat.txt");
                    if (!log.exists()) log.createNewFile();
                    if (!log.canRead()) log.setReadable(true);
                    if (!log.canWrite()) log.setWritable(true);
                    FileOutputStream logFile = new FileOutputStream(log);
                    FileDescriptor logFileFileDescriptor = logFile.getFD();
                    TermSessionInputReader___logcat_running.set(true);
                    InfiniteLoop infiniteLoop = new InfiniteLoop();
                    infiniteLoop.setSleepTimeInMicroseconds(500);
                    infiniteLoop.loopMayThrowException(() -> sessionIsAlive.get(), () -> {
                        if (termIn.available() != 0) {
                            int read = termIn.read(buffer);
                            if (read == -1) {
                                logUtils.log_Error("stdout/stderr reader return -1");
                                TermSessionInputReader___logcat_running.set(false);
                                return;
                            }
                            logFile.write(buffer, 0, read);
                            logFileFileDescriptor.sync();
                            if (!mProcessToTerminalIOQueue.write(buffer, 0, read)) {
                                logUtils.log_Error("stdout/stderr reader [write] returned false (closed)");
                                TermSessionInputReader___logcat_running.set(false);
                                return;
                            }
                            mMainThreadHandler.sendEmptyMessage(MSG_NEW_INPUT);
                        }
                    });
                    TermSessionInputReader___logcat_running.set(false);
                } catch (Exception e) {
                    // Ignore, just shutting down.
                    TermSessionInputReader___logcat_running.set(false);
                }
            }
        }.start();
    }

    boolean should_exit = false;

    /**
     * Set the terminal emulator's window size and start terminal emulation.
     *
     * @param columns The number of columns in the terminal window.
     * @param rows    The number of rows in the terminal window.
     */
    public void initializeEmulator(int columns, int rows, Context context) {
        mEmulator = new TerminalEmulator(this, columns, rows, /* transcript= */5000);
        mEmulator.currentFontSize = 24;
        mEmulator.appendLine("Terminal Emulator: Initializing...");

        if (isLogView) {
            // should this be true for a shell as well?
            mEmulator.isLogView = true;
            if (
                mShellPath.contentEquals("/bin/logcat") ||
                mShellPath.contentEquals("/sbin/su") ||
                mShellPath.contentEquals("/sbin/magisk")
            ) {
                createLogcatSession(columns, rows, context);
                new Thread("TermSession[pid=" + mShellPid + "]") {
                    @Override
                    public void run() {
                        logcat_restarter_running.set(true);
                        while (sessionIsAlive.get()) {
                            int processExitCode = JNI.waitFor(mShellPid);
                            mEmulator.appendLine("Logcat returned " + processExitCode);
                            if (sessionIsAlive.get()) {
                                mEmulator.appendLine("restarting...");
                                createLogcatSession(columns, rows, context);
                            }
                        }
                        logcat_restarter_running.set(false);
                    }
                }.start();
            } else {
                createLogSession(columns, rows, context);
            }
        } else {
            createShellSession(columns, rows);
            new Thread("TermSession[pid=" + mShellPid + "]") {
                @Override
                public void run() {
                    int processExitCode = JNI.waitFor(mShellPid);
                    mEmulator.appendLine("shell returned " + processExitCode);
                    should_exit = true;
                    mMainThreadHandler.sendMessage(mMainThreadHandler.obtainMessage(MSG_PROCESS_EXITED, processExitCode));
                }
            }.start();
        }
        mEmulator.appendLine("Terminal Emulator: Initialized");
        String fmt = "emulator initialized";
        JNI.puts(fmt);
        logUtils.log_Info(fmt);
    }

    /** Write data to the shell process. */
    @Override
    public void write(byte[] data, int offset, int count) {
        if (!isLogView) if (mShellPid > 0) mTerminalToProcessIOQueue.write(data, offset, count);
    }

    /** Write the Unicode code point to the terminal encoded in UTF-8. */
    public void writeCodePoint(boolean prependEscape, int codePoint) {
        if (codePoint > 1114111 || (codePoint >= 0xD800 && codePoint <= 0xDFFF)) {
            // 1114111 (= 2**16 + 1024**2 - 1) is the highest code point, [0xD800,0xDFFF] is the surrogate range.
            throw new IllegalArgumentException("invalid code point: " + codePoint);
        }

        int bufferPosition = 0;
        if (prependEscape) mUtf8InputBuffer[bufferPosition++] = 27;

        if (codePoint <= /* 7 bits */0b1111111) {
            mUtf8InputBuffer[bufferPosition++] = (byte) codePoint;
        } else if (codePoint <= /* 11 bits */0b11111111111) {
            /* 110xxxxx leading byte with leading 5 bits */
            mUtf8InputBuffer[bufferPosition++] = (byte) (0b11000000 | (codePoint >> 6));
			/* 10xxxxxx continuation byte with following 6 bits */
            mUtf8InputBuffer[bufferPosition++] = (byte) (0b10000000 | (codePoint & 0b111111));
        } else if (codePoint <= /* 16 bits */0b1111111111111111) {
			/* 1110xxxx leading byte with leading 4 bits */
            mUtf8InputBuffer[bufferPosition++] = (byte) (0b11100000 | (codePoint >> 12));
			/* 10xxxxxx continuation byte with following 6 bits */
            mUtf8InputBuffer[bufferPosition++] = (byte) (0b10000000 | ((codePoint >> 6) & 0b111111));
			/* 10xxxxxx continuation byte with following 6 bits */
            mUtf8InputBuffer[bufferPosition++] = (byte) (0b10000000 | (codePoint & 0b111111));
        } else { /* We have checked codePoint <= 1114111 above, so we have max 21 bits = 0b111111111111111111111 */
			/* 11110xxx leading byte with leading 3 bits */
            mUtf8InputBuffer[bufferPosition++] = (byte) (0b11110000 | (codePoint >> 18));
			/* 10xxxxxx continuation byte with following 6 bits */
            mUtf8InputBuffer[bufferPosition++] = (byte) (0b10000000 | ((codePoint >> 12) & 0b111111));
			/* 10xxxxxx continuation byte with following 6 bits */
            mUtf8InputBuffer[bufferPosition++] = (byte) (0b10000000 | ((codePoint >> 6) & 0b111111));
			/* 10xxxxxx continuation byte with following 6 bits */
            mUtf8InputBuffer[bufferPosition++] = (byte) (0b10000000 | (codePoint & 0b111111));
        }
        write(mUtf8InputBuffer, 0, bufferPosition);
    }

    public TerminalEmulator getEmulator() {
        return mEmulator;
    }

    /** Notify the {@link #mChangeCallback} that the screen has changed. */
    protected void notifyScreenUpdate() {
        mChangeCallback.onTextChanged(this);
    }

    /** Reset state for terminal emulator state. */
    public void reset(boolean erase) {
        mEmulator.reset(erase);
        notifyScreenUpdate();
    }

    /** Finish this terminal session by sending SIGKILL to the shell. */
    public void finishIfRunning() {
        if (isRunning()) {
            if (!isLogView) {
                try {
                    Os.kill(mShellPid, OsConstants.SIGKILL);
                } catch (ErrnoException e) {
                    logUtils.log_Error("failed sending SIGKILL: " + e.getMessage());
                }
            }
        }
    }

    /** Cleanup resources when the process exits. */
    void cleanupResources(int exitStatus) {
        synchronized (this) {
            if (!isLogView) {
                mShellPid = -1;
                mShellExitStatus = exitStatus;
            }
        }

        // Stop the reader and writer threads, and close the I/O streams
        mTerminalToProcessIOQueue.close();
        mProcessToTerminalIOQueue.close();
        JNI.close(mTerminalFileDescriptor);
    }

    @Override
    public void titleChanged(String oldTitle, String newTitle) {
        mChangeCallback.onTitleChanged(this);
    }

    public synchronized boolean isRunning() {
        return mShellPid != -1 && !should_exit;
    }

    /** Only valid if not {@link #isRunning()}. */
    public synchronized int getExitStatus() {
        return mShellExitStatus;
    }

    @Override
    public void clipboardText(String text) {
        mChangeCallback.onClipboardText(this, text);
    }

    @Override
    public void onBell() {
        mChangeCallback.onBell(this);
    }

    @Override
    public void onColorsChanged() {
        mChangeCallback.onColorsChanged(this);
    }

    public int getPid() {
        return mShellPid;
    }

    public boolean isLogView() {
        return isLogView;
    }

    public boolean isShell() {
        return !isLogView;
    }
}

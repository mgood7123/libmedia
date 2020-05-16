package alpine.term;

import android.app.Activity;
import android.content.ClipData;
import android.content.ClipboardManager;
import android.content.ComponentName;
import android.content.Context;
import android.content.Intent;
import android.content.pm.ActivityInfo;
import android.content.pm.PackageManager;
import android.content.res.Resources;
import android.graphics.Color;
import android.graphics.Paint;
import android.graphics.Typeface;
import android.os.Bundle;
import android.os.IBinder;
import android.text.SpannableString;
import android.text.Spanned;
import android.text.TextUtils;
import android.text.style.StyleSpan;
import android.view.View;
import android.view.ViewGroup;
import android.widget.ArrayAdapter;
import android.widget.ListView;
import android.widget.TextView;

import androidx.annotation.NonNull;

import libclient_service.LibService_Messenger;
import libclient_service.LibService_Service_Connection;

import java.util.ArrayList;
import java.util.Locale;

import alpine.term.emulator.JNI;
import alpine.term.emulator.TerminalSession;
import media.player.pro.R;

public class TerminalControllerService extends LibService_Service_Connection {

    public int[] pseudoTerminal = null;
    TerminalService mTerminalService = null;
    TerminalControllerService terminalControllerService = this;

    // this value is set when this instance is created
    TerminalController terminalController = null;

    @Override
    public void onServiceConnectedCallback(IBinder boundService) {
        mTerminalService = (TerminalService) service;
        terminalControllerService = (TerminalControllerService) service.manager;

        log.errorAndThrowIfNull(mTerminalService, terminalControllerService);

        mTerminalService.terminalControllerService = this;

        terminalController.mTermService = mTerminalService;
        terminalController.terminalControllerService = terminalControllerService;
        context = terminalController.activity;

        mTerminalService.mSessionChangeCallback = new TerminalSession.SessionChangedCallback() {
            @Override
            public void onTextChanged(TerminalSession changedSession) {
                if (!terminalController.mIsVisible) return;
                if (terminalController.mTerminalView.getCurrentSession() == changedSession) {
                    terminalController.activity.runOnUiThread(new Runnable() {
                        @Override
                        public void run() {
                            if (!LibService_Messenger.mainThread.equals(Thread.currentThread())) {
                                logUtils.errorAndThrow(
                                    "ERROR: runOnUiThread IS NOT BEING INVOKED ON REAL UI THREAD"
                                );
                            }
                            terminalController.mTerminalView.onScreenUpdated();
                        }
                    });
                }
            }

            @Override
            public void onTitleChanged(TerminalSession updatedSession) {
                if (!terminalController.mIsVisible) return;
                if (updatedSession != terminalController.mTerminalView.getCurrentSession()) {
                    // Only show toast for other sessions than the current one, since the user
                    // probably consciously caused the title change to change in the current session
                    // and don't want an annoying toast for that.
                    terminalController.showToast(terminalController.toToastTitle(updatedSession), false);
                }
                terminalController.activity.runOnUiThread(new Runnable() {
                    @Override
                    public void run() {
                        if (!LibService_Messenger.mainThread.equals(Thread.currentThread())) {
                            logUtils.errorAndThrow(
                                "ERROR: runOnUiThread IS NOT BEING INVOKED ON REAL UI THREAD"
                            );
                        }
                        mListViewAdapter.notifyDataSetChanged();
                    }
                });
            }

            @Override
            public void onSessionFinished(final TerminalSession finishedSession) {
                if (mTerminalService.getSessions().isEmpty()) {
                    if (mTerminalService.mWantsToStop) {
                        // The service wants to stop as soon as possible.
                        terminalController.activity.finish();
                        return;
                    }

                    mTerminalService.terminateService();
                } else {
                    terminalController.switchToPreviousSession();
                    mTerminalService.removeSession(finishedSession);
                }
            }

            @Override
            public void onClipboardText(TerminalSession session, String text) {
                if (!terminalController.mIsVisible) return;
                ClipboardManager clipboard = (ClipboardManager) terminalController.activity.getSystemService(Context.CLIPBOARD_SERVICE);
                if (clipboard != null) clipboard.setPrimaryClip(new ClipData(null, new String[]{"text/plain"}, new ClipData.Item(text)));
            }

            @Override
            public void onBell(TerminalSession session) {
                if (!terminalController.mIsVisible) {
                    return;
                }

                Bell.getInstance(terminalController.activity).doBell();
            }

            @Override
            public void onColorsChanged(TerminalSession changedSession) {
                if (terminalController.mTerminalView.getCurrentSession() == changedSession)
                    terminalController.updateBackgroundColor();
            }
        };

        mListViewAdapter = new ArrayAdapter<TerminalSession>(terminalController.activity, R.layout.line_in_drawer, mTerminalService.getSessions()) {
            final StyleSpan boldSpan = new StyleSpan(Typeface.BOLD);
            final StyleSpan italicSpan = new StyleSpan(Typeface.ITALIC);

            @NonNull
            @Override
            public View getView(int position, View convertView, @NonNull ViewGroup parent) {
                View row = convertView;
                if (row == null) {
                    row = terminalController.inflater.inflate(R.layout.line_in_drawer, parent, false);
                }

                TerminalSession sessionAtRow = getItem(position);
                boolean sessionRunning = sessionAtRow.isRunning();

                TextView firstLineView = row.findViewById(R.id.row_line);

                String name = sessionAtRow.mSessionName;
                String sessionTitle = sessionAtRow.getTitle();

                String numberPart = "[" + (position + 1) + "] ";
                String sessionNamePart = (TextUtils.isEmpty(name) ? "" : name);
                String sessionTitlePart = (TextUtils.isEmpty(sessionTitle) ? "" : ((sessionNamePart.isEmpty() ? "" : "\n") + sessionTitle));

                String text = numberPart + sessionNamePart + sessionTitlePart;
                SpannableString styledText = new SpannableString(text);
                styledText.setSpan(boldSpan, 0, numberPart.length() + sessionNamePart.length(), Spanned.SPAN_EXCLUSIVE_EXCLUSIVE);
                styledText.setSpan(italicSpan, numberPart.length() + sessionNamePart.length(), text.length(), Spanned.SPAN_EXCLUSIVE_EXCLUSIVE);

                firstLineView.setText(styledText);

                if (sessionRunning) {
                    firstLineView.setPaintFlags(firstLineView.getPaintFlags() & ~Paint.STRIKE_THRU_TEXT_FLAG);
                } else {
                    firstLineView.setPaintFlags(firstLineView.getPaintFlags() | Paint.STRIKE_THRU_TEXT_FLAG);
                }
                int color = sessionRunning || sessionAtRow.getExitStatus() == 0 ? Color.BLACK : Color.RED;
                firstLineView.setTextColor(color);
                return row;
            }
        };

        ListView listView = terminalController.activity.findViewById(R.id.left_drawer_list);
        mListViewAdapter = new ArrayAdapter<TerminalSession>(terminalController.activity.getApplicationContext(), R.layout.line_in_drawer, mTerminalService.getSessions()) {
            final StyleSpan boldSpan = new StyleSpan(Typeface.BOLD);
            final StyleSpan italicSpan = new StyleSpan(Typeface.ITALIC);

            @NonNull
            @Override
            public View getView(int position, View convertView, @NonNull ViewGroup parent) {
                View row = convertView;
                if (row == null) {
                    row = terminalController.inflater.inflate(R.layout.line_in_drawer, parent, false);
                }

                TerminalSession sessionAtRow = getItem(position);
                boolean sessionRunning = sessionAtRow.isRunning();

                TextView firstLineView = row.findViewById(R.id.row_line);

                String name = sessionAtRow.mSessionName;
                String sessionTitle = sessionAtRow.getTitle();

                String numberPart = "[" + (position + 1) + "] ";
                String sessionNamePart = (TextUtils.isEmpty(name) ? "" : name);
                String sessionTitlePart = (TextUtils.isEmpty(sessionTitle) ? "" : ((sessionNamePart.isEmpty() ? "" : "\n") + sessionTitle));

                String text = numberPart + sessionNamePart + sessionTitlePart;
                SpannableString styledText = new SpannableString(text);
                styledText.setSpan(boldSpan, 0, numberPart.length() + sessionNamePart.length(), Spanned.SPAN_EXCLUSIVE_EXCLUSIVE);
                styledText.setSpan(italicSpan, numberPart.length() + sessionNamePart.length(), text.length(), Spanned.SPAN_EXCLUSIVE_EXCLUSIVE);

                firstLineView.setText(styledText);

                if (sessionRunning) {
                    firstLineView.setPaintFlags(firstLineView.getPaintFlags() & ~Paint.STRIKE_THRU_TEXT_FLAG);
                } else {
                    firstLineView.setPaintFlags(firstLineView.getPaintFlags() | Paint.STRIKE_THRU_TEXT_FLAG);
                }
                int color = sessionRunning || sessionAtRow.getExitStatus() == 0 ? Color.BLACK : Color.RED;
                firstLineView.setTextColor(color);
                return row;
            }
        };

        listView.setAdapter(mListViewAdapter);

        listView.setOnItemClickListener((parent, view, position, id) -> {
            TerminalSession clickedSession = mListViewAdapter.getItem(position);
            TerminalSession currentSession = terminalController.mTerminalView.getCurrentSession();
            terminalController.switchToSession(currentSession, clickedSession);
            terminalController.getDrawer().closeDrawers();
        });

        if (mTerminalService.getSessions().isEmpty()) {
            if (terminalController.mIsVisible) {
                TerminalSession log = createLog(true, context);
                createLogcat(context);
                createShell(context);
                terminalController.switchToSession(log);
            } else {
                // The service connected while not in foreground - just bail out.
                terminalController.activity.finish();
            }
        } else {
            terminalController.switchToSession(terminalController.getStoredCurrentSessionOrLast());
        }
    }

    LogUtils logUtils = new LogUtils("Terminal Controller Service");

    /**
     * Initialized in {@link #onServiceConnected(ComponentName, IBinder)}.
     */
    ArrayAdapter<TerminalSession> mListViewAdapter;

    /** Flag indicating whether we have called bind on the service. */
    public boolean mIsBound;

    private ArrayList<Runnable> runnableArrayList = new ArrayList<>();

    Context context = null;

    @Override
    public void onServiceDisconnectedCallback(ComponentName name) {
        // Respect being stopped from the TerminalService notification action.
        terminalController.activity.finish();
    }

    public TerminalSession getCurrentSession() {
        return
            mTerminalService == null
                ? null : terminalController.mTerminalView.getCurrentSession();
    }

    public TerminalSession createLog(boolean printWelcomeMessage, Context context) {
        return createLog(null, printWelcomeMessage, context);
    }

    public TerminalSession createLog(TrackedActivity trackedActivity, boolean printWelcomeMessage, Context context) {
        log.errorAndThrowIfNull(context, "a context is required");
        if (mTerminalService == null) {
            logUtils.log_Error("error: mTerminalService is null");
            return null;
        }
        TerminalSession session;
        TerminalSession currentSession = terminalController.mTerminalView.getCurrentSession();

        session = mTerminalService.createShellSession(true, trackedActivity, pseudoTerminal, printWelcomeMessage, context);
        if (trackedActivity != null) log.assertTrue(session.isTrackedActivity);
        terminalController.mTerminalView.attachSession(session);

        int logPid;
        logPid = session.getPid();
        if (session.isTrackedActivity) {
            session.mSessionName = "LOG [CONNECTED: pid=" + logPid + ", " + trackedActivity.packageName + "]";
        } else {
            session.mSessionName = "LOG [LOCAL: pid=" + logPid + "]";
        }
        JNI.puts(String.format(Locale.ENGLISH, "log has started, pid is %d", logPid));

        terminalController.activity.runOnUiThread(new Runnable() {
            @Override
            public void run() {
                if (!LibService_Messenger.mainThread.equals(Thread.currentThread())) {
                    logUtils.errorAndThrow(
                        "ERROR: runOnUiThread IS NOT BEING INVOKED ON REAL UI THREAD"
                    );
                }
                terminalController.switchToSession(currentSession, session);
                mListViewAdapter.notifyDataSetChanged();
            }
        });
        return session;
    }

    private TerminalSession createLogcat(Context context) {
        return createLogcat(null, false, context);
    }

    public TerminalSession createLogcat(TrackedActivity trackedActivity, Context context) {
        return createLogcat(null, false, context);
    }

    public TerminalSession createLogcat(TrackedActivity trackedActivity, boolean useRoot, Context context) {
        log.errorAndThrowIfNull(context, "a context is required");
        if (mTerminalService == null) {
            logUtils.log_Error("error: mTerminalService is null");
            return null;
        }

        TerminalSession session;
        TerminalSession currentSession = terminalController.mTerminalView.getCurrentSession();

        session = mTerminalService.createLogcatSession(trackedActivity, pseudoTerminal, useRoot, context);
        if (trackedActivity != null) log.assertTrue(session.isTrackedActivity);
        terminalController.mTerminalView.attachSession(session);

        int logPid;
        logPid = session.getPid();
        if (session.isTrackedActivity) {
            session.mSessionName =
                "logcat -C --pid=" + session.trackedActivityPid +
                    " [CONNECTED: " + trackedActivity.packageName + "]";
        } else {
            session.mSessionName = "logcat -C --pid=" + JNI.getPid() + " [LOCAL: pid=" + logPid + "]";
        }
        JNI.puts(String.format(Locale.ENGLISH, "logcat has started, pid is %d", logPid));

        terminalController.activity.runOnUiThread(new Runnable() {
            @Override
            public void run() {
                if (!LibService_Messenger.mainThread.equals(Thread.currentThread())) {
                    logUtils.errorAndThrow(
                        "ERROR: runOnUiThread IS NOT BEING INVOKED ON REAL UI THREAD"
                    );
                }
                terminalController.switchToSession(currentSession, session);
                mListViewAdapter.notifyDataSetChanged();
            }
        });
        return session;
    }

    public TerminalSession createShell(Context context) {
        return createShell(null, context);
    }

    public TerminalSession createShell(TrackedActivity trackedActivity, Context context) {
        if (mTerminalService == null) {
            logUtils.log_Error("error: mTerminalService is null");
            return null;
        }

        TerminalSession session;
        TerminalSession currentSession = terminalController.mTerminalView.getCurrentSession();

        session = mTerminalService.createShellSession(false, trackedActivity, pseudoTerminal, false, context);
        terminalController.mTerminalView.attachSession(session);

        int shellPid;
        shellPid = session.getPid();
        session.mSessionName = "SHELL [LOCAL: pid=" + shellPid + "]";
        JNI.puts(String.format(Locale.ENGLISH, "shell has started, pid is %d", shellPid));

        terminalController.activity.runOnUiThread(new Runnable() {
            @Override
            public void run() {
                if (!LibService_Messenger.mainThread.equals(Thread.currentThread())) {
                    logUtils.errorAndThrow(
                        "ERROR: runOnUiThread IS NOT BEING INVOKED ON REAL UI THREAD"
                    );
                }
                terminalController.switchToSession(currentSession, session);
                mListViewAdapter.notifyDataSetChanged();
            }
        });
        return session;
    }

    public Boolean isCurrentSessionShell() {
        return
            mTerminalService == null
                ? null : terminalController.mTerminalView.getCurrentSession().isShell();
    }

    public Boolean isCurrentSessionLogView() {
        return
            mTerminalService == null
                ? null : terminalController.mTerminalView.getCurrentSession().isLogView();
    }

    public void runWhenConnectedToService(Runnable runnable) {
        runnableArrayList.add(runnable);
    }

    public void bindToTerminalService(Activity activity) {
        Intent serviceIntent = new Intent("alpine.term.TerminalControllerService");
        serviceIntent.setPackage("alpine.term");

        // Establish a connection with the service.  We use an explicit
        // class name because there is no reason to be able to let other
        // applications replace our component.
        if (!activity.bindService(serviceIntent, this, Context.BIND_EXTERNAL_SERVICE)) {
            throw new RuntimeException("bindService() failed");
        }
        mIsBound = true;
    }

    public void registerActivity(final Activity activity, final int[] pseudoTerminal) {
        runWhenConnectedToService(() -> {
            logUtils.log_Error("REGISTERING ACTIVITY");
            registerActivity_(activity, pseudoTerminal);
            logUtils.log_Error("REGISTERED ACTIVITY");
        });
    }

    private void registerActivity_(Activity activity, int[] pseudoTerminal) {
        TrackedActivity trackedActivity = new TrackedActivity();
        trackedActivity.storePseudoTerminal(pseudoTerminal);
        trackedActivity.packageName = activity.getPackageName();
        trackedActivity.pid = JNI.getPid();
        trackedActivity.pidAsString = Integer.toString(trackedActivity.pid);
        PackageManager pm = activity.getPackageManager();
        ComponentName componentName = activity.getComponentName();
        ActivityInfo activityInfo;
        try {
            activityInfo = pm.getActivityInfo(componentName, 0);
            trackedActivity.activityInfo = activityInfo;
            Resources resources = activity.getResources();
            int labelRes = activityInfo.labelRes;
            if (labelRes != 0)
                trackedActivity.label = resources.getString(labelRes);
            int descriptionRes = activityInfo.descriptionRes;
            if (descriptionRes != 0)
                trackedActivity.description = resources.getString(descriptionRes);
        } catch (PackageManager.NameNotFoundException e) {
            e.printStackTrace();
        }
        Bundle bundle = new Bundle();
        bundle.putParcelable("ACTIVITY", trackedActivity);
        messenger.sendMessageToServer(TerminalService.MSG_REGISTER_ACTIVITY, bundle);
    }

    public void startTerminalActivity() {
        runWhenConnectedToService(
            () -> {
                messenger.sendMessageToServer(
                    TerminalService.MSG_START_TERMINAL_ACTIVITY
                );
            }
        );
    }
}

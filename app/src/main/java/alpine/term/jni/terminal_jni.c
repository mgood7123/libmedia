/*
*************************************************************************
Alpine Term - a VM-based terminal emulator.
Copyright (C) 2019  Leonid Plyushch <leonid.plyushch@gmail.com>

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
#include <dirent.h>
#include <fcntl.h>
#include <jni.h>
#include <signal.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/ioctl.h>
#include <sys/wait.h>
#include <termios.h>
#include <unistd.h>
#include <android/log.h>
#include <errno.h>
#include <stdbool.h>

#ifndef MODULE_NAME
#define MODULE_NAME  "TERMINAL"
#endif

#define LOGV(...) __android_log_print(ANDROID_LOG_VERBOSE, MODULE_NAME, __VA_ARGS__)
#define LOGD(...) __android_log_print(ANDROID_LOG_DEBUG, MODULE_NAME, __VA_ARGS__)
#define LOGI(...) __android_log_print(ANDROID_LOG_INFO, MODULE_NAME, __VA_ARGS__)
#define LOGW(...) __android_log_print(ANDROID_LOG_WARN, MODULE_NAME, __VA_ARGS__)
#define LOGE(...) __android_log_print(ANDROID_LOG_ERROR, MODULE_NAME, __VA_ARGS__)
#define LOGF(...) __android_log_print(ANDROID_LOG_FATAL, MODULE_NAME, __VA_ARGS__)


#define ALPINE_TERM_UNUSED(x) x __attribute__((__unused__))
#ifdef __APPLE__
# define LACKS_PTSNAME_R
#endif

static int throw_runtime_exception(JNIEnv* env, char const* message)
{
    jclass exClass = (*env)->FindClass(env, "java/lang/RuntimeException");
    (*env)->ThrowNew(env, exClass, message);
    return -1;
}

jintArray createJniArray(JNIEnv *env, size_t size) {
    jintArray result = (*env)->NewIntArray(env, size);
    if (result == NULL) {
        return NULL; /* out of memory error thrown */
    } else return result;
}

void setJniArrayIndex(JNIEnv *env, jintArray * array, int index, int value) {
    // fill a temp structure to use to populate the java int array
    jint fill[1];

    // populate the values
    fill[0] = value;

    // move from the temp structure to the java structure
    (*env)->SetIntArrayRegion(env, *array, index, 1, fill);
}

bool setJniArrayIndexes(
    JNIEnv *env, jintArray * array, int index,
    int * pointer, int totalIndexesInPointer
) {
    // fill a temp structure to use to populate the java int array
    jint * fill = (jint*) malloc(totalIndexesInPointer * sizeof(jint));
    if (fill == NULL) return false;

    // populate the values
    // if valueTotalIndexes is 1, then
    for (int i = 0; i < totalIndexesInPointer; ++i) {
        fill[i] = pointer[i];
    }

    // move from the temp structure to the java structure
    (*env)->SetIntArrayRegion(env, *array, index, totalIndexesInPointer, fill);
    free(fill);
    return true;
}

static int create_subprocess(JNIEnv* env,
        char const* cmd,
        char const* cwd,
        char* const argv[],
        char** envp,
        int* pProcessId,
        jint rows,
        jint columns)
{
    int ptm = open("/dev/ptmx", O_RDWR | O_CLOEXEC);
    if (ptm < 0) return throw_runtime_exception(env, "Cannot open /dev/ptmx");

#ifdef LACKS_PTSNAME_R
    char* devname;
#else
    char devname[64];
#endif
    if (grantpt(ptm) || unlockpt(ptm) ||
#ifdef LACKS_PTSNAME_R
            (devname = ptsname(ptm)) == NULL
#else
            ptsname_r(ptm, devname, sizeof(devname))
#endif
       ) {
        return throw_runtime_exception(env, "Cannot grantpt()/unlockpt()/ptsname_r() on /dev/ptmx");
    }

    // Enable UTF-8 mode.
    struct termios tios;
    tcgetattr(ptm, &tios);
    tios.c_iflag |= IUTF8;
    tcsetattr(ptm, TCSANOW, &tios);

    /** Set initial winsize. */
    struct winsize sz = { .ws_row = rows, .ws_col = columns };
    ioctl(ptm, TIOCSWINSZ, &sz);

    pid_t pid = fork();
    if (pid < 0) {
        return throw_runtime_exception(env, "Fork failed");
    } else if (pid > 0) {
        *pProcessId = (int) pid;
        return ptm;
    } else {
        // Clear signals which the Android java process may have blocked:
        sigset_t signals_to_unblock;
        sigfillset(&signals_to_unblock);
        sigprocmask(SIG_UNBLOCK, &signals_to_unblock, 0);

        close(ptm);
        setsid();

        int pts = open(devname, O_RDWR);
        if (pts < 0) exit(-1);

        dup2(pts, 0);
        dup2(pts, 1);
        dup2(pts, 2);

        DIR* self_dir = opendir("/proc/self/fd");
        if (self_dir != NULL) {
            int self_dir_fd = dirfd(self_dir);
            struct dirent* entry;
            while ((entry = readdir(self_dir)) != NULL) {
                int fd = atoi(entry->d_name);
                if(fd > 2 && fd != self_dir_fd) close(fd);
            }
            closedir(self_dir);
        }

        clearenv();
        if (envp) for (; *envp; ++envp) putenv(*envp);

        if (chdir(cwd) != 0) {
            char* error_message;
            // No need to free asprintf()-allocated memory since doing execvp() or exit() below.
            if (asprintf(&error_message, "chdir(\"%s\")", cwd) == -1) error_message = "chdir()";
            perror(error_message);
            fflush(stderr);
        }
        execvp(cmd, argv);
        // Show terminal output about failing exec() call:
        char* error_message;
        if (asprintf(&error_message, "exec(\"%s\")", cmd) == -1) error_message = "exec()";
        perror(error_message);
        _exit(1);
    }
}

JNIEXPORT jint JNICALL Java_alpine_term_emulator_JNI_createSubprocess(
        JNIEnv* env,
        jclass ALPINE_TERM_UNUSED(clazz),
        jstring cmd,
        jstring cwd,
        jobjectArray args,
        jobjectArray envVars,
        jintArray processIdArray,
        jint rows,
        jint columns)
{
    jsize size = args ? (*env)->GetArrayLength(env, args) : 0;
    char** argv = NULL;
    if (size > 0) {
        argv = (char**) malloc((size + 1) * sizeof(char*));
        if (!argv) return throw_runtime_exception(env, "Couldn't allocate argv array");
        for (int i = 0; i < size; ++i) {
            jstring arg_java_string = (jstring) (*env)->GetObjectArrayElement(env, args, i);
            char const* arg_utf8 = (*env)->GetStringUTFChars(env, arg_java_string, NULL);
            if (!arg_utf8) return throw_runtime_exception(env, "GetStringUTFChars() failed for argv");
            argv[i] = strdup(arg_utf8);
            (*env)->ReleaseStringUTFChars(env, arg_java_string, arg_utf8);
        }
        argv[size] = NULL;
    }

    size = envVars ? (*env)->GetArrayLength(env, envVars) : 0;
    char** envp = NULL;
    if (size > 0) {
        envp = (char**) malloc((size + 1) * sizeof(char *));
        if (!envp) return throw_runtime_exception(env, "malloc() for envp array failed");
        for (int i = 0; i < size; ++i) {
            jstring env_java_string = (jstring) (*env)->GetObjectArrayElement(env, envVars, i);
            char const* env_utf8 = (*env)->GetStringUTFChars(env, env_java_string, 0);
            if (!env_utf8) return throw_runtime_exception(env, "GetStringUTFChars() failed for env");
            envp[i] = strdup(env_utf8);
            (*env)->ReleaseStringUTFChars(env, env_java_string, env_utf8);
        }
        envp[size] = NULL;
    }

    int procId = 0;
    char const* cmd_cwd = (*env)->GetStringUTFChars(env, cwd, NULL);
    char const* cmd_utf8 = (*env)->GetStringUTFChars(env, cmd, NULL);
    int ptm = create_subprocess(env, cmd_utf8, cmd_cwd, argv, envp, &procId, rows, columns);
    (*env)->ReleaseStringUTFChars(env, cmd, cmd_utf8);
    (*env)->ReleaseStringUTFChars(env, cmd, cmd_cwd);

    if (argv) {
        for (char** tmp = argv; *tmp; ++tmp) free(*tmp);
        free(argv);
    }
    if (envp) {
        for (char** tmp = envp; *tmp; ++tmp) free(*tmp);
        free(envp);
    }

    int* pProcId = (int*) (*env)->GetPrimitiveArrayCritical(env, processIdArray, NULL);
    if (!pProcId) return throw_runtime_exception(env, "JNI call GetPrimitiveArrayCritical(processIdArray, &isCopy) failed");

    *pProcId = procId;
    (*env)->ReleasePrimitiveArrayCritical(env, processIdArray, pProcId, 0);

    return ptm;
}

JNIEXPORT void JNICALL Java_alpine_term_emulator_JNI_setPtyWindowSize(JNIEnv* ALPINE_TERM_UNUSED(env), jclass ALPINE_TERM_UNUSED(clazz), jint fd, jint rows, jint cols)
{
    struct winsize sz = { .ws_row = rows, .ws_col = cols };
    ioctl(fd, TIOCSWINSZ, &sz);
}

JNIEXPORT void JNICALL Java_alpine_term_emulator_JNI_setPtyUTF8Mode(JNIEnv* ALPINE_TERM_UNUSED(env), jclass ALPINE_TERM_UNUSED(clazz), jint fd)
{
    struct termios tios;
    tcgetattr(fd, &tios);
    if ((tios.c_iflag & IUTF8) == 0) {
        tios.c_iflag |= IUTF8;
        tcsetattr(fd, TCSANOW, &tios);
    }
}

JNIEXPORT int JNICALL Java_alpine_term_emulator_JNI_waitFor(JNIEnv* ALPINE_TERM_UNUSED(env), jclass ALPINE_TERM_UNUSED(clazz), jint pid)
{
    int status;
    int r = waitpid(pid, &status, 0);
    int errno_ = errno;
    const char * errnoString = strerror(errno);
    if (errno_ != 0) {
        printf("waitpid returned %d, errno: %d, errno string: %s\n", r, errno_, errnoString);
        LOGE("waitpid returned %d, errno: %d, errno string: %s\n", r, errno_, errnoString);
    }
    if (r != -1) {
        if (WIFEXITED(status)) {
            return WEXITSTATUS(status);
        } else if (WIFSIGNALED(status)) {
            return -WTERMSIG(status);
        } else {
            // Should never happen - waitpid(2) says "One of the first three macros will evaluate to a non-zero (true) value".
            return 0;
        }
    } else return -999;
}

JNIEXPORT void JNICALL Java_alpine_term_emulator_JNI_close(JNIEnv* ALPINE_TERM_UNUSED(env), jclass ALPINE_TERM_UNUSED(clazz), jint fileDescriptor)
{
    close(fileDescriptor);
}

JNIEXPORT void JNICALL Java_alpine_term_emulator_JNI_test_1puts(JNIEnv * ALPINE_TERM_UNUSED(env), jclass ALPINE_TERM_UNUSED(clazz)) {
    printf("HELLO FROM NATIVE CPP\n");
}

JNIEXPORT void JNICALL Java_alpine_term_emulator_JNI_puts(JNIEnv * ALPINE_TERM_UNUSED(env), jclass ALPINE_TERM_UNUSED(clazz), jstring fmt) {
    char const* fmt_utf8 = (*env)->GetStringUTFChars(env, fmt, NULL);
    printf("%s\n", fmt_utf8);
    (*env)->ReleaseStringUTFChars(env, fmt, fmt_utf8);
}

JNIEXPORT jint JNICALL
Java_alpine_term_emulator_JNI_getPid(JNIEnv *ALPINE_TERM_UNUSED(env), jclass ALPINE_TERM_UNUSED(clazz)) {
    return getpid();
}

JNIEXPORT int* JNICALL
Java_alpine_term_emulator_JNI_createPseudoTerminal(
    JNIEnv *env,
    jclass ALPINE_TERM_UNUSED(clazz),
    jboolean printWelcomeMessage
) {
    LOGV("opening ptmx (master) device");
    int ptm = open("/dev/ptmx", O_RDWR | O_CLOEXEC);
    LOGV("opened ptmx (master) device: %d", ptm);
    if (ptm < 0) {
        jintArray a = createJniArray(env, 1);
        setJniArrayIndex(env, &a, 0, throw_runtime_exception(env, "Cannot open /dev/ptmx"));
        return a;
    }

#ifdef LACKS_PTSNAME_R
    char* devname;
#else
    char devname[64];
#endif
    if (grantpt(ptm) || unlockpt(ptm) ||
        #ifdef LACKS_PTSNAME_R
        (devname = ptsname(ptm)) == NULL
        #else
        ptsname_r(ptm, devname, sizeof(devname))
#endif
        ) {
        char * msg = "Cannot grantpt()/unlockpt()/ptsname_r() on /dev/ptmx";
        jintArray a = createJniArray(env, 1);
        setJniArrayIndex(env, &a, 0, throw_runtime_exception(env, msg));
        return a;
    }

    // Enable UTF-8 mode.
    struct termios tios;
    tcgetattr(ptm, &tios);
    tios.c_iflag |= IUTF8;
    tcsetattr(ptm, TCSANOW, &tios);

    /** Set initial winsize to 80x80. */
    struct winsize sz = { .ws_row = 80, .ws_col = 80 };
    ioctl(ptm, TIOCSWINSZ, &sz);

    // Clear signals which the Android java process may have blocked:
    sigset_t signals_to_unblock;
    sigfillset(&signals_to_unblock);
    sigprocmask(SIG_UNBLOCK, &signals_to_unblock, 0);

    setsid();

    LOGV("opening %s (slave) device", devname);
    int pts = open(devname, O_RDWR);
    if (pts < 0) {
        LOGE("cannot open %s (slave) device: %d (error: %s)", devname, pts, strerror(errno));
        char * msg = "failed to open slave device";
        jintArray a = createJniArray(env, 1);
        setJniArrayIndex(env, &a, 0, throw_runtime_exception(env, msg));
        return a;
    }
    LOGV("opened %s (slave) device: %d", devname, pts);
    LOGV("duping current stdin (fd 0), stdout (fd 1), and stderr (fd 2) to %s (slave) device: %d", devname, pts);
    dup2(pts, 0);
    dup2(pts, 1);
    dup2(pts, 2);

    if (printWelcomeMessage) {
        puts("Welcome to the Android Terminal Log\n");

        puts("To view a demonstration long press on the screen");
        puts("tap \"More...\"");
        puts("tap \"printf something to the terminal");
        puts("\nBy default this invokes the following C/C++ code:\n");
        puts("    printf(\"HELLO FROM NATIVE CPP\\n\");\n");
        puts("like a terminal, output is sent by printing a new line");
        puts("so if no output appears, try putting a new line in your printing function");
        puts("\nNow printing logging info:\n");
    }

    printf("opening ptmx (master) device\n");
    printf("opened ptmx (master) device: %d\n", ptm);
    printf("opening %s (slave) device\n", devname);
    printf("opened %s (slave) device: %d\n", devname, pts);
    printf("duping current stdin (fd 0), stdout (fd 1), and stderr (fd 2) to %s (slave) device: %d\n", devname, pts);
    printf("returning ptmx (master) device: %d\n", ptm);
    LOGV("returning ptmx (master) device: %d\n", ptm);
    jintArray a = createJniArray(env, 2);
    setJniArrayIndex(env, &a, 0, ptm);
    setJniArrayIndex(env, &a, 1, pts);
    return a;
}

#include "regex_str.h"
#include "env.h"
//#include "pidof.h"

JNIEXPORT jboolean JNICALL
Java_alpine_term_emulator_JNI_hasDied(
    JNIEnv * env,
    jclass ALPINE_TERM_UNUSED(clazz),
    jstring package_name
) {

//    int pid_Count = NULL;
//
//    char const *package_name_utf8 = (*env)->GetStringUTFChars(env, package_name, NULL);
//    pidof(package_name_utf8, &pid_Count);
//    (*env)->ReleaseStringUTFChars(env, package_name, package_name_utf8);
//
//    return pid_Count == 0;

    env_t argv = env__new();
    argv = env__add(argv, "/sbin/su");
    argv = env__add(argv, "-c");

    str_new_fast(command);
    str_insert_string_fast(command, "pidof ");
    char const *package_name_utf8 = (*env)->GetStringUTFChars(env, package_name, NULL);
    str_insert_string_fast(command, package_name_utf8);
    argv = env__add(argv, command.string);
    (*env)->ReleaseStringUTFChars(env, package_name, package_name_utf8);

    str_free_fast(command);

    env_t envp = env__new();
    envp = env__add(envp, getenv("PATH"));
    int status;
    pid_t child_id;
    int null = open("/dev/null", O_WRONLY);
    bool nullOpened = true;
    if (null < 0) {
        nullOpened = false;
        throw_runtime_exception(env, "Cannot open /dev/null");
        // continue
    }
    child_id = fork();
    if (child_id == 0) {
        clearenv();
        env__put(envp);

        // redirect stdout and stderr to /dev/null
        if (nullOpened) {
            dup2(null, 1);
            dup2(null, 2);
        }

        execv(argv[0], argv);
    }
    if (child_id < 0) {
        if (nullOpened) close(null);
        env__free(envp);
        env__free(argv);
        throw_runtime_exception(env, "Fork failed");
        return false;
    }
    wait(&status);
    env__free(envp);
    env__free(argv);
    if (nullOpened) close(null);
    if (WIFEXITED(status)) {
        int returned = WEXITSTATUS(status);
        if (returned == 0)
            return false;
        else
            return true;
    }
    throw_runtime_exception(env, "process did not exit correctly");
    // unreached due to throw
    return false;
}

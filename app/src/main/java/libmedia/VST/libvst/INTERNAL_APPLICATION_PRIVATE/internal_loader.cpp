//
// Created by konek on 7/14/2019.
//

#include <jni.h>
#include <dlfcn.h>
#include <android/log.h>
#include <functionPointer.h>
#include <VST.h>
#include "libMonitor/Monitor.h"
#include <Windows/windows.h>
#include <EGL/egl.h>
#include <GLES3/gl32.h>

#define LOG_TAG "VST Manager"
#define LOGV(...) __android_log_print(ANDROID_LOG_VERBOSE, LOG_TAG, __VA_ARGS__)

int VST_API_IMPLEMENTATION;
HANDLE GRAPHICSHANDLER = nullptr;
PROCESS_INFORMATION processInfo;
functionPointerDeclare0(VST_TYPEDEF_RETURN_TYPE, GraphicsCreate);
functionPointerDeclare2(VST_TYPEDEF_RETURN_TYPE, GraphicsResize, int, int);
functionPointerDeclare0(VST_TYPEDEF_RETURN_TYPE, GraphicsDraw);
functionPointerDeclare0(VST_TYPEDEF_RETURN_TYPE, GraphicsDestroy);

const char * VST = "libVSTRotatingSquaresLinux.so";

Monitor<VST_TYPEDEF_RETURN_TYPE *> VST_STATE_MONITOR;

extern "C"
JNIEXPORT void JNICALL
Java_libvst_INTERNAL_1APPLICATION_1PRIVATE_graphicsManager_load(JNIEnv *env, jclass type) {
    if (eglGetCurrentContext() != EGL_NO_CONTEXT) {
        LOGV("There is a current context.");
    } else LOGV("There is no current context.");
    LOGV("load");
    VST_STATE_MONITOR.add("VST_STATE", &VST_STATE);
    if (GRAPHICSHANDLER == nullptr) {
        LOGV("attempting to open %s", VST);
        GRAPHICSHANDLER = dlopen(VST, RTLD_NOW);
        if (GRAPHICSHANDLER == nullptr) {
            LOGV("%s could not be opened: %s", VST, dlerror());
            VST_STATE = VST_RETURN_STOP;
        } else {
            LOGV("%s opened successfully", VST);
            void * vsttype = dlsym(GRAPHICSHANDLER, "VST_API_IMPLEMENTATION");
            functionPointerAssign0(VST_TYPEDEF_RETURN_TYPE, GraphicsCreate,
                                   dlsym(GRAPHICSHANDLER, "GraphicsCreate"));
            char * GC = dlerror();
            functionPointerAssign2(VST_TYPEDEF_RETURN_TYPE, GraphicsResize,
                                   dlsym(GRAPHICSHANDLER, "GraphicsResize"), int, int);
            char * GR = dlerror();
            functionPointerAssign0(VST_TYPEDEF_RETURN_TYPE, GraphicsDraw,
                                   dlsym(GRAPHICSHANDLER, "GraphicsDraw"));
            char * GDR = dlerror();
            functionPointerAssign0(VST_TYPEDEF_RETURN_TYPE, GraphicsDestroy,
                                   dlsym(GRAPHICSHANDLER, "GraphicsDestroy"));
            char * GDE = dlerror();
            if (vsttype == nullptr) {
                if (GraphicsCreate == nullptr &&
                    GraphicsResize == nullptr &&
                    GraphicsDraw == nullptr &&
                    GraphicsDestroy == nullptr) {
                    LOGV("%s is not a VST", VST);
                } else {
                    std::string MSG = "no api implementation specified for VST '";
                    MSG += VST;
                    MSG += "'\nplease define an api implementation using the following:\n";
                    MSG += "int VST_API_IMPLEMENTATION = VST_API_IMPLEMENTATION_LINUX;\n";
                    MSG += "or\n";
                    MSG += "int VST_API_IMPLEMENTATION = VST_API_IMPLEMENTATION_WINDOWS;";
                    LOGV("%s", MSG.c_str());
                }
                VST_STATE = VST_RETURN_STOP;
            } else {
                VST_API_IMPLEMENTATION = *reinterpret_cast<int *>(vsttype);
                if (VST_API_IMPLEMENTATION == VST_API_IMPLEMENTATION_LINUX) {
                    LOGV("%s requested API implementation: Linux", VST);
                    if (GraphicsCreate == nullptr) {
                        LOGV("function %s could not be located in %s: %s", "GraphicsCreate", VST, GC);
                        VST_STATE = VST_RETURN_STOP;
                    }
                    if (GraphicsResize == nullptr) {
                        LOGV("function %s could not be located in %s: %s", "GraphicsResize", VST, GR);
                        VST_STATE = VST_RETURN_STOP;
                    }
                    if (GraphicsDraw == nullptr) {
                        LOGV("function %s could not be located in %s: %s", "GraphicsDraw", VST, GDR);
                        VST_STATE = VST_RETURN_STOP;
                    }
                    if (GraphicsDestroy == nullptr) {
                        LOGV("function %s could not be located in %s: %s", "GraphicsDestroy", VST, GDE);
                        VST_STATE = VST_RETURN_STOP;
                    }
                    if (GraphicsCreate == nullptr ||
                        GraphicsResize == nullptr ||
                        GraphicsDraw == nullptr ||
                        GraphicsDestroy == nullptr) {
                        LOGV("%s is corrupted", VST);
                        VST_STATE = VST_RETURN_STOP;
                        return;
                    }
                } else if (VST_API_IMPLEMENTATION == VST_API_IMPLEMENTATION_WINDOWS) {
                    LOGV("%s requested API implementation: Windows", VST);
                    dlclose(GRAPHICSHANDLER);
                    char * DLC = dlerror();
                    if (DLC != nullptr) LOGV("%s failed to close: %s", VST, DLC);
                    if (!CreateProcessA(VST, nullptr, nullptr, nullptr, false, 0, nullptr, nullptr, nullptr, &processInfo)) {
                        LOGV("%s failed to created windows process", VST);
                    } else {
                        LOGV("%s successfully created windows process", VST);
                    }
                }
                VST_STATE = VST_RETURN_CONTINUE;
            }
        }
    }
}

extern "C"
JNIEXPORT void JNICALL
Java_libvst_INTERNAL_1APPLICATION_1PRIVATE_graphicsManager_create(JNIEnv *env, jclass type) {
    if (VST_API_IMPLEMENTATION == VST_API_IMPLEMENTATION_LINUX) {
        LOGV("%s: create", VST);
        if (GRAPHICSHANDLER != nullptr) {
            if (VST_STATE != VST_RETURN_STOP) VST_STATE = GraphicsCreate();
        }
        if (VST_STATE == VST_RETURN_STOP) LOGV("%s: stopped", VST);
    } else if (VST_API_IMPLEMENTATION == VST_API_IMPLEMENTATION_WINDOWS) {
        PostMessageA(processInfo.hProcess, WM_CREATE, MAKEWPARAM(0,0), MAKELPARAM(0,0));
    }
}

extern "C"
JNIEXPORT void JNICALL
Java_libvst_INTERNAL_1APPLICATION_1PRIVATE_graphicsManager_resize(JNIEnv *env, jclass type, jint width, jint height) {
    if (VST_API_IMPLEMENTATION == VST_API_IMPLEMENTATION_LINUX) {
        LOGV("%s: resize", VST);
        if (GRAPHICSHANDLER != nullptr) {
            if (VST_STATE == VST_RETURN_RESTART) {
                VST_STATE = GraphicsDestroy();
                if (VST_STATE == VST_RETURN_CONTINUE) VST_STATE = GraphicsCreate();
            }
            if (VST_STATE == VST_RETURN_CONTINUE) VST_STATE = GraphicsResize(width, height);
            if (VST_STATE == VST_RETURN_STOP) LOGV("%s: stopped", VST);
        }
    } else if (VST_API_IMPLEMENTATION == VST_API_IMPLEMENTATION_WINDOWS) {
        PostMessageA(processInfo.hProcess, WM_SIZE, MAKEWPARAM(SIZE_MAXIMIZED, 0), MAKELPARAM(width, height));
    }
}

extern "C"
JNIEXPORT void JNICALL
Java_libvst_INTERNAL_1APPLICATION_1PRIVATE_graphicsManager_draw(JNIEnv *env, jclass type) {
    if (VST_API_IMPLEMENTATION == VST_API_IMPLEMENTATION_LINUX) {
//    LOGV("%s: draw", VST);
        if (GRAPHICSHANDLER != nullptr) {
            if (VST_STATE == VST_RETURN_RESTART) {
                LOGV("%s: restarting", VST);
                VST_STATE = GraphicsDestroy();
                if (VST_STATE == VST_RETURN_CONTINUE) VST_STATE = GraphicsCreate();
            }
            if (VST_STATE == VST_RETURN_CONTINUE) {
                VST_STATE = GraphicsDraw();
            }
            if (VST_STATE == VST_RETURN_STOP) LOGV("%s: stopped", VST);
        }
    }
}

extern "C"
JNIEXPORT void JNICALL
Java_libvst_INTERNAL_1APPLICATION_1PRIVATE_graphicsManager_destroy(JNIEnv *env, jclass type) {
    if (VST_API_IMPLEMENTATION == VST_API_IMPLEMENTATION_LINUX) {
        LOGV("%s: destroy", VST);
        if (GRAPHICSHANDLER != nullptr) {
            if (VST_STATE == VST_RETURN_RESTART) {
                VST_STATE = GraphicsDestroy();
                if (VST_STATE == VST_RETURN_CONTINUE) VST_STATE = GraphicsCreate();
            } else if (VST_STATE == VST_RETURN_CONTINUE) {
                VST_STATE = GraphicsDestroy();
            }
            if (VST_STATE == VST_RETURN_STOP) LOGV("%s: stopped", VST);
        }
    }
}

//
// Created by konek on 7/14/2019.
//

#include <jni.h>
#include <dlfcn.h>
#include <android/log.h>
#include "functionPointer.h"
#include "../API/VST.h"
#include "../../libMonitor/Monitor.h"
#include "windowsAPI.h"

#define LOG_TAG "VST Manager"
#define ALOGV(...) __android_log_print(ANDROID_LOG_VERBOSE, LOG_TAG, __VA_ARGS__)
int VST_API_IMPLEMENTATION;
void * GRAPHICSHANDLER = nullptr;

functionPointerDeclare0(VST_TYPEDEF_RETURN_TYPE, GraphicsCreate);
functionPointerDeclare2(VST_TYPEDEF_RETURN_TYPE, GraphicsResize, int, int);
functionPointerDeclare0(VST_TYPEDEF_RETURN_TYPE, GraphicsDraw);
functionPointerDeclare0(VST_TYPEDEF_RETURN_TYPE, GraphicsDestroy);

const char * VST = "libVSTRotatingSquares.so";

Monitor<VST_TYPEDEF_RETURN_TYPE *> VST_STATE_MONITOR;

extern "C"
JNIEXPORT void JNICALL
Java_libvst_graphicsManager_load(JNIEnv *env, jclass type) {
    ALOGV("load");
    VST_STATE_MONITOR.add("VST_STATE", &VST_STATE);
    if (GRAPHICSHANDLER == nullptr) {
        ALOGV("attempting to open %s", VST);
        GRAPHICSHANDLER = dlopen(VST, RTLD_NOW);
        if (GRAPHICSHANDLER == nullptr) {
            ALOGV("%s could not be opened", VST);
            VST_STATE = VST_RETURN_STOP;
        } else {
            ALOGV("%s opened successfully", VST);
            void * vsttype = dlsym(GRAPHICSHANDLER, "VST_API_IMPLEMENTATION");
            functionPointerAssign0(VST_TYPEDEF_RETURN_TYPE, GraphicsCreate,
                                   dlsym(GRAPHICSHANDLER, "GraphicsCreate"));
            functionPointerAssign2(VST_TYPEDEF_RETURN_TYPE, GraphicsResize,
                                   dlsym(GRAPHICSHANDLER, "GraphicsResize"), int, int);
            functionPointerAssign0(VST_TYPEDEF_RETURN_TYPE, GraphicsDraw,
                                   dlsym(GRAPHICSHANDLER, "GraphicsDraw"));
            functionPointerAssign0(VST_TYPEDEF_RETURN_TYPE, GraphicsDestroy,
                                   dlsym(GRAPHICSHANDLER, "GraphicsDestroy"));
            if (vsttype == nullptr) {
                if (GraphicsCreate == nullptr &&
                    GraphicsResize == nullptr &&
                    GraphicsDraw == nullptr &&
                    GraphicsDestroy == nullptr) {
                    ALOGV("%s is not a VST", VST);
                } else {
                    std::string MSG = "no api implementation specified for VST '";
                    MSG += VST;
                    MSG += "'\nplease define an api implementation using the following:\n";
                    MSG += "int VST_API_IMPLEMENTATION = VST_API_IMPLEMENTATION_LINUX;\n";
                    MSG += "or\n";
                    MSG += "int VST_API_IMPLEMENTATION = VST_API_IMPLEMENTATION_WINDOWS;";
                    ALOGV("%s", MSG.c_str());
                }
                VST_STATE = VST_RETURN_STOP;
            } else {
                VST_API_IMPLEMENTATION = *reinterpret_cast<int *>(vsttype);
                if (VST_API_IMPLEMENTATION == VST_API_IMPLEMENTATION_LINUX) {
                    ALOGV("%s requested API implementation: Linux", VST);
                    if (GraphicsCreate == nullptr) {
                        ALOGV("function %s could not be located in %s", "GraphicsCreate", VST);
                        VST_STATE = VST_RETURN_STOP;
                    }
                    if (GraphicsResize == nullptr) {
                        ALOGV("function %s could not be located in %s", "GraphicsResize", VST);
                        VST_STATE = VST_RETURN_STOP;
                    }
                    if (GraphicsDraw == nullptr) {
                        ALOGV("function %s could not be located in %s", "GraphicsDraw", VST);
                        VST_STATE = VST_RETURN_STOP;
                    }
                    if (GraphicsDestroy == nullptr) {
                        ALOGV("function %s could not be located in %s", "GraphicsDestroy", VST);
                        VST_STATE = VST_RETURN_STOP;
                    }
                    if (GraphicsCreate == nullptr ||
                        GraphicsResize == nullptr ||
                        GraphicsDraw == nullptr ||
                        GraphicsDestroy == nullptr) {
                        ALOGV("%s is corrupted", VST);
                        VST_STATE = VST_RETURN_STOP;
                        return;
                    }
                } else if (VST_API_IMPLEMENTATION == VST_API_IMPLEMENTATION_WINDOWS) {
                    ALOGV("%s requested API implementation: Windows", VST);
                    dlclose(GRAPHICSHANDLER);
                    PROCESS_INFORMATION a;
                    if (!CreateProcessA(VST, nullptr, nullptr, nullptr, false, 0, nullptr, nullptr, nullptr, &a)) {
                        ALOGV("%s failed to created windows process", VST);
                    } else ALOGV("%s successfully created windows process", VST);
                }
                VST_STATE = VST_RETURN_CONTINUE;
            }
        }
    }
}

extern "C"
JNIEXPORT void JNICALL
Java_libvst_graphicsManager_create(JNIEnv *env, jclass type) {
    if (VST_API_IMPLEMENTATION == VST_API_IMPLEMENTATION_LINUX) {
        ALOGV("%s: create", VST);
        if (GRAPHICSHANDLER != nullptr) {
            if (VST_STATE != VST_RETURN_STOP) VST_STATE = GraphicsCreate();
        }
        if (VST_STATE == VST_RETURN_STOP) ALOGV("%s: stopped", VST);
    }
}

extern "C"
JNIEXPORT void JNICALL
Java_libvst_graphicsManager_resize(JNIEnv *env, jclass type, jint width, jint height) {
    if (VST_API_IMPLEMENTATION == VST_API_IMPLEMENTATION_LINUX) {
        ALOGV("%s: resize", VST);
        if (GRAPHICSHANDLER != nullptr) {
            if (VST_STATE == VST_RETURN_RESTART) {
                VST_STATE = GraphicsDestroy();
                if (VST_STATE == VST_RETURN_CONTINUE) VST_STATE = GraphicsCreate();
            }
            if (VST_STATE == VST_RETURN_CONTINUE) VST_STATE = GraphicsResize(width, height);
            if (VST_STATE == VST_RETURN_STOP) ALOGV("%s: stopped", VST);
        }
    }
}

extern "C"
JNIEXPORT void JNICALL
Java_libvst_graphicsManager_draw(JNIEnv *env, jclass type) {
    if (VST_API_IMPLEMENTATION == VST_API_IMPLEMENTATION_LINUX) {
//    ALOGV("%s: draw", VST);
        if (GRAPHICSHANDLER != nullptr) {
            if (VST_STATE == VST_RETURN_RESTART) {
                ALOGV("%s: restarting", VST);
                VST_STATE = GraphicsDestroy();
                if (VST_STATE == VST_RETURN_CONTINUE) VST_STATE = GraphicsCreate();
            }
            if (VST_STATE == VST_RETURN_CONTINUE) {
                VST_STATE = GraphicsDraw();
            }
            if (VST_STATE == VST_RETURN_STOP) ALOGV("%s: stopped", VST);
        }
    }
}

extern "C"
JNIEXPORT void JNICALL
Java_libvst_graphicsManager_destroy(JNIEnv *env, jclass type) {
    if (VST_API_IMPLEMENTATION == VST_API_IMPLEMENTATION_LINUX) {
        ALOGV("%s: destroy", VST);
        if (GRAPHICSHANDLER != nullptr) {
            if (VST_STATE == VST_RETURN_RESTART) {
                VST_STATE = GraphicsDestroy();
                if (VST_STATE == VST_RETURN_CONTINUE) VST_STATE = GraphicsCreate();
            } else if (VST_STATE == VST_RETURN_CONTINUE) {
                VST_STATE = GraphicsDestroy();
            }
            if (VST_STATE == VST_RETURN_STOP) ALOGV("%s: stopped", VST);
        }
    }
}

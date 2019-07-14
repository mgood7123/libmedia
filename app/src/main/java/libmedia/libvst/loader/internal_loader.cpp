//
// Created by konek on 7/14/2019.
//

#include <jni.h>
#include <dlfcn.h>
#include <android/log.h>
#include "functionPointer.h"
#include "../API/VST.h"
#include "../../libMonitor/Monitor.h"

#define LOG_TAG "VST Manager"
#define ALOGV(...) __android_log_print(ANDROID_LOG_VERBOSE, LOG_TAG, __VA_ARGS__)

void * GRAPHICSHANDLER = nullptr;

functionPointerDeclare0(VST_TYPEDEF_RETURN_TYPE, GraphicsCreate);
functionPointerDeclare2(VST_TYPEDEF_RETURN_TYPE, GraphicsResize, int, int);
functionPointerDeclare0(VST_TYPEDEF_RETURN_TYPE, GraphicsStep);
functionPointerDeclare0(VST_TYPEDEF_RETURN_TYPE, GraphicsDestroy);

const char * VST = "libVSTWaveform.so";

Monitor<VST_TYPEDEF_RETURN_TYPE *> VST_STATE_MONITOR;

extern "C"
JNIEXPORT void JNICALL
Java_libvst_graphicsManager_load(JNIEnv *env, jclass type) {
    ALOGV("load");
    VST_STATE_MONITOR.add("VST_STATE", &VST_STATE);
    if (GRAPHICSHANDLER == nullptr) {
        GRAPHICSHANDLER = dlopen(VST, RTLD_NOW);
        if (GRAPHICSHANDLER == nullptr) {
            ALOGV("%s not found", VST);
            VST_STATE = VST_RETURN_STOP;
        } else {
            ALOGV("%s found and loaded successfully", VST);
            functionPointerAssign0(VST_TYPEDEF_RETURN_TYPE, GraphicsCreate, dlsym(GRAPHICSHANDLER, "GraphicsCreate"));
            functionPointerAssign2(VST_TYPEDEF_RETURN_TYPE, GraphicsResize, dlsym(GRAPHICSHANDLER, "GraphicsResize"), int, int);
            functionPointerAssign0(VST_TYPEDEF_RETURN_TYPE, GraphicsStep, dlsym(GRAPHICSHANDLER, "GraphicsStep"));
            functionPointerAssign0(VST_TYPEDEF_RETURN_TYPE, GraphicsDestroy, dlsym(GRAPHICSHANDLER, "GraphicsDestroy"));
            VST_STATE = VST_RETURN_CONTINUE;
        }
    }
}

extern "C"
JNIEXPORT void JNICALL
Java_libvst_graphicsManager_create(JNIEnv *env, jclass type) {
    ALOGV("create");
    if (GRAPHICSHANDLER != nullptr) {
        if (VST_STATE != VST_RETURN_STOP) VST_STATE = GraphicsCreate();
    }
    if (VST_STATE == VST_RETURN_STOP) ALOGV("stopped");
}

extern "C"
JNIEXPORT void JNICALL
Java_libvst_graphicsManager_resize(JNIEnv *env, jclass type, jint width, jint height) {
    ALOGV("resize");
    if (GRAPHICSHANDLER != nullptr) {
        if (VST_STATE == VST_RETURN_RESTART) {
            VST_STATE = GraphicsDestroy();
            if (VST_STATE == VST_RETURN_CONTINUE) VST_STATE = GraphicsCreate();
        }
        if (VST_STATE == VST_RETURN_CONTINUE) VST_STATE = GraphicsResize(width, height);
        if (VST_STATE == VST_RETURN_STOP) ALOGV("stopped");
    }
}

extern "C"
JNIEXPORT void JNICALL
Java_libvst_graphicsManager_step(JNIEnv *env, jclass type) {
//    ALOGV("step");
    if (GRAPHICSHANDLER != nullptr) {
        if (VST_STATE == VST_RETURN_RESTART) {
            ALOGV("restarting");
            VST_STATE = GraphicsDestroy();
            if (VST_STATE == VST_RETURN_CONTINUE) VST_STATE = GraphicsCreate();
        }
        if (VST_STATE == VST_RETURN_CONTINUE) {
            VST_STATE = GraphicsStep();
        }
        if (VST_STATE == VST_RETURN_STOP) ALOGV("stopped");
    }
}

extern "C"
JNIEXPORT void JNICALL
Java_libvst_graphicsManager_destroy(JNIEnv *env, jclass type) {
    ALOGV("destroy");
    if (GRAPHICSHANDLER != nullptr) {
        if (VST_STATE == VST_RETURN_RESTART) {
            VST_STATE = GraphicsDestroy();
            if (VST_STATE == VST_RETURN_CONTINUE) VST_STATE = GraphicsCreate();
        }
        else if (VST_STATE == VST_RETURN_CONTINUE) {
            VST_STATE = GraphicsDestroy();
        }
        if (VST_STATE == VST_RETURN_STOP) ALOGV("stopped");
    }
}

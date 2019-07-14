//
// Created by konek on 7/14/2019.
//

#include <jni.h>
#include <dlfcn.h>
#include <android/log.h>
#include "functionPointer.h"
#define LOG_TAG "VST Manager"
#define ALOGV(...) __android_log_print(ANDROID_LOG_VERBOSE, LOG_TAG, __VA_ARGS__)

void * GRAPHICSHANDLER = nullptr;
functionPointerDeclare0(void, GraphicsInit);
functionPointerDeclare2(void, GraphicsResize, int, int);
functionPointerDeclare0(void, GraphicsStep);

extern "C"
JNIEXPORT void JNICALL
Java_libvst_graphicsManager_load(JNIEnv *env, jclass type) {
    ALOGV("load");
    if (GRAPHICSHANDLER == nullptr) {
        GRAPHICSHANDLER = dlopen("libRotatingSquares.so", RTLD_NOW);
        if (GRAPHICSHANDLER == nullptr) {
            ALOGV("libRotatingSquares.so not found");
        } else {
            ALOGV("libRotatingSquares.so found and loaded succesfully");
            functionPointerAssign0(void, GraphicsInit, dlsym(GRAPHICSHANDLER, "GraphicsInit"));
            functionPointerAssign2(void, GraphicsResize, dlsym(GRAPHICSHANDLER, "GraphicsResize"), int, int);
            functionPointerAssign0(void, GraphicsStep, dlsym(GRAPHICSHANDLER, "GraphicsStep"));
        }
    }
}

extern "C"
JNIEXPORT void JNICALL
Java_libvst_graphicsManager_init(JNIEnv *env, jclass type) {
    // TODO
    ALOGV("init");
    if (GRAPHICSHANDLER != nullptr) {
        GraphicsInit();
    }
}

extern "C"
JNIEXPORT void JNICALL
Java_libvst_graphicsManager_resize(JNIEnv *env, jclass type, jint width, jint height) {
    ALOGV("resize");
    if (GRAPHICSHANDLER != nullptr) {
        GraphicsResize(width, height);
    }
}

extern "C"
JNIEXPORT void JNICALL
Java_libvst_graphicsManager_step(JNIEnv *env, jclass type) {
//    ALOGV("step");
    if (GRAPHICSHANDLER != nullptr) {
        GraphicsStep();
    }
}
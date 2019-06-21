//
// Created by macropreprocessor on 16/06/19.
//

#ifndef MEDIA_PLAYER_PRO_NATIVE_H
#define MEDIA_PLAYER_PRO_NATIVE_H

#include <jni.h>

#define NATIVE(returnType, prefix, name) extern "C" JNIEXPORT returnType JNICALL Java_libmedia_Media_##prefix##_1##name
#endif //MEDIA_PLAYER_PRO_NATIVE_H

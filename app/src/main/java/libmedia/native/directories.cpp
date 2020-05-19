//
// Created by Mac on 17/5/20.
//

#include <sys/stat.h>
#include <unistd.h>
#include "native.h"
#include "JniHelpers.h"

#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h>

char TMPDIR[4096*2];
char ASSETSDIR[4096*2];

NATIVE(void, Oboe, SetAssetsDir)(JNIEnv *env, jobject type, jstring dir) {
    JniHelpers::Strings::copyJniStringUTF(env, dir, ASSETSDIR);
    struct stat st = {0};
    if (stat(ASSETSDIR, &st) == -1) mkdir(ASSETSDIR, 0700);
}

NATIVE(void, Oboe, SetTempDir)(JNIEnv *env, jobject type, jstring dir) {
    JniHelpers::Strings::copyJniStringUTF(env, dir, TMPDIR);
    struct stat st = {0};
    if (stat(TMPDIR, &st) == -1) mkdir(TMPDIR, 0700);
}
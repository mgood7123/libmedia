//
// Created by Mac on 17/5/20.
//

#pragma clang diagnostic push
#pragma ide diagnostic ignored "OCUnusedGlobalDeclarationInspection"
#ifndef MEDIA_PLAYER_PRO_JNIHELPERS_H
#define MEDIA_PLAYER_PRO_JNIHELPERS_H

#include <jni.h>
#include <deque>

class JniHelpers {
public:
    class Strings {
    public:
        static size_t jniStrlenUTF(JNIEnv *env, jstring from);

        static void deleteJniStringUTF(const char **string);

        static const char *newJniStringUTF(JNIEnv *env, jstring from, size_t *len);

        static const char *newJniStringUTF(JNIEnv *env, jstring from);

        static void copyJniStringUTF(JNIEnv *env, jstring from, char *to);
    };

    class Exceptions {
    public:
        static int throwException(JNIEnv *env, const char *exception, const char *message);

        static int throwRuntimeException(JNIEnv *env, const char *message);
    };

    class Arrays {
    public:
        static jintArray createJniIntArray(JNIEnv *env, size_t size);

        static void setJniIntArrayIndex(JNIEnv *env, jintArray *array, int index, int value);

        static bool setJniIntArrayIndexes(JNIEnv *env, jintArray *array, int index, int *pointer,
                                int totalIndexesInPointer);
    };

    class Jvm {
    public:
        static JavaVM *getJvm(JNIEnv *env);

        static void destroyJvm(JavaVM *JVM);
    };

    class Types {
    public:
        const char * Boolean = "Z";
        const char * Byte = "B";
        const char * Char = "C";
        const char * Short = "S";
        const char * Int = "I";
        const char * Long = "J";
        const char * Float = "F";
        const char * Double = "D";
    };

    class Classes {
    public:

        class Internal {
        public:
            class JavaClassMember {
            public:
                JavaClassMember(const char *type, const char *parameter,
                                void *value);

                const char *type;
                const char *parameter;
                void *value;
            };

            class JavaClassConverter {
            public:

                Types types = Types();

                JavaClassConverter();

                void addBooleanMember(const char *parameter, void *value);

                void read(JNIEnv *env, const char * className, jobject classInstance);
                void clean();

                std::deque<JniHelpers::Classes::Internal::JavaClassMember> * javaClassMembers = nullptr;

            private:
                const char *targetClass = nullptr;
            };
        };

        typedef JniHelpers::Classes::Internal::JavaClassConverter JavaClassConverter;

        static JavaClassConverter *newJavaClassConverter();
    };

    class Methods {
    public:

    };
};

#endif //MEDIA_PLAYER_PRO_JNIHELPERS_H

#pragma clang diagnostic pop
//
// Created by Mac on 17/5/20.
//

#include <cstring>
#include <cstdlib>
#include <deque>
#include "JniHelpers.h"

#pragma clang diagnostic push
#pragma ide diagnostic ignored "OCUnusedGlobalDeclarationInspection"
/**
 * @return the length of a java string
 *
 * the code below is used internally in newJniStringUTF(JNIEnv *env, jstring from, size_t * len)
 *
 * @code size_t len = jniStrlenUTF(env, from);
 */
size_t JniHelpers::Strings::jniStrlenUTF(JNIEnv *env, jstring from) {
    // TODO: error check
    return static_cast<size_t>(env->GetStringUTFLength(from));
}

/**
 * @return an allocated string, this must be freed by calling deleteJniStringUTF(&ReturnedString)
 *
 * @code // allocate a new jniString
 * char * jniString = newJniString(env, javaString);
 * // jniString is now allocated
 * // do something with the jniString
 * // de-allocate the jnistring
 * deleteJniString(&jniString);
 * // jniString is now de-allocated and set to nullptr
 * @endcode
 *
 * this version saves the string length inside the len parameter
 * @code size_t jniStringLength;
 * char * jniString = newJniString(env, javaString, &jniStringLength);
 * assert(jniString[jniStringLength] == '\0');
 * @endcode
 */
const char * JniHelpers::Strings::newJniStringUTF(JNIEnv *env, jstring from, size_t * len) {
    // TODO: error check
    *len = jniStrlenUTF(env, from);
    char * ret = new char[*len];
    jboolean val;
    const char * tmp = env->GetStringUTFChars(from, &val);
    memcpy(ret, tmp, *len);
    ret[*len] = '\0';
    env->ReleaseStringUTFChars(from, tmp);
    return ret;
}

/**
 * this is just
 * @code size_t unused;
 * return newJniStringUTF(env, from, &unused);
 * @endcode
 *
 * @return an allocated string, this must be freed by calling deleteJniStringUTF(&ReturnedString)
 *
 * @code // allocate a new jniString
 * char * jniString = newJniString(env, javaString);
 * // jniString is now allocated
 * // do something with the jniString
 * // de-allocate the jnistring
 * deleteJniString(&jniString);
 * // jniString is now de-allocated and set to nullptr
 * @endcode
 */
const char *JniHelpers::Strings::newJniStringUTF(JNIEnv *env, jstring from) {
    size_t unused;
    return newJniStringUTF(env, from, &unused);
}


/**
 * de-allocates a jniString returned by newJniStringUTF
 * @param string the jniString to de-allocate
 *
 * @code // allocate a new jniString
 * char * jniString = newJniString(env, javaString);
 * // jniString is now allocated
 * // do something with the jniString
 * // de-allocate the jnistring
 * deleteJniString(&jniString);
 * // jniString is now de-allocated and set to nullptr
 * @endcode
 */
void JniHelpers::Strings::deleteJniStringUTF(const char ** string) {
    const char * stringToDelete = *string;
    delete[] stringToDelete;
    *string = nullptr;
}

void JniHelpers::Strings::copyJniStringUTF(JNIEnv *env, jstring from, char * to) {
    size_t len;
    const char * str = newJniStringUTF(env, from, &len);
    memcpy(to, str, len);
    deleteJniStringUTF(&str);
}

int JniHelpers::Exceptions::throwException(JNIEnv* env, const char * exception, char const* message) {
    // TODO: error check
    jclass exClass = env->FindClass(exception);
    env->ThrowNew(exClass, message);
    return -1;
}

int JniHelpers::Exceptions::throwRuntimeException(JNIEnv* env, char const* message) {
    return throwException(env, "java/lang/RuntimeException", message);
}

jintArray JniHelpers::Arrays::createJniIntArray(JNIEnv *env, size_t size) {
    // TODO: error check
    jintArray result = env->NewIntArray(size);
    if (result == NULL) {
        return NULL; /* out of memory error thrown */
    } else return result;
}

void JniHelpers::Arrays::setJniIntArrayIndex(JNIEnv *env, jintArray * array, int index, int value) {
    if (array == NULL) return;
    // fill a temp structure to use to populate the java int array
    jint fill[1];

    // populate the values
    fill[0] = value;

    // move from the temp structure to the java structure
    env->SetIntArrayRegion(*array, index, 1, fill);
}

bool JniHelpers::Arrays::setJniIntArrayIndexes(
        JNIEnv *env, jintArray * array, int index,
        int * pointer, int totalIndexesInPointer
) {
    if (array == NULL) return false;
    // fill a temp structure to use to populate the java int array
    jint * fill = (jint*) malloc(totalIndexesInPointer * sizeof(jint));
    if (fill == NULL) return false;

    // populate the values
    // if valueTotalIndexes is 1, then
    for (int i = 0; i < totalIndexesInPointer; ++i) {
        fill[i] = pointer[i];
    }

    // move from the temp structure to the java structure
    env->SetIntArrayRegion(*array, index, totalIndexesInPointer, fill);
    free(fill);
    return true;
}

JavaVM * JniHelpers::Jvm::getJvm(JNIEnv *env) {
    // TODO: error check
    JavaVM * JVM;
    jint ret = env->GetJavaVM(&JVM);
    return JVM;
}

void JniHelpers::Jvm::destroyJvm(JavaVM * JVM) {
    // TODO: error check
    jint ret = JVM->DestroyJavaVM();
}

JniHelpers::Classes::Internal::JavaClassMember::JavaClassMember(
        const char *type, const char *parameter, void *value
) {
    this->type = type;
    this->parameter = parameter;
    this->value = value;
}

JniHelpers::Classes::Internal::JavaClassConverter::JavaClassConverter() {}

JniHelpers::Classes::JavaClassConverter * JniHelpers::Classes::newJavaClassConverter() {
    JniHelpers::Classes::JavaClassConverter * JCC = new JavaClassConverter();
    JCC->javaClassMembers = new std::deque<JniHelpers::Classes::Internal::JavaClassMember>;
    return JCC;
}

void JniHelpers::Classes::Internal::JavaClassConverter::addBooleanMember(
        const char *parameter, void *value
) {
    javaClassMembers->push_front(JavaClassMember(types.Boolean, parameter, value));
}

void JniHelpers::Classes::Internal::JavaClassConverter::read(
        JNIEnv *env, const char * className, jobject classInstance
) {
    jclass c = env->FindClass(className);
    size_t size = javaClassMembers->size();
    for (int i = 0; i < size; i++) {
        JavaClassMember member = javaClassMembers[0][i];
        jfieldID id = env->GetFieldID(c, member.parameter, member.type);
        if (member.type[0] == types.Boolean[0])
            *static_cast<jboolean *>(member.value) = env->GetBooleanField(classInstance, id);
        else if (member.type[0] == types.Byte[0])
            *static_cast<jbyte *>(member.value) = env->GetByteField(classInstance, id);
        else if (member.type[0] == types.Char[0])
            *static_cast<jchar *>(member.value) = env->GetCharField(classInstance, id);
        else if (member.type[0] == types.Short[0])
            *static_cast<jshort *>(member.value) = env->GetShortField(classInstance, id);
        else if (member.type[0] == types.Int[0])
            *static_cast<jint *>(member.value) = env->GetIntField(classInstance, id);
        else if (member.type[0] == types.Long[0])
            *static_cast<jlong *>(member.value) = env->GetLongField(classInstance, id);
        else if (member.type[0] == types.Float[0])
            *static_cast<jfloat *>(member.value) = env->GetFloatField(classInstance, id);
        else if (member.type[0] == types.Double[0])
            *static_cast<jdouble *>(member.value) = env->GetDoubleField(classInstance, id);
        else JniHelpers::Exceptions::throwRuntimeException(env, "type not found");
    }
}

void JniHelpers::Classes::Internal::JavaClassConverter::clean() {
    while (!javaClassMembers->empty()) javaClassMembers->pop_front();
}

#pragma clang diagnostic pop
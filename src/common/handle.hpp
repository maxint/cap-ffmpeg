#ifndef __JNI_HANDLE_HPP__
#define __JNI_HANDLE_HPP__

#include <stdlib.h>

inline static jfieldID getHandleField(JNIEnv *env, jobject obj)
{
    jclass c = env->GetObjectClass(obj);
    // J is the type signature for long:
    return env->GetFieldID(c, "mNativeHandle", "J");
}

template <typename T>
T *getHandle(JNIEnv *env, jobject obj)
{
    jlong handle = env->GetLongField(obj, getHandleField(env, obj));
    return reinterpret_cast<T *>(handle);
}

template <typename T>
void setHandle(JNIEnv *env, jobject obj, T* t)
{
	// delete old object
    T* oldObj = getHandle<T>(env, obj);
    if (oldObj != NULL)
    	delete oldObj;

    jlong handle = reinterpret_cast<jlong>((void*)t);
    env->SetLongField(obj, getHandleField(env, obj), handle);
}

static jboolean cast(bool ret) {
    return ret ? JNI_TRUE : JNI_FALSE;
}

#endif /* end of include guard */

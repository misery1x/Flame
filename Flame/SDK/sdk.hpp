#pragma once

#include <Windows.h>
#include <string>
#include "../Framework/JNI/jni.h"

namespace SDK
{
    void    SetVM(JavaVM* vm);
    JavaVM* GetVM();
    JNIEnv* GetEnv();
    std::string JStringToString(JNIEnv* env, jstring str);
    bool CheckException(JNIEnv* env);
}

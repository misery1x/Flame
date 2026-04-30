#include "sdk.hpp"
#include <cstdio>

namespace SDK
{
    static JavaVM* g_pJavaVM = nullptr;

    void SetVM(JavaVM* vm)
    {
        g_pJavaVM = vm;
    }

    JavaVM* GetVM()
    {
        return g_pJavaVM;
    }

    JNIEnv* GetEnv()
    {
        if (!g_pJavaVM)
            return nullptr;

        JNIEnv* env = nullptr;
        jint result = g_pJavaVM->GetEnv(reinterpret_cast<void**>(&env), JNI_VERSION_1_8);

        if (result == JNI_EDETACHED)
        {
            g_pJavaVM->AttachCurrentThread(reinterpret_cast<void**>(&env), nullptr);
        }

        return env;
    }

    std::string JStringToString(JNIEnv* env, jstring str)
    {
        if (!env || !str)
            return "";

        const char* chars = env->GetStringUTFChars(str, nullptr);
        if (!chars)
            return "";

        std::string result(chars);
        env->ReleaseStringUTFChars(str, chars);
        return result;
    }

    bool CheckException(JNIEnv* env)
    {
        if (env && env->ExceptionCheck())
        {
            env->ExceptionDescribe();
            env->ExceptionClear();
            return true;
        }
        return false;
    }
}

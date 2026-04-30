#pragma once

#include "../../Framework/JNI/jni.h"

class CGameSettings
{
private:
    JNIEnv* m_env;
    jobject m_obj;

public:
    CGameSettings() : m_env(nullptr), m_obj(nullptr) {}
    CGameSettings(JNIEnv* env, jobject obj) : m_env(env), m_obj(obj) {}

    bool    isValid()   const { return m_env && m_obj; }

    float getFOV();
    int   getThirdPersonView();
    void  deleteRef();
};

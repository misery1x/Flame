#pragma once

#include "../../Framework/JNI/jni.h"

class CItem
{
private:
    JNIEnv* m_env;
    jobject m_obj;

public:
    CItem() : m_env(nullptr), m_obj(nullptr) {}
    CItem(JNIEnv* env, jobject obj) : m_env(env), m_obj(obj) {}

    bool    isValid()   const { return m_env && m_obj; }
    jobject getObject() const { return m_obj; }

    bool isBlock();
    void deleteRef();
};

#pragma once

#include <string>
#include "../../Framework/JNI/jni.h"
#include "../Item/Item.hpp"

class CItemStack
{
private:
    JNIEnv* m_env;
    jobject m_obj;

public:
    CItemStack() : m_env(nullptr), m_obj(nullptr) {}
    CItemStack(JNIEnv* env, jobject obj) : m_env(env), m_obj(obj) {}

    bool    isValid()   const { return m_env && m_obj; }
    jobject getObject() const { return m_obj; }

    std::string getDisplayName();
    CItem getItem();
    void deleteRef();
};

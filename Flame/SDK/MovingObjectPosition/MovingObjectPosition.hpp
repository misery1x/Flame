#pragma once

#include "../../Framework/JNI/jni.h"

enum class HitType { MISS = 0, BLOCK = 1, ENTITY = 2 };

class CMovingObjectPosition
{
private:
    JNIEnv* m_env;
    jobject m_obj;

public:
    CMovingObjectPosition() : m_env(nullptr), m_obj(nullptr) {}
    CMovingObjectPosition(JNIEnv* env, jobject obj) : m_env(env), m_obj(obj) {}

    bool    isValid()   const { return m_env && m_obj; }
    jobject getObject() const { return m_obj; }

    HitType getTypeOfHit();
    void    deleteRef();
};

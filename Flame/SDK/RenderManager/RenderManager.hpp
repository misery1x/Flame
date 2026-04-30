#pragma once

#include "../../Framework/JNI/jni.h"

class CRenderManager
{
private:
    JNIEnv* m_env;
    jobject m_obj;

public:
    CRenderManager() : m_env(nullptr), m_obj(nullptr) {}
    CRenderManager(JNIEnv* env, jobject obj) : m_env(env), m_obj(obj) {}

    bool    isValid()   const { return m_env && m_obj; }
    jobject getObject() const { return m_obj; }

    double getRenderPosX();
    double getRenderPosY();
    double getRenderPosZ();
    void   getRenderPos(double& x, double& y, double& z);
    void   deleteRef();
};

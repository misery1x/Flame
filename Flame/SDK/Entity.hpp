#pragma once

#include <string>
#include "../Framework/JNI/jni.h"

class CEntity
{
protected:
    JNIEnv* m_env;
    jobject m_obj;

public:
    CEntity() : m_env(nullptr), m_obj(nullptr) {}
    CEntity(JNIEnv* env, jobject obj) : m_env(env), m_obj(obj) {}

    bool    isValid()    const { return m_env && m_obj; }
    jobject getObject()  const { return m_obj; }
    JNIEnv* getEnv()     const { return m_env; }

    double getPosX();
    double getPosY();
    double getPosZ();

    double getLastTickPosX();
    double getLastTickPosY();
    double getLastTickPosZ();

    double getRenderX(float partialTicks);
    double getRenderY(float partialTicks);
    double getRenderZ(float partialTicks);

    std::string getName();
    float       getHeight();

    float getRotationYaw();
    float getRotationPitch();

    bool isInstanceOf(const char* className);
};

#pragma once

#include <vector>
#include "../Entity.hpp"

class CWorld
{
private:
    JNIEnv* m_env;
    jobject m_obj;

public:
    CWorld() : m_env(nullptr), m_obj(nullptr) {}
    CWorld(JNIEnv* env, jobject obj) : m_env(env), m_obj(obj) {}

    bool    isValid()   const { return m_env && m_obj; }
    jobject getObject() const { return m_obj; }

    std::vector<CEntity> getLoadedEntityList();
};

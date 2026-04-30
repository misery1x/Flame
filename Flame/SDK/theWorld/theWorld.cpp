#include "theWorld.hpp"
#include "../sdk.hpp"

static jclass    s_worldClass       = nullptr;
static jfieldID  s_loadedEntityList = nullptr;

static jclass    s_listClass        = nullptr;
static jmethodID s_listSize         = nullptr;
static jmethodID s_listGet          = nullptr;

static void EnsureWorldCached(JNIEnv* env)
{
    if (s_worldClass) return;

    jclass local = env->FindClass("adm");
    if (!local) { SDK::CheckException(env); return; }
    s_worldClass = (jclass)env->NewGlobalRef(local);
    env->DeleteLocalRef(local);

    s_loadedEntityList = env->GetFieldID(s_worldClass, "f", "Ljava/util/List;");

    local = env->FindClass("java/util/List");
    if (local)
    {
        s_listClass = (jclass)env->NewGlobalRef(local);
        s_listSize  = env->GetMethodID(s_listClass, "size", "()I");
        s_listGet   = env->GetMethodID(s_listClass, "get",  "(I)Ljava/lang/Object;");
        env->DeleteLocalRef(local);
    }

    SDK::CheckException(env);
}

std::vector<CEntity> CWorld::getLoadedEntityList()
{
    std::vector<CEntity> entities;
    if (!isValid()) return entities;

    EnsureWorldCached(m_env);
    if (!s_loadedEntityList || !s_listSize || !s_listGet)
        return entities;

    jobject list = m_env->GetObjectField(m_obj, s_loadedEntityList);
    if (!list) return entities;

    int size = m_env->CallIntMethod(list, s_listSize);
    if (SDK::CheckException(m_env) || size <= 0)
    {
        m_env->DeleteLocalRef(list);
        return entities;
    }

    entities.reserve(size);

    for (int i = 0; i < size; i++)
    {
        jobject entityObj = m_env->CallObjectMethod(list, s_listGet, i);
        if (SDK::CheckException(m_env)) break;

        if (entityObj)
        {
            entities.emplace_back(m_env, entityObj);
        }
    }

    m_env->DeleteLocalRef(list);
    return entities;
}

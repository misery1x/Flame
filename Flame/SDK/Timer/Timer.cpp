#include "Timer.hpp"
#include "../sdk.hpp"

static jclass   s_timerClass   = nullptr;
static jfieldID s_partialTicks = nullptr;

static void EnsureCached(JNIEnv* env) {
    if (s_timerClass) return;

    jclass local = env->FindClass("avl");
    if (!local) { SDK::CheckException(env); return; }
    s_timerClass   = (jclass)env->NewGlobalRef(local);
    s_partialTicks = env->GetFieldID(s_timerClass, "c", "F");
    env->DeleteLocalRef(local);

    SDK::CheckException(env);
}

float CTimer::getPartialTicks() {
    if (!isValid()) return 1.0f;
    EnsureCached(m_env);
    if (!s_partialTicks) return 1.0f;
    return m_env->GetFloatField(m_obj, s_partialTicks);
}

void CTimer::deleteRef() {
    if (m_env && m_obj) {
        m_env->DeleteLocalRef(m_obj);
        m_obj = nullptr;
    }
}

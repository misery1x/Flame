#include "GameSettings.hpp"
#include "../sdk.hpp"

static jclass   s_gsClass    = nullptr;
static jfieldID s_fovSetting = nullptr;
static jfieldID s_tpView     = nullptr;

static void EnsureCached(JNIEnv* env) {
    if (s_gsClass) return;

    jclass local = env->FindClass("avh");
    if (!local) { SDK::CheckException(env); return; }
    s_gsClass    = (jclass)env->NewGlobalRef(local);
    s_fovSetting = env->GetFieldID(s_gsClass, "aI", "F");
    s_tpView     = env->GetFieldID(s_gsClass, "aB", "I");
    env->DeleteLocalRef(local);

    SDK::CheckException(env);
}

float CGameSettings::getFOV() {
    if (!isValid()) return 70.0f;
    EnsureCached(m_env);
    if (!s_fovSetting) return 70.0f;
    float fov = m_env->GetFloatField(m_obj, s_fovSetting);
    return (fov < 10.0f || fov > 170.0f) ? 70.0f : fov;
}

int CGameSettings::getThirdPersonView() {
    if (!isValid()) return 0;
    EnsureCached(m_env);
    if (!s_tpView) return 0;
    return m_env->GetIntField(m_obj, s_tpView);
}

void CGameSettings::deleteRef() {
    if (m_env && m_obj) {
        m_env->DeleteLocalRef(m_obj);
        m_obj = nullptr;
    }
}

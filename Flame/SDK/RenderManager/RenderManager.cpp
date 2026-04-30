#include "RenderManager.hpp"
#include "../sdk.hpp"

static jclass   s_rmClass    = nullptr;
static jfieldID s_renderPosX = nullptr;
static jfieldID s_renderPosY = nullptr;
static jfieldID s_renderPosZ = nullptr;

static void EnsureCached(JNIEnv* env) {
    if (s_rmClass) return;

    jclass local = env->FindClass("biu");
    if (!local) { SDK::CheckException(env); return; }
    s_rmClass    = (jclass)env->NewGlobalRef(local);
    s_renderPosX = env->GetFieldID(s_rmClass, "o", "D");
    s_renderPosY = env->GetFieldID(s_rmClass, "p", "D");
    s_renderPosZ = env->GetFieldID(s_rmClass, "q", "D");
    env->DeleteLocalRef(local);

    SDK::CheckException(env);
}

double CRenderManager::getRenderPosX() {
    if (!isValid()) return 0.0;
    EnsureCached(m_env);
    return s_renderPosX ? m_env->GetDoubleField(m_obj, s_renderPosX) : 0.0;
}

double CRenderManager::getRenderPosY() {
    if (!isValid()) return 0.0;
    EnsureCached(m_env);
    return s_renderPosY ? m_env->GetDoubleField(m_obj, s_renderPosY) : 0.0;
}

double CRenderManager::getRenderPosZ() {
    if (!isValid()) return 0.0;
    EnsureCached(m_env);
    return s_renderPosZ ? m_env->GetDoubleField(m_obj, s_renderPosZ) : 0.0;
}

void CRenderManager::getRenderPos(double& x, double& y, double& z) {
    x = getRenderPosX();
    y = getRenderPosY();
    z = getRenderPosZ();
}

void CRenderManager::deleteRef() {
    if (m_env && m_obj) {
        m_env->DeleteLocalRef(m_obj);
        m_obj = nullptr;
    }
}

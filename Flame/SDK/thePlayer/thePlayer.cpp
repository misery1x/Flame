#include "thePlayer.hpp"
#include "../sdk.hpp"

static jclass    s_livingClass       = nullptr;
static jmethodID s_getHeldItem       = nullptr;
static jfieldID  s_hurtTime          = nullptr;
static jfieldID  s_onGround          = nullptr;

static void EnsurePlayerCached(JNIEnv* env) {
    if (s_livingClass) return;

    jclass local = env->FindClass("pr");
    if (local) {
        s_livingClass      = (jclass)env->NewGlobalRef(local);
        s_getHeldItem      = env->GetMethodID(s_livingClass, "bA", "()Lzx;");
        s_hurtTime         = env->GetFieldID(s_livingClass, "au", "I");
        env->DeleteLocalRef(local);
    }

    local = env->FindClass("pk");
    if (local) {
        s_onGround = env->GetFieldID(local, "C", "Z");
        env->DeleteLocalRef(local);
    }

    SDK::CheckException(env);
}

CItemStack CEntityPlayer::getHeldItem() {
    if (!isValid()) return CItemStack();
    EnsurePlayerCached(m_env);
    if (!s_getHeldItem) return CItemStack();

    jobject stack = m_env->CallObjectMethod(m_obj, s_getHeldItem);
    if (SDK::CheckException(m_env) || !stack) return CItemStack();
    return CItemStack(m_env, stack);
}

int CEntityPlayer::getHurtTime() {
    if (!isValid()) return 0;
    EnsurePlayerCached(m_env);
    if (!s_hurtTime) return 0;
    return m_env->GetIntField(m_obj, s_hurtTime);
}

bool CEntityPlayer::isOnGround() {
    if (!isValid()) return false;
    EnsurePlayerCached(m_env);
    if (!s_onGround) return false;
    return m_env->GetBooleanField(m_obj, s_onGround);
}

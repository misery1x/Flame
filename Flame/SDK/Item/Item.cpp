#include "Item.hpp"
#include "../sdk.hpp"

static jclass s_itemBlockClass = nullptr;

static void EnsureCached(JNIEnv* env) {
    if (s_itemBlockClass) return;

    jclass local = env->FindClass("aeb");
    if (local) {
        s_itemBlockClass = (jclass)env->NewGlobalRef(local);
        env->DeleteLocalRef(local);
    }

    SDK::CheckException(env);
}

bool CItem::isBlock() {
    if (!isValid()) return false;
    EnsureCached(m_env);
    if (!s_itemBlockClass) return false;
    return m_env->IsInstanceOf(m_obj, s_itemBlockClass);
}

void CItem::deleteRef() {
    if (m_env && m_obj) {
        m_env->DeleteLocalRef(m_obj);
        m_obj = nullptr;
    }
}

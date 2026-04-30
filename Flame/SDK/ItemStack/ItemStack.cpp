#include "ItemStack.hpp"
#include "../sdk.hpp"

static jclass    s_itemStackClass = nullptr;
static jmethodID s_getDisplayName = nullptr;
static jmethodID s_getItem        = nullptr;

static void EnsureCached(JNIEnv* env) {
    if (s_itemStackClass) return;

    jclass local = env->FindClass("zx");
    if (!local) { SDK::CheckException(env); return; }
    s_itemStackClass = (jclass)env->NewGlobalRef(local);
    env->DeleteLocalRef(local);

    s_getDisplayName = env->GetMethodID(s_itemStackClass, "q", "()Ljava/lang/String;");
    s_getItem        = env->GetMethodID(s_itemStackClass, "b", "()Lzw;");

    SDK::CheckException(env);
}

std::string CItemStack::getDisplayName() {
    if (!isValid()) return "";
    EnsureCached(m_env);
    if (!s_getDisplayName) return "";

    jstring jName = (jstring)m_env->CallObjectMethod(m_obj, s_getDisplayName);
    if (SDK::CheckException(m_env) || !jName) return "";

    std::string name = SDK::JStringToString(m_env, jName);
    m_env->DeleteLocalRef(jName);
    return name;
}

CItem CItemStack::getItem() {
    if (!isValid()) return CItem();
    EnsureCached(m_env);
    if (!s_getItem) return CItem();

    jobject item = m_env->CallObjectMethod(m_obj, s_getItem);
    if (SDK::CheckException(m_env) || !item) return CItem();
    return CItem(m_env, item);
}

void CItemStack::deleteRef() {
    if (m_env && m_obj) {
        m_env->DeleteLocalRef(m_obj);
        m_obj = nullptr;
    }
}

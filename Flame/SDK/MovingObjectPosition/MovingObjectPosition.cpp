#include "MovingObjectPosition.hpp"
#include "../sdk.hpp"

static jclass   s_mopClass    = nullptr;
static jfieldID s_typeOfHit   = nullptr;
static jmethodID s_ordinal    = nullptr;

static void EnsureCached(JNIEnv* env) {
    if (s_mopClass) return;

    jclass local = env->FindClass("auh");
    if (!local) { SDK::CheckException(env); return; }
    s_mopClass = (jclass)env->NewGlobalRef(local);
    env->DeleteLocalRef(local);

    s_typeOfHit = env->GetFieldID(s_mopClass, "a", "Lauh$a;");

    jclass enumClass = env->FindClass("java/lang/Enum");
    if (enumClass) {
        s_ordinal = env->GetMethodID(enumClass, "ordinal", "()I");
        env->DeleteLocalRef(enumClass);
    }

    SDK::CheckException(env);
}

HitType CMovingObjectPosition::getTypeOfHit() {
    if (!isValid()) return HitType::MISS;
    EnsureCached(m_env);
    if (!s_typeOfHit || !s_ordinal) return HitType::MISS;

    jobject hitEnum = m_env->GetObjectField(m_obj, s_typeOfHit);
    if (!hitEnum) return HitType::MISS;

    int ord = m_env->CallIntMethod(hitEnum, s_ordinal);
    m_env->DeleteLocalRef(hitEnum);
    return (HitType)ord;
}

void CMovingObjectPosition::deleteRef() {
    if (m_env && m_obj) {
        m_env->DeleteLocalRef(m_obj);
        m_obj = nullptr;
    }
}

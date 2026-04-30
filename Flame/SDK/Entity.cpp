#include "Entity.hpp"
#include "sdk.hpp"

static jclass    s_entityClass   = nullptr;
static jfieldID  s_posX          = nullptr;
static jfieldID  s_posY          = nullptr;
static jfieldID  s_posZ          = nullptr;
static jfieldID  s_lastTickPosX  = nullptr;
static jfieldID  s_lastTickPosY  = nullptr;
static jfieldID  s_lastTickPosZ  = nullptr;
static jfieldID  s_height        = nullptr;
static jmethodID s_getName       = nullptr;
static jfieldID  s_rotationYaw   = nullptr;
static jfieldID  s_rotationPitch = nullptr;

static void EnsureCached(JNIEnv* env) {
    if (s_entityClass) return;

    jclass local = env->FindClass("pk");
    if (!local) { SDK::CheckException(env); return; }
    s_entityClass = (jclass)env->NewGlobalRef(local);
    env->DeleteLocalRef(local);

    s_posX          = env->GetFieldID(s_entityClass, "s", "D");
    s_posY          = env->GetFieldID(s_entityClass, "t", "D");
    s_posZ          = env->GetFieldID(s_entityClass, "u", "D");
    s_lastTickPosX  = env->GetFieldID(s_entityClass, "P", "D");
    s_lastTickPosY  = env->GetFieldID(s_entityClass, "Q", "D");
    s_lastTickPosZ  = env->GetFieldID(s_entityClass, "R", "D");
    s_height        = env->GetFieldID(s_entityClass, "K", "F");
    s_getName       = env->GetMethodID(s_entityClass, "e_", "()Ljava/lang/String;");
    s_rotationYaw   = env->GetFieldID(s_entityClass, "y", "F");
    s_rotationPitch = env->GetFieldID(s_entityClass, "z", "F");
    SDK::CheckException(env);
}

double CEntity::getPosX() {
    EnsureCached(m_env);
    return s_posX ? m_env->GetDoubleField(m_obj, s_posX) : 0.0;
}

double CEntity::getPosY() {
    EnsureCached(m_env);
    return s_posY ? m_env->GetDoubleField(m_obj, s_posY) : 0.0;
}

double CEntity::getPosZ() {
    EnsureCached(m_env);
    return s_posZ ? m_env->GetDoubleField(m_obj, s_posZ) : 0.0;
}

double CEntity::getLastTickPosX() {
    EnsureCached(m_env);
    return s_lastTickPosX ? m_env->GetDoubleField(m_obj, s_lastTickPosX) : 0.0;
}

double CEntity::getLastTickPosY() {
    EnsureCached(m_env);
    return s_lastTickPosY ? m_env->GetDoubleField(m_obj, s_lastTickPosY) : 0.0;
}

double CEntity::getLastTickPosZ() {
    EnsureCached(m_env);
    return s_lastTickPosZ ? m_env->GetDoubleField(m_obj, s_lastTickPosZ) : 0.0;
}

double CEntity::getRenderX(float pt) {
    return getLastTickPosX() + (getPosX() - getLastTickPosX()) * pt;
}

double CEntity::getRenderY(float pt) {
    return getLastTickPosY() + (getPosY() - getLastTickPosY()) * pt;
}

double CEntity::getRenderZ(float pt) {
    return getLastTickPosZ() + (getPosZ() - getLastTickPosZ()) * pt;
}

std::string CEntity::getName() {
    EnsureCached(m_env);
    if (!s_getName) return "";
    jstring jstr = (jstring)m_env->CallObjectMethod(m_obj, s_getName);
    if (SDK::CheckException(m_env) || !jstr) return "";
    std::string result = SDK::JStringToString(m_env, jstr);
    m_env->DeleteLocalRef(jstr);
    return result;
}

float CEntity::getHeight() {
    EnsureCached(m_env);
    return s_height ? m_env->GetFloatField(m_obj, s_height) : 1.8f;
}

float CEntity::getRotationYaw() {
    EnsureCached(m_env);
    return s_rotationYaw ? m_env->GetFloatField(m_obj, s_rotationYaw) : 0.0f;
}

float CEntity::getRotationPitch() {
    EnsureCached(m_env);
    return s_rotationPitch ? m_env->GetFloatField(m_obj, s_rotationPitch) : 0.0f;
}

bool CEntity::isInstanceOf(const char* className) {
    if (!m_env || !m_obj) return false;
    jclass cls = m_env->FindClass(className);
    if (!cls || SDK::CheckException(m_env)) return false;
    bool result = m_env->IsInstanceOf(m_obj, cls);
    m_env->DeleteLocalRef(cls);
    return result;
}

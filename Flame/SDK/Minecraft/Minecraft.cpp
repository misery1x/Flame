#include "Minecraft.hpp"
#include "../sdk.hpp"

static jclass    s_mcClass        = nullptr;
static jmethodID s_getMinecraft   = nullptr;
static jfieldID  s_thePlayer      = nullptr;
static jfieldID  s_theWorld       = nullptr;
static jfieldID  s_timer          = nullptr;
static jfieldID  s_currentScreen  = nullptr;
static jmethodID s_getRenderMgr   = nullptr;
static jfieldID  s_gameSettings   = nullptr;
static jfieldID  s_objectMouseOver = nullptr;
static jmethodID s_rightClickMouse = nullptr;

static void EnsureCached(JNIEnv* env) {
    if (s_mcClass) return;

    jclass local = env->FindClass("ave");
    if (!local) { SDK::CheckException(env); return; }
    s_mcClass = (jclass)env->NewGlobalRef(local);
    env->DeleteLocalRef(local);

    s_getMinecraft    = env->GetStaticMethodID(s_mcClass, "A", "()Lave;");
    s_thePlayer       = env->GetFieldID(s_mcClass, "h", "Lbew;");
    s_theWorld        = env->GetFieldID(s_mcClass, "f", "Lbdb;");
    s_timer           = env->GetFieldID(s_mcClass, "Y", "Lavl;");
    s_currentScreen   = env->GetFieldID(s_mcClass, "m", "Laxu;");
    s_getRenderMgr    = env->GetMethodID(s_mcClass, "af", "()Lbiu;");
    s_gameSettings    = env->GetFieldID(s_mcClass, "t", "Lavh;");
    s_objectMouseOver = env->GetFieldID(s_mcClass, "s", "Lauh;");
    s_rightClickMouse = env->GetMethodID(s_mcClass, "ax", "()V");

    SDK::CheckException(env);
}

CMinecraft::CMinecraft(JNIEnv* env) : m_env(env), m_obj(nullptr) {
    if (!env) return;
    EnsureCached(env);
    if (s_getMinecraft) {
        m_obj = env->CallStaticObjectMethod(s_mcClass, s_getMinecraft);
        SDK::CheckException(env);
    }
}

CEntityPlayer CMinecraft::thePlayer() {
    if (!isValid() || !s_thePlayer) return CEntityPlayer();
    return CEntityPlayer(m_env, m_env->GetObjectField(m_obj, s_thePlayer));
}

CWorld CMinecraft::theWorld() {
    if (!isValid() || !s_theWorld) return CWorld();
    return CWorld(m_env, m_env->GetObjectField(m_obj, s_theWorld));
}

CMovingObjectPosition CMinecraft::getObjectMouseOver() {
    if (!isValid() || !s_objectMouseOver) return CMovingObjectPosition();
    jobject mop = m_env->GetObjectField(m_obj, s_objectMouseOver);
    if (!mop) return CMovingObjectPosition();
    return CMovingObjectPosition(m_env, mop);
}

float CMinecraft::getRenderPartialTicks() {
    CTimer timer(m_env, nullptr);
    if (!isValid() || !s_timer) return 1.0f;
    jobject tObj = m_env->GetObjectField(m_obj, s_timer);
    if (!tObj) return 1.0f;
    CTimer t(m_env, tObj);
    float pt = t.getPartialTicks();
    t.deleteRef();
    return pt;
}

void CMinecraft::getRenderPos(double& x, double& y, double& z) {
    x = y = z = 0.0;
    if (!isValid() || !s_getRenderMgr) return;
    jobject rm = m_env->CallObjectMethod(m_obj, s_getRenderMgr);
    if (SDK::CheckException(m_env) || !rm) return;
    CRenderManager renderMgr(m_env, rm);
    renderMgr.getRenderPos(x, y, z);
    renderMgr.deleteRef();
}

bool CMinecraft::isInGui() {
    if (!isValid() || !s_currentScreen) return false;
    jobject screen = m_env->GetObjectField(m_obj, s_currentScreen);
    if (!screen) return false;
    m_env->DeleteLocalRef(screen);
    return true;
}

float CMinecraft::getFOV() {
    if (!isValid() || !s_gameSettings) return 70.0f;
    jobject gsObj = m_env->GetObjectField(m_obj, s_gameSettings);
    if (!gsObj) return 70.0f;
    CGameSettings gs(m_env, gsObj);
    float fov = gs.getFOV();
    gs.deleteRef();
    return fov;
}

int CMinecraft::getThirdPersonView() {
    if (!isValid() || !s_gameSettings) return 0;
    jobject gsObj = m_env->GetObjectField(m_obj, s_gameSettings);
    if (!gsObj) return 0;
    CGameSettings gs(m_env, gsObj);
    int view = gs.getThirdPersonView();
    gs.deleteRef();
    return view;
}

void CMinecraft::rightClickMouse() {
    if (!isValid() || !s_rightClickMouse) return;
    m_env->CallVoidMethod(m_obj, s_rightClickMouse);
    SDK::CheckException(m_env);
}

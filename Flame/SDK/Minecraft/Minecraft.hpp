#pragma once

#include "../Entity.hpp"
#include "../thePlayer/thePlayer.hpp"
#include "../theWorld/theWorld.hpp"
#include "../GameSettings/GameSettings.hpp"
#include "../Timer/Timer.hpp"
#include "../RenderManager/RenderManager.hpp"
#include "../MovingObjectPosition/MovingObjectPosition.hpp"

class CMinecraft
{
private:
    JNIEnv* m_env;
    jobject m_obj;

public:
    CMinecraft(JNIEnv* env);

    bool isValid() const { return m_env && m_obj; }
    jobject getObject() const { return m_obj; }

    CEntityPlayer  thePlayer();
    CWorld         theWorld();
    CMovingObjectPosition getObjectMouseOver();

    float getRenderPartialTicks();
    void  getRenderPos(double& x, double& y, double& z);
    bool  isInGui();
    float getFOV();
    int   getThirdPersonView();

    void rightClickMouse();
};

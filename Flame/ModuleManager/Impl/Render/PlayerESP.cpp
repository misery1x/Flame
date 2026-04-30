#include "PlayerESP.hpp"

#include "../../Framework/ImGui/imgui.h"
#include "../../Rendering/RenderUtils.hpp"
#include "../../SDK/Minecraft/Minecraft.hpp"
#include "../../SDK/sdk.hpp"

#include <cmath>

#ifndef M_PI
#define M_PI 3.14159265358979323846
#endif

static float CalcBoxWidth(ImVec2 top, ImVec2 bot, double eX, double eY, double eZ, float height, double camX, double camY, double camZ)
{
    float hw = 0.3f;
    ImVec2 pts[4];
    double corners[4][3] =
    {
        { eX - hw, eY + height * 0.5, eZ - hw },
        { eX + hw, eY + height * 0.5, eZ - hw },
        { eX + hw, eY + height * 0.5, eZ + hw },
        { eX - hw, eY + height * 0.5, eZ + hw },
    };
    float minX = 99999, maxX = -99999;
    for (int i = 0; i < 4; i++)
    {
        bool vis = RenderUtils::WorldToScreen(corners[i][0], corners[i][1], corners[i][2], camX, camY, camZ, pts[i]);
        if (vis)
        {
            if (pts[i].x < minX) minX = pts[i].x;
            if (pts[i].x > maxX) maxX = pts[i].x;
        }
    }
    float w = (maxX - minX) / 2.0f;
    if (w > 4.0f) return w;
    return (bot.y - top.y) * 0.25f;
}

static void Draw2DBox(ImDrawList* dl, ImVec2 top, ImVec2 bot, ImU32 color, bool outline, float w)
{
    ImVec2 tl(top.x - w, top.y);
    ImVec2 br(top.x + w, bot.y);

    if (outline)
    {
        dl->AddRect(ImVec2(tl.x - 1, tl.y - 1), ImVec2(br.x + 1, br.y + 1), IM_COL32(0, 0, 0, 200), 0.0f, 0, 1.0f);
        dl->AddRect(ImVec2(tl.x + 1, tl.y + 1), ImVec2(br.x - 1, br.y - 1), IM_COL32(0, 0, 0, 200), 0.0f, 0, 1.0f);
    }
    dl->AddRect(tl, br, color, 0.0f, 0, 1.0f);
}

void PlayerESP::onRender()
{
    JNIEnv* env = SDK::GetEnv();
    if (!env) return;

    bool showBox = getBool("Box");
    bool outline = getBool("Outline");

    if (!showBox) return;

    if (env->PushLocalFrame(512) < 0) return;

    CMinecraft mc(env);
    if (!mc.isValid()) { env->PopLocalFrame(nullptr); return; }

    CWorld world = mc.theWorld();
    if (!world.isValid()) { env->PopLocalFrame(nullptr); return; }

    CEntityPlayer localPlayer = mc.thePlayer();
    if (!localPlayer.isValid()) { env->PopLocalFrame(nullptr); return; }

    float partialTicks = mc.getRenderPartialTicks();
    float yaw          = localPlayer.getRotationYaw();
    float pitch        = localPlayer.getRotationPitch();
    float fov          = mc.getFOV();
    int   thirdPerson  = mc.getThirdPersonView();

    float viewYaw   = (thirdPerson == 2) ? yaw + 180.0f : yaw;
    float viewPitch = (thirdPerson == 2) ? -pitch        : pitch;

    RenderUtils::UpdateMatrices(viewYaw, viewPitch, fov);

    double camX, camY, camZ;
    mc.getRenderPos(camX, camY, camZ);
    camY += 1.62;

    if (thirdPerson > 0)
    {
        constexpr double TP_DIST = 4.0;
        double yawRad   = yaw   * M_PI / 180.0;
        double pitchRad = pitch * M_PI / 180.0;
        double sign     = (thirdPerson == 2) ? -1.0 : 1.0;
        camX += sign *  sin(yawRad) * cos(pitchRad) * TP_DIST;
        camY += sign *  sin(pitchRad) * TP_DIST;
        camZ += sign * -cos(yawRad) * cos(pitchRad) * TP_DIST;
    }

    double lpX = localPlayer.getRenderX(partialTicks);
    double lpY = localPlayer.getRenderY(partialTicks);
    double lpZ = localPlayer.getRenderZ(partialTicks);

    ImDrawList* drawList = ImGui::GetBackgroundDrawList();
    if (!drawList) { env->PopLocalFrame(nullptr); return; }

    auto entities = world.getLoadedEntityList();
    ImU32 boxColor = IM_COL32(255, 40, 40, 255);

    for (auto& entity : entities)
    {
        if (!entity.isValid()) continue;
        if (!entity.isInstanceOf("bex")) continue;
        if (env->IsSameObject(entity.getObject(), localPlayer.getObject())) continue;

        double eX = entity.getRenderX(partialTicks);
        double eY = entity.getRenderY(partialTicks);
        double eZ = entity.getRenderZ(partialTicks);

        double dx = eX - lpX, dy = eY - lpY, dz = eZ - lpZ;
        double dist = sqrt(dx * dx + dy * dy + dz * dz);
        if (dist < 0.5) continue;

        float height = entity.getHeight();

        ImVec2 footScreen, headScreen;
        bool footVis = RenderUtils::WorldToScreen(eX, eY, eZ, camX, camY, camZ, footScreen);
        bool headVis = RenderUtils::WorldToScreen(eX, eY + height + 0.15, eZ, camX, camY, camZ, headScreen);

        if (!footVis || !headVis) continue;

        float boxW = CalcBoxWidth(headScreen, footScreen, eX, eY, eZ, height, camX, camY, camZ);
        Draw2DBox(drawList, headScreen, footScreen, boxColor, outline, boxW);
    }

    env->PopLocalFrame(nullptr);
}

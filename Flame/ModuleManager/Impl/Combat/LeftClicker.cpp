#include "LeftClicker.hpp"

#include "../../SDK/sdk.hpp"
#include "../../SDK/Minecraft/Minecraft.hpp"
#include "../../SDK/MovingObjectPosition/MovingObjectPosition.hpp"
#include "../../Rendering/Hooking/hooking.hpp"

#include <Windows.h>
#include <cstdlib>
#include <chrono>

static double GetTimeMs() {
    auto now = std::chrono::high_resolution_clock::now();
    return std::chrono::duration<double, std::milli>(now.time_since_epoch()).count();
}

static double RandDelay(int minCPS, int maxCPS) {
    if (minCPS <= 0) minCPS = 1;
    if (maxCPS <= minCPS) maxCPS = minCPS;

    int range = maxCPS - minCPS + 1;
    int r1 = rand() % range;
    int r2 = rand() % range;
    int cps = minCPS + (r1 + r2) / 2;

    double base = 1000.0 / cps;

    double jitter = base * (((rand() % 160) - 80) / 1000.0);
    return base + jitter;
}

static int RandInt(int min, int max) {
    if (min >= max) return min;
    return min + (rand() % (max - min + 1));
}

void LeftClicker::onUpdate() {
    if (!(GetAsyncKeyState(VK_LBUTTON) & 0x8000)) {
        m_mouseDown = false;
        m_blockHitAllowed = false;
        m_blockHitTimerStarted = false;
        m_blockHitUntil = 0.0;
        return;
    }

    JNIEnv* env = SDK::GetEnv();
    if (!env) return;

    int  minCPS    = getInt("Min CPS");
    int  maxCPS    = getInt("Max CPS");
    bool breakBlk  = getBool("Break Blocks");
    bool swordOnly = getBool("Sword Only");
    bool blockHit  = getBool("Block Hit");
    int  blockMs   = getInt("Block Hit Ms");

    CMinecraft mc(env);
    if (!mc.isValid()) return;
    if (mc.isInGui()) return;

    if (breakBlk) {
        CMovingObjectPosition mop = mc.getObjectMouseOver();
        if (mop.isValid()) {
            if (mop.getTypeOfHit() == HitType::BLOCK) {
                mop.deleteRef();
                return;
            }
            mop.deleteRef();
        }
    }

    CEntityPlayer player = mc.thePlayer();
    if (!player.isValid()) return;

    std::string heldItemName;
    if (swordOnly || blockHit) {
        CItemStack held = player.getHeldItem();
        if (held.isValid()) {
            heldItemName = held.getDisplayName();
            held.deleteRef();

            for (auto& c : heldItemName) c = (char)tolower((unsigned char)c);
        }
        if (swordOnly && heldItemName.find("sword") == std::string::npos)
            return;
    }

    double now = GetTimeMs();

    if (blockHit)
    {

        if (m_blockHitTimerStarted)
        {
            if (now >= m_blockHitUntil)
            {

                POINT pos;
                GetCursorPos(&pos);
                PostMessage(Hooking::hGameWindow, WM_RBUTTONUP, 0, MAKELPARAM(pos.x, pos.y));
                m_blockHitTimerStarted = false;
            }

            return;
        }

        if (heldItemName.find("sword") != std::string::npos)
        {
            CMovingObjectPosition mop = mc.getObjectMouseOver();
            if (mop.isValid())
            {
                if (mop.getTypeOfHit() == HitType::ENTITY)
                {

                    CWorld world = mc.theWorld();
                    if (world.isValid())
                    {
                        auto entities = world.getLoadedEntityList();
                        float pt = mc.getRenderPartialTicks();
                        double myX = player.getRenderX(pt);
                        double myY = player.getRenderY(pt) + 1.62;
                        double myZ = player.getRenderZ(pt);

                        int enemyHurtTime = 0;
                        double bestDist = 6.0;

                        for (auto& entity : entities) {
                            if (!entity.isValid()) continue;
                            if (!entity.isInstanceOf("bex")) continue;
                            if (env->IsSameObject(entity.getObject(), player.getObject())) continue;

                            double eX = entity.getRenderX(pt);
                            double eY = entity.getRenderY(pt);
                            double eZ = entity.getRenderZ(pt);
                            double dx = eX - myX, dy = (eY + entity.getHeight() * 0.5) - myY, dz = eZ - myZ;
                            double dist = sqrt(dx * dx + dy * dy + dz * dz);

                            if (dist < bestDist) {
                                bestDist = dist;
                                CEntityPlayer ep(env, entity.getObject());
                                enemyHurtTime = ep.getHurtTime();
                            }
                        }

                        if (m_blockHitAllowed && enemyHurtTime > 0 && !m_blockHitTimerStarted)
                        {

                            POINT pos;
                            GetCursorPos(&pos);
                            PostMessage(Hooking::hGameWindow, WM_RBUTTONDOWN, MK_RBUTTON, MAKELPARAM(pos.x, pos.y));

                            int randMs = RandInt(blockMs - 5, blockMs + 5);
                            m_blockHitUntil = now + (double)randMs;
                            m_blockHitTimerStarted = true;
                            m_blockHitAllowed = false;
                        }
                        else
                        {
                            if (enemyHurtTime <= 1)
                                m_blockHitAllowed = true;
                        }
                    }
                }
                mop.deleteRef();
            }
        }
    }

    if (!m_mouseDown) {

        if (now < m_nextClickTime) return;

        POINT pos;
        GetCursorPos(&pos);
        PostMessage(Hooking::hGameWindow, WM_LBUTTONDOWN, MK_LBUTTON, MAKELPARAM(pos.x, pos.y));
        m_mouseDown = true;

        m_mouseUpTime = now + 40.0 + (rand() % 40);
    } else {

        if (now >= m_mouseUpTime) {
            POINT pos;
            GetCursorPos(&pos);
            PostMessage(Hooking::hGameWindow, WM_LBUTTONUP, 0, MAKELPARAM(pos.x, pos.y));
            m_mouseDown = false;
            m_nextClickTime = now + RandDelay(minCPS, maxCPS);
        }
    }
}

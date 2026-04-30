#include "RightClicker.hpp"

#include "../../SDK/sdk.hpp"
#include "../../SDK/Minecraft/Minecraft.hpp"

#include <Windows.h>
#include <cstdlib>
#include <chrono>

static double GetTimeMs() {
    auto now = std::chrono::high_resolution_clock::now();
    return std::chrono::duration<double, std::milli>(now.time_since_epoch()).count();
}

static double RandDelay(int minCPS, int maxCPS) {
    if (minCPS >= maxCPS) return 1000.0 / minCPS;
    int cps = minCPS + (rand() % (maxCPS - minCPS + 1));
    double base = 1000.0 / cps;
    return base + (rand() % 20) - 10;
}

void RightClicker::onUpdate() {
    if (!(GetAsyncKeyState(VK_RBUTTON) & 0x8000))
        return;

    JNIEnv* env = SDK::GetEnv();
    if (!env) return;

    int  minCPS     = getInt("Min CPS");
    int  maxCPS     = getInt("Max CPS");
    bool blocksOnly = getBool("Blocks Only");

    CMinecraft mc(env);
    if (!mc.isValid()) return;
    if (mc.isInGui()) return;

    if (blocksOnly) {
        CEntityPlayer player = mc.thePlayer();
        if (player.isValid()) {
            CItemStack held = player.getHeldItem();
            if (held.isValid()) {
                CItem item = held.getItem();
                if (!item.isValid() || !item.isBlock()) {
                    if (item.isValid()) item.deleteRef();
                    return;
                }
                if (item.isValid()) item.deleteRef();
            } else {
                return;
            }
        }
    }

    double now = GetTimeMs();
    if (now < m_nextClickTime) return;

    mc.rightClickMouse();

    m_nextClickTime = now + RandDelay(minCPS, maxCPS);
}

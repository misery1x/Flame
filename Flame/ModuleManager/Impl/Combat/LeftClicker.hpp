#pragma once

#include "../../Module.hpp"

class LeftClicker : public Module
{
public:
    LeftClicker() : Module("LeftClicker", "Combat")
    {
        addInt("Min CPS", 8, 1, 20);
        addInt("Max CPS", 12, 1, 20);
        addBool("Break Blocks", false);
        addBool("Sword Only", false);
        addBool("Block Hit", false);
        addInt("Block Hit Ms", 50, 20, 200);
    }

    void onUpdate() override;

private:
    double m_nextClickTime = 0.0;
    double m_mouseUpTime   = 0.0;
    bool   m_mouseDown     = false;

    bool   m_blockHitAllowed      = false;
    bool   m_blockHitTimerStarted = false;
    double m_blockHitUntil        = 0.0;
};

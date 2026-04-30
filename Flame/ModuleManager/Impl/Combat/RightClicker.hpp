#pragma once

#include "../../Module.hpp"

class RightClicker : public Module
{
public:
    RightClicker() : Module("RightClicker", "Combat")
    {
        addInt("Min CPS", 8, 1, 20);
        addInt("Max CPS", 12, 1, 20);
        addBool("Break Blocks", false);
        addBool("Combo", false);
        addBool("Blocks Only", false);
    }

    void onUpdate() override;

private:
    double m_nextClickTime = 0.0;
};

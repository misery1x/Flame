#pragma once

#include "../../Module.hpp"

class HUD : public Module
{
public:
    HUD() : Module("HUD", "Render")
    {
        enabled = true;
        addBool("Watermark", true);
        addFloat("Watermark Scale", 1.0f, 0.5f, 2.5f);
    }

    void onRender() override;
};

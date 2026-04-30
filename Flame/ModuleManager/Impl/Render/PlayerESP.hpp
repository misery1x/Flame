#pragma once

#include <string>
#include "../../Module.hpp"
#include "../../Framework/JNI/jni.h"

class PlayerESP : public Module
{
public:
    PlayerESP() : Module("PlayerESP", "Render")
    {
        addBool("Box", false);
        addBool("Outline", true);
    }

    void onRender() override;
};

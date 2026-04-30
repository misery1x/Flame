#pragma once

#include "../../../Framework/ImGui/imgui.h"

namespace FlameUI
{

    bool Checkbox(const char* label, bool* v);
    bool SliderFloat(const char* label, float* v, float v_min, float v_max, const char* fmt = "%.1f");
    bool SliderInt(const char* label, int* v, int v_min, int v_max);
    bool Combo(const char* label, int* current, const char* const items[], int count);
    bool KeybindButton(const char* label, int* vk, int& waitingState);
}

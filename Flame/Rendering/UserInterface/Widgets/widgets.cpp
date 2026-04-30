#define IMGUI_DEFINE_MATH_OPERATORS
#include "widgets.hpp"

#include "../../../Framework/ImGui/imgui_internal.h"

#include <windows.h>
#include <cstdio>
#include <cmath>
#include <string>

static const char* StripID(const char* label) {
    const char* p = strstr(label, "##");
    return p ? p : label + strlen(label);
}

static std::string GetDisplayLabel(const char* label) {
    const char* end = strstr(label, "##");
    if (end) return std::string(label, end);
    return std::string(label);
}

static const ImU32 COL_BG           = IM_COL32(28, 28, 36, 255);
static const ImU32 COL_BG_HOVER     = IM_COL32(38, 38, 50, 255);
static const ImU32 COL_ACCENT       = IM_COL32(120, 90, 255, 255);
static const ImU32 COL_ACCENT_DIM   = IM_COL32(80, 60, 180, 255);
static const ImU32 COL_TEXT         = IM_COL32(220, 220, 230, 255);
static const ImU32 COL_TEXT_DIM     = IM_COL32(120, 120, 140, 255);
static const ImU32 COL_TOGGLE_OFF   = IM_COL32(50, 50, 65, 255);
static const ImU32 COL_FIELD_BG     = IM_COL32(22, 22, 30, 255);
static const ImU32 COL_BORDER       = IM_COL32(55, 50, 75, 180);

static float AnimLerp(float a, float b, float t) {
    if (t < 0.0f) t = 0.0f;
    if (t > 1.0f) t = 1.0f;
    return a + (b - a) * t;
}

static ImGuiStorage s_animStorage;

static float GetAnim(ImGuiID id, float target, float speed = 8.0f) {
    float cur = s_animStorage.GetFloat(id, target);
    float dt = ImGui::GetIO().DeltaTime;
    cur = AnimLerp(cur, target, dt * speed);
    if (fabsf(cur - target) < 0.002f) cur = target;
    s_animStorage.SetFloat(id, cur);
    return cur;
}

bool FlameUI::Checkbox(const char* label, bool* v)
{
    ImGuiWindow* window = ImGui::GetCurrentWindow();
    if (window->SkipItems) return false;

    ImGuiID id = window->GetID(label);
    std::string displayLabel = GetDisplayLabel(label);

    float height = 20.0f;
    float width  = 36.0f;
    float radius = height * 0.5f;
    float spacing = 10.0f;

    ImVec2 pos = window->DC.CursorPos;
    ImVec2 labelSize = ImGui::CalcTextSize(displayLabel.c_str(), nullptr, true);
    float totalH = height > labelSize.y ? height : labelSize.y;

    ImRect totalBB(pos, ImVec2(pos.x + width + spacing + labelSize.x, pos.y + totalH + 6.0f));
    ImGui::ItemSize(totalBB, 0.0f);
    if (!ImGui::ItemAdd(totalBB, id)) return false;

    bool hovered, held;
    bool pressed = ImGui::ButtonBehavior(totalBB, id, &hovered, &held);
    if (pressed) *v = !*v;

    float animT = GetAnim(id, *v ? 1.0f : 0.0f, 12.0f);
    float hoverT = GetAnim(id + 50000, hovered ? 1.0f : 0.0f, 10.0f);

    ImDrawList* dl = window->DrawList;

    float cy = pos.y + (totalH - height) * 0.5f;
    ImVec2 toggleMin(pos.x, cy);
    ImVec2 toggleMax(pos.x + width, cy + height);

    ImU32 trackCol = ImGui::GetColorU32(ImVec4(
        AnimLerp(50.0f/255, 120.0f/255, animT),
        AnimLerp(50.0f/255,  90.0f/255, animT),
        AnimLerp(65.0f/255, 255.0f/255, animT),
        1.0f
    ));
    dl->AddRectFilled(toggleMin, toggleMax, trackCol, radius);

    if (animT > 0.01f) {
        dl->AddRectFilled(
            ImVec2(toggleMin.x - 2, toggleMin.y - 2),
            ImVec2(toggleMax.x + 2, toggleMax.y + 2),
            IM_COL32(120, 90, 255, (int)(30 * animT)), radius + 2
        );
    }

    float knobX = AnimLerp(toggleMin.x + radius, toggleMax.x - radius, animT);
    float knobY = cy + radius;
    float knobR = radius - 3.0f;
    dl->AddCircleFilled(ImVec2(knobX, knobY), knobR + 0.5f, IM_COL32(0, 0, 0, 40));
    dl->AddCircleFilled(ImVec2(knobX, knobY), knobR, IM_COL32(245, 245, 255, 255));

    ImU32 textCol = ImGui::GetColorU32(ImVec4(
        AnimLerp(180.0f/255, 240.0f/255, animT),
        AnimLerp(180.0f/255, 240.0f/255, animT),
        AnimLerp(195.0f/255, 250.0f/255, animT),
        1.0f
    ));
    dl->AddText(ImVec2(toggleMax.x + spacing, pos.y + (totalH - labelSize.y) * 0.5f), textCol, displayLabel.c_str());

    return pressed;
}

static bool FlameSliderBase(const char* label, float* v, float v_min, float v_max,
                            const char* valueTxt)
{
    ImGuiWindow* window = ImGui::GetCurrentWindow();
    if (window->SkipItems) return false;

    ImGuiID id = window->GetID(label);
    std::string displayLabel = GetDisplayLabel(label);

    float sliderW = ImGui::CalcItemWidth();
    float trackH  = 4.0f;
    float grabR   = 6.0f;
    float padY    = grabR + 4.0f;

    ImVec2 labelSize = ImGui::CalcTextSize(displayLabel.c_str(), nullptr, true);
    float totalH = labelSize.y + 6.0f + padY * 2;

    ImVec2 pos = window->DC.CursorPos;
    ImRect frameBB(pos, ImVec2(pos.x + sliderW, pos.y + totalH));
    ImRect totalBB(pos, ImVec2(pos.x + sliderW, pos.y + totalH + 4.0f));

    ImGui::ItemSize(totalBB, 0.0f);
    if (!ImGui::ItemAdd(totalBB, id)) return false;

    bool hovered, held;
    ImGui::ButtonBehavior(frameBB, id, &hovered, &held, ImGuiButtonFlags_None);

    float trackY  = pos.y + labelSize.y + 6.0f + padY - trackH * 0.5f;
    float trackX0 = pos.x + grabR;
    float trackX1 = pos.x + sliderW - grabR;
    float trackW  = trackX1 - trackX0;

    float t = 0.0f;
    if (v_max > v_min) {
        t = (*v - v_min) / (v_max - v_min);
        if (t < 0.0f) t = 0.0f;
        if (t > 1.0f) t = 1.0f;
    }

    if (held) {
        float mouseX = ImGui::GetIO().MousePos.x;
        t = (mouseX - trackX0) / trackW;
        if (t < 0.0f) t = 0.0f;
        if (t > 1.0f) t = 1.0f;
        *v = v_min + t * (v_max - v_min);
    }

    float animT = GetAnim(id + 80000, t, 14.0f);

    ImDrawList* dl = window->DrawList;

    ImVec2 valueSize = ImGui::CalcTextSize(valueTxt);
    dl->AddText(ImVec2(pos.x, pos.y), COL_TEXT, displayLabel.c_str());
    dl->AddText(ImVec2(pos.x + sliderW - valueSize.x, pos.y), COL_ACCENT, valueTxt);

    dl->AddRectFilled(
        ImVec2(trackX0, trackY), ImVec2(trackX1, trackY + trackH),
        COL_TOGGLE_OFF, trackH * 0.5f
    );

    float fillX = trackX0 + trackW * animT;
    if (fillX > trackX0 + 1.0f) {
        dl->AddRectFilled(
            ImVec2(trackX0, trackY), ImVec2(fillX, trackY + trackH),
            COL_ACCENT, trackH * 0.5f
        );
    }

    float grabX = fillX;
    float grabY = trackY + trackH * 0.5f;
    if (hovered || held) {
        dl->AddCircleFilled(ImVec2(grabX, grabY), grabR + 3, IM_COL32(120, 90, 255, 35));
    }
    dl->AddCircleFilled(ImVec2(grabX, grabY), grabR, COL_ACCENT);
    dl->AddCircleFilled(ImVec2(grabX, grabY), grabR - 2.0f, IM_COL32(245, 245, 255, 255));

    return held;
}

bool FlameUI::SliderFloat(const char* label, float* v, float v_min, float v_max, const char* fmt) {
    char buf[32];
    snprintf(buf, sizeof(buf), fmt, *v);
    return FlameSliderBase(label, v, v_min, v_max, buf);
}

bool FlameUI::SliderInt(const char* label, int* v, int v_min, int v_max) {
    char buf[32];
    snprintf(buf, sizeof(buf), "%d", *v);
    float fv = (float)*v;
    bool changed = FlameSliderBase(label, &fv, (float)v_min, (float)v_max, buf);
    *v = (int)roundf(fv);
    return changed;
}

static ImGuiID s_openCombo = 0;
static int     s_comboScroll = 0;

bool FlameUI::Combo(const char* label, int* current, const char* const items[], int count)
{
    ImGuiWindow* window = ImGui::GetCurrentWindow();
    if (window->SkipItems) return false;

    ImGuiID id = window->GetID(label);
    std::string displayLabel = GetDisplayLabel(label);

    float width  = ImGui::CalcItemWidth();
    float height = 28.0f;

    ImVec2 pos = window->DC.CursorPos;
    ImRect frameBB(pos, ImVec2(pos.x + width, pos.y + height));
    ImRect totalBB(pos, ImVec2(pos.x + width, pos.y + height + 4.0f));

    ImGui::ItemSize(totalBB, 0.0f);
    if (!ImGui::ItemAdd(totalBB, id)) return false;

    bool hovered, held;
    bool pressed = ImGui::ButtonBehavior(frameBB, id, &hovered, &held);
    if (pressed) {
        s_openCombo = (s_openCombo == id) ? 0 : id;
    }

    float hoverT = GetAnim(id + 60000, hovered ? 1.0f : 0.0f, 10.0f);

    ImDrawList* dl = window->DrawList;

    ImU32 bgCol = ImGui::GetColorU32(ImVec4(
        AnimLerp(22.0f/255, 35.0f/255, hoverT),
        AnimLerp(22.0f/255, 32.0f/255, hoverT),
        AnimLerp(30.0f/255, 48.0f/255, hoverT),
        1.0f
    ));
    dl->AddRectFilled(frameBB.Min, frameBB.Max, bgCol, 5.0f);
    dl->AddRect(frameBB.Min, frameBB.Max, COL_BORDER, 5.0f);

    ImVec2 labelSize = ImGui::CalcTextSize(displayLabel.c_str());
    dl->AddText(ImVec2(pos.x + 10, pos.y + (height - labelSize.y) * 0.5f), COL_TEXT_DIM, displayLabel.c_str());

    const char* preview = (*current >= 0 && *current < count) ? items[*current] : "---";
    ImVec2 previewSize = ImGui::CalcTextSize(preview);
    dl->AddText(ImVec2(frameBB.Max.x - previewSize.x - 26, pos.y + (height - previewSize.y) * 0.5f), COL_TEXT, preview);

    float ax = frameBB.Max.x - 14;
    float ay = pos.y + height * 0.5f;
    bool isOpen = (s_openCombo == id);
    if (isOpen) {
        dl->AddTriangleFilled(ImVec2(ax - 4, ay + 2), ImVec2(ax + 4, ay + 2), ImVec2(ax, ay - 3), COL_ACCENT);
    } else {
        dl->AddTriangleFilled(ImVec2(ax - 4, ay - 2), ImVec2(ax + 4, ay - 2), ImVec2(ax, ay + 3), COL_TEXT_DIM);
    }

    bool changed = false;

    if (isOpen) {
        float itemH = 26.0f;
        int maxVisible = 6;
        int visCount = count < maxVisible ? count : maxVisible;
        float popupH = itemH * visCount;
        ImVec2 popupMin(pos.x, frameBB.Max.y + 3);
        ImVec2 popupMax(pos.x + width, popupMin.y + popupH);

        ImDrawList* fgDl = ImGui::GetForegroundDrawList();

        fgDl->AddRectFilled(
            ImVec2(popupMin.x + 3, popupMin.y + 3),
            ImVec2(popupMax.x + 3, popupMax.y + 3),
            IM_COL32(0, 0, 0, 100), 5.0f
        );

        fgDl->AddRectFilled(popupMin, popupMax, IM_COL32(18, 18, 26, 250), 5.0f);
        fgDl->AddRect(popupMin, popupMax, COL_BORDER, 5.0f);

        ImVec2 mousePos = ImGui::GetIO().MousePos;

        for (int i = 0; i < visCount && i < count; i++) {
            ImVec2 itemMin(popupMin.x + 2, popupMin.y + i * itemH);
            ImVec2 itemMax(popupMax.x - 2, itemMin.y + itemH);

            bool itemHovered = (mousePos.x >= itemMin.x && mousePos.x <= itemMax.x &&
                                mousePos.y >= itemMin.y && mousePos.y <= itemMax.y);

            if (itemHovered) {
                fgDl->AddRectFilled(
                    ImVec2(itemMin.x + 1, itemMin.y + 1),
                    ImVec2(itemMax.x - 1, itemMax.y - 1),
                    IM_COL32(55, 48, 90, 200), 3.0f
                );
            }

            if (i == *current) {
                fgDl->AddRectFilled(
                    ImVec2(itemMin.x + 2, itemMin.y + 3),
                    ImVec2(itemMin.x + 5, itemMax.y - 3),
                    COL_ACCENT, 2.0f
                );
            }

            ImVec2 txtSize = ImGui::CalcTextSize(items[i]);
            fgDl->AddText(
                ImVec2(itemMin.x + 12, itemMin.y + (itemH - txtSize.y) * 0.5f),
                (i == *current) ? COL_ACCENT : COL_TEXT, items[i]
            );

            if (itemHovered && ImGui::IsMouseClicked(0)) {
                *current = i;
                s_openCombo = 0;
                changed = true;
            }
        }

        if (ImGui::IsMouseClicked(0) && !changed) {
            bool inFrame = (mousePos.x >= frameBB.Min.x && mousePos.x <= frameBB.Max.x &&
                            mousePos.y >= frameBB.Min.y && mousePos.y <= frameBB.Max.y);
            bool inPopup = (mousePos.x >= popupMin.x && mousePos.x <= popupMax.x &&
                            mousePos.y >= popupMin.y && mousePos.y <= popupMax.y);
            if (!inFrame && !inPopup) {
                s_openCombo = 0;
            }
        }
    }

    return changed;
}

bool FlameUI::KeybindButton(const char* label, int* vk, int& waitingState)
{
    ImGuiWindow* window = ImGui::GetCurrentWindow();
    if (window->SkipItems) return false;

    ImGuiID id = window->GetID(label);
    std::string displayLabel = GetDisplayLabel(label);

    float height = 24.0f;
    float btnW   = 90.0f;

    bool isWaiting = (waitingState == *vk && waitingState >= 0);

    char keyName[64] = "None";
    if (*vk > 0) {
        UINT scanCode = MapVirtualKeyA(*vk, MAPVK_VK_TO_VSC);
        GetKeyNameTextA(scanCode << 16, keyName, sizeof(keyName));
    }

    const char* btnText = isWaiting ? "Press key..." : keyName;

    ImVec2 pos = window->DC.CursorPos;
    ImVec2 labelSize = ImGui::CalcTextSize(displayLabel.c_str(), nullptr, true);
    ImVec2 btnTextSize = ImGui::CalcTextSize(btnText);

    ImRect frameBB(pos, ImVec2(pos.x + btnW, pos.y + height));
    ImRect totalBB(pos, ImVec2(pos.x + btnW + 10.0f + labelSize.x, pos.y + height + 4.0f));

    ImGui::ItemSize(totalBB, 0.0f);
    if (!ImGui::ItemAdd(totalBB, id)) return false;

    bool hovered, held;
    bool pressed = ImGui::ButtonBehavior(frameBB, id, &hovered, &held);

    if (pressed && !isWaiting) {
        waitingState = *vk;
    }

    float hoverT = GetAnim(id + 70000, (hovered || isWaiting) ? 1.0f : 0.0f, 10.0f);

    ImDrawList* dl = window->DrawList;
    ImU32 bgCol = isWaiting ? COL_ACCENT_DIM : ImGui::GetColorU32(ImVec4(
        AnimLerp(22.0f/255, 35.0f/255, hoverT),
        AnimLerp(22.0f/255, 32.0f/255, hoverT),
        AnimLerp(30.0f/255, 48.0f/255, hoverT),
        1.0f
    ));
    dl->AddRectFilled(frameBB.Min, frameBB.Max, bgCol, 5.0f);
    dl->AddRect(frameBB.Min, frameBB.Max, COL_BORDER, 5.0f);

    dl->AddText(
        ImVec2(pos.x + (btnW - btnTextSize.x) * 0.5f, pos.y + (height - btnTextSize.y) * 0.5f),
        isWaiting ? IM_COL32(255, 120, 120, 255) : COL_TEXT, btnText
    );

    dl->AddText(ImVec2(frameBB.Max.x + 10.0f, pos.y + (height - labelSize.y) * 0.5f), COL_TEXT_DIM, displayLabel.c_str());

    bool changed = false;
    if (isWaiting) {
        for (int k = 1; k < 256; k++) {
            if (k == VK_INSERT || k == VK_ESCAPE || k == VK_LBUTTON || k == VK_RBUTTON) continue;
            if (GetAsyncKeyState(k) & 1) {
                *vk = k;
                waitingState = -1;
                changed = true;
                break;
            }
        }
    }

    return changed;
}

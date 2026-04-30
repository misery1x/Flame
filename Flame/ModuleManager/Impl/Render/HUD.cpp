#include "HUD.hpp"

#include "../../Framework/ImGui/imgui.h"
#include "../../Rendering/Overlay/overlay.hpp"

void HUD::onRender() {
    if (Overlay::bShowMenu) return;

    bool  showWatermark = getBool("Watermark");
    float wmScale       = getFloat("Watermark Scale");

    if (!showWatermark) return;

    ImGuiIO& io = ImGui::GetIO();
    ImDrawList* draw = ImGui::GetBackgroundDrawList();

    ImFont* font = ImGui::GetFont();
    float origSize = ImGui::GetFontSize();
    float scaledSize = origSize * wmScale;

    const char* flame = "Flame";
    const char* rest = " | Badlion 1.8.9";

    ImVec2 flameSize = font->CalcTextSizeA(scaledSize, FLT_MAX, 0, flame);
    ImVec2 restSize = font->CalcTextSizeA(scaledSize, FLT_MAX, 0, rest);
    float totalW = flameSize.x + restSize.x;
    float totalH = flameSize.y;

    float pad = 8.0f;
    float x = pad;
    float y = pad;

    ImVec2 bgMin(x - 8, y - 4);
    ImVec2 bgMax(x + totalW + 8, y + totalH + 4);

    draw->AddRectFilled(bgMin, bgMax, IM_COL32(12, 12, 18, 220), 4.0f);
    draw->AddRectFilled(ImVec2(bgMin.x, bgMin.y), ImVec2(bgMin.x + 2, bgMax.y), IM_COL32(120, 90, 255, 255));

    draw->AddText(font, scaledSize, ImVec2(x, y), IM_COL32(120, 90, 255, 255), flame);
    draw->AddText(font, scaledSize, ImVec2(x + flameSize.x, y), IM_COL32(140, 140, 150, 200), rest);
}

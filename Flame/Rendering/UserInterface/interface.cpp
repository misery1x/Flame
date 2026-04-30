#include "interface.hpp"
#include "../Overlay/overlay.hpp"
#include "Widgets/widgets.hpp"

#include "../../Framework/ImGui/imgui.h"
#include "../../Framework/ImGui/imgui_internal.h"
#include "../../ModuleManager/ModuleManager.hpp"

#include <algorithm>
#include <set>
#include <string>
#include <vector>

static int s_waitingKeybind = -1;
static std::string s_activeCategory = "";

namespace UserInterface
{
    static void RenderSetting(Setting& setting, const std::string& moduleId)
    {
        std::string id = setting.name + "##" + moduleId;

        switch (setting.type)
        {
            case Setting::BOOL:
                FlameUI::Checkbox(id.c_str(), &setting.bValue);
                break;

            case Setting::FLOAT:
                FlameUI::SliderFloat(id.c_str(), &setting.fValue, setting.fMin, setting.fMax, "%.1f");
                break;

            case Setting::INT:
                if (setting.name == "Keybind") {
                    FlameUI::KeybindButton(id.c_str(), &setting.iValue, s_waitingKeybind);
                } else {
                    FlameUI::SliderInt(id.c_str(), &setting.iValue, setting.iMin, setting.iMax);
                }
                break;

            case Setting::DOUBLE:
            {
                float tmp = (float)setting.dValue;
                if (FlameUI::SliderFloat(id.c_str(), &tmp, (float)setting.dMin, (float)setting.dMax, "%.2f"))
                    setting.dValue = (double)tmp;
                break;
            }

            case Setting::COMBO:
            {
                if (!setting.comboOptions.empty())
                {
                    std::vector<const char*> items;
                    for (auto& s : setting.comboOptions) items.push_back(s.c_str());
                    FlameUI::Combo(id.c_str(), &setting.iValue, items.data(), (int)items.size());
                }
                break;
            }
        }
    }

    static void RenderModuleList(const std::string& category)
    {
        auto modules = ModuleManager::get().getModulesByCategory(category);

        if (modules.empty()) {
            ImGui::Spacing();
            ImGui::TextColored(ImVec4(0.4f, 0.4f, 0.5f, 0.6f), "  No modules in this category");
            return;
        }

        for (auto* mod : modules)
        {
            std::string checkId = mod->name + "##toggle_" + mod->name;
            FlameUI::Checkbox(checkId.c_str(), &mod->enabled);

            bool hasKeybind = false;
            for (auto& setting : mod->getSettings())
                if (setting.name == "Keybind") { hasKeybind = true; break; }

            if (hasKeybind && !mod->enabled)
            {
                ImGui::Indent(20.0f);
                for (auto& setting : mod->getSettings())
                {
                    if (setting.name == "Keybind")
                        RenderSetting(setting, mod->name);
                }
                ImGui::Unindent(20.0f);
            }
            else if (mod->enabled && !mod->getSettings().empty())
            {
                ImGui::Indent(20.0f);
                ImGui::Spacing();
                for (auto& setting : mod->getSettings())
                {
                    RenderSetting(setting, mod->name);
                }
                ImGui::Spacing();
                ImGui::Unindent(20.0f);
            }

            ImGui::Spacing();
        }
    }

    void Render()
    {
        if (!Overlay::bShowMenu)
            return;

        ImGuiIO& io = ImGui::GetIO();

        ImGui::PushStyleColor(ImGuiCol_WindowBg,       ImVec4(0.055f, 0.055f, 0.075f, 0.97f));
        ImGui::PushStyleColor(ImGuiCol_Border,          ImVec4(0.22f, 0.18f, 0.35f, 0.50f));
        ImGui::PushStyleColor(ImGuiCol_ChildBg,         ImVec4(0.045f, 0.045f, 0.065f, 1.00f));
        ImGui::PushStyleColor(ImGuiCol_ScrollbarBg,     ImVec4(0.04f, 0.04f, 0.06f, 0.50f));
        ImGui::PushStyleColor(ImGuiCol_ScrollbarGrab,   ImVec4(0.30f, 0.25f, 0.50f, 0.50f));
        ImGui::PushStyleVar(ImGuiStyleVar_WindowRounding, 8.0f);
        ImGui::PushStyleVar(ImGuiStyleVar_WindowBorderSize, 1.0f);
        ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, ImVec2(0, 0));
        ImGui::PushStyleVar(ImGuiStyleVar_ItemSpacing, ImVec2(8, 4));

        ImGui::SetNextWindowSize(ImVec2(640, 480), ImGuiCond_FirstUseEver);
        ImGui::SetNextWindowPos(ImVec2(io.DisplaySize.x * 0.5f, io.DisplaySize.y * 0.5f),ImGuiCond_FirstUseEver, ImVec2(0.5f, 0.5f));

        ImGui::Begin("##FlameMain", &Overlay::bShowMenu,ImGuiWindowFlags_NoCollapse | ImGuiWindowFlags_NoScrollbar |ImGuiWindowFlags_NoTitleBar | ImGuiWindowFlags_NoResize);

        ImDrawList* dl = ImGui::GetWindowDrawList();
        ImVec2 wp = ImGui::GetWindowPos();
        ImVec2 ws = ImGui::GetWindowSize();

        float topH = 48.0f;
        dl->AddRectFilled(wp, ImVec2(wp.x + ws.x, wp.y + topH), IM_COL32(16, 16, 22, 255), 8.0f, ImDrawFlags_RoundCornersTop);
        dl->AddRectFilled(ImVec2(wp.x, wp.y + topH - 2), ImVec2(wp.x + ws.x, wp.y + topH), IM_COL32(120, 90, 255, 100));

        ImFont* font = ImGui::GetFont();
        float fontSize = ImGui::GetFontSize();
        dl->AddText(font, fontSize * 1.3f, ImVec2(wp.x + 18, wp.y + 12), IM_COL32(120, 90, 255, 255), "FLAME");

        ImVec2 vSize = font->CalcTextSizeA(fontSize * 0.85f, FLT_MAX, 0, "v1.0 | Badlion 1.8.9");
        dl->AddText(font, fontSize * 0.85f, ImVec2(wp.x + ws.x - vSize.x - 18, wp.y + 18), IM_COL32(100, 100, 120, 180), "v1.0 | Badlion 1.8.9");

        float sideW = 130.0f;
        dl->AddRectFilled(ImVec2(wp.x, wp.y + topH), ImVec2(wp.x + sideW, wp.y + ws.y), IM_COL32(18, 18, 25, 255), 0.0f);
        dl->AddLine(ImVec2(wp.x + sideW, wp.y + topH), ImVec2(wp.x + sideW, wp.y + ws.y), IM_COL32(55, 50, 75, 120));

        auto& allModules = ModuleManager::get().getModules();
        std::vector<std::string> categories;
        {
            std::set<std::string> catSet;
            for (auto& mod : allModules)
            {
                if (catSet.insert(mod->category).second)
                    categories.push_back(mod->category);
            }
        }

        if (s_activeCategory.empty() && !categories.empty())
            s_activeCategory = categories[0];

        float catY = wp.y + topH + 12;
        float catBtnH = 32.0f;
        ImVec2 mousePos = io.MousePos;

        for (auto& cat : categories)
        {
            ImVec2 btnMin(wp.x + 6, catY);
            ImVec2 btnMax(wp.x + sideW - 6, catY + catBtnH);

            bool isActive = (cat == s_activeCategory);
            bool catHovered = (mousePos.x >= btnMin.x && mousePos.x <= btnMax.x &&mousePos.y >= btnMin.y && mousePos.y <= btnMax.y);

            if (isActive)
            {
                dl->AddRectFilled(btnMin, btnMax, IM_COL32(120, 90, 255, 30), 5.0f);
                dl->AddRectFilled(ImVec2(btnMin.x, btnMin.y + 4), ImVec2(btnMin.x + 3, btnMax.y - 4), IM_COL32(120, 90, 255, 255), 2.0f);
            }
            else if (catHovered)
            {
                dl->AddRectFilled(btnMin, btnMax, IM_COL32(40, 38, 55, 150), 5.0f);
            }

            ImVec2 txtSize = font->CalcTextSizeA(fontSize, FLT_MAX, 0, cat.c_str());
            dl->AddText(ImVec2(btnMin.x + 14, btnMin.y + (catBtnH - txtSize.y) * 0.5f),
                isActive ? IM_COL32(120, 90, 255, 255) : (catHovered ? IM_COL32(200, 200, 210, 255) : IM_COL32(140, 140, 160, 200)),
                cat.c_str());

            if (catHovered && ImGui::IsMouseClicked(0))
            {
                s_activeCategory = cat;
            }

            catY += catBtnH + 4;
        }

        ImGui::SetCursorPos(ImVec2(sideW + 12, topH + 10));

        ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, ImVec2(12, 8));
        ImGui::PushStyleVar(ImGuiStyleVar_ItemSpacing, ImVec2(8, 3));
        ImGui::PushItemWidth(ws.x - sideW - 50);

        ImGui::BeginChild("##content", ImVec2(ws.x - sideW - 16, ws.y - topH - 16), false,
            ImGuiWindowFlags_NoBackground);

        if (!s_activeCategory.empty())
        {
            ImGui::Spacing();
            ImGui::TextColored(ImVec4(0.47f, 0.35f, 1.0f, 0.8f), "%s", s_activeCategory.c_str());
            ImGui::Spacing();

            ImVec2 sepPos = ImGui::GetCursorScreenPos();
            dl->AddRectFilled(sepPos, ImVec2(sepPos.x + ws.x - sideW - 40, sepPos.y + 1), IM_COL32(55, 50, 75, 80));
            ImGui::Spacing();
            ImGui::Spacing();

            RenderModuleList(s_activeCategory);
        }

        ImGui::EndChild();
        ImGui::PopItemWidth();
        ImGui::PopStyleVar(2);

        ImGui::End();

        ImGui::PopStyleVar(4);
        ImGui::PopStyleColor(5);
    }
}

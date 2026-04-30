#include "overlay.hpp"

#include "../../Framework/ImGui/imgui.h"
#include "../../Framework/ImGui/imgui_impl_win32.h"
#include "../../Framework/ImGui/imgui_impl_opengl3.h"

#include <cstdio>

namespace Overlay
{
    bool Initialize(HWND hWnd)
    {
        IMGUI_CHECKVERSION();
        ImGui::CreateContext();

        ImGuiIO& io = ImGui::GetIO();
        io.ConfigFlags |= ImGuiConfigFlags_NavEnableKeyboard;
        io.IniFilename = nullptr;

        ImFont* interFont = io.Fonts->AddFontFromFileTTF("C:\\Flame\\Fonts\\Helvetica.ttf", 16.0f);
        if (!interFont) {
            printf("[Flame] Inter font not found, using default\n");
            io.Fonts->AddFontDefault();
        }

        ImGui::StyleColorsDark();
        ImGuiStyle& style = ImGui::GetStyle();

        style.WindowRounding    = 8.0f;
        style.FrameRounding     = 4.0f;
        style.GrabRounding      = 4.0f;
        style.TabRounding       = 4.0f;
        style.ScrollbarRounding = 4.0f;
        style.ChildRounding     = 4.0f;
        style.PopupRounding     = 4.0f;

        style.WindowPadding  = ImVec2(12, 12);
        style.FramePadding   = ImVec2(8, 4);
        style.ItemSpacing    = ImVec2(8, 6);
        style.WindowBorderSize = 0.0f;

        ImVec4* colors = style.Colors;
        colors[ImGuiCol_WindowBg]           = ImVec4(0.08f, 0.08f, 0.12f, 0.94f);
        colors[ImGuiCol_ChildBg]            = ImVec4(0.10f, 0.10f, 0.15f, 1.00f);
        colors[ImGuiCol_Border]             = ImVec4(0.30f, 0.25f, 0.50f, 0.50f);
        colors[ImGuiCol_FrameBg]            = ImVec4(0.14f, 0.13f, 0.20f, 1.00f);
        colors[ImGuiCol_FrameBgHovered]     = ImVec4(0.22f, 0.20f, 0.35f, 1.00f);
        colors[ImGuiCol_FrameBgActive]      = ImVec4(0.30f, 0.25f, 0.45f, 1.00f);
        colors[ImGuiCol_TitleBg]            = ImVec4(0.06f, 0.06f, 0.09f, 1.00f);
        colors[ImGuiCol_TitleBgActive]      = ImVec4(0.12f, 0.10f, 0.22f, 1.00f);
        colors[ImGuiCol_TitleBgCollapsed]   = ImVec4(0.06f, 0.06f, 0.09f, 0.50f);
        colors[ImGuiCol_Tab]                = ImVec4(0.14f, 0.13f, 0.22f, 1.00f);
        colors[ImGuiCol_TabHovered]         = ImVec4(0.40f, 0.30f, 0.70f, 0.80f);
        colors[ImGuiCol_TabActive]          = ImVec4(0.30f, 0.22f, 0.58f, 1.00f);
        colors[ImGuiCol_Button]             = ImVec4(0.20f, 0.16f, 0.36f, 1.00f);
        colors[ImGuiCol_ButtonHovered]      = ImVec4(0.35f, 0.28f, 0.60f, 1.00f);
        colors[ImGuiCol_ButtonActive]       = ImVec4(0.45f, 0.35f, 0.75f, 1.00f);
        colors[ImGuiCol_CheckMark]          = ImVec4(0.55f, 0.40f, 1.00f, 1.00f);
        colors[ImGuiCol_SliderGrab]         = ImVec4(0.45f, 0.35f, 0.80f, 1.00f);
        colors[ImGuiCol_SliderGrabActive]   = ImVec4(0.55f, 0.42f, 0.95f, 1.00f);
        colors[ImGuiCol_Header]             = ImVec4(0.20f, 0.16f, 0.36f, 1.00f);
        colors[ImGuiCol_HeaderHovered]      = ImVec4(0.30f, 0.24f, 0.55f, 1.00f);
        colors[ImGuiCol_HeaderActive]       = ImVec4(0.38f, 0.30f, 0.65f, 1.00f);
        colors[ImGuiCol_Separator]          = ImVec4(0.25f, 0.20f, 0.45f, 0.50f);
        colors[ImGuiCol_Text]              = ImVec4(0.90f, 0.90f, 0.95f, 1.00f);
        colors[ImGuiCol_TextDisabled]      = ImVec4(0.45f, 0.45f, 0.55f, 1.00f);

        ImGui_ImplWin32_Init(hWnd);
        ImGui_ImplOpenGL3_Init("#version 120");

        return true;
    }

    void BeginFrame()
    {
        ImGui_ImplOpenGL3_NewFrame();
        ImGui_ImplWin32_NewFrame();
        ImGui::NewFrame();
    }

    void EndFrame()
    {
        ImGui::EndFrame();
        ImGui::Render();
        ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());
    }

    void Shutdown()
    {
        ImGui_ImplOpenGL3_Shutdown();
        ImGui_ImplWin32_Shutdown();
        ImGui::DestroyContext();

        printf("[Flame] ImGui overlay shutdown\n");
    }
}

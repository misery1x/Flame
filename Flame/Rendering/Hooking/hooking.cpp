#include "hooking.hpp"
#include "../Overlay/overlay.hpp"
#include "../UserInterface/interface.hpp"
#include "../RenderUtils.hpp"
#include "../../Framework/Minhook/MinHook.h"
#include "../../SDK/sdk.hpp"
#include "../../ModuleManager/ModuleManager.hpp"
#include "../../KeybindHandler/KeybindHandler.hpp"

#include "../../Framework/ImGui/imgui.h"
#include "../../Framework/ImGui/imgui_impl_win32.h"
#include "../../Framework/ImGui/imgui_impl_opengl3.h"

#include <cstdio>

extern IMGUI_IMPL_API LRESULT ImGui_ImplWin32_WndProcHandler(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam);

namespace Hooking
{
    bool Initialize()
    {
        if (MH_Initialize() != MH_OK)
        {
            printf("[Flame] Failed to initialize MinHook\n");
            return false;
        }

        HMODULE hOpenGL = GetModuleHandleA("opengl32.dll");
        if (!hOpenGL)
        {
            printf("[Flame] Failed to find opengl32.dll\n");
            return false;
        }

        void* pWglSwapBuffers = GetProcAddress(hOpenGL, "wglSwapBuffers");
        if (!pWglSwapBuffers)
        {
            printf("[Flame] Failed to find wglSwapBuffers\n");
            return false;
        }

        if (MH_CreateHook(pWglSwapBuffers, &hkWglSwapBuffers, reinterpret_cast<void**>(&oWglSwapBuffers)) != MH_OK)
        {
            printf("[Flame] Failed to create wglSwapBuffers hook\n");
            return false;
        }

        if (MH_EnableHook(pWglSwapBuffers) != MH_OK)
        {
            printf("[Flame] Failed to enable wglSwapBuffers hook\n");
            return false;
        }

        return true;
    }

    void Shutdown()
    {
        MH_DisableHook(MH_ALL_HOOKS);
        MH_Uninitialize();

        if (hGameWindow && oWndProc)
        {
            SetWindowLongPtrA(hGameWindow, GWLP_WNDPROC, reinterpret_cast<LONG_PTR>(oWndProc));
            oWndProc = nullptr;
        }

        Overlay::Shutdown();

        printf("[Flame] Hooks removed\n");
    }

    BOOL WINAPI hkWglSwapBuffers(HDC hdc)
    {
        if (!bInitialized)
        {
            hGameDC     = hdc;
            hGameWindow = WindowFromDC(hdc);

            if (!hGameWindow)
                return oWglSwapBuffers(hdc);

            hGameContext = wglGetCurrentContext();

            if (!Overlay::Initialize(hGameWindow))
            {
                printf("[Flame] Failed to initialize overlay\n");
                return oWglSwapBuffers(hdc);
            }

            oWndProc = reinterpret_cast<WNDPROC>(SetWindowLongPtrA(hGameWindow, GWLP_WNDPROC, reinterpret_cast<LONG_PTR>(WndProc)));

            bInitialized = true;
        }

        RenderUtils::CaptureViewport();

        Overlay::BeginFrame();

        KeybindHandler::Poll();

        ModuleManager::get().onUpdate();

        ModuleManager::get().onRender();

        UserInterface::Render();

        Overlay::EndFrame();

        return oWglSwapBuffers(hdc);
    }

    LRESULT CALLBACK WndProc(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam)
    {
        if (msg == WM_KEYDOWN && wParam == VK_INSERT)
        {
            Overlay::bShowMenu = !Overlay::bShowMenu;
            return 0;
        }

        if (Overlay::bShowMenu)
        {
            if (ImGui_ImplWin32_WndProcHandler(hWnd, msg, wParam, lParam))
                return 0;

            switch (msg)
            {
            case WM_LBUTTONDOWN: case WM_LBUTTONUP:
            case WM_RBUTTONDOWN: case WM_RBUTTONUP:
            case WM_MBUTTONDOWN: case WM_MBUTTONUP:
            case WM_MOUSEWHEEL:
            case WM_MOUSEMOVE:
                return 0;
            }
        }

        return CallWindowProcA(oWndProc, hWnd, msg, wParam, lParam);
    }
}

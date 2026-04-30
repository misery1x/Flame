#pragma once

#include <Windows.h>
#include <gl/GL.h>

typedef BOOL(WINAPI* tWglSwapBuffers)(HDC hdc);

namespace Hooking
{

    inline tWglSwapBuffers oWglSwapBuffers = nullptr;

    inline bool  bInitialized = false;
    inline HWND  hGameWindow  = nullptr;
    inline HDC   hGameDC      = nullptr;
    inline HGLRC hGameContext  = nullptr;
    inline HGLRC hImGuiContext = nullptr;

    inline WNDPROC oWndProc = nullptr;

    bool Initialize();
    void Shutdown();

    BOOL WINAPI hkWglSwapBuffers(HDC hdc);

    LRESULT CALLBACK WndProc(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam);
}

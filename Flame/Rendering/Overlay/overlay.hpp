#pragma once

#include <Windows.h>

namespace Overlay
{
    inline bool bShowMenu = true;
    bool Initialize(HWND hWnd);
    void BeginFrame();
    void EndFrame();
    void Shutdown();
}


#include <Windows.h>
#include <cstdio>
#include <cstring>

#include "Rendering/Hooking/hooking.hpp"
#include "Framework/JNI/jni.h"
#include "SDK/sdk.hpp"
#include "ModuleManager/ModuleManager.hpp"

namespace Flame
{
    inline HMODULE hModule    = nullptr;
    inline JavaVM* pJavaVM    = nullptr;
    inline JNIEnv* pJNIEnv    = nullptr;
    inline bool    bRunning   = true;
}

namespace Flame
{
    bool AttachToJVM()
    {
        using tJNI_GetCreatedJavaVMs = jint(JNICALL*)(JavaVM**, jsize, jsize*);

        HMODULE hJVM = nullptr;

        while (!hJVM)
        {
            hJVM = GetModuleHandleA("jvm.dll");
            if (!hJVM)
                Sleep(200);
        }

        auto fnGetCreatedJavaVMs = reinterpret_cast<tJNI_GetCreatedJavaVMs>(
            GetProcAddress(hJVM, "JNI_GetCreatedJavaVMs")
        );

        if (!fnGetCreatedJavaVMs)
        {
            printf("[Flame] Failed to find JNI_GetCreatedJavaVMs\n");
            return false;
        }

        jsize vmCount = 0;
        if (fnGetCreatedJavaVMs(&pJavaVM, 1, &vmCount) != JNI_OK || vmCount == 0)
        {
            printf("[Flame] No JVM instances found\n");
            return false;
        }

        jint result = pJavaVM->AttachCurrentThread(reinterpret_cast<void**>(&pJNIEnv), nullptr);
        if (result != JNI_OK)
        {
            printf("[Flame] Failed to attach to JVM (error %d)\n", result);
            return false;
        }

        SDK::SetVM(pJavaVM);

        return true;
    }

    void DetachFromJVM()
    {
        if (pJavaVM)
        {
            pJavaVM->DetachCurrentThread();
            pJavaVM = nullptr;
            pJNIEnv = nullptr;
            printf("[Flame] Detached from JVM\n");
        }
    }
}

DWORD WINAPI MainThread(LPVOID lpParam)
{
    AllocConsole();
    FILE* fp = nullptr;
    freopen_s(&fp, "CONOUT$", "w", stdout);
    freopen_s(&fp, "CONIN$", "r", stdin);

    while (!GetModuleHandleA("opengl32.dll")) Sleep(200);

    if (!Flame::AttachToJVM())
    {
        printf("[Flame] JVM attachment failed. Ejecting...\n");
        FreeConsole();
        FreeLibraryAndExitThread(Flame::hModule, 0);
        return 0;
    }

    ModuleManager::get().init();

    if (!Hooking::Initialize())
    {
        printf("[Flame] Hook installation failed. Ejecting...\n");
        Flame::DetachFromJVM();
        FreeConsole();
        FreeLibraryAndExitThread(Flame::hModule, 0);
        return 0;
    }

    while (Flame::bRunning)
    {
        if (GetAsyncKeyState(VK_END) & 1)
        {
            Flame::bRunning = false;
        }
        Sleep(50);
    }

    Hooking::Shutdown();
    Flame::DetachFromJVM();

    Sleep(200);

    FreeConsole();
    FreeLibraryAndExitThread(Flame::hModule, 0);
    return 0;
}

BOOL APIENTRY DllMain(HMODULE hModule, DWORD dwReason, LPVOID lpReserved)
{
    if (dwReason == DLL_PROCESS_ATTACH)
    {
        DisableThreadLibraryCalls(hModule);

        Flame::hModule = hModule;

        HANDLE hThread = CreateThread(nullptr, 0, MainThread, nullptr, 0, nullptr);
        if (hThread)
            CloseHandle(hThread);
    }
    return TRUE;
}

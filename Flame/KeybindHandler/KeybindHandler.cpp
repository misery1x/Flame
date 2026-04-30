#include "KeybindHandler.hpp"

static bool s_keyStates[256] = {};

namespace KeybindHandler
{
    void Poll() {
        auto& modules = ModuleManager::get().getModules();

        for (auto& mod : modules) {
            int key = mod->keyBind;

            Setting* keybindSetting = mod->getSetting("Keybind");
            if (keybindSetting && keybindSetting->type == Setting::INT)
                key = keybindSetting->iValue;

            if (key <= 0 || key >= 256) continue;

            bool pressed = (GetAsyncKeyState(key) & 0x8000) != 0;

            if (pressed && !s_keyStates[key])
                mod->toggle();

            s_keyStates[key] = pressed;
        }
    }
}

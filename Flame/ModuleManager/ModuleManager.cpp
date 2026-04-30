#include "ModuleManager.hpp"
#include "Impl/Render/PlayerESP.hpp"
#include "Impl/Render/HUD.hpp"
#include "Impl/Combat/LeftClicker.hpp"
#include "Impl/Combat/RightClicker.hpp"

#include <cstdio>

void ModuleManager::init()
{
    m_modules.push_back(std::make_shared<LeftClicker>());
    m_modules.push_back(std::make_shared<RightClicker>());

    m_modules.push_back(std::make_shared<PlayerESP>());
    m_modules.push_back(std::make_shared<HUD>());

    printf("[Flame] ModuleManager initialized with %zu modules\n", m_modules.size());
}

void ModuleManager::onRender()
{
    for (auto& mod : m_modules)
        if (mod->isEnabled()) mod->onRender();
}

void ModuleManager::onUpdate()
{
    for (auto& mod : m_modules)
        if (mod->isEnabled()) mod->onUpdate();
}

Module* ModuleManager::getModule(const std::string& name)
{
    for (auto& mod : m_modules)
        if (mod->name == name) return mod.get();
    return nullptr;
}

std::vector<Module*> ModuleManager::getModulesByCategory(const std::string& category)
{
    std::vector<Module*> result;
    for (auto& mod : m_modules)
        if (mod->category == category) result.push_back(mod.get());
    return result;
}

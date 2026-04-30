#pragma once

#include "Module.hpp"
#include <memory>
#include <string>
#include <vector>

class ModuleManager {
public:
  static ModuleManager &get() {
    static ModuleManager instance;
    return instance;
  }

  void init();

  void onRender();

  void onUpdate();

  Module *getModule(const std::string &name);

  std::vector<std::shared_ptr<Module>> &getModules() { return m_modules; }

  std::vector<Module *> getModulesByCategory(const std::string &category);

private:
  ModuleManager() = default;
  std::vector<std::shared_ptr<Module>> m_modules;
};

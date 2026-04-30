# 🔥 Flame

A lightweight Badlion jni hacked client
Hooks into the JVM at runtime via DLL injection

---

## Features

| Category | Module | Description |
|----------|--------|-------------|
| **Combat** | LeftClicker | Automated left-click with randomized CPS and hit-select |
| **Combat** | RightClicker | Automated right-click |
| **Render** | PlayerESP | 2D bounding box overlay on players |
| **Render** | HUD | Minimal on-screen watermark |

## Architecture

```
Flame/          → Core DLL (injected into javaw.exe)
├── SDK/        → JNI wrappers for Minecraft 1.8.9 classes
├── Rendering/  → OpenGL hook, ImGui overlay, custom UI widgets
├── ModuleManager/  → Module system with per-module settings
└── Framework/  → ImGui, MinHook, JNI headers, OpenGL loader

Loader/         → Standalone injector with ImGui UI
```

## Build

- **Visual Studio 2022** (v143 toolset)
- **C++17**
- **x64** — targets 64-bit JVM

Open `Flame.sln`, build both projects. The DLL outputs to `Build/Flame.dll`.

## Usage

1. Launch Minecraft 1.8.9
2. Inject via process hacker
3. Press **INSERT** to toggle the menu
4. Press **END** to eject

## License

This project is provided for educational purposes only.

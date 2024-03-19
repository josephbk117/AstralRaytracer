#pragma once

#include "Compositor/Compositor.h"
#include "Raytracer/Camera.h"
#include "Raytracer/Renderer.h"
#include "Utils/AssetManager.h"
#include "Utils/Common.h"
#include "WindowFramework/Input.h"
#include "WindowFramework/Window.h"
#include "WindowFramework/WindowUtils.h"

class Application
{
  public:
    enum class LogLevel : uint8
    {
        Info,
        Warning,
        Error,
        None
    };

    enum class LogDisplay : uint8
    {
        None,
        Console,
        File,
        ConsoleAndFile
    };

    struct LaunchOptions
    {
        LogLevel logLevel = LogLevel::None;
        LogDisplay logDisplay = LogDisplay::None;
        std::filesystem::path projectPath;
    };

    Application() = default;
    AstralRaytracer::Errors::GenericError initialize(int32 argCount, char *argValues[]);
    void run();
    void shutdown();

  private:
    AstralRaytracer::AssetManager m_assetManager;
    AstralRaytracer::Window m_window{"Astral Raytracer"};
    AstralRaytracer::Renderer m_renderer;
    AstralRaytracer::Camera m_cam{15.0f, 15.0f, 10.0f, 0.0f};
    AstralRaytracer::Scene m_scene;
    AstralRaytracer::Compositor m_compositor;

    AstralRaytracer::Errors::GenericError parseCommandLineArgs(int32 argCount, char *argValues[], LaunchOptions& outLaunchOptions);
    void internalInitialize(LaunchOptions options);
};
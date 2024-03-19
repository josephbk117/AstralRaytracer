#include "Application.h"

#include <CLI/CLI.hpp>
#include <CLI/Validators.hpp>
#include <mimalloc.h>

namespace
{
spdlog::level::level_enum getSpdLogLevel(Application::LogLevel logLevel)
{
    switch (logLevel)
    {
    case Application::LogLevel::None:
        return spdlog::level::off;
    case Application::LogLevel::Error:
        return spdlog::level::err;
    case Application::LogLevel::Warning:
        return spdlog::level::warn;
    case Application::LogLevel::Info:
        return spdlog::level::trace;
    default:
        return spdlog::level::off;
    }
}
} // namespace

AstralRaytracer::Errors::GenericError Application::initialize(int32 argCount, char *argValues[])
{
    LaunchOptions options;

    if (parseCommandLineArgs(argCount, argValues, options) != AstralRaytracer::Errors::GenericError::SUCCESS)
    {
        return AstralRaytracer::Errors::GenericError::FAILURE;
    }

    internalInitialize(options);

    // Load project if provided as argument
    if (m_assetManager.loadProject(options.projectPath))
    {
        m_scene.deserialize(m_assetManager, m_assetManager.getDefaultSceneAbsolutePath());
        m_window.setWindowTitle(m_assetManager, m_scene);
    }

    return AstralRaytracer::Errors::GenericError::SUCCESS;
}

void Application::run()
{
    AstralRaytracer::UI::AppStateInfo appStateInfo;

    float64 prevTime = AstralRaytracer::Input::getTimeSinceStart();
    while (!m_window.shouldWindowClose())
    {
        m_window.clear();

        if (appStateInfo.isSceneDirty || appStateInfo.cameraUpdatedThisFrame)
        {
            m_renderer.renderEnd();
            m_renderer.resetFrameIndex();
            appStateInfo.isSceneDirty = false;
        }

        // Render scene
        m_renderer.renderStart(m_scene, m_cam);
        if (m_renderer.onRenderComplete())
        {
            m_compositor.processImage(m_scene, m_cam.getResolution(), m_renderer.getTextureId());
            appStateInfo.outputTextureId = reinterpret_cast<ImTextureID>(m_compositor.getTextureId());

            const float32 currentTimeSinceStart = AstralRaytracer::Input::getTimeSinceStart();
            const float32 deltaTime = currentTimeSinceStart - prevTime;
            prevTime = currentTimeSinceStart;

            m_window.processInput(appStateInfo, deltaTime, m_renderer, m_cam, m_scene);
        }

        // Display UI
        m_window.startUI();
        m_window.displayUI(appStateInfo, m_renderer, m_scene, m_cam, m_assetManager);
        m_window.endUI();

        m_window.swapBuffers();
        m_window.pollEvents();
    }
}

void Application::shutdown()
{
    m_renderer.renderEnd();
    m_window.shutdown();
}

AstralRaytracer::Errors::GenericError Application::parseCommandLineArgs(int32 argCount, char *argValues[],
                                                                        LaunchOptions &outLaunchOptions)
{
    CLI::App app{"A decent Raytracer", m_window.getName()};

    const std::map<std::string, LogLevel> logLevelMap{
        {"none", LogLevel::None}, {"error", LogLevel::Error}, {"warning", LogLevel::Warning}, {"info", LogLevel::Info}};

    const std::map<std::string, LogDisplay> logDisplayMap{{"none", LogDisplay::None},
                                                          {"console_and_file", LogDisplay::ConsoleAndFile},
                                                          {"console", LogDisplay::Console},
                                                          {"file", LogDisplay::File}};

    app.add_option("-l,--log-level", outLaunchOptions.logLevel, "Choose a log level: None, Error, Warning, or Info")
        ->transform(CLI::CheckedTransformer(logLevelMap, CLI::ignore_case));

    app.add_option("-d,--log-display", outLaunchOptions.logDisplay,
                   "Choose a log display: None, console_and_file, console, or file")
        ->transform(CLI::CheckedTransformer(logLevelMap, CLI::ignore_case));

    app.add_option("-p,--project-path", outLaunchOptions.projectPath, "Project path");

    try
    {
        app.parse(argCount, argValues);
    }
    catch (const CLI::ParseError &e)
    {
        return AstralRaytracer::Errors::GenericError::FAILURE;
    }

    return AstralRaytracer::Errors::GenericError::SUCCESS;
}

void Application::internalInitialize(LaunchOptions options)
{
    if (options.logLevel != LogLevel::None)
    {
        mi_option_enable(mi_option_t::mi_option_verbose);
        mi_option_enable(mi_option_t::mi_option_verbose);
        mi_option_enable(mi_option_t::mi_option_show_stats);
        mi_option_enable(mi_option_t::mi_option_show_errors);
    }

    mi_option_enable(mi_option_t::mi_option_allow_large_os_pages);
    mi_option_set(mi_option_t::mi_option_purge_delay, 0);

    spdlog::default_logger_raw()->set_level(getSpdLogLevel(options.logLevel));
    spdlog::default_logger_raw()->set_pattern("[Thread:%t][Time Delta(ms):+%-6i][%D %T][%l] %v");

    // Needs initialization in a certain order

    m_window.initialize();
    m_renderer.initialize();
    m_scene.initialize();
    m_compositor.initialize();
}

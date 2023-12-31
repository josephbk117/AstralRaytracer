#include "Application.h"

#include <mimalloc.h>

Application::Application(spdlog::level::level_enum logLevel)
{

	mi_option_enable(mi_option_t::mi_option_verbose);
	mi_option_enable(mi_option_t::mi_option_show_stats);
	mi_option_enable(mi_option_t::mi_option_show_errors);
	mi_option_enable(mi_option_t::mi_option_allow_large_os_pages);
	mi_option_set(mi_option_t::mi_option_purge_delay, 0);

	spdlog::default_logger_raw()->set_level(logLevel);
	spdlog::default_logger_raw()->set_pattern("[Thread:%t][Time Delta(ms):+%-6i][%D %T][%l] %v");
}

void Application::initialize()
{
	// Needs initialization in a certain order

	m_window.initialize();
	m_renderer.initialize();
	m_scene.initialize();
	m_compositor.initialize();

	// Load default project
	if(m_assetManager.loadProject(R"(../../../../ExampleProject/defaultProject.asproj)"))
	{
		m_scene.deserialize(m_assetManager, m_assetManager.getDefaultSceneAbsolutePath());
		m_window.setWindowTitle(m_assetManager, m_scene);
	}
}

void Application::run()
{
	AstralRaytracer::UI::AppStateInfo appStateInfo;

	float64 prevTime= AstralRaytracer::Input::getTimeSinceStart();
	while(!m_window.shouldWindowClose())
	{
		const float32 currentTimeSinceStart= AstralRaytracer::Input::getTimeSinceStart();

		const float32 deltaTime= currentTimeSinceStart - prevTime;
		prevTime               = currentTimeSinceStart;

		// Process Input
		m_window.processInput(appStateInfo, deltaTime, m_renderer, m_cam, m_scene);

		m_window.clear();

		if(appStateInfo.isSceneDirty || appStateInfo.cameraUpdatedThisFrame)
		{
			m_renderer.resetFrameIndex();
			appStateInfo.isSceneDirty= false;
		}

		// Render scene
		m_renderer.render(m_scene, m_cam);
		m_compositor.processImage(m_scene, m_cam.getResolution(), m_renderer.getTextureId());

		appStateInfo.outputTextureId= reinterpret_cast<ImTextureID>(m_compositor.getTextureId());

		// Display UI
		m_window.startUI();
		m_window.displayUI(appStateInfo, m_renderer, m_scene, m_cam, m_assetManager);
		m_window.endUI();

		m_window.swapBuffers();
		m_window.pollEvents();
	}
}

void Application::shutdown() { m_window.shutdown(); }

#include "Compositor/Compositor.h"
#include "Raytracer/Camera.h"
#include "Raytracer/Renderer.h"
#include "Utils/AssetManager.h"
#include "Utils/Common.h"
#include "WindowFramework/Input.h"
#include "WindowFramework/Window.h"
#include "WindowFramework/WindowUtils.h"

#include <mimalloc.h>

int main()
{
	mi_option_enable(mi_option_t::mi_option_verbose);
	mi_option_enable(mi_option_t::mi_option_show_stats);
	mi_option_enable(mi_option_t::mi_option_show_errors);
	mi_option_enable(mi_option_t::mi_option_allow_large_os_pages);
	mi_option_set(mi_option_t::mi_option_purge_delay, 0);

	spdlog::default_logger_raw()->set_level(spdlog::level::level_enum::trace);
	spdlog::default_logger_raw()->set_pattern("[Thread:%t][Time Delta(ms):+%-6i][%D %T][%l] %v");

	AstralRaytracer::AssetManager assetManager;
	AstralRaytracer::Window       window("Astral Raytracer");
	window.initialize();

	// Destructors need to be called before context is removed
	{
		AstralRaytracer::Renderer   renderer;
		AstralRaytracer::Camera     cam(15.0f, 15.0f, 10.0f, 0.0f);
		AstralRaytracer::Scene      scene;
		AstralRaytracer::Compositor compositor;

		// Load default project
		if(assetManager.loadProject(R"(../../../../ExampleProject/defaultProject.asproj)"))
		{
			scene.deserialize(assetManager, assetManager.getDefaultSceneAbsolutePath());
			window.setWindowTitle(assetManager, scene);
		}

		AstralRaytracer::UI::AppStateInfo appStateInfo;

		float64 prevTime= AstralRaytracer::Input::getTimeSinceStart();
		while(!window.shouldWindowClose())
		{
			const float32 currentTimeSinceStart= AstralRaytracer::Input::getTimeSinceStart();

			const float32 deltaTime= currentTimeSinceStart - prevTime;
			prevTime               = currentTimeSinceStart;

			// Process Input
			window.processInput(appStateInfo, deltaTime, renderer, cam, scene);

			window.clear();

			if(appStateInfo.isSceneDirty || appStateInfo.cameraUpdatedThisFrame)
			{
				renderer.resetFrameIndex();
				appStateInfo.isSceneDirty= false;
			}

			// Render scene
			renderer.render(scene, cam);
			compositor.processImage(scene, cam.getResolution(), renderer.getTextureId());

			appStateInfo.outputTextureId= reinterpret_cast<ImTextureID>(compositor.getTextureId());

			// Display UI
			window.startUI();
			window.displayUI(appStateInfo, renderer, scene, cam, assetManager);
			window.endUI();

			window.swapBuffers();
			window.pollEvents();
		}
	}
	window.shutdown();
	return 0;
}
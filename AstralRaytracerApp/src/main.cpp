#include "Compositor/Compositor.h"
#include "Raytracer/Camera.h"
#include "Raytracer/Renderer.h"
#include "Utils/AssetManager.h"
#include "WindowFramework/Input.h"
#include "WindowFramework/Window.h"
#include "WindowFramework/WindowUtils.h"

int main()
{
	spdlog::default_logger_raw()->set_level(spdlog::level::level_enum::trace);

	AstralRaytracer::AssetManager assetManager;
	AstralRaytracer::Window       window("Astral Raytracer");
	window.initialize();

	// Destructors need to be called before context is removed
	{
		AstralRaytracer::Renderer   renderer;
		AstralRaytracer::Camera     cam(1.0f, 1.0f, 10.0f, 0.0f, 0.1f, 100.0f);
		AstralRaytracer::Scene      scene;
		AstralRaytracer::Compositor compositor;

		// Load default scene
		scene.deserialize(assetManager, R"(../../../../ExampleProject/scenes/scene1.ascene)");

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
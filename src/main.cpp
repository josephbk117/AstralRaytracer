#include "Raytracer/Camera.h"
#include "Raytracer/DrawingPanel.h"
#include "Raytracer/ModelManager.h"
#include "Raytracer/Renderer.h"
#include "Raytracer/ShaderProgram.h"
#include "Raytracer/TextureData.h"
#include "Raytracer/TextureManager.h"
#include "Raytracer/Traceable/SphereTraceable.h"
#include "Raytracer/Traceable/StaticMesh.h"
#include "Raytracer/Traceable/TriangleTraceable.h"
#include "Utils/AssetManager.h"
#include "WindowFramework/Input.h"
#include "WindowFramework/UI/CommonUI.h"
#include "WindowFramework/UI/ContentBrowser.h"
#include "WindowFramework/UI/ImageInspect.h"
#include "WindowFramework/Window.h"
#include "WindowFramework/WindowUtils.h"

AstralRaytracer::AssetManager       assetManager;

int main()
{
	AstralRaytracer::Window window("Astral Raytracer");
	window.initialize();

	// Destructors need to be called before context is removed
	{
		AstralRaytracer::Renderer renderer;
		AstralRaytracer::Camera   cam(60.0f, 0.1f, 100.0f);
		AstralRaytracer::Scene    scene;
		scene.deserialize(assetManager, "resources/scenes/scene1.ascene");

		AstralRaytracer::UI::AppStateInfo appStateInfo;

		float64 prevTime= AstralRaytracer::Input::getTimeSinceStart();
		while(!window.shouldWindowClose())
		{
			const float32 deltaTime= AstralRaytracer::Input::getTimeSinceStart() - prevTime;

			// Process Input
			window.processInput(appStateInfo, renderer, cam, scene);

			gl::glClear(gl::ClearBufferMask::GL_COLOR_BUFFER_BIT);

			const uint32 resolutionX= appStateInfo.rendererSize.x * appStateInfo.resolutionScale * 0.01f;
			const uint32 resolutionY= appStateInfo.rendererSize.y * appStateInfo.resolutionScale * 0.01f;

			const bool cameraUpdated= cam.update(deltaTime, {resolutionX, resolutionY});

			if(appStateInfo.isSceneDirty || cameraUpdated)
			{
				renderer.resetFrameIndex();
				appStateInfo.isSceneDirty= false;
			}

			prevTime= AstralRaytracer::Input::getTimeSinceStart();

			// Render scene
			renderer.render(scene, cam);

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
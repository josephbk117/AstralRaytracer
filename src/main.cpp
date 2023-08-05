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
#include "WindowFramework/Input.h"
#include "WindowFramework/Window.h"
#include "WindowFramework/WindowUtils.h"

#include <cstring>
#include <ext/quaternion_geometric.hpp>
#include <iostream>
#include <memory>

int main()
{
	AstralRaytracer::Window window("new window test");
	window.initialize();
	AstralRaytracer::Input::initialize(window);

	// Destructors need to be called before context is removed
	{
		AstralRaytracer::Renderer renderer;
		AstralRaytracer::Camera   cam(60.0f, 0.1f, 100.0f);

		AstralRaytracer::Scene scene;
		scene.m_sceneTraceables.push_back(std::make_unique<AstralRaytracer::SphereTraceable>());
		scene.m_sceneTraceables.push_back(std::make_unique<AstralRaytracer::StaticMesh>(
				AstralRaytracer::ModelManager::getStaticMeshFromGLTF("resources/testCube.gltf")));

		float64 prevTime= AstralRaytracer::Input::getTimeSinceStart();
		while(!window.shouldWindowClose())
		{
			gl::glClear(gl::ClearBufferMask::GL_COLOR_BUFFER_BIT);

			cam.update(AstralRaytracer::Input::getTimeSinceStart() - prevTime);
			prevTime= AstralRaytracer::Input::getTimeSinceStart();

			renderer.render(scene, cam);

			window.startUI();

			ImGui::Begin("GameWindow");
			{
				ImGui::BeginChild("GameRender");
				ImVec2 wsize= ImGui::GetWindowSize();
				ImGui::Image(reinterpret_cast<ImTextureID>(renderer.getTextureId()), wsize, ImVec2(0, 1),
										 ImVec2(1, 0));
				ImGui::EndChild();
			}
			ImGui::End();

			window.endUI();

			window.swapBuffers();
			window.pollEvents();
		}
	}

	window.shutdown();
	return 0;
}
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
	AstralRaytracer::Window window("Astral Raytracer");
	window.initialize();
	AstralRaytracer::Input::initialize(window);

	// Destructors need to be called before context is removed
	{
		AstralRaytracer::Renderer renderer;
		AstralRaytracer::Camera   cam(60.0f, 0.001f, 100.0f);

		AstralRaytracer::Scene scene;
		scene.m_materials.push_back(AstralRaytracer::Material{AstralRaytracer::Colors::Blue, 0.925f});
		scene.m_materials.push_back(AstralRaytracer::Material{AstralRaytracer::Colors::Yellow, 0.925f});
		scene.m_materials.push_back(AstralRaytracer::Material{AstralRaytracer::Colors::White, 0.925f});

		scene.m_sceneTraceables.push_back(std::make_unique<AstralRaytracer::SphereTraceable>());
		scene.m_sceneTraceables.push_back(std::make_unique<AstralRaytracer::SphereTraceable>());
		scene.m_sceneTraceables.push_back(std::make_unique<AstralRaytracer::StaticMesh>(
				AstralRaytracer::ModelManager::getStaticMeshFromGLTF("resources/testCube.gltf")));
		scene.m_sceneTraceables.push_back(std::make_unique<AstralRaytracer::TriangleTraceable>(
				glm::vec3(-100.0f, -1.0f, -100.0f), glm::vec3(0.0f, -1.0f, 100.0f),
				glm::vec3(100.0f, -1.0f, -100.0f)));
		;
		scene.m_sceneTraceables.at(0)->setPosition(glm::vec3(4.0f, 0.0f, -2.0f));
		scene.m_sceneTraceables.at(2)->setPosition(glm::vec3(1.0f, 0.0f, -2.0f));
		scene.m_sceneTraceables.at(1)->setMaterialIndex(1);
		scene.m_sceneTraceables.at(2)->setMaterialIndex(2);
		scene.m_sceneTraceables.at(3)->setMaterialIndex(3);

		float64 prevTime= AstralRaytracer::Input::getTimeSinceStart();
		while(!window.shouldWindowClose())
		{
			gl::glClear(gl::ClearBufferMask::GL_COLOR_BUFFER_BIT);

			if(cam.update(AstralRaytracer::Input::getTimeSinceStart() - prevTime))
			{
				renderer.resetFrameIndex();
			}
			prevTime= AstralRaytracer::Input::getTimeSinceStart();

			renderer.render(scene, cam);

			window.startUI();

			static ImGuiWindowFlags flags= ImGuiWindowFlags_NoDecoration | ImGuiWindowFlags_NoMove |
																		 ImGuiWindowFlags_NoSavedSettings | ImGuiWindowFlags_MenuBar;

			const ImGuiViewport* viewport= ImGui::GetMainViewport();
			ImGui::SetNextWindowPos(viewport->WorkPos);
			ImGui::SetNextWindowSize(viewport->WorkSize);

			bool isOpen= true;
			if(ImGui::Begin("Main Window", &isOpen, flags))
			{
				if(ImGui::BeginMenuBar())
				{
					if(ImGui::BeginMenu("File"))
					{
						if(ImGui::MenuItem("Open", "Ctrl+O")) {}
						ImGui::EndMenu();
					}
					ImGui::EndMenuBar();
				}

				ImGui::BeginChild("RaytracerView");
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
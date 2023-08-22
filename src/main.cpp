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

#include <gtc/type_ptr.hpp>
#include <memory>

int main()
{
	AstralRaytracer::Window window("Astral Raytracer");
	window.initialize();
	AstralRaytracer::Input::initialize(window);

	// Destructors need to be called before context is removed
	{
		AstralRaytracer::Renderer renderer;
		AstralRaytracer::Camera   cam(60.0f, 0.1f, 100.0f);

		AstralRaytracer::Scene scene;
		scene.addTexture(
				TextureManager::loadTextureDataFromFile("resources/textures/floor_texture.jpg"), "Floor");

		scene.addMaterial(AstralRaytracer::Material{AstralRaytracer::Colors::Blue,
																								AstralRaytracer::Colors::White, 0.0f, 0.925f},
											"Mat1");
		scene.addMaterial(AstralRaytracer::Material{AstralRaytracer::Colors::Yellow,
																								AstralRaytracer::Colors::White, 0.0f, 0.925f},
											"Mat2");
		scene.addMaterial(AstralRaytracer::Material{AstralRaytracer::Colors::White,
																								AstralRaytracer::Colors::White, 0.0f, 0.925f},
											"Mat3");
		scene.m_materials.at(3).texture= 1;

		scene.addTraceable(std::make_unique<AstralRaytracer::SphereTraceable>(), "Sphere1");
		scene.addTraceable(std::make_unique<AstralRaytracer::SphereTraceable>(), "Sphere2");
		scene.addTraceable(
				std::make_unique<AstralRaytracer::StaticMesh>(
						AstralRaytracer::ModelManager::getStaticMeshFromGLTF("resources/testCube.gltf")),
				"Cube");
		scene.addTraceable(std::make_unique<AstralRaytracer::TriangleTraceable>(
													 glm::vec3(-100.0f, 0.0f, -100.0f), glm::vec3(0.0f, 0.0f, 100.0f),
													 glm::vec3(100.0f, 0.0f, -100.0f)),
											 "Floor");

		scene.m_sceneTraceables.at(0)->setPosition(glm::vec3(4.0f, 0.0f, -2.0f));
		scene.m_sceneTraceables.at(2)->setPosition(glm::vec3(1.0f, 0.0f, -2.0f));
		scene.m_sceneTraceables.at(3)->setPosition(glm::vec3(0.0f, -1.0f, 0.0f));
		scene.m_sceneTraceables.at(1)->setMaterialIndex(1);
		scene.m_sceneTraceables.at(2)->setMaterialIndex(2);
		scene.m_sceneTraceables.at(3)->setMaterialIndex(3);

		float64      prevTime= AstralRaytracer::Input::getTimeSinceStart();
		glm::u32vec2 rendererSize{500, 500};
		float32      resolutionScale= 50.0f;
		bool         isSceneDirty   = false;

		uint32 selectedObjectIndex= 0;
		while(!window.shouldWindowClose())
		{
			gl::glClear(gl::ClearBufferMask::GL_COLOR_BUFFER_BIT);

			const float32 deltaTime= AstralRaytracer::Input::getTimeSinceStart() - prevTime;

			const uint32 resolutionX= rendererSize.x * resolutionScale * 0.01f;
			const uint32 resolutionY= rendererSize.y * resolutionScale * 0.01f;

			const bool cameraUpdated= cam.update(deltaTime, {resolutionX, resolutionY});

			if(isSceneDirty || cameraUpdated)
			{
				renderer.resetFrameIndex();
				isSceneDirty= false;
			}
			prevTime= AstralRaytracer::Input::getTimeSinceStart();

			renderer.render(scene, cam);

			window.startUI();
			ImGuizmo::BeginFrame();
			ImGuizmo::SetOrthographic(false);
			ImGuizmo::SetRect(ImGui::GetWindowPos().x, ImGui::GetWindowPos().y, rendererSize.x,
												rendererSize.y);

			constexpr ImGuiWindowFlags flags= ImGuiWindowFlags_NoDecoration | ImGuiWindowFlags_NoMove |
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
				constexpr ImGuiWindowFlags flags2= ImGuiWindowFlags_NoDecoration | ImGuiWindowFlags_NoMove;

				ImGui::BeginChild(1, ImVec2(ImGui::GetWindowWidth(), 32), true, flags2);
				int32 bounceCount= renderer.getBounceCount();

				ImGui::SetNextItemWidth(ImGui::GetContentRegionAvail().x * 0.35f);
				if(ImGui::SliderInt("Bounce Count", &bounceCount, 1, 12))
				{
					renderer.setBounceCount(bounceCount);
					isSceneDirty= true;
				}

				ImGui::SameLine();
				ImGui::SetNextItemWidth(ImGui::GetContentRegionAvail().x * 0.35f);
				if(ImGui::SliderFloat("Resolution Scale", &resolutionScale, 10.0f, 120.0f))
				{
					isSceneDirty= true;
				}
				ImGui::EndChild();

				constexpr int32 tableFlags= ImGuiTableFlags_BordersOuter | ImGuiTableFlags_Resizable;

				if(ImGui::BeginTable("viewSplit", 2, tableFlags))
				{
					ImGui::TableNextRow();
					ImGui::TableSetColumnIndex(0);

					ImVec2 availableRegion= ImGui::GetContentRegionAvail();
					rendererSize          = {availableRegion.x, availableRegion.y};

					ImGui::Image(reinterpret_cast<ImTextureID>(renderer.getTextureId()), availableRegion,
											 ImVec2(0, 1), ImVec2(1, 0));

					ImGuizmo::SetDrawlist();

					glm::mat4 transform=
							scene.m_sceneTraceables.at(selectedObjectIndex).get()->getTransformMatrix();

					ImGuizmo::Manipulate(glm::value_ptr(cam.getView()), glm::value_ptr(cam.getProjection()),
															 IMGUIZMO_NAMESPACE::TRANSLATE, IMGUIZMO_NAMESPACE::LOCAL,
															 glm::value_ptr(transform));

					if(ImGuizmo::IsUsing())
					{
						scene.m_sceneTraceables.at(selectedObjectIndex)->setPosition(glm::vec3(transform[3]));
						isSceneDirty= true;
					}

					ImGui::TableSetColumnIndex(1);

					ImGui::Text("Materials");
					const uint32 materialCount= scene.m_materials.size();
					const uint32 textureCount = scene.m_textures.size();
					for(uint32 matIndex= 0; matIndex < materialCount; ++matIndex)
					{
						ImGui::PushID(matIndex);
						AstralRaytracer::Material& mat= scene.m_materials.at(matIndex);
						if(ImGui::ColorEdit3("Albedo", reinterpret_cast<float*>(&mat.albedo)))
						{
							isSceneDirty= true;
						}
						if(ImGui::SliderInt("Texture", reinterpret_cast<int*>(&mat.texture), 0,
																textureCount - 1))
						{
							isSceneDirty= true;
						}
						if(ImGui::ColorEdit3("Emission", reinterpret_cast<float*>(&mat.emission)))
						{
							isSceneDirty= true;
						}
						if(ImGui::SliderFloat("Emission Strength", &mat.emissionStrength, 0.0f, 1000.0f))
						{
							isSceneDirty= true;
						}
						if(ImGui::SliderFloat("Roughness", &mat.roughness, 0.0f, 1.0f))
						{
							isSceneDirty= true;
						}
						ImGui::Separator();
						ImGui::PopID();
					}

					ImGui::Separator();
					ImGui::Separator();

					ImGui::Text("Objects");

					for(uint32 objIndex= 0; objIndex < scene.m_sceneTraceables.size(); ++objIndex)
					{
						bool isSelected= objIndex == selectedObjectIndex;
						if(ImGui::Selectable(scene.getTraceableName(objIndex).c_str(), &isSelected))
						{
							selectedObjectIndex= objIndex;
						}
					}

					ImGui::EndTable();
				}
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
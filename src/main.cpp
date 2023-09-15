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
#include "WindowFramework/Window.h"
#include "WindowFramework/WindowUtils.h"

#include <gtc/type_ptr.hpp>
#include <gtx/matrix_decompose.hpp>
#include <memory>

struct AppStateInfo
{
	glm::u32vec2 rendererSize{500, 500};
	glm::u32vec2 rendererResolution{500, 500};
	UiBounds     uiBounds;
	float32      resolutionScale    = 50.0f;
	bool         isSceneDirty       = false;
	bool         canSelectObjects   = false;
	uint32       selectedObjectIndex= 0;
};

AstralRaytracer::AssetManager       assetManager;
AstralRaytracer::UI::ContentBrowser contentBrowser(assetManager);

void processInput(AppStateInfo& appStateInfo, AstralRaytracer::Renderer& renderer,
									AstralRaytracer::Camera& cam, const AstralRaytracer::Scene& scene);

void displayUI(AstralRaytracer::Renderer& renderer, AppStateInfo& appStateInfo,
							 const AstralRaytracer::Window& window, AstralRaytracer::Scene& scene,
							 AstralRaytracer::Camera& cam);

int main()
{
	AstralRaytracer::Window window("Astral Raytracer");
	window.initialize();
	AstralRaytracer::Input::initialize(window);

	// Destructors need to be called before context is removed
	{
		AstralRaytracer::Renderer renderer;
		AstralRaytracer::Camera   cam(60.0f, 0.1f, 100.0f);
		AstralRaytracer::Scene    scene;
		scene.deserialize(assetManager, "resources/scenes/scene1.ascene");

		AppStateInfo appStateInfo;

		float64 prevTime= AstralRaytracer::Input::getTimeSinceStart();
		while(!window.shouldWindowClose())
		{
			const float32 deltaTime= AstralRaytracer::Input::getTimeSinceStart() - prevTime;

			// Process Input
			processInput(appStateInfo, renderer, cam, scene);

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
			displayUI(renderer, appStateInfo, window, scene, cam);
			window.endUI();

			window.swapBuffers();
			window.pollEvents();
		}
	}

	window.shutdown();
	return 0;
}

void processInput(AppStateInfo& appStateInfo, AstralRaytracer::Renderer& renderer,
									AstralRaytracer::Camera& cam, const AstralRaytracer::Scene& scene)
{
	const glm::vec2& mousePos= AstralRaytracer::Input::getMousePosition();
	if(appStateInfo.canSelectObjects && !appStateInfo.isSceneDirty &&
		 AstralRaytracer::Input::isMouseButtonDown(AstralRaytracer::MouseButtonIndex::MOUSE_BUTTON_1) &&
		 appStateInfo.uiBounds.isPointInBounds(mousePos))
	{

		HitInfo   closestHitInfo;
		glm::vec2 coOrd((mousePos.x - appStateInfo.uiBounds.min.x) / appStateInfo.rendererSize.x,
										(mousePos.y - appStateInfo.uiBounds.min.y) / appStateInfo.rendererSize.y);
		coOrd.y= 1.0f - coOrd.y;

		const glm::vec3& rayDir= renderer.getRayDirectionFromNormalizedCoord(
				coOrd, cam.getInverseProjection(), cam.getInverseView());
		renderer.findClosestHit(closestHitInfo, scene, cam.getPosition(), rayDir);

		if(closestHitInfo.isValid())
		{
			appStateInfo.selectedObjectIndex= closestHitInfo.objectIndex;
		}
	}
}

void displayMaterialUI(AstralRaytracer::Scene& scene, const AstralRaytracer::Window& window,
											 AppStateInfo& appStateInfo)
{
	ImGui::PushFont(window.getTertiaryFont());
	ImGui::Text("Material");
	ImGui::PopFont();
	const uint32 textureCount= scene.m_textures.size();

	const uint32 matIndex=
			scene.m_sceneTraceables.at(appStateInfo.selectedObjectIndex).get()->getMaterialIndex();
	AstralRaytracer::Material& mat= scene.m_materials.at(matIndex);
	ImGui::Text(assetManager.getNameAndPathOfMaterial(matIndex).value().assetName.c_str());

	if(ImGui::ColorEdit3("Albedo", reinterpret_cast<float*>(&mat.albedo)))
	{
		appStateInfo.isSceneDirty= true;
	}
	if(ImGui::SliderInt("Texture", reinterpret_cast<int*>(&mat.texture), 0, textureCount - 1, "%d",
											ImGuiSliderFlags_AlwaysClamp))
	{
		appStateInfo.isSceneDirty= true;
	}
	if(ImGui::ColorEdit3("Emission", reinterpret_cast<float*>(&mat.emission)))
	{
		appStateInfo.isSceneDirty= true;
	}
	if(ImGui::SliderFloat("Emission Strength", &mat.emissionStrength, 0.0f, 1000.0f, "%.2f",
												ImGuiSliderFlags_AlwaysClamp))
	{
		appStateInfo.isSceneDirty= true;
	}
	if(ImGui::SliderFloat("Roughness", &mat.roughness, 0.0f, 1.0f, "%.2f",
												ImGuiSliderFlags_AlwaysClamp))
	{
		appStateInfo.isSceneDirty= true;
	}
	ImGui::Separator();
}

void displayTransformUI(AstralRaytracer::Scene& scene, const AstralRaytracer::Window& window,
												AppStateInfo& appStateInfo)
{
	ImGui::PushFont(window.getTertiaryFont());
	ImGui::Text("Transform");
	ImGui::PopFont();
	if(AstralRaytracer::UI::displayTransform(
				 *scene.m_sceneTraceables[appStateInfo.selectedObjectIndex]))
	{
		appStateInfo.isSceneDirty= true;
	}
}

void displaySceneObjectsUI(const AstralRaytracer::Scene&        scene,
													 const AstralRaytracer::Window&       window,
													 const AstralRaytracer::AssetManager& assetManager,
													 AppStateInfo&                        appStateInfo)
{
	ImGui::PushFont(window.getSecondaryFont());
	ImGui::Text("SCENE");
	ImGui::PopFont();

	for(uint32 objIndex= 0; objIndex < scene.m_sceneTraceables.size(); ++objIndex)
	{
		bool isSelected= objIndex == appStateInfo.selectedObjectIndex;
		if(ImGui::Selectable(assetManager.getNameAndPathOfTraceable(objIndex).value().assetName.c_str(),
												 &isSelected))
		{
			appStateInfo.selectedObjectIndex= objIndex;
		}
	}
}

void displayUI(AstralRaytracer::Renderer& renderer, AppStateInfo& appStateInfo,
							 const AstralRaytracer::Window& window, AstralRaytracer::Scene& scene,
							 AstralRaytracer::Camera& cam)
{
	ImGuizmo::BeginFrame();
	ImGuizmo::SetOrthographic(false);

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

		const uint32 toolBarHeight= 32;

		ImGui::BeginChild(1, ImVec2(ImGui::GetWindowWidth(), toolBarHeight), true, flags2);
		int32 bounceCount= renderer.getBounceCount();

		const float32 sliderWidth= ImGui::GetContentRegionAvail().x / 4.0f;

		ImGui::SetNextItemWidth(sliderWidth);
		if(ImGui::SliderInt("##Bounce Count", &bounceCount, 1, 32, "Bounce Count:%d",
												ImGuiSliderFlags_AlwaysClamp))
		{
			renderer.setBounceCount(bounceCount);
			appStateInfo.isSceneDirty= true;
		}

		ImGui::SameLine();
		ImGui::SetNextItemWidth(sliderWidth);

		if(ImGui::SliderFloat("##Resolution Scale", &appStateInfo.resolutionScale, 10.0f, 100.0f,
													"Resolution Scale:%.1f%%", ImGuiSliderFlags_AlwaysClamp))
		{
			appStateInfo.isSceneDirty= true;
		}

		ImGui::SameLine();
		ImGui::SetNextItemWidth(sliderWidth);
		if(ImGui::SliderInt2("Resolution", reinterpret_cast<int32*>(&appStateInfo.rendererResolution),
												 64, 1920, "%dpx", ImGuiSliderFlags_AlwaysClamp))
		{
			appStateInfo.isSceneDirty= true;
		}
		ImGui::EndChild();

		constexpr int32 tableFlags= ImGuiTableFlags_BordersOuter | ImGuiTableFlags_Resizable |
																ImGuiTableFlags_NoHostExtendY | ImGuiTableFlags_NoHostExtendX;
		constexpr ImGuiTableRowFlags rowFlags= ImGuiTableRowFlags_None;
		if(ImGui::BeginTable("viewportContentSplit", 1, tableFlags, ImGui::GetContentRegionAvail()))
		{
			ImGui::TableNextRow(rowFlags, 100.0f);
			ImGui::TableSetColumnIndex(0);
			const float32 viewportSceneInfoSplitHeight= ImGui::GetContentRegionAvail().y * 0.6f;
			if(ImGui::BeginTable("viewportSceneInfoSplit", 3, tableFlags,
													 {ImGui::GetContentRegionAvail().x, viewportSceneInfoSplitHeight}))
			{
				ImGui::TableNextRow(rowFlags, 100.0f);
				ImGui::TableSetColumnIndex(0);

				displaySceneObjectsUI(scene, window, assetManager, appStateInfo);

				ImGui::TableSetColumnIndex(1);

				ImVec2 availableRegion= ImGui::GetContentRegionAvail();
				availableRegion.y     = viewportSceneInfoSplitHeight;
				const float32 scale=
						glm::min(availableRegion.x / (float32)appStateInfo.rendererResolution.x,
										 availableRegion.y / (float32)appStateInfo.rendererResolution.y);

				const ImVec2 newRegion= {appStateInfo.rendererResolution.x * scale,
																 appStateInfo.rendererResolution.y * scale};
				const ImVec2 gapRegion= {(availableRegion.x - newRegion.x) * 0.5f,
																 (availableRegion.y - newRegion.y) * 0.5f};

				ImGui::Dummy({availableRegion.x, gapRegion.y});
				ImGui::Dummy({gapRegion.x, newRegion.y});

				appStateInfo.rendererSize= {newRegion.x, newRegion.y};

				ImGui::SameLine();
				ImGui::BeginChild(2, newRegion, false,
													ImGuiWindowFlags_NoDecoration | ImGuiWindowFlags_NoMove |
															ImGuiWindowFlags_NoResize);

				ImGui::Image(reinterpret_cast<ImTextureID>(renderer.getTextureId()),
										 ImGui::GetContentRegionAvail(), ImVec2(0, 1), ImVec2(1, 0));

				appStateInfo.uiBounds.min= {ImGui::GetItemRectMin().x, ImGui::GetItemRectMin().y};
				appStateInfo.uiBounds.max= {ImGui::GetItemRectMax().x, ImGui::GetItemRectMax().y};

				ImGuizmo::SetRect(ImGui::GetWindowPos().x, ImGui::GetWindowPos().y, newRegion.x,
													newRegion.y);
				ImGuizmo::SetDrawlist();

				glm::mat4 transform= scene.m_sceneTraceables.at(appStateInfo.selectedObjectIndex)
																 .get()
																 ->getTransformMatrix();

				ImGuizmo::Manipulate(glm::value_ptr(cam.getView()), glm::value_ptr(cam.getProjection()),
														 IMGUIZMO_NAMESPACE::TRANSLATE | IMGUIZMO_NAMESPACE::SCALE,
														 IMGUIZMO_NAMESPACE::LOCAL, glm::value_ptr(transform));

				appStateInfo.canSelectObjects= !ImGuizmo::IsOver();

				if(ImGuizmo::IsUsing())
				{
					glm::vec3 newScale;
					glm::vec3 newTranslation;
					glm::quat newRot;
					glm::vec3 skew;
					glm::vec4 perspective;
					glm::decompose(transform, newScale, newRot, newTranslation, skew, perspective);

					scene.m_sceneTraceables.at(appStateInfo.selectedObjectIndex)->setPosition(newTranslation);
					scene.m_sceneTraceables.at(appStateInfo.selectedObjectIndex)->setScale(newScale);
					appStateInfo.isSceneDirty= true;
				}

				ImGui::EndChild();
				ImGui::Dummy({availableRegion.x, gapRegion.y});

				ImGui::TableSetColumnIndex(2);

				ImGui::PushFont(window.getSecondaryFont());
				ImGui::Text("INSPECTOR");
				ImGui::PopFont();
				displayTransformUI(scene, window, appStateInfo);

				ImGui::Separator();
				ImGui::Separator();

				displayMaterialUI(scene, window, appStateInfo);

				ImGui::EndTable();
			}

			ImGui::TableNextRow(rowFlags, 100.0f);
			ImGui::TableSetColumnIndex(0);

			contentBrowser.display();

			ImGui::EndTable();
		}
	}

	ImGui::End();
}
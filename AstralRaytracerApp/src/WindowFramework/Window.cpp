#include "WindowFramework/Window.h"

#include "Raytracer/Renderer.h"
#include "Raytracer/TextureManager.h"
#include "WindowFramework/Input.h"
#include "WindowFramework/UI/ImageInspect.h"

#include <imgui_internal.h>

namespace AstralRaytracer
{
	Window*            Window::m_instance= nullptr;
	const std::string& Window::getName() const { return m_name; }

	void Window::initialize()
	{
		assert(m_instance == nullptr);
		m_instance= this;

		m_resolution= {500, 500};

		if(!glfwInit())
		{
			exit(EXIT_FAILURE);
		}

		glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);
		glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 2);
		glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

		m_glfwWindow=
				glfwCreateWindow(m_resolution.first, m_resolution.second, m_name.c_str(), nullptr, nullptr);

		if(!m_glfwWindow)
		{
			glfwTerminate();
			exit(EXIT_FAILURE);
		}

		glfwSetWindowSizeCallback(m_glfwWindow, windowSizeCallback);
		glfwMakeContextCurrent(m_glfwWindow);

		TextureData iconTexData=
				TextureManager::loadTextureDataFromFile("app_assets/icons/astralraytracer.png", 4);

		GLFWimage image[1];
		image[0].width = iconTexData.getWidth();
		image[0].height= iconTexData.getHeight();
		image[0].pixels= const_cast<uint8*>(iconTexData.getTextureData().data());
		glfwSetWindowIcon(m_glfwWindow, 1, image);

		glbinding::initialize(glfwGetProcAddress);

		IMGUI_CHECKVERSION();
		ImGui::CreateContext();
		ImGui::StyleColorsDark();
		ImGui_ImplGlfw_InitForOpenGL(m_glfwWindow, true);
		ImGui_ImplOpenGL3_Init("#version 420 core");

		ImGuiIO& io= ImGui::GetIO();

		m_primaryFont  = io.Fonts->AddFontFromFileTTF("app_assets/fonts/ABeeZee-Regular.ttf", 16.0f);
		m_secondaryFont= io.Fonts->AddFontFromFileTTF("app_assets/fonts/Roboto-Regular.ttf", 22.0f);
		m_tertiaryFont = io.Fonts->AddFontFromFileTTF("app_assets/fonts/Roboto-Regular.ttf", 18.0f);

		gl::glViewport(0, 0, m_resolution.first, m_resolution.second);

		AstralRaytracer::Input::initialize(*this);
	}

	void Window::processInput(UI::AppStateInfo& appStateInfo, Renderer& renderer, Camera& cam,
														const Scene& scene)
	{
		const glm::vec2& mousePos= Input::getMousePosition();
		if(appStateInfo.canSelectObjects && !appStateInfo.isSceneDirty &&
			 Input::isMouseButtonDown(MouseButtonIndex::MOUSE_BUTTON_1) &&
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

	void Window::displayUI(UI::AppStateInfo& appStateInfo, Renderer& renderer, Scene& scene,
												 Camera& cam, AssetManager& assetManager)
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

					displaySceneObjectsUI(appStateInfo, scene, assetManager);

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

					ImVec2 imageDisplaySize= ImGui::GetContentRegionAvail();
					ImGui::Image(reinterpret_cast<ImTextureID>(renderer.getTextureId()), imageDisplaySize,
											 ImVec2(0, 1), ImVec2(1, 0));

					auto&  io= ImGui::GetIO();
					ImRect rc= ImRect(ImGui::GetItemRectMin(), ImGui::GetItemRectMax());
					ImVec2 mouseUVCoord;
					mouseUVCoord.x= (io.MousePos.x - rc.Min.x) / rc.GetSize().x;
					mouseUVCoord.y= (io.MousePos.y - rc.Min.y) / rc.GetSize().y;

					mouseUVCoord.y= 1.f - mouseUVCoord.y;

					if(io.KeyShift && io.MouseDown[0] && mouseUVCoord.x >= 0.f && mouseUVCoord.y >= 0.f &&
						 mouseUVCoord.x < 1.0f && mouseUVCoord.y < 1.0f)
					{
						UI::inspect(renderer.getTextureData(), mouseUVCoord, imageDisplaySize);
					}

					appStateInfo.uiBounds.min= {ImGui::GetItemRectMin().x, ImGui::GetItemRectMin().y};
					appStateInfo.uiBounds.max= {ImGui::GetItemRectMax().x, ImGui::GetItemRectMax().y};

					ImGuizmo::SetRect(ImGui::GetWindowPos().x, ImGui::GetWindowPos().y, newRegion.x,
														newRegion.y);
					ImGuizmo::SetDrawlist();

					if(scene.hasSceneLoaded())
					{

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

							scene.m_sceneTraceables.at(appStateInfo.selectedObjectIndex)
									->setPosition(newTranslation);
							scene.m_sceneTraceables.at(appStateInfo.selectedObjectIndex)->setScale(newScale);
							appStateInfo.isSceneDirty= true;
						}
					}

					ImGui::EndChild();
					ImGui::Dummy({availableRegion.x, gapRegion.y});

					ImGui::TableSetColumnIndex(2);

					ImGui::PushFont(getSecondaryFont());
					ImGui::SeparatorText("INSPECTOR");
					ImGui::PopFont();
					displayTransformUI(appStateInfo, scene, assetManager);

					ImGui::Separator();
					ImGui::Separator();

					displayMaterialUI(appStateInfo, scene, assetManager);

					ImGui::EndTable();
				}

				ImGui::TableNextRow(rowFlags, 100.0f);
				ImGui::TableSetColumnIndex(0);

				m_contentBrowser.display(assetManager);

				ImGui::EndTable();
			}
		}

		ImGui::End();
	}

	void Window::displaySceneObjectsUI(UI::AppStateInfo& appStateInfo, const Scene& scene,
																		 const AssetManager& assetManager)
	{
		ImGui::PushFont(getSecondaryFont());
		ImGui::SeparatorText("SCENE");
		ImGui::PopFont();

		for(uint32 objIndex= 0; objIndex < scene.m_sceneTraceables.size(); ++objIndex)
		{
			bool isSelected= objIndex == appStateInfo.selectedObjectIndex;
			if(ImGui::Selectable(
						 assetManager.getNameAndPathOfTraceable(objIndex).value().assetName.c_str(),
						 &isSelected))
			{
				appStateInfo.selectedObjectIndex= objIndex;
			}
		}
	}

	void Window::displayMaterialUI(UI::AppStateInfo& appStateInfo, Scene& scene,
																 const AssetManager& assetManager)
	{
		ImGui::PushFont(getTertiaryFont());
		ImGui::Text("Material");
		ImGui::PopFont();

		if(!scene.hasSceneLoaded())
		{
			return;
		}
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

	void Window::displayTransformUI(UI::AppStateInfo& appStateInfo, Scene& scene,
																	const AssetManager& assetManager)
	{
		ImGui::PushFont(getTertiaryFont());
		ImGui::Text("Transform");
		ImGui::PopFont();
		if(!scene.hasSceneLoaded())
		{
			return;
		}
		if(AstralRaytracer::UI::displayTransform(
					 *scene.m_sceneTraceables[appStateInfo.selectedObjectIndex]))
		{
			appStateInfo.isSceneDirty= true;
		}
	}

	void Window::windowSizeCallback(GLFWwindow* window, int32 width, int32 height)
	{
		m_instance->m_resolution.first = width;
		m_instance->m_resolution.second= height;
		gl::glViewport(0, 0, width, height);
	}

	bool Window::shouldWindowClose() const { return glfwWindowShouldClose(m_glfwWindow); }

	void Window::shutdown()
	{
		ImGui_ImplOpenGL3_Shutdown();
		ImGui_ImplGlfw_Shutdown();
		ImGui::DestroyContext();
		glfwTerminate();
	}

	void Window::swapBuffers() { glfwSwapBuffers(m_glfwWindow); }

	void Window::pollEvents() { glfwPollEvents(); }

	void Window::startUI()
	{
		ImGui_ImplOpenGL3_NewFrame();
		ImGui_ImplGlfw_NewFrame();
		ImGui::NewFrame();
	}

	void Window::endUI()
	{
		ImGui::Render();
		ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());
	}
} // namespace AstralRaytracer
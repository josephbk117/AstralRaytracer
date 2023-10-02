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

		m_resolution= {1280, 720};

		if(!glfwInit())
		{
			exit(EXIT_FAILURE);
		}

		glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);
		glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 2);
		glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

		m_glfwWindow=
				glfwCreateWindow(m_resolution.first, m_resolution.second, m_name.c_str(), nullptr, nullptr);

		if(m_glfwWindow == nullptr)
		{
			glfwTerminate();
			exit(EXIT_FAILURE);
		}

		glfwSetWindowSizeCallback(m_glfwWindow, windowSizeCallback);
		glfwMakeContextCurrent(m_glfwWindow);

		TextureDataRGBA iconTexData=
				TextureManager::loadTextureDataFromFileRGBA("app_assets/icons/astralraytracer.png");

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

		setDefaultTheme();
	}

	void Window::setSelectedObjectIndexFromMouseCoord(const glm::vec2&                   mousePos,
																										AstralRaytracer::UI::AppStateInfo& appStateInfo,
																										const AstralRaytracer::Renderer&   renderer,
																										const AstralRaytracer::Camera&     cam,
																										const AstralRaytracer::Scene&      scene)
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

	void Window::processInput(UI::AppStateInfo& appStateInfo, float32 deltaTime, Renderer& renderer,
														Camera& cam, const Scene& scene)
	{
		m_frameTimes.push(deltaTime);
		if(m_frameTimes.size() > FrameSampleCount)
		{
			m_frameTimes.pop();
		}

		//------- Object selection--------//

		const glm::vec2& mousePos= Input::getMousePosition();
		if(appStateInfo.canSelectObjects && !appStateInfo.isSceneDirty &&
			 Input::isMouseButtonDown(MouseButtonIndex::MOUSE_BUTTON_LEFT) &&
			 appStateInfo.uiBounds.isPointInBounds(mousePos))
		{
			setSelectedObjectIndexFromMouseCoord(mousePos, appStateInfo, renderer, cam, scene);
		}

		//------- Camera update--------//

		appStateInfo.cameraUpdatedThisFrame= false;

		const float32 resScale      = appStateInfo.resolutionScale * 0.01f;
		const uint32  camResolutionX= appStateInfo.rendererSize.x * resScale;
		const uint32  camResolutionY= appStateInfo.rendererSize.y * resScale;

		const glm::u32vec2 newCamRes(camResolutionX, camResolutionY);

		static glm::vec2 m_lastMousePosition(0.0f);

		if(m_lastMousePosition == glm::vec2(0.0f))
		{
			m_lastMousePosition= Input::getMousePosition();
			return;
		}

		const glm::vec2& mouseDelta= (mousePos - m_lastMousePosition);
		m_lastMousePosition        = mousePos;

		bool forceRecalculate= cam.getResolution() != newCamRes;

		if(!Input::isMouseButtonDown(MouseButtonIndex::MOUSE_BUTTON_RIGHT) && !forceRecalculate)
		{
			Input::setCursorMode(CursorMode::NORMAL);
			return;
		}

		bool moved= false;

		if(!forceRecalculate)
		{
			Input::setCursorMode(CursorMode::CAPTURED);

			float32 moveSpeed= deltaTime;
			if(Input::isKeyDown(InputKey::LEFT_SHIFT))
			{
				moveSpeed*= 5.0f;
			}

			if(Input::isKeyDown(InputKey::W))
			{
				cam.moveForward(moveSpeed);
				moved= true;
			}

			if(Input::isKeyDown(InputKey::S))
			{
				cam.moveForward(-moveSpeed);
				moved= true;
			}

			if(Input::isKeyDown(InputKey::A))
			{
				cam.moveRight(-moveSpeed);
				moved= true;
			}

			if(Input::isKeyDown(InputKey::D))
			{
				cam.moveRight(moveSpeed);
				moved= true;
			}

			if(Input::isKeyDown(InputKey::Q))
			{
				cam.moveUp(moveSpeed);
				moved= true;
			}

			if(Input::isKeyDown(InputKey::E))
			{
				cam.moveUp(-moveSpeed);
				moved= true;
			}

			// Rotation

			if(mouseDelta.x != 0.0f || mouseDelta.y != 0.0f)
			{
				cam.rotate(mouseDelta * 0.1f);
				moved= true;
			}
		}

		if(moved || forceRecalculate)
		{
			cam.update(newCamRes);
		}

		appStateInfo.cameraUpdatedThisFrame= moved || forceRecalculate;
	}

	void Window::setDefaultTheme() const
	{
		ImGuiStyle* style = &ImGui::GetStyle();
		ImVec4*     colors= style->Colors;
		ImGui::StyleColorsDark(style); // Reset to base/dark theme
		colors[ImGuiCol_Text]                = ImVec4(0.85f, 0.87f, 0.91f, 0.88f);
		colors[ImGuiCol_TextDisabled]        = ImVec4(0.49f, 0.50f, 0.53f, 1.00f);
		colors[ImGuiCol_WindowBg]            = ImVec4(0.18f, 0.20f, 0.25f, 1.00f);
		colors[ImGuiCol_ChildBg]             = ImVec4(0.16f, 0.17f, 0.20f, 1.00f);
		colors[ImGuiCol_PopupBg]             = ImVec4(0.23f, 0.26f, 0.32f, 1.00f);
		colors[ImGuiCol_Border]              = ImVec4(0.14f, 0.16f, 0.19f, 1.00f);
		colors[ImGuiCol_BorderShadow]        = ImVec4(0.09f, 0.09f, 0.09f, 0.00f);
		colors[ImGuiCol_FrameBg]             = ImVec4(0.23f, 0.26f, 0.32f, 1.00f);
		colors[ImGuiCol_FrameBgHovered]      = ImVec4(0.56f, 0.74f, 0.73f, 1.00f);
		colors[ImGuiCol_FrameBgActive]       = ImVec4(0.53f, 0.75f, 0.82f, 1.00f);
		colors[ImGuiCol_TitleBg]             = ImVec4(0.16f, 0.16f, 0.20f, 1.00f);
		colors[ImGuiCol_TitleBgActive]       = ImVec4(0.16f, 0.16f, 0.20f, 1.00f);
		colors[ImGuiCol_TitleBgCollapsed]    = ImVec4(0.16f, 0.16f, 0.20f, 1.00f);
		colors[ImGuiCol_MenuBarBg]           = ImVec4(0.16f, 0.16f, 0.20f, 1.00f);
		colors[ImGuiCol_ScrollbarBg]         = ImVec4(0.18f, 0.20f, 0.25f, 1.00f);
		colors[ImGuiCol_ScrollbarGrab]       = ImVec4(0.23f, 0.26f, 0.32f, 0.60f);
		colors[ImGuiCol_ScrollbarGrabHovered]= ImVec4(0.23f, 0.26f, 0.32f, 1.00f);
		colors[ImGuiCol_ScrollbarGrabActive] = ImVec4(0.23f, 0.26f, 0.32f, 1.00f);
		colors[ImGuiCol_CheckMark]           = ImVec4(0.37f, 0.51f, 0.67f, 1.00f);
		colors[ImGuiCol_SliderGrab]          = ImVec4(0.51f, 0.63f, 0.76f, 1.00f);
		colors[ImGuiCol_SliderGrabActive]    = ImVec4(0.37f, 0.51f, 0.67f, 1.00f);
		colors[ImGuiCol_Button]              = ImVec4(0.18f, 0.20f, 0.25f, 1.00f);
		colors[ImGuiCol_ButtonHovered]       = ImVec4(0.51f, 0.63f, 0.76f, 1.00f);
		colors[ImGuiCol_ButtonActive]        = ImVec4(0.37f, 0.51f, 0.67f, 1.00f);
		colors[ImGuiCol_Header]              = ImVec4(0.51f, 0.63f, 0.76f, 1.00f);
		colors[ImGuiCol_HeaderHovered]       = ImVec4(0.53f, 0.75f, 0.82f, 1.00f);
		colors[ImGuiCol_HeaderActive]        = ImVec4(0.37f, 0.51f, 0.67f, 1.00f);
		colors[ImGuiCol_SeparatorHovered]    = ImVec4(0.56f, 0.74f, 0.73f, 1.00f);
		colors[ImGuiCol_SeparatorActive]     = ImVec4(0.53f, 0.75f, 0.82f, 1.00f);
		colors[ImGuiCol_ResizeGrip]          = ImVec4(0.53f, 0.75f, 0.82f, 0.86f);
		colors[ImGuiCol_ResizeGripHovered]   = ImVec4(0.61f, 0.74f, 0.87f, 1.00f);
		colors[ImGuiCol_ResizeGripActive]    = ImVec4(0.37f, 0.51f, 0.67f, 1.00f);
		colors[ImGuiCol_Tab]                 = ImVec4(0.18f, 0.20f, 0.25f, 1.00f);
		colors[ImGuiCol_TabHovered]          = ImVec4(0.22f, 0.24f, 0.31f, 1.00f);
		colors[ImGuiCol_TabActive]           = ImVec4(0.23f, 0.26f, 0.32f, 1.00f);
		colors[ImGuiCol_TabUnfocused]        = ImVec4(0.13f, 0.15f, 0.18f, 1.00f);
		colors[ImGuiCol_TabUnfocusedActive]  = ImVec4(0.17f, 0.19f, 0.23f, 1.00f);
		colors[ImGuiCol_PlotHistogram]       = ImVec4(0.56f, 0.74f, 0.73f, 1.00f);
		colors[ImGuiCol_PlotHistogramHovered]= ImVec4(0.53f, 0.75f, 0.82f, 1.00f);
		colors[ImGuiCol_TextSelectedBg]      = ImVec4(0.37f, 0.51f, 0.67f, 1.00f);
		colors[ImGuiCol_NavHighlight]        = ImVec4(0.53f, 0.75f, 0.82f, 0.86f);

		style->GrabMinSize       = 20.00f;
		style->GrabRounding      = 2.0f;
		style->FrameRounding     = 2.0f;
		style->FramePadding      = ImVec2(4.0f, 4.0f);
		style->WindowPadding     = ImVec2(2.0f, 2.0f);
		style->ChildRounding     = 2.0f;
		style->ColumnsMinSpacing = 400.0f;
		style->FrameBorderSize   = 0.0f;
		style->ChildBorderSize   = 0.0f;
		style->WindowBorderSize  = 0.0f;
		style->IndentSpacing     = 24.0f;
		style->SeparatorTextAlign= ImVec2(0.5f, 0.0f);
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
				if(ImGui::BeginMenu("Preference"))
				{
					if(ImGui::MenuItem("Themes")) {}
					if(ImGui::MenuItem("Layout")) {}
					ImGui::EndMenu();
				}
				if(ImGui::BeginMenu("View"))
				{
					if(ImGui::MenuItem("Toggle Preview View", "Alt+P")) {}
					if(ImGui::MenuItem("Toggle Debug View", "Alt+D")) {}
					ImGui::EndMenu();
				}
				ImGui::EndMenuBar();
			}
			constexpr ImGuiWindowFlags flags2= ImGuiWindowFlags_NoDecoration | ImGuiWindowFlags_NoMove |
																				 ImGuiWindowFlags_AlwaysUseWindowPadding;

			const uint32 toolBarHeight= 28;

			ImGui::PushStyleVar(ImGuiStyleVar_FramePadding, ImVec2(0, 4));
			ImGui::BeginChild(1, ImVec2(ImGui::GetWindowWidth(), toolBarHeight), true, flags2);
			int32 bounceCount= renderer.getBounceCount();

			const float32 sliderWidth= ImGui::GetContentRegionAvail().x / 6.0f;

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

			{
				std::array<float32, FrameSampleCount> frameTimeCopy= {};

				std::queue<float32> copiedQueue= m_frameTimes;

				uint32  index   = 0;
				float32 maxValue= 0.0f;

				while(!copiedQueue.empty())
				{
					frameTimeCopy[index]= copiedQueue.front();
					if(frameTimeCopy[index] > maxValue)
					{
						maxValue= frameTimeCopy[index];
					}
					copiedQueue.pop();
					index++;
				}

				float32 average= 0.0f;
				for(int32 n= 0; n < frameTimeCopy.size(); n++)
					average+= frameTimeCopy[n];
				average/= (float32)frameTimeCopy.size();

				const float32 ms = average * 1000.0f;
				const float32 fps= 1.0f / average;

				std::array<char, 32> overlay= {};
				sprintf(overlay.data(), "%.1f ms (%.1f fps)", ms, fps);
				ImGui::SameLine();
				ImGui::SetNextItemWidth(sliderWidth);
				ImGui::PlotLines("##Framerate", frameTimeCopy.data(), frameTimeCopy.size(), 0,
												 overlay.data(), 0.0f, maxValue);
			}

			const float32 progress= glm::clamp(renderer.getFrameIndex() / 1000.0f, 0.0f, 1.0f);

			ImGui::SameLine();
			ImGui::SetNextItemWidth(sliderWidth);
			std::array<char, 32> overlay= {};
			sprintf(overlay.data(), "Samples: %d/%d", renderer.getFrameIndex(), 1000);
			ImGui::ProgressBar(progress, ImVec2(0.0f, 0.0f), overlay.data());

			ImGui::EndChild();
			ImGui::PopStyleVar();

			constexpr int32 tableFlags= ImGuiTableFlags_BordersInner | ImGuiTableFlags_Resizable |
																	ImGuiTableFlags_NoHostExtendY | ImGuiTableFlags_NoHostExtendX;
			constexpr ImGuiTableRowFlags rowFlags= ImGuiTableRowFlags_None;
			if(ImGui::BeginTable("viewportContentSplit", 1, tableFlags, ImGui::GetContentRegionAvail()))
			{
				ImGui::TableNextRow(rowFlags, 100.0f);
				ImGui::TableSetColumnIndex(0);
				const float32 viewportSceneInfoSplitHeight= ImGui::GetContentRegionAvail().y * 0.6f;
				if(ImGui::BeginTable("viewportSceneInfoSplit", 3,
														 tableFlags | ImGuiTableFlags_ContextMenuInBody |
																 ImGuiTableFlags_Hideable,
														 {ImGui::GetContentRegionAvail().x, viewportSceneInfoSplitHeight}))
				{
					ImGui::TableSetupColumn("Scene", ImGuiTableColumnFlags_WidthStretch, 20.0f);
					ImGui::TableSetupColumn("Viewport", ImGuiTableColumnFlags_WidthStretch, 60.0f);
					ImGui::TableSetupColumn("Inspector", ImGuiTableColumnFlags_WidthStretch, 20.0f);

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
					ImVec2       gapRegion= {(availableRegion.x - newRegion.x) * 0.5f,
																	 (availableRegion.y - newRegion.y) * 0.5f};
					gapRegion.x-= 8.0f; // Adjustment to keep Image centered horizontally

					ImGui::Dummy({availableRegion.x, gapRegion.y});
					ImGui::Dummy({gapRegion.x, newRegion.y});

					appStateInfo.rendererSize= {newRegion.x, newRegion.y};

					ImGui::SameLine();
					ImGui::BeginChild(2, newRegion, false,
														ImGuiWindowFlags_NoDecoration | ImGuiWindowFlags_NoMove |
																ImGuiWindowFlags_NoResize |
																ImGuiWindowFlags_AlwaysUseWindowPadding);

					ImVec2 imageDisplaySize= ImGui::GetContentRegionAvail();

					const gl::GLuint renderTextureId= renderer.getTextureId();
					ImGui::Image(reinterpret_cast<ImTextureID>(renderTextureId), imageDisplaySize,
											 ImVec2(0, 1), ImVec2(1, 0));

					auto&  io= ImGui::GetIO();
					ImRect rc= ImRect(ImGui::GetItemRectMin(), ImGui::GetItemRectMax());
					ImVec2 mouseUVCoord;
					mouseUVCoord.x= (io.MousePos.x - rc.Min.x) / rc.GetSize().x;
					mouseUVCoord.y= (io.MousePos.y - rc.Min.y) / rc.GetSize().y;

					mouseUVCoord.y= 1.f - mouseUVCoord.y;

					if(io.KeyCtrl && mouseUVCoord.x >= 0.f && mouseUVCoord.y >= 0.f &&
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

					ImGuiTabBarFlags tab_bar_flags= ImGuiTabBarFlags_None;
					if(ImGui::BeginTabBar("MyTabBar", tab_bar_flags))
					{
						if(ImGui::BeginTabItem("Inspector"))
						{
							ImGui::PushFont(getSecondaryFont());
							ImGui::SeparatorText("INSPECTOR");
							ImGui::PopFont();
							displayTransformUI(appStateInfo, scene, assetManager);

							ImGui::Separator();
							ImGui::Separator();

							displayMaterialUI(appStateInfo, scene, assetManager);
							ImGui::EndTabItem();
						}
						if(ImGui::BeginTabItem("Post-Process Stack"))
						{
							ImGui::PushFont(getSecondaryFont());
							ImGui::SeparatorText("POST-PROCESS STACK");
							ImGui::PopFont();

							m_postProcessingStack.display(scene);
							ImGui::EndTabItem();
						}
						ImGui::EndTabBar();
					}

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
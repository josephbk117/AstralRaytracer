#include "WindowFramework/Window.h"

#include "Raytracer/Renderer.h"
#include "Utils/TextureManager.h"
#include "WindowFramework/Input.h"
#include "WindowFramework/UI/ImGuizmo/ImGuizmo.h"
#include "WindowFramework/UI/ImageInspect.h"

#include <nfd.hpp>

namespace AstralRaytracer
{

	Window::~Window() { glfwTerminate(); }

	const std::string& Window::getName() const { return m_name; }

	void Window::initialize()
	{
		ASTRAL_LOG_TRACE("Window creation started");
		if(!glfwInit())
		{
			ASTRAL_LOG_ERROR("Window failed to initialize");
			exit(EXIT_FAILURE);
		}

		glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);
		glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 2);
		glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

		GLFWmonitor* primaryMonitor= glfwGetPrimaryMonitor();
		// Get the video mode of the primary monitor
		const GLFWvidmode* mode= glfwGetVideoMode(primaryMonitor);
		m_glfwWindow= glfwCreateWindow(mode->width, mode->height, m_name.c_str(), nullptr, nullptr);

		if(m_glfwWindow == nullptr)
		{
			ASTRAL_LOG_ERROR("Window creation failed");
			glfwTerminate();
			exit(EXIT_FAILURE);
		}

		glfwSetWindowSizeLimits(m_glfwWindow, 720, 500, GLFW_DONT_CARE, GLFW_DONT_CARE);
		glfwSetWindowSizeCallback(m_glfwWindow, windowSizeCallback);
		glfwMakeContextCurrent(m_glfwWindow);
		glfwSwapInterval(0); // Disable v-sync

		SetWindowIcon("app_assets/icons/astralraytracer.png");

		glbinding::initialize(glfwGetProcAddress);

		gl::glViewport(0, 0, mode->width, mode->height);

		ASTRAL_LOG_TRACE("Window creation finished");

		nfdresult_t nfdInitResult= NFD::Init();
		if(nfdInitResult != nfdresult_t::NFD_OKAY)
		{
			ASTRAL_LOG_ERROR("NFD failed to initialize");
		}

		imGuiInit();
	}

	void Window::imGuiInit()
	{
		IMGUI_CHECKVERSION();
		ImGui::CreateContext();
		ImGui::StyleColorsDark();
		ImGui_ImplGlfw_InitForOpenGL(m_glfwWindow, true);
		ImGui_ImplOpenGL3_Init("#version 420 core");

		ImFontAtlas& fontAtlas= *ImGui::GetIO().Fonts;

		constexpr float32 primaryFontSize  = 16.0f;
		constexpr float32 secondaryFontSize= 22.0f;
		constexpr float32 tertiaryFontSize = 18.0f;

		m_primaryFont=
				fontAtlas.AddFontFromFileTTF("app_assets/fonts/ABeeZee-Regular.ttf", primaryFontSize);
		m_secondaryFont=
				fontAtlas.AddFontFromFileTTF("app_assets/fonts/Roboto-Regular.ttf", secondaryFontSize);
		m_tertiaryFont=
				fontAtlas.AddFontFromFileTTF("app_assets/fonts/Roboto-Regular.ttf", tertiaryFontSize);

		setDefaultTheme();
	}

	void Window::SetWindowIcon(const std::filesystem::path& iconPath)
	{
		AstralRaytracer::TextureDataRGBA iconTexData;
		AstralRaytracer::TextureManager::loadTextureDataFromFile(iconPath, iconTexData, false);

		GLFWimage image[1]{};
		image[0].width = iconTexData.getWidth();
		image[0].height= iconTexData.getHeight();
		image[0].pixels= const_cast<uint8*>(iconTexData.getTextureData().data());
		glfwSetWindowIcon(m_glfwWindow, 1, image);
	}

	void Window::clear() const { gl::glClear(gl::ClearBufferMask::GL_COLOR_BUFFER_BIT); }

	void Window::setSelectedObjectIndexFromMouseCoord(
			const glm::vec2&                   mousePos,
			AstralRaytracer::UI::AppStateInfo& appStateInfo,
			const AstralRaytracer::Renderer&   renderer,
			const AstralRaytracer::Camera&     cam,
			const AstralRaytracer::Scene&      scene
	)
	{
		HitInfo   closestHitInfo;
		glm::vec2 coOrd(
				(mousePos.x - appStateInfo.uiBounds.min.x) / appStateInfo.rendererSize.x,
				(mousePos.y - appStateInfo.uiBounds.min.y) / appStateInfo.rendererSize.y
		);
		coOrd.y= 1.0f - coOrd.y;

		const glm::vec3& rayDir= renderer.getRayDirectionFromNormalizedCoord(
				coOrd, cam.getInverseProjection(), cam.getInverseView()
		);
		renderer.findClosestHit(closestHitInfo, scene, cam.getPosition(), rayDir);

		if(closestHitInfo.isValid())
		{
			appStateInfo.selectedObjectIndex= closestHitInfo.objectIndex;
		}
	}

	void Window::processInput(
			UI::AppStateInfo& appStateInfo,
			float32           deltaTime,
			Renderer&         renderer,
			Camera&           cam,
			const Scene&      scene
	)
	{
		m_frameTimes.push(deltaTime);
		if(m_frameTimes.size() > FrameSampleCount)
		{
			m_frameTimes.pop();
		}

		//------- Transform Manipulation-------//

		if(m_appState == ApplicationState::NONE)
		{
			const bool isLeftShiftDown= Input::isKeyDown(*this, InputKey::LEFT_SHIFT);
			if(Input::isKeyDown(*this, InputKey::T))
			{
				m_TransformOperation= isLeftShiftDown ? m_TransformOperation | IMGUIZMO_NAMESPACE::TRANSLATE
																							: IMGUIZMO_NAMESPACE::TRANSLATE;
			}
			else if(Input::isKeyDown(*this, InputKey::S))
			{
				m_TransformOperation= isLeftShiftDown ? m_TransformOperation | IMGUIZMO_NAMESPACE::SCALE
																							: IMGUIZMO_NAMESPACE::SCALE;
			}
			else if(Input::isKeyDown(*this, InputKey::R))
			{
				m_TransformOperation= isLeftShiftDown ? m_TransformOperation | IMGUIZMO_NAMESPACE::ROTATE
																							: IMGUIZMO_NAMESPACE::ROTATE;
			}
		}

		//------- Object selection--------//

		const glm::vec2& mousePos= Input::getMousePosition(*this);
		if(appStateInfo.canSelectObjects && !appStateInfo.isSceneDirty &&
			 Input::isMouseButtonDown(*this, MouseButtonIndex::MOUSE_BUTTON_LEFT) &&
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

		static glm::vec2 lastMousePosition(0.0f);

		if(lastMousePosition == glm::vec2(0.0f))
		{
			lastMousePosition= Input::getMousePosition(*this);
			return;
		}

		const glm::vec2& mouseDelta= (mousePos - lastMousePosition);
		lastMousePosition          = mousePos;

		const bool forceRecalculate= cam.getResolution() != newCamRes;

		if(!Input::isMouseButtonDown(*this, MouseButtonIndex::MOUSE_BUTTON_RIGHT) && !forceRecalculate)
		{
			m_appState= ApplicationState::NONE;
			Input::setCursorMode(*this, CursorMode::NORMAL);
			return;
		}

		bool moved= false;

		if(!forceRecalculate)
		{
			Input::setCursorMode(*this, CursorMode::CAPTURED);
			m_appState= ApplicationState::SCENE_CAMERA_MANIPULATION;

			float32 moveSpeed= deltaTime;
			if(Input::isKeyDown(*this, InputKey::LEFT_SHIFT))
			{
				moveSpeed*= 5.0f;
			}

			if(Input::isKeyDown(*this, InputKey::W))
			{
				cam.moveForward(moveSpeed);
				moved= true;
			}

			if(Input::isKeyDown(*this, InputKey::S))
			{
				cam.moveForward(-moveSpeed);
				moved= true;
			}

			if(Input::isKeyDown(*this, InputKey::A))
			{
				cam.moveRight(-moveSpeed);
				moved= true;
			}

			if(Input::isKeyDown(*this, InputKey::D))
			{
				cam.moveRight(moveSpeed);
				moved= true;
			}

			if(Input::isKeyDown(*this, InputKey::Q))
			{
				cam.moveUp(moveSpeed);
				moved= true;
			}

			if(Input::isKeyDown(*this, InputKey::E))
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
		ImGuiStyle& style = ImGui::GetStyle();
		ImVec4*     colors= static_cast<ImVec4*>(style.Colors);
		ImGui::StyleColorsDark(&style); // Reset to base/dark theme
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
		colors[ImGuiCol_Button]              = ImVec4(0.51f, 0.63f, 0.76f, 1.00f);
		colors[ImGuiCol_ButtonHovered]       = ImVec4(0.37f, 0.51f, 0.67f, 1.00f);
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

		style.GrabMinSize       = 20.00f;
		style.GrabRounding      = 2.0f;
		style.FrameRounding     = 2.0f;
		style.FramePadding      = ImVec2(4.0f, 4.0f);
		style.WindowPadding     = ImVec2(0.0f, 2.0f);
		style.CellPadding       = ImVec2(4.0f, 4.0f);
		style.ChildRounding     = 2.0f;
		style.ColumnsMinSpacing = 100.0f;
		style.FrameBorderSize   = 0.0f;
		style.ChildBorderSize   = 0.0f;
		style.WindowBorderSize  = 0.0f;
		style.IndentSpacing     = 24.0f;
		style.SeparatorTextAlign= ImVec2(0.5f, 0.0f);
	}

	void Window::setWindowTitle(const AssetManager& assetManager, const Scene& activeScene)
	{
		const std::string& projectName= assetManager.getCurrentProjectName();
		const std::string& sceneName  = activeScene.getName();
		glfwSetWindowTitle(m_glfwWindow, (m_name + " | " + projectName + " | " + sceneName).c_str());
	}

	void Window::drawSampleProgress(const uint32 frameIndex)
	{
		std::array<char, 32> overlay= {};
		snprintf(overlay.data(), overlay.size(), "Samples: %d/%d", frameIndex, 1000);

		const float32 progress= glm::clamp(frameIndex / 1000.0f, 0.0f, 1.0f);
		ImGui::ProgressBar(progress, ImVec2(0.0f, 0.0f), overlay.data());
	}

	void Window::displayUI(
			UI::AppStateInfo& appStateInfo,
			Renderer&         renderer,
			Scene&            scene,
			Camera&           cam,
			AssetManager&     assetManager
	)
	{
		IMGUIZMO_NAMESPACE::BeginFrame();
		IMGUIZMO_NAMESPACE::SetOrthographic(false);

		constexpr ImGuiWindowFlags flags= ImGuiWindowFlags_NoDecoration | ImGuiWindowFlags_NoMove |
																			ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoSavedSettings |
																			ImGuiWindowFlags_MenuBar;

		const ImGuiViewport* viewport= ImGui::GetMainViewport();
		ImGui::SetNextWindowPos(viewport->WorkPos);
		ImGui::SetNextWindowSize(viewport->WorkSize);

		if(ImGui::Begin("Main Window", nullptr, flags))
		{
			drawMenuBar(scene, assetManager, appStateInfo);
			drawToolbar(renderer, assetManager, appStateInfo);

			constexpr int32 tableFlags= ImGuiTableFlags_BordersInner | ImGuiTableFlags_Resizable |
																	ImGuiTableFlags_NoHostExtendY | ImGuiTableFlags_NoHostExtendX;
			constexpr ImGuiTableRowFlags rowFlags= ImGuiTableRowFlags_None;
			if(ImGui::BeginTable("viewportContentSplit", 1, tableFlags, ImGui::GetContentRegionAvail()))
			{
				ImGui::TableNextRow(rowFlags, 100.0f);
				ImGui::TableSetColumnIndex(0);
				const float32 viewportSceneInfoSplitHeight= ImGui::GetContentRegionAvail().y * 0.6f;
				if(ImGui::BeginTable(
							 "viewportSceneInfoSplit", 3,
							 tableFlags | ImGuiTableFlags_ContextMenuInBody | ImGuiTableFlags_BordersOuter |
									 ImGuiTableFlags_Hideable,
							 { ImGui::GetContentRegionAvail().x, viewportSceneInfoSplitHeight }
					 ))
				{
					ImGui::TableSetupColumn("Scene", ImGuiTableColumnFlags_WidthStretch, 20.0f);
					ImGui::TableSetupColumn("Viewport", ImGuiTableColumnFlags_WidthStretch, 60.0f);
					ImGui::TableSetupColumn("Inspector", ImGuiTableColumnFlags_WidthStretch, 20.0f);

					ImGui::TableNextRow(rowFlags, 100.0f);
					ImGui::TableSetColumnIndex(0);

					m_sceneHierarchy.display(
							appStateInfo, scene, assetManager, *this, ImVec2(0.0f, viewportSceneInfoSplitHeight)
					);

					ImGui::TableSetColumnIndex(1);

					ImVec2 availableRegion= ImGui::GetContentRegionAvail();
					availableRegion.y     = viewportSceneInfoSplitHeight;
					availableRegion.y-= 25.0f;
					const float32 scale= glm::min(
							availableRegion.x / (float32)appStateInfo.rendererResolution.x,
							availableRegion.y / (float32)appStateInfo.rendererResolution.y
					);

					const ImVec2 newRegion= { appStateInfo.rendererResolution.x * scale,
																		appStateInfo.rendererResolution.y * scale };

					ImVec2 gapRegion= { (availableRegion.x - newRegion.x) * 0.5f,
															(availableRegion.y - newRegion.y) * 0.5f };
					gapRegion.x-= 8.0f; // Adjustment to keep Image centered horizontally

					ImGui::Dummy({ availableRegion.x, gapRegion.y });
					ImGui::Dummy({ gapRegion.x, newRegion.y });

					appStateInfo.rendererSize= { newRegion.x, newRegion.y };

					ImGui::SameLine();
					ImGui::BeginChild(
							2, newRegion, false,
							ImGuiWindowFlags_NoDecoration | ImGuiWindowFlags_NoMove | ImGuiWindowFlags_NoResize |
									ImGuiWindowFlags_AlwaysUseWindowPadding
					);

					ImVec2 imageDisplaySize= ImGui::GetContentRegionAvail();
					ImGui::Image(appStateInfo.outputTextureId, imageDisplaySize, ImVec2(0, 1), ImVec2(1, 0));

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

					appStateInfo.uiBounds.min= { ImGui::GetItemRectMin().x, ImGui::GetItemRectMin().y };
					appStateInfo.uiBounds.max= { ImGui::GetItemRectMax().x, ImGui::GetItemRectMax().y };

					IMGUIZMO_NAMESPACE::SetRect(
							ImGui::GetWindowPos().x, ImGui::GetWindowPos().y, newRegion.x, newRegion.y
					);
					IMGUIZMO_NAMESPACE::SetDrawlist();

					if(scene.hasSceneLoaded())
					{

						glm::mat4 transform= scene.m_sceneTraceables.at(appStateInfo.selectedObjectIndex)
																		 .get()
																		 ->getTransformMatrix();

						IMGUIZMO_NAMESPACE::Manipulate(
								glm::value_ptr(cam.getView()), glm::value_ptr(cam.getProjection()),
								m_TransformOperation, IMGUIZMO_NAMESPACE::LOCAL, glm::value_ptr(transform)
						);

						appStateInfo.canSelectObjects= !IMGUIZMO_NAMESPACE::IsOver();

						if(IMGUIZMO_NAMESPACE::IsUsing())
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
					ImGui::Dummy({ availableRegion.x, gapRegion.y });

					ImGui::TableSetColumnIndex(2);

					ImGuiTabBarFlags tab_bar_flags= ImGuiTabBarFlags_None;
					if(ImGui::BeginTabBar("MyTabBar", tab_bar_flags))
					{
						if(ImGui::BeginTabItem("Inspector"))
						{
							m_inspector.display(
									*this, appStateInfo, scene, assetManager,
									ImVec2(0.0f, viewportSceneInfoSplitHeight)
							);

							ImGui::EndTabItem();
						}
						if(ImGui::BeginTabItem("Post-Process Stack"))
						{
							m_postProcessingStack.display(
									scene, *this, ImVec2(0.0f, viewportSceneInfoSplitHeight)
							);
							ImGui::EndTabItem();
						}
						if(ImGui::BeginTabItem("Environment"))
						{
							m_environmentSettings.display(appStateInfo, cam, *this);
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

	void Window::drawToolbar(
			AstralRaytracer::Renderer&           renderer,
			const AstralRaytracer::AssetManager& assetManager,
			AstralRaytracer::UI::AppStateInfo&   appStateInfo
	)
	{
		constexpr ImGuiWindowFlags flags= ImGuiWindowFlags_NoDecoration | ImGuiWindowFlags_NoMove |
																			ImGuiWindowFlags_NoResize |
																			ImGuiWindowFlags_AlwaysUseWindowPadding;

		const float32 sliderWidth  = ImGui::GetContentRegionAvail().x / 7.0f;
		const uint32  toolBarHeight= 28;

		ImGui::PushStyleVar(ImGuiStyleVar_FramePadding, ImVec2(0, 4));
		ImGui::BeginChild(1, ImVec2(ImGui::GetWindowWidth(), toolBarHeight), true, flags);

		ImGui::SetNextItemWidth(sliderWidth);

		int32 bounceCount= renderer.getBounceCount();
		if(ImGui::SliderInt(
					 "##Bounce Count", &bounceCount, 1, 32, "Bounce Count:%d", ImGuiSliderFlags_AlwaysClamp
			 ))
		{
			renderer.setBounceCount(bounceCount);
			appStateInfo.isSceneDirty= true;
		}

		ImGui::SameLine();
		ImGui::SetNextItemWidth(sliderWidth);

		if(ImGui::SliderFloat(
					 "##Resolution Scale", &appStateInfo.resolutionScale, 10.0f, 100.0f,
					 "Resolution Scale:%.1f%%", ImGuiSliderFlags_AlwaysClamp
			 ))
		{
			appStateInfo.isSceneDirty= true;
		}

		ImGui::SameLine();
		ImGui::SetNextItemWidth(sliderWidth);
		if(ImGui::SliderInt2(
					 "Resolution", reinterpret_cast<int32*>(&appStateInfo.rendererResolution), 64, 1920,
					 "%dpx", ImGuiSliderFlags_AlwaysClamp
			 ))
		{
			appStateInfo.isSceneDirty= true;
		}

		ImGui::SameLine();
		ImGui::SetNextItemWidth(sliderWidth);
		drawFrameratePlot();

		ImGui::SameLine();
		ImGui::SetNextItemWidth(sliderWidth);

		const uint32 frameIndex= renderer.getFrameIndex();
		drawSampleProgress(frameIndex);

		ImGui::SameLine();
		if(ImGui::Button("screenshot", { sliderWidth, 0 }))
		{
			TextureManager::saveTextureToFile(
					renderer.getTextureData(), assetManager.getCurrentRelativePath() + "screenshot.hdr"
			);
		}

		ImGui::EndChild();
		ImGui::PopStyleVar();
	}

	void Window::handleChooseSceneDialog(
			AstralRaytracer::Scene&            scene,
			AstralRaytracer::AssetManager&     assetManager,
			const std::string&                 filePathName,
			AstralRaytracer::UI::AppStateInfo& appStateInfo
	)
	{
		if(scene.hasSceneLoaded())
		{
			scene.unload();
			assetManager.clearAndResetCachedData();
		}

		scene.deserialize(assetManager, filePathName);
		setWindowTitle(assetManager, scene);

		appStateInfo.isSceneDirty= true;
	}

	void Window::handleChooseProjectDialog(
			AstralRaytracer::Scene&            scene,
			AstralRaytracer::AssetManager&     assetManager,
			const std::string&                 filePathName,
			AstralRaytracer::UI::AppStateInfo& appStateInfo
	)
	{
		if(assetManager.loadProject(filePathName))
		{
			if(scene.hasSceneLoaded())
			{
				scene.unload();
			}

			scene.deserialize(assetManager, assetManager.getDefaultSceneAbsolutePath());
			setWindowTitle(assetManager, scene);

			appStateInfo.isSceneDirty= true;
			return;
		}
	}

	void Window::drawMenuBar(Scene& scene, AssetManager& assetManager, UI::AppStateInfo& appStateInfo)
	{
		if(ImGui::BeginMenuBar())
		{
			if(ImGui::BeginMenu("File"))
			{
				if(ImGui::MenuItem("Open Project", "Ctrl+O"))
				{
					const nfdu8filteritem_t filterItem[1]= {
						{"Project", "asproj"}
					};

					NFD::UniquePathU8 outPath;
					const nfdresult_t result= NFD::OpenDialog(outPath, filterItem, 1);

					if(result == nfdresult_t::NFD_OKAY)
					{
						const std::string filePathName(outPath.get());

						ASTRAL_LOG_INFO("Choose project {}", filePathName);
						handleChooseProjectDialog(scene, assetManager, filePathName, appStateInfo);
					}
					else if(result == nfdresult_t::NFD_ERROR)
					{
						ASTRAL_LOG_ERROR(NFD::GetError());
					}
				}
				if(ImGui::MenuItem("Open Scene", "Ctrl+O+S"))
				{
					const nfdu8filteritem_t filterItem[1]= {
						{"Scene", "ascene"}
					};

					NFD::UniquePathU8 outPath;
					const nfdresult_t result= NFD::OpenDialog(outPath, filterItem, 1);

					if(result == nfdresult_t::NFD_OKAY)
					{
						const std::string filePathName(outPath.get());

						ASTRAL_LOG_INFO("Choose scene {}", filePathName);
						handleChooseSceneDialog(scene, assetManager, filePathName, appStateInfo);
					}
					else if(result == nfdresult_t::NFD_ERROR)
					{
						ASTRAL_LOG_ERROR(NFD::GetError());
					}
				}
				ImGui::EndMenu();
			}
			if(ImGui::BeginMenu("Preference"))
			{
				if(ImGui::MenuItem("Themes")) { }
				if(ImGui::MenuItem("Layout")) { }
				ImGui::EndMenu();
			}
			if(ImGui::BeginMenu("View"))
			{
				if(ImGui::MenuItem("Toggle Preview View", "Alt+P")) { }
				if(ImGui::MenuItem("Toggle Debug View", "Alt+D")) { }
				ImGui::EndMenu();
			}
			ImGui::EndMenuBar();
		}
	}

	void Window::drawFrameratePlot()
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
		{
			average+= frameTimeCopy[n];
		}
		average/= (float32)frameTimeCopy.size();

		const float32 ms = average * 1000.0f;
		const float32 fps= 1.0f / average;

		std::array<char, 32> overlay= {};
		sprintf(overlay.data(), "%.1f ms (%.1f fps)", ms, fps);
		ImGui::PlotLines(
				"##Framerate", frameTimeCopy.data(), frameTimeCopy.size(), 0, overlay.data(), 0.0f, maxValue
		);
	}

	void Window::windowSizeCallback(GLFWwindow* window, int32 width, int32 height)
	{
		gl::glViewport(0, 0, width, height);
	}

	bool Window::shouldWindowClose() const { return glfwWindowShouldClose(m_glfwWindow); }

	void Window::shutdown() const
	{
		NFD::Quit();

		ImGui_ImplOpenGL3_Shutdown();
		ImGui_ImplGlfw_Shutdown();
		ImGui::DestroyContext();
	}

	void Window::swapBuffers() const { glfwSwapBuffers(m_glfwWindow); }

	void Window::pollEvents() const { glfwPollEvents(); }

	void Window::startUI() const
	{
		ImGui_ImplOpenGL3_NewFrame();
		ImGui_ImplGlfw_NewFrame();
		ImGui::NewFrame();
	}

	void Window::endUI() const
	{
		ImGui::Render();
		ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());
	}

	glm::u32vec2 Window::getResolution() const
	{
		int32 width;
		int32 height;
		glfwGetWindowSize(m_glfwWindow, &width, &height);
		return { width, height };
	}
} // namespace AstralRaytracer
#pragma once
#include "UI/CommonUI.h"
#include "UI/ContentBrowser.h"
#include "Utils/Common.h"

#include <queue>

namespace AstralRaytracer
{
	class Scene;
	class Camera;
	class Renderer;
	class AssetManager;
} // namespace AstralRaytracer

namespace AstralRaytracer
{
	class Window
	{
		public:
		friend class Input;

		Window(): m_name{"default"} {}
		Window(const std::string& name): m_name{name} {}
		Window(const Window&)           = delete;
		Window& operator=(const Window&)= delete;
		~Window(){};
		const std::string& getName() const;
		ImFont* const      getPrimaryFont() const { return m_primaryFont; }
		ImFont* const      getSecondaryFont() const { return m_secondaryFont; }
		ImFont* const      getTertiaryFont() const { return m_tertiaryFont; }
		bool               shouldWindowClose() const;

		void initialize();
		void processInput(UI::AppStateInfo& appStateInfo, float32 deltaTime, Renderer& renderer,
											Camera& cam, const Scene& scene);

		void displayUI(UI::AppStateInfo& appStateInfo, Renderer& renderer, Scene& scene, Camera& cam,
									 AssetManager& assetManager);
		void shutdown();
		void swapBuffers();
		void pollEvents();
		void startUI();
		void endUI();

		const std::pair<uint32, uint32>& getResolution() const { return m_resolution; };

		private:
		static Window* m_instance;

		const static uint32 FrameSampleCount= 100;

		GLFWwindow*               m_glfwWindow   = nullptr;
		ImFont*                   m_primaryFont  = nullptr;
		ImFont*                   m_secondaryFont= nullptr;
		ImFont*                   m_tertiaryFont = nullptr;
		std::string               m_name;
		std::pair<uint32, uint32> m_resolution{0, 0};
		std::queue<float32>       m_frameTimes;

		UI::ContentBrowser m_contentBrowser;

		static void windowSizeCallback(GLFWwindow* window, int32 width, int32 height);
		void        setSelectedObjectIndexFromMouseCoord(const glm::vec2&                   mousePos,
																										 AstralRaytracer::UI::AppStateInfo& appStateInfo,
																										 const AstralRaytracer::Renderer&   renderer,
																										 const AstralRaytracer::Camera&     cam,
																										 const AstralRaytracer::Scene&      scene);

		void displaySceneObjectsUI(UI::AppStateInfo& appStateInfo, const Scene& scene,
															 const AssetManager& assetManager);

		void displayMaterialUI(UI::AppStateInfo& appStateInfo, Scene& scene,
													 const AssetManager& assetManager);
		void displayTransformUI(UI::AppStateInfo& appStateInfo, Scene& scene,
														const AssetManager& assetManager);
	};
} // namespace AstralRaytracer
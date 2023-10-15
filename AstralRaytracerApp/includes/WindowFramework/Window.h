#pragma once
#include "UI/CommonUI.h"
#include "UI/ContentBrowser.h"
#include "UI/Inspector.h"
#include "UI/PostProcessingStack.h"
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

			Window(): m_name{ "default" } { }

			Window(const std::string& name): m_name{ name } { }

			Window(const Window&)           = delete;
			Window& operator=(const Window&)= delete;
			~Window(){};
			const std::string& getName() const;

			const glm::u32vec2& getMinResolution() const { return m_minResolution; };

			const glm::u32vec2& getMaxResolution() const { return m_maxResolution; };

			ImFont* const getPrimaryFont() const { return m_primaryFont; }

			ImFont* const getSecondaryFont() const { return m_secondaryFont; }

			ImFont* const getTertiaryFont() const { return m_tertiaryFont; }

			bool shouldWindowClose() const;

			void initialize();

			void SetWindowIcon(const std::filesystem::path& iconPath);

			void clear() const;
			void processInput(
					UI::AppStateInfo& appStateInfo,
					float32           deltaTime,
					Renderer&         renderer,
					Camera&           cam,
					const Scene&      scene
			);

			void setDefaultTheme() const;
			void displayUI(
					UI::AppStateInfo& appStateInfo,
					Renderer&         renderer,
					Scene&            scene,
					Camera&           cam,
					AssetManager&     assetManager
			);
			void shutdown() const;
			void swapBuffers() const;
			void pollEvents() const;
			void startUI() const;
			void endUI() const;

			const std::pair<uint32, uint32>& getResolution() const { return m_resolution; };
		private:
			static Window* m_instance;

			const static uint32 FrameSampleCount= 100;

			GLFWwindow*               m_glfwWindow   = nullptr;
			ImFont*                   m_primaryFont  = nullptr;
			ImFont*                   m_secondaryFont= nullptr;
			ImFont*                   m_tertiaryFont = nullptr;
			std::string               m_name;
			std::pair<uint32, uint32> m_resolution{ 0, 0 };
			glm::u32vec2              m_minResolution{ 720, 500 };
			glm::u32vec2              m_maxResolution{ 3840, 2160 };
			std::queue<float32>       m_frameTimes;

			UI::Inspector           m_inspector;
			UI::ContentBrowser      m_contentBrowser;
			UI::PostProcessingStack m_postProcessingStack;

			static void windowSizeCallback(GLFWwindow* window, int32 width, int32 height);
			void        setSelectedObjectIndexFromMouseCoord(
								 const glm::vec2&                   mousePos,
								 AstralRaytracer::UI::AppStateInfo& appStateInfo,
								 const AstralRaytracer::Renderer&   renderer,
								 const AstralRaytracer::Camera&     cam,
								 const AstralRaytracer::Scene&      scene
						 );

			void displaySceneObjectsUI(
					UI::AppStateInfo&   appStateInfo,
					const Scene&        scene,
					const AssetManager& assetManager
			);
	};
} // namespace AstralRaytracer
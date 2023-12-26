#pragma once
#include "UI/CommonUI.h"
#include "UI/ContentBrowser.h"
#include "UI/EnvironmentSettings.h"
#include "UI/Inspector.h"
#include "UI/PostProcessingStack.h"
#include "UI/SceneHierarchy.h"
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
	enum class ApplicationState
	{
		INVALID,
		NONE,
		LOADING_PROJECT,
		LOADING_SCENE,
		SAVING_PROJECT,
		SAVING_SCENE,
		ADDING_ASSET,
		DELETING_ASSET,
		TRANSFORM_MANIPULATION,
		SCENE_CAMERA_MANIPULATION,
		MAX= SCENE_CAMERA_MANIPULATION
	};

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
			void setProjectName(const std::string& projectName);
			void displayUI(
					UI::AppStateInfo& appStateInfo,
					Renderer&         renderer,
					Scene&            scene,
					Camera&           cam,
					AssetManager&     assetManager
			);

			void drawToolbar(
					AstralRaytracer::Renderer&         renderer,
					AstralRaytracer::UI::AppStateInfo& appStateInfo
			);

			void handleChooseSceneDialog(
					AstralRaytracer::Scene&            scene,
					AstralRaytracer::AssetManager&     assetManager,
					const std::string&                 filePathName,
					AstralRaytracer::UI::AppStateInfo& appStateInfo
			);

			void handleChooseProjectDialog(
					AstralRaytracer::Scene&            scene,
					AstralRaytracer::AssetManager&     assetManager,
					const std::string&                 filePathName,
					AstralRaytracer::UI::AppStateInfo& appStateInfo
			);

			void drawMenuBar();
			void drawFrameratePlot();
			void drawSampleProgress(const uint32 frameIndex);

			void shutdown() const;
			void swapBuffers() const;
			void pollEvents() const;
			void startUI() const;
			void endUI() const;

			glm::u32vec2 getResolution() const;
		private:
			static constexpr const char* FileDialogProjectKey= "ChooseProjectDialog";
			static constexpr const char* FileDialogSceneKey= "ChooseSceneDialog";

			const static uint32 FrameSampleCount= 100;

			GLFWwindow*         m_glfwWindow   = nullptr;
			ImFont*             m_primaryFont  = nullptr;
			ImFont*             m_secondaryFont= nullptr;
			ImFont*             m_tertiaryFont = nullptr;
			std::string         m_name;
			std::string         m_projectName;
			glm::u32vec2        m_minResolution{ 720, 500 };
			glm::u32vec2        m_maxResolution{ 3840, 2160 };
			std::queue<float32> m_frameTimes;

			ApplicationState m_appState= ApplicationState::NONE;

			IMGUIZMO_NAMESPACE::OPERATION m_TransformOperation= IMGUIZMO_NAMESPACE::TRANSLATE;

			UI::SceneHierarchy      m_sceneHierarchy;
			UI::Inspector           m_inspector;
			UI::ContentBrowser      m_contentBrowser;
			UI::PostProcessingStack m_postProcessingStack;
			UI::EnvironmentSettings m_environmentSettings;

			static void windowSizeCallback(GLFWwindow* window, int32 width, int32 height);
			void        createThumbnailCallback(IGFD_Thumbnail_Info* thumbnailInfo);
			void        destroyThumbnailCallback(IGFD_Thumbnail_Info* thumbnailInfo);
			void        setSelectedObjectIndexFromMouseCoord(
								 const glm::vec2&                   mousePos,
								 AstralRaytracer::UI::AppStateInfo& appStateInfo,
								 const AstralRaytracer::Renderer&   renderer,
								 const AstralRaytracer::Camera&     cam,
								 const AstralRaytracer::Scene&      scene
						 );
	};
} // namespace AstralRaytracer
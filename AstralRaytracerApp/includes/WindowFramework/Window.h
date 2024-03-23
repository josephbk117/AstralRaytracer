#pragma once
#include "UI/CommonUI.h"
#include "UI/ContentBrowser.h"
#include "UI/EnvironmentSettings.h"
#include "UI/ImGuizmo/ImGuizmo.h"
#include "UI/Inspector.h"
#include "UI/PostProcessingStack.h"
#include "UI/SceneHierarchy.h"
#include "Utils/Common.h"

#include <queue>

namespace AstralRaytracer
{
class Scene;
class Camera;
class BaseRenderer;
template <ArithmeticType T, uint32 componentCount> class GenericRenderer;
using RendererRGB = GenericRenderer<uint8, 3>;
using RendererRGBA = GenericRenderer<uint8, 4>;
using RendererRF = GenericRenderer<float32, 1>;
using RendererRGBF = GenericRenderer<float32, 3>;
using RendererRGBAF = GenericRenderer<float32, 4>;
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
    MAX = SCENE_CAMERA_MANIPULATION
};

class Window
{
  public:
    friend class Input;

    Window() : m_name{"default"}
    {
    }

    Window(const std::string &name) : m_name{name}
    {
    }

    Window(const Window &) = delete;
    Window &operator=(const Window &) = delete;
    ~Window();
    const std::string &getName() const;

    const glm::u32vec2 &getMinResolution() const
    {
        return m_minResolution;
    };

    const glm::u32vec2 &getMaxResolution() const
    {
        return m_maxResolution;
    };

    ImFont *const getPrimaryFont() const
    {
        return m_primaryFont;
    }

    ImFont *const getSecondaryFont() const
    {
        return m_secondaryFont;
    }

    ImFont *const getTertiaryFont() const
    {
        return m_tertiaryFont;
    }

    bool shouldWindowClose() const;

    void initialize();

    void imGuiInit();

    void SetWindowIcon(const std::filesystem::path &iconPath);

    void clear() const;
    void processInput(UI::AppStateInfo &appStateInfo, float32 deltaTime, BaseRenderer &renderer, Camera &cam,
                      const Scene &scene);

    void setDefaultTheme() const;
    void setWindowTitle(const AssetManager &assetManager, const Scene &activeScene);

    void displayUI(UI::AppStateInfo &appStateInfo, BaseRenderer &renderer, Scene &scene, Camera &cam,
                   AssetManager &assetManager);

    void drawToolbar(BaseRenderer &renderer, const AssetManager &assetManager, UI::AppStateInfo &appStateInfo);

    void handleChooseSceneDialog(Scene &scene, AssetManager &assetManager, const std::string &filePathName,
                                 UI::AppStateInfo &appStateInfo);

    void handleChooseProjectDialog(Scene &scene, AssetManager &assetManager, const std::string &filePathName,
                                   UI::AppStateInfo &appStateInfo);

    void drawMenuBar(Scene &scene, AssetManager &assetManager, UI::AppStateInfo &appStateInfo);
    void drawFrameratePlot();
    void drawSampleProgress(const uint32 frameIndex);

    void shutdown() const;
    void swapBuffers() const;
    void pollEvents() const;
    void startUI() const;
    void endUI() const;

    Resolution getResolution() const;

  private:
    static constexpr const char *FileDialogProjectKey = "ChooseProjectDialog";
    static constexpr const char *FileDialogSceneKey = "ChooseSceneDialog";

    const static uint32 FrameSampleCount = 100;

    GLFWwindow *m_glfwWindow = nullptr;
    ImFont *m_primaryFont = nullptr;
    ImFont *m_secondaryFont = nullptr;
    ImFont *m_tertiaryFont = nullptr;
    std::string m_name;
    glm::u32vec2 m_minResolution{720, 500};
    glm::u32vec2 m_maxResolution{3840, 2160};
    std::queue<float32> m_frameTimes;

    ApplicationState m_appState = ApplicationState::NONE;

    IMGUIZMO_NAMESPACE::OPERATION m_TransformOperation = IMGUIZMO_NAMESPACE::TRANSLATE;

    UI::SceneHierarchy m_sceneHierarchy;
    UI::Inspector m_inspector;
    UI::ContentBrowser m_contentBrowser;
    UI::PostProcessingStack m_postProcessingStack;
    UI::EnvironmentSettings m_environmentSettings;

    static void windowSizeCallback(GLFWwindow *window, int32 width, int32 height);
    void setSelectedObjectIndexFromMouseCoord(const glm::vec2 &mousePos, UI::AppStateInfo &appStateInfo,
                                              const BaseRenderer &renderer, const Camera &cam, const Scene &scene);
};
} // namespace AstralRaytracer
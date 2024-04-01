#pragma once
#include "CommonUI.h"
#include "FileInspector.h"
#include "Utils/AssetManager.h"

#include <efsw/efsw.hpp>
#include <filesystem>

namespace AstralRaytracer
{
namespace UI
{
class ContentBrowser : public efsw::FileWatchListener
{
  public:
    ContentBrowser(){};
    ContentBrowser(const ContentBrowser &) = delete;
    ~ContentBrowser();

    void display(AssetManager &assetManager);

    void updateProjectRootPath();

    void handleCreateFilePopupModal(AssetManager &assetManager);

    void handleFileAction(efsw::WatchID watchId, const std::string &dir, const std::string &filename,
                          efsw::Action action, std::string oldFilename = "") override;

  private:
    fs::path m_selectedFile{""};
    fs::path m_directoryForNewFile{""};
    fs::path m_currentProjectRootPath{""};

    FileInspector m_fileInspector;
    efsw::FileWatcher m_fileWatcher;
    efsw::WatchID m_watchID = 0;

    bool m_dirToWatchIsDirty = true;
    bool m_showCreateNewFilePopUp = false;

    std::unique_ptr<FileSystem::PathNode> m_projectRootNode;

    void createNewMaterial(const fs::path &path, const std::string &name, AssetManager &assetManager);
    void drawPathNode(std::unique_ptr<FileSystem::PathNode> &node);
};
} // namespace UI
} // namespace AstralRaytracer
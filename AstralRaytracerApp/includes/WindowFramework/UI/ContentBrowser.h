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
		class ContentBrowser: public efsw::FileWatchListener
		{
			public:
				ContentBrowser(){};
				ContentBrowser(const ContentBrowser&)= delete;
				~ContentBrowser();

				void display(AssetManager& assetManager);

				void updateProjectRootPath();

				void handleCreateFilePopupModal(AssetManager& assetManager);

				void handleFileAction(
						efsw::WatchID      watchId,
						const std::string& dir,
						const std::string& filename,
						efsw::Action       action,
						std::string        oldFilename= ""
				) override;
			private:
				struct PathNode
				{
						PathNode*                              parent= nullptr;
						fs::path                               pathStr;
						std::vector<std::unique_ptr<PathNode>> nodes;
						bool                                   visited= false;
				};

				fs::path m_selectedFile{ "" };
				fs::path m_directoryForNewFile{ "" };
				fs::path m_currentProjectRootPath{ "" };

				FileInspector     m_fileInspector;
				efsw::FileWatcher m_fileWatcher;
				efsw::WatchID     m_watchID= 0;

				bool m_dirToWatchIsDirty     = true;
				bool m_showCreateNewFilePopUp= false;

				std::unique_ptr<PathNode> m_projectRootNode;

				void createNewMaterial(
						const fs::path&    path,
						const std::string& name,
						AssetManager&      assetManager
				);

				void traverseDirectoryFromRoot(std::unique_ptr<PathNode>& root);
				void drawPathNode(std::unique_ptr<PathNode>& node);
		};
	} // namespace UI
} // namespace AstralRaytracer
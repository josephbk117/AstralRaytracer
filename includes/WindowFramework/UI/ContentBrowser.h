#pragma once
#include "CommonUI.h"
#include "FileInspector.h"
#include "Utils/AssetManager.h"

#include <filesystem>

namespace AstralRaytracer
{
	namespace UI
	{
		class ContentBrowser
		{
			public:
			ContentBrowser()                     = delete;
			ContentBrowser(const ContentBrowser&)= delete;
			ContentBrowser(AssetManager& assetManager);

			void            setRootContentPath(const fs::path& path) { m_rootContentPath= path; };
			const fs::path& getRootContentPath() const { return m_rootContentPath; };

			void display();

			private:
			struct PathNode
			{
				PathNode*                              parent= nullptr;
				fs::path                               pathStr;
				std::vector<std::unique_ptr<PathNode>> nodes;
				bool                                   visited= false;
			};

			fs::path      m_selectedFile{""};
			fs::path      m_directoryForNewFile{""};
			fs::path      m_rootContentPath{"/resources"};
			AssetManager& m_assetManager;
			FileInspector m_fileInspector;

			bool m_showCreateNewFilePopUp= false;

			void createNewMaterial(const fs::path& path, const std::string& name);

			void traverseDirectoryFromRoot(std::unique_ptr<PathNode>& root);
			void drawPathNode(std::unique_ptr<PathNode>& node);
		};
	} // namespace UI
} // namespace AstralRaytracer
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
			ContentBrowser(){};
			ContentBrowser(const ContentBrowser&)= delete;

			void            setRootContentPath(const fs::path& path) { m_rootContentPath= path; };
			const fs::path& getRootContentPath() const { return m_rootContentPath; };

			void display(AssetManager& assetManager);

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
			FileInspector m_fileInspector;

			bool m_showCreateNewFilePopUp= false;

			void createNewMaterial(const fs::path& path, const std::string& name,
														 AssetManager& assetManager);

			void traverseDirectoryFromRoot(std::unique_ptr<PathNode>& root);
			void drawPathNode(std::unique_ptr<PathNode>& node);
		};
	} // namespace UI
} // namespace AstralRaytracer
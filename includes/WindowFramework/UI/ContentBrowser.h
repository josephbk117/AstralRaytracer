#pragma once
#include "Utils/AssetManager.h"
#include "Utils/Common.h"

#include <filesystem>

namespace AstralRaytracer
{
	namespace UI
	{
		class ContentBrowser
		{
			public:
			ContentBrowser() = delete;
			ContentBrowser(const ContentBrowser&) = delete;
			ContentBrowser(AssetManager& assetManager);

			void setRootContentPath(const std::filesystem::path& path) { m_rootContentPath= path; };
			const std::filesystem::path& getRootContentPath() const { return m_rootContentPath; };

			void display();

			private:
			struct PathNode
			{
				PathNode*                              parent= nullptr;
				std::filesystem::path                  pathStr;
				std::vector<std::unique_ptr<PathNode>> nodes;
				bool                                   visited= false;
			};

			std::filesystem::path m_selectedFile{""};
			std::filesystem::path m_directoryForNewFile{""};
			std::filesystem::path m_rootContentPath{"/resources"};
			AssetManager&         m_assetManager;

			bool m_showCreateNewFilePopUp= false;

			void createNewMaterial(const std::filesystem::path& path, const std::string& name);

			void traverseDirectoryFromRoot(std::unique_ptr<PathNode>& root);
			void drawPathNode(std::unique_ptr<PathNode>& node);
		};
	} // namespace UI
} // namespace AstralRaytracer
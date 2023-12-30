#include "WindowFramework/UI/ContentBrowser.h"

#include <imgui.h>

namespace AstralRaytracer
{
	namespace UI
	{

		ContentBrowser::~ContentBrowser()
		{
			if(m_watchID != 0)
			{
				m_fileWatcher.removeWatch(m_watchID);
			}
		}

		void ContentBrowser::display(AssetManager& assetManager)
		{
			if(!assetManager.isProjectOpen())
			{
				return;
			}

			const std::string& currentRelativePath= assetManager.getCurrentRelativePath();
			const fs::path     projectRootPath    = fs::path(currentRelativePath).parent_path();
			if(m_currentProjectRootPath != projectRootPath || m_dirToWatchIsDirty)
			{
				if(m_watchID != 0)
				{
					m_fileWatcher.removeWatch(m_watchID);
				}

				m_currentProjectRootPath= projectRootPath;
				updateProjectRootPath();

				m_watchID= m_fileWatcher.addWatch(currentRelativePath, this, true);
				m_fileWatcher.watch();
			}

			constexpr int32 tableFlags= ImGuiTableFlags_ContextMenuInBody | ImGuiTableFlags_Hideable |
																	ImGuiTableFlags_BordersOuter | ImGuiTableFlags_Resizable |
																	ImGuiTableFlags_NoHostExtendY | ImGuiTableFlags_NoHostExtendX;

			if(ImGui::BeginTable("contentBrowserSplit", 2, tableFlags, ImGui::GetContentRegionAvail()))
			{
				ImGui::TableSetupColumn("Content Browser", ImGuiTableColumnFlags_WidthStretch, 80.0f);
				ImGui::TableSetupColumn("File Inspector", ImGuiTableColumnFlags_WidthStretch, 20.0f);

				ImGui::TableNextRow();
				ImGui::TableSetColumnIndex(0);
				ImGui::SeparatorText("CONTENT BROWSER");

				ImVec2 contentBrowserSize= ImGui::GetContentRegionAvail();
				contentBrowserSize.y-= 12.0f;

				ImGui::BeginChild("Content Browser", contentBrowserSize);

				drawPathNode(m_projectRootNode);

				handleCreateFilePopupModal(assetManager);

				ImGui::EndChild();
			}

			ImGui::TableSetColumnIndex(1);

			m_fileInspector.setFile(m_selectedFile);
			m_fileInspector.display();

			ImGui::EndTable();
		}

		void ContentBrowser::updateProjectRootPath()
		{
			m_projectRootNode         = std::make_unique<PathNode>();
			m_projectRootNode->pathStr= m_currentProjectRootPath;
			traverseDirectoryFromRoot(m_projectRootNode);

			m_dirToWatchIsDirty= false;
		}

		void ContentBrowser::handleCreateFilePopupModal(AssetManager& assetManager)
		{
			if(m_showCreateNewFilePopUp)
			{
				ImGui::OpenPopup("Create New File");
			}

			if(ImGui::BeginPopupModal(
						 "Create New File", nullptr,
						 ImGuiWindowFlags_AlwaysAutoResize | ImGuiWindowFlags_NoMove |
								 ImGuiWindowFlags_NoSavedSettings
				 ))
			{
				ImGui::Text("File Name");
				static std::array<char, 128> inputBuffer{};
				ImGui::InputText(
						"File Name Input", inputBuffer.data(), inputBuffer.size(),
						ImGuiInputTextFlags_AutoSelectAll
				);

				if(ImGui::Button("Accept"))
				{
					const fs::path newFilePath= m_directoryForNewFile.string() + "/";
					createNewMaterial(newFilePath, inputBuffer.data(), assetManager);

					inputBuffer= { 0 };

					m_showCreateNewFilePopUp= false;
					ImGui::CloseCurrentPopup();
				}
				ImGui::SameLine();
				if(ImGui::Button("Close"))
				{
					inputBuffer= { 0 };

					m_showCreateNewFilePopUp= false;
					ImGui::CloseCurrentPopup();
				}
				ImGui::EndPopup();
			}
		}

		void ContentBrowser::handleFileAction(
				efsw::WatchID      watchId,
				const std::string& dir,
				const std::string& filename,
				efsw::Action       action,
				std::string        oldFilename /*= "" */
		)
		{
			m_dirToWatchIsDirty= true;
			ASTRAL_LOG_TRACE("Update to watched directory: {}, File {}", dir, filename);
		}

		void ContentBrowser::createNewMaterial(
				const fs::path&    path,
				const std::string& name,
				AssetManager&      assetManager
		)
		{
			Material newMat;
			assetManager.saveMaterialAsset(path, name, newMat);
		}

		void ContentBrowser::traverseDirectoryFromRoot(std::unique_ptr<PathNode>& root)
		{
			PathNode* currentNode= root.get();

			while(currentNode != nullptr)
			{
				if(!currentNode->visited)
				{
					currentNode->visited= true;

					if(fs::is_directory(currentNode->pathStr))
					{
						fs::directory_iterator iter;

						std::error_code ec;
						iter= fs::directory_iterator(currentNode->pathStr, ec);

						if(ec)
						{
							ASTRAL_LOG_INFO("Failed to get directory iterator, reason: {}", ec.message());
							continue;
						}

						for(const auto& entry : iter)
						{
							const fs::file_status entryFileStatus    = fs::status(entry.path());
							const fs::perms       entryFilePermission= entryFileStatus.permissions();
							const fs::file_type   entryFileType      = entryFileStatus.type();

							const bool ownerAccessible= (entryFilePermission & std::filesystem::perms::owner_all
																					) != std::filesystem::perms::none;

							const bool isRegular= entryFileType == fs::file_type::directory ||
																		entryFileType == fs::file_type::regular;

							if(ownerAccessible && isRegular)
							{
								std::unique_ptr<PathNode> newNode= std::make_unique<PathNode>();
								newNode->parent                  = currentNode;
								newNode->pathStr                 = entry.path();

								currentNode->nodes.push_back(std::move(newNode));
							}
						}
					}
				}

				bool isEverythingVisited= true;

				if(currentNode->nodes.size() > 0)
				{
					for(uint32_t nodeIndex= 0; nodeIndex < currentNode->nodes.size(); ++nodeIndex)
					{
						const auto& node= currentNode->nodes.at(nodeIndex);
						if(!node->visited)
						{
							currentNode        = node.get();
							isEverythingVisited= false;
							break;
						}
					}
				}

				if(isEverythingVisited)
				{
					currentNode= currentNode->parent;
				}
			}
		}

		void ContentBrowser::drawPathNode(std::unique_ptr<PathNode>& node)
		{
			const std::filesystem::path& nodePath= node->pathStr;

			const bool isFile= std::filesystem::is_regular_file(nodePath);

			std::string stem;
			if(nodePath.root_path() == nodePath) // Handle special case when its root directory (D://)
			{
				stem= nodePath.generic_string();
			}
			else
			{
				stem= nodePath.stem().generic_string();
			}

			bool selected= m_selectedFile == nodePath;
			if(isFile)
			{
				if(ImGui::Selectable(stem.c_str(), selected))
				{
					m_selectedFile= nodePath;
				}
				return;
			}

			ImGui::PushID(nodePath.generic_string().c_str());
			if(ImGui::BeginPopupContextItem())
			{
				bool clicked= false;
				if(ImGui::Button("Create New File"))
				{
					m_directoryForNewFile   = node->pathStr;
					m_showCreateNewFilePopUp= true;
				}
				ImGui::EndPopup();
			}
			ImGui::PopID();

			bool nodeOpen= ImGui::TreeNode(stem.c_str());

			if(nodeOpen)
			{
				for(std::unique_ptr<PathNode>& child : node->nodes)
				{
					drawPathNode(child);
				}
				ImGui::TreePop();
			}
		}

	} // namespace UI
} // namespace AstralRaytracer
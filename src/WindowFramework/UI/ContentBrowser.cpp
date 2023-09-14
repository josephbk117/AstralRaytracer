#include "WindowFramework/UI/ContentBrowser.h"

#include "Raytracer/Material.h"

#include <imgui.h>

namespace AstralRaytracer
{
	namespace UI
	{
		namespace fs= std::filesystem;

		ContentBrowser::ContentBrowser(AssetManager& assetManager): m_assetManager(assetManager) {}

		void ContentBrowser::display()
		{
			ImGui::Text("CONTENT BROWSER");

			const fs::path resourcesPath= fs::current_path().string() + "/../../../resources";

			std::unique_ptr<PathNode> rootNode= std::make_unique<PathNode>();
			rootNode->pathStr                 = resourcesPath;

			traverseDirectoryFromRoot(rootNode);

			drawPathNode(rootNode);

			if(m_showCreateNewFilePopUp)
			{
				ImGui::OpenPopup("Create New File");
			}

			if(ImGui::BeginPopupModal("Create New File", nullptr,
																ImGuiWindowFlags_AlwaysAutoResize | ImGuiWindowFlags_NoMove |
																		ImGuiWindowFlags_NoSavedSettings))
			{
				ImGui::Text("File Name");
				char inputBuffer[128];
				std::memset(inputBuffer, 0, sizeof(inputBuffer));
				if(ImGui::InputText("File Name Input", inputBuffer, sizeof(inputBuffer),
														ImGuiInputTextFlags_EnterReturnsTrue))
				{
					const fs::path newFilePath= m_directoryForNewFile.string() + "/";
					createNewMaterial(newFilePath, inputBuffer);
				}
				if(ImGui::Button("Accept"))
				{
					m_showCreateNewFilePopUp= false;
					ImGui::CloseCurrentPopup();
				}
				ImGui::SameLine();
				if(ImGui::Button("Close"))
				{
					m_showCreateNewFilePopUp= false;
					ImGui::CloseCurrentPopup();
				}
				ImGui::EndPopup();
			}
		}

		void ContentBrowser::createNewMaterial(const std::filesystem::path& path,
																					 const std::string&           name)
		{
			Material newMat;
			m_assetManager.SaveMaterialAsset(path, name, newMat);
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
						for(const auto& entry: fs::directory_iterator(currentNode->pathStr))
						{
							std::unique_ptr<PathNode> newNode= std::make_unique<PathNode>();
							newNode->parent                  = currentNode;
							newNode->pathStr                 = entry.path();

							currentNode->nodes.push_back(std::move(newNode));
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
			const bool isFile= node->pathStr.has_extension();

			bool selected= m_selectedFile == node->pathStr;
			if(isFile)
			{
				if(ImGui::Selectable(node->pathStr.stem().string().c_str(), selected))
				{
					m_selectedFile= node->pathStr;
				}
				return;
			}

			bool nodeOpen= ImGui::TreeNode(node->pathStr.stem().string().c_str());

			ImGui::PushID(node->pathStr.string().c_str());
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

			if(nodeOpen)
			{
				for(std::unique_ptr<PathNode>& child: node->nodes)
				{
					drawPathNode(child);
				}
				ImGui::TreePop();
			}
		}

	} // namespace UI
} // namespace AstralRaytracer
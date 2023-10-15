#include "WindowFramework/UI/SceneHierarchy.h"

#include "Raytracer/Scene.h"
#include "WindowFramework/Window.h"

namespace AstralRaytracer
{
	namespace UI
	{
		void SceneHierarchy::display(
				AppStateInfo& appStateInfo,
				Scene&        scene,
				AssetManager& assetManager,
				const Window& window,
				ImVec2        availableRegion
		)
		{
			ImGui::PushFont(window.getSecondaryFont());
			ImGui::SeparatorText("SCENE");
			ImGui::PopFont();

			availableRegion.y-= 54.0f;
			ImGui::BeginChild(
					"SceneHierarchy", availableRegion, false, ImGuiWindowFlags_AlwaysAutoResize
			);

			for(uint32 objIndex= 0; objIndex < scene.m_sceneTraceables.size(); ++objIndex)
			{
				bool isSelected= objIndex == appStateInfo.selectedObjectIndex;
				if(ImGui::Selectable(
							 assetManager.getNameAndPathOfTraceable(objIndex).value().assetName.c_str(),
							 &isSelected
					 ))
				{
					appStateInfo.selectedObjectIndex= objIndex;
				}
			}

			ImGui::EndChild();
		}
	} // namespace UI
} // namespace AstralRaytracer
#include "WindowFramework/UI/Inspector.h"

#include "Raytracer/Scene.h"
#include "WindowFramework/Window.h"

namespace AstralRaytracer
{
	namespace UI
	{
		void Inspector::display(const Window& window, AppStateInfo& appStateInfo, Scene& scene,
														const AssetManager& assetManager, ImVec2 availableRegion)
		{
			ImGui::BeginChild("PostProcessingStack", availableRegion, false,
												ImGuiWindowFlags_AlwaysAutoResize);
			ImGui::PushFont(window.getSecondaryFont());
			ImGui::SeparatorText("INSPECTOR");
			ImGui::PopFont();
			displayTransformUI(window, appStateInfo, scene, assetManager);

			ImGui::Separator();
			ImGui::Separator();

			displayMaterialUI(window, appStateInfo, scene, assetManager);

			ImGui::EndChild();
		}

		void Inspector::displayMaterialUI(const Window& window, AppStateInfo& appStateInfo,
																			Scene& scene, const AssetManager& assetManager)
		{
			ImGui::PushFont(window.getTertiaryFont());
			ImGui::TextUnformatted("Material");
			ImGui::PopFont();

			if(!scene.hasSceneLoaded())
			{
				return;
			}
			const uint32 textureCount= scene.m_textures.size();

			const uint32 matIndex=
					scene.m_sceneTraceables.at(appStateInfo.selectedObjectIndex).get()->getMaterialIndex();
			AstralRaytracer::Material& mat= scene.m_materials.at(matIndex);
			ImGui::TextUnformatted(
					assetManager.getNameAndPathOfMaterial(matIndex).value().assetName.c_str());

			if(ImGui::ColorEdit3("Albedo", reinterpret_cast<float32*>(&mat.albedo)))
			{
				appStateInfo.isSceneDirty= true;
			}
			if(ImGui::SliderInt("Texture", reinterpret_cast<int32*>(&mat.texture), 0, textureCount - 1,
													"%d", ImGuiSliderFlags_AlwaysClamp))
			{
				appStateInfo.isSceneDirty= true;
			}
			if(ImGui::ColorEdit3("Emission", reinterpret_cast<float32*>(&mat.emission)))
			{
				appStateInfo.isSceneDirty= true;
			}
			if(ImGui::SliderFloat("Emission Strength", &mat.emissionStrength, 0.0f, 1000.0f, "%.2f",
														ImGuiSliderFlags_AlwaysClamp))
			{
				appStateInfo.isSceneDirty= true;
			}
			if(ImGui::SliderFloat("Roughness", &mat.roughness, 0.0f, 1.0f, "%.2f",
														ImGuiSliderFlags_AlwaysClamp))
			{
				appStateInfo.isSceneDirty= true;
			}
			ImGui::Separator();
		}

		void Inspector::displayTransformUI(const Window& window, AppStateInfo& appStateInfo,
																			 Scene& scene, const AssetManager& assetManager)
		{
			ImGui::PushFont(window.getTertiaryFont());
			ImGui::TextUnformatted("Transform");
			ImGui::PopFont();
			if(!scene.hasSceneLoaded())
			{
				return;
			}

			if(AstralRaytracer::UI::displayTransform(
						 *scene.m_sceneTraceables[appStateInfo.selectedObjectIndex]))
			{
				appStateInfo.isSceneDirty= true;
			}
		}
	} // namespace UI
} // namespace AstralRaytracer
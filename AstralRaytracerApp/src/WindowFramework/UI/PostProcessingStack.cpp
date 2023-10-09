#include "WindowFramework/UI/PostProcessingStack.h"

#include "Raytracer/PostProcessing/BilateralFilterPostProcess.h"
#include "Raytracer/PostProcessing/DesaturatePostProcess.h"
#include "Raytracer/PostProcessing/GammaCorrectionPostProcess.h"
#include "Raytracer/PostProcessing/LuminanceThresholdPostProcess.h"
#include "Raytracer/PostProcessing/PostProcessing.h"
#include "Raytracer/Scene.h"
#include "Utils/Common.h"
#include "WindowFramework/Window.h"

namespace AstralRaytracer
{
	namespace UI
	{
		void PostProcessingStack::display(Scene& scene, const Window& window, ImVec2 availableRegion)
		{
			ImGui::PushFont(window.getSecondaryFont());
			ImGui::SeparatorText("POST-PROCESS STACK");
			ImGui::PopFont();

			availableRegion.y-= 100.0f;
			static bool addNewPostProcess= false;
			if(ImGui::Button("Add", ImVec2(ImGui::GetContentRegionAvail().x, 0.0f)))
			{
				addNewPostProcess= true;
			}

			if(addNewPostProcess)
			{
				ImGui::OpenPopup("Add New Post Process");
			}

			if(ImGui::BeginPopupModal("Add New Post Process", &addNewPostProcess,
																ImGuiWindowFlags_AlwaysAutoResize |
																		ImGuiWindowFlags_AlwaysUseWindowPadding |
																		ImGuiWindowFlags_NoMove | ImGuiWindowFlags_NoSavedSettings))
			{

				const std::array<std::string, 4> items= {"Bilateral Filter", "Desaturate",
																								 "Gamma Correction", "Luminance Threshold"};
				if(ImGui::BeginCombo("##Post Processing Effect", items[m_selectedItem].c_str(),
														 ImGuiComboFlags_None))
				{
					for(int i= 0; i < items.size(); i++)
					{
						bool isSelected= (m_selectedItem == i);
						if(ImGui::Selectable(items[i].c_str(), isSelected))
						{
							m_selectedItem= i;
						}
						if(isSelected)
						{
							ImGui::SetItemDefaultFocus();
						}
					}
					ImGui::EndCombo();
				}

				float32 buttonWidth= (ImGui::GetWindowContentRegionWidth() * 0.5f) - 4.0f;

				if(ImGui::Button("Select", ImVec2(buttonWidth, 0.0f)))
				{
					addPostProcessing(scene, static_cast<PostProcessingType>(m_selectedItem + 1));
					addNewPostProcess= false;
				}
				ImGui::SameLine();
				if(ImGui::Button("Cancel", ImVec2(buttonWidth, 0.0f)))
				{
					addNewPostProcess= false;
				}

				ImGui::EndPopup();
			}

			ImGui::BeginChild("PostProcessingStack", availableRegion, false,
												ImGuiWindowFlags_AlwaysAutoResize);

			std::vector<std::unique_ptr<PostProcessing>>& postProcessStack= scene.m_postProcessingStack;

			std::vector<int32> indicesToDelete;

			for(uint32 index= 0; index < postProcessStack.size(); ++index)
			{
				const std::string& postProcessName= postProcessStack[index].get()->getName();

				bool showHeader= true;
				ImGui::PushID(index);
				if(ImGui::CollapsingHeader(postProcessName.c_str(), &showHeader))
				{
					const std::unordered_map<std::string, UniformData>& uniformData=
							postProcessStack[index].get()->getShader().getUniformData();

					for(auto it= uniformData.begin(); it != uniformData.end(); ++it)
					{
						const std::string& uniformName= it->first;
						const UniformData& uniformData= it->second;
						PostProcessing&    postProcess= *postProcessStack[index].get();

						std::any uniformValue= postProcess.getUniform(uniformName);

						const size_t typeHash= uniformValue.type().hash_code();

						using namespace AstralRaytracer;

						if(typeHash == Float32Hash)
						{
							float32 value= std::any_cast<float32>(uniformValue);
							if(ImGui::SliderFloat(uniformName.c_str(), &value, uniformData.min, uniformData.max))
							{
								postProcess.setUniform(uniformName, value);
							}
						}
					}
				}

				ImGui::PopID();

				if(!showHeader)
				{
					indicesToDelete.push_back(index);
				}
			}

			// Now delete the marked elements in reverse order
			for(auto it= indicesToDelete.rbegin(); it != indicesToDelete.rend(); ++it)
			{
				postProcessStack.erase(postProcessStack.begin() + *it);
			}

			ImGui::EndChild();
		}

		void PostProcessingStack::addPostProcessing(Scene& scene, PostProcessingType type)
		{
			switch(type)
			{
				case PostProcessingType::BILATERAL_FILTER:
					scene.addPostProcessing(std::make_unique<BilateralFilterPostProcess>());
					return;
				case PostProcessingType::DESATURATE:
					scene.addPostProcessing(std::make_unique<DesaturatePostProcessing>());
					return;
				case PostProcessingType::GAMMA_CORRECTION:
					scene.addPostProcessing(std::make_unique<GammaCorrectionPostProcessing>());
					return;
				case PostProcessingType::LUMINANCE_THRESHOLD:
					scene.addPostProcessing(std::make_unique<LuminanceThresholdPostProcessing>());
					return;
				default: assertm(false, "Invalid Post Processing Type");
			}
		}
	} // namespace UI

} // namespace AstralRaytracer
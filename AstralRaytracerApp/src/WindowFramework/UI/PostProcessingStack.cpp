#include "WindowFramework/UI/PostProcessingStack.h"

#include "Raytracer/PostProcessing/PostProcessing.h"
#include "Raytracer/Scene.h"
#include "WindowFramework/Window.h"

namespace AstralRaytracer
{
	namespace UI
	{
		void PostProcessingStack::display(Scene& scene, const Window& window)
		{
			ImGui::BeginChild("PostProcessingStack", ImGui::GetContentRegionAvail());
			const std::vector<std::unique_ptr<PostProcessing>>& postProcessStack=
					scene.m_postProcessingStack;

			for(uint32 index= 0; index < postProcessStack.size(); ++index)
			{
				const std::string& postProcessName= postProcessStack[index].get()->getName();

				if(ImGui::TreeNode(postProcessName.c_str()))
				{
					const std::unordered_map<std::string, UniformData>& uniformData=
							postProcessStack[index].get()->getShader().getUniformData();

					for(auto it= uniformData.begin(); it != uniformData.end(); ++it)
					{
						const std::string&    uniformName= it->first;
						PostProcessing& postProcess= *postProcessStack[index].get();

						std::any uniformValue= postProcess.getUniform(uniformName);

						const size_t typeHash= uniformValue.type().hash_code();

						using namespace AstralRaytracer;

						if(typeHash == Float32Hash)
						{
							float32 value= std::any_cast<float32>(uniformValue);
							if(ImGui::SliderFloat(uniformName.c_str(), &value, 0.0f, 100.0f))
							{
								postProcess.setUniform(uniformName, value);
							}
						}
					}

					ImGui::TreePop();
				}
			}

			ImGui::EndChild();
		}
	} // namespace UI

} // namespace AstralRaytracer
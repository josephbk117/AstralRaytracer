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
			std::vector<std::unique_ptr<PostProcessing>>& postProcessStack= scene.m_postProcessingStack;

			std::vector<int32> indicesToDelete;

			for(uint32 index= 0; index < postProcessStack.size(); ++index)
			{
				const std::string& postProcessName= postProcessStack[index].get()->getName();

				bool showHeader= true;
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
	} // namespace UI

} // namespace AstralRaytracer
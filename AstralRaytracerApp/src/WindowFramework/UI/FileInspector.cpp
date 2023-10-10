#include "WindowFramework/UI/FileInspector.h"

#include <imgui.h>

namespace AstralRaytracer
{
	namespace UI
	{
		void FileInspector::display()
		{
			const FileType fileType= getFileTypeFromFilePath(m_currentFIle);

			ImGui::SeparatorText("FILE INSPECTOR");

			if(!m_currentFIle.empty())
			{
				ImGui::TextUnformatted(m_currentFIle.filename().string().c_str());
				ImGui::SameLine();

				std::string typeNameStr= "";

				if(fileType == FileType::ASSET)
				{
					typeNameStr= toString(getAssetTypeFromFilePath(m_currentFIle));
				}
				else if(fileType == FileType::RESOURCE)
				{
					typeNameStr= toString(getResourceTypeFromFilePath(m_currentFIle));
				}

				ImGui::TextUnformatted(typeNameStr.c_str());
			}
		}

	} // namespace UI
} // namespace AstralRaytracer
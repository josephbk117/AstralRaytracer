#include "WindowFramework/UI/FileInspector.h"
namespace AstralRaytracer
{
	namespace UI
	{
		void FileInspector::display()
		{
			const FileType fileType= getFileTypeFromFilePath(m_currentFIle);

			ImGui::SeparatorText("FILE INSPECTOR");
			ImGui::Text(m_currentFIle.filename().string().c_str());
			ImGui::SameLine();

			if(fileType == FileType::ASSET)
			{
				ImGui::Text(toString(getAssetTypeFromFilePath(m_currentFIle)).c_str());
			}
			else if(fileType == FileType::RESOURCE) 
			{
				ImGui::Text(toString(getResourceTypeFromFilePath(m_currentFIle)).c_str());
			}
		}

	} // namespace UI
} // namespace AstralRaytracer
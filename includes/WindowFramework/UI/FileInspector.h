#pragma once
#include "CommonUI.h"
namespace AstralRaytracer
{
	namespace UI
	{
		class FileInspector
		{
			public:
			void setFile(const fs::path& filePath) { m_currentFIle= filePath; }
			void display();

			private:
			fs::path m_currentFIle;
		};

	} // namespace UI
} // namespace AstralRaytracer
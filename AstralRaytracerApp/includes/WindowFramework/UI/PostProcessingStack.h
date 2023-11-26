#pragma once

#include "CommonUI.h"

namespace AstralRaytracer
{
	class Scene;
	class Window;
} // namespace AstralRaytracer

namespace AstralRaytracer
{
	namespace UI
	{
		class PostProcessingStack
		{
			public:
				void display(Scene& scene, const Window& window, ImVec2 availableRegion);
			private:
				int32 m_selectedItem= 0;
				enum class PostProcessingType
				{
					INVALID,
					MIN,
					BILATERAL_FILTER= MIN,
					DESATURATE,
					GAMMA_CORRECTION,
					LUMINANCE_THRESHOLD,
					HORIZONTAl_GAUSS_BLUR,
					VERTICAL_GAUSS_BLUR,
					MAX= VERTICAL_GAUSS_BLUR
				};

				void addPostProcessing(Scene& scene, PostProcessingType type);
		};
	} // namespace UI
} // namespace AstralRaytracer
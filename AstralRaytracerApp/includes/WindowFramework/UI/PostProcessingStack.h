#pragma once

#include "CommonUI.h"

namespace AstralRaytracer
{
	class Scene;
	class Window;
}

namespace AstralRaytracer
{
	namespace UI
	{
		class PostProcessingStack
		{
			public:
			void display(Scene& scene, const Window& window);

			private:
			enum class PostProcessingType
			{
				INVALID,
				MIN,
				BILATERAL_FILTER = MIN,
				DESATURATE,
				GAMMA_CORRECTION,
				LUMINANCE_THRESHOLD,
				MAX = LUMINANCE_THRESHOLD
			};

			void addPostProcessing(Scene& scene, PostProcessingType type);
		};
	} // namespace UI
} // namespace AstralRaytracer
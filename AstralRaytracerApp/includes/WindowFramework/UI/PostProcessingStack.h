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
		};
	} // namespace UI
} // namespace AstralRaytracer
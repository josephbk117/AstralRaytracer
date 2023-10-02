#pragma once

#include "CommonUI.h"

namespace AstralRaytracer
{
	class Scene;
}

namespace AstralRaytracer
{
	namespace UI
	{
		class PostProcessingStack
		{
			public:
			void display(Scene& scene);

			private:
		};
	} // namespace UI
} // namespace AstralRaytracer
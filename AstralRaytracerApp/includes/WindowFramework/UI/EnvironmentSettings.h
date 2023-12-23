#pragma once

#include "CommonUI.h"

namespace AstralRaytracer
{
	class Window;
	class Camera;
} // namespace AstralRaytracer

namespace AstralRaytracer
{
	namespace UI
	{
		class EnvironmentSettings
		{
			public:
				void display(AppStateInfo& appStateInfo, Camera& cam, const Window& window);
			private:
		};
	} // namespace UI
} // namespace AstralRaytracer
#pragma once

#include "CommonUI.h"

namespace AstralRaytracer
{
	class Scene;
	class Window;
	class AssetManager;
} // namespace AstralRaytracer

namespace AstralRaytracer
{
	namespace UI
	{
		class SceneHierarchy
		{
			public:
				void display(
						AppStateInfo& appStateInfo,
						Scene&        scene,
						AssetManager& assetManager,
						const Window& window,
						ImVec2        availableRegion
				);
			private:
		};
	} // namespace UI
} // namespace AstralRaytracer
#pragma once
#include "CommonUI.h"

namespace AstralRaytracer
{
	class AssetManager;
	class Scene;
	class Window;
} // namespace AstralRaytracer

namespace AstralRaytracer
{
	namespace UI
	{
		class Inspector
		{
			public:
			void display(const Window& window, AppStateInfo& appStateInfo, Scene& scene,
									 const AssetManager& assetManager, ImVec2 availableRegion);

			private:
			void displayMaterialUI(const Window& window, AppStateInfo& appStateInfo, Scene& scene,
														 const AssetManager& assetManager);
			void displayTransformUI(const Window& window, AppStateInfo& appStateInfo, Scene& scene,
															const AssetManager& assetManager);
		};
	} // namespace UI
} // namespace AstralRaytracer
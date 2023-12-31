#pragma once
#include "PostProcessing.h"

namespace AstralRaytracer
{
	class BilateralFilterPostProcess: public PostProcessing
	{
		public:
			const std::string& getName() const override;

			void init() override;

			void processImage(
					DrawingPanel&        drawPanel,
					const RenderTexture& renderTexture,
					gl::GLuint           imageTexture
			) const override;
		protected:
			const char* const getFragmentShaderSrcCode() const override;
	};
} // namespace AstralRaytracer
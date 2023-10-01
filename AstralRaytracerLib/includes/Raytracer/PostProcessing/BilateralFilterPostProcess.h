#pragma once
#include "PostProcessing.h"
namespace AstralRaytracer
{
	class BilateralFilterPostProcess: public PostProcessing
	{
		public:
		void init() override;

		void processImage(DrawingPanel& drawPanel, const RenderTexture& renderTexture,
											gl::GLuint imageTexture) override;

		protected:
		const char* const getFragmentShaderSrcCode() const override;
	};
}
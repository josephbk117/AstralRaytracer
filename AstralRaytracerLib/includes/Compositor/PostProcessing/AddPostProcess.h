#pragma once
#include "PostProcessing.h"

namespace AstralRaytracer
{
	class AddPostProcessing: public PostProcessing
	{
		public:
			constexpr std::string_view getName() const override { return "Add"; }

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
#pragma once
#include "Utils/Common.h"

#include <glbinding/gl/gl.h>

namespace AstralRaytracer
{
	class RenderTexture
	{
		public:
			RenderTexture()= default;
			~RenderTexture();

			void init(const Resolution& size);
			[[nodiscard]]
			gl::GLuint getFBO() const;
			[[nodiscard]]
			TextureId getTexture() const;
			void      bind() const;
			void      unbind() const;
			[[nodiscard]]
			const Resolution& getSize() const;
			void              resize(const Resolution& resolution);
		private:
			Resolution m_resolution{ 1, 1 };
			gl::GLuint m_framebuffer= 0;
			gl::GLuint m_texture    = 0;
	};
} // namespace AstralRaytracer
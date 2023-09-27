#pragma once
#include "Utils/Common.h"

#include <glbinding/gl/gl.h>

namespace AstralRaytracer
{
	class RenderTexture
	{
		public:
		RenderTexture() = default;
		~RenderTexture();

		void       init(const glm::u32vec2& size);
		gl::GLuint getTexture() const;
		void       bind();
		void       unbind();
		void       resize(const glm::u32vec2& size);

		private:
		glm::u32vec2 m_size{1, 1};
		gl::GLuint   m_framebuffer= 0;
		gl::GLuint   m_texture    = 0;
	};
} // namespace AstralRaytracer
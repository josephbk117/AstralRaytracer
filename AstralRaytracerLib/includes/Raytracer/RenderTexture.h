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

		void         init(const glm::u32vec2& size);
		gl::GLuint   getFBO() const;
		gl::GLuint   getProcessedTexture() const;
		void         setProcessedTextureIndex(uint32 index);
		gl::GLuint   getTexture(uint32 index) const;
		void         setTextureIndexToBind(uint32 index);
		void         bind() const;
		void         unbind() const;
		glm::u32vec2 getSize() const;
		void         resize(const glm::u32vec2& size);

		private:
		glm::u32vec2 m_size{1, 1};
		gl::GLuint   m_framebuffer= 0;

		// These textures alternate each time bind is called
		std::array<gl::GLuint, 2> m_pingPongTextures     = {0};
		uint32                    m_processedTextureIndex= 0;
		uint32                    m_textureIndexToBind   = 0;
	};
} // namespace AstralRaytracer
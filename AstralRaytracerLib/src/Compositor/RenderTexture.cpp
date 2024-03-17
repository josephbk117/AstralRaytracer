#include "Compositor/RenderTexture.h"

namespace AstralRaytracer
{
RenderTexture::~RenderTexture()
{
    gl::glDeleteFramebuffers(1, &m_framebuffer);
    gl::glDeleteTextures(1, &m_texture);
}

void RenderTexture::init(const Resolution &resolution)
{
    m_resolution = resolution;
    // Step 2: Create FBO
    gl::glGenFramebuffers(1, &m_framebuffer);
    gl::glBindFramebuffer(gl::GL_FRAMEBUFFER, m_framebuffer);

    // Step 3: Create Texture Attachment
    gl::glGenTextures(1, &m_texture);

    gl::glBindTexture(gl::GL_TEXTURE_2D, m_texture);
    gl::glTexImage2D(gl::GL_TEXTURE_2D, 0, gl::GL_RGB32F, m_resolution.x, m_resolution.y, 0, gl::GL_RGB, gl::GL_FLOAT,
                     nullptr);

    gl::glTexParameteri(gl::GL_TEXTURE_2D, gl::GL_TEXTURE_WRAP_S, gl::GL_CLAMP_TO_EDGE);
    gl::glTexParameteri(gl::GL_TEXTURE_2D, gl::GL_TEXTURE_WRAP_T, gl::GL_CLAMP_TO_EDGE);
    gl::glTexParameteri(gl::GL_TEXTURE_2D, gl::GL_TEXTURE_MIN_FILTER, gl::GL_NEAREST);
    gl::glTexParameteri(gl::GL_TEXTURE_2D, gl::GL_TEXTURE_MAG_FILTER, gl::GL_NEAREST);

    gl::glFramebufferTexture2D(gl::GL_FRAMEBUFFER, gl::GL_COLOR_ATTACHMENT0, gl::GL_TEXTURE_2D, m_texture, 0);

    // Step 5: Check Completeness
    if (gl::glCheckFramebufferStatus(gl::GL_FRAMEBUFFER) != gl::GL_FRAMEBUFFER_COMPLETE)
    {
        ASTRAL_LOG_WARN("Failed to create framebuffer");
    }
    else
    {
        ASTRAL_LOG_TRACE("Successfully created framebuffer");
    }

    // Step 6: Switch back to default framebuffer

    gl::glBindTexture(gl::GL_TEXTURE_2D, 0);
    gl::glBindFramebuffer(gl::GL_FRAMEBUFFER, 0);
}

gl::GLuint RenderTexture::getFBO() const
{
    return m_framebuffer;
}

TextureId RenderTexture::getTexture() const
{
    return m_texture;
}

void RenderTexture::bind() const
{
    gl::glBindFramebuffer(gl::GL_FRAMEBUFFER, m_framebuffer);
    gl::glViewport(0, 0, m_resolution.x, m_resolution.y);
}

void RenderTexture::unbind() const
{
    gl::glBindFramebuffer(gl::GL_FRAMEBUFFER, 0);
}

const Resolution &RenderTexture::getSize() const
{
    return m_resolution;
}

void RenderTexture::resize(const Resolution &resolution)
{
    if (m_resolution != resolution)
    {
        m_resolution = resolution;
        // Resize Texture Attachment

        gl::glBindTexture(gl::GL_TEXTURE_2D, m_texture);
        gl::glTexImage2D(gl::GL_TEXTURE_2D, 0, gl::GL_RGB32F, m_resolution.x, m_resolution.y, 0, gl::GL_RGB,
                         gl::GL_FLOAT, nullptr);
        gl::glBindTexture(gl::GL_TEXTURE_2D, 0);
    }
}
} // namespace AstralRaytracer
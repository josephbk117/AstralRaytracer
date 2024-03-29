#include "Compositor/DrawingPanel.h"

#include <glbinding/gl/gl.h>

DrawingPanel::DrawingPanel()
{
    m_textureID.fill(0);
}

DrawingPanel::~DrawingPanel()
{
    if (m_vboID != 0)
    {
        gl::glDeleteBuffers(1, &m_vboID);
    }
    if (m_vaoID != 0)
    {
        gl::glDeleteVertexArrays(1, &m_vaoID);
    }
}

void DrawingPanel::init(float32 width, float32 height)
{
    m_width = width;
    m_height = height;
    if (m_vaoID == 0)
    {
        gl::glGenVertexArrays(1, &m_vaoID);
    }
    if (m_vboID == 0)
    {
        gl::glGenBuffers(1, &m_vboID);
    }

    std::array<float32, 24> vertexData = {};
    vertexData[0] = width;
    vertexData[1] = height;

    vertexData[2] = 1.0f;
    vertexData[3] = 1.0f;

    vertexData[4] = -width;
    vertexData[5] = height;

    vertexData[6] = 0.0f;
    vertexData[7] = 1.0f;

    vertexData[8] = -width;
    vertexData[9] = -height;

    vertexData[10] = 0.0f;
    vertexData[11] = 0.0f;

    vertexData[12] = -width;
    vertexData[13] = -height;

    vertexData[14] = 0.0f;
    vertexData[15] = 0.0f;

    vertexData[16] = width;
    vertexData[17] = -height;

    vertexData[18] = 1.0f;
    vertexData[19] = 0;

    vertexData[20] = width;
    vertexData[21] = height;

    vertexData[22] = 1.0f;
    vertexData[23] = 1.0f;

    gl::glBindVertexArray(m_vaoID);
    gl::glBindBuffer(gl::GL_ARRAY_BUFFER, m_vboID);

    gl::glEnableVertexAttribArray(0);
    gl::glVertexAttribPointer(0, 2, gl::GL_FLOAT, gl::GL_FALSE, 4 * sizeof(float32), (void *)0);
    gl::glEnableVertexAttribArray(1);
    gl::glVertexAttribPointer(1, 2, gl::GL_FLOAT, gl::GL_FALSE, 4 * sizeof(float32), (void *)(2 * sizeof(float32)));
    gl::glBufferData(gl::GL_ARRAY_BUFFER, sizeof(vertexData), vertexData.data(), gl::GL_STATIC_DRAW);
    gl::glBindBuffer(gl::GL_ARRAY_BUFFER, 0);
    gl::glBindVertexArray(0);
}

void DrawingPanel::setTextureID(TextureId textureID, uint32 index)
{
    m_textureID[index] = textureID;
}

TextureId DrawingPanel::getTextureID(uint32 index) const noexcept
{
    return m_textureID[index];
}

void DrawingPanel::draw() const noexcept
{
    gl::glActiveTexture(gl::GL_TEXTURE0);
    gl::glBindTexture(gl::GL_TEXTURE_2D, m_textureID[0]);
    if (m_textureID[1] != 0)
    {
        gl::glActiveTexture(gl::GL_TEXTURE1);
        gl::glBindTexture(gl::GL_TEXTURE_2D, m_textureID[1]);
    }

    gl::glBindVertexArray(m_vaoID);
    gl::glDrawArrays(gl::GL_TRIANGLES, 0, 6);
    gl::glBindVertexArray(0);
}
#include "Raytracer/DrawingPanel.h"

#include <glbinding/gl/gl.h>

DrawingPanel::DrawingPanel()
{
	m_vboID    = 0;
	m_vaoID    = 0;
	m_width    = 0;
	m_height   = 0;
	m_textureID= 0;
}

DrawingPanel::~DrawingPanel()
{
	if(m_vboID != 0)
		gl::glDeleteBuffers(1, &m_vboID);
	if(m_vaoID != 0)
		gl::glDeleteVertexArrays(1, &m_vaoID);
}

void DrawingPanel::init(float32 width, float32 height)
{
	width*= 2;
	height*= 2;
	m_width = width;
	m_height= height;
	if(m_vaoID == 0)
		gl::glGenVertexArrays(1, &m_vaoID);
	if(m_vboID == 0)
		gl::glGenBuffers(1, &m_vboID);

	std::array<float32, 24> vertexData;
	vertexData[0]= width * 0.5f;
	vertexData[1]= height * 0.5f;

	vertexData[2]= 1.0f;
	vertexData[3]= 1.0f;

	vertexData[4]= -width * 0.5f;
	vertexData[5]= height * 0.5f;

	vertexData[6]= 0.0f;
	vertexData[7]= 1.0f;

	vertexData[8]= -width * 0.5f;
	vertexData[9]= -height * 0.5f;

	vertexData[10]= 0.0f;
	vertexData[11]= 0.0f;

	vertexData[12]= -width * 0.5f;
	vertexData[13]= -height * 0.5f;

	vertexData[14]= 0.0f;
	vertexData[15]= 0.0f;

	vertexData[16]= width * 0.5f;
	vertexData[17]= -height * 0.5f;

	vertexData[18]= 1.0f;
	vertexData[19]= 0;

	vertexData[20]= width * 0.5f;
	vertexData[21]= height * 0.5f;

	vertexData[22]= 1.0f;
	vertexData[23]= 1.0f;

	gl::glBindVertexArray(m_vaoID);
	gl::glBindBuffer(gl::GL_ARRAY_BUFFER, m_vboID);

	gl::glEnableVertexAttribArray(0);
	gl::glVertexAttribPointer(0, 2, gl::GL_FLOAT, gl::GL_FALSE, 4 * sizeof(float32), (void*)0);
	gl::glEnableVertexAttribArray(1);
	gl::glVertexAttribPointer(1, 2, gl::GL_FLOAT, gl::GL_FALSE, 4 * sizeof(float32),
														(void*)(2 * sizeof(float32)));
	gl::glBufferData(gl::GL_ARRAY_BUFFER, sizeof(vertexData), vertexData.data(), gl::GL_STATIC_DRAW);
	gl::glBindBuffer(gl::GL_ARRAY_BUFFER, 0);
	gl::glBindVertexArray(0);
}

const Transform& DrawingPanel::getTransform() const noexcept { return m_transform; }

Transform& DrawingPanel::getTransform() noexcept { return m_transform; }

void DrawingPanel::setTextureID(uint32 textureID)
{
	if(m_textureID != textureID)
		gl::glDeleteTextures(1, &m_textureID);
	m_textureID= textureID;
}

uint32 DrawingPanel::getTextureID() const noexcept { return m_textureID; }

bool DrawingPanel::isPointInPanel(float32 xpos, float32 ypos) noexcept
{
	glm::vec4 dimensions= getPanelWorldDimension();
	if(xpos >= dimensions.x && xpos <= dimensions.y && ypos >= dimensions.w && ypos <= dimensions.z)
		return true;
	return false;
}

glm::vec4 DrawingPanel::getPanelWorldDimension() noexcept
{
	const float32 left  = (-m_transform.getScale().x * 0.5f) + 0.5f;
	const float32 right = (m_transform.getScale().x * 0.5f) + 0.5f;
	const float32 top   = (m_transform.getScale().y * 0.5f) + 0.5f;
	const float32 bottom= (-m_transform.getScale().y * 0.5f) + 0.5f;
	return glm::vec4(left + m_transform.getPosition().x, right + m_transform.getPosition().x,
									 top + m_transform.getPosition().y, bottom + m_transform.getPosition().y);
}

void DrawingPanel::draw() noexcept
{
	gl::glBindTexture(gl::GL_TEXTURE_2D, m_textureID);
	gl::glBindVertexArray(m_vaoID);
	gl::glDrawArrays(gl::GL_TRIANGLES, 0, 6);
	gl::glBindVertexArray(0);
}
#include "Raytracer/DrawingPanel.h"
#include <glbinding/gl/gl.h>

DrawingPanel::DrawingPanel()
{
	vboID = 0;
	vaoID = 0;
	width = 0;
	height = 0;
	textureID = 0;
}

DrawingPanel::~DrawingPanel()
{
	if (vboID != 0)
		gl::glDeleteBuffers(1, &vboID);
	if (vaoID != 0)
		gl::glDeleteVertexArrays(1, &vaoID);
}

void DrawingPanel::init(float32 width, float32 height)
{
	width *= 2;
	height *= 2;
	this->width = width;
	this->height = height;
	if (vaoID == 0)
		gl::glGenVertexArrays(1, &vaoID);
	if (vboID == 0)
		gl::glGenBuffers(1, &vboID);

	float32 vertexData[24];
	vertexData[0] = width * 0.5f;
	vertexData[1] = height * 0.5f;

	vertexData[2] = 1.0f;
	vertexData[3] = 1.0f;

	vertexData[4] = -width * 0.5f;
	vertexData[5] = height * 0.5f;

	vertexData[6] = 0.0f;
	vertexData[7] = 1.0f;

	vertexData[8] = -width * 0.5f;
	vertexData[9] = -height * 0.5f;

	vertexData[10] = 0.0f;
	vertexData[11] = 0.0f;

	vertexData[12] = -width * 0.5f;
	vertexData[13] = -height * 0.5f;

	vertexData[14] = 0.0f;
	vertexData[15] = 0.0f;

	vertexData[16] = width * 0.5f;
	vertexData[17] = -height * 0.5f;

	vertexData[18] = 1.0f;
	vertexData[19] = 0;

	vertexData[20] = width * 0.5f;
	vertexData[21] = height * 0.5f;

	vertexData[22] = 1.0f;
	vertexData[23] = 1.0f;

	gl::glBindVertexArray(vaoID);
	gl::glBindBuffer(gl::GL_ARRAY_BUFFER, vboID);

	gl::glEnableVertexAttribArray(0);
	gl::glVertexAttribPointer(0, 2, gl::GL_FLOAT, gl::GL_FALSE, 4 * sizeof(float32), (void*)0);
	gl::glEnableVertexAttribArray(1);
	gl::glVertexAttribPointer(1, 2, gl::GL_FLOAT, gl::GL_FALSE, 4 * sizeof(float32), (void*)(2 * sizeof(float32)));
	gl::glBufferData(gl::GL_ARRAY_BUFFER, sizeof(vertexData), vertexData, gl::GL_STATIC_DRAW);
	gl::glBindBuffer(gl::GL_ARRAY_BUFFER, 0);
	gl::glBindVertexArray(0);
}

const Transform& DrawingPanel::getTransform() const noexcept
{
	return transform;
}

Transform& DrawingPanel::getTransform() noexcept
{
	return transform;
}

void DrawingPanel::setTextureID(uint32 textureID)
{
	if (this->textureID != textureID)
		gl::glDeleteTextures(1, &this->textureID);
	this->textureID = textureID;
}

uint32 DrawingPanel::getTextureID() const noexcept
{
	return this->textureID;
}

bool DrawingPanel::isPointInPanel(float32 xpos, float32 ypos)noexcept
{
	glm::vec4 dimensions = getPanelWorldDimension();
	if (xpos >= dimensions.x && xpos <= dimensions.y && ypos >= dimensions.w && ypos <= dimensions.z)
		return true;
	return false;
}

glm::vec4 DrawingPanel::getPanelWorldDimension()noexcept
{
	float32 left = (-transform.getScale().x * 0.5f) + 0.5f;
	float32 right = (transform.getScale().x * 0.5f) + 0.5f;
	float32 top = (transform.getScale().y * 0.5f) + 0.5f;
	float32 bottom = (-transform.getScale().y * 0.5f) + 0.5f;
	return glm::vec4
	(left + transform.getPosition().x,
	right + transform.getPosition().x,
	top + transform.getPosition().y,
	bottom + transform.getPosition().y);
}

void DrawingPanel::draw()noexcept
{
	gl::glBindTexture(gl::GL_TEXTURE_2D, textureID);
	gl::glBindVertexArray(vaoID);
	gl::glDrawArrays(gl::GL_TRIANGLES, 0, 6);
	gl::glBindVertexArray(0);
}
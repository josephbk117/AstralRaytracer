#pragma once
#include "Transform.h"
#include "Utils/Common.h"

class DrawingPanel
{
	public:
	DrawingPanel();
	~DrawingPanel();
	void             init(float32 width, float32 height);
	const Transform& getTransform() const noexcept;
	Transform&       getTransform() noexcept;
	void             setTextureID(uint32 textureID);
	uint32           getTextureID() const noexcept;
	bool             isPointInPanel(float32 xpos, float32 ypos) noexcept;
	glm::vec4        getPanelWorldDimension() noexcept;
	void             draw() noexcept;

	private:
	Transform transform;
	float32   width, height;
	uint32    vaoID;
	uint32    vboID;
	uint32    textureID;
};
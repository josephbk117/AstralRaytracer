#pragma once
#include "Utils/Common.h"
#include <glm.hpp>
class Transform
{
public:
	Transform(const glm::vec2& position, float32 rotation, const glm::vec2& scale) noexcept;
	Transform() noexcept;
	Transform(const Transform& copy) noexcept;
	const glm::mat4& getMatrix()const;
	void setPosition(const glm::vec2& position);
	void setPosition(float32 xCoord, float32 yCoord);
	void translate(float32 x, float32 y);
	void setX(float32 xValue);
	void setY(float32 yValue);
	void setRotation(float32 rotation);
	void rotate(float32 rotation);
	void setScale(const glm::vec2& scale);
	const glm::vec2& getPosition()const noexcept;
	float32 getRotation()const noexcept;
	const glm::vec2& getScale()const noexcept;
	bool operator==(const Transform& transform)const noexcept;
	bool operator!=(const Transform& transform)const noexcept;
private:
	glm::mat4 modelMatrix = glm::mat4(1.0);
	glm::vec2 scale = glm::vec2(1.0f, 1.0f);
	glm::vec2 position = glm::vec2(0.0f, 0.0f);
	float32 rotation = 0.0f;
	bool needsUpdate = true;
	void setModelMatrix();
};
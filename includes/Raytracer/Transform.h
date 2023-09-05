#pragma once
#include "Utils/Common.h"
class Transform
{
	public:
	Transform(const glm::vec3& position, float32 rotation, const glm::vec3& scale) noexcept;
	Transform() noexcept;
	Transform(const Transform& copy) noexcept;
	const glm::mat4& getMatrix() const;
	void             setPosition(const glm::vec3& position);
	void             setPosition(float32 xCoord, float32 yCoord, float32 zCoord);
	void             translate(float32 x, float32 y, float32 z);
	void             setX(float32 xValue);
	void             setY(float32 yValue);
	void             setZ(float32 zValue);
	void             setRotation(float32 rotation);
	void             rotate(float32 rotation);
	void             setScale(const glm::vec3& scale);
	const glm::vec3& getPosition() const noexcept;
	float32          getRotation() const noexcept;
	const glm::vec3& getScale() const noexcept;
	bool             operator==(const Transform& transform) const noexcept;
	bool             operator!=(const Transform& transform) const noexcept;

	private:
	glm::mat4 modelMatrix= glm::mat4(1.0);
	glm::vec3 m_scale    = glm::vec3(1.0f, 1.0f, 1.0f);
	glm::vec3 m_position = glm::vec3(0.0f, 0.0f, 0.0f);
	float32   m_rotation = 0.0f;
	void      updateModelMatrix();
};

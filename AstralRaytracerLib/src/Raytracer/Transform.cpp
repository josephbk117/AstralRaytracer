#include "Raytracer/Transform.h"

#include <glm/gtc/matrix_transform.hpp>

Transform::Transform(const glm::vec3 &position, float32 rotation, const glm::vec3 &scale) noexcept
{
    assignTransformAndUpdateModelMatrix(position, rotation, scale);
}

Transform::Transform() noexcept
{
    updateModelMatrix();
}

Transform::Transform(const Transform &copy) noexcept
{
    assignTransformAndUpdateModelMatrix(copy.m_position, copy.m_rotation, copy.m_scale);
}

Transform &Transform::operator=(const Transform &rhs) noexcept
{
    if (&rhs == this)
    {
        return *this;
    }

    assignTransformAndUpdateModelMatrix(rhs.m_position, rhs.m_rotation, rhs.m_scale);

    return *this;
}

const glm::mat4 &Transform::getMatrix() const
{
    return m_modelMatrix;
}
glm::mat4 &Transform::getMatrix()
{
    return m_modelMatrix;
}

void Transform::setPosition(const glm::vec3 &position)
{
    m_position = position;
    updateModelMatrix();
}
void Transform::setPosition(float32 xCoord, float32 yCoord, float32 zCoord)
{
    m_position = glm::vec3(xCoord, yCoord, zCoord);
    updateModelMatrix();
}

void Transform::translate(float32 x, float32 y, float32 z)
{
    m_position = glm::vec3(m_position.x + x, m_position.y + y, m_position.z + z);
    updateModelMatrix();
}

void Transform::setX(float32 xValue)
{
    setPosition(xValue, m_position.y, m_position.z);
}
void Transform::setY(float32 yValue)
{
    setPosition(m_position.x, yValue, m_position.z);
}
void Transform::setZ(float32 zValue)
{
    setPosition(m_position.x, m_position.y, zValue);
}
void Transform::setRotation(float32 rotation)
{
    m_rotation = rotation;
    updateModelMatrix();
}
void Transform::rotate(float32 rotation)
{
    m_rotation += rotation;
    updateModelMatrix();
}
void Transform::setScale(const glm::vec3 &scale)
{
    m_scale = scale;
    updateModelMatrix();
}

const glm::vec3 &Transform::getPosition() const noexcept
{
    return m_position;
}
float32 Transform::getRotation() const noexcept
{
    return m_rotation;
}
const glm::vec3 &Transform::getScale() const noexcept
{
    return m_scale;
}

bool Transform::operator==(const Transform &rhs) const noexcept
{
    return (m_modelMatrix == rhs.m_modelMatrix);
}
bool Transform::operator!=(const Transform &rhs) const noexcept
{
    return !(*this == rhs);
}
void Transform::updateModelMatrix()
{
    m_modelMatrix = glm::translate(glm::mat4(1.0f), glm::vec3(m_position.x, m_position.y, m_position.z));
    m_modelMatrix = glm::rotate(m_modelMatrix, m_rotation, glm::vec3(0.0f, 1.0f, 0.0f));
    m_modelMatrix = glm::scale(m_modelMatrix, glm::vec3(m_scale.x, m_scale.y, m_scale.z));
}

void Transform::assignTransformAndUpdateModelMatrix(const glm::vec3 &position, float32 rotation, const glm::vec3 &scale)
{
    m_position = position;
    m_rotation = rotation;
    m_scale = scale;
    updateModelMatrix();
}

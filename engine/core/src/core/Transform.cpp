#include "infPCH.h"
#include "core/Transform.h"
#define GLM_ENABLE_EXPERIMENTAL
#include <glm/gtx/matrix_decompose.hpp>

using namespace INF;

void Transform::SetPosition(const glm::vec3& pos)
{
	m_position = pos;
	SetDirty();
}

void Transform::Translate(const glm::vec3& translation)
{
	m_position += translation;
	SetDirty();
}

void Transform::SetRotation(const glm::quat& quaternion)
{
	m_rotation = quaternion;
	SetDirty();
}

void Transform::SetRotation(const glm::vec3& euler)
{
	m_rotation = glm::quat(euler);
	SetDirty();
}

void Transform::Rotate(const glm::quat& quaternion)
{
	m_rotation += quaternion;
	SetDirty();
}

void Transform::Rotate(const glm::vec3& eulerAxis, float angle)
{
	m_rotation = glm::rotate(m_rotation, angle, eulerAxis);
	SetDirty();
}

void Transform::SetScale(const glm::vec3& scale)
{
	m_scale = scale;
	SetDirty();
}

const glm::vec3& Transform::GetPosition() const
{
	return glm::vec3(m_world[3]);
}

const glm::quat& Transform::GetRotation() const
{
	glm::vec3 scale;
	glm::quat rotation;
	glm::vec3 translation;
	glm::vec3 skew;
	glm::vec4 perspective;
	glm::decompose(m_world, scale, rotation, translation, skew, perspective);
	rotation = glm::conjugate(rotation);
	return rotation;
}

const glm::vec3& Transform::GetScale() const
{
	glm::vec3 scale;
	glm::quat rotation;
	glm::vec3 translation;
	glm::vec3 skew;
	glm::vec4 perspective;
	glm::decompose(m_world, scale, rotation, translation, skew, perspective);
	return scale;
}

glm::mat4 Transform::GetLocalMatrix() const
{
	glm::mat4 local{1.0f};
	local = glm::translate(local, m_position);
	local = local * glm::mat4_cast(m_rotation);
	local = glm::scale(local, m_scale);
	return local;
}

void Transform::UpdateParentTransform(const Transform& parent)
{
	glm::mat4 local = GetLocalMatrix();
	const auto& parentWorld = parent.m_world;
	m_world = local * parentWorld;
}

void Transform::UpdateTransform()
{
	if (IsDirty())
	{
		SetDirty(false);

		m_world = GetLocalMatrix();
	}
}

void Transform::ApplyTransform()
{
	SetDirty();

	glm::vec3 skew;
	glm::vec4 perspective;
	glm::decompose(m_world, m_scale, m_rotation, m_position, skew, perspective);
}

Transform::Transform() : m_position(0), m_scale(1.0f), m_rotation(glm::vec3(0.0f)), m_world(1.0f), m_dirty(false)
{

}

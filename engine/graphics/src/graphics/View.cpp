#include "infPCH.h"
#include "graphics/View.h"

using namespace INF::GFX;

View::View()
{

}

View::View(float fov, float aspect, float near, float far)
{
	SetProjectionMatrix(fov, aspect, near, far);
}


void View::Translate(const glm::vec3& translation)
{
	glm::vec3 pos = GetPosition();
	pos += translation;
	SetPosition(pos);
}

void View::SetPosition(const glm::vec3& pos)
{
	glm::mat4 local{ 1.0f };
	m_view = glm::translate(local, pos);
}

glm::vec3 View::GetPosition() const
{
	return glm::vec3(m_view[3]);
}

void View::LookAt(const glm::vec3& point, const glm::vec3& up)
{
	m_view = glm::lookAt(GetPosition(), point, up);
}

void View::SetProjectionMatrix(float fov, float aspect, float near, float far)
{
	m_fov = fov;
	m_aspectRatio = aspect;
	m_near = near;
	m_far = far;

	m_perspective = glm::perspective(fov, aspect, near, far);
}

const glm::vec3& View::Front() const
{
	return m_view[2];
}

const glm::vec3& View::Up() const
{
	return m_view[1];
}

const glm::vec3& View::Right() const
{
	return m_view[0];
}

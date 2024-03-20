#pragma once
#include "graphicDefines.h"

namespace INF::GFX
{
	class View
	{
	public:
		View();
		View(float fov, float aspect, float near, float far);

		void Translate(const glm::vec3& translation);
		void SetPosition(const glm::vec3& pos);
		glm::vec3 GetPosition() const;

		void LookAt(const glm::vec3& point, const glm::vec3& up);

		void SetProjectionMatrix(float fov, float aspect, float near, float far);
		void SetViewMatrix(const glm::mat4& view) { m_view = view; }

		void SetViewport(const Viewport& viewport) { m_viewport = viewport; }
		void SetScissor(const Rect& scissor) { m_scissor = scissor; }

		const glm::mat4& GetViewMatrix() const { return m_view; }
		const glm::mat4& GetProjectionMatrix() const { return m_perspective; }
		const Viewport& GetViewport() const { return m_viewport; }
		const Rect& GetScissor() const { return m_scissor; }
	private:
		glm::mat4 m_view{ 1 };
		glm::mat4 m_perspective{ 1 };

		float m_fov{ 75.0 };
		float m_aspectRatio{ 1.77f };
		float m_near{ 0.1f };
		float m_far{ 100.0f };

		Viewport m_viewport;
		Rect m_scissor;
	};
}
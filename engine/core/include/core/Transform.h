#pragma once
#include "glm/glm.hpp"
#include "glm/gtc/quaternion.hpp"

namespace INF
{
	struct Transform
	{
	public:
		Transform();

		void SetPosition(const glm::vec3& pos);
		void Translate(const glm::vec3& translation);

		void SetRotation(const glm::quat& quaternion);
		void SetRotation(const glm::vec3& euler);
		void Rotate(const glm::quat& quaternion);
		void Rotate(const glm::vec3& eulerAxis, float angle);

		void SetScale(const glm::vec3& scale);

		const glm::vec3& GetPosition() const;
		const glm::quat& GetRotation() const;
		const glm::vec3& GetScale() const;

		const glm::vec3& GetLocalPosition() const { return m_position; }
		const glm::quat& GetLocalRotation() const { return m_rotation; }
		const glm::vec3& GetLocalScale() const { return m_scale; }

		glm::mat4 GetLocalMatrix() const;
		const glm::mat4& GetWorldMatrix() const { return m_world; }

		void UpdateParentTransform(const Transform& parent); //Applies a parent transform to this world transform
		void UpdateTransform(); //Applies the local space to world space (scale/pos/rotation into m_world)
		void ApplyTransform(); //Applies the world space back into local space (scale/pos/rotation into m_world)

		inline void SetDirty(bool dirty = true) { m_dirty = dirty; }
		inline bool IsDirty() const { return m_dirty; }
	private:
		glm::quat m_rotation;
		glm::vec3 m_position;
		glm::vec3 m_scale;

		glm::mat4 m_world;
		bool m_dirty;
	};
}
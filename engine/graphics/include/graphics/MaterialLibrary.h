#pragma once
#include "graphics/Material.h"

namespace INF::GFX
{
	class MaterialLibrary
	{
	public:
		MaterialLibrary(IDevice* device);

		template <typename T>
		MaterialHandle CreateMaterial(const std::string& name);

		MaterialHandle Find(const std::string& name) const;
	private:
		IDevice* m_device;
		std::unordered_map<std::string, MaterialHandle> m_materials;
	};

	template <typename T>
	MaterialHandle INF::GFX::MaterialLibrary::CreateMaterial(const std::string& name)
	{
		MaterialHandle material = Find(name);
		INF_ASSERT(!material, std::format("Material already exists with name:{0}", name));
		if (material)
			return material;

		material = MaterialHandle(new Material(name));
		material->SetObject<T>(m_device);

		m_materials.emplace(name, material);
		return material;
	}

}
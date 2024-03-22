#pragma once
#include "graphics/Material.h"

namespace INF::GFX
{
	class MaterialLibrary
	{
	public:
		MaterialLibrary(IDevice* device);

		template <typename T>
		MaterialHandle CreateMaterial(std::string_view name);

		MaterialHandle Find(std::string_view name) const;
	private:
		IDevice* m_device;
		std::unordered_map<std::string_view, MaterialHandle> m_materials;
	};

	template <typename T>
	MaterialHandle INF::GFX::MaterialLibrary::CreateMaterial(std::string_view name)
	{
		MaterialHandle material = Find(name);
		INF_ASSERT(!material, std::format("Material already exists with name:{0}", name));
		if (material)
			return material;

		material = MaterialHandle(new Material(name));
		material->SetObject<T>(m_device);
		return material;
	}

}
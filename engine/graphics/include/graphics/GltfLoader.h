#pragma once
#include "graphicDefines.h"
#include "graphics/MaterialLibrary.h"
#include "graphics/MeshGenerator.h"

namespace INF::GFX
{
	class GLTFLoader
	{
	public:
		bool Load(std::string_view path, IDevice* device, MaterialLibrary& materialLibrary, std::vector<MeshInfo>& meshes, std::vector<MeshInstance>& meshInstances);
	private:
	};
}
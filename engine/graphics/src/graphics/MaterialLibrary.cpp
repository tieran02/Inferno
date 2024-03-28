#pragma once
#include "infPCH.h"
#include "graphics/MaterialLibrary.h"

using namespace INF::GFX;

MaterialLibrary::MaterialLibrary(IDevice* device) : m_device(device)
{

}

MaterialHandle MaterialLibrary::Find(const std::string& name) const
{
	auto it = m_materials.find(name);
	if (it == m_materials.end())
		return nullptr;

	return it->second;
}

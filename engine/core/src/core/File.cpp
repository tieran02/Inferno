#include "infPCH.h"
#include "core/File.h"
#include "core/Log.h"

using namespace INF;

File::File()
{

}

bool File::Open(std::string_view filename)
{
	std::ifstream file(filename.data(), std::ios::ate | std::ios::binary);
	bool exists = (bool)file;

	if (!exists || !file.is_open())
	{
		Log::Error(std::format("failed to open file: {}", filename));
		return false;
	}

	size_t fileSize = (size_t)file.tellg();
	m_contents.resize(fileSize);

	file.seekg(0);
	file.read(m_contents.data(), fileSize);

	file.close();
	return true;
}

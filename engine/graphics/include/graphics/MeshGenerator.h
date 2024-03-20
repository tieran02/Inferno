#pragma once
#include "graphicDefines.h"

namespace INF::GFX
{
	typedef uint16_t IndexFormat;

	struct MeshData
	{
		std::vector<glm::vec3> Positions;
		std::vector<glm::vec3> Normals;
		std::vector<glm::vec3> Colours;
		std::vector<glm::vec2> TexCoords;
		std::vector<IndexFormat> Indices;
	};

	typedef std::vector<uint8_t> MeshDataBuffer;

	class MeshGenerator
	{
	public:
		static void PackMesh(const MeshData& mesh, MeshDataBuffer& outVertexData, MeshDataBuffer& outIndexData, bool useNormals, bool useTexCoords, bool useColours);
		static MeshData TrianglePrimative();
		static MeshData QuadPrimative();
		static MeshData CubePrimative();
		static MeshData UVSphere(uint32_t slices = 36, uint32_t stacks = 18, float radius = 1.0f);
	};
}
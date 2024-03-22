#pragma once
#include "graphicDefines.h"
#include "interface/Buffer.h"
#include "core/Transform.h"

namespace INF::GFX
{
	class IDevice;
	class Material;

	typedef uint16_t IndexFormat;

	struct MeshBuffer
	{
		IndexBufferHandle indexBuffer;
		VertexBufferHandle vertexBuffer;
	};

	struct MeshInfo
	{
		std::string name;
		MeshBuffer buffer;
		uint32_t indexOffset{ 0 };
		uint32_t vertexOffset{ 0 };
		uint32_t numIndices{ 0 };
		uint32_t numVertices{ 0 };
	};

	struct MeshInstance
	{
		MeshInfo* mesh;
		Material* material;
		Transform transform;
		uint32_t instanceOffset;
	};

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
		static void PackMesh(const MeshData& mesh, IDevice* device, MeshBuffer& outMeshBuffer, bool useNormals, bool useTexCoords, bool useColours);
		static MeshData TrianglePrimative();
		static MeshData QuadPrimative();
		static MeshData CubePrimative();
		static MeshData UVSphere(uint32_t slices = 36, uint32_t stacks = 18, float radius = 1.0f);
	};
}
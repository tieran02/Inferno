#include "infPCH.h"
#include "graphics/MeshGenerator.h"

using namespace INF::GFX;

namespace
{
	void CalculatePlane(std::vector<glm::vec3>& vertices, std::vector<glm::vec2>& uvs, std::vector<glm::vec3>& colours, std::vector<IndexFormat>& indices,
		const glm::vec3& origin, const glm::vec3& right, const glm::vec3& up, int rSegments = 2, int uSegments = 2)
	{
		float rInv = 1.0f / (rSegments - 1);
		float uInv = 1.0f / (uSegments - 1);

		size_t triangleOffset = vertices.size();

		for (int y = 0; y < uSegments; y++) {
			float ru = y * uInv;
			for (int x = 0; x < rSegments; x++) {
				float rr = x * rInv;
				vertices.push_back(origin + right * (rr - 0.5f) + up * (ru - 0.5f));
				colours.push_back(right * (rr - 0.5f) + up * (ru - 0.5f));
				uvs.push_back(glm::vec2(rr, -ru));
			}

			if (y < uSegments - 1) {
				auto offset = y * rSegments + triangleOffset;
				for (auto x = 0, n = rSegments - 1; x < n; x++)
				{
					indices.push_back(IndexFormat(offset + x));
					indices.push_back(IndexFormat(offset + x + rSegments));
					indices.push_back(IndexFormat(offset + x + 1));

					indices.push_back(IndexFormat(offset + x + 1));
					indices.push_back(IndexFormat(offset + x + rSegments));
					indices.push_back(IndexFormat(offset + x + 1 + rSegments));
				}
			}
		}

	}
}

void MeshGenerator::PackMesh(const MeshData& mesh, MeshDataBuffer& outVertexData, MeshDataBuffer& outIndexData, bool useNormals, bool useTexCoords, bool useColours)
{
	const size_t vertexCount = mesh.Positions.size();
	const size_t indexSize = mesh.Indices.size() * sizeof(IndexFormat);
	size_t vertexDataSize = vertexCount * sizeof(glm::vec3);

	if (useNormals)
		vertexDataSize += vertexCount * sizeof(glm::vec3);
	if (useColours)
		vertexDataSize += vertexCount * sizeof(glm::vec3);
	if (useTexCoords)
		vertexDataSize += vertexCount * sizeof(glm::vec2);

	outVertexData.resize(vertexDataSize);
	outIndexData.resize(indexSize);

	size_t offset = 0;
	for (int i = 0; i < vertexCount; i++)
	{
		memcpy(&outVertexData[offset], &mesh.Positions[i], sizeof(glm::vec3));
		offset += sizeof(glm::vec3);

		if (useNormals)
		{
			memcpy(&outVertexData[offset], &mesh.Normals[i], sizeof(glm::vec3));
			offset += sizeof(glm::vec3);
		}

		if (useColours)
		{
			memcpy(&outVertexData[offset], &mesh.Colours[i], sizeof(glm::vec3));
			offset += sizeof(glm::vec3);
		}

		if (useTexCoords)
		{
			memcpy(&outVertexData[offset], &mesh.TexCoords[i], sizeof(glm::vec2));
			offset += sizeof(glm::vec2);
		}
	}

	offset = 0;
	memcpy(outIndexData.data(), mesh.Indices.data(), indexSize);
}

MeshData MeshGenerator::TrianglePrimative()
{
	MeshData TriangleMeshData;
	TriangleMeshData.Positions =
	{
		{1.0f, -1.0f, 0.0f},
		{-1.0f, -1.0f, 0.0f},
		{0.0f, 1.0f, 0.0f}
	};
	TriangleMeshData.Colours =
	{
		{1.0f, 0.0f, 0.0f},
		{0.0f, 1.0f, 0.0f},
		{0.0f, 0.0f, 1.0f}
	};
	TriangleMeshData.TexCoords =
	{
		{1.0f, 1.0f},
		{0.0f, 1.0f},
		{0.5f, 0.0f}
	};
	TriangleMeshData.Indices = { 0, 1, 2 };

	return TriangleMeshData;
}

MeshData MeshGenerator::QuadPrimative()
{
	MeshData meshData;
	meshData.Positions =
	{
		{-1.0f,  1.0f, 0.0f},
		{1.0f, -1.0f, 0.0f},
		{-1.0f, -1.0f, 0.0f},
		{1.0f,  1.0f, 0.0f}
	};

	meshData.Colours =
	{
		{1.0f, 0.0f, 0.0f},
		{0.0f, 1.0f, 0.0f},
		{0.0f, 0.0f, 1.0f},
		{1.0f, 1.0f, 0.0f}
	};

	meshData.TexCoords =
	{
		{0.0f, 0.0f},
		{1.0f, 1.0f},
		{0.0f, 1.0f},
		{1.0f, 0.0f}
	};

	meshData.Indices =
	{
		0, 1, 2,
		0, 3, 1
	};

	return meshData;
}

MeshData MeshGenerator::CubePrimative()
{
	MeshData meshData;

	const int widthSegments = 2;
	const int heightSegments = 2;
	const int depthSegments = 2;

	const float width(1.0f), height(1.0f), depth(1.0f);
	const auto hw = width * 0.5f;
	const auto hh = height * 0.5f;
	const auto hd = depth * 0.5f;

	glm::vec3 forward(0, 0, 1);
	glm::vec3 back(0, 0, -1);
	glm::vec3 up(0, 1, 0);
	glm::vec3 right(1, 0, 0);
	glm::vec3 left(-1, 0, 0);

	// front
	CalculatePlane(
		meshData.Positions, meshData.TexCoords, meshData.Colours, meshData.Indices,
		forward * -hd, right * width, up * height, widthSegments, heightSegments
	);

	// right
	CalculatePlane(
		meshData.Positions, meshData.TexCoords, meshData.Colours, meshData.Indices,
		right * hw, forward * depth, up * height, depthSegments, heightSegments
	);

	// back
	CalculatePlane(
		meshData.Positions, meshData.TexCoords, meshData.Colours, meshData.Indices,
		forward * hd, left * width, up * height, widthSegments, heightSegments
	);

	// left
	CalculatePlane(
		meshData.Positions, meshData.TexCoords, meshData.Colours, meshData.Indices,
		right * -hw, back * depth, up * height, depthSegments, heightSegments
	);

	// top
	CalculatePlane(
		meshData.Positions, meshData.TexCoords, meshData.Colours, meshData.Indices,
		up * hh, right * width, forward * depth, widthSegments, depthSegments
	);

	// bottom
	CalculatePlane(
		meshData.Positions, meshData.TexCoords, meshData.Colours, meshData.Indices,
		up * -hh, right * width, back * depth, widthSegments, depthSegments
	);

	return meshData;
}

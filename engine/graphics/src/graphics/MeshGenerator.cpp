#include "infPCH.h"
#include "graphics/MeshGenerator.h"
#include <math.h>
#include "graphics/interface/device.h"

using namespace INF::GFX;

namespace
{
	void CalculatePlane(std::vector<glm::vec3>& vertices, std::vector<glm::vec3>& normals, std::vector<glm::vec2>& uvs, std::vector<glm::vec3>& colours, std::vector<IndexFormat>& indices,
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
				uvs.push_back(glm::vec2(1.0f - rr, 1.0f - ru));
				normals.push_back(glm::normalize(origin));
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


void MeshGenerator::PackMesh(const MeshData& mesh, IDevice* device, MeshBuffer& outMeshBuffer, bool useNormals, bool useTexCoords, bool useColours)
{
	const size_t vertexCount = mesh.Positions.size();
	const size_t indexSize = mesh.Indices.size() * sizeof(IndexFormat);
	size_t vertexDataStride = sizeof(glm::vec3);

	if (useNormals)
		vertexDataStride += sizeof(glm::vec3);
	if (useColours)
		vertexDataStride += sizeof(glm::vec3);
	if (useTexCoords)
		vertexDataStride += sizeof(glm::vec2);

	size_t vertexDataSize = vertexCount * vertexDataStride;

	GFX::VertexBufferDesc vertexBufferDesc;
	vertexBufferDesc.access = GFX::CpuVisible::WRITE;
	vertexBufferDesc.byteSize = static_cast<uint32_t>(vertexDataSize);
	vertexBufferDesc.strideInBytes = static_cast<uint32_t>(vertexDataStride);
	outMeshBuffer.vertexBuffer = device->CreateVertexBuffer(vertexBufferDesc);

	GFX::IndexBufferDesc indexBufferDesc;
	indexBufferDesc.access = GFX::CpuVisible::WRITE;
	indexBufferDesc.format = GFX::Format::R16_UINT;
	indexBufferDesc.byteSize = static_cast<uint32_t>(indexSize);
	outMeshBuffer.indexBuffer = device->CreateIndexBuffer(indexBufferDesc);

	uint8_t* outVertexData = (uint8_t*)device->MapBuffer(outMeshBuffer.vertexBuffer->GetBuffer());
	uint8_t* outIndexData = (uint8_t*)device->MapBuffer(outMeshBuffer.indexBuffer->GetBuffer());

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
	memcpy(outIndexData, mesh.Indices.data(), indexSize);

	device->UnmapBuffer(outMeshBuffer.vertexBuffer->GetBuffer());
	device->UnmapBuffer(outMeshBuffer.indexBuffer->GetBuffer());
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
		meshData.Positions, meshData.Normals, meshData.TexCoords, meshData.Colours, meshData.Indices,
		forward * -hd, right * width, up * height, widthSegments, heightSegments
	);

	// right
	CalculatePlane(
		meshData.Positions, meshData.Normals, meshData.TexCoords, meshData.Colours, meshData.Indices,
		right * hw, forward * depth, up * height, depthSegments, heightSegments
	);

	// back
	CalculatePlane(
		meshData.Positions, meshData.Normals, meshData.TexCoords, meshData.Colours, meshData.Indices,
		forward * hd, left * width, up * height, widthSegments, heightSegments
	);

	// left
	CalculatePlane(
		meshData.Positions, meshData.Normals, meshData.TexCoords, meshData.Colours, meshData.Indices,
		right * -hw, back * depth, up * height, depthSegments, heightSegments
	);

	// top
	CalculatePlane(
		meshData.Positions, meshData.Normals, meshData.TexCoords, meshData.Colours, meshData.Indices,
		up * hh, right * width, forward * depth, widthSegments, depthSegments
	);

	// bottom
	CalculatePlane(
		meshData.Positions, meshData.Normals, meshData.TexCoords, meshData.Colours, meshData.Indices,
		up * -hh, right * width, back * depth, widthSegments, depthSegments
	);

	return meshData;
}

MeshData MeshGenerator::UVSphere(uint32_t slices, uint32_t stacks, float radius)
{
	MeshData meshData;
	const float lengthInv = 1.0f / radius;

	const float sectorStep = 2 * PI / slices;
	const float stackStep = PI / stacks;
	float sectorAngle, stackAngle;

	for (int i = 0; i <= stacks; ++i)
	{
		stackAngle = PI / 2 - i * stackStep;        // starting from pi/2 to -pi/2
		float xy = radius * cosf(stackAngle);             // r * cos(u)
		float y = radius * sinf(stackAngle);              // r * sin(u)

		// add (sectorCount+1) vertices per stack
		// first and last vertices have same position and normal, but different tex coords
		for (int j = 0; j <= slices; ++j)
		{
			sectorAngle = j * sectorStep;           // starting from 0 to 2pi

			// vertex position (x, y, z)
			float x = xy * cosf(sectorAngle);             // r * cos(u) * cos(v)
			float z = xy * sinf(sectorAngle);             // r * cos(u) * sin(v)
			meshData.Positions.emplace_back(x, y, z);

			// normalized vertex normal (nx, ny, nz)
			float nx = x * lengthInv;
			float ny = y * lengthInv;
			float nz = z * lengthInv;
			meshData.Normals.emplace_back(nx, ny, nz);


			// vertex tex coord (s, t) range between [0, 1]
			float u = (float)j / slices;
			float v = (float)i / stacks;
			meshData.TexCoords.emplace_back(1.0f - u, v);

			meshData.Colours.emplace_back(0, 0, 0);
		}
	}

	//now the indices
	for (int i = 0; i < stacks; ++i)
	{
		float k1 = i * (slices + 1);     // beginning of current stack
		float k2 = k1 + slices + 1;      // beginning of next stack

		for (int j = 0; j < slices; ++j, ++k1, ++k2)
		{
			// 2 triangles per sector excluding first and last stacks
			// k1 => k2 => k1+1
			if (i != 0)
			{
				meshData.Indices.push_back(k1);
				meshData.Indices.push_back(k2);
				meshData.Indices.push_back(k1 + 1);
			}

			// k1+1 => k2 => k2+1
			if (i != (stacks - 1))
			{
				meshData.Indices.push_back(k1 + 1);
				meshData.Indices.push_back(k2);
				meshData.Indices.push_back(k2 + 1);
			}
		}
	}

	return meshData;
}

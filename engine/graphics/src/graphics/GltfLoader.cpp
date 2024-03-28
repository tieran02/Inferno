#include "infPCH.h"
#include "graphics/GltfLoader.h"
#include "core/Log.h"


#define TINYGLTF_IMPLEMENTATION
#define STB_IMAGE_WRITE_IMPLEMENTATION
#define TINYGLTF_NOEXCEPTION
#define JSON_NOEXCEPTION
#include "tiny_gltf.h"
#include "graphics/Material.h"
#include "graphics/Bitmap.h"
#include "graphics/interface/device.h"

using namespace INF::GFX;

struct MeshLoaderData
{
	IDevice* device;
	MaterialLibrary* materialLibrary;
	std::vector<MeshInfo>* meshes;
	std::vector<MeshInstance>* meshInstances;
	
	std::string rootDir;
	std::unordered_map<std::string, uint32_t> loadedMeshIndices;
};

void TestSize(size_t objectSize, size_t objectCount, size_t gltfBufferSize)
{
	size_t objectTotalSize = objectSize * objectCount;
	assert(objectTotalSize == gltfBufferSize);
}

enum class AttributeType
{
	POSITION,
	NORMAL,
	TEXCOORD
};

void CopyData(const tinygltf::Model model, const tinygltf::Accessor& accessor, MeshData& outMesh, AttributeType attributeType)
{
	uint32_t componentByteSize = tinygltf::GetComponentSizeInBytes(accessor.componentType) * tinygltf::GetNumComponentsInType(accessor.type);

	const tinygltf::BufferView& bufferView = model.bufferViews[accessor.bufferView];
	size_t elementCount = bufferView.byteLength / componentByteSize;


	const tinygltf::Buffer& buffer = model.buffers[bufferView.buffer];
	auto src = &buffer.data.at(bufferView.byteOffset);

	switch (attributeType)
	{
	case AttributeType::POSITION:
		outMesh.Positions.resize(elementCount);
		memcpy(outMesh.Positions.data(), src, bufferView.byteLength);
		break;
	case AttributeType::NORMAL:
		outMesh.Normals.resize(elementCount);
		memcpy(outMesh.Normals.data(), src, bufferView.byteLength);
		break;
	case AttributeType::TEXCOORD:
		outMesh.TexCoords.resize(elementCount);
		memcpy(outMesh.TexCoords.data(), src, bufferView.byteLength);
		break;
	default:
		assert(false);
	}
}

MeshData processMesh(const tinygltf::Model& model, const tinygltf::Mesh& mesh)
{
	MeshData meshData;


	for (size_t i = 0; i < model.bufferViews.size(); ++i)
	{
		const tinygltf::BufferView& bufferView = model.bufferViews[i];
		if (bufferView.target == 0)
		{
			std::cout << "WARN: bufferView.target is zero" << std::endl;
			continue;  // Unsupported bufferView.
		}

		const tinygltf::Buffer& buffer = model.buffers[bufferView.buffer];
		std::cout << "bufferview.target " << bufferView.target << std::endl;

		std::cout << "buffer.data.size = " << buffer.data.size()
			<< ", bufferview.byteOffset = " << bufferView.byteOffset
			<< std::endl;
	}


	for (size_t i = 0; i < mesh.primitives.size(); ++i)
	{
		tinygltf::Primitive primitive = mesh.primitives[i];

		{
			tinygltf::Accessor indexAccessor = model.accessors[primitive.indices];
			uint32_t componentByteSize = tinygltf::GetComponentSizeInBytes(indexAccessor.componentType) * tinygltf::GetNumComponentsInType(indexAccessor.type);

			const tinygltf::BufferView& bufferView = model.bufferViews[indexAccessor.bufferView];
			size_t elementCount = bufferView.byteLength / componentByteSize;
			meshData.Indices.resize(elementCount);


			TestSize(sizeof(IndexFormat), meshData.Indices.size(), bufferView.byteLength);

			const tinygltf::Buffer& buffer = model.buffers[bufferView.buffer];
			auto src = &buffer.data.at(bufferView.byteOffset);

			size_t indexesCount = indexAccessor.count;
			IndexFormat* indexes = (IndexFormat*)src;
			for (int i = 0; i < indexesCount; i += 3)
			{
				//change order for correct back face culling
				IndexFormat tmp = indexes[i];
				indexes[i] = indexes[i + 2];
				indexes[i + 2] = tmp;

				meshData.Indices[i] = indexes[i];
				meshData.Indices[i+1] = indexes[i+1];
				meshData.Indices[i+2] = indexes[i+2];
			}
		}

		for (auto& attrib : primitive.attributes)
		{
			tinygltf::Accessor accessor = model.accessors[attrib.second];

			if (attrib.first.compare("POSITION") == 0)
			{
				CopyData(model, accessor, meshData, AttributeType::POSITION);
			}
			else if (attrib.first.compare("NORMAL") == 0)
			{
				CopyData(model, accessor, meshData, AttributeType::NORMAL);
			}
			else if (attrib.first.compare("TEXCOORD_0") == 0)
			{
				CopyData(model, accessor, meshData, AttributeType::TEXCOORD);
			}
		}
	}

	return meshData;
}

void processNode(const tinygltf::Model& model, const tinygltf::Node& node, MeshLoaderData& loaderData)
{
	if ((node.mesh >= 0) && (node.mesh < model.meshes.size()))
	{
		MeshInfo* meshInfo = nullptr;

		//check if mesh with the name already exists?
		auto it = loaderData.loadedMeshIndices.find(model.meshes[node.mesh].name);
		if (it != loaderData.loadedMeshIndices.end())
		{
			meshInfo = &loaderData.meshes->at(it->second);
		}
		else
		{
			//create a new mesh info
			MeshInfo& newMeshInfo = loaderData.meshes->emplace_back();

			std::wstring meshName(model.meshes[node.mesh].name.begin(), model.meshes[node.mesh].name.end());
			MeshData meshData = processMesh(model, model.meshes[node.mesh]);
			newMeshInfo.name = meshName;
			MeshGenerator::PackMesh(meshData, loaderData.device, newMeshInfo, true, true, false);

			loaderData.loadedMeshIndices.emplace(model.meshes[node.mesh].name, loaderData.meshes->size() - 1);
			meshInfo = &newMeshInfo;

		}

		//add new mesh instance for mesh info
		MeshInstance instance;
		instance.instanceOffset = 0;
		instance.mesh = meshInfo;

		int matIndex = model.meshes[node.mesh].primitives[0].material;
		const auto& mat = model.materials[matIndex];
		instance.material = loaderData.materialLibrary->Find(mat.name).get();

		//transform
		const std::vector<double>& t = node.translation;
		const std::vector<double>& r = node.rotation;
		const std::vector<double>& s = node.scale;

		if (!t.empty())
		{
			auto translation = glm::vec3(static_cast<float>(t[0]), static_cast<float>(t[1]), static_cast<float>(t[2]));
			instance.transform.SetPosition(translation);
		}
		if (!r.empty())
		{
			instance.transform.SetRotation(glm::make_quat(node.rotation.data()));
		}
		if (!s.empty())
		{
			auto scale = glm::vec3(static_cast<float>(s[0]), static_cast<float>(s[1]), static_cast<float>(s[2]));
			instance.transform.SetScale(scale);
		}
		instance.transform.UpdateTransform();

		loaderData.meshInstances->push_back(instance);
	}

	//recursively process nodes
	for (size_t i = 0; i < node.children.size(); i++)
	{
		assert((node.children[i] >= 0) && (node.children[i] < model.nodes.size()));
		processNode(model, model.nodes[node.children[i]], loaderData);
	}
}

void ProcessScene(const tinygltf::Model& model, MeshLoaderData& loaderData)
{
	const tinygltf::Scene& scene = model.scenes[model.defaultScene];

	//process materials
	for (size_t i = 0; i < model.materials.size(); ++i)
	{
		const auto& mat = model.materials[i];
		int textureIndex = mat.pbrMetallicRoughness.baseColorTexture.index;

		if (textureIndex < 0)
			continue;

		const tinygltf::Texture& texture = model.textures[textureIndex];
		if (texture.source < 0)
			continue;

		const std::string& source = std::format("{0}/{1}", loaderData.rootDir,model.images[texture.source].uri);
		INF::Log::Info(std::format("Texture Found:{0}", source));

		INF::GFX::Bitmap bitmap;
		bitmap.Load(source);

		INF::GFX::TextureDesc textureDesc;
		textureDesc.width = bitmap.Width();
		textureDesc.height = bitmap.Height();
		textureDesc.format = bitmap.GetFormat();
		std::wstring textureName(model.images[texture.source].uri.begin(), model.images[texture.source].uri.end());
		textureDesc.name = textureName;
		textureDesc.initialState = (INF::GFX::TRANSITION_STATES_FLAGS)INF::GFX::TRANSITION_STATES::PIXEL_SHADER_RESOURCE;
		INF::GFX::TextureHandle textureHandle = loaderData.device->CreateTexture(textureDesc);
		loaderData.device->ImmediateSubmit([=](INF::GFX::ICommandList* cmd)
			{
				cmd->WriteTexture(textureHandle.get(), bitmap);
			});

		INF::GFX::SamplerDesc samplerDesc;
		samplerDesc.wrapMode[0] = INF::GFX::WrapMode::WRAP;
		samplerDesc.wrapMode[1] = INF::GFX::WrapMode::WRAP;
		samplerDesc.wrapMode[2] = INF::GFX::WrapMode::WRAP;
		INF::GFX::SamplerHandle sampler = loaderData.device->CreateSampler(samplerDesc);

		INF::GFX::MaterialHandle materialHandle = loaderData.materialLibrary->CreateMaterial<INF::GFX::MaterialData>(mat.name);
		materialHandle->SetObject<INF::GFX::MaterialData>(loaderData.device);
		materialHandle->RegisterData("diffuseColour", &MaterialData::diffuseColour);
		materialHandle->RegisterTexture("diffuseTexture", 0);

		materialHandle->SetData("diffuseTexture", textureHandle, sampler);
		materialHandle->UpdateDescriptorSet(loaderData.device);
	}

	//process meshes
	for (size_t i = 0; i < scene.nodes.size(); ++i)
	{
		assert((scene.nodes[i] >= 0) && (scene.nodes[i] < model.nodes.size()));
		processNode(model, model.nodes[scene.nodes[i]], loaderData);
	}

}

bool GLTFLoader::Load(std::string_view fileName, IDevice* device, MaterialLibrary& materialLibrary, std::vector<MeshInfo>& meshes, std::vector<MeshInstance>& meshInstances)
{
	tinygltf::TinyGLTF loader;
	std::string err;
	std::string warn;

	tinygltf::Model model;
	bool res = loader.LoadASCIIFromFile(&model, &err, &warn, fileName.data());
	if (!warn.empty())
	{
		Log::Warning(std::format("GLTFLoader warning: {0}", warn));
	}

	if (!err.empty())
	{
		Log::Error(std::format("GLTFLoader error: {0}",err));
	}

	if (!res)
		Log::Error(std::format("Failed to load glTF: {0}", fileName));
	else
		Log::Info(std::format("Loaded glTF: {0}", fileName));

	MeshLoaderData loaderData;
	loaderData.device = device;
	loaderData.materialLibrary = &materialLibrary;
	loaderData.meshes = &meshes;
	loaderData.meshInstances = &meshInstances;

	std::filesystem::path path(fileName);
	loaderData.rootDir = path.parent_path().string();
	ProcessScene(model, loaderData);

	return res;
}

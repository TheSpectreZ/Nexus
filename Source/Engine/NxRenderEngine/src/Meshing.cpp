#include "NxRenderEngine/Meshing.h"
#include "glm/gtc/type_ptr.hpp"

Nexus::Meshing::MeshSource::BinaryBlob Nexus::Meshing::MeshSource::OnSerialize()
{
	uint64_t blobSize = sizeof(uint64_t) * meshes.size();

	for (auto& m : meshes)
	{
		blobSize += (m.vertices.size() * sizeof(Meshing::Vertex) +
					 m.submeshes.size() * sizeof(Meshing::Submesh) +
					 m.indices.size() * sizeof(uint32_t) + 3 * sizeof(uint64_t));
	}

	BinaryBlob blob;
	blob.Resize(blobSize);
	
	uint64_t size = meshes.size();
	blob.Write(&size, sizeof(uint64_t));

	for (auto& m : meshes)
	{
		size = m.vertices.size() * sizeof(Meshing::Vertex);
		blob.Write(&size, sizeof(uint64_t));
		blob.Write(m.vertices.data(), size);

		size = m.indices.size() * sizeof(uint32_t);
		blob.Write(&size, sizeof(uint64_t));
		blob.Write(m.indices.data(), size);

		size = m.submeshes.size() * sizeof(Meshing::Submesh);
		blob.Write(&size, sizeof(uint64_t));
		blob.Write(m.submeshes.data(), size);
	}

	return blob;
}

void Nexus::Meshing::MeshSource::OnDeserialize(BinaryBlob blob)
{
	uint64_t size = 0;
	blob.Read(&size, sizeof(uint64_t));

	meshes.resize(size);

	for (auto& m : meshes)
	{
		size = 0;
		blob.Read(&size, sizeof(uint64_t));

		m.vertices.resize(size / sizeof(Meshing::Vertex));
		blob.Read(m.vertices.data(), size);

		size = 0;
		blob.Read(&size, sizeof(uint64_t));

		m.indices.resize(size / sizeof(uint32_t));
		blob.Read(m.indices.data(), size);

		size = 0;
		blob.Read(&size, sizeof(uint64_t));

		m.submeshes.resize(size / sizeof(Meshing::Submesh));
		blob.Read(m.submeshes.data(), size);
	}
}

Nexus::Meshing::Texture::BinaryBlob Nexus::Meshing::Texture::OnSerialize()
{
	uint64_t blobSize = (sizeof(uint32_t) * 4 +
						sizeof(uint8_t) * image.pixels.size() +
						sizeof(char) * image.fileName.size() +
						sizeof(uint64_t) * 2);

	BinaryBlob blob;
	blob.Resize(blobSize);
	
	blob.Write(&samplerHash, sizeof(uint32_t));
	blob.Write(&image.width, sizeof(uint32_t));
	blob.Write(&image.height, sizeof(uint32_t));
	blob.Write(&image.channels, sizeof(uint32_t));

	uint64_t size = sizeof(char) * image.fileName.size();
	blob.Write(&size, sizeof(uint64_t));
	blob.Write(image.fileName.data(), size);

	size = sizeof(uint8_t) * image.pixels.size();
	blob.Write(&size, sizeof(uint64_t));
	blob.Write(image.pixels.data(), size);
	
	return blob;
}

void Nexus::Meshing::Texture::OnDeserialize(BinaryBlob blob)
{
	blob.Read(&samplerHash, sizeof(uint32_t));
	blob.Read(&image.width, sizeof(uint32_t));
	blob.Read(&image.height, sizeof(uint32_t));
	blob.Read(&image.channels, sizeof(uint32_t));

	uint64_t size = 0;
	blob.Read(&size, sizeof(uint64_t));
	image.fileName.resize(size / sizeof(char));
	blob.Read(image.fileName.data(), size);

	size = 0;
	blob.Read(&size, sizeof(uint64_t));
	image.pixels.resize(size);
	blob.Read(image.pixels.data(), size);
}

nlohmann::json Nexus::Meshing::Material::OnSerialize()
{
	nlohmann::json Json;
	Json["Name"] = Name;

	nlohmann::json TexCoords;
	TexCoords["baseColor"] = textureCoords.baseColor;
	TexCoords["normal"] = textureCoords.normal;
	TexCoords["occulsion"] = textureCoords.occlusion;
	TexCoords["emissive"] = textureCoords.emissive;
	TexCoords["metallicRoughness"] = textureCoords.metallicRoughness;
	TexCoords["specularGlossiness"] = textureCoords.specularGlossiness;
	
	Json["TexCoords"] = TexCoords;

	nlohmann::json MR;
	MR["albedoColor"]["r"] = metalicRoughness.albedoColor.r;
	MR["albedoColor"]["g"] = metalicRoughness.albedoColor.g;
	MR["albedoColor"]["b"] = metalicRoughness.albedoColor.b;
	MR["albedoColor"]["a"] = metalicRoughness.albedoColor.a;
	MR["roughness"] = metalicRoughness.roughness;
	MR["metallic"] = metalicRoughness.metallic;
	MR["albedoTexture"] = metalicRoughness.albedoTexture;
	MR["metallicRoughness"] = metalicRoughness.metallicRoughnessTexture;

	Json["MetallicRoughness"] = MR;

	nlohmann::json SG;
	SG["support"] = specularGlossiness.support;
	SG["albedoColor"]["r"] = specularGlossiness.albedoColor.r;
	SG["albedoColor"]["g"] = specularGlossiness.albedoColor.g;
	SG["albedoColor"]["b"] = specularGlossiness.albedoColor.b;
	SG["albedoColor"]["a"] = specularGlossiness.albedoColor.b;
	SG["specular"]["r"] = specularGlossiness.specular.r;
	SG["specular"]["g"] = specularGlossiness.specular.g;
	SG["specular"]["b"] = specularGlossiness.specular.b;
	SG["glossiness"] = specularGlossiness.glossiness;
	SG["albedoTexture"] = specularGlossiness.albedoTexture;
	SG["specularGlossinessTexture"] = specularGlossiness.specularGlossinessTexture;
	
	Json["SpecularGlossiness"] = SG;

	Json["NormalTexture"] = normalTexture;
	Json["OcculsionTexture"] = occulsionTexture;
	Json["EmissiveTexture"] = emissiveTexture;
	Json["EmissiveColor"]["r"] = emissiveColor.r;
	Json["EmissiveColor"]["g"] = emissiveColor.g;
	Json["EmissiveColor"]["b"] = emissiveColor.b;

	return Json;
}

void Nexus::Meshing::Material::OnDeserialize(const nlohmann::json& blob)
{
	Name = blob.at("Name").get<std::string>();

	auto& TexCoords = blob.at("TexCoords");
	textureCoords.baseColor = TexCoords.at("baseColor").get<uint8_t>();
	textureCoords.normal = TexCoords.at("normal").get<uint8_t>();
	textureCoords.occlusion = TexCoords.at("occulsion").get<uint8_t>();
	textureCoords.emissive = TexCoords.at("emissive").get<uint8_t>();
	textureCoords.metallicRoughness = TexCoords.at("metallicRoughness").get<uint8_t>();
	textureCoords.specularGlossiness = TexCoords.at("specularGlossiness").get<uint8_t>();

	auto& MR = blob.at("MetallicRoughness");
	metalicRoughness.albedoColor.r = MR.at("albedoColor").at("r").get<float>();
	metalicRoughness.albedoColor.g = MR.at("albedoColor").at("g").get<float>();
	metalicRoughness.albedoColor.b = MR.at("albedoColor").at("b").get<float>();
	metalicRoughness.albedoColor.a = MR.at("albedoColor").at("a").get<float>();
	metalicRoughness.roughness = MR.at("roughness").get<float>();
	metalicRoughness.metallic = MR.at("metallic").get<float>();
	metalicRoughness.albedoTexture = MR.at("albedoTexture").get<uint64_t>();
	metalicRoughness.metallicRoughnessTexture = MR.at("metallicRoughness").get<uint64_t>();

	auto& SG = blob.at("SpecularGlossiness");
	specularGlossiness.support = SG.at("support").get<bool>();
	specularGlossiness.albedoColor.r = SG.at("albedoColor").at("r").get<float>();
	specularGlossiness.albedoColor.g = SG.at("albedoColor").at("g").get<float>();
	specularGlossiness.albedoColor.b = SG.at("albedoColor").at("b").get<float>();
	specularGlossiness.albedoColor.a = SG.at("albedoColor").at("a").get<float>();
	specularGlossiness.specular.r = SG.at("specular").at("r").get<float>();
	specularGlossiness.specular.g = SG.at("specular").at("g").get<float>();
	specularGlossiness.specular.b = SG.at("specular").at("b").get<float>();
	specularGlossiness.glossiness = SG.at("glossiness").get<float>();
	specularGlossiness.albedoTexture = SG.at("albedoTexture").get<uint64_t>();
	specularGlossiness.specularGlossinessTexture = SG.at("specularGlossinessTexture").get<uint64_t>();

	normalTexture = blob.at("NormalTexture").get<uint64_t>();
	occulsionTexture = blob.at("OcculsionTexture").get<uint64_t>();
	emissiveTexture = blob.at("EmissiveTexture").get<uint64_t>();

	emissiveColor.r = blob.at("EmissiveColor").at("r").get<float>();
	emissiveColor.g = blob.at("EmissiveColor").at("g").get<float>();
	emissiveColor.b = blob.at("EmissiveColor").at("b").get<float>();
}

Nexus::Meshing::Skeleton::BinaryBlob Nexus::Meshing::Skeleton::OnSerialize()
{
	uint64_t BlobSize = sizeof(uint64_t) * (nodes.size() + skins.size());

	for (auto& n : nodes)
	{
		BlobSize += (sizeof(uint32_t) * 2 + sizeof(int32_t) +
					sizeof(glm::vec3) * 2 + sizeof(glm::quat) + sizeof(glm::mat4) +
					sizeof(int32_t) * (n.children.size() + 1) + sizeof(char) * n.name.size() +
					sizeof(uint64_t) * 2);
	}

	for (auto& s : skins)
	{
		BlobSize += (sizeof(glm::mat4) * s.InverseBindMatrices.size() +
					sizeof(char) * s.Name.size() +
					sizeof(int32_t) * (s.Joints.size() + 1) +
					sizeof(uint64_t) * 3);
	}

	BinaryBlob blob;
	blob.Resize(BlobSize);

	uint64_t size = nodes.size();
	blob.Write(&size, sizeof(uint64_t));

	size = skins.size();
	blob.Write(&size, sizeof(uint64_t));

	for (auto& n : nodes)
	{
		blob.Write(&n.Index, sizeof(uint32_t));
		blob.Write(&n.mesh, sizeof(uint32_t));
		blob.Write(&n.skin, sizeof(int32_t));

		blob.Write(glm::value_ptr(n.translation), sizeof(glm::vec3));
		blob.Write(glm::value_ptr(n.scale), sizeof(glm::vec3));
		blob.Write(glm::value_ptr(n.rotation), sizeof(glm::quat));
		blob.Write(glm::value_ptr(n.matrix), sizeof(glm::mat4));

		size = sizeof(char) * n.name.size();
		blob.Write(&size, sizeof(uint64_t));
		blob.Write(n.name.data(), size);

		size = sizeof(int32_t) * n.children.size();
		blob.Write(&size, sizeof(uint64_t));
		blob.Write(&n.children, size);

		blob.Write(&n.parent, sizeof(int32_t));
	}

	for (auto& s : skins)
	{
		size = sizeof(char) * s.Name.size();
		blob.Write(&size, sizeof(uint64_t));
		blob.Write(s.Name.data(), size);

		size = sizeof(glm::mat4) * s.InverseBindMatrices.size();
		blob.Write(&size, sizeof(uint64_t));
		blob.Write(s.InverseBindMatrices.data(), size);

		std::vector<uint32_t> joints(s.Joints.size());
		for (uint64_t i = 0; i < joints.size(); i++)
		{
			joints[i] = s.Joints[i]->Index;
		}

		size = sizeof(uint32_t) * joints.size();
		blob.Write(&size, sizeof(uint64_t));
		blob.Write(joints.data(), size);

		uint32_t root = s.skeletonRoot->Index;
		blob.Write(&root, sizeof(uint32_t));
	}

	return blob;
}

void Nexus::Meshing::Skeleton::OnDeserialize(BinaryBlob blob)
{
	uint64_t size = 0;
	blob.Read(&size, sizeof(uint64_t));
	nodes.resize(size);

	size = 0;
	blob.Read(&size, sizeof(uint64_t));
	skins.resize(size);

	for (auto& n : nodes)
	{
		blob.Read(&n.Index, sizeof(uint32_t));
		blob.Read(&n.mesh, sizeof(uint32_t));
		blob.Read(&n.skin, sizeof(int32_t));

		blob.Read(glm::value_ptr(n.translation), sizeof(glm::vec3));
		blob.Read(glm::value_ptr(n.scale), sizeof(glm::vec3));
		blob.Read(glm::value_ptr(n.rotation), sizeof(glm::quat));
		blob.Read(glm::value_ptr(n.matrix), sizeof(glm::mat4));

		size = 0;
		blob.Read(&size, sizeof(uint64_t));
		n.name.resize(size / sizeof(char));
		blob.Read(n.name.data(), size);

		size = 0;
		blob.Read(&size, sizeof(uint64_t));
		n.children.resize(size);
		blob.Read(n.children.data(), size);

		blob.Read(&n.parent, sizeof(int32_t));
	}

	for (auto& s : skins)
	{
		size = 0;
		blob.Read(&size, sizeof(uint64_t));
		s.Name.resize(size / sizeof(char));
		blob.Read(s.Name.data(), size);

		size = 0;
		blob.Read(&size, sizeof(uint64_t));
		s.InverseBindMatrices.resize(size / sizeof(glm::mat4));
		blob.Read(s.InverseBindMatrices.data(), size);

		size = 0;
		blob.Read(&size, sizeof(uint64_t));
		std::vector<uint32_t> Joints(size / sizeof(uint32_t));
		blob.Read(Joints.data(), size);

		s.Joints.resize(Joints.size());
		for (auto i = 0; i < Joints.size(); i++)
			s.Joints[i] = &nodes[Joints	[i]];

		uint32_t root = 0;
		blob.Read(&root, sizeof(uint32_t));
		s.skeletonRoot = &nodes[root];
	}
}

Nexus::Meshing::Animation::BinaryBlob Nexus::Meshing::Animation::OnSerialize()
{
	uint64_t blobSize = sizeof(uint64_t) * (samplers.size() + channels.size());
	blobSize += sizeof(char) * Name.size();

	for (auto& s : samplers)
	{
		blobSize += (sizeof(uint8_t) +
					 sizeof(float) * s.input.size() +
					 sizeof(glm::vec4) * s.output.size());
	}

	for (auto& c : channels)
	{
		blobSize += (sizeof(uint32_t) * 2 + sizeof(uint8_t));
	}

	BinaryBlob blob;
	blob.Resize(blobSize);

	uint64_t size = sizeof(Sampler) * samplers.size();
	blob.Write(&size, sizeof(uint64_t));

	size = sizeof(Channel) * channels.size();
	blob.Write(&size, sizeof(uint64_t));
	
	size = sizeof(char) * Name.size();
	blob.Write(&size, sizeof(uint64_t));
	blob.Write(Name.data(), size);

	for (auto& s : samplers)
	{
		uint8_t type = (uint8_t)s.type;
		blob.Write(&type, sizeof(uint8_t));;

		size = sizeof(float) * s.input.size();
		blob.Write(&size, sizeof(uint64_t));
		blob.Write(s.input.data(), size);
		
		size = sizeof(glm::vec4) * s.output.size();
		blob.Write(&size, sizeof(uint64_t));
		blob.Write(s.output.data(), size);
	}

	for (auto& c : channels)
	{
		uint8_t type = (uint8_t)c.type;
		blob.Write(&type, sizeof(uint8_t));
		blob.Write(&c.samplerIndex, sizeof(uint32_t));
		blob.Write(&c.node, sizeof(uint32_t));
	}

	return blob;
}

void Nexus::Meshing::Animation::OnDeserialize(BinaryBlob blob)
{
	uint64_t size = 0;
	blob.Read(&size, sizeof(uint64_t));
	samplers.resize(size / sizeof(Sampler));

	size = 0;
	blob.Read(&size, sizeof(uint64_t));
	channels.resize(size / sizeof(Channel));

	size = 0;
	blob.Read(&size, sizeof(uint64_t));
	Name.resize(size / sizeof(char));
	blob.Read(Name.data(), size);

	for (auto& s : samplers)
	{
		blob.Read(&s.type, sizeof(uint8_t));
		
		size = 0;
		blob.Read(&size, sizeof(uint64_t));
		s.input.resize(size / sizeof(float));
		blob.Read(s.input.data(), size);

		size = 0;
		blob.Read(&size, sizeof(uint64_t));
		s.output.resize(size / sizeof(glm::vec4));
		blob.Read(s.output.data(), size);
	}

	for (auto& c : channels)
	{
		blob.Read(&c.type, sizeof(uint8_t));
		blob.Read(&c.samplerIndex, sizeof(uint32_t));
		blob.Read(&c.node, sizeof(uint32_t));		
	}
}
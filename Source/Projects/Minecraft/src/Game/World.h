#pragma once
#include "glm/glm.hpp"
#include "Graphics/Resource.h"
#include <unordered_map>

struct Vertex
{
	glm::vec3 position;
	glm::vec4 color;
};

struct Voxel
{
	enum Face
	{
		FACE_Top = 0, 
		FACE_Right = 1, 
		FACE_Front = 2, 
		FACE_Back = 3,
		FACE_Left = 4, 
		FACE_Down = 5 
	};

	glm::vec3 color;
	std::unordered_map<uint32_t, bool> faces;

	bool IsAir = false;
};

class Chunk
{
	friend class World;
public:
	static float threshold;

	void Create();
	void Destroy();
	void Update();
	void Render(VkCommandBuffer cmd);
private:
	void GenerateVoxels();
	void GenerateMesh();

	static const uint32_t chunkSize = 64;

	inline static const float scale = 10.f;

	Voxel m_voxels[chunkSize][chunkSize][chunkSize];

	Nexus::Graphics::IndexBuffer m_ib;
	Nexus::Graphics::VertexBuffer m_vb;
};

class World
{
public:
	void Create();
	void Destroy();
	void Render(VkCommandBuffer cmd);

	void Update();
private:
	Chunk chunk;
};


#pragma once
#include "glm/glm.hpp"
#include "Graphics/Resource.h"
#include <unordered_map>

struct Vertex
{
	glm::vec3 position;
	glm::vec3 color;
};

struct Voxel
{
	enum Face
	{
		FACE_Top = 0, 
		FACE_Down = 1, 
		FACE_Right = 2, 
		FACE_Left = 3, 
		FACE_Front = 4, 
		FACE_Back = 5
	};

	glm::vec3 color;
	std::unordered_map<uint32_t, bool> faces;

	
};

class Chunk
{
public:
	void Create();
	void Destroy();
	void Render(VkCommandBuffer cmd);
private:
	static const uint32_t chunkSize = 8;

	Voxel m_voxels[chunkSize][chunkSize][chunkSize];

	Nexus::Graphics::VertexBuffer m_vb;
	Nexus::Graphics::IndexBuffer m_ib;
};

class World
{
public:
	void Create();
	void Destroy();
	void Render(VkCommandBuffer cmd);
private:
	Chunk chunk;
};


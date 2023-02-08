#pragma once

#define GLM_ENABLE_EXPERIMENTAL
#include "glm/glm.hpp"
#include "glm/gtx/hash.hpp"

#include "Graphics/Resource.h"

#include <unordered_map>

#include "Player.h"

struct Vertex
{
	glm::vec3 position;
	glm::vec3 TextureInfo;
};

enum class Block
{
	AIR, GRASS, STONE, SAND , BEDROCK, DIRT , SNOW , IRON_ORE
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

	Block Type;
};

class Chunk
{
	friend class World;
public:
	static const uint32_t chunkSize = 16;
	static const uint32_t chunkheight = 64;
	
	bool Created = false;

	void Create(glm::vec2 Coordinate,uint32_t seed);
	void Destroy();
	void Render(VkCommandBuffer cmd);
private:
	void GenerateVoxels(uint32_t seed);
	void GenerateMesh();

	inline static const float scale = 10.f;

	Voxel*** m_voxels;

	glm::vec2 m_coordinate;

	Nexus::Graphics::IndexBuffer m_ib;
	Nexus::Graphics::VertexBuffer m_vb;
};

class World
{
	inline static const glm::ivec2 ChunkSize = glm::ivec2(Chunk::chunkSize);
public:
	void Create();
	void Destroy();
	void Render(VkCommandBuffer cmd);

	void Update();

	Player& GetPlayer() { return m_Player; }
private:
	uint32_t m_worldSeed;
	Player m_Player;

	static std::vector<int> Iters;

	glm::ivec2 m_CurrentChunk;
	std::unordered_map<glm::vec2,Chunk> m_chunks;
	std::vector<Chunk*> m_ActiveChunks;

	void UpdateActiveChunks();
};


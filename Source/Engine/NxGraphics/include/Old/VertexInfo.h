#pragma once
#include <cstdint>

struct VertexBindInfo
{
	enum InputRate
	{
		INPUT_RATE_VERTEX,
		INPUT_RATE_INSTANCE
	};

	uint32_t binding;
	uint32_t stride;
	InputRate inputRate;
};

struct VertexAttribInfo
{
	enum AttribFormat
	{
		ATTRIB_FORMAT_VEC2,
		ATTRIB_FORMAT_VEC3,
		ATTRIB_FORMAT_VEC4,
	};

	uint32_t binding;
	uint32_t location;
	uint32_t offset;
	AttribFormat format;
};
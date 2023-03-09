#pragma once

namespace Nexus
{
	struct vec3
	{
		float x, y, z;
	};

	struct ExampleVertex
	{
		vec3 position;
		vec3 color;
	};

	class Shader;

	struct PipelineCreateInfo
	{
		Ref<Shader> shader;
	};

	class Pipeline
	{
	public:
		static Ref<Pipeline> Create(const PipelineCreateInfo& Info);

		Pipeline() = default;
		virtual ~Pipeline() = default;
	};
}

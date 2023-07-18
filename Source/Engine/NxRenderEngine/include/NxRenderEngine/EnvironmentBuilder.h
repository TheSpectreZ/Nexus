#pragma once
#include "GraphicsInterface.h"
#include "NxCore/UUID.h"

namespace Nexus
{
	struct NEXUS_RENDERER_API Environment
	{
		Ref<Texture> envMap;
		Ref<Texture> IrradianceMap;
		Ref<Texture> specularMap;
	};

	class NEXUS_RENDERER_API EnvironmentBuilder
	{
		static EnvironmentBuilder* s_Instance;
	public:
		static void Initialize();
		static void Shutdown();

		static Ref<Environment> Build(const std::string& hdrPath, UUID Id);
		static Ref<Texture> GetBRDFLut() { return s_Instance->m_BRDFLutTex; }
	private:
		struct ComputeWorker
		{
			Ref<Shader> shader;
			ResourceHeapHandle heap;

			Ref<Pipeline> pipeline;

			void Build(const std::string& shaderpath);
		};

		Ref<CommandQueue> m_queue;

		ComputeWorker m_Eq2Cube;
		ComputeWorker m_brdfLut;
		ComputeWorker m_Irradiance;
		ComputeWorker m_Specular;

		Ref<Texture> m_BRDFLutTex;
	};
}
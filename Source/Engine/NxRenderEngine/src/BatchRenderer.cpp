#include "NxRenderEngine/BatchRenderer.h"
#include "NxCore/Logger.h"
#include "NxCore/UUID.h"
#include "NxRenderEngine/ResourcePool.h"

Nexus::BatchRenderer* Nexus::BatchRenderer::s_Instance = nullptr;

void Nexus::BatchRenderer::Initialize(Ref<Renderpass> renderpass)
{
	s_Instance = new BatchRenderer;
	s_Instance->InitImpl(renderpass);
}

void Nexus::BatchRenderer::Shutdown()
{
	s_Instance->ShutImpl();
	delete s_Instance;
}

void Nexus::BatchRenderer::DrawLine(const glm::vec3& a, const glm::vec3& b)
{
	if (m_CurrentVertexIndex + 2 >= s_VertexCount)
		FlushToBuffer();

	m_VertexData[m_CurrentVertexIndex] = a;
	m_VertexData[m_CurrentVertexIndex + 1] = b;
	
	m_CurrentVertexIndex += 2;

}

//void Nexus::BatchRenderer::DrawTriangle(const glm::vec3& a, const glm::vec3& b, const glm::vec3& c)
//{
//	if (m_CurrentVertexIndex + 3 >= s_VertexCount)
//		FlushToBuffer();
//
//	m_VertexData[m_CurrentVertexIndex] = a;
//	m_VertexData[m_CurrentVertexIndex + 1] = b;
//	m_VertexData[m_CurrentVertexIndex + 2] = c;
//
//	m_CurrentVertexIndex += 3;
//}

void Nexus::BatchRenderer::Flush(Ref<CommandQueue> queue, Camera* sceneCamera)
{
	auto buff = ResourcePool::Get()->GetUniformBuffer(m_UniformHandle.hashId);
	buff->Update(sceneCamera);
	
	queue->BindPipeline(m_pipeline);
	queue->BindShaderResourceHeap(m_Shader, m_HeapHandle, PipelineBindPoint::Graphics);

	for (uint32_t i = 0; i < m_CurrentBufferIndex; i++)
	{
		auto& vb = m_VertexBuffer[i];
		queue->BindVertexBuffer(vb);
		queue->DrawVertices(s_VertexCount, 1, 0, 0);
	}
	
	if(m_CurrentVertexIndex > 0)
	{
		auto& vb = m_VertexBuffer[m_CurrentBufferIndex];
		vb->Update(m_VertexData.data());

		queue->BindVertexBuffer(vb);
		queue->DrawVertices(m_CurrentVertexIndex, 1, 0, 0);
	}

	m_CurrentVertexIndex = 0;
	m_CurrentBufferIndex = 0;
}

void Nexus::BatchRenderer::InitImpl(Ref<Renderpass> renderpass)
{
	// Pipeline
	{
		ShaderSpecification shaderSpecs = ShaderCompiler::CompileFromFile("Resources/Shaders/batchRender.glsl");
		m_Shader = GraphicsInterface::CreateShader(shaderSpecs);

		std::vector<VertexBindInfo> pipelineVertexBindInfo(1);
		{
			pipelineVertexBindInfo[0].binding = 0;
			pipelineVertexBindInfo[0].inputRate = VertexBindInfo::INPUT_RATE_VERTEX;

			pipelineVertexBindInfo[0].stride = sizeof(glm::vec3);
		}

		std::vector<VertexAttribInfo> pipelineVertexAttribInfo(1);
		{
			pipelineVertexAttribInfo[0].binding = 0;
			pipelineVertexAttribInfo[0].location = 0;
			pipelineVertexAttribInfo[0].offset = 0;
			pipelineVertexAttribInfo[0].format = VertexAttribInfo::ATTRIB_FORMAT_VEC3;
		}

		GraphicsPipelineSpecification pipelineSpecs{};
		pipelineSpecs.shader = m_Shader;
		pipelineSpecs.renderpass = renderpass;
		pipelineSpecs.subpass = 0;
		pipelineSpecs.multisampled = true;
		pipelineSpecs.rasterizerInfo.cullMode = CullMode::None;
		pipelineSpecs.rasterizerInfo.frontFace = FrontFaceType::Clockwise;
		pipelineSpecs.rasterizerInfo.lineWidth = 1.f;
		pipelineSpecs.rasterizerInfo.topology = TopologyType::LineList;
		pipelineSpecs.vertexBindInfo = pipelineVertexBindInfo;
		pipelineSpecs.vertexAttribInfo = pipelineVertexAttribInfo;

		pipelineSpecs.rasterizerInfo.polygonMode = PolygonMode::Line;
		m_pipeline = GraphicsInterface::CreatePipeline(pipelineSpecs);

		m_HeapHandle.set = 0;
		m_HeapHandle.hashId = UUID();

		m_Shader->AllocateShaderResourceHeap(m_HeapHandle);

		m_UniformHandle.set = 0;
		m_UniformHandle.binding = 0;
		m_UniformHandle.hashId = UUID();

		auto buff = ResourcePool::Get()->AllocateUniformBuffer(m_Shader, m_UniformHandle);
		m_Shader->BindUniformWithResourceHeap(m_HeapHandle, m_UniformHandle.binding, buff);
	}

	// Buffer
	{
		s_Instance->m_VertexData.resize(s_VertexCount);

		s_Instance->bspecs.type = BufferType::Vertex;
		s_Instance->bspecs.data = nullptr;
		s_Instance->bspecs.cpuMemory = true;
		s_Instance->bspecs.size = (uint32_t) sizeof(glm::vec3) * s_Instance->m_VertexData.size(); // Triangle Count x 3 Vertices

		s_Instance->m_VertexBuffer.emplace_back(GraphicsInterface::CreateBuffer(s_Instance->bspecs));
	}
}

void Nexus::BatchRenderer::ShutImpl()
{
	ResourcePool::Get()->DeallocateUniformBuffer(m_UniformHandle.hashId);
	m_Shader->DeallocateShaderResourceHeap(m_HeapHandle);
}

void Nexus::BatchRenderer::FlushToBuffer()
{
	auto& vb = m_VertexBuffer[m_CurrentBufferIndex++];
	vb->Update(m_VertexData.data());

	m_CurrentVertexIndex = 0;
	if (m_CurrentBufferIndex >= m_VertexBuffer.size())
		m_VertexBuffer.emplace_back(GraphicsInterface::CreateBuffer(bspecs));
}

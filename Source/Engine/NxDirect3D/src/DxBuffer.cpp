#include "NxDirect3D/DxBuffer.h"
#include "NxDirect3D/DxContext.h"	

Nexus::DirectXBuffer::DirectXBuffer(const BufferSpecification& specs)
{
	m_Type = specs.type;

	auto device = DirectXContext::Get()->GetDevice();

	// Buffer
	{
		D3D12_HEAP_PROPERTIES props{};
		props.Type = D3D12_HEAP_TYPE_UPLOAD;
		props.CPUPageProperty = D3D12_CPU_PAGE_PROPERTY_UNKNOWN;
		props.MemoryPoolPreference = D3D12_MEMORY_POOL_UNKNOWN;
		props.CreationNodeMask = 1;
		props.VisibleNodeMask = 1;

		D3D12_RESOURCE_DESC desc{};
		desc.Dimension = D3D12_RESOURCE_DIMENSION_BUFFER;
		desc.Alignment = 0;
		desc.Width = specs.size;
		desc.Height = 1;
		desc.DepthOrArraySize = 1;
		desc.MipLevels = 1;
		desc.Format = DXGI_FORMAT_UNKNOWN;
		desc.SampleDesc.Count = 1;
		desc.SampleDesc.Quality = 0;
		desc.Layout = D3D12_TEXTURE_LAYOUT_ROW_MAJOR;
		desc.Flags = D3D12_RESOURCE_FLAG_NONE;

		if (m_Type == BufferType::Uniform || m_Type == BufferType::Storage)
			desc.Width = (desc.Width + 255) & ~255;

		m_Size = desc.Width;
		m_Stride = specs.stride;

		CHECK_CALL(device->CreateCommittedResource(&props, D3D12_HEAP_FLAG_NONE, &desc, D3D12_RESOURCE_STATE_GENERIC_READ, nullptr, __uuidof(ID3D12Resource), (void**)&m_Buffer));

		m_Buffer->SetName(ToWString(specs.debugName).c_str());
	}

	if (specs.data)
		Update(specs.data);
}

Nexus::DirectXBuffer::~DirectXBuffer()
{
	m_Buffer->Release();
}

void Nexus::DirectXBuffer::Update(void* data)
{
	if (!data) return;

	D3D12_RANGE range = { 0,0 };

	void* pData = nullptr;
	m_Buffer->Map(0, &range, &pData);
	memcpy(pData, data, m_Size);
	m_Buffer->Unmap(0, nullptr);
}

#include "NxDirect3D/DxFramebuffer.h"
#include "NxDirect3D/DxContext.h"
#include "NxDirect3D/DxSwapchain.h"

Nexus::DirectXFramebuffer::DirectXFramebuffer(const FramebufferSpecification& specs)
{
	auto device = DirectXContext::Get()->GetDevice();
	auto swapchain = DirectXSwapchain::Get();

	// Heap
	{
		D3D12_DESCRIPTOR_HEAP_DESC desc{};
		desc.NumDescriptors = specs.attachments.size();
		desc.Type = D3D12_DESCRIPTOR_HEAP_TYPE_RTV;
		desc.Flags = D3D12_DESCRIPTOR_HEAP_FLAG_NONE;

		CHECK_CALL(device->CreateDescriptorHeap(&desc, __uuidof(ID3D12DescriptorHeap), (void**)&m_Heap));
		
		m_IncrementalSize = device->GetDescriptorHandleIncrementSize(D3D12_DESCRIPTOR_HEAP_TYPE_RTV);
	}

	// Render Targets
	{
		D3D12_CPU_DESCRIPTOR_HANDLE dHandle{ m_Heap->GetCPUDescriptorHandleForHeapStart() };

		uint32_t FrameCount = swapchain->GetImageCount();

		for (auto& a : specs.attachments)
		{
			auto& attch = m_Attachments.emplace_back();

			for (auto i = 0; i < FrameCount; i++)
			{
				auto& r = attch.emplace_back();

				// Resource
				{
					D3D12_HEAP_PROPERTIES props{};
					props.Type = D3D12_HEAP_TYPE_DEFAULT;
					props.CPUPageProperty = D3D12_CPU_PAGE_PROPERTY_UNKNOWN;
					props.MemoryPoolPreference = D3D12_MEMORY_POOL_UNKNOWN;
					props.CreationNodeMask = 1;
					props.VisibleNodeMask = 1;

					D3D12_RESOURCE_DESC desc{};
					desc.Dimension = D3D12_RESOURCE_DIMENSION_TEXTURE2D;
					desc.Alignment = 0;
					desc.Width = specs.extent.width;
					desc.Height = specs.extent.height;
					desc.DepthOrArraySize = 1;
					desc.MipLevels = 1;
					desc.Layout = D3D12_TEXTURE_LAYOUT_ROW_MAJOR;
					desc.Flags = D3D12_RESOURCE_FLAG_ALLOW_RENDER_TARGET;

					switch (a.Type)
					{
					case FramebufferAttachmentType::Color:
						desc.Format = swapchain->GetFormat();
						break;
					case FramebufferAttachmentType::DepthStencil:
						break;
					case FramebufferAttachmentType::ShaderReadOnly_Color:
						desc.Format = swapchain->GetFormat();
						break;
					default:
						break;
					}

					CHECK_CALL(device->CreateCommittedResource(&props, D3D12_HEAP_FLAG_NONE, &desc, D3D12_RESOURCE_STATE_GENERIC_READ, nullptr, __uuidof(ID3D12Resource), (void**)&r));
				}

			}
		}
	}
}

Nexus::DirectXFramebuffer::~DirectXFramebuffer()
{
}

#pragma once

namespace Nexus
{
	class TransferCommandQueue
	{
	public:
		static Ref<TransferCommandQueue> Create();

		TransferCommandQueue() = default;
		virtual ~TransferCommandQueue() = default;

		virtual void Flush() = 0;
	};
}

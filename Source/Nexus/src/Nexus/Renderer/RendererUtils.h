#pragma once

namespace Nexus
{
	struct Viewport
	{
		float x, y, width, height, minDepth, maxDepth;
	};

	struct Offset
	{
		int x, y;
	};

	struct Extent
	{
		unsigned int width, height;
	};

	struct Scissor
	{
		Offset Offset;
		Extent Extent;
	};
}
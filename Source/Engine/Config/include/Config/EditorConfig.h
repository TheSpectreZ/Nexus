#pragma once
#include "Build.h"
#include <filesystem>
#include <string>

namespace Nexus::Config
{
	struct NEXUS_SERIALIZER_API PlatformConfig
	{
		uint32_t width;
		uint32_t height;
	};

	struct NEXUS_SERIALIZER_API GraphicsConfig
	{
		enum Attachment
		{
			COLOR, DEPTH, RESOLVE, SWAPCHAIN
		};

		struct Framebuffer
		{
			std::vector<Attachment> pAttachments;
		};

		uint32_t physicalDeviceIndex;
		Framebuffer frameBuffer;
	};

	struct NEXUS_SERIALIZER_API ProjectConfig
	{
		std::filesystem::path configPath;
		
		std::string Name;
		std::filesystem::path AssetDirectory;

		GraphicsConfig graphicsConfig;
	};

	struct NEXUS_SERIALIZER_API EditorConfig
	{
		ProjectConfig projectConfig;
		PlatformConfig platformConfig;
	};

	void SerializeConfig(EditorConfig* config, std::filesystem::path OutputPath);
	void DeserializeConfig(EditorConfig* config, std::filesystem::path InputPath);

}
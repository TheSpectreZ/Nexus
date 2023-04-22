#pragma once
#include "vulkan/vulkan.h"
#include "vma/vk_mem_alloc.h"
#include "vulkan/vk_enum_string_helper.h"

#include "Core/Assert.h"

static VkResult _VKR;

#define CLEAR_VKR _VKR = VK_SUCCESS
#define CHECK_LOG_VKR if(_VKR != VK_SUCCESS) { NEXUS_LOG_ERROR("VkResult: {0}",string_VkResult(_VKR)); CLEAR_VKR; }

#define CHECK_HANDLE(handle,type) if(handle == nullptr)\
{\
NEXUS_LOG_ERROR("{0}:{1} is Null",#type,#handle) \
CHECK_LOG_VKR \
NEXUS_ASSERT(1,"Vulkan Handle Error");\
}
#pragma once
#define NOMINMAX

#include "vulkan/vulkan.h"
#include "vma/vk_mem_alloc.h"
#include "vulkan/vk_enum_string_helper.h"

#include "NxCore/Assertion.h"

static VkResult _VKR;

#define CLEAR_VKR _VKR = VK_SUCCESS
#define CHECK_LOG_VKR NEXUS_ASSERT((_VKR != VK_SUCCESS),"VkResult: {0}",string_VkResult(_VKR)); CLEAR_VKR
#define CHECK_HANDLE(handle,type) CHECK_LOG_VKR; NEXUS_ASSERT((handle == nullptr),"%s:%s is Null",#type,#handle);
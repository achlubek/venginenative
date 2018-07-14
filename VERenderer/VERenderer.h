#pragma once

#include <string>
#include <vector>
#include <array>
#include <set>
#include <map>
#include <unordered_map>
#include <vulkan.h>
#include <GLFW/glfw3.h>

#include "Object3dInfo.h"
#include "VulkanBinaryBufferBuilder.h"
#include "VulkanComputeStage.h"
#include "VulkanDescriptorSet.h"
#include "VulkanDescriptorSetLayout.h"
#include "VulkanGenericBuffer.h"
#include "VulkanImage.h"
#include "VulkanRenderStage.h"
#include "VulkanShaderModule.h"
#include "VulkanToolkit.h"
#include "VulkanSwapChainOutput.h"
#include "VulkanAttachment.h"

#include "Object3dInfoFactory.h"
#include "VulkanShaderFactory.h"
#include "VulkanDescriptorSetLayoutFactory.h"
#include "VulkanRenderStageFactory.h"
#include "VulkanComputeStageFactory.h"
#include "VulkanBufferFactory.h"
#include "VulkanImageFactory.h"
#include "VulkanSwapChainOutputFactory.h"

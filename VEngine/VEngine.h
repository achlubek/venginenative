#pragma once

#include <string>
#include <vector>
#include <array>
#include <set>
#include <map>
#include <unordered_map>
#include <functional>
#include <vulkan.h>
#ifdef _MSC_VER
#include <windows.h>
#endif
#include <GLFW/glfw3.h>

#include "FileSystem/Media.h"

#include "Input/Joystick.h"
#include "Input/Keyboard.h"
#include "Input/Mouse.h"

#include "Renderer/Object3dInfo.h"
#include "Renderer/VulkanBinaryBufferBuilder.h"
#include "Renderer/VulkanComputeStage.h"
#include "Renderer/VulkanDescriptorSet.h"
#include "Renderer/VulkanDescriptorSetLayout.h"
#include "Renderer/VulkanGenericBuffer.h"
#include "Renderer/VulkanImage.h"
#include "Renderer/VulkanRenderStage.h"
#include "Renderer/VulkanShaderModule.h"
#include "Renderer/VulkanToolkit.h"
#include "Renderer/VulkanSemaphore.h"
#include "Renderer/VulkanSwapChainOutput.h"
#include "Renderer/VulkanAttachment.h"

#include "Renderer/Object3dInfoFactory.h"
#include "Renderer/VulkanShaderFactory.h"
#include "Renderer/VulkanDescriptorSetLayoutFactory.h"
#include "Renderer/VulkanRenderStageFactory.h"
#include "Renderer/VulkanComputeStageFactory.h"
#include "Renderer/VulkanBufferFactory.h"
#include "Renderer/VulkanImageFactory.h"
#include "Renderer/VulkanSemaphoreFactory.h"
#include "Renderer/VulkanSwapChainOutputFactory.h"

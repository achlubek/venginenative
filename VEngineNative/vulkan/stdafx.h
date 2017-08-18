// stdafx.h: do��cz plik do standardowych systemowych plik�w do��czanych,
// lub specyficzne dla projektu pliki do��czane, kt�re s� cz�sto wykorzystywane, ale
// s� rzadko zmieniane
//

#pragma once

#include "targetver.h"

#include <stdio.h>
#include <tchar.h>
#include <vector>
#include <fstream>
#include <array>
#include "vulkan.h"
#include <GLFW/glfw3.h>
#include <memory>
 

#include "VulkanShaderModule.h"
#include "VulkanAttachment.h"
#include "VulkanSubpass.h"
#include "VulkanRenderPass.h"
#include "VulkanFramebuffer.h"
#include "VulkanGraphicsPipeline.h"
#include "VulkanCommandBuffer.h"
#include "VulkanImage.h"
#include "VulkanGenericBuffer.h"
#include "VulkanDescriptorSet.h"
#include "VulkanDescriptorSetsManager.h"
#include "VulkanMesh3d.h"
#include "VulkanRenderStage.h"

#include "VulkanSwapChain.h"
#include "VulkanToolkit.h"

#include "VulkanBinaryBufferBuilder.h"

// TODO: W tym miejscu odwo�aj si� do dodatkowych nag��wk�w wymaganych przez program

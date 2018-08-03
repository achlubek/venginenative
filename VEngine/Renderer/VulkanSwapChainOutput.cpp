#include "stdafx.h"
#include "VulkanSwapChainOutput.h"
#include "Internal/VulkanDevice.h"
#include "Internal/VulkanInternalImage.h"
#include "VulkanRenderStage.h"
#include "VulkanImage.h"

namespace VEngine
{
    namespace Renderer
    {
        using namespace VEngine::Renderer::Internal;


        VulkanSwapChainOutput::VulkanSwapChainOutput(VulkanDevice* device, VulkanRenderStage* stage)
            : device(device)
        {
            VkFormat format = device->getSwapChain()->getImageFormat();
            outputStages.resize(device->getSwapChain()->getImagesCount());
            for (int i = 0; i < device->getSwapChain()->getImagesCount(); i++) {

                VulkanInternalImage* internalImage = new VulkanInternalImage(device, format, device->getSwapChain()->getImage(i), device->getSwapChain()->getImageView(i));
                VulkanImage* img = new VulkanImage(device, internalImage, format);
                VkClearColorValue clear = {};
                auto attachment = img->getAttachment(VulkanAttachmentBlending::None, true, clear, true);
                outputStages[i] = stage->copy({ attachment });
            }
            VkSemaphoreCreateInfo semaphoreInfo = {};
            semaphoreInfo.sType = VK_STRUCTURE_TYPE_SEMAPHORE_CREATE_INFO;
            vkCreateSemaphore(device->getDevice(), &semaphoreInfo, nullptr, &imageAvailableSemaphore);
        }


        VulkanSwapChainOutput::~VulkanSwapChainOutput()
        {
        }

        void VulkanSwapChainOutput::beginDrawing()
        {
            vkAcquireNextImageKHR(device->getDevice(), device->getSwapChain()->getHandle(),
                9999999, imageAvailableSemaphore, VK_NULL_HANDLE, &imageIndex);
            outputStages[imageIndex]->beginDrawing();
        }

        void VulkanSwapChainOutput::endDrawing()
        {
            outputStages[imageIndex]->endDrawing();
        }

        void VulkanSwapChainOutput::drawMesh(Object3dInfo * info, uint32_t instances)
        {
            outputStages[imageIndex]->drawMesh(info, instances);
        }

        void VulkanSwapChainOutput::submit(std::vector<VkSemaphore> waitSemaphores)
        {
            waitSemaphores.push_back(imageAvailableSemaphore);
            outputStages[imageIndex]->submit(waitSemaphores);
            device->getSwapChain()->present({ outputStages[imageIndex]->getSignalSemaphore() }, imageIndex);
            firstFrameDrawn = true;
            vkQueueWaitIdle(device->getMainQueue());// Todo make it synchronized better
        }

        std::vector<VkSemaphore> VulkanSwapChainOutput::getSignalSemaphores()
        { // Todo make it synchronized better
            if (!firstFrameDrawn) {
                return { };
            }
            else {
                return { };
            }
        }

    }
}
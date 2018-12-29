#include "stdafx.h"
#include "VulkanSwapChainOutput.h"
#include "Internal/VulkanDevice.h"
#include "Internal/VulkanInternalImage.h"
#include "VulkanRenderStage.h"
#include "VulkanImage.h"
#include "VulkanSemaphore.h"
#include "VulkanSemaphoreFactory.h"

namespace VEngine
{
    namespace Renderer
    {
        using namespace VEngine::Renderer::Internal;


        VulkanSwapChainOutput::VulkanSwapChainOutput(VulkanDevice* device, SemaphoreFactoryInterface* semaphoreFactory, RenderStageInterface* stage)
            : device(device)
        {
            VkFormat format = device->getSwapChain()->getImageFormat();
            outputStages.resize(device->getSwapChain()->getImagesCount());
            for (int i = 0; i < device->getSwapChain()->getImagesCount(); i++) {

                VulkanInternalImage* internalImage = new VulkanInternalImage(device, format, device->getSwapChain()->getImage(i), device->getSwapChain()->getImageView(i));
                VulkanImage* img = new VulkanImage(device, internalImage, format);
                VEngineClearColorValue clear = {};
                auto attachment = img->getAttachment(VEngineAttachmentBlending::None, true, clear, true);
                outputStages[i] = stage->copy({ attachment });
            }
            safedelete(stage); // this is clear but todo if its not confusing
            imageAvailableSemaphore = semaphoreFactory->build();
        }


        VulkanSwapChainOutput::~VulkanSwapChainOutput()
        {
            for (int i = 0; i < outputStages.size(); i++) {
                safedelete(outputStages[i]);
            }
            safedelete(imageAvailableSemaphore);
        }

        void VulkanSwapChainOutput::beginDrawing()
        {
            vkAcquireNextImageKHR(device->getDevice(), device->getSwapChain()->getHandle(),
                9999999, static_cast<VulkanSemaphore*>(imageAvailableSemaphore)->getHandle(), VK_NULL_HANDLE, &imageIndex);
            outputStages[imageIndex]->beginDrawing();
        }

        void VulkanSwapChainOutput::endDrawing()
        {
            outputStages[imageIndex]->endDrawing();
        }

        void VulkanSwapChainOutput::drawMesh(Object3dInfoInterface * info, uint32_t instances)
        {
            outputStages[imageIndex]->drawMesh(info, instances);
        }

        void VulkanSwapChainOutput::submit(std::vector<SemaphoreInterface*> waitSemaphores)
        {
            waitSemaphores.push_back(imageAvailableSemaphore);
            outputStages[imageIndex]->submit(waitSemaphores);
            device->getSwapChain()->present({ static_cast<VulkanSemaphore*>(outputStages[imageIndex]->getSignalSemaphore())->getHandle() }, imageIndex);
            firstFrameDrawn = true;
            vkQueueWaitIdle(device->getMainQueue());// Todo make it synchronized better
        }

        std::vector<SemaphoreInterface*> VulkanSwapChainOutput::getSignalSemaphores()
        { // Todo make it synchronized at least
            if (!firstFrameDrawn) {
                return { };
            }
            else {
                return { };
            }
        }

    }
}
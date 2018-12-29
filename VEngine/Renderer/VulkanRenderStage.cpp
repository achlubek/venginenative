#include "stdafx.h"
#include "VulkanRenderStage.h"
#include "Object3dInfo.h"
#include "VulkanDescriptorSetLayout.h"
#include "VulkanDescriptorSet.h"
#include "Object3dInfo.h"
#include "Internal/VulkanCommandBuffer.h"
#include "Internal/VulkanRenderPass.h"
#include "Internal/VulkanSubpass.h"
#include "Internal/VulkanGraphicsPipeline.h"
#include "Internal/VulkanVertexBuffer.h"
#include "Internal/VulkanFramebuffer.h"
#include "VulkanAttachment.h"
#include "VulkanImage.h"
#include "Internal/VulkanDevice.h"
#include "VulkanShaderModule.h"
#include "VulkanSemaphore.h"
#include "VulkanSemaphoreFactory.h"


namespace VEngine
{
    namespace Renderer
    {
        using namespace VEngine::Renderer::Internal;

        VulkanRenderStage::VulkanRenderStage(VulkanDevice * device, SemaphoreFactoryInterface* semaphoreFactory, int width, int height,
            std::vector<ShaderModuleInterface*> shaders,
            std::vector<DescriptorSetLayoutInterface*> layouts,
            std::vector<AttachmentInterface*> outputImages,
            VEngineCullMode cullMode)
            : device(device), semaphoreFactory(semaphoreFactory),
            width(width), height(height), setLayouts(layouts), 
            outputImages(outputImages), shaders(shaders), cullMode(cullMode)
        {
            sets = {};
            cullFlags = VK_CULL_MODE_NONE;
            if (cullMode == VEngineCullMode::CullModeFront) {
                cullFlags = VK_CULL_MODE_FRONT_BIT;
            }
            if (cullMode == VEngineCullMode::CullModeBack) {
                cullFlags = VK_CULL_MODE_BACK_BIT;
            }
        }


        VulkanRenderStage::~VulkanRenderStage()
        {
            if (compiled) {
                safedelete(signalSemaphore);
            }

            safedelete(renderPass);
            safedelete(framebuffer);

            safedelete(pipeline);
            safedelete(commandBuffer);
        }

        void VulkanRenderStage::beginDrawing()
        {
            if (!compiled) {
                compile();
            }
            commandBuffer->begin(VK_COMMAND_BUFFER_USAGE_SIMULTANEOUS_USE_BIT);

            VkRenderPassBeginInfo renderPassInfo = {};
            renderPassInfo.sType = VK_STRUCTURE_TYPE_RENDER_PASS_BEGIN_INFO;
            renderPassInfo.renderPass = renderPass->getHandle();
            renderPassInfo.framebuffer = framebuffer->getHandle();
            renderPassInfo.renderArea.offset = { 0, 0 };
            VkExtent2D extent = {};
            extent.width = width;
            extent.height = height;
            renderPassInfo.renderArea.extent = extent;
            std::vector<VkClearValue> clearValues = {};

            for (int i = 0; i < renderPass->getAttachments().size(); i++) {
                clearValues.push_back(VkClearValue());
                if (renderPass->getAttachments()[i]->isCleared()) {
                    if (renderPass->getAttachments()[i]->getImage()->isDepthBuffer()) {
                        clearValues[clearValues.size() - 1].depthStencil = { 1.0f, 0 };
                    }
                    else {
                        // todo maybe theres better way
                        std::memcpy(
                            &clearValues[clearValues.size() - 1].color, 
                            &renderPass->getAttachments()[i]->getClearColor(), 
                            sizeof(VkClearColorValue));
                    }
                }
            }

            renderPassInfo.clearValueCount = static_cast<uint32_t>(clearValues.size());
            renderPassInfo.pClearValues = clearValues.data();


            vkCmdBeginRenderPass(commandBuffer->getHandle(), &renderPassInfo, VK_SUBPASS_CONTENTS_INLINE);

            vkCmdBindPipeline(commandBuffer->getHandle(), VK_PIPELINE_BIND_POINT_GRAPHICS, pipeline->getPipeline());

        }
        void VulkanRenderStage::endDrawing()
        {
            vkCmdEndRenderPass(commandBuffer->getHandle());
            commandBuffer->end();
        }

        void VulkanRenderStage::setSets(std::vector<DescriptorSetInterface*> isets)
        {
            for (auto &set : isets) {
                sets.push_back(static_cast<VulkanDescriptorSet*>(set));
            }
        }

        void VulkanRenderStage::setSet(size_t index, DescriptorSetInterface * set)
        {
            if (sets.size() > index) sets[index] = static_cast<VulkanDescriptorSet*>(set);
            else sets.push_back(static_cast<VulkanDescriptorSet*>(set));
        }

        void VulkanRenderStage::drawMesh(Object3dInfoInterface * info, uint32_t instances)
        {
            static_cast<Object3dInfo*>(info)
                ->getVertexBuffer()
                ->drawInstanced(pipeline, sets, commandBuffer, instances);
        }

        void VulkanRenderStage::compile()
        {
            signalSemaphore = static_cast<VulkanSemaphore*>(semaphoreFactory->build());

            commandBuffer = new VulkanCommandBuffer(device, VK_COMMAND_BUFFER_LEVEL_PRIMARY);

            std::vector<VulkanAttachment*> attachments = {};
            std::vector<VkImageView> attachmentsViews = {};
            std::vector<VulkanAttachment*> colorAttachments = { };
            VulkanAttachment* depthAttachment = nullptr;
            std::vector<VkImageLayout> colorattachmentsLayouts = { };

            bool foundDepthBuffer = false;
            for (int i = 0; i < outputImages.size(); i++) {
                auto atta = outputImages[i];
                auto image = atta->getImage();
                auto view = static_cast<VulkanImage*>(image)->getFirstMipmapImageView();
                if (image->isDepthBuffer()) {
                    depthAttachment = static_cast<VulkanAttachment*>(atta);
                    foundDepthBuffer = true;
                }
                else {
                    colorAttachments.push_back(static_cast<VulkanAttachment*>(atta));
                    colorattachmentsLayouts.push_back(VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL);
                }
                attachments.push_back(static_cast<VulkanAttachment*>(atta));
                attachmentsViews.push_back(view);
            }

            VulkanSubpass subpass;
            if (foundDepthBuffer) subpass = VulkanSubpass(colorAttachments, colorattachmentsLayouts, depthAttachment, VK_IMAGE_LAYOUT_DEPTH_STENCIL_ATTACHMENT_OPTIMAL);
            else  subpass = VulkanSubpass(colorAttachments, colorattachmentsLayouts);

            std::vector<VulkanSubpass> subpasses = { subpass };

            renderPass = new VulkanRenderPass(device, attachments, subpasses);

            framebuffer = new VulkanFramebuffer(device, width, height, renderPass, attachmentsViews);

            std::vector<VkPipelineShaderStageCreateInfo> shaderStages = {};
            for (int i = 0; i < shaders.size(); i++) {
                auto shader = shaders[i];
                VkPipelineShaderStageCreateInfo info = {};
                info.sType = VK_STRUCTURE_TYPE_PIPELINE_SHADER_STAGE_CREATE_INFO;

                auto type = VK_SHADER_STAGE_VERTEX_BIT;
                if (shader->getType() == VEngineShaderModuleType::Fragment) {
                    type = VK_SHADER_STAGE_FRAGMENT_BIT;
                }
                if (shader->getType() == VEngineShaderModuleType::Compute) {
                    type = VK_SHADER_STAGE_COMPUTE_BIT;
                }

                info.stage = type;
                info.module = static_cast<VulkanShaderModule*>(shader)->getHandle();
                info.pName = "main";
                shaderStages.push_back(info);
            }

            std::vector<VulkanDescriptorSetLayout*> vulkanSetLayouts = {};
            for (auto &setLayout : setLayouts) {
                vulkanSetLayouts.push_back(static_cast<VulkanDescriptorSetLayout*>(setLayout));
            }
            
            pipeline = new VulkanGraphicsPipeline(device, width, height,
                vulkanSetLayouts, shaderStages, renderPass, foundDepthBuffer, topology, cullFlags);

            compiled = true;
        }

        VulkanRenderStage* VulkanRenderStage::copy()
        {
            auto v = new VulkanRenderStage(device, semaphoreFactory, width, height, shaders, setLayouts, outputImages, cullMode);
            v->setSets(sets);
            return v;
        }

        VulkanRenderStage * VulkanRenderStage::copy(std::vector<AttachmentInterface*> ioutputImages)
        {
            auto v = new VulkanRenderStage(device, semaphoreFactory, width, height, shaders, setLayouts, ioutputImages, cullMode);
            v->setSets(sets);
            return v;
        }

        SemaphoreInterface* VulkanRenderStage::getSignalSemaphore()
        {
            if (!compiled) {
                compile();
            }
            return signalSemaphore;
        }

        void VulkanRenderStage::submit(std::vector<SemaphoreInterface*> waitSemaphores)
        {
            std::vector<VkPipelineStageFlags> stageFlags = {};
            std::vector<VkSemaphore> vulkanSemaphores = {};
            for (int i = 0; i < waitSemaphores.size(); i++) {
                stageFlags.push_back(
                    VK_PIPELINE_STAGE_ALL_COMMANDS_BIT |
                    VK_PIPELINE_STAGE_ALL_GRAPHICS_BIT);
                vulkanSemaphores.push_back(static_cast<VulkanSemaphore*>(waitSemaphores[i])->getHandle());
            }
            VkSubmitInfo submitInfo = {};
            submitInfo.sType = VK_STRUCTURE_TYPE_SUBMIT_INFO;
            submitInfo.waitSemaphoreCount = static_cast<uint32_t>(vulkanSemaphores.size());
            submitInfo.pWaitSemaphores = vulkanSemaphores.data();
            submitInfo.pWaitDstStageMask = stageFlags.data();

            submitInfo.commandBufferCount = 1;
            auto cbufferHandle = commandBuffer->getHandle();
            submitInfo.pCommandBuffers = &cbufferHandle;

            submitInfo.signalSemaphoreCount = 1;
            submitInfo.pSignalSemaphores = static_cast<VulkanSemaphore*>(signalSemaphore)->getHandlePointer();

            if (vkQueueSubmit(device->getMainQueue(), 1, &submitInfo, VK_NULL_HANDLE) != VK_SUCCESS) {
                throw std::runtime_error("failed to submit draw command buffer!");
            }
        }

        void VulkanRenderStage::submitNoSemaphores(std::vector<SemaphoreInterface*> waitSemaphores)
        {
            std::vector<VkPipelineStageFlags> stageFlags = {};
            std::vector<VkSemaphore> vulkanSemaphores = {};
            for (int i = 0; i < waitSemaphores.size(); i++) {
                stageFlags.push_back(
                    VK_PIPELINE_STAGE_ALL_COMMANDS_BIT |
                    VK_PIPELINE_STAGE_ALL_GRAPHICS_BIT);
                vulkanSemaphores.push_back(static_cast<VulkanSemaphore*>(waitSemaphores[i])->getHandle());
            }
            VkSubmitInfo submitInfo = {};
            submitInfo.sType = VK_STRUCTURE_TYPE_SUBMIT_INFO;
            submitInfo.waitSemaphoreCount = static_cast<uint32_t>(vulkanSemaphores.size());
            submitInfo.pWaitSemaphores = vulkanSemaphores.data();
            submitInfo.pWaitDstStageMask = stageFlags.data();

            submitInfo.commandBufferCount = 1;
            auto cbufferHandle = commandBuffer->getHandle();
            submitInfo.pCommandBuffers = &cbufferHandle;

            submitInfo.signalSemaphoreCount = 0;
            submitInfo.pSignalSemaphores = nullptr;

            if (vkQueueSubmit(device->getMainQueue(), 1, &submitInfo, VK_NULL_HANDLE) != VK_SUCCESS) {
               throw std::runtime_error("failed to submit draw command buffer!");
            }
        }
    }
}
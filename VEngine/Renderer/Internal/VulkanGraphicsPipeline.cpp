#include "stdafx.h" 
#include "VulkanGraphicsPipeline.h" 
#include "VulkanRenderPass.h" 
#include "../VulkanAttachment.h"
#include "../VulkanImage.h" 
#include "VulkanDevice.h" 
#include "../VulkanDescriptorSetLayout.h" 
namespace VEngine
{
    namespace Renderer
    {
        namespace Internal
        {

            VkVertexInputBindingDescription bindingDescription;
            void getBindingDescription() {
                bindingDescription = {};
                bindingDescription.binding = 0;
                bindingDescription.stride = sizeof(float) * 12;
                bindingDescription.inputRate = VK_VERTEX_INPUT_RATE_VERTEX;
            }

            std::array<VkVertexInputAttributeDescription, 4> attributeDescriptions;
            void getAttributeDescriptions() {
                //std::array<VkVertexInputAttributeDescription, 4> attributeDescriptions = {};
                attributeDescriptions[0].binding = 0;
                attributeDescriptions[0].location = 0;
                attributeDescriptions[0].format = VK_FORMAT_R32G32B32_SFLOAT;
                attributeDescriptions[0].offset = sizeof(float) * 0;

                attributeDescriptions[1].binding = 0;
                attributeDescriptions[1].location = 1;
                attributeDescriptions[1].format = VK_FORMAT_R32G32_SFLOAT;
                attributeDescriptions[1].offset = sizeof(float) * 3;

                attributeDescriptions[2].binding = 0;
                attributeDescriptions[2].location = 2;
                attributeDescriptions[2].format = VK_FORMAT_R32G32B32_SFLOAT;
                attributeDescriptions[2].offset = sizeof(float) * 5;

                attributeDescriptions[3].binding = 0;
                attributeDescriptions[3].location = 3;
                attributeDescriptions[3].format = VK_FORMAT_R32G32B32A32_SFLOAT;
                attributeDescriptions[3].offset = sizeof(float) * 8;
            }

            VkPipelineVertexInputStateCreateInfo getVertexInputStateCreateInfo() {
                VkPipelineVertexInputStateCreateInfo vertexInputInfo = {};
                getBindingDescription();
                getAttributeDescriptions();

                vertexInputInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_VERTEX_INPUT_STATE_CREATE_INFO;
                vertexInputInfo.vertexBindingDescriptionCount = 1;
                vertexInputInfo.vertexAttributeDescriptionCount = static_cast<uint32_t>(attributeDescriptions.size());
                vertexInputInfo.pVertexBindingDescriptions = &bindingDescription;
                vertexInputInfo.pVertexAttributeDescriptions = attributeDescriptions.data();
                return vertexInputInfo;
            }

            VulkanGraphicsPipeline::VulkanGraphicsPipeline(VulkanDevice * device, int viewportwidth, int viewportheight, std::vector<VulkanDescriptorSetLayout*> setlayouts,
                std::vector<VkPipelineShaderStageCreateInfo> shaderstages, VulkanRenderPass* renderpass, bool enableDepthTest,
                VkPrimitiveTopology topology, VkCullModeFlags cullflags)
                : device(device)
            {
                VkPipelineVertexInputStateCreateInfo vertexInputInfo = getVertexInputStateCreateInfo();

                VkPipelineInputAssemblyStateCreateInfo inputAssembly = {};
                inputAssembly.sType = VK_STRUCTURE_TYPE_PIPELINE_INPUT_ASSEMBLY_STATE_CREATE_INFO;
                inputAssembly.topology = topology;
                inputAssembly.primitiveRestartEnable = VK_FALSE;

                VkViewport viewport = {};
                viewport.x = 0.0f;
                viewport.y = 0.0f;
                viewport.width = (float)viewportwidth;
                viewport.height = (float)viewportheight;
                viewport.minDepth = 0.0f;
                viewport.maxDepth = 1.0f;

                VkRect2D scissor = {};
                scissor.offset = { 0, 0 };
                scissor.extent = VkExtent2D();
                scissor.extent.width = viewportwidth;
                scissor.extent.height = viewportheight;

                VkPipelineViewportStateCreateInfo viewportState = {};
                viewportState.sType = VK_STRUCTURE_TYPE_PIPELINE_VIEWPORT_STATE_CREATE_INFO;
                viewportState.viewportCount = 1;
                viewportState.pViewports = &viewport;
                viewportState.scissorCount = 1;
                viewportState.pScissors = &scissor;

                VkPipelineRasterizationStateCreateInfo rasterizer = {};
                rasterizer.sType = VK_STRUCTURE_TYPE_PIPELINE_RASTERIZATION_STATE_CREATE_INFO;
                rasterizer.depthClampEnable = VK_TRUE;
                rasterizer.rasterizerDiscardEnable = VK_FALSE;
                rasterizer.polygonMode = VK_POLYGON_MODE_FILL;
                rasterizer.lineWidth = 1.0f;
                rasterizer.cullMode = cullflags;// enableDepthTest ? VK_CULL_MODE_FRONT_BIT : 0;
                rasterizer.frontFace = VK_FRONT_FACE_CLOCKWISE;
                rasterizer.depthBiasEnable = VK_FALSE;

                VkPipelineMultisampleStateCreateInfo multisampling = {};
                multisampling.sType = VK_STRUCTURE_TYPE_PIPELINE_MULTISAMPLE_STATE_CREATE_INFO;
                multisampling.sampleShadingEnable = VK_FALSE;
                multisampling.rasterizationSamples = VK_SAMPLE_COUNT_1_BIT;

                VkPipelineDepthStencilStateCreateInfo depthStencil = {};
                depthStencil.sType = VK_STRUCTURE_TYPE_PIPELINE_DEPTH_STENCIL_STATE_CREATE_INFO;
                depthStencil.depthTestEnable = enableDepthTest ? VK_TRUE : VK_FALSE;
                depthStencil.depthWriteEnable = enableDepthTest ? VK_TRUE : VK_FALSE;
                depthStencil.depthCompareOp = enableDepthTest ? VK_COMPARE_OP_LESS : VK_COMPARE_OP_ALWAYS;
                depthStencil.depthBoundsTestEnable = VK_FALSE;
                depthStencil.stencilTestEnable = VK_FALSE;

                std::vector<VkPipelineColorBlendAttachmentState> atts = {  };


                for (int i = 0; i < renderpass->getAttachments().size(); i++) {
                    if (!renderpass->getAttachments()[i]->getImage()->isDepthBuffer()) {
                        VkPipelineColorBlendAttachmentState colorBlendAttachment = {};
                        colorBlendAttachment.colorWriteMask = VK_COLOR_COMPONENT_R_BIT | VK_COLOR_COMPONENT_G_BIT | VK_COLOR_COMPONENT_B_BIT | VK_COLOR_COMPONENT_A_BIT;

                        if (renderpass->getAttachments()[i]->getBlending() == VulkanAttachmentBlending::Alpha) {
                            colorBlendAttachment.blendEnable = VK_TRUE;
                            colorBlendAttachment.srcColorBlendFactor = VK_BLEND_FACTOR_SRC_ALPHA;
                            colorBlendAttachment.dstColorBlendFactor = VK_BLEND_FACTOR_ONE_MINUS_SRC_ALPHA;
                            colorBlendAttachment.colorBlendOp = VK_BLEND_OP_ADD;
                            colorBlendAttachment.srcAlphaBlendFactor = VK_BLEND_FACTOR_SRC_ALPHA;
                            colorBlendAttachment.dstAlphaBlendFactor = VK_BLEND_FACTOR_ONE_MINUS_SRC_ALPHA;
                            colorBlendAttachment.alphaBlendOp = VK_BLEND_OP_ADD;
                        }
                        else if (renderpass->getAttachments()[i]->getBlending() == VulkanAttachmentBlending::Additive) {
                            colorBlendAttachment.blendEnable = VK_TRUE;
                            colorBlendAttachment.srcColorBlendFactor = VK_BLEND_FACTOR_ONE;
                            colorBlendAttachment.dstColorBlendFactor = VK_BLEND_FACTOR_ONE;
                            colorBlendAttachment.colorBlendOp = VK_BLEND_OP_ADD;
                            colorBlendAttachment.srcAlphaBlendFactor = VK_BLEND_FACTOR_ONE;
                            colorBlendAttachment.dstAlphaBlendFactor = VK_BLEND_FACTOR_ONE;
                            colorBlendAttachment.alphaBlendOp = VK_BLEND_OP_ADD;
                        }
                        else {
                            colorBlendAttachment.blendEnable = VK_FALSE;
                        }

                        atts.push_back(colorBlendAttachment);
                    }
                }


                VkPipelineColorBlendStateCreateInfo colorBlending = {};
                colorBlending.sType = VK_STRUCTURE_TYPE_PIPELINE_COLOR_BLEND_STATE_CREATE_INFO;
                colorBlending.logicOpEnable = VK_FALSE;
                colorBlending.logicOp = VK_LOGIC_OP_COPY;
                colorBlending.attachmentCount = static_cast<uint32_t>(atts.size());
                colorBlending.pAttachments = atts.data();
                colorBlending.blendConstants[0] = 1.0f;
                colorBlending.blendConstants[1] = 1.0f;
                colorBlending.blendConstants[2] = 1.0f;
                colorBlending.blendConstants[3] = 1.0f;

                std::vector<VkDescriptorSetLayout> layouts = {};
                for (int i = 0; i < setlayouts.size(); i++) {
                    layouts.push_back(setlayouts[i]->getHandle());
                }

                VkPipelineLayoutCreateInfo pipelineLayoutInfo = {};
                pipelineLayoutInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_LAYOUT_CREATE_INFO;
                pipelineLayoutInfo.setLayoutCount = static_cast<uint32_t>(layouts.size());
                pipelineLayoutInfo.pSetLayouts = layouts.data();
                pipelineLayoutInfo.pushConstantRangeCount = 0;

                if (vkCreatePipelineLayout(device->getDevice(), &pipelineLayoutInfo, nullptr, &pipelineLayout) != VK_SUCCESS) {
                    throw std::runtime_error("failed to create pipeline layout!");
                }

                VkGraphicsPipelineCreateInfo pipelineInfo = {};
                pipelineInfo.sType = VK_STRUCTURE_TYPE_GRAPHICS_PIPELINE_CREATE_INFO;
                pipelineInfo.stageCount = static_cast<uint32_t>(shaderstages.size());
                pipelineInfo.pStages = shaderstages.data();
                pipelineInfo.pVertexInputState = &vertexInputInfo;
                pipelineInfo.pInputAssemblyState = &inputAssembly;
                pipelineInfo.pViewportState = &viewportState;
                pipelineInfo.pRasterizationState = &rasterizer;
                pipelineInfo.pMultisampleState = &multisampling;
                pipelineInfo.pDepthStencilState = &depthStencil;
                pipelineInfo.pColorBlendState = &colorBlending;
                pipelineInfo.layout = pipelineLayout;
                pipelineInfo.renderPass = renderpass->getHandle();
                pipelineInfo.subpass = 0;
                pipelineInfo.basePipelineHandle = VK_NULL_HANDLE;

                if (vkCreateGraphicsPipelines(device->getDevice(), VK_NULL_HANDLE, 1, &pipelineInfo, nullptr, &graphicsPipeline) != VK_SUCCESS) {
                    throw std::runtime_error("failed to create graphics pipeline!");
                }
            }

            VulkanGraphicsPipeline::VulkanGraphicsPipeline(VulkanDevice * device, std::vector<VulkanDescriptorSetLayout*> setlayouts,
                VkPipelineShaderStageCreateInfo shader)
            {
                std::vector<VkDescriptorSetLayout> layouts = {};
                for (int i = 0; i < setlayouts.size(); i++) {
                    layouts.push_back(setlayouts[i]->getHandle());
                }

                VkPipelineLayoutCreateInfo pipelineLayoutInfo = {};
                pipelineLayoutInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_LAYOUT_CREATE_INFO;
                pipelineLayoutInfo.setLayoutCount = static_cast<uint32_t>(layouts.size());
                pipelineLayoutInfo.pSetLayouts = layouts.data();
                pipelineLayoutInfo.pushConstantRangeCount = 0;

                if (vkCreatePipelineLayout(device->getDevice(), &pipelineLayoutInfo, nullptr, &pipelineLayout) != VK_SUCCESS) {
                    throw std::runtime_error("failed to create pipeline layout!");
                }

                VkComputePipelineCreateInfo pipelineInfo = {};
                pipelineInfo.sType = VK_STRUCTURE_TYPE_COMPUTE_PIPELINE_CREATE_INFO;
                pipelineInfo.stage = shader;
                pipelineInfo.layout = pipelineLayout;
                pipelineInfo.basePipelineHandle = VK_NULL_HANDLE;
                // todo maybe exclude it from this class
                if (vkCreateComputePipelines(device->getDevice(), VK_NULL_HANDLE, 1, &pipelineInfo, nullptr, &graphicsPipeline) != VK_SUCCESS) {
                    throw std::runtime_error("failed to create compute pipeline!");
                }
            }


            VulkanGraphicsPipeline::~VulkanGraphicsPipeline()
            {
                vkDestroyPipeline(device->getDevice(), graphicsPipeline, nullptr);
                vkDestroyPipelineLayout(device->getDevice(), pipelineLayout, nullptr);
            }

            VkPipeline VulkanGraphicsPipeline::getPipeline()
            {
                return graphicsPipeline;
            }

            VkPipelineLayout VulkanGraphicsPipeline::getPipelineLayout()
            {
                return pipelineLayout;
            }

        }
    }
}
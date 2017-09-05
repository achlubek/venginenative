#include "stdafx.h"
#include "UIRenderer.h"
#include "vulkan.h"


UIRenderer::UIRenderer(VulkanToolkit* ivulkan, int iwidth, int iheight) :
    width(iwidth), height(iheight), vulkan(ivulkan)
{
    VulkanRenderStage* stage = new VulkanRenderStage();

    outputImage = new VulkanImage(width, height, VK_FORMAT_R8G8B8A8_UNORM, VK_IMAGE_TILING_OPTIMAL,
        VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT | VK_IMAGE_USAGE_SAMPLED_BIT, VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT, VK_IMAGE_ASPECT_COLOR_BIT, VK_IMAGE_LAYOUT_PREINITIALIZED, false);

    auto vert = VulkanShaderModule("../../shaders/compiled/ui.vert.spv");
    auto frag = VulkanShaderModule("../../shaders/compiled/ui.frag.spv");

    auto layout = new VulkanDescriptorSetLayout();
    layout->addField(0, VK_DESCRIPTOR_TYPE_STORAGE_BUFFER, VK_SHADER_STAGE_ALL_GRAPHICS);
    layout->compile();

    stage = new VulkanRenderStage();
    stage->setViewport(width, height);
    stage->addShaderStage(vert.createShaderStage(VK_SHADER_STAGE_VERTEX_BIT, "main"));
    stage->addShaderStage(frag.createShaderStage(VK_SHADER_STAGE_FRAGMENT_BIT, "main"));
    stage->addDescriptorSetLayout(layout->layout);
    stage->addOutputImage(*outputImage);
    stage->alphaBlending = true; //yisss

    auto buffer = VulkanGenericBuffer(VK_BUFFER_USAGE_STORAGE_BUFFER_BIT, sizeof(float) * 1024 * 10);

    set = layout->generateDescriptorSet();
    set.bindUniformBuffer(0, buffer);
    set.update();

    renderer = new VulkanRenderer();
    renderer->setMeshStage(stage);
    renderer->compile();
}

/*
buffer struct layout for elements

*/

void UIRenderer::draw() {
    // update buffers.........
    for (int i = 0; i < boxes.size(); i++) {
        boxes[i]->draw();
    }

    renderer->beginDrawing();
    // and then this beast
    stage->drawMesh(vulkan->fullScreenQuad3dInfo, { set }, boxes.size() + texts.size() + bitmaps.size());
    renderer->endDrawing();
}

UIRenderer::~UIRenderer()
{
}

#include "stdafx.h"
#include "UIRenderer.h"
#include "vulkan.h"
#include "UIBox.h"
#include "UIBitmap.h"
#include "UIText.h"


UIRenderer::UIRenderer(VulkanToolkit* ivulkan, int iwidth, int iheight) :
    width(iwidth), height(iheight), vulkan(ivulkan)
{

    ImageData *img = new ImageData();
    img->width = 1;
    img->height = 1;
    unsigned char * emptytexture = new unsigned char[4]{ (unsigned char)0x255, (unsigned char)0x255, (unsigned char)0x255, (unsigned char)0x255 };
    img->data = (void*)emptytexture;
    dummyTexture = vulkan->createTexture(img);

    layout = new VulkanDescriptorSetLayout();
    layout->addField(0, VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER, VK_SHADER_STAGE_ALL_GRAPHICS);
    layout->addField(1, VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER, VK_SHADER_STAGE_FRAGMENT_BIT);
    layout->compile();

    VulkanRenderStage* stage = new VulkanRenderStage();

    outputImage = new VulkanImage(width, height, VK_FORMAT_R8G8B8A8_UNORM, VK_IMAGE_TILING_OPTIMAL,
        VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT | VK_IMAGE_USAGE_SAMPLED_BIT, VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT, VK_IMAGE_ASPECT_COLOR_BIT, VK_IMAGE_LAYOUT_PREINITIALIZED, false);

    auto vert = VulkanShaderModule("../../shaders/compiled/ui.vert.spv");
    auto frag = VulkanShaderModule("../../shaders/compiled/ui.frag.spv");
    
    stage = new VulkanRenderStage();
    stage->setViewport(width, height);
    stage->addShaderStage(vert.createShaderStage(VK_SHADER_STAGE_VERTEX_BIT, "main"));
    stage->addShaderStage(frag.createShaderStage(VK_SHADER_STAGE_FRAGMENT_BIT, "main"));
    stage->addDescriptorSetLayout(layout->layout);
    stage->addOutputImage(*outputImage);
    stage->alphaBlending = true; //yisss
    
    renderer = new VulkanRenderer();
    renderer->setMeshStage(stage);
    renderer->compile();
}

/*
buffer struct layout for elements

*/

void UIRenderer::draw() {
    // update buffers........
    for (int i = 0; i < boxes.size(); i++) {
        boxes[i]->updateBuffer();
    }
    renderer->beginDrawing();

    auto stage = renderer->getMesh3dStage();
    for (int i = 0; i < boxes.size(); i++) {
        stage->drawMesh(vulkan->fullScreenQuad3dInfo, { boxes[i]->set }, 1);
    }

    // and then this beast
    //stage->drawMesh(vulkan->fullScreenQuad3dInfo, { set }, boxes.size() + texts.size() + bitmaps.size());
    renderer->endDrawing();
}

UIRenderer::~UIRenderer()
{
}

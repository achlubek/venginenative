#include "stdafx.h"
#include "UIRenderer.h"
#include "vulkan.h"
#include "UIBox.h"
#include "UIBitmap.h"
#include "UIText.h"
#include "Mouse.h"


UIRenderer::UIRenderer(VulkanToolkit* vulkan, Mouse* mouse, VulkanImage* outputImage, int width, int height) :
    vulkan(vulkan), width(width), height(height), mouse(mouse)
{
    unsigned char * emptytexture = new unsigned char[4]{ (unsigned char)0x255, (unsigned char)0x255, (unsigned char)0x255, (unsigned char)0x255 };
    dummyTexture = vulkan->getVulkanImageFactory()->build(1, 1, 4, emptytexture);

    layout = vulkan->getVulkanDescriptorSetLayoutFactory()->build();
    layout->addField(VulkanDescriptorSetFieldType::FieldTypeUniformBuffer, VulkanDescriptorSetFieldStage::FieldStageAll);
    layout->addField(VulkanDescriptorSetFieldType::FieldTypeSampler, VulkanDescriptorSetFieldStage::FieldStageFragment);

    auto vert = vulkan->getVulkanShaderFactory()->build(VulkanShaderModuleType::Vertex, "ui.vert.spv");
    auto frag = vulkan->getVulkanShaderFactory()->build(VulkanShaderModuleType::Fragment, "ui.frag.spv");

    auto stage = vulkan->getVulkanRenderStageFactory()->build(width, height, { vert, frag }, { layout },
        { outputImage->getAttachment(VulkanAttachmentBlending::Alpha, true,{ { 0.5f, 0.5f, 0.5f, 1.f } }, false)  });

    mouse->onMouseDown.add([&](int a) {
        auto cursor = mouse->getCursorPosition();
        auto hits = rayCast(static_cast<float>(std::get<0>(cursor)) / width, static_cast<float>(std::get<1>(cursor)) / height);
        if (hits.size() > 0) {
            hits[0]->onMouseDown.invoke(0);
        }
    });

    mouse->onMouseUp.add([&](int a) {
        auto cursor = mouse->getCursorPosition();
        auto hits = rayCast(static_cast<float>(std::get<0>(cursor)) / width, static_cast<float>(std::get<1>(cursor)) / height);
        if (hits.size() > 0 ) {
            hits[0]->onMouseUp.invoke(0);
        }
    });
}


void UIRenderer::draw() {

    for (int a = 0; a < drawables.size(); a++) {
        for (int b = 0; b < drawables.size(); b++) {
            auto index_a = drawables[a]->zIndex;
            auto index_b = drawables[b]->zIndex;
            if (index_a > index_b) {
                auto tmp = drawables[b];
                drawables[b] = drawables[a];
                drawables[a] = tmp;
            }
        }
    }

    for (int i = 0; i < drawables.size(); i++) {
        drawables[i]->updateBuffer();
    }

    stage->beginDrawing();

    for (int i = 0; i < drawables.size(); i++) {
        drawables[i]->draw(stage);
    }

    stage->endDrawing();
}

void UIRenderer::addDrawable(UIAbsDrawable * drawable)
{
    drawables.push_back(drawable);
}

void UIRenderer::removeDrawable(UIAbsDrawable * drawable)
{
    auto found = std::find(drawables.begin(), drawables.end(), drawable);
    if (found != drawables.end()) {
        drawables.erase(found);
    }
}

std::vector<UIAbsDrawable*> UIRenderer::getDrawables()
{
    return drawables;
}

void UIRenderer::removeAllDrawables()
{
    drawables.clear();
} 

std::vector<UIAbsDrawable*> UIRenderer::rayCast(float x, float y)
{
    auto result = std::vector<UIAbsDrawable*>();
    for (int a = 0; a < drawables.size(); a++) {
        if (
            x > drawables[a]->x 
            && x < (drawables[a]->x + drawables[a]->width)
            && y > drawables[a]->y 
            && y < (drawables[a]->y + drawables[a]->height)) {
            result.push_back(drawables[a]);
        }
    }
    return result;
}

VulkanToolkit * UIRenderer::getToolkit()
{
    return vulkan;
}

uint32_t UIRenderer::getWidth()
{
    return width;
}

uint32_t UIRenderer::getHeight()
{
    return height;
}

VulkanDescriptorSetLayout * UIRenderer::getSetLayout()
{
    return layout;
}

VulkanImage * UIRenderer::getDummyTexture()
{
    return dummyTexture;
}

UIRenderer::~UIRenderer()
{
}

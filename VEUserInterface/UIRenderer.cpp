#include "stdafx.h"
#include "UIRenderer.h"
#include "vulkan.h"
#include "UIBox.h"
#include "UIBitmap.h"
#include "UIText.h"
#include "Mouse.h"


UIRenderer::UIRenderer(VulkanToolkit* ivulkan, Mouse* imouse, VulkanImage* outputImage, int iwidth, int iheight) :
    vulkan(ivulkan), width(iwidth), height(iheight), mouse(imouse)
{

    ImageData img = ImageData();
    img.width = 1;
    img.height = 1;
    img.channelCount = 4;
    unsigned char * emptytexture = new unsigned char[4]{ (unsigned char)0x255, (unsigned char)0x255, (unsigned char)0x255, (unsigned char)0x255 };
    img.data = (void*)emptytexture;
    dummyTexture = vulkan->createTexture(img, VK_FORMAT_R8G8B8A8_UNORM);

    layout = new VulkanDescriptorSetLayout(vulkan);
    layout->addField(0, VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER, VK_SHADER_STAGE_ALL_GRAPHICS);
    layout->addField(1, VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER, VK_SHADER_STAGE_FRAGMENT_BIT);
    layout->compile();

    VulkanRenderStage* stage = new VulkanRenderStage(vulkan);

    auto vert = new VulkanShaderModule(vulkan, "../../shaders/compiled/ui.vert.spv");
    auto frag = new VulkanShaderModule(vulkan, "../../shaders/compiled/ui.frag.spv");
    
    stage = new VulkanRenderStage(vulkan);
    stage->setViewport(width, height);
    stage->addShaderStage(vert->createShaderStage(VK_SHADER_STAGE_VERTEX_BIT, "main"));
    stage->addShaderStage(frag->createShaderStage(VK_SHADER_STAGE_FRAGMENT_BIT, "main"));
    stage->addDescriptorSetLayout(layout->layout);
    stage->addOutputImage(outputImage);
    stage->alphaBlending = true;
    
    renderer = new VulkanRenderer(vulkan);
    renderer->setMeshStage(stage);
    renderer->compile();

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

    renderer->beginDrawing();

    auto stage = renderer->getMesh3dStage();
    for (int i = 0; i < drawables.size(); i++) {
        drawables[i]->draw(stage);
    }

    renderer->endDrawing();
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

UIRenderer::~UIRenderer()
{
}

// Example_SimpleFullScreenPass.cpp: Okreœla punkt wejœcia dla aplikacji konsoli.
//

#include "stdafx.h"


int main()
{
    auto toolkit = new VulkanToolkit();
    toolkit->initialize(640, 480);

    VkExtent2D ext;
    ext.width = toolkit->windowWidth;
    ext.height = toolkit->windowHeight;

    auto vert = VulkanShaderModule("../../shaders/compiled/example-simplefullscreenpass.vert.spv");
    auto frag = VulkanShaderModule("../../shaders/compiled/example-simplefullscreenpass.frag.spv");

    auto layout = new VulkanDescriptorSetLayout();
    layout->addField(0, VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER, VK_SHADER_STAGE_ALL_GRAPHICS);
    layout->compile();

    auto stage = new VulkanRenderStage();
    stage->setViewport(ext);
    stage->addShaderStage(vert.createShaderStage(VK_SHADER_STAGE_VERTEX_BIT, "main"));
    stage->addShaderStage(frag.createShaderStage(VK_SHADER_STAGE_FRAGMENT_BIT, "main"));
    stage->addDescriptorSetLayout(layout->layout);

    auto buffer = VulkanGenericBuffer(VK_BUFFER_USAGE_UNIFORM_BUFFER_BIT, sizeof(float) * 20);

    auto set = layout->generateDescriptorSet();
    set.bindUniformBuffer(0, buffer); 
    set.update();

    auto renderer = new VulkanRenderer(); 
    renderer->setOutputStage(stage);
    renderer->setPostProcessingDescriptorSet(&set);
    renderer->compile();

    while (!toolkit->shouldCloseWindow()) {
        void* ptr;
        buffer.map(0, sizeof(float) * 1, &ptr);
        ((float*)ptr)[0] = (float)toolkit->getExecutionTime();
        buffer.unmap();
        renderer->beginDrawing();
        // no meshes drawn, only post process which is handled automatically
        renderer->endDrawing();
        toolkit->poolEvents();
    }

    return 0;
}


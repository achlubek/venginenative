// Example_SimpleFullScreenPass.cpp: Okreœla punkt wejœcia dla aplikacji konsoli.
//

#include "stdafx.h"


int main()
{
    auto toolkit = new VulkanToolkit(640, 480, true, "Full screen pass example");
    auto device = toolkit->getDevice();
    
    auto vert = new VulkanShaderModule(device, VulkanShaderModuleType::Vertex, "../../shaders/compiled/example-simplefullscreenpass.vert.spv");
    auto frag = new VulkanShaderModule(device, VulkanShaderModuleType::Fragment, "../../shaders/compiled/example-simplefullscreenpass.frag.spv");

    auto layout = new VulkanDescriptorSetLayout(device);
    layout->addField(0, VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER, VK_SHADER_STAGE_ALL_GRAPHICS);
    layout->compile();

    auto stage = new VulkanRenderStage(device, 640, 480, { vert, frag }, { layout }, {});

    auto buffer = new VulkanGenericBuffer(device, VK_BUFFER_USAGE_UNIFORM_BUFFER_BIT, sizeof(float) * 20);

    auto set = layout->generateDescriptorSet();
    set->bindUniformBuffer(0, buffer); 
    set->update();

    stage->setSets({ set });

    auto output = new VulkanSwapChainOutput(device, stage);

    while (!toolkit->shouldCloseWindow()) {

        void* ptr;
        buffer->map(0, sizeof(float) * 1, &ptr);
        ((float*)ptr)[0] = (float)toolkit->getExecutionTime();
        buffer->unmap();

        output->beginDrawing();
        // no meshes drawn, only post process which is handled automatically
        output->endDrawing();
        output->submit({});

        toolkit->poolEvents();
    }

    return 0;
}


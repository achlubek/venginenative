// Example_SimpleFullScreenPass.cpp: Okreœla punkt wejœcia dla aplikacji konsoli.
//

#include "stdafx.h"

#include "Media.h"

int main()
{
    Media::loadFileMap("../../media");
    Media::loadFileMap("../../shaders");
    auto toolkit = new VulkanToolkit(640, 480, true, "Full screen pass example");
    auto device = toolkit->getDevice();
    
    auto vert = toolkit->getVulkanShaderFactory()->build(VulkanShaderModuleType::Vertex, "example-simplefullscreenpass.vert.spv");
    auto frag = toolkit->getVulkanShaderFactory()->build(VulkanShaderModuleType::Fragment, "example-simplefullscreenpass.frag.spv");

    auto texture = toolkit->getVulkanImageFactory()->build("blaze.png");

    auto layout = toolkit->getVulkanDescriptorSetLayoutFactory()->build();
    layout->addField(VulkanDescriptorSetFieldType::FieldTypeUniformBuffer, VulkanDescriptorSetFieldStage::FieldStageAll);
    layout->addField(VulkanDescriptorSetFieldType::FieldTypeSampler, VulkanDescriptorSetFieldStage::FieldStageFragment);
    layout->compile();

    auto stage = toolkit->getVulkanRenderStageFactory()->build(640, 480, { vert, frag }, { layout }, {});

    auto buffer = toolkit->getVulkanBufferFactory()->build(VulkanBufferType::BufferTypeUniform, sizeof(float) * 20);

    auto set = layout->generateDescriptorSet();
    set->bindBuffer(0, buffer);
    set->bindImageViewSampler(1, texture);
    set->update();

    stage->setSets({ set });

    auto output = toolkit->getVulkanSwapChainOutputFactory()->build(stage);

    auto fullScreenQuad3dInfo = toolkit->getObject3dInfoFactory()->getFullScreenQuad();

    while (!toolkit->shouldCloseWindow()) {

        void* ptr;
        buffer->map(0, sizeof(float) * 1, &ptr);
        ((float*)ptr)[0] = (float)toolkit->getExecutionTime();
        buffer->unmap();

        output->beginDrawing();
        // no meshes drawn, only post process which is handled automatically
        output->drawMesh(fullScreenQuad3dInfo, 1);
        output->endDrawing();
        output->submit({});

        toolkit->poolEvents();
    }

    return 0;
}


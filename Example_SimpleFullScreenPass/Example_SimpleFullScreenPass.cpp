#include "stdafx.h"

using namespace VEngine::Renderer;

int main()
{
    auto toolkit = new VulkanToolkit(640, 480, true, "Full screen pass example");
    toolkit->getMedia()->scanDirectory("../../media");
    toolkit->getMedia()->scanDirectory("../../shaders");
    
    auto vert = toolkit->getShaderFactory()->build(VEngineShaderModuleType::Vertex, "example-simplefullscreenpass.vert.spv");
    auto frag = toolkit->getShaderFactory()->build(VEngineShaderModuleType::Fragment, "example-simplefullscreenpass.frag.spv");

    auto texture = toolkit->getImageFactory()->build("blaze.png");

    auto layout = toolkit->getDescriptorSetLayoutFactory()->build();
    layout->addField(VEngineDescriptorSetFieldType::FieldTypeUniformBuffer, VEngineDescriptorSetFieldStage::FieldStageAllGraphics);
    layout->addField(VEngineDescriptorSetFieldType::FieldTypeSampler, VEngineDescriptorSetFieldStage::FieldStageFragment);

    auto stage = toolkit->getRenderStageFactory()->build(640, 480, { vert, frag }, { layout });

    auto buffer = toolkit->getBufferFactory()->build(VEngineBufferType::BufferTypeUniform, sizeof(float) * 20);

    auto set = layout->generateDescriptorSet();
    set->bindBuffer(0, buffer);
    set->bindImageViewSampler(1, texture);

    stage->setSets({ set });

    auto output = toolkit->getSwapChainOutputFactory()->build(stage);

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


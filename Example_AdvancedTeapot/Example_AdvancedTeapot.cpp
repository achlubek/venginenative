#include "stdafx.h"

using namespace VEngine::Renderer;

RenderStageInterface* createTeapotStage(VulkanToolkit* vulkan, GenericBufferInterface* buffer, ImageInterface* texture, ImageInterface* colorImage) {
    auto vertTeapot = vulkan->getShaderFactory()->build(VEngineShaderModuleType::Vertex, "example-advancedteapot-teapot.vert.spv");
    auto fragTeapot = vulkan->getShaderFactory()->build(VEngineShaderModuleType::Fragment, "example-advancedteapot-teapot.frag.spv");

    auto layoutTeapot = vulkan->getDescriptorSetLayoutFactory()->build();
    layoutTeapot->addField(VEngineDescriptorSetFieldType::FieldTypeUniformBuffer, VEngineDescriptorSetFieldStage::FieldStageAllGraphics);
    layoutTeapot->addField(VEngineDescriptorSetFieldType::FieldTypeSampler, VEngineDescriptorSetFieldStage::FieldStageFragment);

    auto depthImage = vulkan->getImageFactory()->build(640, 480, VEngineImageFormat::Depth32f, VEngineImageUsage::Depth, VEngineImageAspect::DepthAspect, VEngineImageLayout::Preinitialized);

    auto stageTeapot = vulkan->getRenderStageFactory()->build(640, 480, { vertTeapot, fragTeapot }, { layoutTeapot },
    {
        colorImage->getAttachment(VEngineAttachmentBlending::None, true, { { 0.1f, 0.2f, 0.3f, 1.0f } }),
        depthImage->getAttachment(VEngineAttachmentBlending::None, true)
    });

    auto setTeapot = layoutTeapot->generateDescriptorSet();
    setTeapot->bindBuffer(0, buffer);
    setTeapot->bindImageViewSampler(1, texture);

    stageTeapot->setSets({ setTeapot });

    return stageTeapot;
}

std::tuple<ComputeStageInterface*, DescriptorSetInterface*> createPostprocessStage(VulkanToolkit* vulkan, GenericBufferInterface* buffer, ImageInterface* colorImage) {
    auto compPostprocess = vulkan->getShaderFactory()->build(VEngineShaderModuleType::Compute, "example-advancedteapot-postprocess.comp.spv");

    auto layoutPostprocess = vulkan->getDescriptorSetLayoutFactory()->build();
    layoutPostprocess->addField(VEngineDescriptorSetFieldType::FieldTypeUniformBuffer, VEngineDescriptorSetFieldStage::FieldStageAll);
    layoutPostprocess->addField(VEngineDescriptorSetFieldType::FieldTypeStorageImage, VEngineDescriptorSetFieldStage::FieldStageCompute);

    auto stagePostprocess = vulkan->getComputeStageFactory()->build({ compPostprocess }, { layoutPostprocess });

    auto setPostprocess = layoutPostprocess->generateDescriptorSet();
    setPostprocess->bindBuffer(0, buffer);
    setPostprocess->bindImageStorage(1, colorImage);

    return { stagePostprocess, setPostprocess };
}

RenderStageInterface* createOutputStage(VulkanToolkit* vulkan, GenericBufferInterface* buffer, ImageInterface* colorImage) {
    auto vertOutput = vulkan->getShaderFactory()->build(VEngineShaderModuleType::Vertex, "example-advancedteapot-output.vert.spv");
    auto fragOutput = vulkan->getShaderFactory()->build(VEngineShaderModuleType::Fragment, "example-advancedteapot-output.frag.spv");

    auto layoutOutput = vulkan->getDescriptorSetLayoutFactory()->build();
    layoutOutput->addField(VEngineDescriptorSetFieldType::FieldTypeUniformBuffer, VEngineDescriptorSetFieldStage::FieldStageAll);
    layoutOutput->addField(VEngineDescriptorSetFieldType::FieldTypeSampler, VEngineDescriptorSetFieldStage::FieldStageFragment);

    auto stageOutput = vulkan->getRenderStageFactory()->build(640, 480, { vertOutput, fragOutput }, { layoutOutput });

    auto setOutput = layoutOutput->generateDescriptorSet();
    setOutput->bindBuffer(0, buffer);
    setOutput->bindImageViewSampler(1, colorImage);

    stageOutput->setSets({ setOutput });

    return stageOutput;
}

int main()
{
    auto vulkan = new VulkanToolkit(640, 480, true, "Full screen pass example");
    vulkan->getMedia()->scanDirectory("../../media");
    vulkan->getMedia()->scanDirectory("../../shaders");

    auto texture = vulkan->getImageFactory()->build("blaze.png");

    auto buffer = vulkan->getBufferFactory()->build(VEngineBufferType::BufferTypeUniform, sizeof(float) * 20);

    auto colorImage = vulkan->getImageFactory()->buildMipmapped(640, 480, VEngineImageFormat::RGBA8unorm, VEngineImageUsage::ColorAttachment | VEngineImageUsage::Sampled | VEngineImageUsage::Storage);
    
    auto stageTeapot = createTeapotStage(vulkan, buffer, texture, colorImage);

    auto stagePostprocessCreationResult = createPostprocessStage(vulkan, buffer, colorImage);
    auto stagePostprocess = std::get<0>(stagePostprocessCreationResult);
    auto setPostprocess = std::get<1>(stagePostprocessCreationResult);

    auto stageOutput = createOutputStage(vulkan, buffer, colorImage);

    auto output = vulkan->getSwapChainOutputFactory()->build(stageOutput);

    auto teapot = vulkan->getObject3dInfoFactory()->build("teapot.raw");
    auto fullScreenQuad = vulkan->getObject3dInfoFactory()->getFullScreenQuad();

    while (!vulkan->shouldCloseWindow()) {

        void* ptr;
        buffer->map(0, sizeof(float) * 1, &ptr);
        ((float*)ptr)[0] = (float)vulkan->getExecutionTime();
        buffer->unmap();

        stageTeapot->beginDrawing();
        stageTeapot->drawMesh(teapot, 1);
        stageTeapot->endDrawing();
        stageTeapot->submit(output->getSignalSemaphores());
        
        stagePostprocess->beginRecording();
        stagePostprocess->dispatch({ setPostprocess }, 10, 480, 1);
        stagePostprocess->endRecording();
        stagePostprocess->submit({ stageTeapot->getSignalSemaphore() });
        
        colorImage->regenerateMipmaps();

        output->beginDrawing();
        output->drawMesh(fullScreenQuad, 1);
        output->endDrawing();
        output->submit({ stagePostprocess->getSignalSemaphore() });

        vulkan->poolEvents();
    }

    return 0;
}


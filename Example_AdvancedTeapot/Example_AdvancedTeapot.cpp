#include "stdafx.h"

using namespace VEngine::Renderer;

VulkanRenderStage* createTeapotStage(VulkanToolkit* vulkan, VulkanGenericBuffer* buffer, VulkanImage* texture, VulkanImage* colorImage) {
    auto vertTeapot = vulkan->getVulkanShaderFactory()->build(VulkanShaderModuleType::Vertex, "example-advancedteapot-teapot.vert.spv");
    auto fragTeapot = vulkan->getVulkanShaderFactory()->build(VulkanShaderModuleType::Fragment, "example-advancedteapot-teapot.frag.spv");

    auto layoutTeapot = vulkan->getVulkanDescriptorSetLayoutFactory()->build();
    layoutTeapot->addField(VulkanDescriptorSetFieldType::FieldTypeUniformBuffer, VulkanDescriptorSetFieldStage::FieldStageAllGraphics);
    layoutTeapot->addField(VulkanDescriptorSetFieldType::FieldTypeSampler, VulkanDescriptorSetFieldStage::FieldStageFragment);

    auto depthImage = vulkan->getVulkanImageFactory()->build(640, 480, VulkanImageFormat::Depth32f, VulkanImageUsage::Depth, VulkanImageAspect::DepthAspect, VulkanImageLayout::Preinitialized);

    auto stageTeapot = vulkan->getVulkanRenderStageFactory()->build(640, 480, { vertTeapot, fragTeapot }, { layoutTeapot },
    {
        colorImage->getAttachment(VulkanAttachmentBlending::None, true, { { 0.1f, 0.2f, 0.3f, 1.0f } }),
        depthImage->getAttachment(VulkanAttachmentBlending::None, true)
    });

    auto setTeapot = layoutTeapot->generateDescriptorSet();
    setTeapot->bindBuffer(0, buffer);
    setTeapot->bindImageViewSampler(1, texture);

    stageTeapot->setSets({ setTeapot });

    return stageTeapot;
}

std::tuple<VulkanComputeStage*, VulkanDescriptorSet*> createPostprocessStage(VulkanToolkit* vulkan, VulkanGenericBuffer* buffer, VulkanImage* colorImage) {
    auto compPostprocess = vulkan->getVulkanShaderFactory()->build(VulkanShaderModuleType::Compute, "example-advancedteapot-postprocess.comp.spv");

    auto layoutPostprocess = vulkan->getVulkanDescriptorSetLayoutFactory()->build();
    layoutPostprocess->addField(VulkanDescriptorSetFieldType::FieldTypeUniformBuffer, VulkanDescriptorSetFieldStage::FieldStageAll);
    layoutPostprocess->addField(VulkanDescriptorSetFieldType::FieldTypeStorageImage, VulkanDescriptorSetFieldStage::FieldStageCompute);

    auto stagePostprocess = vulkan->getVulkanComputeStageFactory()->build({ compPostprocess }, { layoutPostprocess });

    auto setPostprocess = layoutPostprocess->generateDescriptorSet();
    setPostprocess->bindBuffer(0, buffer);
    setPostprocess->bindImageStorage(1, colorImage);

    return { stagePostprocess, setPostprocess };
}

VulkanRenderStage* createOutputStage(VulkanToolkit* vulkan, VulkanGenericBuffer* buffer, VulkanImage* colorImage) {
    auto vertOutput = vulkan->getVulkanShaderFactory()->build(VulkanShaderModuleType::Vertex, "example-advancedteapot-output.vert.spv");
    auto fragOutput = vulkan->getVulkanShaderFactory()->build(VulkanShaderModuleType::Fragment, "example-advancedteapot-output.frag.spv");

    auto layoutOutput = vulkan->getVulkanDescriptorSetLayoutFactory()->build();
    layoutOutput->addField(VulkanDescriptorSetFieldType::FieldTypeUniformBuffer, VulkanDescriptorSetFieldStage::FieldStageAll);
    layoutOutput->addField(VulkanDescriptorSetFieldType::FieldTypeSampler, VulkanDescriptorSetFieldStage::FieldStageFragment);

    auto stageOutput = vulkan->getVulkanRenderStageFactory()->build(640, 480, { vertOutput, fragOutput }, { layoutOutput });

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

    auto texture = vulkan->getVulkanImageFactory()->build("blaze.png");

    auto buffer = vulkan->getVulkanBufferFactory()->build(VulkanBufferType::BufferTypeUniform, sizeof(float) * 20);

    auto colorImage = vulkan->getVulkanImageFactory()->build(640, 480, VulkanImageFormat::RGBA8unorm, VulkanImageUsage::ColorAttachment | VulkanImageUsage::Sampled | VulkanImageUsage::Storage);
    
    auto stageTeapot = createTeapotStage(vulkan, buffer, texture, colorImage);

    auto stagePostprocessCreationResult = createPostprocessStage(vulkan, buffer, colorImage);
    auto stagePostprocess = std::get<0>(stagePostprocessCreationResult);
    auto setPostprocess = std::get<1>(stagePostprocessCreationResult);

    auto stageOutput = createOutputStage(vulkan, buffer, colorImage);

    auto output = vulkan->getVulkanSwapChainOutputFactory()->build(stageOutput);

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
        
        output->beginDrawing();
        output->drawMesh(fullScreenQuad, 1);
        output->endDrawing();
        output->submit({ stagePostprocess->getSignalSemaphore() });

        vulkan->poolEvents();
    }

    return 0;
}


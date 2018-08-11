#include "stdafx.h"

using namespace VEngine::FileSystem;
using namespace VEngine::Renderer;

VulkanRenderStage* createTeapotStage(VulkanToolkit* toolkit, VulkanGenericBuffer* buffer, VulkanImage* texture, VulkanImage* colorImage) {
    auto vertTeapot = toolkit->getVulkanShaderFactory()->build(VulkanShaderModuleType::Vertex, "example-advancedteapot-teapot.vert.spv");
    auto fragTeapot = toolkit->getVulkanShaderFactory()->build(VulkanShaderModuleType::Fragment, "example-advancedteapot-teapot.frag.spv");

    auto layoutTeapot = toolkit->getVulkanDescriptorSetLayoutFactory()->build();
    layoutTeapot->addField(VulkanDescriptorSetFieldType::FieldTypeUniformBuffer, VulkanDescriptorSetFieldStage::FieldStageAllGraphics);
    layoutTeapot->addField(VulkanDescriptorSetFieldType::FieldTypeSampler, VulkanDescriptorSetFieldStage::FieldStageFragment);

    auto depthImage = toolkit->getVulkanImageFactory()->build(640, 480, VulkanImageFormat::Depth32f, VulkanImageUsage::Depth, VulkanImageAspect::DepthAspect, VulkanImageLayout::Preinitialized);

    auto stageTeapot = toolkit->getVulkanRenderStageFactory()->build(640, 480, { vertTeapot, fragTeapot }, { layoutTeapot },
        {
            colorImage->getAttachment(VulkanAttachmentBlending::None, true, { { 0.1f, 0.2f, 0.3f, 1.0f } }),
            depthImage->getAttachment(VulkanAttachmentBlending::None)
        });

    auto setTeapot = layoutTeapot->generateDescriptorSet();
    setTeapot->bindBuffer(0, buffer);
    setTeapot->bindImageViewSampler(1, texture);

    stageTeapot->setSets({ setTeapot });

    return stageTeapot;
}

std::tuple<VulkanComputeStage*, VulkanDescriptorSet*> createPostprocessStage(VulkanToolkit* toolkit, VulkanGenericBuffer* buffer, VulkanImage* colorImage) {
    auto compPostprocess = toolkit->getVulkanShaderFactory()->build(VulkanShaderModuleType::Compute, "example-advancedteapot-postprocess.comp.spv");

    auto layoutPostprocess = toolkit->getVulkanDescriptorSetLayoutFactory()->build();
    layoutPostprocess->addField(VulkanDescriptorSetFieldType::FieldTypeUniformBuffer, VulkanDescriptorSetFieldStage::FieldStageAll);
    layoutPostprocess->addField(VulkanDescriptorSetFieldType::FieldTypeStorageImage, VulkanDescriptorSetFieldStage::FieldStageCompute);

    auto stagePostprocess = toolkit->getVulkanComputeStageFactory()->build({ compPostprocess }, { layoutPostprocess });

    auto setPostprocess = layoutPostprocess->generateDescriptorSet();
    setPostprocess->bindBuffer(0, buffer);
    setPostprocess->bindImageStorage(1, colorImage);

    return { stagePostprocess, setPostprocess };
}

VulkanRenderStage* createOutputStage(VulkanToolkit* toolkit, VulkanGenericBuffer* buffer, VulkanImage* colorImage) {
    auto vertOutput = toolkit->getVulkanShaderFactory()->build(VulkanShaderModuleType::Vertex, "example-advancedteapot-output.vert.spv");
    auto fragOutput = toolkit->getVulkanShaderFactory()->build(VulkanShaderModuleType::Fragment, "example-advancedteapot-output.frag.spv");

    auto layoutOutput = toolkit->getVulkanDescriptorSetLayoutFactory()->build();
    layoutOutput->addField(VulkanDescriptorSetFieldType::FieldTypeUniformBuffer, VulkanDescriptorSetFieldStage::FieldStageAll);
    layoutOutput->addField(VulkanDescriptorSetFieldType::FieldTypeSampler, VulkanDescriptorSetFieldStage::FieldStageFragment);

    auto stageOutput = toolkit->getVulkanRenderStageFactory()->build(640, 480, { vertOutput, fragOutput }, { layoutOutput }, {});

    auto setOutput = layoutOutput->generateDescriptorSet();
    setOutput->bindBuffer(0, buffer);
    setOutput->bindImageViewSampler(1, colorImage);

    stageOutput->setSets({ setOutput });

    return stageOutput;
}

int main()
{
    Media::loadFileMap("../../media");
    Media::loadFileMap("../../shaders");
    auto toolkit = new VulkanToolkit(640, 480, true, "Full screen pass example");

    auto texture = toolkit->getVulkanImageFactory()->build("blaze.png");

    auto buffer = toolkit->getVulkanBufferFactory()->build(VulkanBufferType::BufferTypeUniform, sizeof(float) * 20);

    auto colorImage = toolkit->getVulkanImageFactory()->build(640, 480, VulkanImageFormat::RGBA16f, VulkanImageUsage::ColorAttachment | VulkanImageUsage::Storage | VulkanImageUsage::Sampled);
    
    auto stageTeapot = createTeapotStage(toolkit, buffer, texture, colorImage);

    auto stagePostprocessCreationResult = createPostprocessStage(toolkit, buffer, colorImage);
    auto stagePostprocess = std::get<0>(stagePostprocessCreationResult);
    auto setPostprocess = std::get<1>(stagePostprocessCreationResult);

    auto stageOutput = createOutputStage(toolkit, buffer, colorImage);

    auto output = toolkit->getVulkanSwapChainOutputFactory()->build(stageOutput);

    auto teapot = toolkit->getObject3dInfoFactory()->build("teapot.raw");
    auto fullScreenQuad = toolkit->getObject3dInfoFactory()->getFullScreenQuad();

    while (!toolkit->shouldCloseWindow()) {

        void* ptr;
        buffer->map(0, sizeof(float) * 1, &ptr);
        ((float*)ptr)[0] = (float)toolkit->getExecutionTime();
        buffer->unmap();

        stageTeapot->beginDrawing();
        stageTeapot->drawMesh(teapot, 1);
        stageTeapot->endDrawing();
        stageTeapot->submit(output->getSignalSemaphores());

        stagePostprocess->beginRecording();
        stagePostprocess->dispatch({ setPostprocess }, 10, 48, 1);
        stagePostprocess->endRecording();
        stagePostprocess->submit({ stageTeapot->getSignalSemaphore() });

        output->beginDrawing();
        // no meshes drawn, only post process which is handled automatically
        output->drawMesh(fullScreenQuad, 1);
        output->endDrawing();
        output->submit({ stagePostprocess->getSignalSemaphore() });

        toolkit->poolEvents();
    }

    return 0;
}


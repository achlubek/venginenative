// Example_Compute.cpp: Okreœla punkt wejœcia dla aplikacji konsoli.
//

#include "stdafx.h"

#include "Media.h"
#include "Internal/VulkanDevice.h"
#include "VulkanComputeStage.h"

int main()
{
    Media::loadFileMap("../../media");
    Media::loadFileMap("../../shaders");
    auto toolkit = new VulkanToolkit(0, 0, true, "No window mode");
    
    auto shader = toolkit->getVulkanShaderFactory()->build(VulkanShaderModuleType::Compute, "example-compute.comp.spv");

    auto layout = toolkit->getVulkanDescriptorSetLayoutFactory()->build();
    layout->addField(VulkanDescriptorSetFieldType::FieldTypeStorageBuffer, VulkanDescriptorSetFieldStage::FieldStageCompute);
    layout->compile();

    auto stage = toolkit->getVulkanComputeStageFactory()->build(shader, { layout });
    stage->compile();
    
    auto buffer = toolkit->getVulkanBufferFactory()->build(VulkanBufferType::BufferTypeStorage, sizeof(float) * 20);
    
    auto set = layout->generateDescriptorSet();
    set->bindBuffer(0, buffer);
    set->update();

    void* ptr;
    buffer->map(0, sizeof(float) * 4, &ptr);
    ((float*)ptr)[0] = (float)1.0f;
    ((float*)ptr)[1] = (float)2.0f;
    ((float*)ptr)[2] = (float)3.0f;
    ((float*)ptr)[3] = (float)4.0f;
    buffer->unmap();

    stage->beginRecording();
    stage->dispatch({ set }, 1, 1, 1);
    stage->endRecording();
    stage->submitNoSemaphores({});

    vkQueueWaitIdle(toolkit->getDevice()->getMainQueue());
     
    buffer->map(0, sizeof(float) * 4, &ptr);
    printf("%f", ((float*)ptr)[0]);
    printf("%f", ((float*)ptr)[1]);
    printf("%f", ((float*)ptr)[2]);
    printf("%f", ((float*)ptr)[3]);
    buffer->unmap();


    return 0;
}


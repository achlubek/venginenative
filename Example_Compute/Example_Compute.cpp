// Example_Compute.cpp: Okreœla punkt wejœcia dla aplikacji konsoli.
//

#include "stdafx.h"


int main()
{
    auto toolkit = new VulkanToolkit();
    toolkit->initialize(0, 0); // no window
    
    auto shader = new VulkanShaderModule(toolkit, "../../shaders/compiled/example-compute.comp.spv");

    auto layout = new VulkanDescriptorSetLayout(toolkit);
    layout->addField(0, VK_DESCRIPTOR_TYPE_STORAGE_BUFFER, VK_SHADER_STAGE_COMPUTE_BIT);
    layout->compile();

    auto stage = new VulkanComputeStage(toolkit);
    stage->setShaderStage(shader->createShaderStage(VK_SHADER_STAGE_COMPUTE_BIT, "main"));
    stage->addDescriptorSetLayout(layout->layout);
    stage->compile();

    auto buffer = new VulkanGenericBuffer(toolkit, VK_BUFFER_USAGE_STORAGE_BUFFER_BIT, sizeof(float) * 20);

    auto set = layout->generateDescriptorSet();
    set->bindStorageBuffer(0, buffer);
    set->update();

    void* ptr;
    buffer->map(0, sizeof(float) * 4, &ptr);
    ((float*)ptr)[0] = (float)1.0f;
    ((float*)ptr)[1] = (float)2.0f;
    ((float*)ptr)[2] = (float)3.0f;
    ((float*)ptr)[3] = (float)4.0f;
    buffer->unmap();

    stage->beginRecording();
    stage->dispatch({ set }, 4, 1, 1);
    stage->endRecording();
    stage->submitNoSemaphores({});

    vkQueueWaitIdle(toolkit->mainQueue);
     
    buffer->map(0, sizeof(float) * 4, &ptr);
    printf("%f", ((float*)ptr)[0]);
    printf("%f", ((float*)ptr)[1]);
    printf("%f", ((float*)ptr)[2]);
    printf("%f", ((float*)ptr)[3]);
    buffer->unmap();


    return 0;
}


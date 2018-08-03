#include "stdafx.h"

using namespace VEngine::FileSystem;
using namespace VEngine::Renderer;

/*
This example shows how to do simple computations on GPU using compute shader
*/

int main()
{
    // Initialize media so the system can find needed files

    Media::loadFileMap("../../media");
    Media::loadFileMap("../../shaders");

    // Initialize the system, in headless mode (no window)
    // Setting true to enable validation layers
    auto toolkit = new VulkanToolkit(true);
    
    // Load the compute shader that will do the calculation
    // This expects Spir-V spv file - compiled shader
    auto shader = toolkit->getVulkanShaderFactory()->build(VulkanShaderModuleType::Compute, "example-compute.comp.spv");

    // Build the layout of data that will be available in the shader runtime
    auto layout = toolkit->getVulkanDescriptorSetLayoutFactory()->build();
    // Add a storage buffer to the layout - it will be at binding 0
    layout->addField(VulkanDescriptorSetFieldType::FieldTypeStorageBuffer, VulkanDescriptorSetFieldStage::FieldStageCompute);

    // Create the compute stage that will execute the computation, using given shader and given layout
    auto stage = toolkit->getVulkanComputeStageFactory()->build(shader, { layout });
    
    // Create the storage buffer that will hold input data and where output will be saved
    auto buffer = toolkit->getVulkanBufferFactory()->build(VulkanBufferType::BufferTypeStorage, sizeof(float) * 20);
    
    // Create a descriptor set matching previously created descriptor set layout
    auto set = layout->generateDescriptorSet();
    // Bind newly created buffer at binding 0
    set->bindBuffer(0, buffer);

    // Map the buffer and put some values in it
    void* ptr;
    buffer->map(0, sizeof(float) * 4, &ptr);
    ((float*)ptr)[0] = (float)1.0f;
    ((float*)ptr)[1] = (float)2.0f;
    ((float*)ptr)[2] = (float)3.0f;
    ((float*)ptr)[3] = (float)4.0f;
    // Unmap once finished
    buffer->unmap();

    // Begin recording a command buffer
    stage->beginRecording();
    // Dispatch computation task, using only 1 instance on gpu (as this is just an example)
    // The shader has declared to run in 4x1x1 work group, so 4 units will run, see compute shader source to see how it works
    stage->dispatch({ set }, 1, 1, 1);
    // End recording
    stage->endRecording();
    // Submit the command buffer, this time without synchronization (as this is just an example)
    stage->submitNoSemaphores({});

    // Wait until task is executed and results are available, this is only needed to read the data back to CPU
    toolkit->waitQueueIdle();
     
    // Read the data by mapping the buffer again and print it
    buffer->map(0, sizeof(float) * 4, &ptr);
    printf("%f ", ((float*)ptr)[0]);
    printf("%f ", ((float*)ptr)[1]);
    printf("%f ", ((float*)ptr)[2]);
    printf("%f ", ((float*)ptr)[3]);
    // And finally unmap
    buffer->unmap();

    return 0;
}


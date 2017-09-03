#include "stdafx.h"
#include "Renderer.h"
#include "Application.h"
#include "Camera.h"
#include "FrustumCone.h"
#include "Media.h"

Renderer::Renderer(int iwidth, int iheight)
{
    width = iwidth;
    height = iheight; 

    diffuseImage = VulkanImage(width, height, VK_FORMAT_R8G8B8A8_UNORM, VK_IMAGE_TILING_OPTIMAL,
        VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT | VK_IMAGE_USAGE_SAMPLED_BIT, VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT, VK_IMAGE_ASPECT_COLOR_BIT, VK_IMAGE_LAYOUT_PREINITIALIZED, false);


    normalImage = VulkanImage(width, height, VK_FORMAT_R16G16B16A16_SFLOAT, VK_IMAGE_TILING_OPTIMAL,
        VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT | VK_IMAGE_USAGE_SAMPLED_BIT, VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT, VK_IMAGE_ASPECT_COLOR_BIT, VK_IMAGE_LAYOUT_PREINITIALIZED, false);

    distanceImage = VulkanImage(width, height, VK_FORMAT_R32_SFLOAT, VK_IMAGE_TILING_OPTIMAL,
        VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT | VK_IMAGE_USAGE_SAMPLED_BIT, VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT, VK_IMAGE_ASPECT_COLOR_BIT, VK_IMAGE_LAYOUT_PREINITIALIZED, false);

    ambientImage = VulkanImage(width, height, VK_FORMAT_R8G8B8A8_UNORM, VK_IMAGE_TILING_OPTIMAL,
        VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT | VK_IMAGE_USAGE_SAMPLED_BIT, VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT, VK_IMAGE_ASPECT_COLOR_BIT, VK_IMAGE_LAYOUT_PREINITIALIZED, false);

    depthImage = VulkanImage(width, height, VK_FORMAT_D32_SFLOAT, VK_IMAGE_TILING_OPTIMAL,
        VK_IMAGE_USAGE_DEPTH_STENCIL_ATTACHMENT_BIT, VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT, VK_IMAGE_ASPECT_DEPTH_BIT, VK_IMAGE_LAYOUT_PREINITIALIZED, true);

    uboHighFrequencyBuffer = VulkanGenericBuffer(VK_BUFFER_USAGE_UNIFORM_BUFFER_BIT, sizeof(float) * 20);
    uboLowFrequencyBuffer = VulkanGenericBuffer(VK_BUFFER_USAGE_UNIFORM_BUFFER_BIT, sizeof(float) * 20);

    //setManager = VulkanDescriptorSetsManager();

    //##########################//

    auto vertShaderModule = VulkanShaderModule("../../shaders/compiled/triangle.vert.spv");
    auto fragShaderModule = VulkanShaderModule("../../shaders/compiled/triangle.frag.spv");

    meshSetLayout = new VulkanDescriptorSetLayout();
    meshSetLayout->addField(0, VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER, VK_SHADER_STAGE_ALL_GRAPHICS);
    meshSetLayout->addField(1, VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER, VK_SHADER_STAGE_ALL_GRAPHICS);
    meshSetLayout->compile();

    sharedSet = meshSetLayout->generateDescriptorSet();
    sharedSet.bindUniformBuffer(0, uboHighFrequencyBuffer);
    sharedSet.bindUniformBuffer(1, uboLowFrequencyBuffer);
    sharedSet.update();

    auto meshRenderStage = new VulkanRenderStage();
    VkExtent2D ext = VkExtent2D();
    ext.width = width;
    ext.height = height;
    meshRenderStage->setViewport(ext);
    meshRenderStage->addShaderStage(vertShaderModule.createShaderStage(VK_SHADER_STAGE_VERTEX_BIT, "main"));
    meshRenderStage->addShaderStage(fragShaderModule.createShaderStage(VK_SHADER_STAGE_FRAGMENT_BIT, "main"));
    meshRenderStage->addDescriptorSetLayout(meshSetLayout->layout);
    meshRenderStage->addDescriptorSetLayout(Application::instance->meshModelsDataLayout->layout);
    meshRenderStage->addDescriptorSetLayout(Application::instance->materialLayout->layout);
    meshRenderStage->addOutputImage(diffuseImage);
    meshRenderStage->addOutputImage(normalImage);
    meshRenderStage->addOutputImage(distanceImage);
    meshRenderStage->addOutputImage(depthImage);
    meshRenderStage->meshSharedSet = &sharedSet;

    //####//

    auto ppvertShaderModule = VulkanShaderModule("../../shaders/compiled/pp.vert.spv");

    auto ppSetLayout = new VulkanDescriptorSetLayout();
    ppSetLayout->addField(0, VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER, VK_SHADER_STAGE_ALL_GRAPHICS);
    ppSetLayout->addField(1, VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER, VK_SHADER_STAGE_ALL_GRAPHICS);

    ppSetLayout->addField(2, VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER, VK_SHADER_STAGE_FRAGMENT_BIT);
    ppSetLayout->addField(3, VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER, VK_SHADER_STAGE_FRAGMENT_BIT);
    ppSetLayout->addField(4, VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER, VK_SHADER_STAGE_FRAGMENT_BIT);
    ppSetLayout->addField(5, VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER, VK_SHADER_STAGE_FRAGMENT_BIT);
    ppSetLayout->compile();

    //##########################//

    auto ppshadefragShaderModule = VulkanShaderModule("../../shaders/compiled/pp-shade-ambient.frag.spv");

    auto ppShadeAmbientStage = new VulkanRenderStage();
     
    ppShadeAmbientStage->setViewport(ext);
    ppShadeAmbientStage->addShaderStage(ppvertShaderModule.createShaderStage(VK_SHADER_STAGE_VERTEX_BIT, "main"));
    ppShadeAmbientStage->addShaderStage(ppshadefragShaderModule.createShaderStage(VK_SHADER_STAGE_FRAGMENT_BIT, "main"));
    ppShadeAmbientStage->addDescriptorSetLayout(ppSetLayout->layout);
    ppShadeAmbientStage->addOutputImage(ambientImage); 

    //##########################//

    auto ppoutputfragShaderModule = VulkanShaderModule("../../shaders/compiled/pp-output.frag.spv");

    auto post_process_zygote = new VulkanRenderStage();

    post_process_zygote->setViewport(ext);
    post_process_zygote->addShaderStage(ppvertShaderModule.createShaderStage(VK_SHADER_STAGE_VERTEX_BIT, "main"));
    post_process_zygote->addShaderStage(ppoutputfragShaderModule.createShaderStage(VK_SHADER_STAGE_FRAGMENT_BIT, "main"));
    post_process_zygote->addDescriptorSetLayout(ppSetLayout->layout);

    //##########################//

    postProcessSet = ppSetLayout->generateDescriptorSet();
    postProcessSet.bindUniformBuffer(0, uboHighFrequencyBuffer);
    postProcessSet.bindUniformBuffer(1, uboLowFrequencyBuffer);
    postProcessSet.bindImageViewSampler(2, diffuseImage);
    postProcessSet.bindImageViewSampler(3, normalImage);
    postProcessSet.bindImageViewSampler(4, distanceImage);
    postProcessSet.bindImageViewSampler(5, ambientImage);
    postProcessSet.update();

    renderer = new VulkanRenderer();
    renderer->setMeshStage(meshRenderStage);
    renderer->addPostProcessingStage(ppShadeAmbientStage);
    renderer->setOutputStage(post_process_zygote);
    renderer->setPostProcessingDescriptorSet(&postProcessSet);
    renderer->compile();

}

Renderer::~Renderer()
{ 
}

void Renderer::renderToSwapChain(Camera *camera)
{

    //if (Game::instance->world->scene->getMesh3ds().size() == 0) return;
    VulkanBinaryBufferBuilder bb = VulkanBinaryBufferBuilder();
    double xpos, ypos;
    glfwGetCursorPos(VulkanToolkit::singleton->window, &xpos, &ypos);
     
    glm::mat4 clip(1.0f, 0.0f, 0.0f, 0.0f,
        0.0f, -1.0f, 0.0f, 0.0f,
        0.0f, 0.0f, 0.5f, 0.0f,
        0.0f, 0.0f, 0.5f, 1.0f);
    glm::mat4 vpmatrix = clip * camera->projectionMatrix * camera->transformation->getInverseWorldTransform();

    bb.emplaceFloat32((float)glfwGetTime());
    bb.emplaceFloat32(0.0f);
    bb.emplaceFloat32((float)xpos / (float)width);
    bb.emplaceFloat32((float)ypos / (float)height);
    bb.emplaceGeneric((unsigned char*)&vpmatrix, sizeof(vpmatrix));

    bb.emplaceGeneric((unsigned char*)&camera->transformation->getPosition(), sizeof(camera->cone->leftBottom));
    bb.emplaceFloat32(0.0f);

    bb.emplaceGeneric((unsigned char*)&(camera->cone->leftBottom), sizeof(camera->cone->leftBottom));
    bb.emplaceFloat32(0.0f);
    bb.emplaceGeneric((unsigned char*)&(camera->cone->rightBottom - camera->cone->leftBottom), sizeof(camera->cone->leftBottom));
    bb.emplaceFloat32(0.0f);
    bb.emplaceGeneric((unsigned char*)&(camera->cone->leftTop - camera->cone->leftBottom), sizeof(camera->cone->leftBottom));
    bb.emplaceFloat32(0.0f);

    void* data;
    uboHighFrequencyBuffer.map(0, bb.buffer.size(), &data);
    memcpy(data, bb.getPointer(), bb.buffer.size());
    uboHighFrequencyBuffer.unmap();

    uboLowFrequencyBuffer.map(0, bb.buffer.size(), &data);
    memcpy(data, bb.getPointer(), bb.buffer.size());
    uboLowFrequencyBuffer.unmap();

    uint32_t imageIndex;

    glm::mat4 cameraViewMatrix = camera->transformation->getInverseWorldTransform(); 
    glm::mat4 cameraRotMatrix = camera->transformation->getRotationMatrix();
    glm::mat4 rpmatrix = camera->projectionMatrix * inverse(cameraRotMatrix);
    camera->cone->update(inverse(rpmatrix));

    Application::instance->scene->prepareFrame();
    renderer->beginDrawing();
    Application::instance->scene->draw(renderer->getMesh3dStage());
    renderer->endDrawing();
}

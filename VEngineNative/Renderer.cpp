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


    depthImage = VulkanImage(width, height, VK_FORMAT_D32_SFLOAT, VK_IMAGE_TILING_OPTIMAL,
        VK_IMAGE_USAGE_DEPTH_STENCIL_ATTACHMENT_BIT, VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT, VK_IMAGE_ASPECT_DEPTH_BIT, VK_IMAGE_LAYOUT_PREINITIALIZED, true);

    uboHighFrequencyBuffer = VulkanGenericBuffer(VK_BUFFER_USAGE_UNIFORM_BUFFER_BIT, sizeof(float) * 20);
    uboLowFrequencyBuffer = VulkanGenericBuffer(VK_BUFFER_USAGE_UNIFORM_BUFFER_BIT, sizeof(float) * 20);

    //setManager = VulkanDescriptorSetsManager();

    auto vertShaderModule = VulkanShaderModule("../../shaders/compiled/triangle.vert.spv");
    auto fragShaderModule = VulkanShaderModule("../../shaders/compiled/triangle.frag.spv");

    auto meshRenderStage = new VulkanRenderStage();
    VkExtent2D ext = VkExtent2D();
    ext.width = width;
    ext.height = height;
    meshRenderStage->setViewport(ext);
    meshRenderStage->addShaderStage(vertShaderModule.createShaderStage(VK_SHADER_STAGE_VERTEX_BIT, "main"));
    meshRenderStage->addShaderStage(fragShaderModule.createShaderStage(VK_SHADER_STAGE_FRAGMENT_BIT, "main"));
    meshRenderStage->addDescriptorSetLayout(setManager.mesh3dLayout);
    meshRenderStage->addOutputImage(diffuseImage);
    meshRenderStage->addOutputImage(normalImage);
    meshRenderStage->addOutputImage(distanceImage);
    meshRenderStage->addOutputImage(depthImage);
    
    auto ppvertShaderModule = VulkanShaderModule("../../shaders/compiled/pp.vert.spv");
    auto ppfragShaderModule = VulkanShaderModule("../../shaders/compiled/pp.frag.spv");
     
    auto post_process_zygote = new VulkanRenderStage();

    post_process_zygote->setViewport(ext);
    post_process_zygote->addShaderStage(ppvertShaderModule.createShaderStage(VK_SHADER_STAGE_VERTEX_BIT, "main"));
    post_process_zygote->addShaderStage(ppfragShaderModule.createShaderStage(VK_SHADER_STAGE_FRAGMENT_BIT, "main"));
    post_process_zygote->addDescriptorSetLayout(setManager.ppLayout); 

    postProcessSet = setManager.generatePostProcessingDescriptorSet();
    postProcessSet.bindUniformBuffer(0, uboHighFrequencyBuffer);
    postProcessSet.bindUniformBuffer(1, uboLowFrequencyBuffer);
    postProcessSet.bindImageViewSampler(2, diffuseImage);
    postProcessSet.bindImageViewSampler(3, normalImage);
    postProcessSet.bindImageViewSampler(4, distanceImage);
    postProcessSet.update();

    renderer = new VulkanRenderer();
    renderer->setMeshStage(meshRenderStage);
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
    bb.emplaceMat4(vpmatrix);

    bb.emplaceVec3(camera->transformation->getPosition());
    bb.emplaceFloat32(0.0f);

    bb.emplaceVec3(camera->cone->leftBottom);
    bb.emplaceFloat32(0.0f);
    bb.emplaceVec3(camera->cone->rightBottom - camera->cone->leftBottom);
    bb.emplaceFloat32(0.0f);
    bb.emplaceVec3(camera->cone->leftTop - camera->cone->leftBottom);
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

    renderer->draw();
}

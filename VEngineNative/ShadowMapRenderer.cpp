#include "stdafx.h"
#include "ShadowMapRenderer.h"
#include "Application.h"
#include "Camera.h"
#include "FrustumCone.h"
#include "Media.h"

ShadowMapRenderer::ShadowMapRenderer(VulkanToolkit * ivulkan, int iwidth, int iheight)
    : vulkan(ivulkan)
{
    width = iwidth;
    height = iheight;

    depthImage = new VulkanImage(vulkan, width, height, VK_FORMAT_D16_UNORM, VK_IMAGE_TILING_OPTIMAL,
        VK_IMAGE_USAGE_DEPTH_STENCIL_ATTACHMENT_BIT, VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT, VK_IMAGE_ASPECT_DEPTH_BIT, VK_IMAGE_LAYOUT_PREINITIALIZED, true);

    distanceImage = new VulkanImage(vulkan, width, height, VK_FORMAT_R32_SFLOAT, VK_IMAGE_TILING_OPTIMAL,
        VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT | VK_IMAGE_USAGE_SAMPLED_BIT, VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT, VK_IMAGE_ASPECT_COLOR_BIT, VK_IMAGE_LAYOUT_PREINITIALIZED, false);

    uboHighFrequencyBuffer = new VulkanGenericBuffer(vulkan, VK_BUFFER_USAGE_UNIFORM_BUFFER_BIT, sizeof(float) * 20);
    uboLowFrequencyBuffer = new VulkanGenericBuffer(vulkan, VK_BUFFER_USAGE_UNIFORM_BUFFER_BIT, sizeof(float) * 20);

    //setManager = VulkanDescriptorSetsManager();

    //##########################//

    auto vertShaderModule = VulkanShaderModule(vulkan, "../../shaders/compiled/depthonly.vert.spv");
    auto fragShaderModule = VulkanShaderModule(vulkan, "../../shaders/compiled/depthonly.frag.spv");

    auto meshSetLayout = new VulkanDescriptorSetLayout(vulkan);
    meshSetLayout->addField(0, VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER, VK_SHADER_STAGE_ALL_GRAPHICS);
    meshSetLayout->addField(1, VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER, VK_SHADER_STAGE_ALL_GRAPHICS);
    meshSetLayout->compile();

    sharedSet = meshSetLayout->generateDescriptorSet();
    sharedSet->bindUniformBuffer(0, uboHighFrequencyBuffer);
    sharedSet->bindUniformBuffer(1, uboLowFrequencyBuffer);
    sharedSet->update();


    auto meshRenderStage = new VulkanRenderStage(vulkan);
    VkExtent2D ext = VkExtent2D();
    ext.width = width;
    ext.height = height;
    meshRenderStage->setViewport(ext);
    meshRenderStage->addShaderStage(vertShaderModule.createShaderStage(VK_SHADER_STAGE_VERTEX_BIT, "main"));
    meshRenderStage->addShaderStage(fragShaderModule.createShaderStage(VK_SHADER_STAGE_FRAGMENT_BIT, "main"));
    meshRenderStage->addDescriptorSetLayout(meshSetLayout->layout);
    meshRenderStage->addDescriptorSetLayout(Application::instance->meshModelsDataLayout->layout);
    meshRenderStage->addDescriptorSetLayout(Application::instance->materialLayout->layout);
    meshRenderStage->addOutputImage(distanceImage);
    meshRenderStage->addOutputImage(depthImage);
    meshRenderStage->meshSharedSet = sharedSet;

    renderer = new VulkanRenderer(vulkan);
    renderer->setMeshStage(meshRenderStage);
    renderer->compile();
}

ShadowMapRenderer::~ShadowMapRenderer()
{
}

void ShadowMapRenderer::render(Camera *camera, glm::vec3 color)
{

    //if (Game::instance->world->scene->getMesh3ds().size() == 0) return;
    VulkanBinaryBufferBuilder bb = VulkanBinaryBufferBuilder();
    double xpos, ypos;
    glfwGetCursorPos(vulkan->window, &xpos, &ypos);

    glm::mat4 clip(1.0f, 0.0f, 0.0f, 0.0f,
        0.0f, -1.0f, 0.0f, 0.0f,
        0.0f, 0.0f, 0.5f, 0.0f,
        0.0f, 0.0f, 0.5f, 1.0f);
    glm::mat4 vpmatrix = clip * camera->projectionMatrix * camera->transformation->getInverseWorldTransform();

    glm::mat4 cameraViewMatrix = camera->transformation->getInverseWorldTransform();
    glm::mat4 cameraRotMatrix = camera->transformation->getRotationMatrix();
    glm::mat4 rpmatrix = camera->projectionMatrix * inverse(cameraRotMatrix);
    camera->cone->update(inverse(rpmatrix));

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
    uboHighFrequencyBuffer->map(0, bb.buffer.size(), &data);
    memcpy(data, bb.getPointer(), bb.buffer.size());
    uboHighFrequencyBuffer->unmap();

    uboLowFrequencyBuffer->map(0, bb.buffer.size(), &data);
    memcpy(data, bb.getPointer(), bb.buffer.size());
    uboLowFrequencyBuffer->unmap();

    renderer->beginDrawing();
    Application::instance->scene->draw(glm::mat4(1), renderer->getMesh3dStage());
    renderer->endDrawing();
}

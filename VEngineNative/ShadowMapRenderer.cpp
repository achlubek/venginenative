#include "stdafx.h"
#include "ShadowMapRenderer.h"
#include "Application.h"
#include "Camera.h"
#include "FrustumCone.h"
#include "Media.h"

ShadowMapRenderer::ShadowMapRenderer(int iwidth, int iheight)
{
    width = iwidth;
    height = iheight;

    depthImage = VulkanImage(width, height, VK_FORMAT_D32_SFLOAT, VK_IMAGE_TILING_OPTIMAL,
        VK_IMAGE_USAGE_DEPTH_STENCIL_ATTACHMENT_BIT, VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT, VK_IMAGE_ASPECT_DEPTH_BIT, VK_IMAGE_LAYOUT_PREINITIALIZED, true);

    uboHighFrequencyBuffer = VulkanGenericBuffer(VK_BUFFER_USAGE_UNIFORM_BUFFER_BIT, sizeof(float) * 20);
    uboLowFrequencyBuffer = VulkanGenericBuffer(VK_BUFFER_USAGE_UNIFORM_BUFFER_BIT, sizeof(float) * 20);

    //setManager = VulkanDescriptorSetsManager();

    //##########################//

    auto vertShaderModule = VulkanShaderModule("../../shaders/compiled/depthonly.vert.spv");
    auto fragShaderModule = VulkanShaderModule("../../shaders/compiled/depthonly.frag.spv");

    meshSetLayout = new VulkanDescriptorSetLayout();
    meshSetLayout->addField(0, VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER, VK_SHADER_STAGE_ALL_GRAPHICS);
    meshSetLayout->addField(1, VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER, VK_SHADER_STAGE_ALL_GRAPHICS);
    meshSetLayout->addField(2, VK_DESCRIPTOR_TYPE_STORAGE_BUFFER, VK_SHADER_STAGE_ALL_GRAPHICS);
    meshSetLayout->addField(3, VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER, VK_SHADER_STAGE_ALL_GRAPHICS);

    meshSetLayout->addField(4, VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER, VK_SHADER_STAGE_FRAGMENT_BIT);
    meshSetLayout->addField(5, VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER, VK_SHADER_STAGE_FRAGMENT_BIT);
    meshSetLayout->addField(6, VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER, VK_SHADER_STAGE_FRAGMENT_BIT);
    meshSetLayout->addField(7, VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER, VK_SHADER_STAGE_FRAGMENT_BIT);
    meshSetLayout->addField(8, VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER, VK_SHADER_STAGE_FRAGMENT_BIT);
    meshSetLayout->compile();

    auto meshRenderStage = new VulkanRenderStage();
    VkExtent2D ext = VkExtent2D();
    ext.width = width;
    ext.height = height;
    meshRenderStage->setViewport(ext);
    meshRenderStage->addShaderStage(vertShaderModule.createShaderStage(VK_SHADER_STAGE_VERTEX_BIT, "main"));
    meshRenderStage->addShaderStage(fragShaderModule.createShaderStage(VK_SHADER_STAGE_FRAGMENT_BIT, "main"));
    meshRenderStage->addDescriptorSetLayout(meshSetLayout->layout);
    meshRenderStage->addOutputImage(&depthImage);

    renderer = new VulkanRenderer();
    renderer->setMeshStage(meshRenderStage);
    renderer->compile();

}

ShadowMapRenderer::~ShadowMapRenderer()
{
}

void ShadowMapRenderer::renderToSwapChain(Camera *camera)
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

   // renderer->draw();
}

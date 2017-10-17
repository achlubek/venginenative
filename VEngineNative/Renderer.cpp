#include "stdafx.h"
#include "Renderer.h"
#include "Application.h"
#include "Camera.h"
#include "FrustumCone.h"
#include "Media.h"
#include "ShadowMapRenderer.h"
#include "SpotLight.h"

Renderer::Renderer(VulkanToolkit * ivulkan, int iwidth, int iheight)
    : vulkan(ivulkan)
{
    width = iwidth;
    height = iheight;

    uboHighFrequencyBuffer = new VulkanGenericBuffer(vulkan, VK_BUFFER_USAGE_UNIFORM_BUFFER_BIT, sizeof(float) * 20);
    uboLowFrequencyBuffer = new VulkanGenericBuffer(vulkan, VK_BUFFER_USAGE_UNIFORM_BUFFER_BIT, sizeof(float) * 20);

    meshSetLayout = new VulkanDescriptorSetLayout(vulkan);
    meshSetLayout->addField(0, VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER, VK_SHADER_STAGE_ALL_GRAPHICS);
    meshSetLayout->addField(1, VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER, VK_SHADER_STAGE_ALL_GRAPHICS);
    meshSetLayout->compile();

    sharedSet = meshSetLayout->generateDescriptorSet();
    sharedSet->bindUniformBuffer(0, uboHighFrequencyBuffer);
    sharedSet->bindUniformBuffer(1, uboLowFrequencyBuffer);
    sharedSet->update();

    lights = {};

    ppSetLayout = new VulkanDescriptorSetLayout(vulkan);
    ppSetLayout->addField(0, VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER, VK_SHADER_STAGE_ALL_GRAPHICS);
    ppSetLayout->addField(1, VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER, VK_SHADER_STAGE_ALL_GRAPHICS);

    ppSetLayout->addField(2, VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER, VK_SHADER_STAGE_FRAGMENT_BIT);
    ppSetLayout->addField(3, VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER, VK_SHADER_STAGE_FRAGMENT_BIT);
    ppSetLayout->addField(4, VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER, VK_SHADER_STAGE_FRAGMENT_BIT);
    ppSetLayout->addField(5, VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER, VK_SHADER_STAGE_FRAGMENT_BIT);
    ppSetLayout->addField(6, VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER, VK_SHADER_STAGE_FRAGMENT_BIT);
    ppSetLayout->addField(7, VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER, VK_SHADER_STAGE_FRAGMENT_BIT);
    ppSetLayout->addField(8, VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER, VK_SHADER_STAGE_FRAGMENT_BIT);
    ppSetLayout->compile();

    rebuild(false);
}

Renderer::~Renderer()
{
}
#define safedelete(a) if(a!=nullptr){delete a;a=nullptr;}
void Renderer::rebuild(bool destroy)
{
    vkDeviceWaitIdle(vulkan->device);
    if (destroy) {
        safedelete(shadowMapGenericStage);
        safedelete(ambientShadowRenderer);
        safedelete(meshRenderStage);
        safedelete(ppShadeAmbientStage);
        safedelete(postProcessZygoteStage);
        safedelete(renderer);
        safedelete(postProcessSet);
        vkDeviceWaitIdle(vulkan->device);
        safedelete(diffuseImage);
        safedelete(normalImage);
        safedelete(distanceImage);
        safedelete(ambientImage);
        safedelete(deferredResolvedImage);
        safedelete(depthImage);
    }
    vkDeviceWaitIdle(vulkan->device);
    diffuseImage = new VulkanImage(vulkan, width, height, VK_FORMAT_R8G8B8A8_UNORM, VK_IMAGE_TILING_OPTIMAL,
        VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT | VK_IMAGE_USAGE_SAMPLED_BIT, VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT, VK_IMAGE_ASPECT_COLOR_BIT, VK_IMAGE_LAYOUT_PREINITIALIZED, false);


    normalImage = new VulkanImage(vulkan, width, height, VK_FORMAT_R16G16B16A16_SFLOAT, VK_IMAGE_TILING_OPTIMAL,
        VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT | VK_IMAGE_USAGE_SAMPLED_BIT, VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT, VK_IMAGE_ASPECT_COLOR_BIT, VK_IMAGE_LAYOUT_PREINITIALIZED, false);

    distanceImage = new VulkanImage(vulkan, width, height, VK_FORMAT_R32_SFLOAT, VK_IMAGE_TILING_OPTIMAL,
        VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT | VK_IMAGE_USAGE_SAMPLED_BIT, VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT, VK_IMAGE_ASPECT_COLOR_BIT, VK_IMAGE_LAYOUT_PREINITIALIZED, false);

    ambientImage = new VulkanImage(vulkan, width, height, VK_FORMAT_R8G8B8A8_UNORM, VK_IMAGE_TILING_OPTIMAL,
        VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT | VK_IMAGE_USAGE_SAMPLED_BIT, VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT, VK_IMAGE_ASPECT_COLOR_BIT, VK_IMAGE_LAYOUT_PREINITIALIZED, false);

    deferredResolvedImage = new VulkanImage(vulkan, width, height, VK_FORMAT_R16G16B16A16_SFLOAT, VK_IMAGE_TILING_OPTIMAL,
        VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT | VK_IMAGE_USAGE_SAMPLED_BIT, VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT, VK_IMAGE_ASPECT_COLOR_BIT, VK_IMAGE_LAYOUT_PREINITIALIZED, false);

    depthImage = new VulkanImage(vulkan, width, height, VK_FORMAT_D32_SFLOAT, VK_IMAGE_TILING_OPTIMAL,
        VK_IMAGE_USAGE_DEPTH_STENCIL_ATTACHMENT_BIT, VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT, VK_IMAGE_ASPECT_DEPTH_BIT, VK_IMAGE_LAYOUT_PREINITIALIZED, true);


    //setManager = VulkanDescriptorSetsManager();

    //##########################//

    auto vertShaderModule = VulkanShaderModule(vulkan, "../../shaders/compiled/triangle.vert.spv");
    auto fragShaderModule = VulkanShaderModule(vulkan, "../../shaders/compiled/triangle.frag.spv");


    meshRenderStage = new VulkanRenderStage(vulkan);
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
    meshRenderStage->meshSharedSet = sharedSet;


    ambientShadowRenderer = new ShadowMapRenderer(vulkan, 1024, 1024, "worldposY");
    ambientShadowCamera = new Camera();
    ambientShadowCamera->createProjectionOrthogonal(-50.0, 50.0, -50.0, 50.0, -50.0, 50.0);
    ambientShadowCamera->transformation->setOrientation(glm::inverse(glm::lookAt(glm::vec3(0.0, 1.0, 0.0), glm::vec3(0.0, 0.0, 0.0), glm::vec3(0.0, 0.0, -1.0))));
    //####//

    auto ppvertShaderModule = new VulkanShaderModule(vulkan, "../../shaders/compiled/pp.vert.spv");


    //##########################//

    auto ppshadefragShaderModule = new VulkanShaderModule(vulkan, "../../shaders/compiled/pp-shade-ambient.frag.spv");

    ppShadeAmbientStage = new VulkanRenderStage(vulkan);

    ppShadeAmbientStage->setViewport(ext);
    ppShadeAmbientStage->addShaderStage(ppvertShaderModule->createShaderStage(VK_SHADER_STAGE_VERTEX_BIT, "main"));
    ppShadeAmbientStage->addShaderStage(ppshadefragShaderModule->createShaderStage(VK_SHADER_STAGE_FRAGMENT_BIT, "main"));
    ppShadeAmbientStage->addDescriptorSetLayout(ppSetLayout->layout);
    ppShadeAmbientStage->addOutputImage(ambientImage);

    //##########################//

    auto ppdeferredresolveShaderModule = new VulkanShaderModule(vulkan, "../../shaders/compiled/pp-deferred-resolve.frag.spv");

    shadowMapGenericStage = new VulkanRenderStage(vulkan);

    VkExtent2D shadowmapext = VkExtent2D();
    shadowmapext.width = 1024;
    shadowmapext.height = 1024;
    shadowMapGenericStage->setViewport(ext);
    shadowMapGenericStage->addShaderStage(ppvertShaderModule->createShaderStage(VK_SHADER_STAGE_VERTEX_BIT, "main"));
    shadowMapGenericStage->addShaderStage(ppdeferredresolveShaderModule->createShaderStage(VK_SHADER_STAGE_FRAGMENT_BIT, "main"));
    shadowMapGenericStage->addDescriptorSetLayout(ppSetLayout->layout);
    shadowMapGenericStage->addDescriptorSetLayout(Application::instance->shadowMapDataLayout->layout);
    shadowMapGenericStage->addOutputImage(deferredResolvedImage);
    shadowMapGenericStage->additiveBlending = true;
    shadowMapGenericStage->compile();

    //##########################//

    auto ppoutputfragShaderModule = new VulkanShaderModule(vulkan, "../../shaders/compiled/pp-output.frag.spv");

    postProcessZygoteStage = new VulkanRenderStage(vulkan);

    postProcessZygoteStage->setViewport(ext);
    postProcessZygoteStage->addShaderStage(ppvertShaderModule->createShaderStage(VK_SHADER_STAGE_VERTEX_BIT, "main"));
    postProcessZygoteStage->addShaderStage(ppoutputfragShaderModule->createShaderStage(VK_SHADER_STAGE_FRAGMENT_BIT, "main"));
    postProcessZygoteStage->addDescriptorSetLayout(ppSetLayout->layout);

    //##########################//

    postProcessSet = ppSetLayout->generateDescriptorSet();
    postProcessSet->bindUniformBuffer(0, uboHighFrequencyBuffer);
    postProcessSet->bindUniformBuffer(1, uboLowFrequencyBuffer);
    postProcessSet->bindImageViewSampler(2, diffuseImage);
    postProcessSet->bindImageViewSampler(3, normalImage);
    postProcessSet->bindImageViewSampler(4, distanceImage);
    postProcessSet->bindImageViewSampler(5, ambientImage);
    postProcessSet->bindImageViewSampler(6, Application::instance->ui->outputImage);
    postProcessSet->bindImageViewSampler(7, deferredResolvedImage);
    postProcessSet->bindImageViewSampler(8, ambientShadowRenderer->distanceImage);
    postProcessSet->update();

    ppShadeAmbientStage->setSets({ postProcessSet });
    postProcessZygoteStage->setSets({ postProcessSet });

    renderer = new VulkanRenderer(vulkan);
    renderer->setMeshStage(meshRenderStage);
    renderer->addPostProcessingStage(ppShadeAmbientStage);
    renderer->setOutputStage(postProcessZygoteStage);
    renderer->compile();
}

void Renderer::deferredDraw()
{
    for (int i = 0; i < lights.size(); i++) {
        lights[i]->update();
    }
}

void Renderer::deferredResolve()
{
    shadowMapGenericStage->beginDrawing();
    for (int i = 0; i < lights.size(); i++) {
        lights[i]->recordResolveCommands(shadowMapGenericStage, postProcessSet);
    }
    shadowMapGenericStage->endDrawing();
    //vkQueueWaitIdle(vulkan->mainQueue);
    shadowMapGenericStage->submitNoSemaphores({});//todo no synchronization
}

void Renderer::renderToSwapChain(Camera *camera)
{

    Application::instance->scene->prepareFrame(glm::mat4(1));


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


    //ambientShadowCamera->transformation->setPosition(camera->transformation->getPosition());
   // ambientShadowRenderer->render(ambientShadowCamera);

    deferredDraw();
    renderer->beginDrawing();
    Application::instance->scene->draw(glm::mat4(1), renderer->getMesh3dStage());
    
    renderer->endDrawing();

    deferredResolve();
}

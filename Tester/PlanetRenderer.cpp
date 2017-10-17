#include "stdafx.h"
#include "PlanetRenderer.h"
#include "stdafx.h"
#include "vulkan.h"

PlanetRenderer::PlanetRenderer(VulkanToolkit* ivulkan, int iwidth, int iheight) :
    width(iwidth), height(iheight), vulkan(ivulkan)
{ 
    //**********************//

    starsLayout = new VulkanDescriptorSetLayout(vulkan);
    starsLayout->addField(0, VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER, VK_SHADER_STAGE_ALL_GRAPHICS);
    starsLayout->addField(1, VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER, VK_SHADER_STAGE_ALL_GRAPHICS);
    starsLayout->compile();

    starsImage = new VulkanImage(vulkan, width, height, VK_FORMAT_R16G16B16A16_SFLOAT, VK_IMAGE_TILING_OPTIMAL,
        VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT | VK_IMAGE_USAGE_SAMPLED_BIT, VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT, VK_IMAGE_ASPECT_COLOR_BIT, VK_IMAGE_LAYOUT_PREINITIALIZED, false);

    auto starsvert = new VulkanShaderModule(vulkan, "../../shaders/compiled/cosmos-stars.vert.spv");
    auto starsfrag = new VulkanShaderModule(vulkan, "../../shaders/compiled/cosmos-stars.frag.spv");

    starsStage = new VulkanRenderStage(vulkan);
    starsStage->setViewport(width, height);
    starsStage->addShaderStage(starsvert->createShaderStage(VK_SHADER_STAGE_VERTEX_BIT, "main"));
    starsStage->addShaderStage(starsfrag->createShaderStage(VK_SHADER_STAGE_FRAGMENT_BIT, "main"));
    starsStage->addDescriptorSetLayout(starsLayout->layout);
    starsStage->addOutputImage(starsImage);

    //**********************//

    planetsLayout = new VulkanDescriptorSetLayout(vulkan);
    planetsLayout->addField(0, VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER, VK_SHADER_STAGE_ALL_GRAPHICS);
    planetsLayout->addField(1, VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER, VK_SHADER_STAGE_ALL_GRAPHICS);
    planetsLayout->compile();

    planetsImage = new VulkanImage(vulkan, width, height, VK_FORMAT_R16G16B16A16_SFLOAT, VK_IMAGE_TILING_OPTIMAL,
        VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT | VK_IMAGE_USAGE_SAMPLED_BIT, VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT, VK_IMAGE_ASPECT_COLOR_BIT, VK_IMAGE_LAYOUT_PREINITIALIZED, false);

    auto planetsvert = new VulkanShaderModule(vulkan, "../../shaders/compiled/cosmos-planets.vert.spv");
    auto planetsfrag = new VulkanShaderModule(vulkan, "../../shaders/compiled/cosmos-planets.frag.spv");

    planetsStage = new VulkanRenderStage(vulkan);
    planetsStage->setViewport(width, height);
    planetsStage->addShaderStage(planetsvert->createShaderStage(VK_SHADER_STAGE_VERTEX_BIT, "main"));
    planetsStage->addShaderStage(planetsfrag->createShaderStage(VK_SHADER_STAGE_FRAGMENT_BIT, "main"));
    planetsStage->addDescriptorSetLayout(planetsLayout->layout);
    planetsStage->addOutputImage(planetsImage);

    //**********************//

    combineLayout = new VulkanDescriptorSetLayout(vulkan);
    combineLayout->addField(0, VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER, VK_SHADER_STAGE_FRAGMENT_BIT);
    combineLayout->addField(1, VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER, VK_SHADER_STAGE_FRAGMENT_BIT);
    combineLayout->compile();

    outputImage = new VulkanImage(vulkan, width, height, VK_FORMAT_R16G16B16A16_SFLOAT, VK_IMAGE_TILING_OPTIMAL,
        VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT | VK_IMAGE_USAGE_SAMPLED_BIT, VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT, VK_IMAGE_ASPECT_COLOR_BIT, VK_IMAGE_LAYOUT_PREINITIALIZED, false);

    auto combinevert = new VulkanShaderModule(vulkan, "../../shaders/compiled/cosmos-combine.vert.spv");
    auto combinefrag = new VulkanShaderModule(vulkan, "../../shaders/compiled/cosmos-combine.frag.spv");

    combineStage = new VulkanRenderStage(vulkan);
    combineStage->setViewport(width, height);
    combineStage->addShaderStage(combinevert->createShaderStage(VK_SHADER_STAGE_VERTEX_BIT, "main"));
    combineStage->addShaderStage(combinefrag->createShaderStage(VK_SHADER_STAGE_FRAGMENT_BIT, "main"));
    combineStage->addDescriptorSetLayout(combineLayout->layout);
    combineStage->addOutputImage(outputImage);

    //**********************//

    renderer = new VulkanRenderer(vulkan);
    renderer->addPostProcessingStage(starsStage);
    renderer->addPostProcessingStage(planetsStage);
    renderer->addPostProcessingStage(combineStage);
    renderer->compile();
}


PlanetRenderer::~PlanetRenderer()
{
}

void PlanetRenderer::updateClosestPlanetsAndStar(Camera * camera)
{
    closestPlanets.clear();
    for (int i = 0; i < planets.size(); i++) {
        if (glm::distance(glm::vec3(planets[i].position * scale), camera->transformation->getPosition()) < planets[i].size * scale * 1112.0) {
            closestPlanets.push_back(planets[i]);
        }
    }
    int closestStarIndex = 0;
    float closestStarDistance = glm::distance(camera->transformation->getPosition(), glm::vec3(stars[0].position * scale));
    for (int i = 0; i < stars.size(); i++) {
        float dst = glm::distance(camera->transformation->getPosition(), glm::vec3(stars[i].position * scale));
        if (dst < closestStarDistance) {
            closestStarDistance = dst;
            closestStarIndex = i;
        }
    }
    closestStar = stars[closestStarIndex];
}

void PlanetRenderer::updateCameraBuffer(Camera * camera)
{
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
    cameraDataBuffer->map(0, bb.buffer.size(), &data);
    memcpy(data, bb.getPointer(), bb.buffer.size());
    cameraDataBuffer->unmap();

}

void PlanetRenderer::updateObjectsBuffers()
{
    void* data;
    VulkanBinaryBufferBuilder bb2 = VulkanBinaryBufferBuilder();
    bb2.emplaceFloat32((float)closestStar.noiseSeed);
    bb2.emplaceFloat32((float)closestStar.size);
    bb2.emplaceFloat32((float)0.0f);
    bb2.emplaceFloat32((float)0.0f);

    closestStarData->map(0, bb2.buffer.size(), &data);
    memcpy(data, bb2.getPointer(), bb2.buffer.size());
    closestStarData->unmap();

    VulkanBinaryBufferBuilder bb3 = VulkanBinaryBufferBuilder();
    bb3.emplaceFloat32((int)closestPlanets.size());
    bb3.emplaceFloat32((int)0);
    bb3.emplaceFloat32((int)0);
    bb3.emplaceFloat32((int)0);
    for (int i = 0; i < closestPlanets.size(); i++) {
        glm::dvec2 pos = closestPlanets[i].position;// -closestStar.position;
        bb3.emplaceFloat32((float)closestPlanets[i].position.x);
        bb3.emplaceFloat32((float)closestPlanets[i].position.y);
        bb3.emplaceFloat32((float)closestPlanets[i].position.z);
        bb3.emplaceFloat32((float)0.0f);
        bb3.emplaceFloat32((float)closestPlanets[i].noiseSeed);
        bb3.emplaceFloat32((float)closestPlanets[i].size);
        bb3.emplaceFloat32((float)closestPlanets[i].starDistance);
        bb3.emplaceFloat32((float)0.0f);
    }

    closestPlanetsData->map(0, bb3.buffer.size(), &data);
    memcpy(data, bb3.getPointer(), bb3.buffer.size());
    closestPlanetsData->unmap();
}

void PlanetRenderer::draw()
{
    renderer->beginDrawing();

    renderer->endDrawing();
}

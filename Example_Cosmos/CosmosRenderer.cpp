#include "stdafx.h"
#include "CosmosRenderer.h"
#include "GalaxyGenerator.h"
#include "stdafx.h"
#include "vulkan.h"

CosmosRenderer::CosmosRenderer(VulkanToolkit* ivulkan, int iwidth, int iheight) :
    width(iwidth), height(iheight), vulkan(ivulkan)
{
    nearbyStars = {};
    galaxy = new GalaxyGenerator();

    cameraDataBuffer = new VulkanGenericBuffer(vulkan, VK_BUFFER_USAGE_UNIFORM_BUFFER_BIT, sizeof(float) * 1024 * 1024);
    starsDataBuffer = new VulkanGenericBuffer(vulkan, VK_BUFFER_USAGE_STORAGE_BUFFER_BIT, sizeof(float) * 1024 * 1024);
    planetsDataBuffer = new VulkanGenericBuffer(vulkan, VK_BUFFER_USAGE_STORAGE_BUFFER_BIT, sizeof(float) * 1024 * 1024);
    moonsDataBuffer = new VulkanGenericBuffer(vulkan, VK_BUFFER_USAGE_STORAGE_BUFFER_BIT, sizeof(float) * 1024 * 1024);

    //**********************//

    celestialLayout = new VulkanDescriptorSetLayout(vulkan);
    celestialLayout->addField(0, VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER, VK_SHADER_STAGE_ALL_GRAPHICS);
    celestialLayout->addField(1, VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER, VK_SHADER_STAGE_ALL_GRAPHICS);
    celestialLayout->addField(2, VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER, VK_SHADER_STAGE_ALL_GRAPHICS);
    celestialLayout->addField(3, VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER, VK_SHADER_STAGE_ALL_GRAPHICS);
    celestialLayout->compile();

    celestialSet = celestialLayout->generateDescriptorSet();
    celestialSet->bindUniformBuffer(0, cameraDataBuffer);
    celestialSet->bindUniformBuffer(1, starsDataBuffer);
    celestialSet->bindUniformBuffer(2, planetsDataBuffer);
    celestialSet->bindUniformBuffer(3, moonsDataBuffer);
    celestialSet->update();

    celestialImage = new VulkanImage(vulkan, width, height, VK_FORMAT_R16G16B16A16_SFLOAT, VK_IMAGE_TILING_OPTIMAL,
        VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT | VK_IMAGE_USAGE_SAMPLED_BIT, VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT, VK_IMAGE_ASPECT_COLOR_BIT, VK_IMAGE_LAYOUT_PREINITIALIZED, false);

    auto celestialvert = new VulkanShaderModule(vulkan, "../../shaders/compiled/cosmos-celestial.vert.spv");
    auto celestialfrag = new VulkanShaderModule(vulkan, "../../shaders/compiled/cosmos-celestial.frag.spv");

    celestialStage = new VulkanRenderStage(vulkan);
    celestialStage->setViewport(width, height);
    celestialStage->addShaderStage(celestialvert->createShaderStage(VK_SHADER_STAGE_VERTEX_BIT, "main"));
    celestialStage->addShaderStage(celestialfrag->createShaderStage(VK_SHADER_STAGE_FRAGMENT_BIT, "main"));
    celestialStage->addDescriptorSetLayout(celestialLayout->layout);
    celestialStage->addOutputImage(celestialImage);
    celestialStage->setSets({ celestialSet });

    //**********************//

    combineLayout = new VulkanDescriptorSetLayout(vulkan);
    combineLayout->addField(0, VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER, VK_SHADER_STAGE_FRAGMENT_BIT);
    combineLayout->compile();

    combineSet = combineLayout->generateDescriptorSet();
    combineSet->bindImageViewSampler(0, celestialImage);
    combineSet->update();

    outputImage = new VulkanImage(vulkan, width, height, VK_FORMAT_R16G16B16A16_SFLOAT, VK_IMAGE_TILING_OPTIMAL,
        VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT | VK_IMAGE_USAGE_SAMPLED_BIT, VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT, VK_IMAGE_ASPECT_COLOR_BIT, VK_IMAGE_LAYOUT_PREINITIALIZED, false);

    auto combinevert = new VulkanShaderModule(vulkan, "../../shaders/compiled/cosmos-combine.vert.spv");
    auto combinefrag = new VulkanShaderModule(vulkan, "../../shaders/compiled/cosmos-combine.frag.spv");

    combineStage = new VulkanRenderStage(vulkan);
    combineStage->setViewport(width, height);
    combineStage->addShaderStage(combinevert->createShaderStage(VK_SHADER_STAGE_VERTEX_BIT, "main"));
    combineStage->addShaderStage(combinefrag->createShaderStage(VK_SHADER_STAGE_FRAGMENT_BIT, "main"));
    combineStage->addDescriptorSetLayout(combineLayout->layout);
    combineStage->setSets({ combineSet });
    //combineStage->addOutputImage(outputImage); //outputs to swapchain, managed automatically

    //**********************//

    renderer = new VulkanRenderer(vulkan);
    renderer->addPostProcessingStage(celestialStage);
    renderer->setOutputStage(combineStage);
    renderer->compile();
}


CosmosRenderer::~CosmosRenderer()
{
}

void CosmosRenderer::updateClosestPlanetsAndStar(Camera * camera)
{
    glm::dvec3 hipos = camera->transformation->getPosition();
    nearbyStars.clear();
    nearbyStars.push_back(galaxy->generateStarInfo(galaxy->findClosestStar(hipos.x * 1000.0, hipos.y * 1000.0, hipos.z * 1000.0)));
}

void CosmosRenderer::updateCameraBuffer(Camera * camera)
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

    glm::vec3 newcamerapos = glm::vec3(
        camera->transformation->getPosition()
    );
    bb.emplaceGeneric((unsigned char*)&newcamerapos, sizeof(camera->cone->leftBottom));
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

void CosmosRenderer::updateObjectsBuffers(Camera * camera)
{
    VulkanBinaryBufferBuilder starsBB = VulkanBinaryBufferBuilder();
    VulkanBinaryBufferBuilder planetsBB = VulkanBinaryBufferBuilder();
    VulkanBinaryBufferBuilder moonsBB = VulkanBinaryBufferBuilder();

    int planetsCount = 0;
    int moonsCount = 0;
    for (int s = 0; s < nearbyStars.size(); s++) {
        auto star = nearbyStars[s];
        for (int p = 0; p < star.planets.size(); p++) {
            auto planet = star.planets[p];
            planetsCount++;
            for (int m = 0; m < planet.moons.size(); m++) {
                moonsCount++;
            }
        }
    }

    starsBB.emplaceInt32(nearbyStars.size());
    starsBB.emplaceInt32(nearbyStars.size());
    starsBB.emplaceInt32(nearbyStars.size());
    starsBB.emplaceInt32(nearbyStars.size());

    planetsBB.emplaceInt32(planetsCount);
    planetsBB.emplaceInt32(planetsCount);
    planetsBB.emplaceInt32(planetsCount);
    planetsBB.emplaceInt32(planetsCount);

    moonsBB.emplaceInt32(moonsCount);
    moonsBB.emplaceInt32(moonsCount);
    moonsBB.emplaceInt32(moonsCount);
    moonsBB.emplaceInt32(moonsCount);

    for (int s = 0; s < nearbyStars.size(); s++) {
        auto star = nearbyStars[s];

        glm::dvec3 starpos = glm::dvec3(star.starData.x * 0.001, star.starData.y * 0.001, star.starData.z * 0.001);
        glm::dvec3 spos = starpos - glm::dvec3(camera->transformation->getPosition());
        starsBB.emplaceFloat32((float)spos.x);
        starsBB.emplaceFloat32((float)spos.y);
        starsBB.emplaceFloat32((float)spos.z);
        starsBB.emplaceFloat32((float)star.radius * 0.001);
        
        starsBB.emplaceFloat32((float)star.color.x);
        starsBB.emplaceFloat32((float)star.color.y);
        starsBB.emplaceFloat32((float)star.color.z);
        starsBB.emplaceFloat32((float)star.age);
        
        starsBB.emplaceFloat32((float)star.orbitPlane.x);
        starsBB.emplaceFloat32((float)star.orbitPlane.y);
        starsBB.emplaceFloat32((float)star.orbitPlane.z);
        starsBB.emplaceFloat32((float)star.rotationSpeed);
        
        starsBB.emplaceFloat32((float)star.spotsIntensity);
        starsBB.emplaceFloat32((float)0.0f);
        starsBB.emplaceFloat32((float)0.0f);
        starsBB.emplaceFloat32((float)0.0f);

        for (int p = 0; p < star.planets.size(); p++) {
            auto planet = star.planets[p];
            glm::dvec3 orbitplaneTangent = glm::normalize(glm::cross(star.orbitPlane, glm::vec3(0.0, 1.0, 0.0)));
            glm::dvec3 planetpos = starpos + orbitplaneTangent * planet.starDistance * 0.001;
            glm::dvec3 ppos = planetpos - glm::dvec3(camera->transformation->getPosition());
            planetsBB.emplaceFloat32((float)ppos.x);
            planetsBB.emplaceFloat32((float)ppos.y);
            planetsBB.emplaceFloat32((float)ppos.z);
            planetsBB.emplaceFloat32((float)planet.radius * 0.001);

            planetsBB.emplaceFloat32((float)planet.terrainMaxLevel);
            planetsBB.emplaceFloat32((float)planet.fluidMaxLevel);
            planetsBB.emplaceFloat32((float)planet.starDistance);
            planetsBB.emplaceFloat32((float)0.0f);

            planetsBB.emplaceFloat32((float)planet.habitableChance);
            planetsBB.emplaceFloat32((float)planet.orbitSpeed);
            planetsBB.emplaceFloat32((float)planet.atmosphereRadius);
            planetsBB.emplaceFloat32((float)planet.atmosphereAbsorbStrength);

            planetsBB.emplaceFloat32((float)planet.preferredColor.x);
            planetsBB.emplaceFloat32((float)planet.preferredColor.y);
            planetsBB.emplaceFloat32((float)planet.preferredColor.z);
            planetsBB.emplaceFloat32((float)0.0f);

            planetsBB.emplaceFloat32((float)planet.atmosphereAbsorbColor.x);
            planetsBB.emplaceFloat32((float)planet.atmosphereAbsorbColor.y);
            planetsBB.emplaceFloat32((float)planet.atmosphereAbsorbColor.z);
            planetsBB.emplaceFloat32((float)0.0f);

            for (int m = 0; m < planet.moons.size(); m++) {
                auto moon = planet.moons[m];
                glm::dvec3 orbitplaneTangent = glm::normalize(glm::cross(moon.orbitPlane, glm::vec3(0.0, 1.0, 0.0)));
                glm::dvec3 moonpos = planetpos + orbitplaneTangent * moon.planetDistance * 0.001;
                glm::dvec3 mpos = moonpos - glm::dvec3(camera->transformation->getPosition());
                moonsBB.emplaceFloat32((float)mpos.x);
                moonsBB.emplaceFloat32((float)mpos.y);
                moonsBB.emplaceFloat32((float)mpos.z);
                moonsBB.emplaceFloat32((float)moon.radius * 0.001);
                
                moonsBB.emplaceFloat32((float)moon.orbitPlane.x);
                moonsBB.emplaceFloat32((float)moon.orbitPlane.y);
                moonsBB.emplaceFloat32((float)moon.orbitPlane.z);
                moonsBB.emplaceFloat32((float)moon.orbitSpeed);
                
                moonsBB.emplaceFloat32((float)moon.preferredColor.x);
                moonsBB.emplaceFloat32((float)moon.preferredColor.y);
                moonsBB.emplaceFloat32((float)moon.preferredColor.z);
                moonsBB.emplaceFloat32((float)moon.terrainMaxLevel);
               
                moonsBB.emplaceFloat32((float)moon.planetDistance);
                moonsBB.emplaceFloat32((float)0.0f);
                moonsBB.emplaceFloat32((float)0.0f);
                moonsBB.emplaceFloat32((float)0.0f);
            }
        }
    }


    void* data;
    starsDataBuffer->map(0, starsBB.buffer.size(), &data);
    memcpy(data, starsBB.getPointer(), starsBB.buffer.size());
    starsDataBuffer->unmap();

    planetsDataBuffer->map(0, planetsBB.buffer.size(), &data);
    memcpy(data, planetsBB.getPointer(), planetsBB.buffer.size());
    planetsDataBuffer->unmap();

    moonsDataBuffer->map(0, moonsBB.buffer.size(), &data);
    memcpy(data, moonsBB.getPointer(), moonsBB.buffer.size());
    moonsDataBuffer->unmap();
}

void CosmosRenderer::draw()
{
    renderer->beginDrawing();

    renderer->endDrawing();
}

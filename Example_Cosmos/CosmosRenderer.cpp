#include "stdafx.h"
#include "CosmosRenderer.h"
#include "GalaxyGenerator.h"
#include "stdafx.h"
#include "vulkan.h"

CosmosRenderer::CosmosRenderer(VulkanToolkit* ivulkan, int iwidth, int iheight) :
    width(iwidth), height(iheight), vulkan(ivulkan)
{
    internalCamera = new Camera();
    nearbyStars = {};
    galaxy = new GalaxyGenerator();

    auto assets = AssetLoader(vulkan);

    cube3dInfo = assets.loadObject3dInfoFile("cube1unitradius.raw");

    cameraDataBuffer = new VulkanGenericBuffer(vulkan, VK_BUFFER_USAGE_UNIFORM_BUFFER_BIT, sizeof(float) * 1024);
    starsDataBuffer = new VulkanGenericBuffer(vulkan, VK_BUFFER_USAGE_STORAGE_BUFFER_BIT, 1024 * 1024 * 128); // i want 256 mb 1024 * 1024 * 256
    planetsDataBuffer = new VulkanGenericBuffer(vulkan, VK_BUFFER_USAGE_STORAGE_BUFFER_BIT, sizeof(float) * 1024 * 1024);
    moonsDataBuffer = new VulkanGenericBuffer(vulkan, VK_BUFFER_USAGE_STORAGE_BUFFER_BIT, sizeof(float) * 1024 * 1024);

    //**********************//

    celestialLayout = new VulkanDescriptorSetLayout(vulkan);
    celestialLayout->addField(0, VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER, VK_SHADER_STAGE_ALL_GRAPHICS);
    celestialLayout->addField(1, VK_DESCRIPTOR_TYPE_STORAGE_BUFFER, VK_SHADER_STAGE_ALL_GRAPHICS);
    celestialLayout->addField(2, VK_DESCRIPTOR_TYPE_STORAGE_BUFFER, VK_SHADER_STAGE_ALL_GRAPHICS);
    celestialLayout->addField(3, VK_DESCRIPTOR_TYPE_STORAGE_BUFFER, VK_SHADER_STAGE_ALL_GRAPHICS);
    celestialLayout->compile();

    celestialSet = celestialLayout->generateDescriptorSet();
    celestialSet->bindUniformBuffer(0, cameraDataBuffer);
    celestialSet->bindStorageBuffer(1, starsDataBuffer);
    celestialSet->bindStorageBuffer(2, planetsDataBuffer);
    celestialSet->bindStorageBuffer(3, moonsDataBuffer);
    celestialSet->update();

    celestialImage = new VulkanImage(vulkan, width, height, VK_FORMAT_R16G16B16A16_SFLOAT, VK_IMAGE_TILING_OPTIMAL,
        VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT | VK_IMAGE_USAGE_SAMPLED_BIT, VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT, VK_IMAGE_ASPECT_COLOR_BIT, VK_IMAGE_LAYOUT_PREINITIALIZED, false);

    starsImage = new VulkanImage(vulkan, width, height, VK_FORMAT_R16G16B16A16_SFLOAT, VK_IMAGE_TILING_OPTIMAL,
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

    auto starsvert = new VulkanShaderModule(vulkan, "../../shaders/compiled/cosmos-stars.vert.spv");
    auto starsfrag = new VulkanShaderModule(vulkan, "../../shaders/compiled/cosmos-stars.frag.spv");

    starsStage = new VulkanRenderStage(vulkan);
    starsStage->setViewport(width, height);
    starsStage->addShaderStage(starsvert->createShaderStage(VK_SHADER_STAGE_VERTEX_BIT, "main"));
    starsStage->addShaderStage(starsfrag->createShaderStage(VK_SHADER_STAGE_FRAGMENT_BIT, "main"));
    starsStage->addDescriptorSetLayout(celestialLayout->layout);
    starsStage->addOutputImage(starsImage);
    starsStage->setSets({ celestialSet });
    starsStage->additiveBlending = true;
    starsStage->cullFlags = VK_CULL_MODE_BACK_BIT;
   // starsStage->topology = VK_PRIMITIVE_TOPOLOGY_POINT_LIST;
    starsStage->compile();

    //**********************//

    combineLayout = new VulkanDescriptorSetLayout(vulkan);
    combineLayout->addField(0, VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER, VK_SHADER_STAGE_FRAGMENT_BIT);
    combineLayout->addField(1, VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER, VK_SHADER_STAGE_FRAGMENT_BIT);
    combineLayout->compile();

    combineSet = combineLayout->generateDescriptorSet();
    combineSet->bindImageViewSampler(0, celestialImage);
    combineSet->bindImageViewSampler(1, starsImage);
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
    for (int i = 0; i < 10000; i++) {
        nearbyStars.push_back(galaxy->generateStarInfo(i));
    }
    // galaxy->findClosestStar(hipos.x * 1000.0, hipos.y * 1000.0, hipos.z * 1000.0)));
}

void CosmosRenderer::updateCameraBuffer(Camera * camera)
{
    internalCamera->projectionMatrix = camera->projectionMatrix;
    internalCamera->transformation->setOrientation(camera->transformation->getOrientation());
    internalCamera->transformation->setPosition(glm::vec3(0.0));
    VulkanBinaryBufferBuilder bb = VulkanBinaryBufferBuilder();
    double xpos, ypos;
    glfwGetCursorPos(vulkan->window, &xpos, &ypos);

    glm::mat4 clip(1.0f, 0.0f, 0.0f, 0.0f,
        0.0f, -1.0f, 0.0f, 0.0f,
        0.0f, 0.0f, 0.5f, 0.0f,
        0.0f, 0.0f, 0.5f, 1.0f);
    glm::mat4 vpmatrix = clip * internalCamera->projectionMatrix * internalCamera->transformation->getInverseWorldTransform();

    glm::mat4 cameraViewMatrix = internalCamera->transformation->getInverseWorldTransform();
    glm::mat4 cameraRotMatrix = internalCamera->transformation->getRotationMatrix();
    glm::mat4 rpmatrix = internalCamera->projectionMatrix * inverse(cameraRotMatrix);
    internalCamera->cone->update(inverse(rpmatrix));

    bb.emplaceFloat32((float)glfwGetTime());
    bb.emplaceFloat32(0.0f);
    bb.emplaceFloat32((float)xpos / (float)width);
    bb.emplaceFloat32((float)ypos / (float)height);
    bb.emplaceGeneric((unsigned char*)&rpmatrix, sizeof(rpmatrix));

    glm::vec3 newcamerapos = glm::vec3(
        internalCamera->transformation->getPosition()
    );
    bb.emplaceGeneric((unsigned char*)&newcamerapos, sizeof(internalCamera->cone->leftBottom));
    bb.emplaceFloat32(0.0f);

    bb.emplaceGeneric((unsigned char*)&(internalCamera->cone->leftBottom), sizeof(internalCamera->cone->leftBottom));
    bb.emplaceFloat32(0.0f);
    bb.emplaceGeneric((unsigned char*)&(internalCamera->cone->rightBottom - internalCamera->cone->leftBottom), sizeof(internalCamera->cone->leftBottom));
    bb.emplaceFloat32(0.0f);
    bb.emplaceGeneric((unsigned char*)&(internalCamera->cone->leftTop - internalCamera->cone->leftBottom), sizeof(internalCamera->cone->leftBottom));
    bb.emplaceFloat32(0.0f);
    bb.emplaceFloat32((float)width);
    bb.emplaceFloat32((float)height);

    void* data;
    cameraDataBuffer->map(0, bb.buffer.size(), &data);
    memcpy(data, bb.getPointer(), bb.buffer.size());
    cameraDataBuffer->unmap();

}

void CosmosRenderer::updateObjectsBuffers(Camera * camera)
{
    void* data1, *data2, *data3;
    starsDataBuffer->map(0, starsDataBuffer->size, &data1);
    planetsDataBuffer->map(0, planetsDataBuffer->size, &data2);
    moonsDataBuffer->map(0, moonsDataBuffer->size, &data3);
    lastGravity = glm::vec3(0.0);
    while (true) {
        glm::vec3 newGravity = glm::vec3(0.0);
        float time = glfwGetTime() + 100.0;
        VulkanBinaryBufferBuilder starsBB = VulkanBinaryBufferBuilder();
        VulkanBinaryBufferBuilder planetsBB = VulkanBinaryBufferBuilder();
        VulkanBinaryBufferBuilder moonsBB = VulkanBinaryBufferBuilder();

        int planetsCount = 0;
        int moonsCount = 0;
        int closestStar = 0;
        double closestDistance = 9999999999.0;
        for (int s = 0; s < nearbyStars.size(); s++) {
            auto star = nearbyStars[s];

            glm::dvec3 starpos = glm::dvec3(star.starData.x * scale, star.starData.y * scale, star.starData.z * scale);
            glm::dvec3 spos = starpos - glm::dvec3(camera->transformation->getPosition());

            float dst = glm::length(spos);
            if (dst < closestDistance) {
                closestDistance = dst;
                closestStar = s;
            }
        }
        auto cstar = nearbyStars[closestStar];
        for (int p = 0; p < cstar.planets.size(); p++) {
            auto planet = cstar.planets[p];
            planetsCount++;
            for (int m = 0; m < planet.moons.size(); m++) {
                moonsCount++;
            }
        }
        int planetIndex = 0;

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

            glm::dvec3 starpos = glm::dvec3(star.starData.x * scale, star.starData.y * scale, star.starData.z * scale);
            glm::dvec3 spos = starpos - glm::dvec3(camera->transformation->getPosition());
            glm::vec3 sdir = glm::normalize(spos);
            double sdistance = glm::length(spos) / scale;
            newGravity += glm::dvec3(sdir) * galaxy->calculateGravity(sdistance, galaxy->calculateMass(star.radius, 1408.0));

            starsBB.emplaceFloat32((float)spos.x);
            starsBB.emplaceFloat32((float)spos.y);
            starsBB.emplaceFloat32((float)spos.z);
            starsBB.emplaceFloat32((float)star.radius * scale);

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

        }

        glm::dvec3 starpos = glm::dvec3(cstar.starData.x * scale, cstar.starData.y * scale, cstar.starData.z * scale);
        for (int p = 0; p < cstar.planets.size(); p++) {
            auto planet = cstar.planets[p];
            glm::vec3 orbitplaneTangent = glm::normalize(glm::cross(cstar.orbitPlane, glm::vec3(0.0, 1.0, 0.0)));
            glm::dvec3 planetpos = starpos + glm::dvec3(glm::angleAxis((float)planet.orbitSpeed * time * 0.001f, cstar.orbitPlane) * orbitplaneTangent) * planet.starDistance * scale;
            glm::dvec3 ppos = planetpos - glm::dvec3(camera->transformation->getPosition());

            glm::vec3 pdir = glm::normalize(ppos);
            double pdistance = glm::length(ppos) / scale;
            newGravity += glm::dvec3(pdir) * galaxy->calculateGravity(pdistance, galaxy->calculateMass(planet.radius));

            planetsBB.emplaceFloat32((float)ppos.x);
            planetsBB.emplaceFloat32((float)ppos.y);
            planetsBB.emplaceFloat32((float)ppos.z);
            planetsBB.emplaceFloat32((float)planet.radius * scale);

            planetsBB.emplaceFloat32((float)planet.terrainMaxLevel);
            planetsBB.emplaceFloat32((float)planet.fluidMaxLevel);
            planetsBB.emplaceFloat32((float)planet.starDistance * scale);
            planetsBB.emplaceFloat32((float)0.0f);

            planetsBB.emplaceFloat32((float)planet.habitableChance);
            planetsBB.emplaceFloat32((float)planet.orbitSpeed);
            planetsBB.emplaceFloat32((float)planet.atmosphereRadius * scale);
            planetsBB.emplaceFloat32((float)planet.atmosphereAbsorbStrength);

            planetsBB.emplaceFloat32((float)planet.preferredColor.x);
            planetsBB.emplaceFloat32((float)planet.preferredColor.y);
            planetsBB.emplaceFloat32((float)planet.preferredColor.z);
            planetsBB.emplaceFloat32((float)0.0f);

            planetsBB.emplaceFloat32((float)planet.atmosphereAbsorbColor.x);
            planetsBB.emplaceFloat32((float)planet.atmosphereAbsorbColor.y);
            planetsBB.emplaceFloat32((float)planet.atmosphereAbsorbColor.z);
            planetsBB.emplaceFloat32((float)0.0f);

            planetsBB.emplaceInt32((int)closestStar);
            planetsBB.emplaceInt32((int)0);
            planetsBB.emplaceInt32((int)0);
            planetsBB.emplaceInt32((int)0);

            for (int m = 0; m < planet.moons.size(); m++) {
                auto moon = planet.moons[m];
                glm::vec3 orbitplaneTangent = glm::normalize(glm::cross(moon.orbitPlane, glm::vec3(0.0, 1.0, 0.0)));
                glm::dvec3 moonpos = planetpos + glm::dvec3(glm::angleAxis((float)planet.orbitSpeed * time * 0.001f, cstar.orbitPlane) * orbitplaneTangent) * moon.planetDistance * scale;
                glm::dvec3 mpos = moonpos - glm::dvec3(camera->transformation->getPosition());

                glm::vec3 mdir = glm::normalize(mpos);
                double mdistance = glm::length(mpos) / scale;
                newGravity += glm::dvec3(mdir) * galaxy->calculateGravity(mdistance, galaxy->calculateMass(moon.radius));

                moonsBB.emplaceFloat32((float)mpos.x);
                moonsBB.emplaceFloat32((float)mpos.y);
                moonsBB.emplaceFloat32((float)mpos.z);
                moonsBB.emplaceFloat32((float)moon.radius * scale);

                moonsBB.emplaceFloat32((float)moon.orbitPlane.x);
                moonsBB.emplaceFloat32((float)moon.orbitPlane.y);
                moonsBB.emplaceFloat32((float)moon.orbitPlane.z);
                moonsBB.emplaceFloat32((float)moon.orbitSpeed);

                moonsBB.emplaceFloat32((float)moon.preferredColor.x);
                moonsBB.emplaceFloat32((float)moon.preferredColor.y);
                moonsBB.emplaceFloat32((float)moon.preferredColor.z);
                moonsBB.emplaceFloat32((float)moon.terrainMaxLevel);

                moonsBB.emplaceFloat32((float)moon.planetDistance * scale);
                moonsBB.emplaceFloat32((float)0.0f);
                moonsBB.emplaceFloat32((float)0.0f);
                moonsBB.emplaceFloat32((float)0.0f);

                moonsBB.emplaceInt32((int)planetIndex);
                moonsBB.emplaceInt32((int)0);
                moonsBB.emplaceInt32((int)0);
                moonsBB.emplaceInt32((int)0);
            }
            planetIndex++;
        }



        memcpy(data1, starsBB.getPointer(), starsBB.buffer.size());

        memcpy(data2, planetsBB.getPointer(), planetsBB.buffer.size());

        memcpy(data3, moonsBB.getPointer(), moonsBB.buffer.size());
        lastGravity = newGravity;
    }
    starsDataBuffer->unmap();
    planetsDataBuffer->unmap();
    moonsDataBuffer->unmap();
}

void CosmosRenderer::draw()
{
    starsStage->beginDrawing();

    starsStage->drawMesh(cube3dInfo, nearbyStars.size());

    starsStage->endDrawing();
    starsStage->submitNoSemaphores({});


    renderer->beginDrawing();

    renderer->endDrawing();
}

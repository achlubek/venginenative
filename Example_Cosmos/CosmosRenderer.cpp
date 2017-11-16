#include "stdafx.h"
#include "CosmosRenderer.h"
#include "GalaxyGenerator.h"
#include "stdafx.h"
#include "vulkan.h"

CosmosRenderer::CosmosRenderer(VulkanToolkit* ivulkan, int iwidth, int iheight) :
    width(iwidth), height(iheight), vulkan(ivulkan), assets(AssetLoader(ivulkan))
{
    internalCamera = new Camera();
    nearbyStars = {};
    galaxy = new GalaxyGenerator();
    nearestStarSystems = {};
    ui = new UIRenderer(vulkan, width, height);

    cube3dInfo = assets.loadObject3dInfoFile("cube1unitradius.raw");
    spaceship3dInfo = assets.loadObject3dInfoFile("spaceship2d.raw");

    cameraDataBuffer = new VulkanGenericBuffer(vulkan, VK_BUFFER_USAGE_UNIFORM_BUFFER_BIT, sizeof(float) * 1024);
    starsDataBuffer = new VulkanGenericBuffer(vulkan, VK_BUFFER_USAGE_STORAGE_BUFFER_BIT, 1024 * 1024 * 128); // i want 256 mb 1024 * 1024 * 256
    planetsDataBuffer = new VulkanGenericBuffer(vulkan, VK_BUFFER_USAGE_STORAGE_BUFFER_BIT, sizeof(float) * 1024 * 1024);
    moonsDataBuffer = new VulkanGenericBuffer(vulkan, VK_BUFFER_USAGE_STORAGE_BUFFER_BIT, sizeof(float) * 1024 * 1024);
    shipDataBuffer = new VulkanGenericBuffer(vulkan, VK_BUFFER_USAGE_STORAGE_BUFFER_BIT, sizeof(float) * 1024);

    celestialImage = new VulkanImage(vulkan, width, height, VK_FORMAT_R16G16B16A16_SFLOAT, VK_IMAGE_TILING_OPTIMAL,
        VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT | VK_IMAGE_USAGE_SAMPLED_BIT, VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT, VK_IMAGE_ASPECT_COLOR_BIT, VK_IMAGE_LAYOUT_PREINITIALIZED, false);

    starsImage = new VulkanImage(vulkan, width, height, VK_FORMAT_R16G16B16A16_SFLOAT, VK_IMAGE_TILING_OPTIMAL,
        VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT | VK_IMAGE_USAGE_SAMPLED_BIT, VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT, VK_IMAGE_ASPECT_COLOR_BIT, VK_IMAGE_LAYOUT_PREINITIALIZED, false);

    shipResultImage = new VulkanImage(vulkan, width, height, VK_FORMAT_R16G16B16A16_SFLOAT, VK_IMAGE_TILING_OPTIMAL,
        VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT | VK_IMAGE_USAGE_SAMPLED_BIT, VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT, VK_IMAGE_ASPECT_COLOR_BIT, VK_IMAGE_LAYOUT_PREINITIALIZED, false);

    shipDepthImage = new VulkanImage(vulkan, width, height, VK_FORMAT_D32_SFLOAT, VK_IMAGE_TILING_OPTIMAL,
        VK_IMAGE_USAGE_DEPTH_STENCIL_ATTACHMENT_BIT, VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT, VK_IMAGE_ASPECT_DEPTH_BIT, VK_IMAGE_LAYOUT_PREINITIALIZED, true);

    // cosmosImage = new VulkanImage(vulkan, width, height, VK_FORMAT_R16G16B16A16_SFLOAT, VK_IMAGE_TILING_OPTIMAL,
   //      VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT | VK_IMAGE_USAGE_SAMPLED_BIT, VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT, VK_IMAGE_ASPECT_COLOR_BIT, VK_IMAGE_LAYOUT_PREINITIALIZED, false);

    planetTerrainHeightImage = new VulkanImage(vulkan, 1024 * 6, 1024 * 6, VK_FORMAT_R16_SFLOAT, VK_IMAGE_TILING_OPTIMAL,
        VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT | VK_IMAGE_USAGE_SAMPLED_BIT, VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT, VK_IMAGE_ASPECT_COLOR_BIT, VK_IMAGE_LAYOUT_PREINITIALIZED, false);

    planetTerrainColorImage = new VulkanImage(vulkan, 1024 * 6, 1024 * 6, VK_FORMAT_R8G8B8A8_UNORM, VK_IMAGE_TILING_OPTIMAL,
        VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT | VK_IMAGE_USAGE_SAMPLED_BIT, VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT, VK_IMAGE_ASPECT_COLOR_BIT, VK_IMAGE_LAYOUT_PREINITIALIZED, false);

    planetAtmosphereFlunctuationsImage = new VulkanImage(vulkan, 1024 * 6, 1024 * 6, VK_FORMAT_R8G8B8A8_UNORM, VK_IMAGE_TILING_OPTIMAL,
        VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT | VK_IMAGE_USAGE_SAMPLED_BIT, VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT, VK_IMAGE_ASPECT_COLOR_BIT, VK_IMAGE_LAYOUT_PREINITIALIZED, false);

    shipColorTexture = assets.loadTextureFile("spaceship2d_texture.png");

    celestialLayout = new VulkanDescriptorSetLayout(vulkan);
    celestialLayout->addField(0, VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER, VK_SHADER_STAGE_ALL_GRAPHICS);
    celestialLayout->addField(1, VK_DESCRIPTOR_TYPE_STORAGE_BUFFER, VK_SHADER_STAGE_ALL_GRAPHICS);
    celestialLayout->addField(2, VK_DESCRIPTOR_TYPE_STORAGE_BUFFER, VK_SHADER_STAGE_ALL_GRAPHICS);
    celestialLayout->addField(3, VK_DESCRIPTOR_TYPE_STORAGE_BUFFER, VK_SHADER_STAGE_ALL_GRAPHICS);
    celestialLayout->addField(4, VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER, VK_SHADER_STAGE_FRAGMENT_BIT);
    celestialLayout->addField(5, VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER, VK_SHADER_STAGE_FRAGMENT_BIT);
    celestialLayout->addField(6, VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER, VK_SHADER_STAGE_FRAGMENT_BIT);
    celestialLayout->addField(7, VK_DESCRIPTOR_TYPE_STORAGE_BUFFER, VK_SHADER_STAGE_ALL_GRAPHICS);
    celestialLayout->addField(8, VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER, VK_SHADER_STAGE_FRAGMENT_BIT);
    celestialLayout->compile();

    celestialSet = celestialLayout->generateDescriptorSet();
    celestialSet->bindUniformBuffer(0, cameraDataBuffer);
    celestialSet->bindStorageBuffer(1, starsDataBuffer);
    celestialSet->bindStorageBuffer(2, planetsDataBuffer);
    celestialSet->bindStorageBuffer(3, moonsDataBuffer);
    celestialSet->bindImageViewSampler(4, planetTerrainHeightImage);
    celestialSet->bindImageViewSampler(5, planetTerrainColorImage);
    celestialSet->bindImageViewSampler(6, planetAtmosphereFlunctuationsImage);
    celestialSet->bindStorageBuffer(7, shipDataBuffer);
    celestialSet->bindImageViewSampler(8, shipColorTexture);
    celestialSet->update();


    combineLayout = new VulkanDescriptorSetLayout(vulkan);
    combineLayout->addField(0, VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER, VK_SHADER_STAGE_FRAGMENT_BIT);
    combineLayout->addField(1, VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER, VK_SHADER_STAGE_FRAGMENT_BIT);
    combineLayout->addField(2, VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER, VK_SHADER_STAGE_FRAGMENT_BIT);
    combineLayout->addField(3, VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER, VK_SHADER_STAGE_ALL_GRAPHICS);
    combineLayout->addField(4, VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER, VK_SHADER_STAGE_FRAGMENT_BIT);
    combineLayout->compile();

    combineSet = combineLayout->generateDescriptorSet();
    combineSet->bindImageViewSampler(0, celestialImage);
    combineSet->bindImageViewSampler(1, starsImage);
    combineSet->bindImageViewSampler(2, ui->outputImage);
    combineSet->bindUniformBuffer(3, cameraDataBuffer);
    combineSet->bindImageViewSampler(4, shipResultImage);
    combineSet->update();

    recompileShaders(false);

    readyForDrawing = true;
}


CosmosRenderer::~CosmosRenderer()
{
}

#define safedelete(a) if(a!=nullptr){delete a;a=nullptr;}
void CosmosRenderer::recompileShaders(bool deleteOld)
{
    readyForDrawing = false;
    if (deleteOld) {
        vkDeviceWaitIdle(vulkan->device);
        safedelete(planetDataStage);
        safedelete(celestialStage);
        safedelete(starsStage);
        safedelete(combineStage);
        safedelete(renderer);
    }
    vkDeviceWaitIdle(vulkan->device);

    //**********************//

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

    auto planetdatavert = new VulkanShaderModule(vulkan, "../../shaders/compiled/cosmos-planetdata.vert.spv");
    auto planetdatafrag = new VulkanShaderModule(vulkan, "../../shaders/compiled/cosmos-planetdata.frag.spv");

    planetDataStage = new VulkanRenderStage(vulkan);
    planetDataStage->setViewport(1024 * 6, 1024 * 6);
    planetDataStage->addShaderStage(planetdatavert->createShaderStage(VK_SHADER_STAGE_VERTEX_BIT, "main"));
    planetDataStage->addShaderStage(planetdatafrag->createShaderStage(VK_SHADER_STAGE_FRAGMENT_BIT, "main"));
    planetDataStage->addDescriptorSetLayout(celestialLayout->layout);
    planetDataStage->addOutputImage(planetTerrainHeightImage);
    planetDataStage->addOutputImage(planetTerrainColorImage);
    planetDataStage->addOutputImage(planetAtmosphereFlunctuationsImage);
    planetDataStage->setSets({ celestialSet });

    //**********************//
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

    auto shipvert = new VulkanShaderModule(vulkan, "../../shaders/compiled/cosmos-ship.vert.spv");
    auto shipfrag = new VulkanShaderModule(vulkan, "../../shaders/compiled/cosmos-ship.frag.spv");

    shipStage = new VulkanRenderStage(vulkan);
    shipStage->setViewport(width, height);
    shipStage->addShaderStage(shipvert->createShaderStage(VK_SHADER_STAGE_VERTEX_BIT, "main"));
    shipStage->addShaderStage(shipfrag->createShaderStage(VK_SHADER_STAGE_FRAGMENT_BIT, "main"));
    shipStage->addDescriptorSetLayout(celestialLayout->layout);
    shipStage->addOutputImage(shipResultImage);
    shipStage->addOutputImage(shipDepthImage);
    shipStage->setSets({ celestialSet }); 
    shipStage->cullFlags = 0; 
    shipStage->compile();

    //**********************//

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
    renderer->addPostProcessingStage(planetDataStage);
    renderer->addPostProcessingStage(celestialStage);
    renderer->setOutputStage(combineStage);
    renderer->compile();

    readyForDrawing = true;
    planetDataStage->enabled = true;
}

GeneratedStarInfo CosmosRenderer::getStar(int starIndex)
{
    return nearbyStars[starIndex].star;
}

GeneratedPlanetInfo CosmosRenderer::getPlanet(int starIndex, int planetIndex)
{
    return nearbyStars[starIndex].planets[planetIndex];
}

GeneratedMoonInfo CosmosRenderer::getMoon(int starIndex, int planetIndex, int moonIndex)
{
    for (int i = 0; i < nearbyStars[starIndex].moons.size(); i++) {
        auto m = nearbyStars[starIndex].moons[i];
        if (m.planetIndex == planetIndex && m.moonIndex == moonIndex) return m;
    }
    return GeneratedMoonInfo();
}

void CosmosRenderer::mapBuffers()
{
    starsDataBuffer->map(0, starsDataBuffer->size, &starsDataBufferPointer);
    planetsDataBuffer->map(0, planetsDataBuffer->size, &planetsDataBufferPointer);
    moonsDataBuffer->map(0, moonsDataBuffer->size, &moonsDataBufferPointer);
}

void CosmosRenderer::unmapBuffers()
{
    starsDataBuffer->unmap();
    planetsDataBuffer->unmap();
    moonsDataBuffer->unmap();
}

void CosmosRenderer::updateStars()
{
    VulkanBinaryBufferBuilder starsBB = VulkanBinaryBufferBuilder();
    starsBB.emplaceInt32(nearbyStars.size());
    starsBB.emplaceInt32(nearbyStars.size());
    starsBB.emplaceInt32(nearbyStars.size());
    starsBB.emplaceInt32(nearbyStars.size());
    for (int s = 0; s < nearbyStars.size(); s++) {
        auto star = nearbyStars[s].star;

        glm::dvec3 starpos = star.getPosition() * scale;

        starsBB.emplaceFloat32((float)starpos.x);
        starsBB.emplaceFloat32((float)starpos.y);
        starsBB.emplaceFloat32((float)starpos.z);
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
    memcpy(starsDataBufferPointer, starsBB.getPointer(), starsBB.buffer.size());
}

void CosmosRenderer::updatePlanetsAndMoon(glm::dvec3 observerPosition)
{
    VulkanBinaryBufferBuilder planetsBB = VulkanBinaryBufferBuilder();
    VulkanBinaryBufferBuilder moonsBB = VulkanBinaryBufferBuilder();

    int planetsCount = 1;
    int moonsCount = 0;
    for (int s = 0; s < nearestStarSystems.size(); s++) {
        auto cstar = nearestStarSystems[s];

        for (int p = 0; p < cstar.planets.size(); p++) {
            auto planet = cstar.planets[p];
            if (planet.host.starIndex == nearestPlanet.host.starIndex && planet.planetIndex == nearestPlanet.planetIndex) {
                for (int m = 0; m < cstar.moons.size(); m++) {
                    moonsCount++;
                }
            }
        }
    }

    planetsBB.emplaceInt32(planetsCount);
    planetsBB.emplaceInt32(planetsCount);
    planetsBB.emplaceInt32(planetsCount);
    planetsBB.emplaceInt32(planetsCount);

    moonsBB.emplaceInt32(nearestPlanet.moonsCount);
    moonsBB.emplaceInt32(nearestPlanet.moonsCount);
    moonsBB.emplaceInt32(nearestPlanet.moonsCount);
    moonsBB.emplaceInt32(nearestPlanet.moonsCount);

    int planetIndex = 0;
    for (int s = 0; s < nearestStarSystems.size(); s++) {
        auto cstar = nearestStarSystems[s];
        glm::dvec3 starpos = cstar.star.getPosition() * scale;
        for (int p = 0; p < cstar.planets.size(); p++) {
            auto planet = cstar.planets[p];
            if (planet.host.starIndex == nearestPlanet.host.starIndex && planet.planetIndex == nearestPlanet.planetIndex) {
                glm::dvec3 planetpos = planet.getPosition(0.0) * scale;
                glm::dvec3 ppos = planetpos - observerPosition;

                planetsBB.emplaceFloat32((float)ppos.x);
                planetsBB.emplaceFloat32((float)ppos.y);
                planetsBB.emplaceFloat32((float)ppos.z);
                planetsBB.emplaceFloat32((float)planet.radius * scale);

                planetsBB.emplaceFloat32((float)planet.terrainMaxLevel);
                planetsBB.emplaceFloat32((float)planet.fluidMaxLevel);
                planetsBB.emplaceFloat32((float)planet.starDistance * scale);
                planetsBB.emplaceFloat32((float)s + p);

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

                planetsBB.emplaceInt32((int)cstar.star.starIndex);
                planetsBB.emplaceInt32((int)0);
                planetsBB.emplaceInt32((int)0);
                planetsBB.emplaceInt32((int)0);

            }
            planetIndex++;
        }
        for (int m = 0; m < cstar.moons.size(); m++) {
            auto moon = cstar.moons[m];
            if (moon.host.host.starIndex == nearestPlanet.host.starIndex && moon.host.planetIndex == nearestPlanet.planetIndex) {
                glm::dvec3 moonpos = moon.getPosition(0.0) * scale;
                glm::dvec3 mpos = moonpos - observerPosition;

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

                moonsBB.emplaceInt32((int)0);
                moonsBB.emplaceInt32((int)0);
                moonsBB.emplaceInt32((int)0);
                moonsBB.emplaceInt32((int)0);
            }

        }
    }

    memcpy(planetsDataBufferPointer, planetsBB.getPointer(), planetsBB.buffer.size());

    memcpy(moonsDataBufferPointer, moonsBB.getPointer(), moonsBB.buffer.size());
}

void CosmosRenderer::updateGravity(glm::dvec3 observerPosition)
{
    glm::vec3 newGravity = glm::vec3(0.0);
    glm::vec3 newSurfacePos = glm::vec3(0.0);
    glm::vec3 newBodyPos = glm::vec3(0.0);
    glm::vec3 newSurfaceNorm = glm::vec3(0.0);
    glm::vec3 newClosestObjectLinearAbsoluteSpeed = glm::vec3(0.0);
    double newSurfaceDist = 99999999999999.0;
    for (int s = 0; s < nearestStarSystems.size(); s++) {
        auto starss = nearestStarSystems[s];
        auto star = starss.star;
        auto planets = starss.planets;
        auto moons = starss.moons;

        glm::dvec3 starpos = star.getPosition() * scale;
        glm::dvec3 spos = starpos - observerPosition;
        glm::vec3 sdir = glm::length(spos) == 0 ? glm::vec3(0.0) : glm::normalize(spos);

        glm::dvec3 ssurfacepos = starpos - glm::dvec3(sdir) * star.radius * scale;
        double ssurfacedistance = glm::distance(observerPosition, starpos) - star.radius * scale;
        if (ssurfacedistance < newSurfaceDist) {
            newSurfaceDist = ssurfacedistance;
            newSurfacePos = ssurfacepos;
            newBodyPos = starpos;
            newSurfaceNorm = glm::normalize(starpos - ssurfacepos);
            newClosestObjectLinearAbsoluteSpeed = glm::vec3(0.0f); // stars are not moving yet
        }

        double sdistance = glm::length(spos) / scale;
        newGravity += glm::dvec3(sdir) * galaxy->calculateGravity(sdistance, galaxy->calculateMass(star.radius, 1408.0));

        for (int p = 0; p < planets.size(); p++) {
            auto planet = planets[p];
            glm::dvec3 planetpos = planet.getPosition(0.0) * scale;
            glm::dvec3 ppos = planetpos - observerPosition;
            glm::vec3 pdir = glm::length(ppos) == 0 ? glm::vec3(0.0) : glm::normalize(ppos);

            glm::dvec3 psurfacepos = planetpos - glm::dvec3(pdir) * planet.radius * scale;
            double psurfacedistance = glm::distance(observerPosition, planetpos) - planet.radius * scale;
            if (psurfacedistance < newSurfaceDist) {
                newSurfaceDist = psurfacedistance;
                newSurfacePos = psurfacepos;
                newBodyPos = planetpos;
                newSurfaceNorm = glm::normalize(planetpos - psurfacepos);
                newClosestObjectLinearAbsoluteSpeed = planet.getLinearVelocity() * scale;
            }

            double pdistance = glm::length(ppos) / scale;
            newGravity += glm::dvec3(pdir) * galaxy->calculateGravity(pdistance, galaxy->calculateMass(planet.radius));

            for (int m = 0; m < moons.size(); m++) {
                auto moon = moons[m];
                glm::dvec3 moonpos = moon.getPosition(0.0) * scale;
                glm::dvec3 mpos = moonpos - observerPosition;
                glm::vec3 mdir = glm::length(mpos) == 0 ? glm::vec3(0.0) : glm::normalize(mpos);

                glm::dvec3 msurfacepos = moonpos - glm::dvec3(mdir) * moon.radius * scale;
                double msurfacedistance = glm::distance(observerPosition, moonpos) - moon.radius * scale;
                if (msurfacedistance < newSurfaceDist) {
                    newSurfaceDist = msurfacedistance;
                    newSurfacePos = msurfacepos;
                    newBodyPos = moonpos;
                    newSurfaceNorm = glm::normalize(moonpos - msurfacepos);
                    closestObjectLinearAbsoluteSpeed = moon.getLinearVelocity() * scale;
                }

                double mdistance = glm::length(mpos) / scale;
                newGravity += glm::dvec3(mdir) * galaxy->calculateGravity(mdistance, galaxy->calculateMass(moon.radius));

            }
        }
    }
    lastGravity = newGravity;
    closestBodyPosition = newBodyPos;
    closestSurfaceDistance = newSurfaceDist;
    closestSurfacePosition = newSurfacePos;
    closestSurfaceNormal = newSurfaceNorm;
    closestObjectLinearAbsoluteSpeed = newClosestObjectLinearAbsoluteSpeed;
}

void CosmosRenderer::updateNearestStar(glm::dvec3 observerPosition)
{
    // int closestStar = 0;
    double closestDistance = 9999999999.0;
    std::vector<GeneratedStarSystemInfo> newinfos = {};
    newinfos.resize(0);
    for (int s = 0; s < nearbyStars.size(); s++) {
        auto star = nearbyStars[s];

        glm::dvec3 starpos = star.star.getPosition() * scale;
        glm::dvec3 spos = starpos - observerPosition;

        double dst = glm::length(spos);
        if (dst < star.planets[star.planets.size() - 1].starDistance * 1.1 * scale) {
            newinfos.push_back(galaxy->generateStarInfo(s));
        }
    }
    //  if (newinfos.size() > 0) {
    nearestStarSystems = newinfos;
    // }
    // nearestStarIndex = closestStar;

    closestDistance = 9999999999.0;
    GeneratedPlanetInfo np;
    for (int l = 0; l < nearestStarSystems.size(); l++) {
        auto nearestStar = nearestStarSystems[l];

        for (int s = 0; s < nearestStar.planets.size(); s++) {
            auto body = nearestStar.planets[s];

            glm::dvec3 pos = body.getPosition(0.0) * scale;
            pos = pos - observerPosition;

            float dst = glm::length(pos);
            if (dst < closestDistance) {
                closestDistance = dst;
                np = body;
            }
        }
    }
    nearestPlanet = np;

    closestDistance = 9999999999.0;
    GeneratedMoonInfo nm;
    for (int l = 0; l < nearestStarSystems.size(); l++) {
        auto nearestStar = nearestStarSystems[l];
        for (int s = 0; s < nearestStar.moons.size(); s++) {
            auto body = nearestStar.moons[s];

            glm::dvec3 pos = body.getPosition(0.0) * scale;
            pos = pos - observerPosition;

            float dst = glm::length(pos);
            if (dst < closestDistance) {
                closestDistance = dst;
                nm = body;
            }
        }
    }
    nearestMoon = nm;
}

void CosmosRenderer::updateCameraBuffer(Camera * camera, glm::dvec3 observerPosition, glm::dvec3 spaceShipPosition, glm::dquat spaceShipOrientation)
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

    glm::vec3 newcamerapos = glm::vec3(observerPosition);
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

    VulkanBinaryBufferBuilder bb2 = VulkanBinaryBufferBuilder();
    glm::mat4 shipmat = glm::mat4_cast(spaceShipOrientation);
    bb2.emplaceGeneric((unsigned char*)&shipmat, sizeof(shipmat));
    bb2.emplaceFloat32((float)(spaceShipPosition - observerPosition).x);
    bb2.emplaceFloat32((float)(spaceShipPosition - observerPosition).y);
    bb2.emplaceFloat32((float)(spaceShipPosition - observerPosition).z);
    bb2.emplaceFloat32(0.0f);
    shipDataBuffer->map(0, bb2.buffer.size(), &data);
    memcpy(data, bb2.getPointer(), bb2.buffer.size());
    shipDataBuffer->unmap();
}

void CosmosRenderer::draw()
{
    if (!readyForDrawing) return;

    if (nearestStarSystems.size() > 0) {
        if (lastStarId != nearestPlanet.host.starIndex || lastPlanetId != nearestPlanet.planetIndex) {
            lastStarId = nearestPlanet.host.starIndex;
            lastPlanetId = nearestPlanet.planetIndex;
            planetDataStage->enabled = true;
        }
    }

    ui->draw();

    starsStage->beginDrawing();

    starsStage->drawMesh(cube3dInfo, nearbyStars.size());

    starsStage->endDrawing();
    starsStage->submitNoSemaphores({});

    shipStage->beginDrawing();

    shipStage->drawMesh(spaceship3dInfo, 1);

    shipStage->endDrawing();
    shipStage->submitNoSemaphores({});


    renderer->beginDrawing();

    renderer->endDrawing();

    planetDataStage->enabled = false;
}

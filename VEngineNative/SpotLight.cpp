#include "stdafx.h"
#include "SpotLight.h"


SpotLight::SpotLight(VulkanToolkit* ivulkan, glm::vec3 icolor, TransformationManager *transmgr)
    : vulkan(ivulkan)
{
    color = icolor;
    transformation = transmgr;
    shadowMapCamera = new Camera();
    shadowMapCamera->transformation = transmgr; //  todo memory liek
    setFov(90.0f);
    id = Application::instance->getNextId();
    Application::instance->registerId(id, this);
    lightDataBuffer = new VulkanGenericBuffer(vulkan, VK_BUFFER_USAGE_UNIFORM_BUFFER_BIT, sizeof(float) * 200);
    lightDataSet = Application::instance->shadowMapDataLayout->generateDescriptorSet();
    lightDataSet->bindUniformBuffer(0, lightDataBuffer);
    lightDataSet->bindImageViewSampler(1, Application::instance->dummyTexture);
    lightDataSet->update();
} 

SpotLight::~SpotLight()
{
    delete shadowMapRenderer;
}

void SpotLight::enableShadowMapping(int width, int height)
{
    if (isShadowMappingEnabled()) {
        delete shadowMapRenderer;
    }
    shadowMapRenderer = new ShadowMapRenderer(vulkan, width, height);
    lightDataSet->bindImageViewSampler(1, shadowMapRenderer->distanceImage);
    lightDataSet->update();
    shadowMappingEnabled = true;
}

void SpotLight::disableShadowMapping()
{
    if (isShadowMappingEnabled()) {
        delete shadowMapRenderer;
        lightDataSet->bindImageViewSampler(1, Application::instance->dummyTexture);
        lightDataSet->update();
    }
    shadowMappingEnabled = false;
}

bool SpotLight::isShadowMappingEnabled()
{
    return shadowMappingEnabled;
}

void SpotLight::setFov(float ifov)
{
    fov = ifov;

    shadowMapCamera->createProjectionPerspective(fov, 1.0f, 0.01f, 10000);
}

void SpotLight::update()
{
    if (isShadowMappingEnabled()) {
        shadowMapRenderer->render(shadowMapCamera);
    }
}

void SpotLight::recordResolveCommands(VulkanRenderStage* stage, VulkanDescriptorSet* postProcessingSet)
{
    auto campos = shadowMapCamera->transformation->getPosition();

    VulkanBinaryBufferBuilder lightdata = VulkanBinaryBufferBuilder();

    glm::mat4 clip(1.0f, 0.0f, 0.0f, 0.0f,
        0.0f, -1.0f, 0.0f, 0.0f,
        0.0f, 0.0f, 0.5f, 0.0f,
        0.0f, 0.0f, 0.5f, 1.0f);
    glm::mat4 vpmatrix = clip * shadowMapCamera->projectionMatrix * shadowMapCamera->transformation->getInverseWorldTransform();
    lightdata.emplaceGeneric((unsigned char*)&vpmatrix, sizeof(vpmatrix));

    lightdata.emplaceFloat32(campos.x);
    lightdata.emplaceFloat32(campos.y);
    lightdata.emplaceFloat32(campos.z);
    lightdata.emplaceFloat32(isShadowMappingEnabled() ? 1.0f : 0.0f);

    lightdata.emplaceFloat32(color.x);
    lightdata.emplaceFloat32(color.y);
    lightdata.emplaceFloat32(color.z);
    lightdata.emplaceFloat32(0.0f);

    void* data;
    lightDataBuffer->map(0, lightdata.buffer.size(), &data);
    memcpy(data, lightdata.getPointer(), lightdata.buffer.size());
    lightDataBuffer->unmap();

    stage->drawMesh(vulkan->fullScreenQuad3dInfo, { postProcessingSet, lightDataSet }, 1);
}


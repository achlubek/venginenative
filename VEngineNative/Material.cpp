#include "stdafx.h"
#include "Material.h"
#include "Application.h"

using namespace glm;

Material::Material()
{
    diffuseColorTex = nullptr;
    normalTex = nullptr;
    bumpTex = nullptr;
    roughnessTex = nullptr;
    metalnessTex = nullptr;

    diffuseColor = vec3(1);
    roughness = 1.0;
    metalness = 0.0;

    diffuseColorTexScale = glm::vec2(1.0f);
    normalTexScale = glm::vec2(1.0f);
    bumpTexScale = glm::vec2(1.0f);
    roughnessTexScale = glm::vec2(1.0f);
    metalnessTexScale = glm::vec2(1.0f);
    materialBuffer = new VulkanGenericBuffer(VK_BUFFER_USAGE_UNIFORM_BUFFER_BIT, 2048);
     
    descriptorSet = Application::instance->materialLayout->generateDescriptorSet();
}

#define vcasti(a) (*((float*)&a))

void Material::updateBuffer() {

    if (!needsUpdate) return;
    needsUpdate = false;
    //####//
    descriptorSet.bindUniformBuffer(0, materialBuffer);
    if (diffuseColorTex != nullptr) {
        descriptorSet.bindImageViewSampler(1, diffuseColorTex);
    }
    else {
        descriptorSet.bindImageViewSampler(1, Application::instance->dummyTexture);
    }
    //####//
    if (normalTex != nullptr) {
        descriptorSet.bindImageViewSampler(2, normalTex);
    }
    else {
        descriptorSet.bindImageViewSampler(2, Application::instance->dummyTexture);
    }
    //####//
    if (bumpTex != nullptr) {
        descriptorSet.bindImageViewSampler(3, bumpTex);
    }
    else {
        descriptorSet.bindImageViewSampler(3, Application::instance->dummyTexture);
    }
    //####//
    if (roughnessTex != nullptr) {
        descriptorSet.bindImageViewSampler(4, roughnessTex);
    }
    else {
        descriptorSet.bindImageViewSampler(4, Application::instance->dummyTexture);
    }
    //####//
    if (metalnessTex != nullptr) {
        descriptorSet.bindImageViewSampler(5, metalnessTex);
    }
    else {
        descriptorSet.bindImageViewSampler(5, Application::instance->dummyTexture);
    }
    //####//
    descriptorSet.update();

    vector<float> floats2;
    floats2.reserve(32);
    floats2.push_back(roughness);
    floats2.push_back(metalness);
    floats2.push_back(0.0f);
    floats2.push_back(0.0f);

    floats2.push_back(diffuseColor.x);
    floats2.push_back(diffuseColor.y);
    floats2.push_back(diffuseColor.z);
    floats2.push_back(0.0f);

    int one = 1;
    int zero = 0;

    floats2.push_back(vcasti((diffuseColorTex != nullptr ? one : zero)));
    floats2.push_back(vcasti((normalTex != nullptr ? one : zero)));
    floats2.push_back(vcasti((bumpTex != nullptr ? one : zero)));
    floats2.push_back(vcasti((roughnessTex != nullptr ? one : zero)));

    floats2.push_back(vcasti((metalnessTex != nullptr ? one : zero)));
    floats2.push_back(0.0f);
    floats2.push_back(0.0f);
    floats2.push_back(0.0f); // amd safety 

    floats2.push_back(diffuseColorTexScale.x);
    floats2.push_back(diffuseColorTexScale.y);
    floats2.push_back(normalTexScale.x);
    floats2.push_back(normalTexScale.y);

    floats2.push_back(bumpTexScale.x);
    floats2.push_back(bumpTexScale.y);
    floats2.push_back(roughnessTexScale.x);
    floats2.push_back(roughnessTexScale.y);

    floats2.push_back(metalnessTexScale.x);
    floats2.push_back(metalnessTexScale.y);
    floats2.push_back(0.0f);
    floats2.push_back(0.0f);

    floats2.push_back(0.0f);
    floats2.push_back(0.0f);
    floats2.push_back(0.0f);
    floats2.push_back(0.0f);

    void* materialpointer = nullptr;
    materialBuffer->map(0, 4 * floats2.size(), &materialpointer);
    memcpy(materialpointer, floats2.data(), 4 * floats2.size());
    materialBuffer->unmap();
}

Material::~Material()
{

}
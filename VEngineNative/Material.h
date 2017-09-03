#pragma once 
class Material
{
public:
    Material();
    ~Material();

    glm::vec3 diffuseColor;
    float roughness;
    float metalness;
    VulkanImage* diffuseColorTex;
    VulkanImage* normalTex;
    VulkanImage* bumpTex;
    VulkanImage* roughnessTex;
    VulkanImage* metalnessTex;
    glm::vec2 diffuseColorTexScale;
    glm::vec2 normalTexScale;
    glm::vec2 bumpTexScale;
    glm::vec2 roughnessTexScale;
    glm::vec2 metalnessTexScale;
    VulkanGenericBuffer *materialBuffer;
    VulkanDescriptorSet descriptorSet;
    void updateBuffer();
    bool needsUpdate = true;
};

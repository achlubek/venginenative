#pragma once
class VulkanGraphicsPipeline;
class Object3dInfo
{
public:
    Object3dInfo(vector<float> &vbo);
    ~Object3dInfo();

    void draw(VulkanGraphicsPipeline p, VulkanDescriptorSet &set, VkCommandBuffer cb);
    void drawInstanced(VulkanGraphicsPipeline p, VulkanDescriptorSet &set, VkCommandBuffer cb, size_t instances);
    void updateRadius();
    float radius = 0.0f;
    vector<float> vbo;
    void rebufferVbo(vector<float> data, bool force_resize);

    VkBuffer vertexBuffer;
    VkDeviceMemory vertexBufferMemory;
    int vertexCount = 0;

    //VulkanImage texture;

    static VkVertexInputBindingDescription bindingDescription;
    static void getBindingDescription() {
        bindingDescription = {};
        bindingDescription.binding = 0;
        bindingDescription.stride = sizeof(float) * 12;
        bindingDescription.inputRate = VK_VERTEX_INPUT_RATE_VERTEX;
    }

    static std::array<VkVertexInputAttributeDescription, 4> attributeDescriptions;
    static void getAttributeDescriptions() {
        attributeDescriptions = {};
        attributeDescriptions[0].binding = 0;
        attributeDescriptions[0].location = 0;
        attributeDescriptions[0].format = VK_FORMAT_R32G32B32_SFLOAT;
        attributeDescriptions[0].offset = sizeof(float) * 0;

        attributeDescriptions[1].binding = 0;
        attributeDescriptions[1].location = 1;
        attributeDescriptions[1].format = VK_FORMAT_R32G32_SFLOAT;
        attributeDescriptions[1].offset = sizeof(float) * 3;

        attributeDescriptions[2].binding = 0;
        attributeDescriptions[2].location = 2;
        attributeDescriptions[2].format = VK_FORMAT_R32G32B32_SFLOAT;
        attributeDescriptions[2].offset = sizeof(float) * 5;

        attributeDescriptions[3].binding = 0;
        attributeDescriptions[3].location = 3;
        attributeDescriptions[3].format = VK_FORMAT_R32G32B32A32_SFLOAT;
        attributeDescriptions[3].offset = sizeof(float) * 8;
    }

    static VkPipelineVertexInputStateCreateInfo getVertexInputStateCreateInfo() {
        VkPipelineVertexInputStateCreateInfo vertexInputInfo = {};
        Object3dInfo::getBindingDescription();
        Object3dInfo::getAttributeDescriptions();

        vertexInputInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_VERTEX_INPUT_STATE_CREATE_INFO;
        vertexInputInfo.vertexBindingDescriptionCount = 1;
        vertexInputInfo.vertexAttributeDescriptionCount = static_cast<uint32_t>(attributeDescriptions.size());
        vertexInputInfo.pVertexBindingDescriptions = &bindingDescription;
        vertexInputInfo.pVertexAttributeDescriptions = attributeDescriptions.data();
        return vertexInputInfo;
    }

private:

    void generate();

    bool generated = false;
};

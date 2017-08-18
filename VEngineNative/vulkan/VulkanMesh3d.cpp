#include "stdafx.h" 


VkVertexInputBindingDescription VulkanMesh3d::bindingDescription = {};
std::array<VkVertexInputAttributeDescription, 4> VulkanMesh3d::attributeDescriptions = {};

VulkanMesh3d::VulkanMesh3d(std::string filxe, std::string texturefile)
{
    std::ifstream file(filxe, std::ios::ate | std::ios::binary);

    // if (!file.is_open()) {
   //      throw std::runtime_error("failed to open file!");
   //  }

    size_t fileSize = (size_t)file.tellg();
    std::vector<char> buffer(fileSize);

    file.seekg(0);
    file.read(buffer.data(), fileSize);

    file.close();
    std::vector<float> idata(buffer.size() / 4 + 1);
    memcpy(idata.data(), buffer.data(), buffer.size());

    VkBufferCreateInfo bufferInfo = {};
    bufferInfo.sType = VK_STRUCTURE_TYPE_BUFFER_CREATE_INFO;
    bufferInfo.size = sizeof(idata[0]) * idata.size();
    bufferInfo.usage = VK_BUFFER_USAGE_VERTEX_BUFFER_BIT;
    bufferInfo.sharingMode = VK_SHARING_MODE_EXCLUSIVE;

    if (vkCreateBuffer(VulkanToolkit::singleton->device, &bufferInfo, nullptr, &vertexBuffer) != VK_SUCCESS) {
        throw std::runtime_error("failed to create vertex buffer!");
    }

    VkMemoryRequirements memRequirements;
    vkGetBufferMemoryRequirements(VulkanToolkit::singleton->device, vertexBuffer, &memRequirements);

    VkMemoryAllocateInfo allocInfo = {};
    allocInfo.sType = VK_STRUCTURE_TYPE_MEMORY_ALLOCATE_INFO;
    allocInfo.allocationSize = memRequirements.size;
    allocInfo.memoryTypeIndex = VulkanToolkit::singleton->findMemoryType(memRequirements.memoryTypeBits, VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT);

    if (vkAllocateMemory(VulkanToolkit::singleton->device, &allocInfo, nullptr, &vertexBufferMemory) != VK_SUCCESS) {
        throw std::runtime_error("failed to allocate vertex buffer memory!");
    }

    vkBindBufferMemory(VulkanToolkit::singleton->device, vertexBuffer, vertexBufferMemory, 0);

    void* data;
    vkMapMemory(VulkanToolkit::singleton->device, vertexBufferMemory, 0, bufferInfo.size, 0, &data);
    memcpy(data, idata.data(), (size_t)bufferInfo.size);
    vkUnmapMemory(VulkanToolkit::singleton->device, vertexBufferMemory);
    vertexCount = idata.size() / 12;

	texture = VulkanToolkit::singleton->createTexture(texturefile);

	descriptorSet = VulkanToolkit::singleton->meshSetManager.generateMesh3dDescriptorSet();
	descriptorSet.bindUniformBuffer(0, VulkanToolkit::singleton->uniformBuffer);
	descriptorSet.bindImageViewSampler(1, texture);
	descriptorSet.update();
}

VulkanMesh3d::~VulkanMesh3d()
{
}

void VulkanMesh3d::draw(VkCommandBuffer cb)
{
    VkBuffer vertexBuffers[] = { vertexBuffer };
    VkDeviceSize offsets[] = { 0 };
    vkCmdBindVertexBuffers(cb, 0, 1, vertexBuffers, offsets);

    vkCmdDraw(cb, static_cast<uint32_t>(vertexCount), 1, 0, 0);
}
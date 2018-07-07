#include "stdafx.h"
#include "VulkanToolkit.h" 
#include "VulkanMemoryManager.h" 
#include "VulkanMemoryChunk.h" 

/*
This class expected interleaved buffer in format
pos.xyz-uv.xy-normal.xyz-tangent.xyzw
totals in 12 elements per vertex
*/

Object3dInfo::Object3dInfo(VulkanToolkit * ivulkan, vector<GLfloat> &vboin)
    : vulkan(ivulkan)
{
    
    vbo = move(vboin);
    generated = false;
    vertexCount = (GLsizei)(vbo.size() / 12);
    if (vertexCount == 0 && vbo.size() > 0) vertexCount = 1;
    updateRadius();
}

Object3dInfo::~Object3dInfo()
{
    vbo.clear();
    if (generated) {
        //
    }
}

void Object3dInfo::draw(VulkanGraphicsPipeline* p, std::vector<VulkanDescriptorSet*> sets, VkCommandBuffer cb)
{
    if (!generated) {
        generate();
    }
    std::vector<VkDescriptorSet> realsets = {};
    for (int i = 0; i < sets.size(); i++) {
        realsets.push_back(sets[i]->set);
    }
    vkCmdBindDescriptorSets(cb, VK_PIPELINE_BIND_POINT_GRAPHICS, p->pipelineLayout, 0, realsets.size(), realsets.data(), 0, nullptr);
    VkBuffer vertexBuffers[] = { vertexBuffer };
    VkDeviceSize offsets[] = { 0 };
    vkCmdBindVertexBuffers(cb, 0, 1, vertexBuffers, offsets);

    vkCmdDraw(cb, static_cast<uint32_t>(vertexCount), 1, 0, 0);
}

void Object3dInfo::drawInstanced(VulkanGraphicsPipeline* p, std::vector<VulkanDescriptorSet*> sets, VkCommandBuffer cb, size_t instances)
{
    if (!generated) {
        generate();
    }
    std::vector<VkDescriptorSet> realsets = {};
    for (int i = 0; i < sets.size(); i++) {
        realsets.push_back(sets[i]->set);
    }
    vkCmdBindDescriptorSets(cb, VK_PIPELINE_BIND_POINT_GRAPHICS, p->pipelineLayout, 0, realsets.size(), realsets.data(), 0, nullptr);
    VkBuffer vertexBuffers[] = { vertexBuffer };
    VkDeviceSize offsets[] = { 0 };
    vkCmdBindVertexBuffers(cb, 0, 1, vertexBuffers, offsets);

    vkCmdDraw(cb, static_cast<uint32_t>(vertexCount), instances, 0, 0);
}

void Object3dInfo::updateRadius()
{
    // px py pz ux uy nx ny nz tx ty tz tw | px
    // 0  1  2  3  4  5  6  7  8  9  10 11 | 12
    int size = vbo.size();
    if (vbo.size() < 2) return;
    float r = 0.0f;
    for (int i = 0; i < size; i += 12) { 
        float dst = sqrt(vbo[i] * vbo[i] + vbo[i + 1] * vbo[i + 1] + vbo[i + 2] * vbo[i + 2]);
        if (dst > r) r = dst;
    }
    radius = r;
}

void Object3dInfo::rebufferVbo(vector<GLfloat> data, bool force_resize)
{/*
    if (!generated) {
        vbo = data;
        return;
    }
    if (force_resize) {
        vbo = data;
        generated = false;
    } else {
        glBindBuffer(GL_ARRAY_BUFFER, vboHandle);
        glBufferData(GL_ARRAY_BUFFER, sizeof(GLfloat) * data.size(), data.data(), GL_STATIC_DRAW);
        glBindBuffer(GL_ARRAY_BUFFER, 0);
        glMemoryBarrier(GL_VERTEX_ATTRIB_ARRAY_BARRIER_BIT | GL_ELEMENT_ARRAY_BARRIER_BIT);
    }*/
}

void Object3dInfo::generate()
{
    VkBufferCreateInfo bufferInfo = {};
    bufferInfo.sType = VK_STRUCTURE_TYPE_BUFFER_CREATE_INFO;
    bufferInfo.size = sizeof(vbo[0]) * vbo.size();
    bufferInfo.usage = VK_BUFFER_USAGE_VERTEX_BUFFER_BIT;
    bufferInfo.sharingMode = VK_SHARING_MODE_EXCLUSIVE;

    if (vkCreateBuffer(vulkan->device, &bufferInfo, nullptr, &vertexBuffer) != VK_SUCCESS) {
        throw std::runtime_error("failed to create vertex buffer!");
    }

    VkMemoryRequirements memRequirements;
    vkGetBufferMemoryRequirements(vulkan->device, vertexBuffer, &memRequirements);

    VkMemoryAllocateInfo allocInfo = {};
    allocInfo.sType = VK_STRUCTURE_TYPE_MEMORY_ALLOCATE_INFO;
    allocInfo.allocationSize = memRequirements.size;
    allocInfo.memoryTypeIndex = vulkan->findMemoryType(memRequirements.memoryTypeBits, VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT);

    auto properties = VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT;

    vertexBufferMemory = vulkan->memoryManager->bindBufferMemory(vulkan->findMemoryType(memRequirements.memoryTypeBits, properties), vertexBuffer, memRequirements.size);

    /*
    if (vkAllocateMemory(vulkan->device, &allocInfo, nullptr, &vertexBufferMemory) != VK_SUCCESS) {
        throw std::runtime_error("failed to allocate vertex buffer memory!");
    }

    vkBindBufferMemory(vulkan->device, vertexBuffer, vertexBufferMemory, 0);*/

    void* data;
    vertexBufferMemory.map(&data);
    memcpy(data, vbo.data(), (size_t)bufferInfo.size);
    vertexBufferMemory.unmap();
    vertexCount = vbo.size() / 12;

    generated = true;
}
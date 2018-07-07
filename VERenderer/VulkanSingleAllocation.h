#pragma once
class VulkanMemoryChunk;
class VulkanSingleAllocation {
public:
    VulkanSingleAllocation();
    VulkanSingleAllocation(VulkanMemoryChunk* ichunk, uint64_t isize, uint64_t ioffset);
    ~VulkanSingleAllocation();
    void free();
    uint64_t getOffset();
    uint64_t getSize();
    void map(uint64_t offset, uint64_t size, void** data);
    void map(void** data);
    void unmap();
private:

    VulkanMemoryChunk * chunk;
    uint64_t size;
    uint64_t offset;
};
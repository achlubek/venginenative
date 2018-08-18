#pragma once

namespace VEngine
{
    namespace Renderer
    {
        namespace Internal
        {

            class VulkanMemoryChunk;
            class VulkanSingleAllocation {
            public:
                VulkanSingleAllocation();
                VulkanSingleAllocation(VulkanMemoryChunk* ichunk, uint64_t isize, uint64_t ioffset);
                ~VulkanSingleAllocation();
                void free();
                void map(uint64_t offset, uint64_t size, void** data);
                void map(void** data);
                void unmap();
                uint64_t size; // public instead of get set because of performance issues
                uint64_t offset;
            private:

                VulkanMemoryChunk * chunk;
            };
        }
    }
}
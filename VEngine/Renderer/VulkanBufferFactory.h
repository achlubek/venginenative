#pragma once
#include "../Interface/Renderer/Enums.h"
#include "../Interface/Renderer/BufferFactoryInterface.h"

namespace VEngine
{
    namespace Renderer
    {
        namespace Internal
        {
            class VulkanDevice;
        }
        class GenericBufferInterface;

        class VulkanBufferFactory : public BufferFactoryInterface
        {
        public:
            VulkanBufferFactory(Internal::VulkanDevice* device);
            ~VulkanBufferFactory();

            virtual GenericBufferInterface* build(VEngineBufferType type, uint64_t size) override;
        private:
            Internal::VulkanDevice * device;
        };

    }
}
#pragma once

namespace VEngine
{
    namespace Renderer
    {
        class DescriptorSetLayoutInterface;

        class DescriptorSetLayoutFactoryInterface
        {
        public:
            DescriptorSetLayoutInterface* build();
        };

    }
}
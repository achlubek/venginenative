#pragma once

namespace VEngine
{
    namespace Renderer
    {
        class DescriptorSetLayoutInterface;

        class DescriptorSetLayoutFactoryInterface
        {
        public:
            virtual DescriptorSetLayoutInterface* build() = 0;
        };

    }
}
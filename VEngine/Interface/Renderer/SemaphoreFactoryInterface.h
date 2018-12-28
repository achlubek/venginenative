#pragma once

namespace VEngine
{
    namespace Renderer
    {
        class SemaphoreInterface;

        class SemaphoreFactoryInterface
        {
        public:
            virtual SemaphoreInterface* build() = 0;
        };

    }
}
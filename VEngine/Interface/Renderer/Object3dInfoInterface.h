#pragma once
#include <vector>

namespace VEngine
{
    namespace Renderer
    {
        class Object3dInfoInterface
        {
        public:
            virtual std::vector<float> & getVBO() = 0;
        };

    }
}
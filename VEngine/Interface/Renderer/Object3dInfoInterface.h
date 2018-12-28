#pragma once
#include <vector>

namespace VEngine
{
    namespace Renderer
    {
        class Object3dInfoInterface
        {
        public:
            std::vector<float> & getVBO();
        };

    }
}
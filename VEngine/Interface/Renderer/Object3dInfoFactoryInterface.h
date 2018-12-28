#pragma once
#include <vector>
#include <string>

namespace VEngine
{
    namespace Renderer
    {
        class Object3dInfoInterface;

        class Object3dInfoFactoryInterface
        {
        public:
            Object3dInfoInterface* build(std::string mediakey);
            Object3dInfoInterface* build(std::vector<float> rawData);
            Object3dInfoInterface* getFullScreenQuad();
        };

    }
}
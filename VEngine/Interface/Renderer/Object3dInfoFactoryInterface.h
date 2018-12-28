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
            virtual Object3dInfoInterface* build(std::string mediakey) = 0;
            virtual Object3dInfoInterface* build(std::vector<float> rawData) = 0;
            virtual Object3dInfoInterface* getFullScreenQuad() = 0;
        };

    }
}
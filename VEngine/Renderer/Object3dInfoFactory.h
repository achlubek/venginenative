#pragma once
#include "../Interface/Renderer/Object3dInfoFactoryInterface.h"

namespace VEngine
{
    namespace FileSystem
    {
        class Media;
    }

    namespace Renderer
    {
        namespace Internal
        {
            class VulkanDevice;
        }

        class Object3dInfo;

        class Object3dInfoFactory : public Object3dInfoFactoryInterface
        {
        public:
            Object3dInfoFactory(Internal::VulkanDevice* device, FileSystem::Media* media);
            ~Object3dInfoFactory();

            virtual Object3dInfo* build(std::string mediakey) override;
            virtual Object3dInfo* build(std::vector<float> rawData) override;
            virtual Object3dInfo* getFullScreenQuad() override;
        private:
            Internal::VulkanDevice* device;
            FileSystem::Media* media;
            Object3dInfo* fullScreenQuad;
        };

    }
}
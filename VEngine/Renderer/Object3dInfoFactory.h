#pragma once
#include "../Interface/Renderer/Object3dInfoFactoryInterface.h"

namespace VEngine
{
    namespace FileSystem
    {
        class MediaInterface;
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
            Object3dInfoFactory(Internal::VulkanDevice* device, FileSystem::MediaInterface* media);
            ~Object3dInfoFactory();

            virtual Object3dInfoInterface* build(std::string mediakey) override;
            virtual Object3dInfoInterface* build(std::vector<float> rawData) override;
            virtual Object3dInfoInterface* getFullScreenQuad() override;
        private:
            Internal::VulkanDevice* device;
            FileSystem::MediaInterface* media;
            Object3dInfoInterface* fullScreenQuad;
        };

    }
}
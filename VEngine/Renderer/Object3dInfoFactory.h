#pragma once
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

        class Object3dInfoFactory
        {
        public:
            Object3dInfoFactory(Internal::VulkanDevice* device, FileSystem::Media* media);
            ~Object3dInfoFactory();

            Object3dInfo* build(std::string mediakey);
            Object3dInfo* build(std::vector<float> rawData);
            Object3dInfo* getFullScreenQuad();
        private:
            Internal::VulkanDevice* device;
            FileSystem::Media* media;
            Object3dInfo* fullScreenQuad;
        };

    }
}
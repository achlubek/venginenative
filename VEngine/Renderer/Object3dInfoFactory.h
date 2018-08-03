#pragma once
namespace VEngine
{
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
            Object3dInfoFactory(Internal::VulkanDevice* device);
            ~Object3dInfoFactory();

            Object3dInfo* build(std::string mediakey);
            Object3dInfo* build(std::vector<float> rawData);
            Object3dInfo* getFullScreenQuad();
        private:
            Internal::VulkanDevice * device;
            Object3dInfo* fullScreenQuad;
        };

    }
}
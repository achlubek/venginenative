#pragma once
#include "../Interface/Renderer/Enums.h"
#include "../Interface/Renderer/SwapChainOutputInterface.h"

namespace VEngine
{
    namespace Renderer
    {
        namespace Internal
        {
            class VulkanDevice;
        }
        class SemaphoreFactoryInterface;
        class Object3dInfoInterface;

        class VulkanSwapChainOutput : public SwapChainOutputInterface
        {
        public:
            VulkanSwapChainOutput(Internal::VulkanDevice* device, SemaphoreFactoryInterface* semaphoreFactory, RenderStageInterface* stage);
            ~VulkanSwapChainOutput();
            virtual void beginDrawing() override;
            virtual void endDrawing() override;
            virtual void drawMesh(Object3dInfoInterface *info, uint32_t instances) override;
            virtual void submit(std::vector<SemaphoreInterface*> waitSemaphores) override;
            virtual std::vector<SemaphoreInterface*> getSignalSemaphores() override;
        private:
            Internal::VulkanDevice * device;
            std::vector<RenderStageInterface*> outputStages;
            SemaphoreInterface* imageAvailableSemaphore;
            SemaphoreFactoryInterface* semaphoreFactory;
            uint32_t imageIndex;
            bool firstFrameDrawn = false;
        };

    }
}
#pragma once

namespace VEngine
{
    namespace Renderer
    {
        namespace Internal
        {
            class VulkanDevice;
            class VulkanVertexBuffer;
        }

        class Object3dInfo
        {
        public:
            Object3dInfo(Internal::VulkanDevice * device, std::vector<float> &vbo);
            ~Object3dInfo();

            Internal::VulkanVertexBuffer* getVertexBuffer();
            std::vector<float> & getVBO();

        private:
            Internal::VulkanVertexBuffer * vertexBuffer;
            std::vector<float> vbo;
        };

    }
}
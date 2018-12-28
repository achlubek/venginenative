#pragma once 
#include "Enums.h"

namespace VEngine
{
    namespace FileSystem
    {
        class MediaInterface;
    }

    namespace Renderer
    {
        class AttachmentInterface;

        class ImageInterface
        {
        public:
            // todo abstract out VkClearColorValue
            AttachmentInterface* getAttachment(VEngineAttachmentBlending blending, bool clear, VkClearColorValue clearColor, bool forPresent);
            AttachmentInterface* getAttachment(VEngineAttachmentBlending blending, bool clear, VkClearColorValue clearColor);
            AttachmentInterface* getAttachment(VEngineAttachmentBlending blending, bool clear);
            AttachmentInterface* getAttachment(VEngineAttachmentBlending blending, VkClearColorValue clearColor);
            AttachmentInterface* getAttachment(VEngineAttachmentBlending blending);
            bool isDepthBuffer();
            static bool resolveIsDepthBuffer(VEngineImageFormat format);
            void regenerateMipmaps();
        };
    }
}

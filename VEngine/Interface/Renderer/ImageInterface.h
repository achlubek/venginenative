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
            virtual AttachmentInterface* getAttachment(VEngineAttachmentBlending blending, bool clear, VEngineClearColorValue clearColor, bool forPresent) = 0;
            virtual AttachmentInterface* getAttachment(VEngineAttachmentBlending blending, bool clear, VEngineClearColorValue clearColor) = 0;
            virtual AttachmentInterface* getAttachment(VEngineAttachmentBlending blending, bool clear) = 0;
            virtual AttachmentInterface* getAttachment(VEngineAttachmentBlending blending, VEngineClearColorValue clearColor) = 0;
            virtual AttachmentInterface* getAttachment(VEngineAttachmentBlending blending) = 0;
            virtual bool isDepthBuffer() = 0;
            virtual void regenerateMipmaps() = 0;
        };
    }
}

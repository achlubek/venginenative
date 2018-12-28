#pragma once
#include "Enums.h"

namespace VEngine
{
    namespace Renderer
    {

        class ImageInterface;

        class AttachmentInterface
        {
        public:
            virtual ImageInterface * getImage() = 0;
            virtual VEngineAttachmentBlending getBlending() = 0;
            virtual bool isCleared() = 0;
        };

    }
}
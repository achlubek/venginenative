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
            ImageInterface * getImage();
            VEngineAttachmentBlending getBlending();
            bool isCleared();
        };

    }
}
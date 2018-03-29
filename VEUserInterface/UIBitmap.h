#pragma once
#include "UIAbsDrawable.h"
class UIBitmap : public UIAbsDrawable
{
public:
    UIBitmap(UIRenderer* renderer, float x, float y, float width, float height, VulkanImage* texture, UIColor color);
    ~UIBitmap();
    VulkanImage* texture{ nullptr };
	void updateBuffer();
};


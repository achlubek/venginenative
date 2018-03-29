#pragma once
#include "UIAbsDrawable.h"
class UIBox : public UIAbsDrawable
{
public:
    UIBox(UIRenderer* renderer, float x, float y, float width, float height, UIColor color);
    ~UIBox();
	void updateBuffer();
};


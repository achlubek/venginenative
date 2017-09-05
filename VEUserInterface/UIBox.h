#pragma once
#include "UIColor.h"
class UIBox
{
public:
    UIBox(float x, float y, float width, float height, UIColor color);
    ~UIBox();
    float x{ 0 }, y{ 0 }, width{ 0 }, height{ 0 };
    UIColor color{ UIColor(0.0, 0.0, 0.0, 0.0) };
    void draw();
};


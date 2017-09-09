#pragma once
#include "UIColor.h"
class UIRenderer;
class UIBox
{
public:
    UIBox(UIRenderer* renderer, float x, float y, float width, float height, UIColor color);
    ~UIBox();
    UIRenderer* renderer{ nullptr };
    VulkanDescriptorSet* set;
    VulkanGenericBuffer* dataBuffer{ nullptr };
    float x{ 0 }, y{ 0 }, width{ 0 }, height{ 0 };
    UIColor color{ UIColor(0.0, 0.0, 0.0, 0.0) };
    void updateBuffer();
};


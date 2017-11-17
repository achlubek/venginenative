#pragma once
#include "UIColor.h"
class UIRenderer;
class UIBitmap
{
public:
    UIBitmap(UIRenderer* renderer, float x, float y, float width, float height, VulkanImage* texture, UIColor color);
    ~UIBitmap();
    UIRenderer* renderer{ nullptr };
    VulkanImage* texture{ nullptr };
    VulkanDescriptorSet* set;
    VulkanGenericBuffer* dataBuffer{ nullptr };
    float x{ 0 }, y{ 0 }, width{ 0 }, height{ 0 };
    UIColor color{ UIColor(0.0, 0.0, 0.0, 0.0) };
    void updateBuffer();
    enum class HorizontalAlignment { left, center, right };
    enum class VerticalAlignment { top, center, bottom };
    HorizontalAlignment horizontalAlignment = HorizontalAlignment::left;
    VerticalAlignment verticalAlignment = VerticalAlignment::top;
};


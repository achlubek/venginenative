#pragma once
struct stbtt_fontinfo;
class UIBitmap;
class UIRenderer;
#include "UIColor.h"
class UIText
{
public:
    UIText(UIRenderer* renderer, float x, float y, UIColor color, std::string fontpath, int ifontsize, std::string text);
    ~UIText();    
    UIRenderer* renderer{ nullptr };
    VulkanImage* texture{ nullptr };
    VulkanDescriptorSet* set;
    VulkanGenericBuffer* dataBuffer{ nullptr };
    float x{ 0 }, y{ 0 }, width{ 0 }, height{ 0 };
    UIColor color{ UIColor(0.0, 0.0, 0.0, 0.0) };
    void updateBuffer(); 
    stbtt_fontinfo* font;
    unsigned char* fontBuffer;
    int fontsize = 64;
    void updateText(std::string text);
    std::string currentText = "";
    enum class HorizontalAlignment { left, center, right };
    enum class VerticalAlignment { top, center, bottom };
    HorizontalAlignment horizontalAlignment = HorizontalAlignment::left;
    VerticalAlignment verticalAlignment = VerticalAlignment::top;
};


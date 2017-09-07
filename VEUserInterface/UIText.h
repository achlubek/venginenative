#pragma once
struct stbtt_fontinfo;
class UIBitmap;
class UIRenderer;
#include "UIColor.h"
class UIText
{
public:
    UIText(UIRenderer* renderer, float x, float y, UIColor color, std::string fontpath, std::string text);
    ~UIText();    
    UIRenderer* renderer{ nullptr };
    VulkanImage* texture{ nullptr };
    VulkanDescriptorSet set;
    VulkanGenericBuffer* dataBuffer{ nullptr };
    float x{ 0 }, y{ 0 }, width{ 0 }, height{ 0 };
    UIColor color{ UIColor(0.0, 0.0, 0.0, 0.0) };
    void updateBuffer(); 
    stbtt_fontinfo* font;
    unsigned char* fontBuffer;
    void updateText(std::string text);
};


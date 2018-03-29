#pragma once
struct stbtt_fontinfo;
#include "UIAbsDrawable.h"
class UIText : public UIAbsDrawable
{
public:
    UIText(UIRenderer* renderer, float x, float y, UIColor color, std::string fontpath, int ifontsize, std::string text);
    ~UIText();
	VulkanImage* texture{ nullptr };
    stbtt_fontinfo* font;
    unsigned char* fontBuffer;
    int fontsize = 64;
    void updateText(std::string text);
    std::string currentText = "";
	void updateBuffer();
};


#include "stdafx.h"
#include "Chat.h"
#include "UIText.h"
#include "UIColor.h"
#include "Keyboard.h"
#include "UIRenderer.h"


Chat::Chat(UIRenderer* irenderer, Keyboard* ikeyboard)
    : renderer(irenderer), keyboard(ikeyboard)
{
    onSendText = EventHandler<std::string>();
    float y = 0.0;
    int fontsize = 20;
    float lineheight = (float)(fontsize + 2) / (float)renderer->height;
    y += lineheight;
    lines = {};
    linesStrings = {};
    for (int i = 0; i < 10; i++) {
        auto txt = new UIText(renderer, 0.002, y, UIColor(1, 1, 1, 1.0), Media::getPath("font.ttf"), fontsize, " ");
        lines.push_back(txt);
        renderer->texts.push_back(txt);
        linesStrings.push_back("");
        linesColors.push_back(UIColor(0,0,0,0));
        y += lineheight;
    }
    y += lineheight;
    input = new UIText(renderer, 0.002, y, UIColor(1, 1, 1, 1.0), Media::getPath("font.ttf"), fontsize, " ");
    inputString = "";
    renderer->texts.push_back(input);
    keyboard->onChar.add([&](unsigned int c) {
        if (inputEnabled) {
            if (ignoreCharsCount-- > 0) return;
            if (c == '\n') return;
            inputString += c;
            updateRenders();
        }
    });
    keyboard->onKeyPress.add([&](int c) {
        if (inputEnabled) {
            if (c == GLFW_KEY_ENTER) {
                inputEnabled = false;
                auto s = inputString;
                inputString = "";
                onSendText.invoke(s);
            }
        }
    });
}



Chat::~Chat()
{
}

void Chat::printMessage(UIColor color, std::string msg)
{
    for (int i = 0; i < 9; i++) {
        linesStrings[i] = linesStrings[i + 1];
        linesColors[i] = linesColors[i + 1];
    }
    linesStrings[linesStrings.size() - 1] = msg;
    linesColors[linesColors.size() - 1] = color;
    updateRenders();
}

void Chat::clear()
{
    for (int i = 0; i < 10; i++) {
        linesStrings[i] = "";
    }
    updateRenders();
}

void Chat::enableInput()
{
    inputEnabled = true;
    inputString = "";
    ignoreCharsCount = 1;
    updateRenders();
}

void Chat::disableInput()
{
    inputEnabled = false;
    inputString = "";
    updateRenders();
}

bool Chat::isInputEnabled()
{
    return inputEnabled;
}
 
void Chat::updateRenders()
{
    for (int i = 0; i < 10; i++) {
        lines[i]->color = linesColors[i];
        lines[i]->updateText(linesStrings[i] + " ");
    }
    input->updateText(inputString + " ");
}

#pragma once
class UIRenderer;
class Keyboard;
class UIText;
#include "UIColor.h"
#include "EventHandler.h"

class CommandTerminal
{
public:
    CommandTerminal(UIRenderer* renderer, Keyboard* keyboards);
    ~CommandTerminal();
    void printMessage(UIColor color, std::string msg);
    void clear();
    void enableInput();
    void disableInput();
    bool isInputEnabled();
    EventHandler<std::string> onSendText;

private:
    UIRenderer* renderer;
    Keyboard* keyboard;

    void updateRenders();
    int cursorPosition = 0;
    int ignoreCharsCount = 0;
    std::vector<std::string> linesStrings;
    std::vector<UIColor> linesColors;
    std::string inputString = "";
    bool inputEnabled = false;
    std::vector<UIText*> lines;
    UIText* input;
};


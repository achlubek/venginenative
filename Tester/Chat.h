#pragma once
class UIRenderer;
class Keyboard;
class UIText;

class Chat
{
public:
    Chat(UIRenderer* renderer, Keyboard* keyboards);
    ~Chat();
    void printMessage(std::string msg);
    void clear();
    void enableInput();
    void disableInput();
    bool isInputEnabled();

private:
    UIRenderer* renderer;
    Keyboard* keyboard;

    void updateRenders();
    int cursorPosition = 0;
    int ignoreCharsCount = 0;
    std::vector<std::string> linesStrings;
    std::string inputString = "";
    bool inputEnabled = false;
    std::vector<UIText*> lines;
    UIText* input;
};


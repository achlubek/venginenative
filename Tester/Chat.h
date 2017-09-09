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

    UIRenderer* renderer;
    Keyboard* keyboard;
    std::vector<UIText*> lines;
    std::vector<std::string> linesStrings;
    UIText* input;
    std::string inputString = "";

    void updateRenders();

    bool inputEnabled = false;
    int cursorPosition = 0;
};


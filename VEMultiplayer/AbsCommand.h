#pragma once
class AbsCommand
{
public:
    AbsCommand();
    ~AbsCommand();
    virtual std::vector<unsigned char> serialize() = 0; 
    virtual unsigned int getIdentificator() = 0;
    virtual void handle(unsigned int playerid, std::vector<unsigned char> data) = 0;
};


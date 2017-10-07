#pragma once
class MultiplayerClient;
#include "AbsCommand.h"
#include "AbsServerToClientCommand.h"
class SetIdCommand : public AbsServerToClientCommand
{
public:
    SetIdCommand(MultiplayerClient * client) : AbsServerToClientCommand(client) {}
    ~SetIdCommand();


    virtual std::vector<unsigned char> serialize() override;
    virtual unsigned int getIdentificator() override;
    virtual void handle(unsigned int playerid, std::vector<unsigned char> data) override;

    void prepare(unsigned int id);
private :
    unsigned int currentId = 0;
};


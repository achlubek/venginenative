#pragma once
#include "AbsCommand.h"
class MultiplayerClient;
class MultiplayerServer;

class AbsServerToClientCommand : public AbsCommand
{
public:
    AbsServerToClientCommand(MultiplayerClient* client);
    ~AbsServerToClientCommand();
protected:
    MultiplayerClient* client;
};


#pragma once
#include "AbsCommand.h"
class MultiplayerClient;
class MultiplayerServer;

class AbsClientToServerCommand : public AbsCommand
{
public:
    AbsClientToServerCommand(MultiplayerServer* server);
    ~AbsClientToServerCommand();
protected:
    MultiplayerServer* server;
};


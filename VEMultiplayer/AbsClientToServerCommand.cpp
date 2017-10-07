#include "stdafx.h"
#include "AbsClientToServerCommand.h"


AbsClientToServerCommand::AbsClientToServerCommand(MultiplayerServer* iserver)
{
    server = iserver;
}


AbsClientToServerCommand::~AbsClientToServerCommand()
{
}

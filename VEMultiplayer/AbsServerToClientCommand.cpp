#include "stdafx.h"
#include "AbsServerToClientCommand.h"


AbsServerToClientCommand::AbsServerToClientCommand(MultiplayerClient* iclient)
{
    client = iclient;
}


AbsServerToClientCommand::~AbsServerToClientCommand()
{
}

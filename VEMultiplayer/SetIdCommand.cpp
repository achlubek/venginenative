#include "stdafx.h"
#include "SetIdCommand.h"
#include "MultiplayerClient.h"



SetIdCommand::~SetIdCommand()
{
}

std::vector<unsigned char> SetIdCommand::serialize()
{
    unsigned char *data = reinterpret_cast<unsigned char*>(&currentId);
    return std::vector<unsigned char>{data[0], data[1], data[2], data[3]};
}

unsigned int SetIdCommand::getIdentificator()
{
    return 0xFFFFFF;
}

void SetIdCommand::handle(unsigned int playerid, std::vector<unsigned char> data)
{
    unsigned int *ddata = reinterpret_cast<unsigned int*>(data.data());
    client->clientId = ddata[0];
    printf("Received set id %d\n", client->clientId);
}

void SetIdCommand::prepare(unsigned int id)
{
    currentId = id;
}
 
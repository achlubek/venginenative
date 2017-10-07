#include "stdafx.h"
#include "MultiplayerHelper.h" 
#include "AbsPlayerData.h"
#include "AbsGlobalData.h"
#include "AbsPlayerFactory.h"
#include <vector>


MultiplayerHelper::MultiplayerHelper()
{
}


MultiplayerHelper::~MultiplayerHelper()
{
}

void MultiplayerHelper::appendToPacket(sf::Packet & packet, unsigned char * bytes, size_t count)
{ 
    for (int i = 0; i < count; i++) packet << bytes[i];
}

sf::Packet MultiplayerHelper::createPacketRaw(packetType type)
{
    sf::Packet dataPacket = sf::Packet();
    dataPacket << static_cast<unsigned char>(type);
    return dataPacket;
}

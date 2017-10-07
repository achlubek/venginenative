#pragma once
class AbsPlayerData;
class AbsGlobalData;
class AbsPlayerFactory;
class AbsCommand;
class AbsServerToClientCommand;
class AbsClientToServerCommand;
#include <EventHandler.h>
class MultiplayerClient
{
public:
    MultiplayerClient(unsigned int version);
    ~MultiplayerClient();
    void setGlobalData(AbsGlobalData* data);
    void setPlayerFactory(AbsPlayerFactory* factory);
    void connect(std::string ip, unsigned short port);
    std::vector<AbsPlayerData*> allPlayersData;
    AbsPlayerData* clientPlayerData;
    EventHandler<MultiplayerClient*> onSuccessfulConnect;
    EventHandler<MultiplayerClient*> onFailedConnect;
    EventHandler<MultiplayerClient*> onDisconnect;
    std::vector<AbsServerToClientCommand*> enabledCommands;
    unsigned int clientId;
    void sendPacket(sf::Packet p);
private:
    unsigned int apiVersion;
    sf::TcpSocket client;
    AbsGlobalData* globalData;
    AbsPlayerFactory* playerFactory;
    void handleAllPlayersDataPacket(const void* data, size_t datalength);
    void handleGlobalDataPacket(const void* data, size_t datalength);
    void handleCommandPacket(const void* data, size_t datalength);
    void receiveThread();
    void sendThread();
};


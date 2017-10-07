#pragma once
class AbsPlayerData;
class AbsGlobalData;
class AbsPlayerFactory;
class AbsCommand;
class AbsServerToClientCommand;
class AbsClientToServerCommand;
class SetIdCommand;
#include <EventHandler.h>

class MultiplayerServer
{
public:
    MultiplayerServer(unsigned short port, unsigned int version);
    ~MultiplayerServer();
    void setGlobalData(AbsGlobalData* data);
    void setPlayerFactory(AbsPlayerFactory* factory);
    void start();
    void sendSetIdCommandToPlayer(unsigned int id);
    void sendCommandToAll(unsigned int commandNumber, std::vector<unsigned char> bytes);
    EventHandler<AbsPlayerData*> onPlayerConnect;
    EventHandler<AbsPlayerData*> onPlayerDisconnect;
    std::vector<AbsClientToServerCommand*> enabledCommands;
    AbsPlayerData* getPlayer(unsigned int id);
private:
    SetIdCommand* setIdCommand;
    int port;
    enum class packetType { invalid, allPlayersData, singlePlayerData, globalData, command };
    AbsGlobalData* globalData;
    AbsPlayerFactory* playerFactory;
    struct SinglePlayerDescriptor {
        AbsPlayerData* data;
        sf::TcpSocket* socket;
        SinglePlayerDescriptor(AbsPlayerData* idata, sf::TcpSocket* isocket) {
            data = idata;
            socket = isocket;
        }
    };
    std::vector<SinglePlayerDescriptor> playerDataAndSockets{ {} }; //dat syntax
    sf::TcpListener listener;
    unsigned int apiVersion;
    void handleSinglePlayerPacket(unsigned int sourceId, const void* data, size_t datalength);
    void handleCommandPacket(unsigned int sourceId, const void* data, size_t datalength);
    void receiveThread();
    void sendThread();

    void broadcastPacket(sf::Packet packet);
    void sendPacketToId(sf::Packet packet, unsigned int id);
};


#pragma once
class AbsPlayerData;
class AbsGlobalData;
class AbsPlayerFactory;
#include <EventHandler.h>

class MultiplayerServer
{
public:
    MultiplayerServer(unsigned short port, unsigned int version);
    ~MultiplayerServer();
    void setGlobalData(AbsGlobalData* data);
    void setPlayerFactory(AbsPlayerFactory* factory);
    void start();
    EventHandler<AbsPlayerData*> onPlayerConnect;
    EventHandler<AbsPlayerData*> onPlayerDisconnect;
private:
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
};


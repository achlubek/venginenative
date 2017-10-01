#pragma once
class AbsPlayerData;
class AbsGlobalData;
class AbsPlayerFactory;
class MultiplayerClient
{
public:
    MultiplayerClient(unsigned int version);
    ~MultiplayerClient();
    void setGlobalData(AbsGlobalData* data);
    void setPlayerFactory(AbsPlayerFactory* factory);
    void connect(std::string ip, unsigned short port);
private:
    unsigned int apiVersion;
    sf::TcpSocket client;
    AbsGlobalData* globalData;
    AbsPlayerFactory* playerFactory;
    AbsPlayerData* clientPlayerData;
    std::vector<AbsPlayerData*> allPlayersData;
    void handleAllPlayersDataPacket(const void* data, size_t datalength);
    void handleGlobalDataPacket(const void* data, size_t datalength);
    void handleCommandPacket(const void* data, size_t datalength);
    enum class packetType { invalid, allPlayersData, singlePlayerData, globalData, command };
    void receiveThread();
    void sendThread();
};


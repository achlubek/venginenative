#pragma once
class MultiplayerClient
{
public:
    MultiplayerClient(std::string ip, unsigned short port, unsigned int version);
    ~MultiplayerClient();
private:
    unsigned int apiVersion;
    sf::TcpSocket client;
};


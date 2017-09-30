#include "stdafx.h"
#include "MultiplayerServer.h"
#include "AbsPlayerData.h"
#include "AbsGlobalData.h"
#include "AbsPlayerFactory.h"
#include <vector>


MultiplayerServer::MultiplayerServer(unsigned short iport, unsigned int version)
{
    apiVersion = version;
    port = iport;
    onPlayerConnect = EventHandler<AbsPlayerData*>();
    onPlayerDisconnect = EventHandler<AbsPlayerData*>();

}


MultiplayerServer::~MultiplayerServer()
{
}

void MultiplayerServer::setGlobalData(AbsGlobalData * data)
{
    globalData = data;
}

void MultiplayerServer::setPlayerFactory(AbsPlayerFactory * factory)
{
    playerFactory = factory;
}

void MultiplayerServer::start()
{

    if (listener.listen(port) != sf::Socket::Done)
    {
        printf("Couldn't bind the socket\n");
    }
    std::thread t = std::thread([&]() {
        while (true) {
            sf::TcpSocket* client = new sf::TcpSocket();
            printf("Waiting for any incoming connection\n");
            if (listener.accept(*client) != sf::Socket::Done)
            {
                printf("Couldn't accept incoming connection\n");
            }
            else {
                // send handshake and receive handshake

                printf("Verifying client connection\n");
                unsigned int handshake[2] = { 0xFF00FF00, apiVersion };

                if (client->send(handshake, 2 * sizeof(unsigned int)) != sf::Socket::Done)
                {
                    printf("Couldn't send handshake\n");
                }
                else {
                    printf("Sent handshake\n");
                    unsigned int handshakeResponse[2] = { 0, 0 };
                    std::size_t received;
                    printf("reseived %d %d", handshakeResponse[0], handshakeResponse[1]);
                    auto status = client->receive(handshakeResponse, 2 * sizeof(unsigned int), received);
                    printf("reseived %d %d", handshakeResponse[0], handshakeResponse[1]);
                    if (status != sf::Socket::Done)
                    {
                        printf("Couldn't receive handshake response from client\n");
                    }
                    else if (received != 2 * sizeof(unsigned int)) {
                        printf("Handshake response size is invalid\n");
                    }
                    else if (handshakeResponse[0] != 0xFF00FF00) {
                        printf("Handshake response is invalid\n");
                    }
                    else if (handshakeResponse[1] != apiVersion) {
                        printf("Handshake api version of client is invalid\n");
                    }
                    else {
                        // ........ ok
                        // absolutely too deep here
                        auto ipaddr = client->getRemoteAddress();
                        auto ipaddrstr = ipaddr.toString();
                        printf("Client connected from %s\n", ipaddrstr);
                        AbsPlayerData* data = playerFactory->createPlayer();
                        auto tuple = std::make_tuple(data, client);
                        playerDataAndSockets.push_back(SinglePlayerDescriptor(data, client));
                        onPlayerConnect.invoke(data);
                    }
                }

            }
        }
    });
    t.detach();
    std::thread t2 = std::thread([&]() {
        while (true) {
            sf::Packet globalDataPacket = sf::Packet();
            globalDataPacket << static_cast<unsigned char>(packetType::globalData);
            auto globdata = globalData->serialize();
            for (int i = 0; i < globdata.size(); i++) globalDataPacket << globdata[i];
            // broadcast to everyone
            for (int i = 0; i < playerDataAndSockets.size(); i++) playerDataAndSockets[i].socket->send(globalDataPacket);

            sf::Packet allPlayersDataPacket = sf::Packet();
            allPlayersDataPacket << static_cast<unsigned char>(packetType::allPlayersData);
            allPlayersDataPacket << static_cast<unsigned char>(playerDataAndSockets.size());
            for (int i = 0; i < playerDataAndSockets.size(); i++) {
                auto playerdata = playerDataAndSockets[i].data->serialize();
                for (int i = 0; i < playerdata.size(); i++) allPlayersDataPacket << playerdata[i];
            }
            for (int i = 0; i < playerDataAndSockets.size(); i++) playerDataAndSockets[i].socket->send(allPlayersDataPacket);
            printf("Broadcasted %d players\n", playerDataAndSockets.size());
        }
        // i have bad feelings about this (especially about fragmentation)
    });
    t2.detach();
}
 

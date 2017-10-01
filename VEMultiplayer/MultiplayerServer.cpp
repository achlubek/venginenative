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
        receiveThread();
    });
    t2.detach();
    std::thread t3 = std::thread([&]() {
        sendThread();
    });
    t3.detach();
}

void MultiplayerServer::handleSinglePlayerPacket(unsigned int sourceId, const void * data, size_t datalength)
{
    size_t offset = 1;
    auto ucharpointer = static_cast<const unsigned char*>(data);
    size_t singlePlayerLength = playerFactory->getPlayerDataLength();
    for (int i = 0; i < playerDataAndSockets.size(); i++) {
        if (sourceId == playerDataAndSockets[i].data->id) {
            std::vector<unsigned char> bytes = {};
            bytes.resize(singlePlayerLength);
            memcpy(bytes.data(), ucharpointer + offset, singlePlayerLength);
            unsigned int id = (reinterpret_cast<unsigned int*>(bytes.data()))[0];
            playerDataAndSockets[i].data->deserialize(bytes);
            break;
        }
    }
}

void MultiplayerServer::handleCommandPacket(unsigned int sourceId, const void * data, size_t datalength)
{
}

void MultiplayerServer::receiveThread()
{
    while (true) {
        for (int i = 0; i < playerDataAndSockets.size(); i++) {
            auto client = playerDataAndSockets[i].socket;
            sf::Packet packet;
            std::size_t received;
            sf::SocketSelector selector;
            sf::Time timeout = sf::Time(sf::milliseconds(100));
            selector.add(*client);
            if (selector.wait(timeout)) {
                if (client->receive(packet) != sf::Socket::Done)
                {
                    printf("Couldn't receive packet from client\n");
                }
                else {
                    printf("Received packet from client\n");
                    const void* data = packet.getData();
                    const unsigned char* bytes = static_cast<const unsigned char*>(data);
                    size_t size = packet.getDataSize();
                    if (size > 0) {
                        // got a packet with some length
                        packetType type = static_cast<packetType>(bytes[0]);
                        switch (type) {
                        case packetType::singlePlayerData:
                            handleSinglePlayerPacket(playerDataAndSockets[i].data->id, data, size);
                            break;
                        case packetType::command:
                            handleCommandPacket(playerDataAndSockets[i].data->id, data, size);
                            break;
                        }
                    }
                    // delete data;
                }
            }
        }
    }
}

void MultiplayerServer::sendThread()
{
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
        mySleep(100);
    }
}
 

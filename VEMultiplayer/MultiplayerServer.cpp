#include "stdafx.h"
#include "MultiplayerServer.h"
#include "AbsPlayerData.h"
#include "AbsGlobalData.h"
#include "AbsPlayerFactory.h"
#include "AbsCommand.h"
#include "AbsClientToServerCommand.h"
#include "AbsServerToClientCommand.h"
#include "MultiplayerHelper.h"
#include "SetIdCommand.h"
#include <vector>


MultiplayerServer::MultiplayerServer(unsigned short iport, unsigned int version)
{
    apiVersion = version;
    port = iport;
    onPlayerConnect = EventHandler<AbsPlayerData*>();
    onPlayerDisconnect = EventHandler<AbsPlayerData*>();
    enabledCommands = {};
    setIdCommand = new SetIdCommand(nullptr);
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
                        sendSetIdCommandToPlayer(data->id);
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

void MultiplayerServer::sendSetIdCommandToPlayer(unsigned int id)
{
    sf::Packet dataPacket = MultiplayerHelper::createPacketRaw(MultiplayerHelper::packetType::command);
    MultiplayerHelper::appendToPacket(dataPacket, setIdCommand->getIdentificator());
    setIdCommand->prepare(id);
    MultiplayerHelper::appendToPacket(dataPacket, setIdCommand->serialize());
    sendPacketToId(dataPacket, id);
}

void MultiplayerServer::sendCommandToAll(unsigned int commandNumber, std::vector<unsigned char> bytes)
{
    sf::Packet dataPacket = MultiplayerHelper::createPacketRaw(MultiplayerHelper::packetType::command);
    MultiplayerHelper::appendToPacket(dataPacket, commandNumber);
    MultiplayerHelper::appendToPacket(dataPacket, bytes);
    broadcastPacket(dataPacket);
}

AbsPlayerData * MultiplayerServer::getPlayer(unsigned int id)
{
    for (int i = 0; i < playerDataAndSockets.size(); i++) {
        if (id == playerDataAndSockets[i].data->id) {
            return playerDataAndSockets[i].data;
        }
    }
}

void MultiplayerServer::handleSinglePlayerPacket(unsigned int sourceId, const void * data, size_t datalength)
{
    size_t offset = 1;
    auto ucharpointer = static_cast<const unsigned char*>(data);
    for (int i = 0; i < playerDataAndSockets.size(); i++) {
        if (sourceId == playerDataAndSockets[i].data->id) {
            std::vector<unsigned char> bytes = {};
            bytes.resize(datalength - offset);
            memcpy(bytes.data(), ucharpointer + offset, datalength - offset);
            unsigned int id = (reinterpret_cast<unsigned int*>(bytes.data()))[0];
            playerDataAndSockets[i].data->deserialize(bytes);
            break;
        }
    }
}

void MultiplayerServer::handleCommandPacket(unsigned int sourceId, const void * data, size_t datalength)
{
    size_t offset = 1 + sizeof(unsigned int);
    std::vector<unsigned char> bytes = {};
    bytes.resize(datalength);
    auto ucharpointer = static_cast<const unsigned char*>(data);
    unsigned int commandNumber = reinterpret_cast<const unsigned int*>((static_cast<const unsigned char*>(data) + 1))[0];
    memcpy(bytes.data(), ucharpointer + offset, datalength - offset);
    for (int i = 0; i < enabledCommands.size(); i++) {
        if (enabledCommands[i]->getIdentificator() == commandNumber) {
            enabledCommands[i]->handle(sourceId, bytes);
        }
    }
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
                    playerDataAndSockets.erase(playerDataAndSockets.begin() + i);
                    i--;
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
        sf::Packet globalDataPacket = MultiplayerHelper::createPacketRaw(MultiplayerHelper::packetType::globalData);
        MultiplayerHelper::appendToPacket(globalDataPacket, globalData->serialize());
        broadcastPacket(globalDataPacket);

        sf::Packet allPlayersDataPacket = MultiplayerHelper::createPacketRaw(MultiplayerHelper::packetType::allPlayersData);
        MultiplayerHelper::appendToPacket(allPlayersDataPacket, static_cast<unsigned char>(playerDataAndSockets.size()));
        for (int i = 0; i < playerDataAndSockets.size(); i++) {
            auto pdata = playerDataAndSockets[i].data->serialize();
            MultiplayerHelper::appendToPacket(allPlayersDataPacket, (unsigned int)pdata.size());
            MultiplayerHelper::appendToPacket(allPlayersDataPacket, pdata);
        }
        broadcastPacket(allPlayersDataPacket);
        // printf("Broadcasted %d players\n", playerDataAndSockets.size());
        mySleep(100);
    }
}

void MultiplayerServer::broadcastPacket(sf::Packet packet)
{
    for (int i = 0; i < playerDataAndSockets.size(); i++) {
        playerDataAndSockets[i].socket->send(packet);
    }
}

void MultiplayerServer::sendPacketToId(sf::Packet packet, unsigned int id)
{
    for (int i = 0; i < playerDataAndSockets.size(); i++) {
        if (playerDataAndSockets[i].data->id == id) {
            playerDataAndSockets[i].socket->send(packet);
        }
    }
}


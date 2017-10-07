#include "stdafx.h"
#include "MultiplayerClient.h"
#include "AbsPlayerData.h"
#include "AbsGlobalData.h"
#include "AbsPlayerFactory.h"
#include "MultiplayerHelper.h"
#include "AbsCommand.h"
#include "AbsClientToServerCommand.h"
#include "AbsServerToClientCommand.h"
#include "SetIdCommand.h"


MultiplayerClient::MultiplayerClient(unsigned int version)
{
    apiVersion = version;
    auto setidcmd = new SetIdCommand(this);
    enabledCommands = { setidcmd };
}


MultiplayerClient::~MultiplayerClient()
{
}

void MultiplayerClient::sendPacket(sf::Packet p)
{
    client.send(p);
}

void MultiplayerClient::handleAllPlayersDataPacket(const void * data, size_t datalength)
{
    size_t offset = 2;
    auto ucharpointer = static_cast<const unsigned char*>(data);
    unsigned char playersCount = ucharpointer[1];
    while (allPlayersData.size() < playersCount) {// adjust array sizes and initialize by the way
        allPlayersData.push_back(playerFactory->createPlayer());
    }
    while (allPlayersData.size() > playersCount) {// adjust array sizes and delete
        allPlayersData.pop_back(); // deletes automatically spec says, this is all important because this deconstruct is responsible for all freeing in custom class
    }
    for (unsigned char i = 0; i < playersCount; i++) {
        auto uintpointer = reinterpret_cast<const unsigned int*>(ucharpointer + offset);
        std::vector<unsigned char> bytes = {};
        bytes.resize(uintpointer[0]);
        offset += sizeof(unsigned int);
        memcpy(bytes.data(), ucharpointer + offset, uintpointer[0]);
        unsigned int id = (reinterpret_cast<unsigned int*>(bytes.data()))[0];
        allPlayersData[i]->deserialize(bytes);
        offset += uintpointer[0]; // i find it crazy as fuck but it should work
    }
}

void MultiplayerClient::handleGlobalDataPacket(const void * data, size_t datalength)
{
    size_t offset = 1;
    std::vector<unsigned char> bytes = {};
    bytes.resize(datalength - 1);
    auto ucharpointer = static_cast<const unsigned char*>(data);
    memcpy(bytes.data(), ucharpointer + offset, datalength - 1);
    globalData->deserialize(bytes);
}

void MultiplayerClient::handleCommandPacket(const void * data, size_t datalength)
{
    size_t offset = 1 + sizeof(unsigned int);
    std::vector<unsigned char> bytes = {};
    bytes.resize(datalength);
    auto ucharpointer = static_cast<const unsigned char*>(data);
    unsigned int commandNumber = reinterpret_cast<const unsigned int*>((static_cast<const unsigned char*>(data) + 1))[0];
    memcpy(bytes.data(), ucharpointer + offset, datalength - offset);
    for (int i = 0; i < enabledCommands.size(); i++) {
        if (enabledCommands[i]->getIdentificator() == commandNumber) {
            enabledCommands[i]->handle(clientId, bytes);
        }
    }/*
    size_t offset = 1 + sizeof(unsigned int);
    unsigned int commandNumber = reinterpret_cast<const unsigned int*>((static_cast<const unsigned char*>(data) + 1))[0];

    // casting marathon god damn it
    if (commandNumber == static_cast<unsigned int>(MultiplayerHelper::builtInCommands::setId)) {
        unsigned int id = reinterpret_cast<const unsigned int*>((static_cast<const unsigned char*>(data) + 1))[1];
        printf("Received command SET ID %d\n", id);
        clientId = id;
    }*/
}

void MultiplayerClient::receiveThread()
{
    while (true) {
        sf::Packet packet;
        std::size_t received;
        sf::SocketSelector selector;
        sf::Time timeout = sf::Time(sf::seconds(1));
        selector.add(client);
        if (selector.wait(timeout)) {
            if (client.receive(packet) != sf::Socket::Done)
            {
                printf("Couldn't receive packet from server\n");
                onDisconnect.invoke(this);
                return;
            }
            else {
              //  printf("Received packet from server\n");
                const void* data = packet.getData();
                const unsigned char* bytes = static_cast<const unsigned char*>(data);
                size_t size = packet.getDataSize();
                if (size > 0) {
                    // got a packet with some length
                    auto type = static_cast<MultiplayerHelper::packetType>(bytes[0]);
                    switch (type) {
                    case MultiplayerHelper::packetType::allPlayersData:
                        handleAllPlayersDataPacket(data, size);
                        break;
                    case MultiplayerHelper::packetType::globalData:
                        handleGlobalDataPacket(data, size);
                        break;
                    case MultiplayerHelper::packetType::command:
                        handleCommandPacket(data, size);
                        break;
                    }
                }
                // delete data;
            }
        }
    }
}

void MultiplayerClient::sendThread()
{
    while (true) {
        sf::Packet playerDataPacket = MultiplayerHelper::createPacketRaw(MultiplayerHelper::packetType::singlePlayerData);
        MultiplayerHelper::appendToPacket(playerDataPacket, clientPlayerData->serialize());
        
        client.send(playerDataPacket);
       // printf("Broadcasted client data\n");
        mySleep(100);
    }
}

void MultiplayerClient::setGlobalData(AbsGlobalData * data)
{
    globalData = data;
}

void MultiplayerClient::setPlayerFactory(AbsPlayerFactory * factory)
{
    playerFactory = factory;
}

void MultiplayerClient::connect(std::string ip, unsigned short port)
{

    sf::Socket::Status status = client.connect(ip.c_str(), port);
    if (status != sf::Socket::Done)
    {
        printf("Couldn't connect to server\n");
        onFailedConnect.invoke(this);
    }
    else {
        unsigned int handshakeResponse[2] = { 0, 0 };
        std::size_t received;

        if (client.receive(handshakeResponse, 2 * sizeof(unsigned int), received) != sf::Socket::Done)
        {
            printf("Couldn't receive handshake response from server\n");
            onFailedConnect.invoke(this);
        }
        else if (received != 2 * sizeof(unsigned int)) {
            printf("Handshake response size is invalid\n");
            onFailedConnect.invoke(this);
        }
        else if (handshakeResponse[0] != 0xFF00FF00) {
            printf("Handshake response is invalid\n");
            onFailedConnect.invoke(this);
        }
        else if (handshakeResponse[1] != apiVersion) {
            printf("Handshake api version of server is invalid\n");
            onFailedConnect.invoke(this);
        }
        else {

            unsigned int handshake[2] = { 0xFF00FF00, apiVersion };

            if (client.send(handshake, 2 * sizeof(unsigned int)) != sf::Socket::Done)
            {
                printf("Couldn't send handshake\n");
                onFailedConnect.invoke(this);
            }
            else {
                clientPlayerData = playerFactory->createPlayer();
                std::thread t2 = std::thread([&]() {
                    receiveThread();
                });
                t2.detach();
                std::thread t3 = std::thread([&]() {
                    sendThread();
                });
                t3.detach();
                onSuccessfulConnect.invoke(this);
            }
        }
    }
}

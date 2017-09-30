#include "stdafx.h"
#include "MultiplayerClient.h"
#include "AbsPlayerData.h"
#include "AbsGlobalData.h"
#include "AbsPlayerFactory.h"

MultiplayerClient::MultiplayerClient(unsigned int version)
{
    apiVersion = version;
}


MultiplayerClient::~MultiplayerClient()
{
}

void MultiplayerClient::handleAllPlayersDataPacket(const void * data, size_t datalength)
{
    size_t offset = 2;
    auto ucharpointer = static_cast<const unsigned char*>(data);
    unsigned char playersCount = ucharpointer[1];
    size_t singlePlayerLength = playerFactory->getPlayerDataLength();
    while (allPlayersData.size() < playersCount) {// adjust array sizes and initialize by the way
        allPlayersData.push_back(playerFactory->createPlayer());
    }
    while (allPlayersData.size() > playersCount) {// adjust array sizes and delete
        allPlayersData.pop_back(); // deletes automatically spec says, this is all important because this deconstruct is responsible for all freeing in custom class
    }
    for (unsigned char i = 0; i < playersCount; i++) {
        std::vector<unsigned char> bytes = {};
        bytes.resize(singlePlayerLength);
        memcpy(bytes.data(), ucharpointer + offset, singlePlayerLength);
        unsigned int id = ((unsigned int*)(bytes.data()))[0]; // static cast doesnt trust me here
        allPlayersData[i]->deserialize(bytes);
        offset += singlePlayerLength; // i find it crazy as fuck but it should work
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
    // not implemented yet but it will involve a handler and parsing like globalData
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
    }
    else {
        unsigned int handshakeResponse[2] = { 0, 0 };
        std::size_t received;

        if (client.receive(handshakeResponse, 2 * sizeof(unsigned int), received) != sf::Socket::Done)
        {
            printf("Couldn't receive handshake response from server\n");
        }
        else if (received != 2 * sizeof(unsigned int)) {
            printf("Handshake response size is invalid\n");
        }
        else if (handshakeResponse[0] != 0xFF00FF00) {
            printf("Handshake response is invalid\n");
        }
        else if (handshakeResponse[1] != apiVersion) {
            printf("Handshake api version of server is invalid\n");
        }
        else {

            unsigned int handshake[2] = { 0xFF00FF00, apiVersion };

            if (client.send(handshake, 2 * sizeof(unsigned int)) != sf::Socket::Done)
            {
                printf("Couldn't send handshake\n");
            }
            else {
                std::thread t2 = std::thread([&]() {

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
                            }
                            else {
                                printf("Received packet from server\n");
                                const void* data = packet.getData();
                                const unsigned char* bytes = static_cast<const unsigned char*>(data);
                                size_t size = packet.getDataSize();
                                if (size > 0) {
                                    // got a packet with some length
                                    packetType type = static_cast<packetType>(bytes[0]);
                                    switch (type) {
                                    case packetType::allPlayersData:
                                        handleAllPlayersDataPacket(data, size);
                                        break;
                                    case packetType::globalData:
                                        handleGlobalDataPacket(data, size);
                                        break;
                                    case packetType::command:
                                        handleCommandPacket(data, size);
                                        break;
                                    }
                                }
                               // delete data;
                            }
                        }
                    }
                });
                t2.detach();
            }
        }
    }
}

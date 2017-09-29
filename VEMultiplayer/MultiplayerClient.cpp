#include "stdafx.h"
#include "MultiplayerClient.h"


MultiplayerClient::MultiplayerClient(std::string ip, unsigned short port, unsigned int version)
{
    apiVersion = version;
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
        }
    }
}


MultiplayerClient::~MultiplayerClient()
{
}

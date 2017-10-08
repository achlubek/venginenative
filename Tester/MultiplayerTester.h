#pragma once


#include "SFML\Network.hpp"
#include "MultiplayerClient.h"
#include "MultiplayerServer.h"
#include "MultiplayerHelper.h"
#include "AbsPlayerData.h"
#include "AbsPlayerFactory.h"
#include "AbsGlobalData.h"
#include "AbsCommand.h"
#include "AbsClientToServerCommand.h"
#include "AbsServerToClientCommand.h"
#include "VEInput.h"


class TestPlayerData : public AbsPlayerData {
public:
    volatile glm::vec2 testposition = glm::vec2(0.5, 0.5);
    std::string nick = "";
    TestPlayerData(unsigned int i) : AbsPlayerData(i) {

    }
    virtual std::vector<unsigned char> serialize() override
    {
        auto bytes = std::vector<unsigned char>();
        bytes.resize(sizeof(float) * 2);
        memcpy(bytes.data(), const_cast<float*>(&testposition.x), sizeof(float) * 2);
        //  printf("serializing %f\n", testposition.x);
        return bytes;
    }
    virtual void deserialize(std::vector<unsigned char> data) override
    {
        float* ptr = reinterpret_cast<float*>(data.data());
        testposition.x = ptr[0];
        testposition.y = ptr[1];
        //  printf("Deseializing %f\n", testposition.x);
    }
};
class TestPlayerFactory : public AbsPlayerFactory {

    virtual AbsPlayerData * createPlayer() override
    {
        return new TestPlayerData(nextId());
    }

};
class TestGlobalData : public AbsGlobalData {
public:
    //  volatile glm::vec2 testposition = glm::vec2(0.5, 0.5);

    virtual std::vector<unsigned char> serialize() override
    {
        auto bytes = std::vector<unsigned char>();
        // bytes.resize(sizeof(float) * 2);
        // memcpy(bytes.data(), const_cast<float*>(&testposition.x), sizeof(float) * 2);
        //  printf("serializing %f\n", testposition.x);
        return bytes;
    }
    virtual void deserialize(std::vector<unsigned char> data) override
    {
        //float* ptr = reinterpret_cast<float*>(data.data());
        // testposition.x = ptr[0];
        // testposition.y = ptr[1];
        // printf("Deseializing %f\n", testposition.x);
    }
};

class ChangeNicknameCommand : public AbsClientToServerCommand {
public:
    ChangeNicknameCommand(MultiplayerServer * server) : AbsClientToServerCommand(server) {}
    std::string nick = "";
    void prepare(std::string n) {
        nick = n;
    }

    virtual std::vector<unsigned char> serialize() override
    {
        auto buf = std::vector<unsigned char>();
        buf.resize(nick.length());
        for (int i = 0; i < nick.length(); i++) {
            buf[i] = nick[i];
        }
        return buf;
    }

    virtual unsigned int getIdentificator() override
    {
        return 12; //whatever
    }

    virtual void handle(unsigned int playerid, std::vector<unsigned char> data) override
    {
        auto player = static_cast<TestPlayerData*>(server->getPlayer(playerid));
        player->nick = std::string(reinterpret_cast<const char *>(data.data()), data.size());
        printf("REXCEIVED NET NICK %s", player->nick.c_str());
    }
};
class MultiplayerTester
{
public:
    MultiplayerTester();
    ~MultiplayerTester();
};


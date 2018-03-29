#include "stdafx.h"   

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
        for (int i = 0; i < nick.length(); i++)bytes.push_back(nick[i]);
      //  printf("serializing %f\n", testposition.x);
        return bytes;
    }
    virtual void deserialize(std::vector<unsigned char> data) override
    {
        float* ptr = reinterpret_cast<float*>(data.data());
        auto ptr2 = reinterpret_cast<unsigned char*>(data.data());
        testposition.x = ptr[0];
        testposition.y = ptr[1];
        std::string n = "";
        for (int i = 2 * sizeof(float); i < data.size(); i++)n = n + string(1, ptr2[i]);
        nick = n;
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

int main(int argc, char* argv[])
{
    Media::loadFileMap("../../media");
    Media::loadFileMap("../../shaders");


    // just testing mp
    if (argc > 1 && strcmp(argv[1], "server") == 0) {
        MultiplayerServer* server = new MultiplayerServer(8111, 123);
        server->setPlayerFactory(new TestPlayerFactory());
        ChangeNicknameCommand* nickcmd = new ChangeNicknameCommand(server);
        server->enabledCommands.push_back(nickcmd);
        auto gdata = new TestGlobalData();
        server->setGlobalData(gdata);
        server->start();
        double iter = 0.0;

        // gdata->testposition.x = 0.0f;
        while (true) {
            //  gdata->testposition.x = sin(iter) * 0.5 + 0.5;
            //  std::this_thread::sleep_for(std::chrono::milliseconds(100));
           //   iter += 0.01;
        }
        return 0;
    }


    MultiplayerClient* client = new MultiplayerClient(123);
    ChangeNicknameCommand* nickcmd = new ChangeNicknameCommand(nullptr);
    client->setPlayerFactory(new TestPlayerFactory());
    auto clientgdata = new TestGlobalData();
    client->setGlobalData(clientgdata);
    client->onSuccessfulConnect.add([&](MultiplayerClient* client) {
        auto player = static_cast<TestPlayerData*>(client->clientPlayerData);
        player->nick = argv[1];
        sf::Packet dataPacket = MultiplayerHelper::createPacketRaw(MultiplayerHelper::packetType::command);
        MultiplayerHelper::appendToPacket(dataPacket, nickcmd->getIdentificator());
        nickcmd->prepare(player->nick);
        MultiplayerHelper::appendToPacket(dataPacket, nickcmd->serialize());
        client->sendPacket(dataPacket);
    });
    client->connect("192.168.0.105", 8111);

    Application *game = new Application(800, 600);

    for (int i = 0; i < 20; i++) {
        // out of screen by default
        auto img1 = new UIText(game->ui, 0.2, 0.2, UIColor(1, 0, 0, 0.5), Media::getPath("font.ttf"), 26, " ");
       // game->ui->texts.push_back(img1);
    }

    auto mouse = new Mouse(game->vulkan->window);

    game->onRenderFrame.add([&](int zero) {
        for (int i = 0; i < client->allPlayersData.size(); i++) {
            auto p = static_cast<TestPlayerData*>(client->allPlayersData[i]);
            if (client->allPlayersData[i]->id == client->clientPlayerData->id) continue;
           // game->ui->texts[i]->x = p->testposition.x;
         //   game->ui->texts[i]->y = p->testposition.y;
         //   game->ui->texts[i]->updateText(p->nick);
        }
        for (int i = client->allPlayersData.size(); i < 20; i++) {
          //  game->ui->texts[i]->x = -1.0f;
           // game->ui->texts[i]->y = -1.0f;
        }
        auto p = static_cast<TestPlayerData*>(client->clientPlayerData);
        auto cursor = mouse->getCursorPosition();
        p->testposition.x = std::get<0>(cursor) / 800.0f;
        p->testposition.y = std::get<1>(cursor) / 600.0f;
    });
    game->start();


    return 0;
}


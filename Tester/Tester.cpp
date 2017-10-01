// Tester.cpp : Defines the entry point for the console application.
//

#include "stdafx.h"

#include "windows.h"
 
#include "Application.h"
#include "Media.h"
#include "EditorApp.h"

#include "SFML\Network.hpp"
#include "MultiplayerClient.h"
#include "MultiplayerServer.h"
#include "AbsPlayerData.h"
#include "AbsPlayerFactory.h"
#include "AbsGlobalData.h"

class TestPlayerData : public AbsPlayerData {
public:
    TestPlayerData(unsigned int i) : AbsPlayerData(i) {

    }
    virtual std::vector<unsigned char> serialize() override
    {
        return std::vector<unsigned char>();
    }
    virtual void deserialize(std::vector<unsigned char> data) override
    {
    }
};
class TestPlayerFactory : public AbsPlayerFactory {

    virtual AbsPlayerData * createPlayer() override
    {
        return new TestPlayerData(nextId());
    }

    virtual size_t getPlayerDataLength() override
    {
        return sizeof(unsigned int);
    }
};
class TestGlobalData : public AbsGlobalData {
    
    virtual std::vector<unsigned char> serialize() override
    {
        return std::vector<unsigned char>();
    }
    virtual void deserialize(std::vector<unsigned char> data) override
    {
    }
};

int main(int argc, char* argv[])
{/*
    if (argc == 4 && strcmp(argv[1], "png2int") == 0) {
        auto u = new Utilities();
        u->convertPng16RtoUShortRawFile(argv[2], argv[3]);
        return 0;
    }
    else if (strcmp(argv[1], "treegen") == 0) {

    }
    */

    Media::loadFileMap("../../media");
    Media::loadFileMap("../../shaders");


    // just testing mp
    if (argc > 1 && strcmp(argv[1], "server") == 0) {
        MultiplayerServer* server = new MultiplayerServer(8111, 123);
        server->setPlayerFactory(new TestPlayerFactory());
        server->setGlobalData(new TestGlobalData());
        server->start();
        while (true)getchar();
        return 0;
    }

    Application *game = new Application(800, 600);

    MultiplayerClient* client = new MultiplayerClient(123);
    client->setPlayerFactory(new TestPlayerFactory());
    client->setGlobalData(new TestGlobalData());
    client->connect("127.0.0.1", 8111);

    EditorApp* app = new EditorApp();
    app->initialize();
    app->bind(game);
    game->start();
    
    return 0;
}
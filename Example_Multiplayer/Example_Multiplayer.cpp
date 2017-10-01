#include "stdafx.h"  

#include "Application.h"
#include "Media.h" 

#include "SFML\Network.hpp"
#include "MultiplayerClient.h"
#include "MultiplayerServer.h"
#include "AbsPlayerData.h"
#include "AbsPlayerFactory.h"
#include "AbsGlobalData.h"
int main(int argc, char* argv[])
{
    Media::loadFileMap("../../media");
    Media::loadFileMap("../../shaders");

    /*
    // just testing mp
    if (argc > 1 && strcmp(argv[1], "server") == 0) {
        MultiplayerServer* server = new MultiplayerServer(8111, 123);
        server->setPlayerFactory(new TestPlayerFactory());
        server->setGlobalData(new TestGlobalData());
        server->start();
        while (true)getchar();
        return 0;
    }


    MultiplayerClient* client = new MultiplayerClient(123);
    client->setPlayerFactory(new TestPlayerFactory());
    client->setGlobalData(new TestGlobalData());
    client->connect("127.0.0.1", 8111);*/

    Application *game = new Application(800, 600);

    auto img1 = new UIBitmap(game->ui, 0.1, 0.2, 0.1, 0.1, game->asset->loadTextureFile("witcher_icon.png"), UIColor(1, 0, 0, 0.5));

    game->ui->bitmaps.push_back(img1);

    game->mainDisplayCamera = new Camera();

    game->onRenderFrame.add([&](int zero) {
        img1->x = sin(game->time) * 0.5 + 0.5;
    });
    game->start();

    return 0;
}


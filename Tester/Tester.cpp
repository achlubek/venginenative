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
    
    Application *game = new Application(1600, 900);
    
    EditorApp* app = new EditorApp();
    app->initialize();
    app->bind(game);
    game->start();
    
    return 0;
}
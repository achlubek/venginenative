// Tester.cpp : Defines the entry point for the console application.
//

#include "stdafx.h"

#include "../VEngineNative/Camera.h"
#include "../VEngineNative/Object3dInfo.h"
#include "../VEngineNative/Media.h"
#include "../VEngineNative/World.h"
#include "../VEngineNative/Scene.h"
#include "../VEngineNative/Material.h"
#include "../VEngineNative/Mesh3d.h"
#include "../VEngineNative/Light.h"
#include "../VEngineNative/SquirrelVM.h"
#include "../VEngineNative/GLSLVM.h"
#include "../VEngineNative/SimpleParser.h"
#include "../VEngineNative/imgui/imgui.h"

#include "EditorApp.h"
#include "Car.h"
#include "Utilities.h"
#include "windows.h"


Mesh3d * loadRawMesh(string file) {
    Material *mat = new Material();

    unsigned char* bytes;
    int bytescount = Media::readBinary(file, &bytes);
    GLfloat * floats = (GLfloat*)bytes;
    int floatsCount = bytescount / 4;
    vector<GLfloat> flo(floats, floats + floatsCount);

    Object3dInfo *o3i = new Object3dInfo(flo);

    return Mesh3d::create(o3i, mat);
}



int main(int argc, char* argv[])
{
    if (argc == 4 && strcmp(argv[1], "png2int") == 0) {
        auto u = new Utilities();
        u->convertPng16RtoUShortRawFile(argv[2], argv[3]);
        return 0;
	}
	else if (strcmp(argv[1], "treegen") == 0) {

	}

    EditorApp* app = new EditorApp();
    app->initialize();
    Game *game = new Game(app->width, app->height);
    game->start();
    volatile bool ready = false;
    game->invoke([&ready]() {
        ready = true;
    });
    while (!ready);
    
    app->bind(game);

    while (!game->hasExited) {
        Sleep(100);
    }

    return 0;
}
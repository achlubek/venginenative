// Tester.cpp : Defines the entry point for the console application.
//

#include "stdafx.h"
 
#include "windows.h"

#include "../VEngineNative/stdafx.h"
#include "../VEngineNative/Game.h"
#include "../VEngineNative/Media.h"

/*
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
*/


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
    //EditorApp* app = new EditorApp();
   // app->initialize();
	Media::loadFileMap("../../media");
	Media::loadFileMap("../../shaders");
    Game *game = new Game(640, 480);
    game->start();
    volatile bool ready = false;
    game->invoke([&ready]() {
        ready = true;
    });
    while (!ready);

	auto cam = new Camera();
	float FOV = 85.0f;
	cam->createProjectionPerspective(FOV, (float)game->width / (float)game->height, 0.01f, 10000);
	game->onWindowResize->add([&](int zero) {
		cam->createProjectionPerspective(FOV, (float)game->width / (float)game->height, 0.01f, 10000);
	});
	cam->transformation->translate(glm::vec3(16, 16, 16));
	glm::quat rot = glm::quat_cast(glm::lookAt(cam->transformation->getPosition(), glm::vec3(0), glm::vec3(0, 1, 0)));
	cam->transformation->setOrientation(rot);
	game->world->mainDisplayCamera = cam;
    
	game->world->scene->addMesh3d(game->asset->loadMeshFile("lucy.mesh3d"));
   // app->bind(game);

    while (!game->hasExited) {
        Sleep(100);
    }

    return 0;
}
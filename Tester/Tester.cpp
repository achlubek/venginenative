// Tester.cpp : Defines the entry point for the console application.
//

#include "stdafx.h"

#include "windows.h"

#include "../VEngineNative/stdafx.h"
#include "../VEngineNative/Game.h"
#include "../VEngineNative/Media.h"
#include "EditorApp.h"

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
	Game *game = new Game(1920, 1010);
	
	/*
 

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

	float* lastcx = new float[1];
	float* lastcy = new float[1];
	float* yaw = new float[1];
	float* pitch = new float[1];


	//game->world->scene->addMesh3d(game->asset->loadMeshFile("lucy.mesh3d"));
	auto sponza = game->asset->loadSceneFile("sp.scene");
	game->world->scene = sponza;
	game->world->scene->getMesh3ds()[0]->getLodLevel(0)->ignoreFrustumCulling = true;
	// app->bind(game);

	game->onRenderFrame->add([&](int zero) {

		float speed = 0.1f;
		if (game->getKeyStatus(GLFW_KEY_LEFT_CONTROL) == GLFW_PRESS) {
			speed *= 0.1f;
		}
		if (game->getKeyStatus(GLFW_KEY_LEFT_ALT) == GLFW_PRESS) {
			speed *= 10;
		}
		if (game->getKeyStatus(GLFW_KEY_LEFT_SHIFT) == GLFW_PRESS) {
			speed *= 30.1f;
		}
		glm::vec3 dw = glm::vec3(0);
		float w = 0.0;
		if (game->getKeyStatus(GLFW_KEY_W) == GLFW_PRESS) {
			dw += cam->transformation->getOrientation() * glm::vec3(0, 0, -1);
			w += 1.0;
		}
		if (game->getKeyStatus(GLFW_KEY_S) == GLFW_PRESS) {
			dw += cam->transformation->getOrientation() * glm::vec3(0, 0, 1);
			w += 1.0;
		}
		if (game->getKeyStatus(GLFW_KEY_A) == GLFW_PRESS) {
			dw += cam->transformation->getOrientation() * glm::vec3(-1, 0, 0);
			w += 1.0;
		}
		if (game->getKeyStatus(GLFW_KEY_D) == GLFW_PRESS) {
			dw += cam->transformation->getOrientation() * glm::vec3(1, 0, 0);
			w += 1.0;
		}

		glm::vec3 a = dw / w;
		//dir = mix(dir, w > 0.0 ? a : dw, 0.02);
		glm::vec3 dir = w > 0.0 ? a : dw;
		glm::vec3 newpos = cam->transformation->getPosition();
		newpos += length(dir) > 0.0 ? (normalize(dir) * speed) : (glm::vec3(0.0));

		glm::dvec2 cursor = game->getCursorPosition();

		//int acnt = 0;
		//const float * axes = glfwGetJoystickAxes(0, &acnt);
		float dx = (float)(*lastcx - cursor.x);
		float dy = (float)(*lastcy - cursor.y);
		//if (acnt >= 4) {
		//    dx -= axes[2] * 10.9;
		//    dy += axes[3] * 10.9;
		//    newpos += (cam->transformation->orientation * glm::vec3(0, 0, -1) * axes[1] * 0.1f);
		//   newpos += (cam->transformation->orientation * glm::vec3(1, 0, 0) * axes[0] * 0.1f);
		//}
		*lastcx = cursor.x;
		*lastcy = cursor.y;
		*yaw += dy * 0.2f;
		*pitch += dx * 0.2f;
		if (*yaw < -90.0) *yaw = -90;
		if (*yaw > 90.0) *yaw = 90;
		if (*pitch < -360.0f)*pitch += 360.0f;
		if (*pitch > 360.0f) *pitch -= 360.0f;
		glm::quat newrot = glm::angleAxis(deg2rad(*pitch), glm::vec3(0, 1, 0)) * glm::angleAxis(deg2rad(*yaw), glm::vec3(1, 0, 0));
		cam->transformation->setOrientation(glm::slerp(newrot, cam->transformation->getOrientation(), 0.9f));
		cam->transformation->setPosition(glm::mix(newpos, cam->transformation->getPosition(), 0.8f));
	});*/

	EditorApp* app = new EditorApp();
	app->initialize();
	app->bind(game);
	game->start();

	while (!game->hasExited) {
		Sleep(100);
	}

	return 0;
}
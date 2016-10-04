// Tester.cpp : Defines the entry point for the console application.
//

#include "stdafx.h"

#include "../VEngineNative/Camera.h";
#include "../VEngineNative/Object3dInfo.h";
#include "../VEngineNative/Object3dManager.h";
#include "../VEngineNative/Media.h";
#include "../VEngineNative/World.h";
#include "../VEngineNative/Scene.h";
#include "../VEngineNative/Material.h";
#include "../VEngineNative/Mesh3d.h";
#include "../VEngineNative/Light.h";
#include "../VEngineNative/SquirrelVM.h";
#include "../VEngineNative/imgui/imgui.h";

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

int main()
{
    Media::loadFileMap("../../media");
    Media::loadFileMap("../../shaders");
    Game *game = new Game(1280, 720);
    game->start();
    volatile bool ready = false;
    game->invoke([&ready]() {
        ready = true;
    });
    while (!ready);

    Camera *cam = new Camera();
    cam->createProjectionPerspective(deg2rad(95.0f), (float)game->width / (float)game->height, 0.01f, 1000);
    game->onWindowResize->add([&](int zero) {
        cam->createProjectionPerspective(deg2rad(95.0f), (float)game->width / (float)game->height, 0.01f, 1000);
    });
    cam->transformation->translate(glm::vec3(0, 0, 4));
    glm::quat rot = glm::quat_cast(glm::lookAt(cam->transformation->position, glm::vec3(0), glm::vec3(0, 1, 0)));
    cam->transformation->setOrientation(rot);
    game->world->mainDisplayCamera = cam;

    // mesh loading

    SquirrelVM* sq = new SquirrelVM();
    sq->compileFile(Media::getPath("squireeltest.txt"));
    sq->callProcedureVoid("testme");

     game->world->scene = game->asset->loadSceneFile("terrain.scene");
     //game->world->scene->getMeshes()[0]->getInstance(0)->transformation->translate(glm::vec3(0, -62.5f, 0));
     //game->world->scene->getMeshes()[0]->getInstance(0)->transformation->scale(glm::vec3(0.1));
    //game->world->scene->getMeshes()[0]->getInstance(0)->transformation->rotate(glm::angleAxis(deg2rad(73.75f), glm::vec3(-0.006f, -0.005f, 1.0f)));
  //  game->world->scene->addMesh(game->asset->loadMeshFile("treeground.mesh3d"));
   // auto t = game->asset->loadMeshFile("lucy.mesh3d");
   // game->world->scene->addMesh(t);
    bool isOpened = true;


    unsigned char* pixels;
    int width, height;
    //io.Fonts->GetTexDataAsAlpha8(&pixels, &width, &height);
    bool imugiinit = false;
    game->onRenderUIFrame->add([&](int zero) {
        if (!imugiinit) {
            ImGuiIO& io = ImGui::GetIO();
           // io.Fonts->ClearFonts();
            io.Fonts->AddFontFromFileTTF(Media::getPath("segoeui.ttf").c_str(), 10);
            imugiinit = true;
        }
        static float f = 0.0f;
        if (Game::instance->renderer->cloudsThresholdLow > Game::instance->renderer->cloudsThresholdHigh) {
            Game::instance->renderer->cloudsThresholdHigh = Game::instance->renderer->cloudsThresholdLow;
            Game::instance->renderer->cloudsThresholdLow = Game::instance->renderer->cloudsThresholdHigh;
        }
        else
            if (Game::instance->renderer->cloudsThresholdHigh < Game::instance->renderer->cloudsThresholdLow) {
                Game::instance->renderer->cloudsThresholdLow = Game::instance->renderer->cloudsThresholdHigh;
            }
        ImGui::Begin("Clouds", &isOpened, 0);
        //    ImGui::Text("Terrain roughness:");
       //     ImGui::SliderFloat("roughness", &t->getLodLevel(0)->material->roughness, 0.0f, 1.0f);
       //     ImGui::Text("Terrain metalness:");
       //     ImGui::SliderFloat("metalness", &t->getLodLevel(0)->material->metalness, 0.0f, 1.0f);
        ImGui::SliderFloat("CloudsFloor", &Game::instance->renderer->cloudsFloor, 100.0f, 30000.0f);
        ImGui::SliderFloat("CloudsCeil", &Game::instance->renderer->cloudsCeil, 100.0f, 30000.0f);
        ImGui::SliderFloat("CloudsThresholdLow", &Game::instance->renderer->cloudsThresholdLow, -1.0f, 1.0f);
        ImGui::SliderFloat("CloudsThresholdHigh", &Game::instance->renderer->cloudsThresholdHigh, -1.0f, 1.0f);
        //ImGui::SliderFloat("CloudsAtmosphereShaftsMultiplier", &Game::instance->renderer->cloudsAtmosphereShaftsMultiplier, 0.0f, 10.0f);
        //ImGui::SliderFloat("CloudsWindSpeed", &Game::instance->renderer->cloudsWindSpeed, 0.0f, 10.0f);
        ImGui::SliderFloat("CloudsDensityScale", &Game::instance->renderer->cloudsDensityScale, 0.0f, 5.0f);
        //ImGui::SliderFloat("CloudsDensityThresholdLow", &Game::instance->renderer->cloudsDensityThresholdLow, 0.0f, 1.0f);
       // ImGui::SliderFloat("CloudsDensityThresholdHigh", &Game::instance->renderer->cloudsDensityThresholdHigh, 0.0f, 1.0f);
        //ImGui::SliderFloat("AtmosphereScale", &Game::instance->renderer->atmosphereScale, 0.0f, 1000.0f);
        ImGui::SliderFloat("WaterWavesScale", &Game::instance->renderer->waterWavesScale, 0.0f, 10.0f);
        ImGui::SliderFloat("Noise1", &Game::instance->renderer->noiseOctave1, 0.01f, 10.0f);
        ImGui::SliderFloat("Noise2", &Game::instance->renderer->noiseOctave2, 0.01f, 10.0f);
        ImGui::SliderFloat("Noise3", &Game::instance->renderer->noiseOctave3, 0.01f, 10.0f);
        ImGui::SliderFloat("Noise4", &Game::instance->renderer->noiseOctave4, 0.01f, 10.0f);
        ImGui::SliderFloat("Noise5", &Game::instance->renderer->noiseOctave5, 0.01f, 400.0f);
        ImGui::SliderFloat("Noise6", &Game::instance->renderer->noiseOctave6, 0.01f, 10.0f);
        ImGui::SliderFloat("Noise7", &Game::instance->renderer->noiseOctave7, 0.01f, 10.0f);
        ImGui::SliderFloat("LensBlurAmout", &Game::instance->renderer->lensBlurSize, 0.01f, 10.0f);
        //ImGui::SliderFloat("Noise8", &Game::instance->renderer->noiseOctave8, 0.01f, 10.0f);
//        ImGui::SliderFloat("RoughnessTerra", &game->world->scene->getMeshes()[0]->getLodLevel(0)->material->roughness, 0.01f, 10.0f);
        ImGui::SliderFloat("CloudsIntegrate", &Game::instance->renderer->cloudsIntegrate, 0.3f, 0.99f);
        ImGui::SliderFloat3("CloudsOffset", (float*)&Game::instance->renderer->cloudsOffset, -1000.0f, 1000.0f);
        ImGui::SliderFloat3("SunDirection", (float*)&Game::instance->renderer->sunDirection, -1.0f, 1.0f);
        ImGui::SliderFloat("MieScattering", (float*)&Game::instance->renderer->mieScattCoefficent, 0.0f, 20.0f);
        for (auto i = Game::instance->renderer->cloudsShader->shaderVariables.begin(); i != Game::instance->renderer->cloudsShader->shaderVariables.end(); i++) {

            if (i->second->type == SHADER_VARIABLE_TYPE_FLOAT) ImGui::SliderFloat(i->first.c_str(), &i->second->var_float, 0.01f, 10.0f);
            if (i->second->type == SHADER_VARIABLE_TYPE_INT) ImGui::SliderInt(i->first.c_str(), &i->second->var_int, 0, 20);
            if (i->second->type == SHADER_VARIABLE_TYPE_VEC2) ImGui::SliderFloat2(i->first.c_str(), &i->second->var_vec2.x, 0.01f, 10.0f);
            if (i->second->type == SHADER_VARIABLE_TYPE_IVEC2) ImGui::SliderInt2(i->first.c_str(), &i->second->var_ivec2.x, 0.0, 20);
            if (i->second->type == SHADER_VARIABLE_TYPE_VEC3) ImGui::SliderFloat3(i->first.c_str(), &i->second->var_vec3.x, 0.01f, 10.0f);
            if (i->second->type == SHADER_VARIABLE_TYPE_IVEC3) ImGui::SliderInt3(i->first.c_str(), &i->second->var_ivec3.x, 0.0, 20);
            if (i->second->type == SHADER_VARIABLE_TYPE_VEC4) ImGui::SliderFloat4(i->first.c_str(), &i->second->var_vec4.x, 0.01f, 10.0f);
            if (i->second->type == SHADER_VARIABLE_TYPE_IVEC4) ImGui::SliderInt4(i->first.c_str(), &i->second->var_ivec4.x, 0.0, 20);
        }
        ImGui::Text("%.3f ms/frame %.1f FPS", 1000.0f / ImGui::GetIO().Framerate, ImGui::GetIO().Framerate);
        ImGui::End();
    });
    /*
    for (int i = 0; i < 11; i++) {

        for (int g = 0; g < 11; g++) {
            float rough = (float)i / 11.0f;
            float met = (float)g / 11.0f;
            Mesh3d* mesh = game->asset->loadMeshFile("icosphere.mesh3d");
            mesh->getLodLevel(0)->material->roughness = rough;
            mesh->getLodLevel(0)->material->metalness = met;
            mesh->getInstance(0)->transformation->translate(glm::vec3(i, 0, g) * 8.0f);
            game->world->scene->addMesh(mesh);
        }
    }
    */
    Light* light = game->asset->loadLightFile("test.light");
    light->type = LIGHT_SPOT;
    light->angle = 78;
    //light->cutOffDistance = 90;
    game->world->scene->addLight(light);

    Renderer * envRenderer = new Renderer(512, 512);
    envRenderer->useAmbientOcclusion = false;
    envRenderer->useGammaCorrection = false;
    vector<EnvPlane*> planes = {};
    planes.push_back(new EnvPlane(glm::vec3(0, 0, 0), glm::vec3(0, 1, 0)));/*
    planes.push_back(new EnvPlane(glm::vec3(0, 90, 0), glm::vec3(0, -1, 0)));
    planes.push_back(new EnvPlane(glm::vec3(0, 0, -8), glm::vec3(0, 0, 1)));
    planes.push_back(new EnvPlane(glm::vec3(0, 0, 6), glm::vec3(0, 0, -1)));
    planes.push_back(new EnvPlane(glm::vec3(-39, 0, 0), glm::vec3(1, 0, 0)));
    planes.push_back(new EnvPlane(glm::vec3(40, 0, 0), glm::vec3(-1, 0, 0)));*/

    EnvProbe* probe1 = new EnvProbe(envRenderer, planes);
    probe1->transformation->translate(glm::vec3(15, 6, 15));
    game->world->scene->addEnvProbe(probe1);

    EnvProbe* probe2 = new EnvProbe(envRenderer, planes);
    probe2->transformation->translate(glm::vec3(15, 6, -15));
    game->world->scene->addEnvProbe(probe2);

    EnvProbe* probe3 = new EnvProbe(envRenderer, planes);
    probe3->transformation->translate(glm::vec3(-15, 6, 15));
    game->world->scene->addEnvProbe(probe3);

    EnvProbe* probe4 = new EnvProbe(envRenderer, planes);
    probe4->transformation->translate(glm::vec3(-15, 6, -15));
    game->world->scene->addEnvProbe(probe4);

    bool cursorFree = false;
    bool envRefresh = true;
    game->onKeyPress->add([&game, &cursorFree, &cam, &envRefresh](int key) {
        if (key == GLFW_KEY_PAUSE) {
            game->shaders->materialShader->recompile();
            game->shaders->depthOnlyShader->recompile();
            game->shaders->depthOnlyGeometryShader->recompile();
            game->shaders->materialGeometryShader->recompile();
            game->renderer->recompileShaders();
        }
        if (key == GLFW_KEY_0) {
            game->renderer->useAmbientOcclusion = !game->renderer->useAmbientOcclusion;
        }
        if (key == GLFW_KEY_F2) {
            Light* ca = game->asset->loadLightFile("candle.light");
            ca->transformation->position = cam->transformation->position;
            ca->transformation->orientation = cam->transformation->orientation;
            game->world->scene->addLight(ca);
        }
        if (key == GLFW_KEY_F3) {
            Light* ca = game->asset->loadLightFile("corrector.light");
            ca->transformation->position = cam->transformation->position;
            ca->transformation->orientation = cam->transformation->orientation;
            game->world->scene->addLight(ca);
        }
        if (key == GLFW_KEY_F4) {
            envRefresh = true;
        }
        if (key == GLFW_KEY_TAB) {
            if (!cursorFree) {
                cursorFree = true;
                game->setCursorMode(GLFW_CURSOR_NORMAL);
            }
            else {
                cursorFree = false;
                game->setCursorMode(GLFW_CURSOR_DISABLED);
            }
        }
    });

    float yaw = 0.0f, pitch = 0.0f;
    double lastcx = 0.0f, lastcy = 0.0f;
    bool intializedCameraSystem = false;

    game->setCursorMode(GLFW_CURSOR_DISABLED);

    game->onRenderFrame->add([&](int i) {
        if (envRefresh) {
            probe1->refresh();
            probe2->refresh();
            probe3->refresh();
            probe4->refresh();
            envRefresh = false;
        }
        if (!cursorFree) {
            float speed = 1.1f;
            if (game->getKeyStatus(GLFW_KEY_LEFT_CONTROL) == GLFW_PRESS) {
                speed *= 0.1f;
            }
            if (game->getKeyStatus(GLFW_KEY_LEFT_ALT) == GLFW_PRESS) {
                speed *= 10.0f;
            }
            if (game->getKeyStatus(GLFW_KEY_LEFT_SHIFT) == GLFW_PRESS) {
                speed *= 3.0f;
            }
            if (game->getKeyStatus(GLFW_KEY_F1) == GLFW_PRESS) {
                light->transformation->position = cam->transformation->position;
                light->transformation->orientation = cam->transformation->orientation;
            }
            if (game->getKeyStatus(GLFW_KEY_W) == GLFW_PRESS) {
                glm::vec3 dir = cam->transformation->orientation * glm::vec3(0, 0, -1);
                cam->transformation->translate(dir * speed);
            }
            if (game->getKeyStatus(GLFW_KEY_S) == GLFW_PRESS) {
                glm::vec3 dir = cam->transformation->orientation * glm::vec3(0, 0, 1);
                cam->transformation->translate(dir * speed);
            }
            if (game->getKeyStatus(GLFW_KEY_A) == GLFW_PRESS) {
                glm::vec3 dir = cam->transformation->orientation * glm::vec3(-1, 0, 0);
                cam->transformation->translate(dir * speed);
            }
            if (game->getKeyStatus(GLFW_KEY_D) == GLFW_PRESS) {
                glm::vec3 dir = cam->transformation->orientation * glm::vec3(1, 0, 0);
                cam->transformation->translate(dir * speed);
            }
            if (game->getKeyStatus(GLFW_KEY_ESCAPE) == GLFW_PRESS) {
                game->shouldClose = true;
            }
            glm::dvec2 cursor = game->getCursorPosition();
            if (!intializedCameraSystem) {
                lastcx = cursor.x;
                lastcy = cursor.y;
                intializedCameraSystem = true;
            }
            float dx = (float)(lastcx - cursor.x);
            float dy = (float)(lastcy - cursor.y);
            lastcx = cursor.x;
            lastcy = cursor.y;
            yaw += dy * 0.2f;
            pitch += dx * 0.2f;
            if (yaw < -90.0) yaw = -90;
            if (yaw > 90.0) yaw = 90;
            if (pitch < -360.0f) pitch += 360.0f;
            if (pitch > 360.0f) pitch -= 360.0f;
            glm::quat newrot = glm::angleAxis(deg2rad(pitch), glm::vec3(0, 1, 0)) * glm::angleAxis(deg2rad(yaw), glm::vec3(1, 0, 0));
            cam->transformation->setOrientation(newrot);
        }
    });

    while (!game->shouldClose) {
    }
    return 0;
}
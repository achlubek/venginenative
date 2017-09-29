#include "stdafx.h"
#include "EditorApp.h" 
#include "Physics.h"
#include "Keyboard.h"
#include "Mouse.h"
#include "Camera.h"
#include "Application.h"
#include "Sound.h"
#include "Sound3d.h"
#include "Scene.h"
#include "Mesh3d.h"
#include "Mesh3dInstance.h"
#include "Mesh3dLodLevel.h"
#include "TransformationManager.h"
#include "UIBox.h"
#include "UIBitmap.h"
#include "UIText.h"
#include "UIRenderer.h"
#include "UIColor.h"
#include "Chat.h"
#include "VulkanMemoryChunk.h"
#include "VulkanMemoryManager.h"
#include "AnimationPack.h"
#include "AnimationPlayer.h"
#include "SpotLight.h"
#include <time.h>

#undef max

void writeLog(std::string s) {
    std::ofstream log("log.txt", std::ios_base::app | std::ios_base::out);
    auto t = time(0);
    log << ctime(&t) << ": " << s << endl;
}

EditorApp::EditorApp()
{
    physics = new Physics();
}


EditorApp::~EditorApp()
{
}

void EditorApp::initialize()
{
}

void EditorApp::onRenderFrame(float elapsed)
{
    app->renderer->lights[0]->transformation->rotate(glm::angleAxis(0.01f, glm::vec3(0.0, 1.0, 0.0)));
    app->renderer->lights[0]->transformation->setPosition(glm::vec3(0.0, 10.0, 10.0f * sin(app->time)));
    app->renderer->lights[1]->transformation->rotate(glm::angleAxis(0.02f, glm::vec3(0.0, 1.0, 0.0)));
    app->renderer->lights[1]->transformation->setPosition(glm::vec3(0.0, 10.0, 20.0f * sin(1.234f * app->time)));
  //  app->renderer->lights[0]->transformation->rotate(glm::angleAxis(0.007f, glm::vec3(1.0, 0.0, 0.0)));
  //  app->renderer->lights[0]->transformation->rotate(glm::angleAxis(0.003f, glm::vec3(0.0, 0.0, 1.0)));

    walker->update();
    app->ui->texts[0]->updateText(std::to_string((float)rand()));
    physics->simulationStep(1.0 / 60.0);
    for (int i = 0; i < players.size(); i++) {
        players[i]->nextFrame();
    }
    //app->world->scene->getMesh3ds()[0]->getInstance(0)->transformation->setPosition(glm::vec3(sin(app->time), cos(app->time * 1.2), sin(app->time * 1.6)));
    //app->world->scene->getMesh3ds()[0]->getInstance(0)->transformation->setSize(glm::vec3(sin(app->time) * 0.5 + 1.5));
    if (currentMode == EDITOR_MODE_WRITING_TEXT && chat->isInputEnabled() == false) {
        currentMode = lastMode;
    }
    if (currentMode == EDITOR_MODE_MOVE_CAMERA) {
        float speed = 0.1f;
        if (keyboard->getKeyStatus(GLFW_KEY_LEFT_CONTROL) == GLFW_PRESS) {
            speed *= 0.1f;
        }
        if (keyboard->getKeyStatus(GLFW_KEY_LEFT_ALT) == GLFW_PRESS) {
            speed *= 10;
        }
        if (keyboard->getKeyStatus(GLFW_KEY_LEFT_SHIFT) == GLFW_PRESS) {
            speed *= 30.1f;
        }/*
         if (app->getKeyStatus(GLFW_KEY_F1) == GLFW_PRESS) {
         light->transformation->position = cam->transformation->position;
         light->transformation->orientation = cam->transformation->orientation;
         }*/
        glm::vec3 dw = glm::vec3(0);
        float w = 0.0;
        if (keyboard->getKeyStatus(GLFW_KEY_W) == GLFW_PRESS) {
            dw += cam->transformation->getOrientation() * glm::vec3(0, 0, -1);
            w += 1.0;
        }
        if (keyboard->getKeyStatus(GLFW_KEY_S) == GLFW_PRESS) {
            dw += cam->transformation->getOrientation() * glm::vec3(0, 0, 1);
            w += 1.0;
        }
        if (keyboard->getKeyStatus(GLFW_KEY_A) == GLFW_PRESS) {
            dw += cam->transformation->getOrientation() * glm::vec3(-1, 0, 0);
            w += 1.0;
        }
        if (keyboard->getKeyStatus(GLFW_KEY_D) == GLFW_PRESS) {
            dw += cam->transformation->getOrientation() * glm::vec3(1, 0, 0);
            w += 1.0;
        }

        if (keyboard->getKeyStatus(GLFW_KEY_UP) == GLFW_PRESS) {
            auto walkerpos = walker->walkerScene->transformation->getPosition();
            auto walkerdir = glm::inverse(walker->walkerScene->transformation->getOrientation()) * glm::vec3(0.0, 0.0, 1.0);
            if (keyboard->getKeyStatus(GLFW_KEY_LEFT_SHIFT) == GLFW_PRESS) {
                walker->run(walkerpos + walkerdir * 2.0f);
            }
            else {
                walker->walk(walkerpos + walkerdir * 2.0f);
            }
            
        } else 
        if (keyboard->getKeyStatus(GLFW_KEY_DOWN) == GLFW_PRESS) {
            auto walkerpos = walker->walkerScene->transformation->getPosition();
            auto walkerdir = glm::inverse(walker->walkerScene->transformation->getOrientation()) * glm::vec3(0.0, 0.0, 1.0);
            if (keyboard->getKeyStatus(GLFW_KEY_LEFT_SHIFT) == GLFW_PRESS) {
                walker->run(walkerpos - walkerdir * 2.0f);
            }
            else {
                walker->walk(walkerpos - walkerdir * 2.0f);
            }
        }
        else {
            walker->stop();
        }

        if (keyboard->getKeyStatus(GLFW_KEY_LEFT) == GLFW_PRESS) {
            walker->walkerScene->transformation->rotate(
                glm::quat(glm::rotate(glm::mat4(1.0f), -0.01f, glm::vec3(0.0f, 1.0f, 0.0f)))
            );
        }
        if (keyboard->getKeyStatus(GLFW_KEY_RIGHT) == GLFW_PRESS) {
            walker->walkerScene->transformation->rotate(
                glm::quat(glm::rotate(glm::mat4(1.0f), 0.01f, glm::vec3(0.0f, 1.0f, 0.0f)))
            );
        }

        glm::vec3 a = dw / w;
        //dir = mix(dir, w > 0.0 ? a : dw, 0.02);
        glm::vec3 dir = w > 0.0 ? a : dw;
        glm::vec3 newpos = cam->transformation->getPosition();
        newpos += length(dir) > 0.0 ? (normalize(dir) * speed) : (glm::vec3(0.0));

        //if (app->getKeyStatus(GLFW_KEY_ESCAPE) == GLFW_PRESS) {
        //    app->shouldClose = true;
        //}
        if (cameraFollowCar) {
            int acnt = 0;
            const float * axes = glfwGetJoystickAxes(0, &acnt);
            auto cartrans = car[0]->getTransformation();
            if (cartrans != nullptr) {
                glm::vec3 backvector = glm::vec3(0.0, 1.0, -4.0);
                auto trsf = glm::quat();
                if (acnt >= 4) {
                    trsf = glm::angleAxis(deg2rad(axes[3] * 80.0f), glm::vec3(-1.0, 0.0, 0.0)) * glm::angleAxis(deg2rad(axes[2] * 80.0f), glm::vec3(0.0, -1.0, 0.0));
                    auto trsf2 = glm::angleAxis(deg2rad(axes[3] * 80.0f), glm::vec3(1.0, 0.0, 0.0)) *  glm::angleAxis(deg2rad(axes[2] * 80.0f), glm::vec3(0.0, 1.0, 0.0));
                    backvector = trsf2 *  (backvector);
                }
                backvector = glm::mat3_cast(cartrans->getOrientation()) * backvector;
                backquat = glm::slerp(backquat, trsf, 0.03f);
                trsf = backquat;

                backvector = backvectorlast * 0.97f + backvector * 0.03f;
                backvectorlast = backvector;
                lastpos = lastpos * 0.9f + cartrans->getPosition() * 0.1f;
                cam->transformation->setPosition(lastpos + backvector);
                cam->transformation->setOrientation(glm::inverse(glm::angleAxis(deg2rad(180.0f), glm::vec3(0.0, 1.0, 0.0)) * backquat * glm::inverse(cartrans->getOrientation())));
            }

        }
        else {
            glm::dvec2 cursor;
            auto tup = mouse->getCursorPosition();
            cursor.x = get<0>(tup);
            cursor.y = get<1>(tup);

            //int acnt = 0;
            //const float * axes = glfwGetJoystickAxes(0, &acnt);
            float dx = (float)(lastcx - cursor.x);
            float dy = (float)(lastcy - cursor.y);
            //if (acnt >= 4) {
                //    dx -= axes[2] * 10.9;
                //    dy += axes[3] * 10.9;
                //    newpos += (cam->transformation->orientation * glm::vec3(0, 0, -1) * axes[1] * 0.1f);
                //   newpos += (cam->transformation->orientation * glm::vec3(1, 0, 0) * axes[0] * 0.1f);
            //}
            lastcx = cursor.x;
            lastcy = cursor.y;
            yaw += dy * 0.2f;
            pitch += dx * 0.2f;
            if (yaw < -90.0) yaw = -90;
            if (yaw > 90.0) yaw = 90;
            if (pitch < -360.0f) pitch += 360.0f;
            if (pitch > 360.0f) pitch -= 360.0f;
            glm::quat newrot = glm::angleAxis(deg2rad(pitch), glm::vec3(0, 1, 0)) * glm::angleAxis(deg2rad(yaw), glm::vec3(1, 0, 0));
            cam->transformation->setOrientation(glm::slerp(newrot, cam->transformation->getOrientation(), 0.9f));
            cam->transformation->setPosition(glm::mix(newpos, cam->transformation->getPosition(), 0.8f));

        }

        testsound->transformation->setPosition(car[0]->getTransformation()->getPosition());
        testsound->update(cam->transformation);
        testsound2->transformation->setPosition(car[0]->getTransformation()->getPosition());
        testsound2->update(cam->transformation);

        wind->transformation->setPosition(car[0]->getTransformation()->getPosition());
        wind->update(cam->transformation);

        auto campos = cam->transformation->getPosition();

        auto rot = glm::angleAxis(app->time * 0.4f, glm::vec3(0.0, 1.0, 0.0));
        /*
        ocean1->transformation->setPosition(campos + rot * glm::vec3(4.0, 0.0, 0.0));
        ocean1->update(cam->transformation);
        ocean2->transformation->setPosition(campos + rot * glm::vec3(0.0, 0.0, 4.0));
        ocean2->update(cam->transformation);
        ocean3->transformation->setPosition(campos + rot * glm::vec3(-4.0, 0.0, 0.0));
        ocean3->update(cam->transformation);
        ocean4->transformation->setPosition(campos + rot * glm::vec3(0.0, 0.0, -4.0));
        ocean4->update(cam->transformation);

        float oceanvol = (1.0f / (1.0 + 0.005 * glm::max(0.0f, campos.y * campos.y))) * 100.0;
        ocean1->setVolume(oceanvol);
        ocean2->setVolume(oceanvol);
        ocean3->setVolume(oceanvol);
        ocean4->setVolume(oceanvol);
        */
        int acnt = 0;
        const float * axes = glfwGetJoystickAxes(0, &acnt);
        if (acnt >= 1) {
            car[0]->setWheelsAngle(axes[0] * 0.9);
        }
        if (acnt >= 6) {
            float acc = (axes[5] * 0.5 + 0.5);
            float brk = (axes[4] * 0.5 + 0.5);
            car[0]->setAcceleration((acc - brk) * 0.5);
            // printf("ACCELERATION: %f\n", (acc - brk) * 1.0);
            float targetpitch = 0.9f + acc * 0.5f;

            glm::vec3 vel = car[0]->getLinearVelocity();
            float doppler = glm::dot(glm::normalize(car[0]->getTransformation()->getPosition() - cam->transformation->getPosition()), glm::normalize(vel)) * glm::length(vel) * 0.03;
            targetpitch *= targetpitch * (1.0 - 0.2 * doppler);

            testsound->setPitch(testsound->getPitch() * 0.96 + 0.04 * targetpitch);
            testsound->setVolume((0.5 + acc) * 100.0);

            float windpitch = (1.0 - 0.2 * doppler);
            testsound2->setPitch(windpitch);
            testsound2->setVolume(car[0]->getSpeed() * 10.0);
        }

    }
}

string vitoa(int i) {
    auto ss = stringstream();
    ss << i;
    return ss.str();
}


template<typename Out>
void split(const std::string &s, char delim, Out result) {
    std::stringstream ss;
    ss.str(s);
    std::string item;
    while (std::getline(ss, item, delim)) {
        *(result++) = item;
    }
}

std::vector<std::string> split(const std::string &s, char delim) {
    std::vector<std::string> elems;
    split(s, delim, std::back_inserter(elems));
    return elems;
}

std::string join(std::vector<std::string> elems, int offset) {
    std::stringstream ss;
    int sz = elems.size();
    for (int i = offset; i < sz; i++)ss << elems[i];
    return ss.str();
}

void EditorApp::onWindowResize(int width, int height)
{
}

void EditorApp::onKeyPress(int key)
{

    if (key == GLFW_KEY_F1) {
        switchMode(EDITOR_MODE_MOVE_CAMERA);
        return;
    }
    else if (key == GLFW_KEY_F2) {
        switchMode(EDITOR_MODE_PICKING);
        //isPickingWindowOpened = true;
        return;
    }
    else if (key == GLFW_KEY_F3) {
        //    switchMode(EDITOR_MODE_EDITING);
        return;
    }
    else if (key == GLFW_KEY_PAUSE) {
        //    app->renderer->recompileShaders();
        return;
    }
    else if (key == GLFW_KEY_T && currentMode != EDITOR_MODE_WRITING_TEXT) {
        switchMode(EDITOR_MODE_WRITING_TEXT);
        chat->enableInput();
        return;
    }
    else if (key == GLFW_KEY_ESCAPE && currentMode == EDITOR_MODE_WRITING_TEXT) {
        chat->disableInput();
        switchMode(lastMode);
    }

    if (key == GLFW_KEY_F9) {
        glm::vec3 hitpos, hitnorm;
        auto res = physics->rayCast(app->mainDisplayCamera->transformation->getPosition(),
            app->mainDisplayCamera->cone->reconstructDirection(glm::vec2(0.5)), hitpos, hitnorm);
        if (res != nullptr) {
            car[0]->body->transformation->setOrientation(glm::quat());
            car[0]->body->transformation->setPosition(hitpos + glm::vec3(0.0, 3.0, 0.0));
            car[0]->body->readChanges();

            car[0]->tyreLF->transformation->setOrientation(glm::quat());
            car[0]->tyreLF->transformation->setPosition(hitpos + glm::vec3(0.0, 3.0, 0.0));
            car[0]->tyreLF->readChanges();

            car[0]->tyreLR->transformation->setOrientation(glm::quat());
            car[0]->tyreLR->transformation->setPosition(hitpos + glm::vec3(0.0, 3.0, 0.0));
            car[0]->tyreLR->readChanges();

            car[0]->tyreRF->transformation->setOrientation(glm::quat());
            car[0]->tyreRF->transformation->setPosition(hitpos + glm::vec3(0.0, 3.0, 0.0));
            car[0]->tyreRF->readChanges();

            car[0]->tyreRR->transformation->setOrientation(glm::quat());
            car[0]->tyreRR->transformation->setPosition(hitpos + glm::vec3(0.0, 3.0, 0.0));
            car[0]->tyreRR->readChanges();

            car[0]->body->getRigidBody()->setLinearVelocity(btVector3(0.0, 0.0, 0.0));
            car[0]->body->getRigidBody()->setAngularVelocity(btVector3(0.0, 0.0, 0.0));

            car[0]->tyreLF->getRigidBody()->setLinearVelocity(btVector3(0.0, 0.0, 0.0));
            car[0]->tyreLF->getRigidBody()->setAngularVelocity(btVector3(0.0, 0.0, 0.0));

            car[0]->tyreRF->getRigidBody()->setLinearVelocity(btVector3(0.0, 0.0, 0.0));
            car[0]->tyreRF->getRigidBody()->setAngularVelocity(btVector3(0.0, 0.0, 0.0));

            car[0]->tyreLR->getRigidBody()->setLinearVelocity(btVector3(0.0, 0.0, 0.0));
            car[0]->tyreLR->getRigidBody()->setAngularVelocity(btVector3(0.0, 0.0, 0.0));

            car[0]->tyreRR->getRigidBody()->setLinearVelocity(btVector3(0.0, 0.0, 0.0));
            car[0]->tyreRR->getRigidBody()->setAngularVelocity(btVector3(0.0, 0.0, 0.0));
        }

    }
    if (key == GLFW_KEY_F10) {
        cameraFollowCar = !cameraFollowCar;
    }

    if (key == GLFW_KEY_F6) {
        testsound->play();
        testsound2->play();
    }
    if (key == GLFW_KEY_F7) {
        testsound->pause();
    }
    if (key == GLFW_KEY_F8) {
        testsound->stop();
    }
}


using namespace glm;
float hashx(float n) {
    return fract(sin(n)*758.5453);
    //return fract(mod(n * 2310.7566730, 21.120312534));
}

float noise3d(vec3 x) {
    vec3 p = floor(x);
    vec3 f = fract(x);
    f = f*f*(3.0f - 2.0f*f);
    float n = p.x + p.y*157.0 + 113.0*p.z;

    return mix(mix(mix(hashx(n + 0.0), hashx(n + 1.0), f.x),
        mix(hashx(n + 157.0), hashx(n + 158.0), f.x), f.y),
        mix(mix(hashx(n + 113.0), hashx(n + 114.0), f.x),
            mix(hashx(n + 270.0), hashx(n + 271.0), f.x), f.y), f.z);
}

#define foreach(a,b,c)

void EditorApp::onKeyRelease(int key)
{
}

void EditorApp::onKeyRepeat(int key)
{
}

void EditorApp::onMouseDown(int button)
{
}

void EditorApp::onMouseUp(int button)
{
}

void EditorApp::onChar(unsigned int c)
{
}

void EditorApp::onBind()
{
    SpotLight* spot = new SpotLight(app->vulkan, glm::vec3(100.0f), new TransformationManager(
        glm::vec3(1.333080, 9.693623, 8.664752),
        glm::inverse(glm::quat(0.241280, 0.615887, 0.206073, -0.721112))
    ));
    spot->enableShadowMapping(1024, 1024);
    SpotLight* spot2 = new SpotLight(app->vulkan, glm::vec3(0.0f, 100.0f, 0.0f), new TransformationManager(
        glm::vec3(1.333080, 9.693623, 8.664752),
        glm::inverse(glm::quat(0.241280, 0.615887, 0.206073, -0.721112))
    ));
    //spot2->enableShadowMapping(1024, 1024);
    app->renderer->lights.push_back(spot);
    app->renderer->lights.push_back(spot2);

    debug_marker = Mesh3d::create(Application::instance->asset->loadObject3dInfoFile("icos.raw"), new Material());

    for (int y = 0; y < 31; y++) {
        debug_marker->addInstance(new Mesh3dInstance(new TransformationManager(glm::vec3(0, 0.0, 0), glm::vec3(5.0))));
    }
    //app->scene->addMesh3d(debug_marker);

    walker = new SimpleWalker(physics, new TransformationManager(glm::vec3(13.0, 6.0, 3.0)));
    app->scene->addScene(walker->walkerScene);
    players = {}; 
    int yy = 0;
    AnimationPack* animpack = new AnimationPack("untitled.animconf");
    players.push_back(animpack->createPlayer(walker->mesh_left_arm_up->getInstance(0)->transformation, "lhumerus"));
    players.push_back(animpack->createPlayer(walker->mesh_left_arm_down->getInstance(0)->transformation, "lradius"));
    players.push_back(animpack->createPlayer(walker->mesh_left_leg_up->getInstance(0)->transformation, "lfemur"));
    players.push_back(animpack->createPlayer(walker->mesh_left_leg_down->getInstance(0)->transformation, "ltibia"));
    players.push_back(animpack->createPlayer(walker->mesh_right_arm_up->getInstance(0)->transformation, "rhumerus"));
    players.push_back(animpack->createPlayer(walker->mesh_right_arm_down->getInstance(0)->transformation, "rradius"));
    players.push_back(animpack->createPlayer(walker->mesh_right_leg_up->getInstance(0)->transformation, "rfemur"));
    players.push_back(animpack->createPlayer(walker->mesh_right_leg_down->getInstance(0)->transformation, "rtibia"));
    players.push_back(animpack->createPlayer(walker->mesh_body->getInstance(0)->transformation, "thorax"));
    players.push_back(animpack->createPlayer(walker->mesh_head->getInstance(0)->transformation, "head"));
    players.push_back(animpack->createPlayer(walker->walkerScene->transformation, "root"));



    auto box = new UIBox(app->ui, 0.1, 0.1, 0.5, 0.5, UIColor(1, 0, 1, 0.5));
    auto box2 = new UIBox(app->ui, 0.2, 0.2, 0.5, 0.5, UIColor(1, 0, 0, 0.5));
    auto img1 = new UIBitmap(app->ui, 0.1, 0.2, 0.3, 0.3, app->asset->loadTextureFile("witcher_icon.png"), UIColor(1, 0, 0, 0.5));
    auto txt = new UIText(app->ui, 0.2, 0.2, UIColor(1, 0, 0, 0.5), Media::getPath("font.ttf"), 16, "Hello World!!");
    //   app->ui->boxes.push_back(box);
     //  app->ui->boxes.push_back(box2);
     //  app->ui->bitmaps.push_back(img1);
    app->ui->texts.push_back(txt);

    chat = new Chat(app->ui, keyboard);
    chat->onSendText.add([&](std::string s) {
        onChatSendText(s);
    });

    car = {};
    cam = new Camera();
    FOV = 85.0f;
    cam->createProjectionPerspective(FOV, (float)app->width / (float)app->height, 0.01f, 10000);
    app->onWindowResize.add([&](int zero) {
        cam->createProjectionPerspective(FOV, (float)app->width / (float)app->height, 0.01f, 10000);
    });
    cam->transformation->translate(glm::vec3(7, 0, 0));
    glm::quat rot = glm::quat_cast(glm::lookAt(glm::vec3(0), glm::vec3(1, 0, 0), glm::vec3(0, 1, 0)));
    cam->transformation->setOrientation(rot);
    app->mainDisplayCamera = cam;

    mouse->setCursorMode(GLFW_CURSOR_NORMAL);
    
    auto t = app->asset->loadSceneFile("sp.scene");
    //auto diftex = new Texture2d("2222.jpg");
    //auto bumtex = new Texture2d("1111.jpg");
    Scene* sponza2 = new Scene();
    for (int i = 0; i < t->getMesh3ds().size(); i++) {
        //    t->getMesh3ds()[i]->getInstance(0)->transformation->translate(vec3(0.0, 1.5, 0.0));
            //t->getMesh3ds()[i]->addInstance(new Mesh3dInstance(new TransformationManager(t->getMesh3ds()[i]->getInstance(0)->transformation->getPosition() + vec3(40.0, 0.0, 23.0))));
            //t->getMesh3ds()[i]->addInstance(new Mesh3dInstance(new TransformationManager(t->getMesh3ds()[i]->getInstance(0)->transformation->getPosition() + vec3(-80.0, 0.0, -72.0))));
            //t->getMesh3ds()[i]->addInstance(new Mesh3dInstance(new TransformationManager(t->getMesh3ds()[i]->getInstance(0)->transformation->getPosition() + vec3(50.0, 0.0, -20.0))));
            //t->getMesh3ds()[i]->addInstance(new Mesh3dInstance(new TransformationManager(t->getMesh3ds()[i]->getInstance(0)->transformation->getPosition() + vec3(-70.0, 0.0, -40.0))));
            //t->getMesh3ds()[i]->addInstance(new Mesh3dInstance(new TransformationManager(t->getMesh3ds()[i]->getInstance(0)->transformation->getPosition() + vec3(90.0, 0.0, 80.0))));
            //t->getMesh3ds()[i]->addInstance(new Mesh3dInstance(new TransformationManager(t->getMesh3ds()[i]->getInstance(0)->transformation->getPosition() + vec3(-90.0, 0.0, 0.0))));

            //t->getMesh3ds()[i]->getLodLevel(0)->material = new Material();
            //t->getMesh3ds()[i]->getLodLevel(0)->material->diffuseColor = glm::vec3(1.0);
            //  t->getMesh3ds()[i]->getLodLevel(0)->material->diffuseColorTex = diftex;
            /// t->getMesh3ds()[i]->getLodLevel(0)->material->bumpTex = bumtex;
        sponza2->addMesh3d(t->getMesh3ds()[i]);
    }
    sponza2->transformation->scale(glm::vec3(10.0f));
    app->scene->addScene(sponza2);

        /*
        auto s = app->asset->loadMeshFile("grass_base.mesh3d");
        s->getLodLevel(0)->disableFaceCulling = true;
        s->getLodLevel(1)->disableFaceCulling = true;
        s->getLodLevel(2)->disableFaceCulling = true;
        s->getLodLevel(3)->disableFaceCulling = true;
        srand(static_cast <unsigned> (time(0)));
        float fx = 0.0f, fy = 0.0f;
        for (int x = 0; x < 200; x++) {
        for (int y = 0; y < 400; y++) {
        float r = static_cast <float> (rand()) / static_cast <float> (RAND_MAX);
        float r2 = static_cast <float> (rand()) / static_cast <float> (RAND_MAX);
        float r3 = static_cast <float> (rand()) / static_cast <float> (RAND_MAX);
        r2 = r2 * 2.0 * 1.0;
        r3 = r3 * 2.0 * 1.0;
        auto rot = glm::angleAxis(deg2rad(r * 3.1415f * 2.0f), glm::vec3(0.0, 1.0, 0.0));
        float noize = noise3d(vec3(fx * 0.2f, fy * 0.2f, 0.0));
        s->addInstance(new Mesh3dInstance(new TransformationManager(glm::vec3(fx * 20.0f + 1.0f * r2, 20.0f + noize * 20.0f, fy * 20.0f + 1.0f * r3), rot)));
        fx += 0.1f;
        }
        fy += 0.1f;
        fx = 0.0f;
        }
        app->world->scene->addMesh3d(s);*/
        /*
        auto s = app->asset->loadMeshFile("terr_grass.mesh3d");
        auto grasses = s->getInstances();
        int grassess = grasses.size();
        for (int i = 0; i < grassess; i++) {
        auto pos = grasses[i]->transformation->getPosition();
        grasses[i]->transformation->setSize(glm::vec3(1.0f));
        grasses[i]->transformation->setPosition(glm::vec3(pos.x * 1.0f + 500.0f, pos.y * 4.0f - 10.0f, pos.z * 1.0f + 500.0f));
        }
        //app->world->scene->addMesh3d(s);
        */

        /*
        int terrainparts = 10;
        float fullsize = 3612.0;
        float partsize = fullsize / 10.0;
        //auto tex = new Texture2d("terrain_diffuse.png");
        for (int x = 0; x < 10; x++) {
            for (int y = 0; y < 10; y++) {
                stringstream ss0;
                ss0 << "real_terr_lod0_" << x << "x" << y << ".raw";
                stringstream ss1;
                ss1 << "real_terr_lod1_" << x << "x" << y << ".raw";
                stringstream ss2;
                ss2 << "real_terr_lod2_" << x << "x" << y << ".raw";
                auto mat = new Material();
                //   mat->diffuseColorTex = tex;
                Mesh3d* m = Mesh3d::create(app->asset->loadObject3dInfoFile(ss0.str()), mat);
                m->getInstance(0)->transformation->setPosition(vec3(partsize * x * 1.0f, -220.5, partsize*y * 1.0f));
                m->getInstance(0)->transformation->setSize(vec3(1.0f, 20.0f, 1.0f));
                m->getLodLevel(0)->distanceStart = 0.0f;
                m->getLodLevel(0)->distanceEnd = 650.0f;
                m->addLodLevel(new Mesh3dLodLevel(app->asset->loadObject3dInfoFile(ss1.str()), mat, 650.0f, 3350.0f));
                m->addLodLevel(new Mesh3dLodLevel(app->asset->loadObject3dInfoFile(ss2.str()), mat, 3350.0f, 11150.0f));
                app->scene->addMesh3d(m);
            }
        }*/



        //  t->name = "flagbase";
        // app->world->scene->addMesh(t);
        //auto t1 = new TransformationManager(glm::vec3(8.0, 6.0, 8.0));
        // car.push_back(new Car(t1));
        //  auto t2 = new TransformationManager(glm::vec3(-8.0, 6.0, -8.0));
        // car.push_back(new Car(t2));



       // auto xt = app->asset->loadMeshFile("icosphere.mesh3d");
       // xt->addInstance(new Mesh3dInstance(new TransformationManager(glm::vec3(0.0, 10.0, 0.0))));
       // xt->addInstance(new Mesh3dInstance(new TransformationManager(glm::vec3(0.0, -10.0, 0.0))));
       // auto abody = physics->createBody(0.1f, xt->getInstance(0)->transformation, new btBoxShape(btVector3(1000.0f, 1.0f, 1000.0f)));

       // app->scene->addMesh3d(xt);
        //physics->addBody(abody);
        //auto abody2 = Game::instance->world->physics->createBody(0.0f, new TransformationManager(glm::vec3(0.0, 0.0, 0.0)), new btStaticPlaneShape(btVector3(0.0, 1.0, 0.0), 5.0));
        //abody2->enable();
        auto groundplane = app->asset->loadMeshFile("2dplane.mesh3d");
        groundplane->addInstance(new Mesh3dInstance(new TransformationManager(glm::vec3(0.0, 1.0, 0.0), glm::vec3(100.0, 1.0, 100.0))));
        groundplane->getLodLevel(0)->material->diffuseColor = glm::vec3(1.0f);
      //  app->scene->addMesh3d(groundplane);
        auto groundpb = physics->createBody(0.0f, new TransformationManager(glm::vec3(0.0, 0.0, 0.0)), new btBoxShape(btVector3(1000.0f, 1.0f, 1000.0f)));
        // groundpb->getCollisionObject()->setFriction(4);
        physics->addBody(groundpb);
        app->invoke([&]() {/*
            auto phys = Game::instance->world->physics;

            vector<float> vertices = {};
            vector<int> indices = {};
            auto wterrobj = app->asset->loadObject3dInfoFile("weirdterrain.raw");
            auto wterr3d = Mesh3d::create(wterrobj, new Material());
            //app->world->scene->addMesh3d(wterr3d);
            int ix = 0;
            auto str = new btTriangleMesh();
            for (int i = 0; i < wterrobj->vbo.size(); i += 12 * 3) {
                // indices.push_back(ix++);
                //   vertices.push_back(wterrobj->vbo[i]);
                //   vertices.push_back(wterrobj->vbo[i+1]);
                //   vertices.push_back(wterrobj->vbo[i+2]);
                str->addTriangle(
                    btVector3(wterrobj->vbo[i], wterrobj->vbo[i + 1], wterrobj->vbo[i + 2]),
                    btVector3(wterrobj->vbo[i + 12], wterrobj->vbo[i + 1 + 12], wterrobj->vbo[i + 2 + 12]),
                    btVector3(wterrobj->vbo[i + 24], wterrobj->vbo[i + 1 + 24], wterrobj->vbo[i + 2 + 24]),
                    true
                );
                str->addIndex(ix++);
            }
            // auto shape = new btBvhTriangleMeshShape(str, true, true);
            unsigned char* bytes;
            int bytescount = Media::readBinary("SAfull.hmap", &bytes);
            float maxh = 0.0;
            float minh = 1231230.0;
            float *bytes2 = new float[bytescount / 2];
            for (int i = 0; i < bytescount; i += 2) {
                int halfi = i / 2;
                int x = halfi % 6000;
                int y = halfi / 6000;
                float bs = 0.0, bw = 0.0;
                for (int ix = -5; ix < 5; ix++) {
                    for (int iy = -5; iy < 5; iy++) {
                        int xx = (halfi % 6000) + ix;
                        int xy = (halfi / 6000) + iy;
                        if (xx < 0)xx = 0;
                        if (xx >= 6000) xx = 5999;
                        if (xy < 0)xy = 0;
                        if (xy >= 6000) xy = 5999;
                        bw += 1.0;
                        int coord = (xy * 6000 + xx) * 2;
                        unsigned short sh = (bytes[coord + 1] << 8) | bytes[coord];
                        bs += ((float)sh / 65535.0f) * 512.0f;
                    }
                }
                bs /= bw;
                bytes2[y * 6000 + x] = bs;
                maxh = glm::max(maxh, bs);
                minh = glm::min(minh, bs);
            }
            auto terrashape = new btHeightfieldTerrainShape(6000, 6000, bytes2, 1.0, minh, maxh, 1, PHY_FLOAT, false);
            */
            //  auto groundpb = phys->createBody(0.0f, new TransformationManager(glm::vec3(3000.0 - 300.0, -0.5 + maxh * 0.5, 3000.0 - 300.0), glm::quat(), glm::vec3(1.0, 1.0, 1.0)), terrashape);


        });

        // virtualbox = app->world->physics->createBody(0.0f, new TransformationManager(), new btBoxShape(btVector3(6.0, 6.0, 6.0)));
        // virtualbox->enable();

        testsound = new Sound3d(Media::getPath("engine.flac"), new TransformationManager(glm::vec3(0.0, 20.0, 0.0)));
        testsound->setLoop(true);
        testsound2 = new Sound3d(Media::getPath("brown_noise.flac"), new TransformationManager(glm::vec3(0.0, 20.0, 0.0)));
        testsound2->setLoop(true);
        wind = new Sound3d(Media::getPath("brown_noise.flac"), new TransformationManager(glm::vec3(0.0, 20.0, 0.0)));
        wind->setLoop(true);
        /*ocean1 = new Sound3d("ocean1.flac", new TransformationManager(glm::vec3(0.0, 20.0, 0.0)));
        ocean1->setLoop(true);
        ocean2 = new Sound3d("ocean2.flac", new TransformationManager(glm::vec3(0.0, 20.0, 0.0)));
        ocean2->setLoop(true);
        ocean3 = new Sound3d("ocean3.flac", new TransformationManager(glm::vec3(0.0, 20.0, 0.0)));
        ocean3->setLoop(true);
        ocean4 = new Sound3d("ocean4.flac", new TransformationManager(glm::vec3(0.0, 20.0, 0.0)));
        ocean4->setLoop(true);

        ocean1->setPitch(0.7f);
        ocean2->setPitch(0.7f);
        ocean3->setPitch(0.7f);
        ocean4->setPitch(0.7f);

        ocean1->play();
        ocean2->play();
        ocean3->play();
        ocean4->play();
        */
        //cursor3dArrow = Mesh3d::create(app->asset->loadObject3dInfoFile("deferredsphere.raw"), new Material());
        //cursor3dArrow->clearInstances();
        // cursor3dArrow->addInstance(new Mesh3dInstance(new TransformationManager(glm::vec3(0.0), glm::quat(), glm::vec3(7.0))));
        car.push_back(new Car(physics, "fiesta.car", new TransformationManager(glm::vec3(0.0, 5.0, 0.0))));
}

void EditorApp::switchMode(int mode)
{
    // EDITOR_MODE_MOVE_CAMERA needs cursor hidden, everything else needs free cursor
    if (mode == EDITOR_MODE_MOVE_CAMERA && currentMode != EDITOR_MODE_MOVE_CAMERA) {
        mouse->setCursorMode(GLFW_CURSOR_DISABLED);
    }
    else if (currentMode == EDITOR_MODE_MOVE_CAMERA && mode != EDITOR_MODE_MOVE_CAMERA) {
        mouse->setCursorMode(GLFW_CURSOR_NORMAL);
    }
    lastMode = currentMode;
    currentMode = mode;
}

void EditorApp::onChatSendText(std::string s)
{
    if (s[0] == '/') {
        std::string cmd = s.substr(1);
        if (cmd == "help") {
            chat->printMessage(UIColor(0.1, 1.0, 0.1, 1.0), "*** Welcome to the Tester project!");
            chat->printMessage(UIColor(0.1, 1.0, 0.1, 1.0), "*** Hope you enjoy.");
            return;
        }
        if (cmd == "memory") {
            for (auto typex : app->vulkan->memoryManager->allAllocationsByType) {
                auto chunks = app->vulkan->memoryManager->allAllocationsByType[typex.first];
                for (auto chunk : chunks) {
                    chat->printMessage(UIColor(0.1, 1.0, 1.0, 1.0), "* type(" + std::to_string(typex.first) + ") usage(" + std::to_string((float)chunk->allAllocationsSize / 1024.0f / 1024.0f) + " MB)");
                }
            }
            return;
        }
        if (cmd == "save") {
            auto pos = cam->transformation->getPosition();
            auto rot = cam->transformation->getOrientation();
            writeLog("position: [" + std::to_string(pos.x) + ", " + std::to_string(pos.y) + ", " + std::to_string(pos.z) + "]");
            writeLog("orientation: [" + std::to_string(rot.x) + ", " + std::to_string(rot.y) + ", " + std::to_string(rot.z) + ", " + std::to_string(rot.w) + "]");
            chat->printMessage(UIColor(0.1, 1.0, 0.1, 1.0), "*** Saved!");
            return;
        }
        if (cmd == "q") {
            app->shouldClose = true;
            return;
        }
        chat->printMessage(UIColor(1.0, 1.0, 0.0, 1.0), "* Invalid command.");
        return;
    }
    chat->printMessage(UIColor(0.5, 0.5, 0.5, 1.0), s);
}

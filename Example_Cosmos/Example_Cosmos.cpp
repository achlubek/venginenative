// Example_Cosmos.cpp: Okreœla punkt wejœcia dla aplikacji konsoli.
//

#include "stdafx.h"
#include "CosmosRenderer.h"
#include "GalaxyGenerator.h"
#include "SpaceShip.h"
#include "SpaceShipModule.h"
#include "SpaceShipEngine.h"


int main()
{
    Media::loadFileMap(".");
    INIReader* configreader = new INIReader("cosmos_example.ini");
    auto toolkit = new VulkanToolkit();
    toolkit->initialize(configreader->geti("window_width"), configreader->geti("window_height"));

    Media::loadFileMap("../../media");
    Media::loadFileMap("../../shaders");

    auto cosmosRenderer = new CosmosRenderer(toolkit, toolkit->windowWidth, toolkit->windowHeight);

    GalaxyGenerator* galaxy = cosmosRenderer->galaxy;
    //printf("gen");
    int64_t galaxyedge = 12490000000;
    int64_t galaxythickness = 24960000000;
    for (int i = 0; i < 10000; i++) {
        galaxy->generateStar(galaxyedge, galaxythickness, 1.0, i);
        cosmosRenderer->nearbyStars.push_back(galaxy->generateStarInfo(i));
    }
    // printf("sea");
    // GeneratedStarInfo starinfo = galaxy->generateStarInfo(galaxy->findClosestStar(1, 1, 1));

    auto text = new UIText(cosmosRenderer->ui, 0.01, 0.01, UIColor(1, 1, 1, 1), Media::getPath("font.ttf"), 16, " ");
    cosmosRenderer->ui->texts.push_back(text);

    auto camera = new Camera();
    camera->createProjectionPerspective(60.0f, (float)toolkit->windowWidth / (float)toolkit->windowHeight, 0.01f, 1000000);
   // glm::dvec3 observerPosition;

    volatile bool spaceShipMode = true;

    Mouse* mouse = new Mouse(toolkit->window);
    Keyboard* keyboard = new Keyboard(toolkit->window);
    Joystick* joystick = new Joystick(toolkit->window);
    float pitch = 0.0;
    float yaw = 0.0;
    int lastcx = 0, lastcy = 0;
    int frames = 0;
    double lastTime = 0.0;
    double lastRawTime = 0.0;
    float speedmultiplier = 1000.0;
    float stabilizerotation = 1.0;
    keyboard->onKeyPress.add([&](int key) {
        if (key == GLFW_KEY_ENTER) {
            //    spaceShipMode = !spaceShipMode;
        }

        if (key == GLFW_KEY_F1) {
            speedmultiplier = 0.05;
        }
        if (key == GLFW_KEY_F2) {
            speedmultiplier = 10.0;
        }
        if (key == GLFW_KEY_F3) {
            speedmultiplier = 100.0;
        }
        if (key == GLFW_KEY_F4) {
            speedmultiplier = 100000.0;
        }

        if (key == GLFW_KEY_F5) {
            stabilizerotation = 0.0;
        }
        if (key == GLFW_KEY_F6) {
            stabilizerotation = 0.25;
        }
        if (key == GLFW_KEY_F7) {
            stabilizerotation = 0.5;
        }
        if (key == GLFW_KEY_F8) {
            stabilizerotation = 1.0;
        }
        if (key == GLFW_KEY_PAUSE) {
            cosmosRenderer->recompileShaders(true);
        }
    });
    //glm::vec3 spaceshipLinearVelocity = glm::vec3(0.0);
    //glm::vec3 spaceshipAngularVelocity = glm::vec3(0.0);
    //glm::dvec3 spaceshipPosition = glm::dvec3(0.0);
    //glm::quat spaceshipOrientation = glm::quat(1.0, 0.0, 0.0, 0.0);
    //double maxFuel = 10000.0;
    //double fuel = maxFuel;
    SpaceShip* ship = new SpaceShip(glm::dvec3(0.0), glm::dquat(1.0, 0.0, 0.0, 0.0));

    SpaceShipEngine* forward_engine = new SpaceShipEngine(glm::dvec3(0.0, 0.0, -2.0), glm::dvec3(0.0, 0.0, 1.0), 10.0, 0.1);
    SpaceShipEngine* backward_engine = new SpaceShipEngine(glm::dvec3(0.0, 0.0, -2.0), glm::dvec3(0.0, 0.0, 1.0), 1.0, 0.01);

    /*
             rX |   / rY
                |  /
                | /
     rZ         |/        rZ
     -----------+-----------
               /|
              / |
             /  |
         rY /   | rX

         total 12 engines
         negXUP
         negXDOWN
         posXUP
         posXDOWN

         negYUP
         negYDOWN
         posYUP
         posYDOWN

         negZUP
         negZDOWN
         posZUP
         posZDOWN
    */
    SpaceShipEngine* negXUP =   new SpaceShipEngine(glm::dvec3(-1.0, 0.0, 0.0), glm::dvec3(0.0, 1.0, 0.0), 1.0, 0.1);
    SpaceShipEngine* negXDOWN = new SpaceShipEngine(glm::dvec3(-1.0, 0.0, 0.0), glm::dvec3(0.0, -1.0, 0.0), 1.0, 0.1);
    SpaceShipEngine* posXUP =   new SpaceShipEngine(glm::dvec3(1.0, 0.0, 0.0), glm::dvec3(0.0, 1.0, 0.0), 1.0, 0.1);
    SpaceShipEngine* posXDOWN = new SpaceShipEngine(glm::dvec3(1.0, 0.0, 0.0), glm::dvec3(0.0, -1.0, 0.0), 1.0, 0.1);

    SpaceShipEngine* negYFORWARD  = new SpaceShipEngine(glm::dvec3(0.0, -1.0, 0.0), glm::dvec3(0.0, 0.0, 1.0), 1.0, 0.1);
    SpaceShipEngine* negYBACKWARD = new SpaceShipEngine(glm::dvec3(0.0, -1.0, 0.0), glm::dvec3(0.0, 0.0, -1.0), 1.0, 0.1);
    SpaceShipEngine* posYFORWARD  = new SpaceShipEngine(glm::dvec3(0.0, 1.0, 0.0), glm::dvec3(0.0, 0.0, 1.0), 1.0, 0.1);
    SpaceShipEngine* posYBACKWARD = new SpaceShipEngine(glm::dvec3(0.0, 1.0, 0.0), glm::dvec3(0.0, 0.0, -1.0), 1.0, 0.1);

    SpaceShipEngine* negZLEFT =   new SpaceShipEngine(glm::dvec3(0.0, 0.0, -1.0), glm::dvec3(-1.0, 0.0, 0.0), 1.0, 0.1);
    SpaceShipEngine* negZRIGHT  = new SpaceShipEngine(glm::dvec3(0.0, 0.0, -1.0), glm::dvec3(1.0, 0.0, 0.0), 1.0, 0.1);
    SpaceShipEngine* posZLEFT =   new SpaceShipEngine(glm::dvec3(0.0, 0.0, 1.0), glm::dvec3(-1.0, 0.0, 0.0), 1.0, 0.1);
    SpaceShipEngine* posZRIGHT  = new SpaceShipEngine(glm::dvec3(0.0, 0.0, 1.0), glm::dvec3(1.0, 0.0, 0.0), 1.0, 0.1);

    ship->modules.push_back(forward_engine);

    for (int i = 0; i < ship->modules.size(); i++) {
        ship->modules[i]->enable();
    }

    cosmosRenderer->mapBuffers();
    cosmosRenderer->updateStars();
    std::thread background1 = std::thread([&]() {
        while (true) {
            cosmosRenderer->updateNearestStar(ship->getPosition());
            cosmosRenderer->updateGravity(ship->getPosition());

        }
    });
    background1.detach();
#define multiplyscale(a,b) (std::pow(a, b))
    while (!toolkit->shouldCloseWindow()) {
        frames++;
        double time = glfwGetTime();
        double nowtime = floor(time);
        if (nowtime != lastTime) {
            printf("FPS %d\n", frames);
            frames = 0;
        }
        float elapsed_x100 = (float)(100.0 * (time - lastRawTime));
        float elapsed = (float)((time - lastRawTime));
        lastRawTime = time;
        lastTime = nowtime;

        if (!spaceShipMode) {
            float speed = 1.1f * elapsed_x100;
            if (keyboard->getKeyStatus(GLFW_KEY_LEFT_CONTROL) == GLFW_PRESS) {
                speed *= 0.1f;
            }
            if (keyboard->getKeyStatus(GLFW_KEY_LEFT_ALT) == GLFW_PRESS) {
                speed *= 10;
            }
            if (keyboard->getKeyStatus(GLFW_KEY_LEFT_SHIFT) == GLFW_PRESS) {
                speed *= 730.1f;
            }
            glm::vec3 dw = glm::vec3(0);
            float w = 0.0;
            if (keyboard->getKeyStatus(GLFW_KEY_W) == GLFW_PRESS) {
                dw += camera->transformation->getOrientation() * glm::vec3(0, 0, -1);
                w += 1.0;
            }
            if (keyboard->getKeyStatus(GLFW_KEY_S) == GLFW_PRESS) {
                dw += camera->transformation->getOrientation() * glm::vec3(0, 0, 1);
                w += 1.0;
            }
            if (keyboard->getKeyStatus(GLFW_KEY_A) == GLFW_PRESS) {
                dw += camera->transformation->getOrientation() * glm::vec3(-1, 0, 0);
                w += 1.0;
            }
            if (keyboard->getKeyStatus(GLFW_KEY_D) == GLFW_PRESS) {
                dw += camera->transformation->getOrientation() * glm::vec3(1, 0, 0);
                w += 1.0;
            }
            glm::vec3 a = dw / w;
            glm::vec3 dir = w > 0.0 ? a : dw;
            glm::vec3 newpos = camera->transformation->getPosition();
            newpos += length(dir) > 0.0 ? (normalize(dir) * speed) : (glm::vec3(0.0));
            glm::dvec2 cursor;
            auto tup = mouse->getCursorPosition();
            cursor.x = get<0>(tup);
            cursor.y = get<1>(tup);

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
            camera->transformation->setOrientation(glm::slerp(newrot, camera->transformation->getOrientation(), 0.9f));
          //  observerPosition = glm::mix(newpos, camera->transformation->getPosition(), 0.8f);
        }
        else {
            ship->update(elapsed_x100);
            camera->transformation->setOrientation(ship->getOrientation());
             
            if (keyboard->getKeyStatus(GLFW_KEY_W) == GLFW_PRESS) {
                forward_engine->currentPowerPercentage = 1.0;
            }
            else {
                forward_engine->currentPowerPercentage = 0.0;
            }/*
            if (keyboard->getKeyStatus(GLFW_KEY_S) == GLFW_PRESS) {
                spaceshipLinearVelocity += speedmultiplier * elapsed_x100 * rotmat * glm::vec3(0, 0, 1);
            }
            if (keyboard->getKeyStatus(GLFW_KEY_R) == GLFW_PRESS) {
                spaceshipLinearVelocity += speedmultiplier * elapsed_x100 * rotmat * glm::vec3(0, 1, 0);
            }
            if (keyboard->getKeyStatus(GLFW_KEY_F) == GLFW_PRESS) {
                spaceshipLinearVelocity += speedmultiplier * elapsed_x100 * rotmat * glm::vec3(0, -1, 0);
            }
            if (keyboard->getKeyStatus(GLFW_KEY_Q) == GLFW_PRESS) {
                spaceshipLinearVelocity += speedmultiplier * elapsed_x100 * rotmat * glm::vec3(-1, 0, 0);
            }
            if (keyboard->getKeyStatus(GLFW_KEY_E) == GLFW_PRESS) {
                spaceshipLinearVelocity += speedmultiplier * elapsed_x100 * rotmat * glm::vec3(1, 0, 0);
            }
            if (keyboard->getKeyStatus(GLFW_KEY_SPACE) == GLFW_PRESS) {
                spaceshipLinearVelocity = glm::vec3(cosmosRenderer->closestObjectLinearAbsoluteSpeed * 0.01);
            }
            spaceshipAngularVelocity *= multiplyscale(1.0f - stabilizerotation * 0.1f, elapsed_x100);
            if (keyboard->getKeyStatus(GLFW_KEY_A) == GLFW_PRESS) {
                spaceshipAngularVelocity.z += elapsed_x100 * 0.0005;
            }
            if (keyboard->getKeyStatus(GLFW_KEY_D) == GLFW_PRESS) {
                spaceshipAngularVelocity.z -= elapsed_x100 * 0.0005;
            }
            if (keyboard->getKeyStatus(GLFW_KEY_U) == GLFW_PRESS) {
                spaceshipAngularVelocity.x -= elapsed_x100 * 0.0005;
            }
            if (keyboard->getKeyStatus(GLFW_KEY_J) == GLFW_PRESS) {
                spaceshipAngularVelocity.x += elapsed_x100 * 0.0005;
            }
            if (keyboard->getKeyStatus(GLFW_KEY_H) == GLFW_PRESS) {
                spaceshipAngularVelocity.y += elapsed_x100 * 0.0005;
            }
            if (keyboard->getKeyStatus(GLFW_KEY_K) == GLFW_PRESS) {
                spaceshipAngularVelocity.y -= elapsed_x100 * 0.0005;
            }*/
            /*
            if (joystick->isPresent(0)) {
                auto axes = joystick->getAxes(0);
                spaceshipLinearVelocity += 70.0f * ((-axes[3])) * elapsed_x100 * rotmat * glm::vec3(0, 0, -1);
                spaceshipAngularVelocity.x += elapsed_x100 * 0.001 * -axes[1];
                spaceshipAngularVelocity.y += elapsed_x100 * 0.001 * -axes[2];
                spaceshipAngularVelocity.z += elapsed_x100 * 0.001 * -axes[0];
            }*/

        }
        cosmosRenderer->updateCameraBuffer(camera, ship->getPosition());
        cosmosRenderer->updatePlanetsAndMoon(ship->getPosition());
        cosmosRenderer->draw();
        text->updateText("Distance to surface:" + std::to_string(cosmosRenderer->closestSurfaceDistance) + "| " + std::to_string(cosmosRenderer->closestObjectLinearAbsoluteSpeed.x)
            + " , " + std::to_string(cosmosRenderer->closestObjectLinearAbsoluteSpeed.y) + " , " + std::to_string(cosmosRenderer->closestObjectLinearAbsoluteSpeed.z));
        toolkit->poolEvents();
    }

    cosmosRenderer->unmapBuffers();
    return 0;
}


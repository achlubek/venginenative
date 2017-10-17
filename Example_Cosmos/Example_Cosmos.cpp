// Example_Cosmos.cpp: Okreœla punkt wejœcia dla aplikacji konsoli.
//

#include "stdafx.h"
#include "CosmosRenderer.h"


int main()
{
    auto toolkit = new VulkanToolkit();
    toolkit->initialize(1920, 1000);

    auto cosmosRenderer = new CosmosRenderer(toolkit, toolkit->windowWidth, toolkit->windowHeight);

    auto camera = new Camera();
    camera->createProjectionPerspective(40.0f, (float)toolkit->windowWidth / (float)toolkit->windowHeight, 0.01f, 10000);

    StarInfo centralStar = StarInfo();
    centralStar.position = glm::dvec3(10.0, 10.0, 10.0);
    centralStar.size = 2.0;
    centralStar.noiseSeed = 123.0;
    cosmosRenderer->stars.push_back(centralStar);

    PlanetInfo planet1 = PlanetInfo();
    planet1.position = glm::dvec3(1.0, 3.0, 4.0);
    planet1.size = 1.0;
    planet1.noiseSeed = 123.0;
    planet1.starDistance = 10.0;
    cosmosRenderer->planets.push_back(planet1);

    PlanetInfo planet2 = PlanetInfo();
    planet2.position = glm::dvec3(5.0, 3.0, 4.0);
    planet2.size = 1.0;
    planet2.noiseSeed = 1123.0;
    planet2.starDistance = 10.0;
    cosmosRenderer->planets.push_back(planet2);

    PlanetInfo planet3 = PlanetInfo();
    planet3.position = glm::dvec3(1.0, -3.0, 6.0);
    planet3.size = 1.0;
    planet3.noiseSeed = 223.0;
    planet3.starDistance = 10.0;
    cosmosRenderer->planets.push_back(planet3);

    Mouse* mouse = new Mouse(toolkit->window);
    Keyboard* keyboard = new Keyboard(toolkit->window);
    float pitch = 0.0;
    float yaw = 0.0;
    int lastcx = 0, lastcy = 0;
    int frames = 0;
    double lastTime = 0.0;
    while (!toolkit->shouldCloseWindow()) {
        frames++;
        double nowtime = floor(glfwGetTime());
        if (nowtime != lastTime) {
            printf("FPS %d\n", frames);
            frames = 0;
        }
        lastTime = nowtime;


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
        //dir = mix(dir, w > 0.0 ? a : dw, 0.02);
        glm::vec3 dir = w > 0.0 ? a : dw;
        glm::vec3 newpos = camera->transformation->getPosition();
        newpos += length(dir) > 0.0 ? (normalize(dir) * speed) : (glm::vec3(0.0));
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
        camera->transformation->setOrientation(glm::slerp(newrot, camera->transformation->getOrientation(), 0.9f));
        camera->transformation->setPosition(glm::mix(newpos, camera->transformation->getPosition(), 0.8f));

        cosmosRenderer->updateCameraBuffer(camera);
        cosmosRenderer->updateClosestPlanetsAndStar(camera);
        cosmosRenderer->updateObjectsBuffers(camera);
        cosmosRenderer->draw();
        toolkit->poolEvents();
    }

    return 0;
}


// Example_Cosmos.cpp: Okre�la punkt wej�cia dla aplikacji konsoli.
//

#include "stdafx.h"
#include "CosmosRenderer.h"
#include "GalaxyGenerator.h"


int main()
{
    auto toolkit = new VulkanToolkit();
    toolkit->initialize(1920, 1080);

    Media::loadFileMap("../../media");
    Media::loadFileMap("../../shaders");

    auto cosmosRenderer = new CosmosRenderer(toolkit, toolkit->windowWidth, toolkit->windowHeight);

    GalaxyGenerator* galaxy = cosmosRenderer->galaxy;
    printf("gen");
    int64_t galaxyedge = 24960000000;
    int64_t galaxythickness = 2496000000;
    for (int i = 0; i < 1000000; i++) {
        galaxy->generateStar(galaxyedge, galaxythickness, 1.0, i);
        cosmosRenderer->nearbyStars.push_back(galaxy->generateStarInfo(i));
    }
    printf("sea");
    GeneratedStarInfo starinfo = galaxy->generateStarInfo(galaxy->findClosestStar(1, 1, 1));



    auto camera = new Camera();
    camera->createProjectionPerspective(40.0f, (float)toolkit->windowWidth / (float)toolkit->windowHeight, 0.01f, 1000000);

    volatile bool spaceShipMode = false;

    Mouse* mouse = new Mouse(toolkit->window);
    Keyboard* keyboard = new Keyboard(toolkit->window);
    Joystick* joystick = new Joystick(toolkit->window);
    float pitch = 0.0;
    float yaw = 0.0;
    int lastcx = 0, lastcy = 0;
    int frames = 0;
    double lastTime = 0.0;
    double lastRawTime = 0.0;
    cosmosRenderer->mapBuffers();
    cosmosRenderer->updateStars(camera);
    std::thread background1 = std::thread([&]() {
        while (true) {
            cosmosRenderer->updateNearestStar(camera);
            cosmosRenderer->updateGravity(camera);

        }
    });
    background1.detach();
    std::thread background2 = std::thread([&]() {
        while (true) {
            cosmosRenderer->updatePlanetsAndMoon(camera);
        }
    });
    background2.detach();
    keyboard->onKeyPress.add([&](int key) {
        if (key == GLFW_KEY_ENTER) {
            spaceShipMode = !spaceShipMode;
        }
    });
    glm::vec3 spaceshipLinearVelocity = glm::vec3(0.0);
    glm::vec3 spaceshipAngularVelocity = glm::vec3(0.0);
    glm::vec3 spaceshipPosition = glm::vec3(0.0);
    glm::quat spaceshipOrientation = glm::quat(1.0, 0.0, 0.0, 0.0);
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
            camera->transformation->setPosition(glm::mix(newpos, camera->transformation->getPosition(), 0.8f));
        }
        else {
            spaceshipPosition += elapsed_x100 * spaceshipLinearVelocity;
            spaceshipPosition += elapsed * cosmosRenderer->lastGravity;
            glm::mat3 rotmat = glm::mat3_cast(spaceshipOrientation);
            spaceshipOrientation = spaceshipOrientation
                * glm::angleAxis(elapsed_x100 * spaceshipAngularVelocity.x,glm::vec3(1.0, 0.0, 0.0))
                * glm::angleAxis(elapsed_x100 * spaceshipAngularVelocity.y,glm::vec3(0.0, 1.0, 0.0))
                * glm::angleAxis(elapsed_x100 * spaceshipAngularVelocity.z,glm::vec3(0.0, 0.0, 1.0))
            ;
            camera->transformation->setPosition(spaceshipPosition);
            camera->transformation->setOrientation(spaceshipOrientation);
            if (keyboard->getKeyStatus(GLFW_KEY_W) == GLFW_PRESS) {
                spaceshipLinearVelocity += 10.0f * elapsed_x100 * rotmat * glm::vec3(0, 0, -1);
            }
            if (keyboard->getKeyStatus(GLFW_KEY_S) == GLFW_PRESS) {
                spaceshipLinearVelocity += 10.0f * elapsed_x100 * rotmat * glm::vec3(0, 0, 1);
            }
            if (keyboard->getKeyStatus(GLFW_KEY_SPACE) == GLFW_PRESS) {
                spaceshipLinearVelocity *= 0.99f;
            }
            spaceshipAngularVelocity *= 0.99f;
            if (keyboard->getKeyStatus(GLFW_KEY_A) == GLFW_PRESS) {
                spaceshipAngularVelocity.z += elapsed_x100 * 0.0001;
            }
            if (keyboard->getKeyStatus(GLFW_KEY_D) == GLFW_PRESS) {
                spaceshipAngularVelocity.z -= elapsed_x100 * 0.0001;
            }
            if (keyboard->getKeyStatus(GLFW_KEY_U) == GLFW_PRESS) {
                spaceshipAngularVelocity.x -= elapsed_x100 * 0.0001;
            }
            if (keyboard->getKeyStatus(GLFW_KEY_J) == GLFW_PRESS) {
                spaceshipAngularVelocity.x += elapsed_x100 * 0.0001;
            }
            if (keyboard->getKeyStatus(GLFW_KEY_H) == GLFW_PRESS) {
                spaceshipAngularVelocity.y += elapsed_x100 * 0.0001;
            }
            if (keyboard->getKeyStatus(GLFW_KEY_K) == GLFW_PRESS) {
                spaceshipAngularVelocity.y -= elapsed_x100 * 0.0001;
            }
            if (joystick->isPresent(0)) {
                auto axes = joystick->getAxes(0);
                spaceshipLinearVelocity += 70.0f * ((-axes[3]) * 0.5f + 0.5f) * elapsed_x100 * rotmat * glm::vec3(0, 0, -1);
                spaceshipAngularVelocity.x += elapsed_x100 * 0.001 * axes[1];
                spaceshipAngularVelocity.y += elapsed_x100 * 0.001 * -axes[2];
                spaceshipAngularVelocity.z += elapsed_x100 * 0.001 * -axes[0];
            }

        }
        cosmosRenderer->updateCameraBuffer(camera);
        cosmosRenderer->draw();
        toolkit->poolEvents();
    }

    cosmosRenderer->unmapBuffers();
    return 0;
}


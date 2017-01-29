#include "stdafx.h"
#include "EditorApp.h"


EditorApp::EditorApp()
{
}


EditorApp::~EditorApp()
{
}

void EditorApp::initialize()
{
    Media::loadFileMap("../../media");
    Media::loadFileMap("../../shaders");
    width = 1920;
    height = 1020;
}

void EditorApp::onRenderFrame(float elapsed)
{
    if (!cursorFree) {
        float speed = 0.1f;
        if (game->getKeyStatus(GLFW_KEY_LEFT_CONTROL) == GLFW_PRESS) {
            speed *= 0.1f;
        }
        if (game->getKeyStatus(GLFW_KEY_LEFT_ALT) == GLFW_PRESS) {
            speed *= 10;
        }
        if (game->getKeyStatus(GLFW_KEY_LEFT_SHIFT) == GLFW_PRESS) {
            speed *= 30.1f;
        }/*
         if (game->getKeyStatus(GLFW_KEY_F1) == GLFW_PRESS) {
         light->transformation->position = cam->transformation->position;
         light->transformation->orientation = cam->transformation->orientation;
         }*/
        glm::vec3 dw = glm::vec3(0);
        float w = 0.0;
        if (game->getKeyStatus(GLFW_KEY_W) == GLFW_PRESS) {
            dw += cam->transformation->orientation * glm::vec3(0, 0, -1);
            w += 1.0;
        }
        if (game->getKeyStatus(GLFW_KEY_S) == GLFW_PRESS) {
            dw += cam->transformation->orientation * glm::vec3(0, 0, 1);
            w += 1.0;
        }
        if (game->getKeyStatus(GLFW_KEY_A) == GLFW_PRESS) {
            dw += cam->transformation->orientation * glm::vec3(-1, 0, 0);
            w += 1.0;
        }
        if (game->getKeyStatus(GLFW_KEY_D) == GLFW_PRESS) {
            dw += cam->transformation->orientation * glm::vec3(1, 0, 0);
            w += 1.0;
        }

        glm::vec3 a = dw / w;
        //dir = mix(dir, w > 0.0 ? a : dw, 0.02);
        glm::vec3 dir = w > 0.0 ? a : dw;
        glm::vec3 newpos = cam->transformation->position;
        newpos += length(dir) > 0.0 ? (normalize(dir) * speed) : (glm::vec3(0.0));

        if (game->getKeyStatus(GLFW_KEY_ESCAPE) == GLFW_PRESS) {
            game->shouldClose = true;
        }
        glm::dvec2 cursor = game->getCursorPosition();

        int acnt = 0;
        const float * axes = glfwGetJoystickAxes(0, &acnt);
        float dx = (float)(lastcx - cursor.x);
        float dy = (float)(lastcy - cursor.y);
        if (acnt >= 4) {
            //    dx -= axes[2] * 10.9;
            //    dy += axes[3] * 10.9;
            //    newpos += (cam->transformation->orientation * glm::vec3(0, 0, -1) * axes[1] * 0.1f);
            //   newpos += (cam->transformation->orientation * glm::vec3(1, 0, 0) * axes[0] * 0.1f);
        }
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
        cam->transformation->setPosition(newpos);
    }
}

void EditorApp::onRenderUIFrame(float elapsed)
{
}

void EditorApp::onWindowResize(int width, int height)
{
}

void EditorApp::onKeyPress(int key)
{
}

void EditorApp::onKeyRelease(int key)
{
}

void EditorApp::onKeyRepeat(int key)
{
}

void EditorApp::onBind()
{
    cam = new Camera();
    float fov = 85.0f;
    float fovnew = 85.0f;
    cam->createProjectionPerspective(fovnew, (float)game->width / (float)game->height, 0.01f, 1000);
    game->onWindowResize->add([&](int zero) {
        cam->createProjectionPerspective(fovnew, (float)game->width / (float)game->height, 0.01f, 1000);
    });
    cam->transformation->translate(glm::vec3(16, 16, 16));
    glm::quat rot = glm::quat_cast(glm::lookAt(cam->transformation->position, glm::vec3(0), glm::vec3(0, 1, 0)));
    cam->transformation->setOrientation(rot);
    game->world->mainDisplayCamera = cam;

    game->setCursorMode(GLFW_CURSOR_DISABLED);
}

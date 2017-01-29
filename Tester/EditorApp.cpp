#include "stdafx.h"
#include "EditorApp.h"


EditorApp::EditorApp()
{
}


EditorApp::~EditorApp()
{
}

void EditorApp::switchMode(int mode)
{
    // EDITOR_MODE_MOVE_CAMERA needs cursor hidden, everything else needs free cursor
    if (mode == EDITOR_MODE_MOVE_CAMERA && currentMode != EDITOR_MODE_MOVE_CAMERA) {
        game->setCursorMode(GLFW_CURSOR_DISABLED);
    }
    else if (currentMode == EDITOR_MODE_MOVE_CAMERA && mode != EDITOR_MODE_MOVE_CAMERA) {
        game->setCursorMode(GLFW_CURSOR_NORMAL);
    }
    lastMode = currentMode;
    currentMode = mode;
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
    if (currentMode == EDITOR_MODE_MOVE_CAMERA) {
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

        //if (game->getKeyStatus(GLFW_KEY_ESCAPE) == GLFW_PRESS) {
        //    game->shouldClose = true;
        //}
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
    else if (currentMode == EDITOR_MODE_PICKING) {
        glm::vec2 cursor = game->getCursorPosition();
        cursor.x = max(0.0f, cursor.x);
        cursor.y = max(0.0f, cursor.y);
        cursor.x = min((float)width, cursor.x);
        cursor.y = min((float)height, cursor.y);
        cursor.x /= (float)width;
        cursor.y /= (float)height;
        cursor.y = 1.0 - cursor.y;
        game->renderer->pick(cam, cursor);
    }
    else if (currentMode == EDITOR_MODE_EDITING) {

    }
    else if (currentMode == EDITOR_MODE_WRITING_TEXT) {

    }
    if (isConsoleWindowOpened == false && currentMode == EDITOR_MODE_WRITING_TEXT) {
        switchMode(EDITOR_MODE_MOVE_CAMERA);
    }
}

void EditorApp::onRenderUIFrame(float elapsed)
{
    if (currentMode == EDITOR_MODE_WRITING_TEXT) {
        ImGui::Begin("CMD", &isConsoleWindowOpened, 0);
        ImGui::Text(currentCommandText.c_str());
        ImGui::End();
    }
    else if (currentMode == EDITOR_MODE_PICKING) {
        if (game->renderer->pickingResult > 0) {

            Mesh3d * m = nullptr;
            auto meshes = game->world->scene->getMeshes();
            for (int i = 0; i < meshes.size(); i++) {
                if (meshes[i]->Id == game->renderer->pickingResult) {
                    m = meshes[i];
                    break;
                }
            }
            if (m != nullptr) {
                auto lod0 = m->getLodLevel(0);
                ImGui::Begin("PickingResult", &isPickingWindowOpened, 0);
                ImGui::Text(m->name.c_str());
                ImGui::End();

            }
        }
    }
}

void EditorApp::onWindowResize(int width, int height)
{
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

void EditorApp::onKeyPress(int key)
{
    if (key == GLFW_KEY_F1) {
        switchMode(EDITOR_MODE_MOVE_CAMERA);
        return;
    }
    else if (key == GLFW_KEY_F2) {
        switchMode(EDITOR_MODE_PICKING);
        isPickingWindowOpened = true;
        return;
    }
    else if (key == GLFW_KEY_F3) {
        switchMode(EDITOR_MODE_EDITING);
        return;
    }
    else if (key == GLFW_KEY_T && currentMode != EDITOR_MODE_WRITING_TEXT) {
        switchMode(EDITOR_MODE_WRITING_TEXT);
        currentCommandText = "";
        isConsoleWindowOpened = true;
        ignoreNextChar = true;
        return;
    }

    if (currentMode == EDITOR_MODE_WRITING_TEXT) {
        if (key == GLFW_KEY_ENTER) {
            onCommandText(currentCommandText);
            switchMode(lastMode);
            ignoreNextChar = true;
        }
        else if (key == GLFW_KEY_BACKSPACE) {
            if (currentCommandText.length() > 0) currentCommandText = currentCommandText.substr(0, currentCommandText.length() - 1);
        }
    }
}

void EditorApp::onKeyRelease(int key)
{
}

void EditorApp::onKeyRepeat(int key)
{
    if (currentMode == EDITOR_MODE_WRITING_TEXT) {
        if (key == GLFW_KEY_BACKSPACE) {
            if (currentCommandText.length() > 0) currentCommandText = currentCommandText.substr(0, currentCommandText.length() - 1);
        }
    }
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

    game->setCursorMode(GLFW_CURSOR_NORMAL);

    auto t = game->asset->loadMeshFile("2dplane.mesh3d");
    t->getLodLevel(0)->info3d->drawMode = GL_TRIANGLE_STRIP;
    t->name = "2dplane";
    game->world->scene->addMesh(t);
}

void EditorApp::onChar(unsigned int c)
{
    if (ignoreNextChar) {
        ignoreNextChar = false;
        return;
    }
    if (currentMode == EDITOR_MODE_WRITING_TEXT) {
        currentCommandText += c;
    }
}

void EditorApp::onCommandText(string text)
{
    auto args = split(text, ' ');
    if (args[0] == "create") {
        string s = join(args, 1);
        auto m = game->asset->loadMeshFile(s);
        m->name = s;
        game->world->scene->addMesh(m);
    }
}

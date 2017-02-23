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
        if (pickedUpMeshInstance != nullptr) {
            int acnt = 0;
            const float * axes = glfwGetJoystickAxes(0, &acnt);
            if (acnt >= 5) {
                //    dx -= axes[2] * 10.9;
                //    dy += axes[3] * 10.9;
                //    newpos += (cam->transformation->orientation * glm::vec3(0, 0, -1) * axes[1] * 0.1f);
                //   newpos += (cam->transformation->orientation * glm::vec3(1, 0, 0) * axes[0] * 0.1f);
                pickedUpMeshInstance->transformation->translate(glm::vec3(axes[0], axes[2], axes[1]) * 0.01f);
            }
        }
    }
    else if (currentMode == EDITOR_MODE_WRITING_TEXT) {

    }
    if (isConsoleWindowOpened == false && currentMode == EDITOR_MODE_WRITING_TEXT) {
        switchMode(lastMode);
    }
}

string vitoa(int i) {
    auto ss = stringstream();
    ss << i;
    return ss.str();
}

void EditorApp::onRenderUIFrame(float elapsed)
{
    ImGui::Begin("Clouds", &isOpened, 0);
    ImGui::Text("%.3f ms/frame %.1f FPS", 1000.0f / ImGui::GetIO().Framerate, ImGui::GetIO().Framerate);
    //    ImGui::Text("Terrain roughness:");
    //     ImGui::SliderFloat("roughness", &t->getLodLevel(0)->material->roughness, 0.0f, 1.0f);
    //     ImGui::Text("Terrain metalness:");
    //     ImGui::SliderFloat("metalness", &t->getLodLevel(0)->material->metalness, 0.0f, 1.0f);
    //ImGui::SliderFloat("FOV", &fovnew, 5.0f, 175.0f);
    #define reset() Game::instance->renderer->cloudsIntegrate = 0.0f;
    if(ImGui::SliderFloat("Exposure", &Game::instance->renderer->exposure, 0.01f, 10.0f))reset();
    if(ImGui::SliderFloat("Contrast", &Game::instance->renderer->contrast, 0.01f, 10.0f))reset();
    if(ImGui::SliderFloat("CloudsHeightStart", &Game::instance->renderer->cloudsFloor, 100.0f, 30000.0f))reset();
    if(ImGui::SliderFloat("CloudsHeightEnd", &Game::instance->renderer->cloudsCeil, 100.0f, 30000.0f))reset();
    if(ImGui::SliderFloat("CloudsThreshold", &Game::instance->renderer->cloudsThresholdLow, 0.0f, 1.0f))reset();
    if(ImGui::SliderFloat("CloudsThresholdHigh", &Game::instance->renderer->cloudsThresholdHigh, 0.0f, 1.0f))reset();
    //ImGui::SliderFloat("CloudsAtmosphereShaftsMultiplier", &Game::instance->renderer->cloudsAtmosphereShaftsMultiplier, 0.0f, 10.0f);
    //ImGui::SliderFloat("CloudsWindSpeed", &Game::instance->renderer->cloudsWindSpeed, 0.0f, 10.0f);
    if(ImGui::SliderFloat("CloudsDensity", &Game::instance->renderer->cloudsDensityScale, 0.0f, 5.0f))reset();
    if(ImGui::SliderFloat("CloudsIntegration", &Game::instance->renderer->cloudsIntegrate, 0.3f, 0.999f))reset();

    Game::instance->renderer->cloudsIntegrate = Game::instance->renderer->cloudsIntegrate * 0.997 + (0.99 * 0.003);
    Game::instance->renderer->cloudsIntegrate /= abs(Game::instance->renderer->dayElapsed - tmpDayElapsed) * 0.2 + 1.0;

    if (ImGui::SliderFloat3("CloudsOffset", (float*)&Game::instance->renderer->cloudsOffset, -1000.0f, 1000.0f))reset();
    ImGui::Separator();

  //  tmpDayElapsed += 0.00003f;
    if (tmpDayElapsed > 1.0) {
        tmpDayElapsed -= 1.0;
        Game::instance->renderer->dayElapsed = tmpDayElapsed;
    }
    
    if (ImGui::SliderFloat("DayElapsed", (float*)&tmpDayElapsed, 0.0f, 1.0f))reset();
    Game::instance->renderer->dayElapsed = Game::instance->renderer->dayElapsed * 0.99 + tmpDayElapsed * 0.01;
    if (ImGui::SliderFloat("YearElapsed", (float*)&Game::instance->renderer->yearElapsed, 0.0f, 1.0f))reset();
    if (ImGui::SliderFloat("Equator/Pole", (float*)&Game::instance->renderer->equatorPoleMix, 0.0f, 1.0f))reset();

   if(ImGui::SliderFloat("MieScattering", (float*)&Game::instance->renderer->mieScattCoefficent, 0.0f, 20.0f))reset();
   if(ImGui::SliderFloat("GodRaysStrength", &Game::instance->renderer->noiseOctave1, 0.01f, 10.0f))reset();
   ImGui::Separator();
   if(ImGui::SliderFloat("WaterWavesHeight", &Game::instance->renderer->waterWavesScale, 0.0f, 10.0f))reset();
   if(ImGui::SliderFloat("WaterSpeed", &Game::instance->renderer->waterSpeed, 0.0f, 7.0f))reset();
   if(ImGui::SliderFloat2("WaterScale", &Game::instance->renderer->waterScale.x, 0.0f, 10.0f))reset();
    //        ImGui::SliderFloat2("MeshRoughness", &t->getLodLevel(0)->material->roughness, 0.0f, 1.0f);
    //ImGui::SliderFloat("CloudsDensityThresholdLow", &Game::instance->renderer->cloudsDensityThresholdLow, 0.0f, 1.0f);
    // ImGui::SliderFloat("CloudsDensityThresholdHigh", &Game::instance->renderer->cloudsDensityThresholdHigh, 0.0f, 1.0f);
    //ImGui::SliderFloat("AtmosphereScale", &Game::instance->renderer->atmosphereScale, 0.0f, 1000.0f);
    // ImGui::SliderFloat("Noise2", &Game::instance->renderer->noiseOctave2, 0.01f, 10.0f);
    // ImGui::SliderFloat("Noise3", &Game::instance->renderer->noiseOctave3, 0.01f, 10.0f);
    //  ImGui::SliderFloat("Noise4", &Game::instance->renderer->noiseOctave4, 0.01f, 10.0f);
    //  ImGui::SliderFloat("Noise5", &Game::instance->renderer->noiseOctave5, 0.01f, 400.0f);
    //  ImGui::SliderFloat("Noise6", &Game::instance->renderer->noiseOctave6, 0.01f, 10.0f);
    //  ImGui::SliderFloat("Noise7", &Game::instance->renderer->noiseOctave7, 0.01f, 10.0f);
    //  ImGui::SliderFloat("Noise8", &Game::instance->renderer->noiseOctave8, 0.01f, 10.0f);
    ImGui::SliderFloat("LensBlurAmount", &Game::instance->renderer->lensBlurSize, 0.01f, 10.0f);
    ImGui::SliderFloat("FocalLength", &cam->focalLength, 0.01f, 10.0f);
    //ImGui::SliderFloat("Noise8", &Game::instance->renderer->noiseOctave8, 0.01f, 10.0f);
    //        ImGui::SliderFloat("RoughnessTerra", &game->world->scene->getMeshes()[0]->getLodLevel(0)->material->roughness, 0.01f, 10.0f);
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
    ImGui::End();
    if (currentMode == EDITOR_MODE_WRITING_TEXT) {
        ImGui::Begin("CMD", &isConsoleWindowOpened, 0);
        ImGui::Text(currentCommandText.c_str());
        ImGui::End();
    }
    else if (currentMode == EDITOR_MODE_PICKING) {
        if (game->renderer->pickingResultMesh > 0) {

            pickedUpMesh = (Mesh3d *)game->getObjectById(game->renderer->pickingResultMesh);
            pickedUpMeshLodLevel = (Mesh3dLodLevel *)game->getObjectById(game->renderer->pickingResultLod);
            pickedUpMeshInstance = (Mesh3dInstance *)game->getObjectById(game->renderer->pickingResultInstance);

         //   cursor3dArrow->getInstance(0)->transformation->setPosition(game->renderer->pickingWorldPos);
         //   cursor3dArrow->getInstance(0)->transformation->setOrientation(glm::lookAt(glm::vec3(0.0), game->renderer->pickingNormal, glm::vec3(0.0, 1.0, 0.0)));
            game->renderer->selectionPosition = game->renderer->pickingWorldPos;
            game->renderer->selectionOrientation = glm::quat(glm::lookAt(glm::vec3(0.0), game->renderer->pickingNormal, game->renderer->pickingNormal == glm::vec3(0.0, 1.0, 0.0) ? glm::vec3(0.0, 0.0, 1.0) : glm::vec3(0.0, 1.0, 0.0)));
            game->renderer->showSelection = true;
            ImGui::Begin("PickingResult", &isPickingWindowOpened, 0);
            ImGui::Text(pickedUpMesh->name.c_str());
            ImGui::End();
                 
        }
    }
    if (pickedUpMesh != nullptr) {
        int win = 0;
        ImGui::Begin("Selected mesh", &customWindowsOpened[win++], 0);
        ImGui::Text(pickedUpMesh->name.c_str());
        //ImGui::Text(vitoa(pickedUpMesh->id).c_str());
        ImGui::End();

        ImGui::Begin("Selected mesh lod level", &customWindowsOpened[win++], 0);
        ImGui::SliderFloat("Distance start", &pickedUpMeshLodLevel->distanceStart, 0.01, 9999.0);
        ImGui::SliderFloat("Distance end", &pickedUpMeshLodLevel->distanceEnd, 0.01, 9999.0);
        ImGui::SliderFloat3("Static diffuse color", &pickedUpMeshLodLevel->material->diffuseColor.x, 0.0, 1.0);
        ImGui::SliderFloat("Static metalness", &pickedUpMeshLodLevel->material->metalness, 0.0, 1.0);
        ImGui::SliderFloat("Static roughness", &pickedUpMeshLodLevel->material->roughness, 0.0, 1.0);
        ImGui::SliderFloat2("Diffuse tex scale", &pickedUpMeshLodLevel->material->diffuseColorTexScale.x, 0.01, 100.0);
        ImGui::SliderFloat2("Normal tex scale", &pickedUpMeshLodLevel->material->normalTexScale.x, 0.01, 100.0);
        ImGui::SliderFloat2("Metalness tex scale", &pickedUpMeshLodLevel->material->metalnessTexScale.x, 0.01, 100.0);
        ImGui::SliderFloat2("Roughness tex scale", &pickedUpMeshLodLevel->material->roughnessTexScale.x, 0.01, 100.0);
        ImGui::SliderFloat2("Bump tex scale", &pickedUpMeshLodLevel->material->bumpTexScale.x, 0.01, 100.0);
        ImGui::End();

        ImGui::Begin("Selected mesh instance", &customWindowsOpened[win++], 0);
        TransformStruct tmp0 = pickedUpMeshInstance->transformation->getStruct();
        auto euler = glm::eulerAngles(tmp0.orientation);
        ImGui::SliderFloat3("Position", &tmp0.position.x, -1000.0, 1000.0);
        ImGui::SliderFloat3("Orientation", &euler.x, -3.1415, 3.1415);
        ImGui::SliderFloat3("Size", &tmp0.size.x, -10.0, 10.0);
        tmp0.orientation = glm::quat(euler);
        pickedUpMeshInstance->transformation->setPosition(tmp0.position);
        pickedUpMeshInstance->transformation->setOrientation(tmp0.orientation);
        pickedUpMeshInstance->transformation->setSize(tmp0.size);
        ImGui::End();
    }
    if (currentMode != EDITOR_MODE_PICKING) game->renderer->showSelection = false;
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
    else if (key == GLFW_KEY_PAUSE) {
        game->renderer->recompileShaders();
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
    glm::quat rot = glm::quat_cast(glm::lookAt(cam->transformation->getPosition(), glm::vec3(0), glm::vec3(0, 1, 0)));
    cam->transformation->setOrientation(rot);
    game->world->mainDisplayCamera = cam;

    game->setCursorMode(GLFW_CURSOR_NORMAL);

  /*  auto t = game->asset->loadSceneFile("cryteksponza.scene");
    for (int i = 0; i < t->getMesh3ds().size(); i++) {
        t->getMesh3ds()[i]->getInstance(0)->transformation->translate(glm::vec3(0.0, 10.0, 0.0));
        game->world->scene->addMesh3d(t->getMesh3ds()[i]);

    }*/
    
    auto s = game->asset->loadMeshFile("grass_base.mesh3d");
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
    game->world->scene->addMesh3d(s);
  //  t->name = "flagbase";
   // game->world->scene->addMesh(t);

    cursor3dArrow = Mesh3d::create(game->asset->loadObject3dInfoFile("arrow.raw"), new Material());
    cursor3dArrow->addInstance(new Mesh3dInstance(new TransformationManager()));
   // game->world->scene->addDrawable((AbsDrawable*)cursor3dArrow);
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
        game->world->scene->addMesh3d(m);
    }
}

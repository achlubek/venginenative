#include "stdafx.h"
#include "EditorApp.h"
#include "Car.h"
#include "LinearMath/btAlignedObjectArray.h"
#include "BulletCollision/CollisionShapes/btHeightfieldTerrainShape.h"

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

glm::vec3 quat2vec3(glm::quat q) {
    return q * glm::vec3(0.0, 0.0, -1.0);
}

void EditorApp::onRenderFrame(float elapsed)
{

    mrstick->getInstance(0)->transformation->setPosition(bonesBodies[0]->transformation->getPosition());
    auto relpos = bonesBodies[0]->transformation->getPosition();
    for (int i = 0; i < bonesBodies.size(); i++) {

        auto wt = bonesBodies[i]->getTransformationManager()->getWorldTransform();
        mrstick->getLodLevel(0)->skeletonPose->pose[i] = glm::translate(glm::mat4(), bonesBodies[i]->getTransformationManager()->getPosition() - relpos)
            //    * glm::translate(glm::mat4(), bonesBinds[i])
            * glm::mat4_cast(bonesBodies[i]->getTransformationManager()->getOrientation())
            * glm::translate(glm::mat4(), -mrstick->getLodLevel(0)->skeleton->positions[i]);

        mrstick->getLodLevel(0)->skeleton->poseNeedsUpdate = true;

        auto we = mrstick->getLodLevel(0)->skeletonPose->pose[i] * glm::vec4(mrstick->getLodLevel(0)->skeleton->positions[i], 1.0);
        // cursor3dArrow->getInstance(i)->transformation->setPosition(glm::vec3(we.x, we.y, we.z));
    }

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
            glm::dvec2 cursor = game->getCursorPosition();

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
            cam->transformation->setOrientation(newrot);
            cam->transformation->setPosition(newpos);

        }

        testsound->transformation->setPosition(car[0]->getTransformation()->getPosition());
        testsound->update(cam);
        testsound2->transformation->setPosition(car[0]->getTransformation()->getPosition());
        testsound2->update(cam);

        wind->transformation->setPosition(car[0]->getTransformation()->getPosition());
        wind->update(cam);

        auto campos = cam->transformation->getPosition();

        auto rot = glm::angleAxis(game->time * 0.4f, glm::vec3(0.0, 1.0, 0.0));

        ocean1->transformation->setPosition(campos + rot * glm::vec3(4.0, 0.0, 0.0));
        ocean1->update(cam);
        ocean2->transformation->setPosition(campos + rot * glm::vec3(0.0, 0.0, 4.0));
        ocean2->update(cam);
        ocean3->transformation->setPosition(campos + rot * glm::vec3(-4.0, 0.0, 0.0));
        ocean3->update(cam);
        ocean4->transformation->setPosition(campos + rot * glm::vec3(0.0, 0.0, -4.0));
        ocean4->update(cam);

        float oceanvol = (1.0f / (1.0 + 0.005 * glm::max(0.0f, campos.y * campos.y))) * 100.0;
        ocean1->setVolume(oceanvol);
        ocean2->setVolume(oceanvol);
        ocean3->setVolume(oceanvol);
        ocean4->setVolume(oceanvol);

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
    else if (currentMode == EDITOR_MODE_PICKING) {
        glm::vec2 cursor = game->getCursorPosition();
        cursor.x = glm::max(0.0f, cursor.x);
        cursor.y = glm::max(0.0f, cursor.y);
        cursor.x = glm::min((float)width, cursor.x);
        cursor.y = glm::min((float)height, cursor.y);
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

    if (inPhysicsPickMode && physicsPickedBody != nullptr) {
        glm::dvec2 cursor = game->getCursorPosition();
        glm::vec2 uv = glm::vec2(cursor.x, cursor.y) / glm::vec2(game->renderer->width, game->renderer->height);
        uv.y = 1.0 - uv.y;
        glm::vec3 raycastDir = cam->cone->reconstructDirection(uv);
        physicsPickDirection = physicsPickDirection * 0.9f + raycastDir * 0.1f;
        auto newpos = cam->transformation->getPosition() + physicsPickDirection * physicsPickDistance;
        auto velocity = newpos - physicsPickedBody->transformation->getPosition();
        // physicsPickedBody->transformation->setPosition(newpos);
        physicsPickedBody->readChanges();
        physicsPickedBody->getRigidBody()->applyCentralForce(bulletify3(velocity * (100.f / physicsPickedBody->getRigidBody()->getInvMass())));
    }
    if (isConsoleWindowOpened == false && currentMode == EDITOR_MODE_WRITING_TEXT) {
        switchMode(lastMode);
    }

    if (currentMode != EDITOR_MODE_MOVE_CAMERA && game->renderer->pickingReady) {

        for (int i = 0; i < car.size(); i++) {
            /*car*/
            if (!car[i]->initialized)continue;
            glm::vec3 targetpos = game->renderer->selectionPosition;
            glm::vec3 flatpos = car[i]->getTransformation()->getPosition();
            glm::vec3 flatdir = -glm::normalize(quat2vec3(car[i]->getTransformation()->getOrientation()));
            glm::vec3 targetdir = glm::normalize(targetpos - flatpos);

            glm::vec3 crs = glm::cross(glm::vec3(targetdir), glm::vec3(flatdir));
            float dt = glm::max(0.0f, glm::dot(glm::vec3(targetdir), glm::vec3(flatdir))); /// 1.0 on track, 0.0 90 degree or more off
            float dt2 = glm::max(0.0f, glm::dot(glm::vec3(-targetdir), glm::vec3(flatdir)));
            if (dt > dt2) {
                // go to point
                float oldangle = car[i]->getWheelsAngle();
                float newangle = (glm::sign(crs.y) * ((1.0 - dt) + 0.06)) / (1.0 + 0.002 * glm::abs(car[i]->getSpeed() * car[i]->getSpeed()));
                car[i]->setWheelsAngle(newangle);

                float speedmult = dt;
                car[i]->setAcceleration(1.0f * (0.89 * speedmult + 0.11));
            }
            else {
                // turn around
                car[i]->setWheelsAngle(-glm::sign(crs.y));
                car[i]->setAcceleration(-0.5565f);
            }

        }
    }
    /* if (car[0]->initialized) {
         glm::vec3 carpos = car[0]->getTransformation()->getPosition();
         glm::vec3 carpos2 = carpos + glm::vec3(300.0, 0.0, 300.0);

         float bc = bytes2[((int)carpos2.z) * 6000 + ((int)carpos2.x)];
         float b = 0.0;
         float w = 0.0;
         glm::vec2 flatnorm = glm::vec2(0.0);
         for (int x = -1; x < 1; x++) {
             for (int y = -1; y < 1; y++) {
                 float r = bytes2[(y + (int)carpos2.z) * 6000 + (x + (int)carpos2.x)];
                 if(x != 0 || y != 0) flatnorm += (r - bc) * glm::normalize(glm::vec2(x, y));
                 b += r;
                 w += 1.0;
             }
         }
         b /= w;
         flatnorm /= w;
         glm::vec3 n = glm::normalize(glm::vec3(flatnorm.x, 4.0, flatnorm.y));


         printf("%f %f %f %f", n.x, n.y, n.z, b);
         virtualbox->transformation->setPosition(glm::vec3(carpos.x, b - 0.5 - 6.0, carpos.z));
         virtualbox->transformation->setOrientation(glm::lookAt(glm::vec3(0.0), n, glm::vec3(1.0, 1.0023556, 0.0)));
         virtualbox->readChanges();
         cursor3dArrow->getInstance(0)->transformation->setPosition(glm::vec3(carpos.x, b - 0.5 , carpos.z));
     }*/
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
    if (ImGui::SliderFloat("Exposure", &Game::instance->renderer->exposure, 0.01f, 10.0f))reset();
    if (ImGui::SliderFloat("Contrast", &Game::instance->renderer->contrast, 0.01f, 10.0f))reset();
    if (ImGui::SliderFloat("CloudsHeightStart", &Game::instance->renderer->cloudsFloor, 100.0f, 30000.0f))reset();
    if (ImGui::SliderFloat("CloudsHeightEnd", &Game::instance->renderer->cloudsCeil, 100.0f, 30000.0f))reset();
    if (ImGui::SliderFloat("CloudsThreshold", &Game::instance->renderer->cloudsThresholdLow, 0.0f, 1.0f))reset();
    if (ImGui::SliderFloat("CloudsThresholdHigh", &Game::instance->renderer->cloudsThresholdHigh, 0.0f, 1.0f))reset();
    //ImGui::SliderFloat("CloudsAtmosphereShaftsMultiplier", &Game::instance->renderer->cloudsAtmosphereShaftsMultiplier, 0.0f, 10.0f);
    //ImGui::SliderFloat("CloudsWindSpeed", &Game::instance->renderer->cloudsWindSpeed, 0.0f, 10.0f);
    if (ImGui::SliderFloat("CloudsDensity", &Game::instance->renderer->cloudsDensityScale, 0.0f, 5.0f))reset();
    if (ImGui::SliderFloat("CloudsIntegration", &Game::instance->renderer->cloudsIntegrate, 0.3f, 0.999f))reset();

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

    if (ImGui::SliderFloat("MieScattering", (float*)&Game::instance->renderer->mieScattCoefficent, 0.0f, 20.0f))reset();
    if (ImGui::SliderFloat("GodRaysStrength", &Game::instance->renderer->noiseOctave1, 0.01f, 10.0f))reset();
    ImGui::Separator();
    if (ImGui::SliderFloat("WaterWavesHeight", &Game::instance->renderer->waterWavesScale, 0.0f, 10.0f))reset();
    if (ImGui::SliderFloat("WaterSpeed", &Game::instance->renderer->waterSpeed, 0.0f, 7.0f))reset();
    if (ImGui::SliderFloat2("WaterScale", &Game::instance->renderer->waterScale.x, 0.0f, 10.0f))reset();
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
    for (int i = 0; i < bonesConstrs.size(); i++) {
        auto c = bonesConstrs[i];
        float val = 0.0;
        btVector3 anglo, anghi;
        ((btGeneric6DofSpring2Constraint*)(c->constraint))->getAngularLowerLimit(anglo);
        ((btGeneric6DofSpring2Constraint*)(c->constraint))->getAngularUpperLimit(anghi);
        glm::vec3 angloG, anghiG;
        angloG = glmify3(anglo);
        anghiG = glmify3(anghi);
        ImGui::SliderFloat3("Bone LIMIT LO", &angloG.x, -1.0f, 1.0f);
        ImGui::SliderFloat3("Bone LIMIT HI", &anghiG.x, -1.0f, 1.0f);
        ((btGeneric6DofSpring2Constraint*)(c->constraint))->setAngularLowerLimit(bulletify3(angloG));
        ((btGeneric6DofSpring2Constraint*)(c->constraint))->setAngularUpperLimit(bulletify3(anghiG));

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

#define invalidate_material() pickedUpMeshLodLevel->materialBufferNeedsUpdate = true

        ImGui::Begin("Selected mesh lod level", &customWindowsOpened[win++], 0);
        if (ImGui::SliderFloat("Distance start", &pickedUpMeshLodLevel->distanceStart, 0.01, 9999.0))invalidate_material();
        if (ImGui::SliderFloat("Distance end", &pickedUpMeshLodLevel->distanceEnd, 0.01, 9999.0))invalidate_material();
        if (ImGui::SliderFloat3("Static diffuse color", &pickedUpMeshLodLevel->material->diffuseColor.x, 0.0, 1.0))invalidate_material();
        if (ImGui::SliderFloat("Static metalness", &pickedUpMeshLodLevel->material->metalness, 0.0, 1.0))invalidate_material();
        if (ImGui::SliderFloat("Static roughness", &pickedUpMeshLodLevel->material->roughness, 0.0, 1.0))invalidate_material();
        if (ImGui::SliderFloat2("Diffuse tex scale", &pickedUpMeshLodLevel->material->diffuseColorTexScale.x, 0.01, 100.0))invalidate_material();
        if (ImGui::SliderFloat2("Normal tex scale", &pickedUpMeshLodLevel->material->normalTexScale.x, 0.01, 100.0))invalidate_material();
        if (ImGui::SliderFloat2("Metalness tex scale", &pickedUpMeshLodLevel->material->metalnessTexScale.x, 0.01, 100.0))invalidate_material();
        if (ImGui::SliderFloat2("Roughness tex scale", &pickedUpMeshLodLevel->material->roughnessTexScale.x, 0.01, 100.0))invalidate_material();
        if (ImGui::SliderFloat2("Bump tex scale", &pickedUpMeshLodLevel->material->bumpTexScale.x, 0.01, 100.0))invalidate_material();
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
    if (key == GLFW_KEY_F5) {
        glm::vec3 hitpos, hitnorm;
        auto res = game->world->physics->rayCast(game->world->mainDisplayCamera->transformation->getPosition(),
            game->world->mainDisplayCamera->cone->reconstructDirection(glm::vec2(0.5)), hitpos, hitnorm);
        if (res != nullptr) {
            Mesh3dInstance* mnt = new Mesh3dInstance(new TransformationManager(hitpos + glm::vec3(0.0, 3.0, 0.0), glm::quat(), glm::vec3(1)));
            cursor3dArrow->addInstance(mnt);
            Physics* p = game->world->physics;
            auto ob = p->createBody(1.0f, mnt, new btSphereShape(1.0f));
            ob->enable();
        }

    }
    if (key == GLFW_KEY_F9) {
        glm::vec3 hitpos, hitnorm;
        auto res = game->world->physics->rayCast(game->world->mainDisplayCamera->transformation->getPosition(),
            game->world->mainDisplayCamera->cone->reconstructDirection(glm::vec2(0.5)), hitpos, hitnorm);
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
    cam->createProjectionPerspective(fovnew, (float)game->width / (float)game->height, 0.01f, 10000);
    game->onWindowResize->add([&](int zero) {
        cam->createProjectionPerspective(fovnew, (float)game->width / (float)game->height, 0.01f, 10000);
    });
    cam->transformation->translate(glm::vec3(16, 16, 16));
    glm::quat rot = glm::quat_cast(glm::lookAt(cam->transformation->getPosition(), glm::vec3(0), glm::vec3(0, 1, 0)));
    cam->transformation->setOrientation(rot);
    game->world->mainDisplayCamera = cam;

    game->setCursorMode(GLFW_CURSOR_NORMAL);
    /*
    auto t = game->asset->loadSceneFile("oldcity.scene");
    for (int i = 0; i < t->getMesh3ds().size(); i++) {
        t->getMesh3ds()[i]->getInstance(0)->transformation->translate(vec3(0.0, 1.5, 0.0));
        game->world->scene->addMesh3d(t->getMesh3ds()[i]);
    }*/

    /*
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
    game->world->scene->addMesh3d(s);*/
    /*
    auto s = game->asset->loadMeshFile("terr_grass.mesh3d");
    auto grasses = s->getInstances();
    int grassess = grasses.size();
    for (int i = 0; i < grassess; i++) {
        auto pos = grasses[i]->transformation->getPosition();
        grasses[i]->transformation->setSize(glm::vec3(1.0f));
        grasses[i]->transformation->setPosition(glm::vec3(pos.x * 1.0f + 500.0f, pos.y * 4.0f - 10.0f, pos.z * 1.0f + 500.0f));
    }
    //game->world->scene->addMesh3d(s);
    */
    /*

    int terrainparts = 10;
    float fullsize = 6000.0;
    float partsize = 600.0;
    auto tex = new Texture2d("terrain_diffuse.png");
    for (int x = 0; x < 10; x++) {
        for (int y = 0; y < 10; y++) {
            stringstream ss0;
            ss0 << "terr_lod0_" << x << "x" << y << ".raw";
            stringstream ss1;
            ss1 << "terr_lod1_" << x << "x" << y << ".raw";
            stringstream ss2;
            ss2 << "terr_lod2_" << x << "x" << y << ".raw";
            auto mat = new Material();
            mat->diffuseColorTex = tex;
            Mesh3d* m = Mesh3d::create(game->asset->loadObject3dInfoFile(ss0.str()), mat);
            m->getInstance(0)->transformation->setPosition(vec3(partsize * x * 1.0f, -0.5, partsize*y * 1.0f));
            m->getInstance(0)->transformation->setSize(vec3(1.0f));
            m->getLodLevel(0)->distanceStart = 0.0f;
            m->getLodLevel(0)->distanceEnd = 650.0f;

            m->addLodLevel(new Mesh3dLodLevel(game->asset->loadObject3dInfoFile(ss1.str()), mat, 650.0f, 3350.0f));
            m->addLodLevel(new Mesh3dLodLevel(game->asset->loadObject3dInfoFile(ss2.str()), mat, 3350.0f, 11150.0f));
            game->world->scene->addMesh3d(m);
        }
    }

    */

    //  t->name = "flagbase";
     // game->world->scene->addMesh(t);
    //auto t1 = new TransformationManager(glm::vec3(8.0, 6.0, 8.0));
   // car.push_back(new Car(t1));
  //  auto t2 = new TransformationManager(glm::vec3(-8.0, 6.0, -8.0));
   // car.push_back(new Car(t2));



    auto xt = game->asset->loadMeshFile("2dplane.mesh3d");
    game->world->scene->addMesh3d(xt);

    game->invoke([&]() {
        auto phys = Game::instance->world->physics;
        /*
        vector<float> vertices = {};
        vector<int> indices = {};
        auto wterrobj = game->asset->loadObject3dInfoFile("weirdterrain.raw");
        auto wterr3d = Mesh3d::create(wterrobj, new Material());
        //game->world->scene->addMesh3d(wterr3d);
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
        bytes2 = new float[bytescount / 2];
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
        auto groundpb = phys->createBody(0.0f, new TransformationManager(glm::vec3(0.0, 1.5, 0.0)), new btBoxShape(btVector3(1000.0f, 1.0f, 1000.0f)));
        groundpb->getCollisionObject()->setFriction(4);
        groundpb->enable();

    });

    // virtualbox = game->world->physics->createBody(0.0f, new TransformationManager(), new btBoxShape(btVector3(6.0, 6.0, 6.0)));
    // virtualbox->enable();

    testsound = new Sound3d("engine.flac", new TransformationManager(glm::vec3(0.0, 20.0, 0.0)));
    testsound->setLoop(true);
    testsound2 = new Sound3d("brown_noise.flac", new TransformationManager(glm::vec3(0.0, 20.0, 0.0)));
    testsound2->setLoop(true);
    wind = new Sound3d("brown_noise.flac", new TransformationManager(glm::vec3(0.0, 20.0, 0.0)));
    wind->setLoop(true);
    ocean1 = new Sound3d("ocean1.flac", new TransformationManager(glm::vec3(0.0, 20.0, 0.0)));
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

    cursor3dArrow = Mesh3d::create(game->asset->loadObject3dInfoFile("deferredsphere.raw"), new Material());
    cursor3dArrow->clearInstances();
    // cursor3dArrow->addInstance(new Mesh3dInstance(new TransformationManager(glm::vec3(0.0), glm::quat(), glm::vec3(7.0))));
    game->world->scene->addMesh3d(cursor3dArrow);
    for (int xx = 0; xx < 1; xx++) {
        for (int yy = 0; yy < 1; yy++)
        {
            auto c = new Car(xx % 2 == 0 ? "fiesta.car" : "fiesta.car", new TransformationManager(glm::vec3(xx * 10.0 + 33.0, 55.0, yy * 10.0)));
            car.push_back(c);
        }
    }

    // SKELETON TEST

    auto skel = game->asset->loadSkeletonFile("tree.skeleton");
    auto mrstickobj = game->asset->loadObject3dInfoFile("tree.raw");
    mrstick = Mesh3d::create(mrstickobj, game->asset->loadMaterialFile("red.material"));
    mrstick->getLodLevel(0)->skeleton = skel;
    auto pose = new SkeletonPose();
   // auto bonesposes = vector<glm::vec3>();
   // auto bonessizes = vector<float>();
    mrstick->getLodLevel(0)->skeletonPose = pose;
    for (int i = 0; i < skel->positions.size(); i++) {

        glm::vec3 sum = skel->positions[i];
        float sumr = skel->radiues[i];

        pose->pose.push_back(glm::translate(glm::mat4(1), sum));
        cursor3dArrow->addInstance(new Mesh3dInstance(new TransformationManager(sum, glm::vec3(sumr * 1.1f))));
    }

    // we have bones positions and hierachy
    // we are able to reweight it completely
    /*
    int weightscnt = skel->weights.size();

    for (int i = 0; i < weightscnt; i++) {
        skel->weights[i].clear();
    }*/

    int vertid = 0;
    for (int z = 0; z < mrstickobj->vbo.size(); z += 12) {
        // if (skel->weights.size() <= vertid) break;
        glm::vec3 vert = glm::vec3(mrstickobj->vbo[z], mrstickobj->vbo[z + 1], mrstickobj->vbo[z + 2]);
        int i = -1;
        float dst = 99999.0f;

        for (int g = 0; g < skel->positions.size(); g++) {
            glm::vec3 pos = skel->positions[g];
            if (glm::distance(pos, vert) < dst) {
                dst = glm::distance(pos, vert);
                i = g;
            }
        }
        int iparent = skel->parents[i];
        int ichild1 = -1;
        int ichild2 = -1;
        int ichild3 = -1;
        int ichild4 = -1;
        for (int g = 0; g < skel->positions.size(); g++) if (skel->parents[g] == i) {
            if (ichild1 == -1) ichild1 = g;
            else if (ichild2 == -1) ichild2 = g;
            else if (ichild3 == -1) ichild3 = g;
            else if (ichild4 == -1) ichild4 = g;
        }
        int tab[6] = { i, iparent , ichild1 , ichild2, ichild3, ichild4};
        vector<SkeletonBoneWeight> ws = {};
        skel->weights.push_back({});
        for (int g = 0; g < 6; g++) {
            int bone = tab[g];
            if (bone < 0) continue;
            glm::vec3 pos = skel->positions[bone];

            float w = 1.0 / ((glm::distance(vert, pos) + 1.0) * 11.0f);

            SkeletonBoneWeight skwk = SkeletonBoneWeight(bone, 1.0);
            skel->weights[vertid].push_back(skwk);
        }

        vertid++;

    }


    bonesBodies = vector<PhysicalBody*>{};
    bonesConstrs = vector<PhysicalConstraint*>{};
    for (int i = 0; i < skel->positions.size(); i++) {
        auto p = skel->positions[i];
        PhysicalBody* pb = game->world->physics->createBody(glm::min(0.05f, 1.0f), new TransformationManager(p + glm::vec3(0.0, 6.0, 0.0)), new btSphereShape(skel->radiues[i]));
        bonesBodies.push_back(pb);
    }
    for (int i = 0; i < skel->positions.size(); i++) {
        auto p = skel->positions[i];
        int closestindex = skel->parents[i];
        if (closestindex < 0) {
            bonesBodies[i]->enable();
            continue;
        }
        auto closest = skel->positions[skel->parents[i]];
        auto midway = (closest + p) * 0.5f;
        auto vector1_to_midway = (midway - p);
        auto vector2_to_midway = (midway - closest);
        btTransform frameInA, frameInB;
        frameInA = btTransform::getIdentity();
        frameInB = btTransform::getIdentity();

        frameInA.setOrigin(bulletify3(vector1_to_midway));
        frameInB.setOrigin(bulletify3(vector2_to_midway));
        auto ct1 = new btGeneric6DofSpring2Constraint(*(bonesBodies[i]->getRigidBody()), *(bonesBodies[closestindex]->getRigidBody()), frameInA, frameInB);
        PhysicalConstraint* pc = new PhysicalConstraint(ct1, bonesBodies[i], bonesBodies[closestindex]);
        ct1->setBreakingImpulseThreshold(999990.0f);
        float moverange = 0.3;
        ct1->setAngularLowerLimit(btVector3(-moverange, -moverange, -moverange));
        ct1->setAngularUpperLimit(btVector3(moverange, moverange, moverange));
        ct1->setLinearLowerLimit(btVector3(0, 0, 0));
        ct1->setLinearUpperLimit(btVector3(0, 0, 0));/*
        ct1->setBounce(0, 0);
        ct1->setBounce(1, 0);
        ct1->setBounce(2, 0);
        ct1->setStiffness(0, 0);
        ct1->setStiffness(1, 0);
        ct1->setStiffness(2, 0);*/

        bonesConstrs.push_back(pc);
        bonesBodies[i]->enable();
        bonesBodies[closestindex]->enable();
        pc->enable();
    }
    for (int i = 0; i < skel->positions.size(); i++) {
        for (int g = 0; g < skel->positions.size(); g++) {
            bonesBodies[i]->getRigidBody()->setIgnoreCollisionCheck(bonesBodies[g]->getRigidBody(), true);
        }
    }
    bonesBodies[2]->getRigidBody()->setLinearVelocity(btVector3(0.0, 1.0, 1));
    mrstick->getInstance(0)->transformation->translate(glm::vec3(0.0, 0.0, 0.0));
    game->world->scene->addMesh3d(mrstick);

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

void EditorApp::onMouseDown(int button)
{
    glm::dvec2 cursor = game->getCursorPosition();
    glm::vec2 uv = glm::vec2(cursor.x, cursor.y) / glm::vec2(game->renderer->width, game->renderer->height);
    uv.y = 1.0 - uv.y;
    glm::vec3 raycastDir = cam->cone->reconstructDirection(uv);
    glm::vec3 hitpos, hitnorm;
    auto body = game->world->physics->rayCast(cam->transformation->getPosition(), raycastDir, hitpos, hitnorm);
    printf("1PICKED %f %f \n", uv.x, uv.y);
    if (body != nullptr && !body->isStatic()) {
        physicsPickedBody = body;
        physicsPickDirection = raycastDir;
        physicsPickDistance = glm::distance(hitpos, cam->transformation->getPosition());
        inPhysicsPickMode = true;
        printf("PICKED");
    }

}

void EditorApp::onMouseUp(int button)
{
    inPhysicsPickMode = false;
    printf("UNPICKED");
}

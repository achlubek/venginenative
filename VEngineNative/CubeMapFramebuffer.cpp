#include "stdafx.h"
#include "CubeMapFramebuffer.h"
 
Camera* CubeMapFramebuffer::switchFace(GLenum face, bool clear)
{
    int vindex = face - GL_TEXTURE_CUBE_MAP_POSITIVE_X;
    for (int i = 0; i < attachedTextures.size(); i++) {
        if (attachedTextures[i]->textureCube != NULL) {
            glFramebufferTexture2D(GL_FRAMEBUFFER, attachedTextures[i]->attachment, face, attachedTextures[i]->textureCube->handle, 0);
        }
    }
    if (clear)
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    return facesCameras[vindex];
}

void CubeMapFramebuffer::generate()
{
    facesCameras = {};

    Camera * cam_posx = new Camera();
    Camera * cam_posy = new Camera();
    Camera * cam_posz = new Camera();

    Camera * cam_newx = new Camera();
    Camera * cam_newy = new Camera();
    Camera * cam_newz = new Camera();

    cam_posx->createProjectionPerspective(deg2rad(90.0f), 1.0f, 0.1f, 10000.0f);
    cam_posy->createProjectionPerspective(deg2rad(90.0f), 1.0f, 0.1f, 10000.0f);
    cam_posz->createProjectionPerspective(deg2rad(90.0f), 1.0f, 0.1f, 10000.0f);

    cam_newx->createProjectionPerspective(deg2rad(90.0f), 1.0f, 0.1f, 10000.0f);
    cam_newy->createProjectionPerspective(deg2rad(90.0f), 1.0f, 0.1f, 10000.0f);
    cam_newz->createProjectionPerspective(deg2rad(90.0f), 1.0f, 0.1f, 10000.0f);

    cam_posx->transformation->orientation = glm::quat_cast(glm::lookAt(glm::vec3(0), glm::vec3(1, 0, 0), glm::vec3(0, -1, 0)));
    cam_posy->transformation->orientation = glm::quat_cast(glm::lookAt(glm::vec3(0), glm::vec3(0, -1, 0), glm::vec3(0, 0, -1)));
    cam_posz->transformation->orientation = glm::quat_cast(glm::lookAt(glm::vec3(0), glm::vec3(0, 0, 1), glm::vec3(0, -1, 0)));

    cam_newx->transformation->orientation = glm::quat_cast(glm::lookAt(glm::vec3(0), glm::vec3(-1, 0, 0), glm::vec3(0, -1, 0)));
    cam_newy->transformation->orientation = glm::quat_cast(glm::lookAt(glm::vec3(0), glm::vec3(0, 1, 0), glm::vec3(0, 0, 1)));
    cam_newz->transformation->orientation = glm::quat_cast(glm::lookAt(glm::vec3(0), glm::vec3(0, 0, -1), glm::vec3(0, -1, 0)));

    facesCameras.push_back(cam_posx);
    facesCameras.push_back(cam_newx);

    facesCameras.push_back(cam_posy);
    facesCameras.push_back(cam_newy);

    facesCameras.push_back(cam_posz);
    facesCameras.push_back(cam_newz);
}
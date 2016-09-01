#pragma once
#include "AbsFramebuffer.h"
#include "Camera.h"
class CubeMapFramebuffer : public AbsFramebuffer
{
public:
    Camera* switchFace(GLenum face, bool clear);
private:
    vector<Camera*> facesCameras;
    void generate();
};

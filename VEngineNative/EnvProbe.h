#pragma once
#include "Renderer.h"

class EnvPlane
{
public:
    glm::vec3 point;
    glm::vec3 normal;
    EnvPlane(glm::vec3 ipoint, glm::vec3 inormal) {
        point = ipoint;
        normal = inormal;
    }
};

class EnvProbe : public AbsTransformable
{
public:
    EnvProbe(Renderer* irenderer, vector<EnvPlane*>& iplanes);
    void refresh();
    void setUniforms();
    ~EnvProbe();
    Renderer * renderer;
    CubeMapFramebuffer * framebuffer;
    CubeMapTexture * texture;
    vector<EnvPlane*> planes;
};

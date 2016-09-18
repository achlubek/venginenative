#pragma once
#include "AbsTransformable.h"
#include "ShaderProgram.h"
#include "Array2dFramebuffer.h"
#include "Camera.h"
class CascadeShadowMap
{
public:
    CascadeShadowMap(int width, int height, vector<float> iradiuses);
    void map(glm::vec3 direction, glm::vec3 position);
    void setUniformsAndBindSampler(ShaderProgram * shader, int sampler);
    ~CascadeShadowMap();
private:
    glm::vec3 direction;
    glm::vec3 position;
    vector<glm::mat4> pmatrices;
    vector<float> farplanes;
    vector<float> radiuses;
    int cascadeCount;
    Array2dFramebuffer *framebuffer;
    Texture2dArray *texture;
    Texture2dArray *textureDepth;
    Camera * camera;
};


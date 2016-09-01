#pragma once
#include "AbsTransformable.h"
#include "Texture2d.h"
#include "Framebuffer.h"
#include "CubeMapFramebuffer.h"
#include "Camera.h"

#define LIGHT_SPOT 0
#define LIGHT_POINT 1
#define LIGHT_AMBIENT 2

class Light : public AbsTransformable
{
public:
    Light();
    ~Light();
    float cutOffDistance;
    float angle;
    glm::vec3 color;
    void resizeShadowMap(int width, int height);
    void switchShadowMapping(bool value);
    void bindShadowMap(int spot, int cube);
    void refreshShadowMap();
    Camera *lightCamera;
    bool shadowMappingEnabled;
    int type;
private:
    int shadowMapWidth;
    int shadowMapHeight;
    Texture2d *depthMap;
    Framebuffer *mapper;
    CubeMapTexture *depthCubeMap;
    CubeMapFramebuffer *cubeMapper;
    void recreateFbo();
    void destroyFbo();
};

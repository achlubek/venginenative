#pragma once
#include "AbsTransformable.h"
#include "FrustumCone.h"
class Camera : public AbsTransformable
{
public:
    Camera();
    ~Camera();

    float brightness;
    float farplane;
    float focalLength;
    float fov;
    FrustumCone *cone;
    glm::mat4 projectionMatrix;

    void createProjectionPerspective(float fov, float aspectRatio, float nearpl, float farpl);
    void createProjectionOrthogonal(float minx, float miny, float minz, float maxx, float maxy, float maxz);

    glm::vec2 projectToScreen(glm::vec3 worldPosition);

private:
    void updateFocalLength();
};

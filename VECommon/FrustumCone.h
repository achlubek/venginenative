#pragma once
class FrustumCone
{
public:
    FrustumCone();
    ~FrustumCone();
    glm::vec3 origin;
    glm::vec3 leftBottom;
    glm::vec3 leftTop;
    glm::vec3 rightBottom;
    glm::vec3 rightTop;
    void update(glm::mat4 rotprojmatrix);
    glm::vec3 reconstructDirection(glm::vec2 uv);
    FrustumCone* clone();
private:
    glm::vec3 getDir(glm::vec2 uv, glm::mat4 inv);
};

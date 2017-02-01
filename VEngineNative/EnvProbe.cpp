#include "stdafx.h"
#include "EnvProbe.h"

EnvProbe::EnvProbe(Renderer * irenderer, vector<EnvPlane*>& iplanes)
{
    initTransformation();
    renderer = irenderer;
    planes = {};
    planes = move(iplanes);
    framebuffer = new CubeMapFramebuffer();
    texture = new CubeMapTexture(irenderer->width, irenderer->height, GL_RGBA16F, GL_RGBA, GL_HALF_FLOAT);
    framebuffer->attachTexture(texture, GL_COLOR_ATTACHMENT0);
}

void EnvProbe::refresh()
{
    renderer->envProbesLightMultiplier = 0.05;
    renderer->renderToFramebuffer(transformation->getPosition(), framebuffer);
    glBindFramebuffer(GL_FRAMEBUFFER, 0);
    texture->generateMipMaps();
}

void EnvProbe::setUniforms()
{
    ShaderProgram *s = ShaderProgram::current;
    s->setUniform("EnvProbePosition", transformation->getPosition());
    s->setUniform("EnvProbePlanesCount", (int)planes.size());
    vector<glm::vec3> points;
    vector<glm::vec3> normals;
    for (int i = 0; i < planes.size(); i++) {
        points.push_back(planes[i]->point);
        normals.push_back(planes[i]->normal);
    }
    s->setUniformVector("EnvProbePlanesPoints", points);
    s->setUniformVector("EnvProbePlanesNormals", normals);
}

EnvProbe::~EnvProbe()
{
}
#pragma once
#include "Framebuffer.h";
#include "Texture.h";
#include "CubeMapTexture.h";
#include "CubeMapFramebuffer.h";
#include "ShaderProgram.h";
#include "Object3dInfo.h";
#include "Texture3d.h";
class Renderer
{
public:
    Renderer(int iwidth, int iheight);
    ~Renderer();
    void renderToFramebuffer(glm::vec3 position, CubeMapFramebuffer *fbo);
    void renderToFramebuffer(Camera *camera, Framebuffer *fbo);
    void recompileShaders();
    void resize(int iwidth, int iheight);
    bool useAmbientOcclusion;
    bool useGammaCorrection;
    float envProbesLightMultiplier;
    float cloudsFloor;
    float cloudsCeil;
    float cloudsThresholdLow;
    float cloudsThresholdHigh;
    float cloudsDensityThresholdLow;
    float cloudsDensityThresholdHigh;
    float cloudsDensityScale;
    float cloudsWindSpeed;
    float atmosphereScale;
    float waterWavesScale;
    float noiseOctave1;
    float noiseOctave2;
    float noiseOctave3;
    float noiseOctave4;
    float noiseOctave5;
    float noiseOctave6;
    glm::vec3 cloudsOffset;
    glm::vec3 sunDirection;
    int width;
    int height;
private:
    void draw(Camera *camera);
    void initializeFbos();
    void destroyFbos();

    CubeMapTexture *skyboxTexture;
    Object3dInfo *quad3dInfo;
    Object3dInfo *sphere3dInfo;

    //MRT Buffers
    Framebuffer *mrtFbo;
    Texture *mrtAlbedoRoughnessTex;
    Texture *mrtNormalMetalnessTex;
    Texture *mrtDistanceTexture;
    Texture *depthTexture;

    // Effects part
    ShaderProgram *deferredShader;
    ShaderProgram *envProbesShader;
    Framebuffer *deferredFbo;
    Texture *deferredTexture;
    void deferred();

    ShaderProgram *ambientLightShader;
    Framebuffer *ambientLightFbo;
    Texture *ambientLightTexture;
    void ambientLight();

    ShaderProgram *ambientOcclusionShader;
    Framebuffer *ambientOcclusionFbo;
    Texture *ambientOcclusionTexture;
    void ambientOcclusion();

    ShaderProgram *fogShader;
    Framebuffer *fogFbo;
    Texture *fogTexture;
    void fog();

    ShaderProgram *atmScattShader;
    Framebuffer *atmScattFbo;
    Texture *atmScattTexture;
    void atmScatt();

    ShaderProgram *cloudsShader;
    CubeMapFramebuffer *cloudsFboEven;
    CubeMapTexture *cloudsTextureEven;
    CubeMapFramebuffer *cloudsFboOdd;
    CubeMapTexture *cloudsTextureOdd;

    bool cloudCycleUseOdd = false;
    void clouds();

    ShaderProgram *fxaaTonemapShader;
   // Framebuffer *fxaaTonemapFbo;
    //Texture *fxaaTonemapTexture;
    void fxaaTonemap();

    ShaderProgram *motionBlurShader;
    Framebuffer *motionBlurFbo;
    Texture *motionBlurTexture;
    void motionBlur();

    ShaderProgram *bloomShader;
    Framebuffer *bloomFbo;
    Texture *bloomXTexture;
    Texture *bloomYTexture;
    void bloom();

    ShaderProgram *combineShader;
    Framebuffer *combineFbo;
    Texture *combineTexture;
    void combine();

    // Output to output fbo
    ShaderProgram *outputShader;
    void output();

    Camera* currentCamera;
};

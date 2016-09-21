#pragma once
#include "Framebuffer.h";
#include "Texture2d.h";
#include "CubeMapTexture.h";
#include "CubeMapFramebuffer.h";
#include "ShaderProgram.h";
#include "Object3dInfo.h";
#include "Texture3d.h";
#include "CascadeShadowMap.h";
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
    float noiseOctave7;
    float noiseOctave8;
    float cloudsIntegrate;
    ShaderProgram *cloudsShader;
    glm::vec3 cloudsOffset;
    glm::vec3 sunDirection;
    int width;
    int height;
private:
    void draw(Camera *camera);
    void initializeFbos();
    void destroyFbos();
    void setCommonUniforms(ShaderProgram *  sp);

    CascadeShadowMap *csm;

    CubeMapTexture *skyboxTexture;
    Object3dInfo *quad3dInfo;
    Object3dInfo *sphere3dInfo;

    //MRT Buffers
    Framebuffer *mrtFbo;
    Texture2d *mrtAlbedoRoughnessTex;
    Texture2d *mrtNormalMetalnessTex;
    Texture2d *mrtDistanceTexture;
    Texture2d *depthTexture;

    // Effects part
    ShaderProgram *deferredShader;
    ShaderProgram *envProbesShader;
    Framebuffer *deferredFbo;
    Texture2d *deferredTexture;
    void deferred();

    ShaderProgram *ambientLightShader;
    Framebuffer *ambientLightFbo;
    Texture2d *ambientLightTexture;
    void ambientLight();

    ShaderProgram *ambientOcclusionShader;
    Framebuffer *ambientOcclusionFbo;
    Texture2d *ambientOcclusionTexture;
    void ambientOcclusion();

    ShaderProgram *fogShader;
    Framebuffer *fogFbo;
    Texture2d *fogTexture;
    void fog();

    ShaderProgram *atmScattShader;
    CubeMapFramebuffer *atmScattFbo;
    CubeMapTexture *atmScattTexture;
    void atmScatt();

    CubeMapFramebuffer *cloudsFboEven;
    CubeMapTexture *cloudsTextureEven;
    CubeMapFramebuffer *cloudsFboOdd;
    CubeMapTexture *cloudsTextureOdd;

    bool cloudCycleUseOdd = false;
    int cloudFace = 0;
    void clouds();

    ShaderProgram *fxaaTonemapShader;
   // Framebuffer *fxaaTonemapFbo;
    //Texture2d *fxaaTonemapTexture;
    void fxaaTonemap();

    ShaderProgram *motionBlurShader;
    Framebuffer *motionBlurFbo;
    Texture2d *motionBlurTexture;
    void motionBlur();

    ShaderProgram *bloomShader;
    Framebuffer *bloomFbo;
    Texture2d *bloomXTexture;
    Texture2d *bloomYTexture;
    void bloom();

    ShaderProgram *combineShader;
    Framebuffer *combineFbo;
    Texture2d *combineTexture;
    void combine();

    ShaderProgram *lensBlurShader;
    Framebuffer *lensBlurFboHorizontal;
    Framebuffer *lensBlurFboVertical;
    Texture2d *lensBlurTextureHorizontal;
    Texture2d *lensBlurTextureVertical;
    void lensBlur();

    // Output to output fbo
    ShaderProgram *outputShader;
    void output();



    Camera* currentCamera;
};

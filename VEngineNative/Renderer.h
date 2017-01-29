#pragma once
#include "Framebuffer.h";
#include "Texture2d.h";
#include "CubeMapTexture.h";
#include "CubeMapFramebuffer.h";
#include "ShaderProgram.h";
#include "Object3dInfo.h";
#include "Texture3d.h";
#include "CascadeShadowMap.h";
#include "ShaderStorageBuffer.h";
#include "FrustumCone.h";
#include "INIReader.h";
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
    float lensBlurSize;
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
    float contrast;
    float exposure;
    float mieScattCoefficent;
    float nightSkyLightPollution;
    float cloudsIntegrate;
    ShaderProgram *cloudsShader;
    glm::vec3 cloudsOffset;
    float dayElapsed;
    float yearElapsed;
    float equatorPoleMix;
    glm::vec3 wind;
    glm::vec2 waterScale;
    float waterHeight;
    float waterSpeed;
    int width;
    int height;
    unsigned int pickingResultMesh = 0;
    unsigned int pickingResultLod = 0;
    unsigned int pickingResultInstance = 0;
    volatile bool pickingReady = true;
    void pick(Camera* camera, glm::vec2 uv);
private:
    void draw(Camera *camera);
    void initializeFbos();
    void destroyFbos(bool onlyViewDependant);
    void setCommonUniforms(ShaderProgram *  sp);

    INIReader *config;

    CascadeShadowMap *csm;

    bool gpuInitialized;
    ShaderProgram * exposureComputeShader;
    ShaderStorageBuffer * exposureBuffer;

    CubeMapTexture *skyboxTexture;
    Texture2d *fresnelTexture;
    Texture2d *starsTexture;
    Texture2d *moonTexture;
    Object3dInfo *quad3dInfo;
    Object3dInfo *sphere3dInfo;

    //MRT Buffers
    Framebuffer *mrtFbo;
    Texture2d *mrtAlbedoRoughnessTex;
    Texture2d *mrtNormalMetalnessTex;
    Texture2d *mrtDistanceTexture;
    Texture2d *depthTexture;

    //Picking by color Buffers
    Framebuffer *pickingFbo;
    Texture2d *pickingDataTex;
    Texture2d *pickingDepthTexture;
    ShaderProgram *pickingDrawingShader;
    ShaderProgram *pickingReadShader;

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
     
    ShaderProgram *waterTileShader;
    Framebuffer *waterTileFbo1;
    Texture2d *waterTileTexture1;
    Framebuffer *waterTileFbo2;
    Texture2d *waterTileTexture2;
    bool waterTileUseFBO1 = true;
    void waterTile();

    ShaderProgram *ambientOcclusionShader;
    Framebuffer *ambientOcclusionFbo;
    Texture2d *ambientOcclusionTexture;
    void ambientOcclusion();

    ShaderProgram *fogShader;
    Framebuffer *fogFbo;
    Texture2d *fogTexture;
    void fog();

    ShaderProgram *waterMeshShader;
    Framebuffer *waterMeshFbo;
    Texture2d *waterMeshTexture;
    void waterMesh();

    ShaderProgram *waterColorShader;
    Framebuffer *waterColorFbo;
    Texture2d *waterColorTexture;
    void waterColorShaded();

    ShaderProgram *atmScattShader;
    CubeMapFramebuffer *atmScattFbo;
    CubeMapTexture *atmScattTexture;
    void atmScatt();

    bool cloudCycleUseOdd = false;
    int passPhrase = 0;
    int cloudFace = 0;

    CubeMapFramebuffer *cloudsFboEven;
    CubeMapTexture *cloudsTextureEven;
    CubeMapFramebuffer *cloudsFboOdd;
    CubeMapTexture *cloudsTextureOdd;

    CubeMapFramebuffer *cloudsShadowsFboEven;
    CubeMapTexture *cloudsShadowsTextureEven;
    CubeMapFramebuffer *cloudsShadowsFboOdd;
    CubeMapTexture *cloudsShadowsTextureOdd;

    ShaderProgram *cloudResolveShader;
    CubeMapFramebuffer *cloudsResolvedFbo;
    CubeMapTexture *cloudsResolvedTexture;

    void clouds();
    void cloudsResolve();

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
    void combine(int step);

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

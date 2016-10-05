#include "stdafx.h"
#include "Renderer.h"
#include "Game.h"
#include "FrustumCone.h"
#include "Media.h"

Renderer::Renderer(int iwidth, int iheight)
{
    envProbesLightMultiplier = 1.0;
    width = iwidth;
    height = iheight;

    useAmbientOcclusion = false;
    useGammaCorrection = true;

    cloudsFloor = 2500;
    cloudsCeil = 6000;
    cloudsThresholdLow = 0.84;
    cloudsThresholdHigh = 0.85;
    cloudsDensityThresholdLow = 0.0;
    cloudsDensityThresholdHigh = 1.0;
    cloudsDensityScale = 1.0;
    cloudsWindSpeed = 0.4;

    noiseOctave1 = 1.0;
    noiseOctave2 = 1.02;
    noiseOctave3 = 1.03;
    noiseOctave4 = 0.0;
    noiseOctave5 = 0.5;
    noiseOctave6 = 4.0;
    noiseOctave7 = 1.01;
    noiseOctave8 = 1.01;
    cloudsIntegrate = 0.90;
    mieScattCoefficent = 1.0;
    lensBlurSize = 1.0;
    gpuInitialized = false;

    csm = new CascadeShadowMap(4096, 4096, {64, 256, 768, 4096, 4096 * 4});

    cloudsOffset = glm::vec3(1);
    sunDirection = glm::vec3(0, 1, 0);
    atmosphereScale = 1.0;
    waterWavesScale = 1.0;

    vector<GLfloat> ppvertices = {
        -1.0f, -1.0f, 1.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f,
        1.0f, -1.0f, 1.0f, 0.0f, 1.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f,
        -1.0f, 1.0f, 1.0f, 1.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f,
        1.0f, 1.0f, 1.0f, 1.0f, 1.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f
    };
    quad3dInfo = new Object3dInfo(ppvertices);
    quad3dInfo->drawMode = GL_TRIANGLE_STRIP;

    unsigned char* bytes;
    int bytescount = Media::readBinary("deferredsphere.raw", &bytes);
    GLfloat * floats = (GLfloat*)bytes;
    int floatsCount = bytescount / 4;
    vector<GLfloat> flo(floats, floats + floatsCount);

    sphere3dInfo = new Object3dInfo(flo);

    outputShader = new ShaderProgram("PostProcess.vertex.glsl", "Output.fragment.glsl");
    deferredShader = new ShaderProgram("PostProcessPerspective.vertex.glsl", "Deferred.fragment.glsl");
    envProbesShader = new ShaderProgram("PostProcess.vertex.glsl", "EnvProbes.fragment.glsl");
    ambientLightShader = new ShaderProgram("PostProcess.vertex.glsl", "AmbientLight.fragment.glsl");
    ambientOcclusionShader = new ShaderProgram("PostProcess.vertex.glsl", "AmbientOcclusion.fragment.glsl");
    fogShader = new ShaderProgram("PostProcess.vertex.glsl", "Fog.fragment.glsl");
    cloudsShader = new ShaderProgram("PostProcess.vertex.glsl", "Clouds.fragment.glsl");
    atmScattShader = new ShaderProgram("PostProcess.vertex.glsl", "AtmScatt.fragment.glsl");
    lensBlurShader = new ShaderProgram("PostProcess.vertex.glsl", "LensBlur.fragment.glsl");
    motionBlurShader = new ShaderProgram("PostProcess.vertex.glsl", "MotionBlur.fragment.glsl");
    bloomShader = new ShaderProgram("PostProcess.vertex.glsl", "Bloom.fragment.glsl");
    combineShader = new ShaderProgram("PostProcess.vertex.glsl", "Combine.fragment.glsl");
    fxaaTonemapShader = new ShaderProgram("PostProcess.vertex.glsl", "FxaaTonemap.fragment.glsl");
    waterTileShader = new ShaderProgram("PostProcess.vertex.glsl", "WaterTile.fragment.glsl");
    exposureComputeShader = new ShaderProgram("CalculateExposure.compute.glsl");
    exposureBuffer = new ShaderStorageBuffer();


    skyboxTexture = new CubeMapTexture("posx.jpg", "posy.jpg", "posz.jpg", "negx.jpg", "negy.jpg", "negz.jpg");
    initializeFbos();
}

void Renderer::resize(int width, int height)
{
    destroyFbos();
    initializeFbos();
}

void Renderer::initializeFbos()
{
    mrtAlbedoRoughnessTex = new Texture2d(width, height, GL_RGBA8, GL_RGBA, GL_UNSIGNED_BYTE);
    mrtNormalMetalnessTex = new Texture2d(width, height, GL_RGBA16F, GL_RGBA, GL_HALF_FLOAT);
    mrtDistanceTexture = new Texture2d(width, height, GL_R32F, GL_RED, GL_FLOAT);
    depthTexture = new Texture2d(width, height, GL_DEPTH_COMPONENT32F, GL_DEPTH_COMPONENT, GL_FLOAT); // most probably overkill

    mrtFbo = new Framebuffer();
    mrtFbo->attachTexture(mrtAlbedoRoughnessTex, GL_COLOR_ATTACHMENT0);
    mrtFbo->attachTexture(mrtNormalMetalnessTex, GL_COLOR_ATTACHMENT1);
    mrtFbo->attachTexture(mrtDistanceTexture, GL_COLOR_ATTACHMENT2);
    mrtFbo->attachTexture(depthTexture, GL_DEPTH_ATTACHMENT);
    
    deferredTexture = new Texture2d(width, height, GL_RGBA16F, GL_RGBA, GL_HALF_FLOAT);
    deferredFbo = new Framebuffer();
    deferredFbo->attachTexture(deferredTexture, GL_COLOR_ATTACHMENT0);

    ambientLightTexture = new Texture2d(width, height, GL_RGBA16F, GL_RGBA, GL_HALF_FLOAT);
    ambientLightFbo = new Framebuffer();
    ambientLightFbo->attachTexture(ambientLightTexture, GL_COLOR_ATTACHMENT0);

    waterTileTexture = new Texture2d(512, 512, GL_R16F, GL_RED, GL_HALF_FLOAT);
    waterTileFbo = new Framebuffer();
    waterTileFbo->attachTexture(waterTileTexture, GL_COLOR_ATTACHMENT0);

    ambientOcclusionTexture = new Texture2d(width, height, GL_RGBA16F, GL_RGBA, GL_HALF_FLOAT);
    ambientOcclusionFbo = new Framebuffer();
    ambientOcclusionFbo->attachTexture(ambientOcclusionTexture, GL_COLOR_ATTACHMENT0);

    fogTexture = new Texture2d(width, height, GL_RGB16F, GL_RGB, GL_HALF_FLOAT);
    fogFbo = new Framebuffer();
    fogFbo->attachTexture(fogTexture, GL_COLOR_ATTACHMENT0);

    //---------/

    cloudsTextureEven = new CubeMapTexture(1024, 1024, GL_RG32F, GL_RG, GL_FLOAT);
    cloudsFboEven = new CubeMapFramebuffer();
    cloudsFboEven->attachTexture(cloudsTextureEven, GL_COLOR_ATTACHMENT0);

    cloudsTextureOdd = new CubeMapTexture(1024, 1024, GL_RG32F, GL_RG, GL_FLOAT);
    cloudsFboOdd = new CubeMapFramebuffer();
    cloudsFboOdd->attachTexture(cloudsTextureOdd, GL_COLOR_ATTACHMENT0);

    //

    cloudsShadowsTextureEven = new CubeMapTexture(1024, 1024, GL_R16F, GL_RED, GL_HALF_FLOAT);
    cloudsShadowsFboEven = new CubeMapFramebuffer();
    cloudsShadowsFboEven->attachTexture(cloudsShadowsTextureEven, GL_COLOR_ATTACHMENT0);

    cloudsShadowsTextureOdd = new CubeMapTexture(1024, 1024, GL_R16F, GL_RED, GL_HALF_FLOAT);
    cloudsShadowsFboOdd = new CubeMapFramebuffer();
    cloudsShadowsFboOdd->attachTexture(cloudsShadowsTextureOdd, GL_COLOR_ATTACHMENT0);

    //

    skyfogTextureEven = new CubeMapTexture(128, 128, GL_R16F, GL_RED, GL_HALF_FLOAT);
    skyfogFboEven = new CubeMapFramebuffer();
    skyfogFboEven->attachTexture(skyfogTextureEven, GL_COLOR_ATTACHMENT0);

    skyfogTextureOdd = new CubeMapTexture(128, 128, GL_R16F, GL_RED, GL_HALF_FLOAT);
    skyfogFboOdd = new CubeMapFramebuffer();
    skyfogFboOdd->attachTexture(skyfogTextureOdd, GL_COLOR_ATTACHMENT0);

    //---------/

    atmScattTexture = new CubeMapTexture(256, 256, GL_RGBA16F, GL_RGBA, GL_HALF_FLOAT);
    atmScattFbo = new CubeMapFramebuffer();
    atmScattFbo->attachTexture(atmScattTexture, GL_COLOR_ATTACHMENT0);

    motionBlurTexture = new Texture2d(width, height, GL_RGBA16F, GL_RGBA, GL_HALF_FLOAT);
    motionBlurFbo = new Framebuffer();
    motionBlurFbo->attachTexture(motionBlurTexture, GL_COLOR_ATTACHMENT0);

    bloomXTexture = new Texture2d(width, height, GL_RGBA16F, GL_RGBA, GL_HALF_FLOAT);
    bloomYTexture = new Texture2d(width, height, GL_RGBA16F, GL_RGBA, GL_HALF_FLOAT);
    bloomFbo = new Framebuffer();
    bloomFbo->attachTexture(bloomXTexture, GL_COLOR_ATTACHMENT0);
    bloomFbo->attachTexture(bloomYTexture, GL_COLOR_ATTACHMENT1);

    combineTexture = new Texture2d(width, height, GL_RGBA16F, GL_RGBA, GL_HALF_FLOAT);
    combineFbo = new Framebuffer();
    combineFbo->attachTexture(combineTexture, GL_COLOR_ATTACHMENT0);

    lensBlurTextureHorizontal = new Texture2d(width, height, GL_RGBA16F, GL_RGBA, GL_HALF_FLOAT);
    lensBlurFboHorizontal = new Framebuffer();
    lensBlurFboHorizontal->attachTexture(lensBlurTextureHorizontal, GL_COLOR_ATTACHMENT0);

    lensBlurTextureVertical = new Texture2d(width, height, GL_RGBA16F, GL_RGBA, GL_HALF_FLOAT);
    lensBlurFboVertical = new Framebuffer();
    lensBlurFboVertical->attachTexture(lensBlurTextureVertical, GL_COLOR_ATTACHMENT0);

}

void Renderer::destroyFbos()
{
    delete mrtAlbedoRoughnessTex;
    delete mrtNormalMetalnessTex;
    delete mrtDistanceTexture;
    delete depthTexture;

    delete deferredFbo;
    delete deferredTexture;

    delete ambientLightFbo;
    delete ambientLightTexture;

    delete ambientOcclusionFbo;
    delete ambientOcclusionTexture;

    delete fogFbo;
    delete fogTexture;

    delete motionBlurFbo;
    delete motionBlurTexture;

    delete bloomFbo;
    delete bloomXTexture;
    delete bloomYTexture;

    delete combineFbo;
    delete combineTexture;
}

void Renderer::setCommonUniforms(ShaderProgram * sp)
{
}

Renderer::~Renderer()
{
    destroyFbos();
    delete quad3dInfo;
    delete skyboxTexture;
    delete deferredShader;
    delete ambientLightShader;
    delete ambientOcclusionShader;
    delete fogShader;
    delete motionBlurShader;
    delete bloomShader;
    delete combineShader;
    delete outputShader;
}

void Renderer::renderToFramebuffer(glm::vec3 position, CubeMapFramebuffer * fboout)
{
    for (int i = 0; i < 6; i++) {
        fboout->use();
        Camera *cam = fboout->switchFace(GL_TEXTURE_CUBE_MAP_POSITIVE_X + i, true);
        currentCamera = cam;
        cam->transformation->setPosition(position);
        draw(cam);
        fboout->use();
        fxaaTonemap();
    }
}

void Renderer::renderToFramebuffer(Camera *camera, Framebuffer * fboout)
{
    currentCamera = camera;
    draw(camera);
    fboout->use(true);
    fxaaTonemap();
}

void Renderer::draw(Camera *camera)
{
    if (!gpuInitialized) {
        float* ones = new float[4] {1.0f, 1.0f, 1.0f, 1.0f};
        exposureBuffer->mapData(4 * 4, &ones);
    }
    csm->map(-sunDirection, camera->transformation->position);
    mrtFbo->use(true);
    Game::instance->world->setUniforms(Game::instance->shaders->materialGeometryShader, camera);
    Game::instance->world->setUniforms(Game::instance->shaders->materialShader, camera);
    Game::instance->world->setSceneUniforms();
    Game::instance->world->draw(Game::instance->shaders->materialShader, camera);
    mrtDistanceTexture->generateMipMaps();
    if (useAmbientOcclusion) {
   //     ambientOcclusion();
    }
    //mrtAlbedoRoughnessTex->setWrapModes(GL_MIRRORED_REPEAT, GL_MIRRORED_REPEAT);
    //mrtNormalMetalnessTex->setWrapModes(GL_MIRRORED_REPEAT, GL_MIRRORED_REPEAT);
    //mrtDistanceTexture->setWrapModes(GL_MIRRORED_REPEAT, GL_MIRRORED_REPEAT);
    //depthTexture->setWrapModes(GL_MIRRORED_REPEAT, GL_MIRRORED_REPEAT);
    deferred();
    ambientLight();
    waterTile();
    atmScatt();
    clouds();
    fog();
    combine();
    lensBlur();
    gpuInitialized = true;
}

void Renderer::bloom()
{
}

void Renderer::combine()
{
    waterTileTexture->use(23);
    exposureBuffer->use(0);
    combineTexture->use(16);
    exposureComputeShader->dispatch(1, 1, 1);
    combineFbo->use(true);
    combineShader->use();
    mrtDistanceTexture->use(2);
    skyboxTexture->use(3);
    deferredTexture->use(5);
    ambientLightTexture->use(6);
    ambientOcclusionTexture->use(16);
    if (!cloudCycleUseOdd) {
        cloudsTextureOdd->use(25);
        cloudsShadowsTextureOdd->use(26);
        skyfogTextureOdd->use(27);
    }
    else {
        cloudsTextureEven->use(25);
        cloudsShadowsTextureEven->use(26);
        skyfogTextureEven->use(27);
    }
    atmScattTexture->use(19);
    fogTexture->use(20);
    FrustumCone *cone = currentCamera->cone;
    //   outputShader->setUniform("VPMatrix", vpmatrix);
    glm::mat4 vpmatrix = currentCamera->projectionMatrix * currentCamera->transformation->getInverseWorldTransform();
    combineShader->setUniform("VPMatrix", vpmatrix);
    combineShader->setUniform("UseAO", useAmbientOcclusion);
    combineShader->setUniform("MieScattCoeff", mieScattCoefficent);
    combineShader->setUniform("UseGamma", useGammaCorrection);
    combineShader->setUniform("Resolution", glm::vec2(width, height));
    combineShader->setUniform("CameraPosition", currentCamera->transformation->position);
    combineShader->setUniform("FrustumConeLeftBottom", cone->leftBottom);
    combineShader->setUniform("FrustumConeBottomLeftToBottomRight", cone->rightBottom - cone->leftBottom);
    combineShader->setUniform("FrustumConeBottomLeftToTopLeft", cone->leftTop - cone->leftBottom);
    double t = glfwGetTime();
    double t100 = t * 100.0;
    double t001 = t * 0.001;
    combineShader->setUniform("Time", (float)t);
    combineShader->setUniform("T100", (float)t100);
    combineShader->setUniform("T001", (float)t001);

    combineShader->setUniform("CloudsFloor", cloudsFloor);
    combineShader->setUniform("CloudsCeil", cloudsCeil);
    combineShader->setUniform("CloudsThresholdLow", cloudsThresholdLow);
    combineShader->setUniform("CloudsThresholdHigh", cloudsThresholdHigh);
    combineShader->setUniform("CloudsWindSpeed", cloudsWindSpeed);
    combineShader->setUniform("CloudsOffset", cloudsOffset);
    combineShader->setUniform("SunDirection", glm::normalize(sunDirection));
    combineShader->setUniform("AtmosphereScale", atmosphereScale);
    combineShader->setUniform("CloudsDensityScale", cloudsDensityScale);
    combineShader->setUniform("CloudsDensityThresholdLow", cloudsDensityThresholdLow);
    combineShader->setUniform("CloudsDensityThresholdHigh", cloudsDensityThresholdHigh);
    combineShader->setUniform("WaterWavesScale", waterWavesScale);
    combineShader->setUniform("NoiseOctave1", noiseOctave1);
    combineShader->setUniform("NoiseOctave2", noiseOctave2);
    combineShader->setUniform("NoiseOctave3", noiseOctave3);
    combineShader->setUniform("NoiseOctave4", noiseOctave4);
    combineShader->setUniform("NoiseOctave5", noiseOctave5);
    combineShader->setUniform("NoiseOctave6", noiseOctave6);
    combineShader->setUniform("NoiseOctave7", noiseOctave7);
    combineShader->setUniform("NoiseOctave8", noiseOctave8);
    csm->setUniformsAndBindSampler(combineShader, 24);
    quad3dInfo->draw();
    combineTexture->generateMipMaps();
}
void Renderer::fxaaTonemap()
{
    fxaaTonemapShader->use();
    exposureBuffer->use(0);
    lensBlurTextureVertical->use(16);
    FrustumCone *cone = currentCamera->cone;
    fxaaTonemapShader->setUniform("Resolution", glm::vec2(width, height));
    fxaaTonemapShader->setUniform("CameraPosition", currentCamera->transformation->position);
    fxaaTonemapShader->setUniform("FrustumConeLeftBottom", cone->leftBottom);
    fxaaTonemapShader->setUniform("FrustumConeBottomLeftToBottomRight", cone->rightBottom - cone->leftBottom);
    fxaaTonemapShader->setUniform("FrustumConeBottomLeftToTopLeft", cone->leftTop - cone->leftBottom);
    fxaaTonemapShader->setUniform("Time", Game::instance->time);

    quad3dInfo->draw();


    Game::instance->firstFullDrawFinished = true;
}
    
void Renderer::lensBlur()
{
    lensBlurFboVertical->use(true);
    lensBlurShader->use();
    mrtDistanceTexture->use(2);
    combineTexture->use(16);
    FrustumCone *cone = currentCamera->cone;
    lensBlurShader->setUniform("Resolution", glm::vec2(width, height));
    lensBlurShader->setUniform("CameraPosition", currentCamera->transformation->position);
    lensBlurShader->setUniform("LensBlurSize", lensBlurSize);
    lensBlurShader->setUniform("FrustumConeLeftBottom", cone->leftBottom);
    lensBlurShader->setUniform("FrustumConeBottomLeftToBottomRight", cone->rightBottom - cone->leftBottom);
    lensBlurShader->setUniform("FrustumConeBottomLeftToTopLeft", cone->leftTop - cone->leftBottom);
    lensBlurShader->setUniform("Time", Game::instance->time);

    quad3dInfo->draw();
    lensBlurTextureVertical->generateMipMaps();
}

void Renderer::output()
{

}

void Renderer::recompileShaders()
{
    deferredShader->recompile();
    ambientLightShader->recompile();
    ambientOcclusionShader->recompile();
    envProbesShader->recompile();
    cloudsShader->recompile();
    atmScattShader->recompile();
    fogShader->recompile();
    combineShader->recompile();
    lensBlurShader->recompile();
    fxaaTonemapShader->recompile();
    exposureComputeShader->recompile();
    waterTileShader->recompile();
}

void Renderer::deferred()
{
    if (!gpuInitialized) return;
    vector<Light*> lights = Game::instance->world->scene->getLights();

    deferredFbo->use(true);
    FrustumCone *cone = currentCamera->cone;
    glm::mat4 vpmatrix = currentCamera->projectionMatrix * currentCamera->transformation->getInverseWorldTransform();
    deferredShader->use();
    deferredShader->setUniform("UseAO", useAmbientOcclusion);
    deferredShader->setUniform("VPMatrix", vpmatrix);
    deferredShader->setUniform("Resolution", glm::vec2(width, height));
    deferredShader->setUniform("CameraPosition", currentCamera->transformation->position);
    deferredShader->setUniform("FrustumConeLeftBottom", cone->leftBottom);
    deferredShader->setUniform("FrustumConeBottomLeftToBottomRight", cone->rightBottom - cone->leftBottom);
    deferredShader->setUniform("FrustumConeBottomLeftToTopLeft", cone->leftTop - cone->leftBottom);
    mrtAlbedoRoughnessTex->use(0);
    mrtNormalMetalnessTex->use(1);
    mrtDistanceTexture->use(2);
    ambientOcclusionTexture->use(16);
    glCullFace(GL_FRONT);
    glEnable(GL_BLEND);
    glBlendFunc(GL_ONE, GL_ONE);

    for (int i = 0; i < lights.size(); i++) {
        deferredShader->setUniform("LightColor", lights[i]->color);
        deferredShader->setUniform("LightPosition", lights[i]->transformation->position);
        deferredShader->setUniform("LightOrientation", glm::inverse(lights[i]->transformation->orientation));
        deferredShader->setUniform("LightAngle", lights[i]->angle);
        deferredShader->setUniform("LightType", lights[i]->type);
        deferredShader->setUniform("LightCutOffDistance", lights[i]->cutOffDistance);
        deferredShader->setUniform("LightUseShadowMap", lights[i]->shadowMappingEnabled);
        lights[i]->transformation->setSize(glm::vec3(lights[i]->cutOffDistance));
        deferredShader->setUniform("LightMMatrix", lights[i]->transformation->getWorldTransform());
        if (lights[i]->shadowMappingEnabled) {
            deferredShader->setUniform("LightVPMatrix", lights[i]->lightCamera->projectionMatrix
                * lights[i]->transformation->getInverseWorldTransform());
            lights[i]->bindShadowMap(14, 15);
        }
        sphere3dInfo->draw();
    }

    vector<EnvProbe*> probes = Game::instance->world->scene->getEnvProbes();

    glCullFace(GL_BACK);
    envProbesShader->use();
    envProbesShader->setUniform("UseAO", useAmbientOcclusion);
    envProbesShader->setUniform("VPMatrix", vpmatrix);
    envProbesShader->setUniform("Resolution", glm::vec2(width, height));
    envProbesShader->setUniform("EnvProbesLightMultiplier", envProbesLightMultiplier);
    envProbesShader->setUniform("CameraPosition", currentCamera->transformation->position);
    envProbesShader->setUniform("FrustumConeLeftBottom", cone->leftBottom);
    envProbesShader->setUniform("FrustumConeBottomLeftToBottomRight", cone->rightBottom - cone->leftBottom);
    envProbesShader->setUniform("FrustumConeBottomLeftToTopLeft", cone->leftTop - cone->leftBottom);
    for (int i = 0; i < probes.size(); i++) {
        probes[i]->texture->use(10);
        probes[i]->setUniforms();
        quad3dInfo->draw();
    }

    glDisable(GL_BLEND);
}

void Renderer::ambientLight()
{
    ambientLightFbo->use(true);
    ambientLightShader->use();
    mrtAlbedoRoughnessTex->use(0);
    mrtNormalMetalnessTex->use(1);
    mrtDistanceTexture->use(2);
    skyboxTexture->use(3);
    FrustumCone *cone = currentCamera->cone;
    glm::mat4 vpmatrix = currentCamera->projectionMatrix * currentCamera->transformation->getInverseWorldTransform();
    ambientLightShader->setUniform("VPMatrix", vpmatrix);
    ambientLightShader->setUniform("Resolution", glm::vec2(width, height));
    ambientLightShader->setUniform("CameraPosition", currentCamera->transformation->position);
    ambientLightShader->setUniform("FrustumConeLeftBottom", cone->leftBottom);
    ambientLightShader->setUniform("FrustumConeBottomLeftToBottomRight", cone->rightBottom - cone->leftBottom);
    ambientLightShader->setUniform("FrustumConeBottomLeftToTopLeft", cone->leftTop - cone->leftBottom);
    ambientLightShader->setUniform("Time", Game::instance->time);
    ambientLightShader->setUniform("SunDirection", glm::normalize(sunDirection));
    quad3dInfo->draw();
}

void Renderer::waterTile()
{
    waterTileTexture->setWrapModes(GL_MIRRORED_REPEAT, GL_MIRRORED_REPEAT);
    waterTileFbo->use(true);
    waterTileShader->use();
    waterTileShader->setUniform("Resolution", glm::vec2(width, height));
    waterTileShader->setUniform("Time", Game::instance->time);
    quad3dInfo->draw();
    waterTileTexture->generateMipMaps();
}

void Renderer::ambientOcclusion()
{
    ambientOcclusionFbo->use(true);
    ambientOcclusionShader->use();
    mrtAlbedoRoughnessTex->use(0);
    mrtNormalMetalnessTex->use(1);
    mrtDistanceTexture->use(2);
    FrustumCone *cone = currentCamera->cone;
    glm::mat4 vpmatrix = currentCamera->projectionMatrix * currentCamera->transformation->getInverseWorldTransform();
    ambientOcclusionShader->setUniform("VPMatrix", vpmatrix);
    ambientOcclusionShader->setUniform("Resolution", glm::vec2(width, height));
    ambientOcclusionShader->setUniform("SunDirection", glm::normalize(sunDirection));
    ambientOcclusionShader->setUniform("CameraPosition", currentCamera->transformation->position);
    ambientOcclusionShader->setUniform("FrustumConeLeftBottom", cone->leftBottom);
    ambientOcclusionShader->setUniform("FrustumConeBottomLeftToBottomRight", cone->rightBottom - cone->leftBottom);
    ambientOcclusionShader->setUniform("FrustumConeBottomLeftToTopLeft", cone->leftTop - cone->leftBottom);
    ambientOcclusionShader->setUniform("Time", Game::instance->time);
    quad3dInfo->draw();
}

void Renderer::fog()
{

    fogFbo->use(true);
    fogShader->use();
    mrtDistanceTexture->use(2);
    skyboxTexture->use(3);
    deferredTexture->use(5);
    ambientLightTexture->use(6);
    ambientOcclusionTexture->use(16);
    if (!cloudCycleUseOdd)
        cloudsTextureOdd->use(18);
    else
        cloudsTextureEven->use(18);
    atmScattTexture->use(19);
    FrustumCone *cone = currentCamera->cone;
    //   outputShader->setUniform("VPMatrix", vpmatrix);
    glm::mat4 vpmatrix = currentCamera->projectionMatrix * currentCamera->transformation->getInverseWorldTransform();
    fogShader->setUniform("VPMatrix", vpmatrix);
    fogShader->setUniform("UseAO", useAmbientOcclusion);
    fogShader->setUniform("UseGamma", useGammaCorrection);
    fogShader->setUniform("Resolution", glm::vec2(width, height));
    fogShader->setUniform("CameraPosition", currentCamera->transformation->position);
    fogShader->setUniform("FrustumConeLeftBottom", cone->leftBottom);
    fogShader->setUniform("FrustumConeBottomLeftToBottomRight", cone->rightBottom - cone->leftBottom);
    fogShader->setUniform("FrustumConeBottomLeftToTopLeft", cone->leftTop - cone->leftBottom);
    double t = glfwGetTime();
    double t100 = t * 100.0;
    double t001 = t * 0.001;
    fogShader->setUniform("Time", (float)t);
    fogShader->setUniform("T100", (float)t100);
    fogShader->setUniform("T001", (float)t001);

    fogShader->setUniform("CloudsFloor", cloudsFloor);
    fogShader->setUniform("CloudsCeil", cloudsCeil);
    fogShader->setUniform("CloudsThresholdLow", cloudsThresholdLow);
    fogShader->setUniform("CloudsThresholdHigh", cloudsThresholdHigh);
    fogShader->setUniform("CloudsWindSpeed", cloudsWindSpeed);
    fogShader->setUniform("CloudsOffset", cloudsOffset);
    fogShader->setUniform("SunDirection", glm::normalize(sunDirection));
    fogShader->setUniform("AtmosphereScale", atmosphereScale);
    fogShader->setUniform("CloudsDensityScale", cloudsDensityScale);
    fogShader->setUniform("CloudsDensityThresholdLow", cloudsDensityThresholdLow);
    fogShader->setUniform("CloudsDensityThresholdHigh", cloudsDensityThresholdHigh);
    fogShader->setUniform("WaterWavesScale", waterWavesScale);
    fogShader->setUniform("NoiseOctave1", noiseOctave1);
    fogShader->setUniform("NoiseOctave2", noiseOctave2);
    fogShader->setUniform("NoiseOctave3", noiseOctave3);
    fogShader->setUniform("NoiseOctave4", noiseOctave4);
    fogShader->setUniform("NoiseOctave5", noiseOctave5);
    fogShader->setUniform("NoiseOctave6", noiseOctave6);
    fogShader->setUniform("NoiseOctave7", noiseOctave7);
    fogShader->setUniform("NoiseOctave8", noiseOctave8);
    csm->setUniformsAndBindSampler(fogShader, 24);
    quad3dInfo->draw();
}

void Renderer::atmScatt()
{
    glDisable(GL_BLEND);
    atmScattShader->use();
    atmScattShader->setUniform("Time", Game::instance->time);
    atmScattShader->setUniform("SunDirection", glm::normalize(sunDirection));
    atmScattShader->setUniform("Resolution", glm::vec2(atmScattTexture->width, atmScattTexture->height));
    atmScattShader->setUniform("MieScattCoeff", mieScattCoefficent);

    atmScattFbo->use(false);
    for (int i = 0; i < 6; i++) {
        Camera* camera = atmScattFbo->switchFace(GL_TEXTURE_CUBE_MAP_POSITIVE_X + i, true);
        camera->transformation->setPosition(currentCamera->transformation->position);
        FrustumCone *cone = camera->cone;
        glm::mat4 vpmatrix = camera->projectionMatrix * camera->transformation->getInverseWorldTransform();
        glm::mat4 cameraRotMatrix = camera->transformation->getRotationMatrix();
        glm::mat4 rpmatrix = camera->projectionMatrix * inverse(cameraRotMatrix);
        camera->cone->update(inverse(rpmatrix));
        atmScattShader->setUniform("VPMatrix", vpmatrix);
        atmScattShader->setUniform("CameraPosition", camera->transformation->position);
        atmScattShader->setUniform("FrustumConeLeftBottom", cone->leftBottom);
        atmScattShader->setUniform("FrustumConeBottomLeftToBottomRight", cone->rightBottom - cone->leftBottom);
        atmScattShader->setUniform("FrustumConeBottomLeftToTopLeft", cone->leftTop - cone->leftBottom);
        quad3dInfo->draw();
    }

    atmScattTexture->generateMipMaps();
}

void Renderer::clouds()
{
    return;
    glDisable(GL_BLEND);

    mrtAlbedoRoughnessTex->use(0);
    mrtNormalMetalnessTex->use(1);
    mrtDistanceTexture->use(2);


 //   if (cloudCycleUseOdd) 
  //      cloudsTextureEven->use(22);
  //  else 
  //      cloudsTextureOdd->use(18);



    cloudsShader->use();
    cloudsShader->setUniform("Time", Game::instance->time);
    cloudsShader->setUniform("CloudsFloor", cloudsFloor);
    cloudsShader->setUniform("CloudsCeil", cloudsCeil);
    cloudsShader->setUniform("MieScattCoeff", mieScattCoefficent);
    cloudsShader->setUniform("CloudsThresholdLow", cloudsThresholdLow);
    cloudsShader->setUniform("CloudsThresholdHigh", cloudsThresholdHigh);
    cloudsShader->setUniform("CloudsWindSpeed", cloudsWindSpeed);
    cloudsShader->setUniform("CloudsOffset", cloudsOffset);
    cloudsShader->setUniform("SunDirection", glm::normalize(sunDirection));
    cloudsShader->setUniform("AtmosphereScale", atmosphereScale);
    cloudsShader->setUniform("CloudsDensityScale", cloudsDensityScale);
    cloudsShader->setUniform("CloudsDensityThresholdLow", cloudsDensityThresholdLow);
    cloudsShader->setUniform("CloudsDensityThresholdHigh", cloudsDensityThresholdHigh);
    cloudsShader->setUniform("WaterWavesScale", waterWavesScale);
    cloudsShader->setUniform("NoiseOctave1", noiseOctave1);
    cloudsShader->setUniform("NoiseOctave2", noiseOctave2);
    cloudsShader->setUniform("NoiseOctave3", noiseOctave3);
    cloudsShader->setUniform("NoiseOctave4", noiseOctave4);
    cloudsShader->setUniform("NoiseOctave5", noiseOctave5);
    cloudsShader->setUniform("NoiseOctave6", noiseOctave6);
    cloudsShader->setUniform("NoiseOctave7", noiseOctave7);
    cloudsShader->setUniform("NoiseOctave8", noiseOctave8);
    cloudsShader->setUniform("CloudsIntegrate", cloudsIntegrate);
    cloudsShader->setUniform("Resolution", glm::vec2(cloudsTextureOdd->width, cloudsTextureOdd->height));
    srand(static_cast <unsigned> (Game::instance->time * 1000.0));
    cloudsShader->setUniform("Rand1", static_cast <float> (rand()) / static_cast <float> (RAND_MAX));
    cloudsShader->setUniform("Rand2", static_cast <float> (rand()) / static_cast <float> (RAND_MAX));

    // RENDER CLOUD COVERAGE::
    cloudsShader->setUniform("RenderPass", 0);

    CubeMapFramebuffer* currentFbo = cloudsFboOdd;
    if (cloudCycleUseOdd)
        currentFbo = cloudsFboOdd;
    else
        currentFbo = cloudsFboEven;
    if (cloudCycleUseOdd) 
        cloudsTextureEven->use(18);
    else 
        cloudsTextureOdd->use(18);

   // for (int i = 0; i < 6; i++) {
        currentFbo->use(false);
        Camera* camera = currentFbo->switchFace(GL_TEXTURE_CUBE_MAP_POSITIVE_X + cloudFace, true);
        camera->transformation->setPosition(currentCamera->transformation->position);
        FrustumCone *cone = camera->cone;
        glm::mat4 vpmatrix = camera->projectionMatrix * camera->transformation->getInverseWorldTransform();
        glm::mat4 cameraRotMatrix = camera->transformation->getRotationMatrix();
        glm::mat4 rpmatrix = camera->projectionMatrix * inverse(cameraRotMatrix);
        camera->cone->update(inverse(rpmatrix));
        cloudsShader->use();
        cloudsShader->setUniform("VPMatrix", vpmatrix);
        cloudsShader->setUniform("CameraPosition", camera->transformation->position);
        cloudsShader->setUniform("FrustumConeLeftBottom", cone->leftBottom);
        cloudsShader->setUniform("FrustumConeBottomLeftToBottomRight", cone->rightBottom - cone->leftBottom);
        cloudsShader->setUniform("FrustumConeBottomLeftToTopLeft", cone->leftTop - cone->leftBottom);
        currentFbo->use(false);
        cloudsShader->use();
        quad3dInfo->draw();
   // }

    if (cloudCycleUseOdd)
        cloudsTextureOdd->generateMipMaps();
    else
        cloudsTextureEven->generateMipMaps();

    // RENDER SHADOWS
    cloudsShader->setUniform("RenderPass", 1);

    currentFbo = cloudsShadowsFboOdd;
    if (cloudCycleUseOdd)
        currentFbo = cloudsShadowsFboOdd;
    else
        currentFbo = cloudsShadowsFboEven;

    if (cloudCycleUseOdd)
        cloudsShadowsTextureEven->use(18);
    else
        cloudsShadowsTextureOdd->use(18);

    if (cloudCycleUseOdd)
        cloudsTextureEven->use(29);
    else
        cloudsTextureOdd->use(29);

    // for (int i = 0; i < 6; i++) {
    currentFbo->use(false);
    camera = currentFbo->switchFace(GL_TEXTURE_CUBE_MAP_POSITIVE_X + cloudFace, true);
    camera->transformation->setPosition(currentCamera->transformation->position);
    cone = camera->cone;
    vpmatrix = camera->projectionMatrix * camera->transformation->getInverseWorldTransform();
    cameraRotMatrix = camera->transformation->getRotationMatrix();
    rpmatrix = camera->projectionMatrix * inverse(cameraRotMatrix);
    camera->cone->update(inverse(rpmatrix));
    cloudsShader->use();
    cloudsShader->setUniform("VPMatrix", vpmatrix);
    cloudsShader->setUniform("CameraPosition", camera->transformation->position);
    cloudsShader->setUniform("FrustumConeLeftBottom", cone->leftBottom);
    cloudsShader->setUniform("FrustumConeBottomLeftToBottomRight", cone->rightBottom - cone->leftBottom);
    cloudsShader->setUniform("FrustumConeBottomLeftToTopLeft", cone->leftTop - cone->leftBottom);
    currentFbo->use(false);
    cloudsShader->use();
    quad3dInfo->draw();
    // }

    if (cloudCycleUseOdd)
        cloudsShadowsTextureOdd->generateMipMaps();
    else
        cloudsShadowsTextureEven->generateMipMaps();

    // RENDER FOG

    cloudsShader->setUniform("RenderPass", 2);

    currentFbo = skyfogFboOdd;
    if (cloudCycleUseOdd)
        currentFbo = skyfogFboOdd;
    else
        currentFbo = skyfogFboEven;
    if (cloudCycleUseOdd)
        skyfogTextureEven->use(18);
    else
        skyfogTextureOdd->use(18);

    if (cloudCycleUseOdd)
        cloudsTextureEven->use(29);
    else
        cloudsTextureOdd->use(29);

    // for (int i = 0; i < 6; i++) {
    currentFbo->use(false);
    camera = currentFbo->switchFace(GL_TEXTURE_CUBE_MAP_POSITIVE_X + cloudFace, true);
    camera->transformation->setPosition(currentCamera->transformation->position);
    cone = camera->cone;
    vpmatrix = camera->projectionMatrix * camera->transformation->getInverseWorldTransform();
    cameraRotMatrix = camera->transformation->getRotationMatrix();
    rpmatrix = camera->projectionMatrix * inverse(cameraRotMatrix);
    camera->cone->update(inverse(rpmatrix));
    cloudsShader->use();
    cloudsShader->setUniform("VPMatrix", vpmatrix);
    cloudsShader->setUniform("CameraPosition", camera->transformation->position);
    cloudsShader->setUniform("FrustumConeLeftBottom", cone->leftBottom);
    cloudsShader->setUniform("FrustumConeBottomLeftToBottomRight", cone->rightBottom - cone->leftBottom);
    cloudsShader->setUniform("FrustumConeBottomLeftToTopLeft", cone->leftTop - cone->leftBottom);
    currentFbo->use(false);
    cloudsShader->use();
    quad3dInfo->draw();
    // }

    if (cloudCycleUseOdd)
        skyfogTextureOdd->generateMipMaps();
    else
        skyfogTextureEven->generateMipMaps();

    cloudFace++;
    if (cloudFace > 5) {
        cloudFace = 0;
        cloudCycleUseOdd = !cloudCycleUseOdd;
    }
}

void Renderer::motionBlur()
{
}
#include "stdafx.h"
#include "Renderer.h"
#include "Game.h"
#include "FrustumCone.h"
#include "Media.h"

Renderer::Renderer(int iwidth, int iheight)
{
    config = new INIReader("renderer.ini");

    envProbesLightMultiplier = 1.0;
    width = iwidth;
    height = iheight;

    useAmbientOcclusion = false;
    useGammaCorrection = true;

    cloudsFloor = 3333;
    cloudsCeil = 7700;
    cloudsThresholdLow = 0.53;
    cloudsThresholdHigh = 0.56;
    cloudsDensityThresholdLow = 0.0;
    cloudsDensityThresholdHigh = 1.0;
    cloudsDensityScale = 0.7;
    cloudsWindSpeed = 0.4;

    noiseOctave1 = 0.0001;
    noiseOctave2 = 1.02;
    noiseOctave3 = 1.03;
    noiseOctave4 = 0.0;
    noiseOctave5 = 0.5;
    noiseOctave6 = 4.0;
    noiseOctave7 = 1.01;
    noiseOctave8 = 1.01;
    exposure = 1.0;
    contrast = 1.0;
    cloudsIntegrate = 0.98;
    mieScattCoefficent = 1.0;
    nightSkyLightPollution = 0.2;
    lensBlurSize = 0.0;
    waterScale = glm::vec2(1.0f, 1.5f);
    waterHeight = 0.3f;
    waterSpeed = 1.0f;
    wind = glm::normalize(glm::vec3(0.3, 0.0, 0.3));
    gpuInitialized = false;

    csm = new CascadeShadowMap(1024, 1024, { 8, 32, 64, 256, 1024 * 4 });
   // csm = new CascadeShadowMap(0, 0, {});

    cloudsOffset = glm::vec3(1);
    dayElapsed = 0.5;
    yearElapsed = 0.5;
    equatorPoleMix = 0.5;

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

    /* unsigned char* bytes;
     int bytescount = Media::readBinary("deferredsphere.raw", &bytes);
     GLfloat * floats = (GLfloat*)bytes;
     int floatsCount = bytescount / 4;
     vector<GLfloat> flo(floats, floats + floatsCount);

     sphere3dInfo = new Object3dInfo(flo);*/

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
    waterMeshShader = new ShaderProgram("PostProcess.vertex.glsl", "WaterMesh.fragment.glsl");
    waterColorShader = new ShaderProgram("PostProcess.vertex.glsl", "WaterColor.fragment.glsl");
    cloudResolveShader = new ShaderProgram("PostProcess.vertex.glsl", "CloudResolve.fragment.glsl");
    exposureComputeShader = new ShaderProgram("CalculateExposure.compute.glsl");
    pickingReadShader = new ShaderProgram("PickerResultReader.compute.glsl");
    exposureBuffer = new ShaderStorageBuffer();

    // skyboxTexture = new CubeMapTexture("posx.jpg", "posy.jpg", "posz.jpg", "negx.jpg", "negy.jpg", "negz.jpg");
    fresnelTexture = new Texture2d("fresnel.png");
    starsTexture = new Texture2d("stars.png");
    moonTexture = new Texture2d("moon.png");
    initializeFbos();
}

void Renderer::resize(int iwidth, int iheight)
{
    width = iwidth;
    height = iheight;
    destroyFbos(true);
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

    pickingDataTex = new Texture2d(width / 4, height / 4, GL_RGBA32UI, GL_RGBA_INTEGER, GL_UNSIGNED_INT);
    pickingWorldPosTex = new Texture2d(width / 4, height / 4, GL_RGBA32F, GL_RGBA, GL_FLOAT);
    pickingNormalTex = new Texture2d(width / 4, height / 4, GL_RGBA32F, GL_RGBA, GL_FLOAT);
    pickingDepthTexture = new Texture2d(width / 4, height / 4, GL_DEPTH_COMPONENT32F, GL_DEPTH_COMPONENT, GL_FLOAT);

    pickingFbo = new Framebuffer();
    pickingFbo->attachTexture(pickingDataTex, GL_COLOR_ATTACHMENT0);
    pickingFbo->attachTexture(pickingWorldPosTex, GL_COLOR_ATTACHMENT1);
    pickingFbo->attachTexture(pickingNormalTex, GL_COLOR_ATTACHMENT2);
    pickingFbo->attachTexture(pickingDepthTexture, GL_DEPTH_ATTACHMENT);
    pickingResultSSBO = new ShaderStorageBuffer();

    deferredTexture = new Texture2d(width, height, GL_RGBA16F, GL_RGBA, GL_HALF_FLOAT);
    deferredFbo = new Framebuffer();
    deferredFbo->attachTexture(deferredTexture, GL_COLOR_ATTACHMENT0);

    ambientLightTexture = new Texture2d(width, height, GL_RGBA16F, GL_RGBA, GL_HALF_FLOAT);
    ambientLightFbo = new Framebuffer();
    ambientLightFbo->attachTexture(ambientLightTexture, GL_COLOR_ATTACHMENT0);

    waterTileTexture1 = new Texture2d(config->geti("water_tile_resolution"), config->geti("water_tile_resolution"), GL_RGBA16F, GL_RGBA, GL_HALF_FLOAT);
    waterTileFbo1 = new Framebuffer();
    waterTileFbo1->attachTexture(waterTileTexture1, GL_COLOR_ATTACHMENT0);

    waterTileTexture2 = new Texture2d(config->geti("water_tile_resolution"), config->geti("water_tile_resolution"), GL_RGBA16F, GL_RGBA, GL_HALF_FLOAT);
    waterTileFbo2 = new Framebuffer();
    waterTileFbo2->attachTexture(waterTileTexture2, GL_COLOR_ATTACHMENT0);

    ambientOcclusionTexture = new Texture2d(width, height, GL_RGBA16F, GL_RGBA, GL_HALF_FLOAT);
    ambientOcclusionFbo = new Framebuffer();
    ambientOcclusionFbo->attachTexture(ambientOcclusionTexture, GL_COLOR_ATTACHMENT0);

    fogTexture = new Texture2d(width, height, GL_RGB16F, GL_RGB, GL_HALF_FLOAT);
    fogFbo = new Framebuffer();
    fogFbo->attachTexture(fogTexture, GL_COLOR_ATTACHMENT0);

    //----*-*-*-*-*-*-*/

    waterMeshTexture = new Texture2d(width * config->getf("water_mesh_resolution_multiplier"), height * config->getf("water_mesh_resolution_multiplier"), GL_R32F, GL_RED, GL_FLOAT);
    waterMeshFbo = new Framebuffer();
    waterMeshFbo->attachTexture(waterMeshTexture, GL_COLOR_ATTACHMENT0);

    waterColorTexture = new Texture2d(width * config->getf("water_color_resolution_multiplier"), height * config->getf("water_color_resolution_multiplier"), GL_RGBA16F, GL_RGBA, GL_HALF_FLOAT);
    waterColorFbo = new Framebuffer();
    waterColorFbo->attachTexture(waterColorTexture, GL_COLOR_ATTACHMENT0);

    //---------/

    cloudsTextureEven = new CubeMapTexture(config->geti("clouds_coverage_resolution"), config->geti("clouds_coverage_resolution"), GL_RG16F, GL_RG, GL_HALF_FLOAT);
    cloudsFboEven = new CubeMapFramebuffer();
    cloudsFboEven->attachTexture(cloudsTextureEven, GL_COLOR_ATTACHMENT0);

    cloudsTextureOdd = new CubeMapTexture(config->geti("clouds_coverage_resolution"), config->geti("clouds_coverage_resolution"), GL_RG16F, GL_RG, GL_HALF_FLOAT);
    cloudsFboOdd = new CubeMapFramebuffer();
    cloudsFboOdd->attachTexture(cloudsTextureOdd, GL_COLOR_ATTACHMENT0);

    //

    cloudsShadowsTextureEven = new CubeMapTexture(config->geti("clouds_shadows_resolution"), config->geti("clouds_shadows_resolution"), GL_RGBA16F, GL_RGBA, GL_HALF_FLOAT);
    cloudsShadowsFboEven = new CubeMapFramebuffer();
    cloudsShadowsFboEven->attachTexture(cloudsShadowsTextureEven, GL_COLOR_ATTACHMENT0);

    cloudsShadowsTextureOdd = new CubeMapTexture(config->geti("clouds_shadows_resolution"), config->geti("clouds_shadows_resolution"), GL_RGBA16F, GL_RGBA, GL_HALF_FLOAT);
    cloudsShadowsFboOdd = new CubeMapFramebuffer();
    cloudsShadowsFboOdd->attachTexture(cloudsShadowsTextureOdd, GL_COLOR_ATTACHMENT0);

    cloudsResolvedTexture = new CubeMapTexture(512, 512, GL_RGBA16F, GL_RGBA, GL_HALF_FLOAT);
    cloudsResolvedFbo = new CubeMapFramebuffer();
    cloudsResolvedFbo->attachTexture(cloudsResolvedTexture, GL_COLOR_ATTACHMENT0);

    //---------/

    atmScattTexture = new CubeMapTexture(512, 512, GL_RGBA16F, GL_RGBA, GL_HALF_FLOAT);
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

void Renderer::destroyFbos(bool onlyViewDependant)
{
    if (!onlyViewDependant) {
        delete skyboxTexture;
        delete waterTileFbo1;
        delete waterTileTexture1;
        delete waterTileFbo2;
        delete waterTileTexture2;
        delete atmScattFbo;
        delete atmScattTexture;
        delete cloudsFboEven;
        delete cloudsTextureEven;
        delete cloudsFboOdd;
        delete cloudsTextureOdd;
        delete cloudsShadowsFboEven;
        delete cloudsShadowsTextureEven;
        delete cloudsShadowsFboOdd;
        delete cloudsShadowsTextureOdd;
    }
    delete mrtFbo;
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
    delete waterMeshFbo;
    delete waterMeshTexture;
    delete waterColorFbo;
    delete waterColorTexture;
    delete motionBlurFbo;
    delete motionBlurTexture;
    delete bloomFbo;
    delete bloomXTexture;
    delete bloomYTexture;
    delete combineFbo;
    delete combineTexture;
    delete lensBlurFboHorizontal;
    delete lensBlurFboVertical;
    delete lensBlurTextureHorizontal;
    delete lensBlurTextureVertical;
}
using namespace glm; 
mat3 rotationMatrix(vec3 axis, float angle)
{
    axis = normalize(axis);
    float s = sin(angle);
    float c = cos(angle);
    float oc = 1.0 - c;

    return mat3(oc * axis.x * axis.x + c, oc * axis.x * axis.y - axis.z * s, oc * axis.z * axis.x + axis.y * s,
        oc * axis.x * axis.y + axis.z * s, oc * axis.y * axis.y + c, oc * axis.y * axis.z - axis.x * s,
        oc * axis.z * axis.x - axis.y * s, oc * axis.y * axis.z + axis.x * s, oc * axis.z * axis.z + c);
}

vec3 transformDirDays(vec3 dir, float elapsed, float yearelapsed, float equator_pole) {
    dir = dir *rotationMatrix(vec3(1.0, 0.0, 0.0), 3.1415 * equator_pole);// move to geo coords
    dir = dir *rotationMatrix(vec3(0.0, 1.0, 0.0), 6.2831 * elapsed);// rotate around rotationaxis
    dir = dir *rotationMatrix(vec3(0.0, 1.0, 0.0), -6.2831 * yearelapsed);
    dir = dir *rotationMatrix(vec3(0.0, 0.0, 1.0), 0.4);
    return dir;
}

DayData calculateDay(float elapsed, float yearelapsed, float equator_pole) {
    vec3 sunorigin = vec3(0.0);
    vec3 earthpos = sunorigin + rotationMatrix(vec3(0.0, 1.0, 0.0), 6.2831 * yearelapsed) * vec3(0.0, 0.0, 1.0) * 149597.870f;

    mat3 surface_frame = mat3(
        transformDirDays(vec3(1.0, 0.0, 0.0), elapsed, yearelapsed, equator_pole),
        transformDirDays(vec3(0.0, 1.0, 0.0), elapsed, yearelapsed, equator_pole),
        transformDirDays(vec3(0.0, 0.0, 1.0), elapsed, yearelapsed, equator_pole)
    );

    vec3 moonpos = earthpos + rotationMatrix(vec3(0.0, 0.0, 1.0), 6.2831 * 0.1 * yearelapsed) * rotationMatrix(vec3(0.0, 1.0, 0.0), 6.2831 * yearelapsed * 12.0) * vec3(0.0, 0.0, 1.0) * 384.402f;
    // earthpos += surfacepos_earthorbitspace;
    auto dd = DayData();
    dd.sunDir = inverse(surface_frame) * normalize(sunorigin - earthpos);
    dd.moonDir = inverse(surface_frame) * normalize(moonpos - earthpos);
    dd.sunSpaceDir = -normalize(sunorigin - moonpos);
    dd.viewFrame = surface_frame;
    dd.moonPos = moonpos;
    dd.earthPos = earthpos;
    return dd;
}

void Renderer::setCommonUniforms(ShaderProgram * sp)
{
    FrustumCone *cone = currentCamera->cone;
    //   outputShader->setUniform("VPMatrix", vpmatrix);
    glm::mat4 vpmatrix = currentCamera->projectionMatrix * currentCamera->transformation->getInverseWorldTransform();
    sp->setUniform("VPMatrix", vpmatrix);
    sp->setUniform("UseAO", useAmbientOcclusion);
    sp->setUniform("MieScattCoeff", mieScattCoefficent);
    sp->setUniform("Resolution", glm::vec2(width, height));
    sp->setUniform("CameraPosition", currentCamera->transformation->getPosition());
    sp->setUniform("FrustumConeLeftBottom", cone->leftBottom);
    sp->setUniform("FrustumConeBottomLeftToBottomRight", cone->rightBottom - cone->leftBottom);
    sp->setUniform("FrustumConeBottomLeftToTopLeft", cone->leftTop - cone->leftBottom);
    sp->setUniform("FocalLength", currentCamera->focalLength);
    sp->setUniform("LensBlurSize", lensBlurSize);
    sp->setUniform("Time", Game::instance->time);
    sp->setUniform("DayElapsed", dayElapsed);
    sp->setUniform("YearElapsed", yearElapsed);
    sp->setUniform("EquatorPoleMix", equatorPoleMix);
    sp->setUniform("Exposure", exposure);
    sp->setUniform("Contrast", contrast);

    auto dd = calculateDay(dayElapsed, yearElapsed, equatorPoleMix);
    dayData = dd;

    sp->setUniform("dd_sunDir", dd.sunDir);
    sp->setUniform("dd_moonDir", dd.moonDir);
    sp->setUniform("dd_sunSpaceDir", dd.sunSpaceDir);
    sp->setUniform("dd_viewFrame", dd.viewFrame);
    sp->setUniform("dd_moonPos", dd.moonPos);
    sp->setUniform("dd_earthPos", dd.earthPos);

    double t100 = Game::instance->time * 100.0;
    double t001 = Game::instance->time * 0.001;
    sp->setUniform("T100", (float)t100);
    sp->setUniform("T001", (float)t001);

    sp->setUniform("CloudsFloor", cloudsFloor);
    sp->setUniform("CloudsCeil", cloudsCeil);
    sp->setUniform("CloudsThresholdLow", cloudsThresholdLow);
    sp->setUniform("CloudsThresholdHigh", cloudsThresholdHigh);
    sp->setUniform("CloudsWindSpeed", cloudsWindSpeed);
    sp->setUniform("CloudsOffset", cloudsOffset);
    sp->setUniform("AtmosphereScale", atmosphereScale);
    sp->setUniform("CloudsDensityScale", cloudsDensityScale);
    sp->setUniform("CloudsDensityThresholdLow", cloudsDensityThresholdLow);
    sp->setUniform("CloudsDensityThresholdHigh", cloudsDensityThresholdHigh);
    sp->setUniform("WaterWavesScale", waterWavesScale);
    sp->setUniform("NoiseOctave1", noiseOctave1);
    sp->setUniform("NoiseOctave2", noiseOctave2);
    sp->setUniform("NoiseOctave3", noiseOctave3);
    sp->setUniform("NoiseOctave4", noiseOctave4);
    sp->setUniform("NoiseOctave5", noiseOctave5);
    sp->setUniform("NoiseOctave6", noiseOctave6);
    sp->setUniform("NoiseOctave7", noiseOctave7);
    sp->setUniform("NoiseOctave8", noiseOctave8);

    sp->setUniform("WaterScale", waterScale);
    sp->setUniform("WaterHeight", waterHeight);
    sp->setUniform("WaterSpeed", waterSpeed);
    sp->setUniform("Wind", wind);
    sp->setUniform("NightSkyLightPollution", nightSkyLightPollution);
    sp->setUniform("Rand1", static_cast <float> (rand()) / static_cast <float> (RAND_MAX));
    sp->setUniform("Rand2", static_cast <float> (rand()) / static_cast <float> (RAND_MAX));
    sp->setUniform("CloudsIntegrate", cloudsIntegrate);
    mrtAlbedoRoughnessTex->use(0);
    mrtNormalMetalnessTex->use(1);
    mrtDistanceTexture->use(2);
}

Renderer::~Renderer()
{
    destroyFbos(false);
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
    delete cloudResolveShader;
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

void Renderer::pick(Camera* camera, glm::vec2 uv)
{
    if (!pickingReady) return;

    pickingReadShader->use();
    pickingReadShader->setUniform("uv", uv);
    pickingDataTex->use(0);
    pickingNormalTex->use(1);
    pickingWorldPosTex->use(2);
    pickingResultSSBO->use(5);
    pickingReadShader->dispatch(1, 1, 1);

    pickingFbo->use(true);

    ShaderProgram *shader = Game::instance->shaders->idWriteShader;
    shader->use();
    Game::instance->world->setUniforms(shader, camera);
    Game::instance->world->draw(shader, camera);
    Game::instance->invoke([&, uv]() {
        int x = (int)(uv.x * (float)pickingDataTex->width);
        int y = (int)(uv.y * (float)pickingDataTex->height);

        unsigned int * data = new unsigned int[4];
        pickingResultSSBO->readSubData(0, 4 * 4, &data[0]);

        float * datawp = new float[4];
        pickingResultSSBO->readSubData(2 * 4 * 4, 4 * 4, &datawp[0]);

        float * datan = new float[4];
        pickingResultSSBO->readSubData(4 * 4, 4 * 4, &datan[0]);

        pickingResultMesh = data[0];
        pickingResultLod = data[1];
        pickingResultInstance = data[2];

        if (pickingResultMesh > 0) {
            pickingWorldPos.x = datawp[0];
            pickingWorldPos.y = datawp[1];
            pickingWorldPos.z = datawp[2];

            pickingNormal.x = datan[0];
            pickingNormal.y = datan[1];
            pickingNormal.z = datan[2];

            free(datawp);
            free(datan);
        }

        pickingReady = true;
        free(data);
    });
}

void Renderer::draw(Camera *camera)
{
    if (!gpuInitialized) {
        float* ones = new float[4]{ 1.0f, 1.0f, 1.0f, 1.0f };
        exposureBuffer->mapData(4 * 4, &ones);
        pickingResultSSBO->mapData(4 * 4 * 3, new float[4 * 4 * 3]{});
        // starsTexture->generateMipMaps();
    }
    Game::instance->bindTexture(GL_TEXTURE_2D, 0, 0);
    Game::instance->bindTexture(GL_TEXTURE_2D, 1, 0);
    mrtFbo->use(true);
    //Game::instance->world->setUniforms(Game::instance->shaders->materialGeometryShader, camera);
    Game::instance->world->setUniforms(Game::instance->shaders->materialShader, camera);
    Game::instance->world->setSceneUniforms();
    Game::instance->world->draw(Game::instance->shaders->materialShader, camera);
    // mrtDistanceTexture->generateMipMaps();
    if (useAmbientOcclusion) {
        //     ambientOcclusion();
    }
    csm->map(-dayData.sunDir, currentCamera->transformation->getPosition());
    //mrtAlbedoRoughnessTex->setWrapModes(GL_MIRRORED_REPEAT, GL_MIRRORED_REPEAT);
    //mrtNormalMetalnessTex->setWrapModes(GL_MIRRORED_REPEAT, GL_MIRRORED_REPEAT);
    //mrtDistanceTexture->setWrapModes(GL_MIRRORED_REPEAT, GL_MIRRORED_REPEAT);
    //depthTexture->setWrapModes(GL_MIRRORED_REPEAT, GL_MIRRORED_REPEAT);
    //deferred();
   // ambientLight();
    //glEnable(GL_TEXTURE_CUBE_MAP_SEAMLESS);
    waterTile();
    mrtAlbedoRoughnessTex->use(0);
    mrtNormalMetalnessTex->use(1);
    mrtDistanceTexture->use(2);
    atmScatt();
    clouds();
    fog();
    cloudsResolve();
    combine(0);
    waterMesh();
    waterColorShaded();
    combine(1);
    //  lensBlur();
    if (passPhrase == 0) {
        cloudFace++;
        if (cloudFace > 5) {
            cloudFace = 0;
            cloudCycleUseOdd = !cloudCycleUseOdd;
        }
        if (cloudFace == 3) cloudFace++;
    }
    passPhrase++;
    if (passPhrase > 3) {
        passPhrase = 0;
        cloudsResolvedTexture->generateMipMaps();
        atmScattTexture->generateMipMaps();
    }
    gpuInitialized = true;
}

void Renderer::bloom()
{
}
void Renderer::cloudsResolve()
{
    deferredTexture->use(5);
    ambientLightTexture->use(6);
    ambientOcclusionTexture->use(16);
    //fogTexture->use(20);
    waterColorTexture->use(21);
    starsTexture->use(24);
    moonTexture->use(28);
    if (passPhrase == 3) {
        cloudsResolvedFbo->use(false);
        cloudResolveShader->use();
        setCommonUniforms(cloudResolveShader);
        Camera* camera = cloudsResolvedFbo->switchFace(GL_TEXTURE_CUBE_MAP_POSITIVE_X + cloudFace, false);
        camera->transformation->setPosition(currentCamera->transformation->getPosition());
        FrustumCone *cone = camera->cone;
        glm::mat4 vpmatrix = camera->projectionMatrix * camera->transformation->getInverseWorldTransform();
        glm::mat4 cameraRotMatrix = camera->transformation->getRotationMatrix();
        glm::mat4 rpmatrix = camera->projectionMatrix * inverse(cameraRotMatrix);
        camera->cone->update(inverse(rpmatrix));
        cloudResolveShader->setUniform("VPMatrix", vpmatrix);
        cloudResolveShader->setUniform("CameraPosition", camera->transformation->getPosition());
        cloudResolveShader->setUniform("FrustumConeLeftBottom", cone->leftBottom);
        cloudResolveShader->setUniform("FrustumConeBottomLeftToBottomRight", cone->rightBottom - cone->leftBottom);
        cloudResolveShader->setUniform("FrustumConeBottomLeftToTopLeft", cone->leftTop - cone->leftBottom);
        quad3dInfo->draw();
    }
}

void Renderer::combine(int step)
{
  //  
    combineFbo->use(false);
    combineShader->use();
    exposureBuffer->use(0);
    deferredTexture->use(5);
    ambientLightTexture->use(6);
    ambientOcclusionTexture->use(16);
    fresnelTexture->use(14);
    //fogTexture->use(20);
    waterColorTexture->use(21);
    starsTexture->use(24);
    moonTexture->use(28);
    cloudsResolvedTexture->use(29);

    setCommonUniforms(combineShader);
    combineShader->setUniform("CombineStep", step);
    if (step == 1) {
        combineShader->setUniform("ShowSelection", showSelection ? 1 : 0);
        if (showSelection) {
            combineShader->setUniform("SelectionPos", selectionPosition);
            combineShader->setUniform("SelectionQuat", selectionOrientation);
        }
    }
    if (step == 0) {

        csm->setUniformsAndBindSampler(combineShader, 30);
    }

    quad3dInfo->draw();
    waterColorTexture->generateMipMaps();
    waterColorTexture->use(16);
    exposureComputeShader->dispatch(1, 1, 1);
    //combineTexture->generateMipMaps();
}
void Renderer::fxaaTonemap()
{
    fxaaTonemapShader->use();
    //lensBlurTextureVertical->use(16);
    combineTexture->use(16);
    setCommonUniforms(fxaaTonemapShader);

    quad3dInfo->draw();

    Game::instance->firstFullDrawFinished = true;
}

void Renderer::lensBlur()
{
    lensBlurFboVertical->use(false);
    lensBlurShader->use();
    mrtDistanceTexture->use(2);
    combineTexture->use(16);
    setCommonUniforms(lensBlurShader);

    quad3dInfo->draw();
    // lensBlurTextureVertical->generateMipMaps();
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
    waterColorShader->recompile();
    waterMeshShader->recompile();
    cloudResolveShader->recompile();
    Game::instance->shaders->depthOnlyShader->recompile();
    Game::instance->shaders->idWriteShader->recompile();
    Game::instance->shaders->materialShader->recompile();
}

void Renderer::deferred()
{
    if (!gpuInitialized) return;
    vector<Light*> lights = Game::instance->world->scene->getLights();

    deferredFbo->use(false);
    deferredShader->use();
    setCommonUniforms(deferredShader);
    moonTexture->use(28);
    glCullFace(GL_FRONT);
    glEnable(GL_BLEND);
    glBlendFunc(GL_ONE, GL_ONE);

    for (int i = 0; i < lights.size(); i++) {
        deferredShader->setUniform("LightColor", lights[i]->color);
        deferredShader->setUniform("LightPosition", lights[i]->transformation->getPosition());
        deferredShader->setUniform("LightOrientation", glm::inverse(lights[i]->transformation->getOrientation()));
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

    glCullFace(GL_BACK);

    glDisable(GL_BLEND);
}

void Renderer::ambientLight()
{
    return;
    ambientLightFbo->use(false);
    ambientLightShader->use();
    setCommonUniforms(ambientLightShader);
    quad3dInfo->draw();
}


void Renderer::waterTile()
{
    Texture2d * tex = waterTileUseFBO1 ? waterTileTexture1 : waterTileTexture2;
    Texture2d * tex2 = waterTileUseFBO1 ? waterTileTexture2 : waterTileTexture1;
    Framebuffer * fbo = waterTileUseFBO1 ? waterTileFbo1 : waterTileFbo2;
    fbo->use(false);
    waterTileShader->use();
    tex2->use(23);
    waterTileShader->setUniform("WaterSpeed", waterSpeed);
    waterTileShader->setUniform("Resolution", glm::vec2(width, height));
    waterTileShader->setUniform("Time", Game::instance->time);
    quad3dInfo->draw();
    tex->setWrapModes(GL_MIRRORED_REPEAT, GL_MIRRORED_REPEAT);
    tex->generateMipMaps();
    waterTileUseFBO1 = !waterTileUseFBO1;
    tex->use(23);
}

void Renderer::ambientOcclusion()
{
    ambientOcclusionFbo->use(true);
    ambientOcclusionShader->use();
    setCommonUniforms(ambientOcclusionShader);
    quad3dInfo->draw();
}

void Renderer::fog()
{
    atmScattTexture->use(19);
    return;
    fogFbo->use(false);
    fogShader->use();
    setCommonUniforms(fogShader);
    // csm->setUniformsAndBindSampler(fogShader, 24);
    quad3dInfo->draw();
}

void Renderer::waterMesh()
{
    exposureBuffer->use(0);
    waterMeshFbo->use(false);
    waterMeshShader->use();
    mrtDistanceTexture->use(2);
    // skyboxTexture->use(3);
    deferredTexture->use(5);
    ambientLightTexture->use(6);
    ambientOcclusionTexture->use(16);
    if (!cloudCycleUseOdd) {
        cloudsTextureOdd->use(25);
        cloudsShadowsTextureOdd->use(26);
    }
    else {
        cloudsTextureEven->use(25);
        cloudsShadowsTextureEven->use(26);
    }
    fogTexture->use(20);
    waterMeshTexture->use(21);
    combineTexture->use(22);
    setCommonUniforms(waterMeshShader);

    quad3dInfo->draw();
    //waterColorTexture->generateMipMaps();
}

void Renderer::waterColorShaded()
{
    waterColorFbo->use(false);
    waterColorShader->use();
    fogTexture->use(20);
    combineTexture->use(22);
    if (!cloudCycleUseOdd) {
        cloudsTextureOdd->use(25);
        cloudsShadowsTextureOdd->use(26);
    }
    else {
        cloudsTextureEven->use(25);
        cloudsShadowsTextureEven->use(26);
    }
    cloudsResolvedTexture->use(29);

    setCommonUniforms(waterColorShader);
    quad3dInfo->draw();
    //waterColorTexture->generateMipMaps();
}

void Renderer::atmScatt()
{
    if (passPhrase == 2) {
        atmScattShader->use();
        setCommonUniforms(atmScattShader);

        atmScattFbo->use(false);
        Camera* camera = atmScattFbo->switchFace(GL_TEXTURE_CUBE_MAP_POSITIVE_X + cloudFace, false);
        camera->transformation->setPosition(currentCamera->transformation->getPosition());
        FrustumCone *cone = camera->cone;
        glm::mat4 vpmatrix = camera->projectionMatrix * camera->transformation->getInverseWorldTransform();
        glm::mat4 cameraRotMatrix = camera->transformation->getRotationMatrix();
        glm::mat4 rpmatrix = camera->projectionMatrix * inverse(cameraRotMatrix);
        camera->cone->update(inverse(rpmatrix));
        atmScattShader->setUniform("VPMatrix", vpmatrix);
        atmScattShader->setUniform("CameraPosition", camera->transformation->getPosition());
        atmScattShader->setUniform("FrustumConeLeftBottom", cone->leftBottom);
        atmScattShader->setUniform("FrustumConeBottomLeftToBottomRight", cone->rightBottom - cone->leftBottom);
        atmScattShader->setUniform("FrustumConeBottomLeftToTopLeft", cone->leftTop - cone->leftBottom);
        quad3dInfo->draw();
    }
    
}

void Renderer::clouds()
{
    cloudsShader->use();
    setCommonUniforms(cloudsShader);
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
    atmScattTexture->use(19);
    Camera* camera = nullptr;
    FrustumCone *cone = nullptr;
    glm::mat4 vpmatrix;
    glm::mat4 cameraRotMatrix;
    glm::mat4 rpmatrix;
    // for (int i = 0; i < 6; i++) {
    if (passPhrase == 1) {
        currentFbo->use(false);
        Camera* camera = currentFbo->switchFace(GL_TEXTURE_CUBE_MAP_POSITIVE_X + cloudFace, false);
        camera->transformation->setPosition(currentCamera->transformation->getPosition());
        FrustumCone *cone = camera->cone;
        vpmatrix = camera->projectionMatrix * camera->transformation->getInverseWorldTransform();
        cameraRotMatrix = camera->transformation->getRotationMatrix();
        rpmatrix = camera->projectionMatrix * inverse(cameraRotMatrix);
        camera->cone->update(inverse(rpmatrix));
        cloudsShader->use();
        cloudsShader->setUniform("VPMatrix", vpmatrix);
        cloudsShader->setUniform("CameraPosition", camera->transformation->getPosition());
        cloudsShader->setUniform("FrustumConeLeftBottom", cone->leftBottom);
        cloudsShader->setUniform("FrustumConeBottomLeftToBottomRight", cone->rightBottom - cone->leftBottom);
        cloudsShader->setUniform("FrustumConeBottomLeftToTopLeft", cone->leftTop - cone->leftBottom);
        currentFbo->use(false);
        cloudsShader->use();
        quad3dInfo->draw();
    }
    // }

   // if (cloudCycleUseOdd)
   //     cloudsTextureOdd->generateMipMaps();
   // else
   //     cloudsTextureEven->generateMipMaps();

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

    atmScattTexture->use(19);
    if (passPhrase == 0) {
        // for (int i = 0; i < 6; i++) {
        currentFbo->use(false);
        camera = currentFbo->switchFace(GL_TEXTURE_CUBE_MAP_POSITIVE_X + cloudFace, false);
        camera->transformation->setPosition(currentCamera->transformation->getPosition());
        cone = camera->cone;
        vpmatrix = camera->projectionMatrix * camera->transformation->getInverseWorldTransform();
        cameraRotMatrix = camera->transformation->getRotationMatrix();
        rpmatrix = camera->projectionMatrix * inverse(cameraRotMatrix);
        camera->cone->update(inverse(rpmatrix));
        cloudsShader->use();
        cloudsShader->setUniform("VPMatrix", vpmatrix);
        cloudsShader->setUniform("CameraPosition", camera->transformation->getPosition());
        cloudsShader->setUniform("FrustumConeLeftBottom", cone->leftBottom);
        cloudsShader->setUniform("FrustumConeBottomLeftToBottomRight", cone->rightBottom - cone->leftBottom);
        cloudsShader->setUniform("FrustumConeBottomLeftToTopLeft", cone->leftTop - cone->leftBottom);
        currentFbo->use(false);
        cloudsShader->use();
        quad3dInfo->draw();
    }
    // }

   // if (cloudCycleUseOdd)
   //     cloudsShadowsTextureOdd->generateMipMaps();
  //  else
   //     cloudsShadowsTextureEven->generateMipMaps();
}

void Renderer::motionBlur()
{
}
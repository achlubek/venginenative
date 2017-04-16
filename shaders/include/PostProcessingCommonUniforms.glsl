layout(binding = 0) uniform sampler2D mrt_Albedo_Roughness_Tex;
layout(binding = 1) uniform sampler2D mrt_Normal_Metalness_Tex;
layout(binding = 2) uniform sampler2D mrt_Distance_Bump_Tex;

layout(binding = 3) uniform sampler2D aboveViewDataTex;
layout(binding = 4) uniform samplerCube atmScattTex;
//layout(binding = 5) uniform sampler2D alTex;
layout(binding = 6) uniform sampler2D aoxTex;
layout(binding = 7) uniform sampler2D directTex;
// 8 is reserved for fresnel lut somehow
layout(binding = 9) uniform samplerCube coverageDistTex;
layout(binding = 10) uniform samplerCube shadowsTex;
layout(binding = 11) uniform sampler3D voxelRenderedLod0Tex;
layout(binding = 12) uniform sampler2DArray CSMTex;
layout(binding = 13) uniform sampler2D fogTex;
layout(binding = 14) uniform sampler2D inputTex;
layout(binding = 15) uniform sampler2D lastFoamTex;
layout(binding = 16) uniform samplerCube mainPassTex;
layout(binding = 17) uniform sampler2D moonTex;
layout(binding = 18) uniform samplerCube resolvedAtmosphereTex;
layout(binding = 19) uniform sampler2D waterColorTex;
layout(binding = 20) uniform sampler2D waterFoamTex;
layout(binding = 21) uniform sampler2D waterDistanceTex;
layout(binding = 22) uniform samplerCube shadowMapCube;
layout(binding = 23) uniform sampler2DShadow shadowMapSingle;
layout(binding = 24) uniform sampler3D voxelRenderedLod1Tex;
layout(binding = 25) uniform sampler2D starsTex;

layout(binding = 26) uniform sampler2D sunRSMTex;
layout(binding = 27) uniform sampler2D sunRSMWPosTex;
layout(binding = 28) uniform sampler2D sunRSMNormTex;
layout(binding = 29) uniform sampler2D temporalBBTex;
layout(binding = 30) uniform sampler2D temporalWposBBTex;
layout(binding = 31) uniform sampler3D voxelRenderedLod2Tex;

uniform vec2 Resolution;

uniform mat4 VPMatrix;
uniform vec3 CameraPosition;
uniform mat4 LightMMatrix;

uniform vec3 FrustumConeLeftBottom;
uniform vec3 FrustumConeBottomLeftToBottomRight;
uniform vec3 FrustumConeBottomLeftToTopLeft;

uniform vec3 Previous_FrustumConeLeftBottom;
uniform vec3 Previous_FrustumConeBottomLeftToBottomRight;
uniform vec3 Previous_FrustumConeBottomLeftToTopLeft;

uniform float AboveSpan;

uniform vec3 FogColor;
uniform float FogMaxDistance;
uniform float FogHeight;

uniform int UseAO;
uniform int CombineStep;
uniform mat4 SunRSMVPMatrix;

uniform float Exposure;
uniform float Contrast;

uniform float FocalLength;
uniform float LensBlurSize;

uniform int ShowSelection;
uniform vec3 SelectionPos;
uniform vec4 SelectionQuat;

uniform float CloudsFloor;
uniform float CloudsCeil;
uniform float CloudsThresholdLow;
uniform float CloudsThresholdHigh;
uniform float CloudsWindSpeed;
uniform vec3 CloudsOffset;
uniform float NoiseOctave1;
uniform float NoiseOctave2;
uniform float NoiseOctave3;
uniform float NoiseOctave4;
uniform float NoiseOctave5;
uniform float NoiseOctave6;
uniform float NoiseOctave7;
uniform float NoiseOctave8;
uniform float CloudsIntegrate;
uniform float AtmosphereScale;
uniform float CloudsDensityScale;
uniform float CloudsDensityThresholdLow;
uniform float CloudsDensityThresholdHigh;
uniform float Time;
uniform float WaterWavesScale;
uniform float Rand1;
uniform float Rand2;
uniform float MieScattCoeff;
uniform float WaterSpeed;
uniform float WaterHeight;
uniform vec2 WaterScale;
uniform float FBMSCALE;

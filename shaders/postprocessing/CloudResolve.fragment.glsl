#version 430 core
#define STEP_PREVIOUS_SUN 0
#define STEP_WATER_CLOUDS 1

#include Constants.glsl
#include PlanetDefinition.glsl
#include PostProcessEffectBase.glsl
#include Atmosphere.glsl
#include noise2D.glsl
#include FXAA.glsl
#include ResolveAtmosphere.glsl

vec3 integrateStepsAndSun(vec3 dir){
	return sampleAtmosphere(dir, 0.0, 1.0, 111);
}

vec4 shade(){
    vec3 color = vec3(0);
	vec3 dir = reconstructCameraSpaceDistance(UV, 1.0);
	//if(dir.y < 0.0){
	//	return textureLod(atmScattTex, vec3(dir.x, -dir.y, dir.z), textureQueryLevels(atmScattTex)).rgba  ;
	//}
	//dir.y = abs(dir.y);
	color = integrateStepsAndSun(vec3(dir.x, abs(dir.y), dir.z));
    if(dir.y < -0.01) color *= pow(1.0 - abs(dir.y), 6.0);
    return vec4( clamp(color, 0.0, 1110.0), textureLod(coverageDistTex, dir, 0.0).r);
}

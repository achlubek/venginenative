#version 450
#extension GL_ARB_separate_shader_objects : enable

layout(location = 0) out vec4 outColor;

#include postProcessingInputs.glsl

#include camera.glsl

//#########//

float fresnelf(vec3 direction, vec3 normal) {
    vec3 nDirection = normalize( direction );
    vec3 nNormal = normalize( normal );
    vec3 halfDirection = normalize( nNormal + nDirection );

    float cosine = dot( halfDirection, nDirection );
    float product = max( cosine, 0.0 );
    float factor = pow( product, 5.0 );

    return factor;
}
vec2 aoSamplesSpeed[] = vec2[](
vec2(0.5, 0.5),

vec2(0.25, 0.5),
vec2(0.75, 0.5),

vec2(0.5, 0.25),
vec2(0.5, 0.75),

vec2(0.25, 0.25),
vec2(0.75, 0.75),

vec2(0.75, 0.25),
vec2(0.75, 0.25),


vec2(0.33, 0.5),
vec2(0.66, 0.5),

vec2(0.5, 0.33),
vec2(0.5, 0.66),

vec2(0.33, 0.33),
vec2(0.66, 0.66),

vec2(0.66, 0.33),
vec2(0.66, 0.33)
);
float rand2s(vec2 co){
    return fract(sin(dot(co.xy,vec2(12.9898,78.233))) * 43758.5453);
}
float rand2sTime(vec2 co){
    return fract(sin(dot(co.xy * hiFreq.Time,vec2(12.9898,78.233))) * 43758.5453);
}
float ambientocclusion(){
    vec3 diffuse = texture(texDiffuse, UV).rgb;
    vec3 normal = normalize(texture(texNormal, UV).rgb);
    float dist = texture(texDistance, UV).r;
    vec3 worldPos = reconstructCameraSpaceDistance(UV, dist);
    vec3 worldPosDFX = dFdx(worldPos);
    vec3 worldPosDFY = dFdy(worldPos);
    float ao = 0.0;
    float w = 0.01;
    float invdistsqr = 1.0 / (dist + 1.0);
    float stepsize = 1.0 / float(aoSamplesSpeed.length());
    float rot = 1.0 + rand2s(UV);
    vec2 seed = UV;

    for(float a = rand2sTime(UV) * 0.412 ; a < 3.1415 * 2.0;a += 0.412){
        for(float r = rand2sTime(UV) * 0.2; r < 1.0; r += 0.2f){

            vec2 displace = vec2(sin(a), cos(a)) * r;// * normalize(vec2(length(worldPosDFX), length(worldPosDFY)));
            vec2 newuv = UV + displace * invdistsqr * 0.4;
            float newdist = texture(texDistance, newuv).r;
            vec3 newworldPos = reconstructCameraSpaceDistance(newuv, newdist);
            float weight = max(0.0, 1.0 - distance(newworldPos, worldPos))
                * step(0.0, newuv.x)
                * step(0.0, newuv.y)
                * (1.0 - step(1.0, newuv.x))
                * (1.0 - step(1.0, newuv.y));
            float dt = max(0.0, dot(normal, normalize(newworldPos - worldPos)));
            ao += weight *  sqrt(sqrt(dt));
            seed += 12.0;
            w += weight * 1.0;
        }
    }

    return clamp((ao * 2.0) / w, 0.0, 1.0);
}

#line 94
float aboveao(){
    vec3 diffuse = texture(texDiffuse, UV).rgb;
    vec3 normal = normalize(texture(texNormal, UV).rgb);
    float dist = texture(texDistance, UV).r;
    vec3 worldPos = FromCameraSpace(reconstructCameraSpaceDistance(UV, dist));
    vec2 xzsp = (worldPos.xz - CameraPosition.xz) * vec2(1.0, -1.0);
    vec2 aboveuv = (xzsp / 50.0) * 0.5 + 0.5; //rendered scale from -50 to 50 units

    float average = 0.0;
    float averagew = 0.0;

    for(float a = rand2sTime(UV) * 0.112 ; a < 3.1415 * 2.0;a += 0.112){
        float r = rand2sTime(UV + a * 1233.0);
        vec2 displace = vec2(sin(a), cos(a)) * r * 0.023;// * normalize(vec2(length(worldPosDFX), length(worldPosDFY)));
        vec2 newwposxz = ((aboveuv + displace) * 2.0 - 1.0) * 50.0 * vec2(1.0, -1.0) +  CameraPosition.xz;
        vec3 newwpos = vec3(newwposxz.x, texture(texAboveDistance, aboveuv + displace).r, newwposxz.y);
        float attenuation = 1.0 / pow(distance(newwpos, worldPos)*0.8 + 0.5, 2.0);
        average += max(0.0, dot(normalize(newwpos - worldPos), vec3(0.0, 1.0, 0.0))) * attenuation;
        averagew += 1.0;
    }

    average /= averagew;
    return pow(1.0 - clamp(average, 0.0, 1.0), 4.0);
}
vec3 extra_cheap_atmosphere(vec3 raydir, vec3 sundir){
	sundir.y = max(sundir.y, -0.07);
	float special_trick = 1.0 / (raydir.y * 1.0 + 0.1);
	float special_trick2 = 1.0 / (sundir.y * 11.0 + 1.0);
	float raysundt = pow(abs(dot(sundir, raydir)), 2.0);
	float sundt = pow(max(0.0, dot(sundir, raydir)), 8.0);
	float mymie = sundt * special_trick * 0.2;
	vec3 suncolor = mix(vec3(1.0), max(vec3(0.0), vec3(1.0) - vec3(5.5, 13.0, 22.4) / 22.4), special_trick2);
	vec3 bluesky= vec3(5.5, 13.0, 22.4) / 22.4 * suncolor;
	vec3 bluesky2 = max(vec3(0.0), bluesky - vec3(5.5, 13.0, 22.4) * 0.004 * (special_trick + -6.0 * sundir.y * sundir.y));
	bluesky2 *= special_trick * (0.24 + raysundt * 0.24);
	return bluesky2 + mymie * suncolor;
}
void main() {
    vec3 diffuse = texture(texDiffuse, UV).rgb;
    vec3 normal = normalize(texture(texNormal, UV).rgb);
    vec3 worldPos = ToCameraSpace(reconstructCameraSpaceDistance(UV, texture(texDistance, UV).r));

//    vec3 dir =

    vec3 res = vec3(0.05) * diffuse;// * aboveao();// * (1.0 - ambientocclusion());// * (0.02 + 0.98 * max(0.0, dot(vec3(0.0, 1.0, 0.0), normal)));
    vec3 mapper = vec3(2.991431, 4.548709, 3.439944);
    float expected = distance(mapper, worldPos);
    //float shadow = step(0.01, expected - texture(shadowMap));
    outColor = vec4(res, 1.0);
}

#version 430 core

#include PostProcessEffectBase.glsl
#include Constants.glsl
#include PlanetDefinition.glsl

layout(location = 1) out vec4 outWpos;

#include FXAA.glsl

uniform int IsFinalPass;
vec2 projectvdao(vec3 pos){
    vec4 tmp = (VPMatrix * vec4(pos, 1.0));
    return (tmp.xy / tmp.w) * 0.5 + 0.5;
}
float rand2sTime(vec2 co){
    co *= Time;
    return fract(sin(dot(co.xy,vec2(12.9898,78.233))) * 43758.5453);
}

const float SRGB_ALPHA = 0.055;
float linear_to_srgb(float channel) {
    if(channel <= 0.0031308)
    return 12.92 * channel;
    else
    return (1.0 + SRGB_ALPHA) * pow(channel, 1.0/2.4) - SRGB_ALPHA;
}

vec3 rgb_to_srgb(vec3 rgb) {
    return vec3(
    linear_to_srgb(rgb.r),
    linear_to_srgb(rgb.g),
    linear_to_srgb(rgb.b)
    );
}

float A = 0.15;
float B = 0.50;
float C = 0.10;
float D = 0.20;
float E = 0.02;
float F = 0.30;
float W = 11.2;


vec3 Uncharted2Tonemap(vec3 x)
{
   return ((x*(A*x+C*B)+D*E)/(x*(A*x+B)+D*F))-E/F;
}
layout (std430, binding = 0) buffer R1
{
  float Luminence;
};

vec3 testmap(vec3 c){
    return vec3(
         1.879574*c.r  -  1.03263*c.g  + 0.153055*c.b,
         -0.21962*c.r  + 1.715146*c.g -  0.49553* c.b,
         0.006956*c.r  -  0.51487*c.g  + 1.507914*c.b
    );
}

vec3 tonemap(vec3 xa){
    vec3 a = xa;//1.0 * xa / max(0.1, Luminence * 0.5 );
    a *= Exposure * 10.0;
    float luma = length(a);
    vec3 xa2 = xa + 0.00001;
    vec3 coloressence = normalize(xa2);
    //a /= 1.0 + log(luma) * 1.1;
    //a = coloressence * mix(sqrt(clamp(luma, 0.0001, 99999.0)), luma, 0.1);
    //a = normalize(a) * mix(l, 0.9, 0.2);
    a = pow(a, vec3(Contrast));
    return (Uncharted2Tonemap(a));

}
float smart_inverse_dot(float dt, float coeff){
    return 1.0 - (1.0 / (1.0 + dt * coeff));
}
layout (binding = 4, r32ui) coherent  uniform uimage2D lensBlurOutputRed;
layout (binding = 5, r32ui) coherent  uniform uimage2D lensBlurOutputGreen;
layout (binding = 6, r32ui) coherent  uniform uimage2D lensBlurOutputBlue;
layout (binding = 7, r32ui) coherent  uniform uimage2D lensBlurOutputWeight;
vec4 shade(){
    if(IsFinalPass == 1){

        vec2 imagesize = vec2(imageSize(lensBlurOutputRed));
        ivec2 UIV = ivec2(UV * imagesize);
        vec3 pix = vec3(1.0 / Resolution.x, 1.0 / Resolution.y, 0.0);
        float d1 = 0.0;//currentData.cameraDistance;
        d1 += textureLod(mrt_Distance_Bump_Tex, UV + pix.xz * 2.0, 1.0).r;
        d1 += textureLod(mrt_Distance_Bump_Tex, UV + pix.zy * 2.0, 1.0).r;
        d1 += textureLod(mrt_Distance_Bump_Tex, UV - pix.xz * 2.0, 1.0).r;
        d1 += textureLod(mrt_Distance_Bump_Tex, UV - pix.zy * 2.0, 1.0).r;
        d1 /= 4.0;
        float distance_mult = smoothstep(0.01, 0.01, abs(currentData.cameraDistance - d1));
        vec3 n1 = vec3(0.0);//currentData.cameraDistance;
        n1 += textureLod(mrt_Normal_Metalness_Tex, UV + pix.xz * 2.0, 1.0).rgb;
        n1 += textureLod(mrt_Normal_Metalness_Tex, UV + pix.zy * 2.0, 1.0).rgb;
        n1 += textureLod(mrt_Normal_Metalness_Tex, UV - pix.xz * 2.0, 1.0).rgb;
        n1 += textureLod(mrt_Normal_Metalness_Tex, UV - pix.zy * 2.0, 1.0).rgb;
        n1 /= 4.0;
        float normal_mult = smoothstep(0.0, 0.1, 1.0 - max(0.0, dot(currentData.normal, n1)));
        //return vec4(smoothstep(0.01, 0.1, abs(currentData.cameraDistance - d1)));
        vec4 blurred = textureLod(inputTex, UV, 1.0);
        blurred += textureLod(inputTex, UV + pix.xz * 2.0, 1.0);
        blurred += textureLod(inputTex, UV - pix.xz * 2.0, 1.0);
        blurred += textureLod(inputTex, UV + pix.zy * 2.0, 1.0);
        blurred += textureLod(inputTex, UV - pix.zy * 2.0, 1.0);
        blurred /= 5.0;
                imageStore(lensBlurOutputRed, UIV, uvec4(0));
                imageStore(lensBlurOutputGreen, UIV, uvec4(0));
                imageStore(lensBlurOutputBlue, UIV, uvec4(0));
                imageStore(lensBlurOutputWeight, UIV, uvec4(0));
        float vignette = 1.0 -  smart_inverse_dot( 1.0 - dot(reconstructCameraSpaceDistance(UV, 1.0), reconstructCameraSpaceDistance(vec2(0.5), 1.0)), 11.0);

        vec4 bloom = texture(blurTestTexture, UV).rgba;
        //return bloom;
        //bloom *= length(bloom.xyz) * 4.0;
        bloom.xyz = pow(bloom.xyz, vec3(6.0));
        bloom = clamp(bloom, 0.0, 11.0);
        bloom.xyz = tonemap(bloom.xyz);
        //bloom *= step(0.5, UV.x);

        return min(1.0, 1.0 - pow(1.0 - vignette , 7.0)) * mix(textureLod(inputTex, UV, 0.0).rgba, fxaa(inputTex, UV).rgba, max(distance_mult, normal_mult));

    } else {
        vec3 dir = reconstructCameraSpaceDistance(UV, 1.0);
        float lastinter = textureLod(temporalBBTex, UV, 0.0).a;
        vec4 lastpos = textureLod(temporalWposBBTex, UV, 0.0).rgba;
        vec2 olduv = projectvdao(lastpos.xyz);
        vec2 nuv = clamp(UV + (UV - olduv), 0.0, 1.0);
        float iter = rand2sTime(UV) * 0.1;
        vec4 last = textureLod(temporalBBTex, nuv, 0.0).rgba;

        vec2 imagesize = vec2(imageSize(lensBlurOutputRed));
        ivec2 UIV = ivec2((UV) * imagesize);

        //vec3 color =  textureLod(inputTex, UV, 0.0).rgb;
        UIV = ivec2(UIV);
/*
        uint rui = imageLoad(lensBlurOutputRed, UIV).r;
        uint gui = imageLoad(lensBlurOutputGreen, UIV).r;
        uint bui = imageLoad(lensBlurOutputBlue, UIV).r;
        uint wui = imageLoad(lensBlurOutputWeight, UIV).r + 1;
        vec3 colorx =  vec3(rui, gui, bui)  / wui / 1.0;
        return vec4(colorx, 1.0);*/
        vec3 color = vec3(0.0);
        for(int i=0;i<10;i++){
            vec2 nnvv = mix(UV, nuv, iter);
            color += textureLod(inputTex, nnvv, 0.0).rgb;
            iter += 0.1;
        }
            //UIV = ivec2(clamp(nnvv, 0.0, 1.0) * imagesize);
/*
            uint rui = imageLoad(lensBlurOutputRed, UIV).r;
            uint gui = imageLoad(lensBlurOutputGreen, UIV).r;
            uint bui = imageLoad(lensBlurOutputBlue, UIV).r;
            uint wui = imageLoad(lensBlurOutputWeight, UIV).r + 1;
            color +=  vec3(rui, gui, bui)  / float(wui) / 1.0;*/
//            iter += 0.1;
//        }
        color *= 0.1;
    //    memoryBarrier();
        float distance_fix = 1.0 - smoothstep(0.001, 0.003, distance(lastpos.xyz, currentData.worldPos));
        distance_fix *= 1.0 - smoothstep(0.001, 0.003, abs(lastpos.a - distance(lastpos.xyz, CameraPosition)));
        float procedural_fix = step(0.01, currentData.cameraDistance);
        //float out_of_screen_fix = (nuv.x < 0.0 || nuv.x > 1.0 || nuv.y < 0.0 || nuv.y > 1.0) ? 0.0 : 1.0;
        float out_of_screen_fix = (nuv.x <= 0.001 || nuv.x >= 0.999 || nuv.y <= 0.001 || nuv.y >= 0.999) ? 0.0 : 1.0;


        color = mix(color, last.rgb, distance_fix * procedural_fix * out_of_screen_fix * lastinter);// (nuv.x < 0.0 || nuv.x > 1.0 || nuv.y < 0.0 || nuv.y > 1.0) ? 0.0 : ( step(0.01, currentData.cameraDistance)) * (0.95 / (1.0 + 1110.0 * abs(currentData.cameraDistance - last.a))));
        outWpos = vec4(currentData.worldPos, distance(currentData.worldPos, CameraPosition));


        if(currentData.cameraDistance < 0.01) outWpos.xyz = CameraPosition + dir * rsi2(Ray(toplanetspace(CameraPosition), dir), Sphere(vec3(0.0), planetradius + atmosphereradius));
        return vec4(clamp(color, 0.0, 2.0), mix(distance_fix * procedural_fix * out_of_screen_fix * 0.898, lastinter, 0.18));
    }
}

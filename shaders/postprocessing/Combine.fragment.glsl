#version 430 core


layout(binding = 3) uniform samplerCube skyboxTex;
layout(binding = 5) uniform sampler2D directTex;
layout(binding = 6) uniform sampler2D alTex;
//layout(binding = 16) uniform sampler2D aoxTex;
//layout(binding = 20) uniform sampler2D fogTex;
layout(binding = 21) uniform sampler2D waterColorTex;
layout(binding = 22) uniform sampler2D inputTex;
layout(binding = 23) uniform sampler2D aboveViewDataTex;

layout(binding = 25) uniform samplerCube coverageDistTex;
layout(binding = 26) uniform samplerCube shadowsTex;
//layout(binding = 27) uniform samplerCube skyfogTex;
layout(binding = 28) uniform sampler2D moonTex;
layout(binding = 23) uniform sampler2D waterTileTex;
layout(binding = 24) uniform sampler2D starsTex;
layout(binding = 29) uniform samplerCube resolvedAtmosphereTex;
layout(binding = 31) uniform sampler2D sunRSMTex;
layout(binding = 34) uniform sampler2D sunRSMWPosTex;
layout(binding = 33) uniform sampler2D sunRSMNormTex;

uniform int UseAO;

uniform int CombineStep;

uniform mat4 SunRSMVPMatrix;
#define STEP_PREVIOUS_SUN 0
#define STEP_WATER_CLOUDS 1

#include Constants.glsl
#include PlanetDefinition.glsl
#include PostProcessEffectBase.glsl
#include Atmosphere.glsl
#include noise2D.glsl
#include FXAA.glsl
#include ResolveAtmosphere.glsl

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
uniform float Exposure;
uniform float Contrast;

vec3 testmap(vec3 c){
    return vec3(
         1.879574*c.r  -  1.03263*c.g  + 0.153055*c.b,
         -0.21962*c.r  + 1.715146*c.g -  0.49553* c.b,
         0.006956*c.r  -  0.51487*c.g  + 1.507914*c.b
    );
}

vec3 tonemap(vec3 xa){
    vec3 a = xa / max(0.1, Luminence * 0.1);
    a *= Exposure;
    float luma = length(a);
    vec3 xa2 = xa + 0.00001;
    vec3 coloressence = normalize(xa2);
    //a /= 1.0 + log(luma) * 1.1;
    //a = coloressence * mix(sqrt(clamp(luma, 0.0001, 99999.0)), luma, 0.5);
    //a = normalize(a) * mix(l, 0.9, 0.2);
    a = pow(a, vec3(Contrast));
    return (rgb_to_srgb(a));

}

//vec3 myfuckingperfecttonemap(vec3 a){
//    float luma = length(vec3(a.r, a.g * 1.5, a.b));
//}

float vignette(vec2 uv, float scale, float strength){
    float d = length(uv * 2.0 - 1.0) * scale;
    return (1.0 - max(0.0, d - 1.0)) * strength + (1.0 - strength);
}


/*
float edgeDetect(vec2 uv){
    vec2 pixel = 1.0 / Resolution;
    vec2 crs[] = vec2[](
        vec2(pixel.x, pixel.y),
        vec2(-pixel.x, pixel.y),
        vec2(pixel.x, -pixel.y),
        vec2(-pixel.x, -pixel.y),
        vec2(pixel.x, 0.0),
        vec2(0.0, pixel.y),
        vec2(-pixel.x, 0.0),
        vec2(0.0, -pixel.y)
    );

}*/

vec3 integrateStepsAndSun(){
    currentData.cameraDistance = length(currentData.normal) < 0.01 ? 999999.0 : currentData.cameraDistance;
    return getNormalLighting(UV, currentData);
}


uniform float FocalLength;
uniform float LensBlurSize;
float getAmountForDistance(float focus, float dist){

	float f = FocalLength * 0.1;
	float d = focus*1000.0; //focal plane in mm
	float o = dist*1000.0; //depth in mm

	float fstop = 64.0;
	float CoC = 1.0;
	float a = (o*f)/(o-f);
	float b = (d*f)/(d-f);
	float c = (d-f)/(d*fstop*CoC);

	float blur = abs(a-b)*c;
	return blur;
}

vec3 BoKeH(vec2 uv){
    float focus = textureLod(waterColorTex, vec2(0.5, 0.5), 0.0).a;
    float dist = textureLod(waterColorTex, uv, 0.0).a;
//    return dist * vec3(0.1);
   // if(dist < focus) dist = focus + abs(dist - focus);
    float amountoriginal = getAmountForDistance(focus, dist) * LensBlurSize * 21.019;

    float amount = clamp(amountoriginal, 0.00, 0.017);
    float xda = amount / 0.017;
    if(amount < 0.0005) return fxaa(waterColorTex, UV).rgb;
    //float amount = getAmountForDistance(focus, dist);
    //return vec3(amount / 0.005);
    float stepsize = 6.283185 / 16.0;
    float ringsize = 1.0 / 4.0;
    vec3 sum = vec3(0);
    float weight = 0.001;
    vec2 ratio = vec2(Resolution.y / Resolution.x, 1.0);
    for(float x=0.0;x<6.283185;x+=stepsize){
        for(float y=ringsize;y<1.0;y+=ringsize){
            vec2 displacement = vec2(sin(x + y * 2.54), cos(x + y * 2.54)) * ratio * (y) * amount;
            float distx = textureLod(waterColorTex, uv + displacement, 2.0 * xda).a;
            vec3 c = textureLod(waterColorTex, uv + displacement, 2.0 * xda).rgb;
            float w = (length(c) + 0.3) ;//*   mix(max(0.0, 1.0 - abs(dist - distx)), 1.0, amount * 7.0);// * pow(1.0 - y, 2.0);
            sum += w * c;
            weight += w;
        }
    }
    return sum / weight;

}

#include Quaternions.glsl

uniform int ShowSelection;
uniform vec3 SelectionPos;
uniform vec4 SelectionQuat;

vec3 showSelection(vec3 dir){
    Sphere sp = Sphere(SelectionPos, 0.5);
    Ray r = Ray(CameraPosition, dir);
    float primdst = rsi2(r, sp);
    vec2 minmax = vec2(minhit, maxhit);
    float outputMult = step(0.0, primdst);
    float cdst = currentData.cameraDistance;
    cdst += 99999.0 * (1.0 - step(0.01, cdst));
    minmax.y = min(minmax.y, cdst);
    minmax.x = min(minmax.x, cdst);
    float middle = (minmax.y - minmax.x);
    float dstmlt = 1.0 - smoothstep(0.0, 1.0, distance(r.o + r.d * middle, sp.pos));
    vec3 c = outputMult * vec3(0.6, 0.7, 1.0) * (minmax.y - minmax.x);
    c += shade_ray_data(currentData, -normalize(currentData.worldPos - SelectionPos), vec3(1.0, 1.0, 2.0) * 0.1);
    // * (minmax.y - minmax.x);
    /*
    vec3 n = normalize((r.o + r.d * minmax.x) - SelectionPos);

    vec3 dirx = vec3(1.0, 0.0, 0.0);
    vec3 diry = vec3(0.0, 1.0, 0.0);
    vec3 dirz = vec3(0.0, 0.0, 1.0);

    dirx = quat_mul_vec(SelectionQuat, dirx);
    diry = quat_mul_vec(SelectionQuat, diry);
    dirz = quat_mul_vec(SelectionQuat, dirz);

    float dtx = pow(abs(dot(dirx, n)), 23.0);
    float dty = pow(abs(dot(diry, n)), 23.0);
    float dtz = pow(abs(dot(dirz, n)), 23.0);*/

    //return outputMult * (c + vec3(dtx, dty, dtz) * 2.0);
    return (c * 7.0);
}

vec3 integrateCloudsWater(){
    vec3 color = LensBlurSize > 0.11 ? BoKeH(UV) : texture(waterColorTex, UV).rgb;;
    //vec3 c = applysimplebloom();
    return color;
}

float lenssun(vec3 dir){
    //return smoothstep(0.997, 0.999, dot(dir, dayData.sunDir));
    vec3 sdir = dayData.sunDir;
    vec2 ss1 = projectvdao(CameraPosition + dir);
    vec2 ss2 = projectvdao(CameraPosition + sdir);
    ss1.x *= Resolution.x / Resolution.y;
    ss2.x *= Resolution.x / Resolution.y;

    vec3 differente = normalize(dir - sdir) * 4.0;
    //return smoothstep(0.997, 0.998, dot(dir, dayData.sunDir));// fuck it pow(1.0 / (distance(dir, dayData.sunDir) * 22.0 - 0.05), 5.0);
    float primary = pow(1.0 / abs((distance(dir, sdir) * 22.0 - 0.05)), 3.0 + supernoise3dX(differente * 1.3 + Time * 0.2) * 1.0);

    return primary;
}

float rdhashx2 = 0.453451 + Time;
vec2 randpoint2(){
    float x = rand2s(UV * rdhashx2);
    rdhashx2 += 2.1231255;
    float y = rand2s(UV * rdhashx2);
    rdhashx2 += 1.6271255;
    return vec2(x, y) * 2.0 - 1.0;
}

vec2 projectsunrsm(vec3 pos){
    vec4 tmp = (SunRSMVPMatrix * vec4(pos, 1.0));
    return (tmp.xy / tmp.w) * 0.5 + 0.5;
}

uniform float WaterSpeed;
#include WaterHeight.glsl

vec3 applyAirLayer(vec3 dir, vec3 color, vec3 atma, float height){
    vec3 volumetrix = vec3(0.0);
    float volumetrix2 = 0.0;
    int steps = 32;
    float stepsize = 1.0 / 32.0;
    float rd = rand2sTime(UV);
    float iter = rd * stepsize;
    vec3 start = CameraPosition;
    float atmceil = rsi2(Ray(toplanetspace(CameraPosition), dir), Sphere(vec3(0.0), planetradius + min(height, 10000.0)));
    float waterfloor = min(9000.0, intersectPlane(CameraPosition, dir, vec3(0.0, waterdepth + WaterLevel, 0.0), vec3(0.0, 1.0, 0.0)));
    float LN = length(currentData.normal);
    vec3 realpos = mix(CameraPosition + dir * (waterfloor > 0.0 ? waterfloor : atmceil), currentData.worldPos, step(0.01, LN));
    vec3 end = realpos;
    float point_full_coverage = atmosphereradius * 0.5;
    float overall_coverage = linearstep(0.0, point_full_coverage, distance(start, end));
    for(int i=0;i<steps ;i++){
        vec3 p = mix(start, end, iter * iter * iter );
        float coverage = 1.0 - linearstep(0.0, point_full_coverage, distance(start, p));
        float vis = clamp(CSMQueryVisibilitySimple(p), 0.0, 1.0);

        volumetrix2 += vis;
        volumetrix += atma * coverage * (1.0 - linearstep(0.0, height, p.y));

        iter += stepsize;
    }
    //color = mix(color + min(coveragex, 1.0) * atma * pow(volumetrix2, 6.0), atma * volumetrix2, length(currentData.normal) > 0.01 ? min(coveragex, 1.0) : 0.0);
    if(LN > 0.01 || waterfloor > 0.0) color = mix(clamp(color, 0.0, 10000.0), volumetrix * clamp(volumetrix2 * stepsize, 0.0, 1.0) * stepsize, overall_coverage);
    else color *=pow( stepsize * volumetrix2, 1.0);
    return color;
}

vec4 shade(){
    vec3 color = vec3(0);
    if(CombineStep == STEP_PREVIOUS_SUN){
        color = integrateStepsAndSun();
    } else {
        color = integrateCloudsWater();


        vec3 dir = reconstructCameraSpaceDistance(UV, 1.0);
        vec3 dirX = reconstructCameraSpaceDistance(vec2(0.5), 1.0);

        if(ShowSelection == 1) color += showSelection(dir);

        vec2 ss2 = projectvdao(CameraPosition + dayData.sunDir);
        float cloudsonsun = 1.0 - textureLod(resolvedAtmosphereTex, dayData.sunDir, 2.0).a;

        float mindst = step(0.0, dot(dirX, dayData.sunDir)) *
            smoothstep(0.0, 0.05, max(0.0, min(ss2.x,
                min(ss2.y, min(1.0 - ss2.x, 1.0 - ss2.y))
                )));

        vec3 sdirsec = reconstructCameraSpaceDistance(1.0 - ss2, 1.0);
        vec3 sdirthr = reconstructCameraSpaceDistance(((((1.0 - ss2) * 2.0 - 1.0) * 0.5) * 0.5 + 0.5), 1.0);
        vec3 sdirqua = reconstructCameraSpaceDistance(((((ss2) * 2.0 - 1.0) * 0.5) * 0.5 + 0.5), 1.0);

        float secondary = 1.0 - smoothstep(0.06, 0.10, abs(distance(dir, sdirsec)));
        float third = 1.0 - smoothstep(0.06, 0.09, abs(distance(dir, sdirthr)));
        float quad = 1.0 - smoothstep(0.06, 0.08, abs(distance(dir, sdirqua)));

        cloudsonsun *= 1.0 - smoothstep(0.996, 1.0, dot(dayData.sunDir, dayData.moonDir));

        color += cloudsonsun * getSunColor(0.0) * mindst * secondary * vec3(1.0, 1.0, 0.8) * 0.02;
        color += cloudsonsun * getSunColor(0.0) * mindst * third* vec3(0.6, 0.5, 0.7) * 0.03;
        color += cloudsonsun * getSunColor(0.0) * mindst * quad* vec3(0.5, 0.5, 0.8) * 0.04;

        vec4 vdao = blurshadowsAO(dir, 0.0);
        float Shadow = vdao.r;
        vec3 atmorg = vec3(0,planetradius,0) + CAMERA;
        Ray r = Ray(atmorg, dir);
        float xhitfloorX = rsi2(r, sphere1);
        float dx = textureLod(coverageDistTex, dir, 2.0).g + xhitfloorX;
        float Cx = textureLod(coverageDistTex, dir, 2.0).r;
        vec3 pos = dir * dx;
        vec3 lightnings = 0.0 * vec3(0.5, 0.6, 0.9) * 0.06 * vec3(1.0 - Shadow) * smoothstep(0.5, 0.7, supernoise3dX(pos * 0.00003 + vec3(0.0, Time * 9.0, 0.0)));
        // want super realistic dirt on lens?
        //color += mindst * cloudsonsun * getSunColor(0.0) * pow(max(0.0, dot(dir, dayData.sunDir)), 2.0) * 0.01 * (1.0 - smoothstep(0.26, 0.6, abs(0.5 - supernoise3d(vec3(UV.x, UV.y * (Resolution.y / Resolution.x), 0.0) * 30.0))));
        //color += mindst * cloudsonsun * getSunColor(0.0) * pow(max(0.0, dot(dir, dayData.sunDir)), 2.0) * 0.01 * (1.0 - smoothstep(0.26, 0.6, abs(0.5 - supernoise3d(vec3(UV.x, UV.y * (Resolution.y / Resolution.x), 0.0) * 60.0))));
        float monsoonconverage2 = (1.0 - smoothstep(0.995, 0.996, dot(dir, dayData.moonDir)));
        vec4 cloudsData = smartblur(reconstructCameraSpaceDistance(ss2, 1.0), 0.0);
        float coverage = cloudsData.r;
        vec4 cloudsData2 = smartblur(dir, 0.0);
        float coverage2 = cloudsData2.r;
        float ssobj = 1.0 - smoothstep(0.0, 1.01, textureLod(mrt_Distance_Bump_Tex, ss2, 3.0).r);
        float ssobj2 = 1.0 - step(0.1, textureLod(mrt_Distance_Bump_Tex, UV, 0.0).r);
        color += (1.0 - coverage) * ssobj * (lenssun(dir)) * getSunColorDirectly(0.0) * 36.0 * step(0.0, dayData.sunDir.y);
        //color += monsoonconverage2 * (1.0 - coverage2) * ssobj2 *  step(0.0, dir.y) * (smoothstep(0.998, 0.9985, max(0.0, dot(dir, dayData.sunDir)))) * getSunColorDirectly(0.0) * 13.0;


//shade_ray_data(currentData, dayData.sunDir, CSMQueryVisibility(currentData.worldPos) * 20.0 * getSunColorDirectly(0.0))
        vec3 csum = vec3(0.0);
        vec2 suncnt = projectsunrsm(currentData.worldPos);
        for(int i=0;i<12;i++){
            vec2 p = suncnt + 0.01 * randpoint2();
            vec3 albedo = textureLod(sunRSMTex, p, 5.0).rgb;
            vec3 wpos = textureLod(sunRSMWPosTex, p, 5.0).rgb;
            vec3 norm = textureLod(sunRSMNormTex, p, 5.0).rgb;
            float dist = distance(wpos, currentData.worldPos);
            float att = 1.0 / (1.0 + dist * dist * 11.0);//* (1.0 - smoothstep(0.0, 2.4, dist));

            vec3 ray_primary = att * albedo * getSunColorDirectly(0.0);
            vec3 ray_secondary = (0.5 + 0.5 * dot(norm, -currentData.normal)) * max(0.0, dot(-norm, normalize(wpos - currentData.worldPos))) *
              ray_primary * mix(currentData.diffuseColor, vec3(1.0), currentData.metalness);
            csum += ray_secondary;
        }
        vec3 rsmres = 15.0 * csum / 12.0;
        rsmres /= 0.01 + length(rsmres) * 1.01;
        color += rsmres;

        vec3 d = dayData.sunDir + vec3(dir.x, dir.y* 0.5 + 0.5, dir.z);//, vec3(dayData.sunDir.x, 0.1, dayData.sunDir.y), max(0.0, -dir.y * 0.9 + 0.1)) : dir;
        d.y = max(0.3, d.y);
        vec2 disp = vec2(0.2, 0.0);
        vec3 atma = textureLod(atmScattTex, d, 3.0).rgb *4.0* (max(0.0, dayData.sunDir.y) * 0.9 + 0.1);
        /*atma += textureLod(atmScattTex, d + disp.xyy, 3.0).rgb;
        atma += textureLod(atmScattTex, d + disp.yxy, 3.0).rgb;
        atma += textureLod(atmScattTex, d + disp.yyx, 3.0).rgb;
        atma += textureLod(atmScattTex, d - disp.xyy, 3.0).rgb;
        atma += textureLod(atmScattTex, d - disp.yxy, 3.0).rgb;
        atma += textureLod(atmScattTex, d - disp.yyx, 3.0).rgb;
        atma /= 7.0;*/
        float dim = 1.0;//dir.y > 0.0 ? 1.0 : (1.0 / (-dir.y * 11.0 + 1.0));

        color = applyAirLayer(dir, color, atma, 16000.0);
        //color = mix(color, volumetrix, overall_coverage);
        //color = textureLod(sunRSMTex, UV, 0.0).rgb;
        color = tonemap( color);
    }

    return vec4( clamp(color, 0.0, 110.0), currentData.cameraDistance * 0.001);
}

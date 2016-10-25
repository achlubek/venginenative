
float roughnessToMipmap(float roughness, samplerCube txt){
    //roughness = roughness * roughness;
    float levels = max(0, float(textureQueryLevels(txt)));
    float mx = log2(roughness*1024+1)/log2(1024);
    return mx * levels;
}
float roughnessToMipmap(float roughness, sampler2D txt){
    //roughness = roughness * roughness;
    float levels = max(0, float(textureQueryLevels(txt)));
    float mx = log2(roughness*1024+1)/log2(1024);
    return mx * levels;
}

float rdhash = 0.453451 + Time;
vec3 randpoint3(){
    float x = rand2s(UV * rdhash);
    rdhash += 2.1231255;
    float y = rand2s(UV * rdhash);
    rdhash += 1.6271255;
    float z = rand2s(UV * rdhash);
    rdhash += 1.6271255;
    return vec3(x, y, z) * 2.0 - 1.0;
}
float blurshadows(vec3 dir, float roughness){
    //if(CloudsDensityScale <= 0.010) return 0.0;
    float levels = max(0, float(textureQueryLevels(shadowsTex)));
    float mx = log2(roughness*1024+1)/log2(1024);
    float mlvel = mx * levels;
    float dst = textureLod(cloudsCloudsTex, dir, mlvel).g;
    float aoc = 1.0;
    
    float centerval = textureLod(shadowsTex, dir, mlvel).r;
    float blurrange = 0.005;
    for(int i=0;i<7;i++){
        vec3 rdp = normalize(dir + randpoint3() * blurrange);
        float there = textureLod(cloudsCloudsTex, rdp, mlvel).g;
        float w = clamp(1.0 / (abs(there - dst)*0.01 + 0.01), 0.0, 1.0);
        centerval += w * textureLod(shadowsTex, rdp, mlvel).r;
        aoc += w;
    }
    centerval /= aoc;
    return centerval;
}
vec4 smartblur(vec3 dir, float roughness){
    float levels = max(0, float(textureQueryLevels(cloudsCloudsTex)));
    float mx = log2(roughness*1024+1)/log2(1024);
    float mlvel = mx * levels;
    vec4 ret = vec4(0);
    ret.xy = textureLod(coverageDistTex, dir, mlvel).rg;
    ret.z = textureLod(shadowsTex, dir, mlvel).r;
    //ret.z = blurshadows(dir, roughness);
   // ret.w = textureLod(skyfogTex, dir, mlvel).r;
   // ret.w = blurskyfog(dir, roughness);
    return ret;
}

float getthatfuckingfresnel(float reflectivity, vec3 normal, vec3 cameraSpace, float roughness){
    vec3 dir = normalize(reflect(normalize(cameraSpace), normal));
    float base =  1.0 - abs(dot(normalize(normal), dir));
    float fresnel = (reflectivity + (1-reflectivity)*(pow(base, mix(5.0, 0.8, roughness))));
    float angle = 1.0 - base;
    return fresnel * (1.0 - roughness);
}

vec3 shadingMetalic(PostProceessingData data, vec3 lightDir, vec3 color){
    float fresnelR = getthatfuckingfresnel(data.diffuseColor.r, data.normal, data.cameraPos, data.roughness);
    float fresnelG = getthatfuckingfresnel(data.diffuseColor.g, data.normal, data.cameraPos, data.roughness);
    float fresnelB = getthatfuckingfresnel(data.diffuseColor.b, data.normal, data.cameraPos, data.roughness);
    float fresnel = getthatfuckingfresnel(0.04, data.normal, normalize(data.cameraPos), data.roughness);
    vec3 newBase = vec3(fresnelR, fresnelG, fresnelB);
    //   return vec3(fresnel);
    float x = 1.0 - max(0, dot(lightDir, data.originalNormal));
    return shade(CameraPosition, newBase, data.normal, data.worldPos, data.worldPos - lightDir * 1.0, color,  max(0.004, data.roughness), false) * mix(x, pow(x, 8.0), 1.0 - data.roughness);
}
float sun(vec3 dir, vec3 sundir, float gloss, float ansiox){
  //  float dt2 = max(0, dot(normalize(vec3(dir.x, abs(sundir.y), dir.z)), sundir));
 //   float dty = 1.0 - max(0, abs(dir.y - sundir.y));
   // dt = mix(dt, dt2, ansio);
    //return dt;
    
    float ansio = pow(abs(dir.y), 2.0);
    //dir.y *= 0.1 + 0.9 * gloss * gloss;
    //sundir.y *= 0.1 + 0.9 * gloss * gloss;
   // dir.y *= 0.1 + 0.9 * ansio;
   // sundir.y *= 0.1 + 0.9 * ansio;
   // dir = normalize(dir);
   // sundir = normalize(sundir);
  // return gloss;
    float dt = clamp(dot(dir, sundir) + 0.0, 0.0, 1.0);
    return pow(dt, 521.0 * gloss * gloss * gloss * gloss * gloss + 111.0) * (101.0 * gloss + 1.0) * smoothstep(-0.02, -0.01, sundir.y);
   // return smoothstep(0.997, 1.0, dt);
}
float sshadow = 1.0;
vec3 shadingWater(PostProceessingData data, vec3 n, vec3 lightDir, vec3 colorA, vec3 colorB){
    float fresnel = getthatfuckingfresnel(0.02, n, normalize(data.cameraPos), 0.0);
    fresnel = mix(fresnel, 0.0, data.roughness);
    return sshadow * sun(reflect(normalize(data.cameraPos), n), dayData.sunDir, 1.0 - data.roughness , data.roughness) * 1.0 * colorA * fresnel + colorB * ( fresnel);
   // return  colorB * (  fresnel);
}


vec3 shadingNonMetalic(PostProceessingData data, vec3 lightDir, vec3 color){
    float fresnel = getthatfuckingfresnel(0.04, data.normal, normalize(data.cameraPos), data.roughness);
    float x = 1.0 - max(0, dot(lightDir, data.normal));
    vec3 radiance = shade(CameraPosition, vec3(fresnel), data.normal, data.worldPos, data.worldPos - lightDir * 1.0, color * data.diffuseColor, max(0.004, data.roughness), false) * mix(x, pow(x, 8.0), 1.0 - data.roughness);    

    vec3 difradiance = shadeDiffuse(CameraPosition, data.diffuseColor * (1.0 - fresnel), data.normal, data.worldPos, data.worldPos - lightDir * 1.0, color, 0.0, false) * x;
    //   return vec3(0);
    return radiance + difradiance; 
}
    
vec3 getDiffuseAtmosphereColor(){
    return textureLod(atmScattTex, VECTOR_UP, textureQueryLevels(atmScattTex) - 1.0).rgb;
}

vec3 getSunColor(float roughness){
    vec3 atm = textureLod(atmScattTex, dayData.sunDir, roughnessToMipmap(roughness, atmScattTex)).rgb * 1.0;
    return 5.0 * mix((atm * 1)  , vec3(1.5), max(0.00, dayData.sunDir.y)) * max(0.0, dayData.sunDir.y);
}

vec3 getAtmosphereScattering(vec3 dir, float roughness){
    return textureLod(atmScattTex, dir, roughnessToMipmap(roughness, atmScattTex)).rgb;
}


vec3 shadeColor(PostProceessingData data, vec3 lightdir, vec3 c){
    return mix(shadingNonMetalic(data, lightdir, c), shadingMetalic(data, lightdir, c), data.metalness);// * (UseAO == 1 ? texture(aoxTex, UV).r : 1.0);
}

vec2 projectvdao(vec3 pos){
    vec4 tmp = (VPMatrix * vec4(pos, 1.0));
    return (tmp.xy / tmp.w) * 0.5 + 0.5;
}
vec4 projectvdao2(vec3 pos){
    vec4 tmp = (VPMatrix * vec4(pos, 1.0));
    return (tmp.xyzw);
}

float godrays(vec3 dir, int steps){
    if(NoiseOctave1 <= 0.011) return 1.0;
    float bias = 58.0 / float(steps);
    float stepsize = 1.0 / float(steps);
    float rd = rand2sTime(UV) * stepsize;
    
    float iter = 0.0;
    float result = 1.0;
    for(int i=0;i<steps;i++){
        result *= 1.0 - textureLod(coverageDistTex, normalize(mix(dir, dayData.sunDir, iter + rd)), 1.0).r * 0.03 * NoiseOctave1 * bias;
        iter += stepsize;
    }
    return result;
}

vec2 xyzToPolar(vec3 xyz){
    float radius = sqrt(xyz.x * xyz.x + xyz.y * xyz.y + xyz.z * xyz.z);
    float theta = atan(xyz.y, xyz.x);
    float phi = acos(xyz.z / radius);
    return vec2(theta, phi) / vec2(2.0 *3.1415,  3.1415);
}


vec3 getStars(vec3 dir, float roughness){
    dir = dayData.viewFrame * dir;
    vec3 c = pow(texture(starsTex, xyzToPolar(dir)).rgb, vec3(2.2));
    return mix(c, vec3(0.01, 0.02, 0.03) * 0.2 * (1.0  - reconstructCameraSpaceDistance(UV, 1.0).y * 0.8), 0.0);
}
mat3 calcLookAtMatrix(vec3 origin, vec3 target, float roll) {
  vec3 rr = vec3(sin(roll), cos(roll), 0.0);
  vec3 ww = normalize(target - origin);
  vec3 uu = normalize(cross(ww, rr));
  vec3 vv = normalize(cross(uu, ww));

  return mat3(uu, vv, ww);
}
float sun_moon_mult = 1.0;
vec3 textureMoon(vec3 dir){
  /*  vec3 mond = dayData.moonDir;
    vec3 mont = cross(vec3(0.0, 1.0, 0.0), mond);
    vec3 monb = cross(mont, mond);
    float dt = max(0.0, dot(dir, mond));
    vec3 dd = normalize(mond - dir);
    vec2 ud = vec2(dot(mont, dd), dot(monb, dd));
    float cr = 0.99885;
    dt = (dt - cr) / (1.0 - cr);
    float st = smoothstep(0.0, 0.01, dt);
    mond = normalize(dayData.moonPos - dayData.earthPos);
    Sphere moon = Sphere(mond * 362600.0, 17360.0);
    Ray r = Ray(CameraPosition, dir);
    float i = rsi2(r, moon);
    vec3 posOnMoon = CameraPosition + dir * i;
    vec3 n = normalize(posOnMoon - mond * 362600.0);
    float l = pow(max(0.0, dot(n, dayData.sunSpaceDir)), 1.2);
    return l * pow(textureLod(moonTex, clamp((sqrt(1.0 - dt) * ud) * 2.22 * 0.5 + 0.5, 0.0, 1.0), 0.0).rgb, vec3(2.4)) * 2.0 + getStars(dir, 0.0) * (1.0 - st);*/
    dir.y = abs(dir.y);
    Sphere moon = Sphere(dayData.moonPos, 17.37);
    Ray r = Ray(dayData.earthPos, dayData.viewFrame * dir);
    float i = rsi2(r, moon);
    vec3 posOnMoon = dayData.earthPos + dayData.viewFrame * dir * i;
    vec3 n = normalize(dayData.moonPos - posOnMoon);
    float l = pow(max(0.0, dot(n, dayData.sunSpaceDir)), 1.2);
    n *= rotationMatrix(vec3(0.0, 1.0, 0.0), 6.1415);
    n *= rotationMatrix(vec3(0.0, 0.0, 1.0), 3.1415);
    vec3 color = vec3(0.5) + pow(textureLod(moonTex, xyzToPolar(calcLookAtMatrix(dayData.moonPos, dayData.earthPos, 0.0) * n.xyz) , 0.0).rgb, vec3(2.4));
    
    vec3 atmdiff = vec3(0.004) * pow(asin(dot(dir, dayData.moonDir)) * 0.5 + 0.5, 4.0);
    
    vec3 atm = mix(vec3(0.7, 0.2, 0.0), vec3(1.0), 1.0 - pow(1.0 - max(0.0, dayData.moonDir.y * 1.1 - 0.1), 4.0));
    atmdiff *= atm;
    color *= atm;
    sun_moon_mult = step(0.0, i);
    float monsoonconverage = 1.0 - smoothstep(0.995, 1.0, dot(dayData.sunDir, dayData.moonDir));
    return clamp(atmdiff + l * color * sun_moon_mult * 0.1 + getStars(dir, 0.0) * (1.0 - sun_moon_mult), 0.0, 111.0);
}


float lenssun(vec3 dir){
    //return smoothstep(0.997, 0.999, dot(dir, dayData.sunDir));
    vec2 ss1 = projectvdao(CameraPosition + dir);
    vec2 ss2 = projectvdao(CameraPosition + dayData.sunDir);
    ss1.x *= Resolution.x / Resolution.y;
    ss2.x *= Resolution.x / Resolution.y;
    
    float angle = 360.0 + asin(dot(normalize(ss1 - ss2), vec2(0, 1))) * 180.0 / 3.1415;
    angle = abs(angle / 180.0);
    angle *= 6.0;
    angle = min(fract(angle), fract(-angle));
  //  return angle;
    float dt = clamp(dot(dir, dayData.sunDir) + 0.001, 0.0, 1.0);
    float d = pow(dt, 32.0);
    d = pow(d, 3.0 + 13.0 * (1.0 - pow(1.0 - clamp(angle , 0.0, 1.0), 5.0)));
    return pow(1.0 / (distance(dir, dayData.sunDir) * 10.0), 4.0);
}

vec3 sampleAtmosphere(vec3 dir, float roughness, float sun, int raysteps){
    float dimmer = max(0, 0.06 + 0.94 * dot(normalize(dayData.sunDir), vec3(0,1,0)));
    vec3 scattering = getAtmosphereScattering(dir, roughness);
    vec3 diffuse = getDiffuseAtmosphereColor();// * (1.0 - pow(1.0 - dimmer, 1.0)) * ( (1.0 - max(0.0, dayData.sunDir.y)) * 0.5 + 0.5);
    vec3 direct = getSunColor(roughness);
   // scattering *= godrays(dir) ;
    vec3 moon = textureMoon(dir);
    scattering += sun * (1.0 - step(0.1, length(currentData.normal))) * lenssun(dir) * getSunColor(0.0) * 1.0;
    float monsoonconverage = (1.0 - smoothstep(0.995, 1.0, dot(dayData.sunDir, dayData.moonDir))) * 0.7 + 0.3;
    vec4 cloudsData = smartblur(dir, roughness);
    float coverage = cloudsData.r;
    sshadow = 1.0 - coverage;
    float dist = cloudsData.g;
    float shadow = cloudsData.b;
    float rays = 1.0;//godrays(dir, raysteps);
        
    vec3 cloud = mix(diffuse * rays, direct, shadow);
    return mix(scattering * rays * monsoonconverage + moon, monsoonconverage * cloud, coverage);
}

vec3 shadeFragment(PostProceessingData data){
    vec3 suncolor = getSunColor(0.0);
    vec3 sun = shadeColor(data, -dayData.sunDir, suncolor);
    vec3 diffuse = shadeColor(data, -data.normal, sampleAtmosphere(data.normal, data.roughness*0.5, 1.0, 0)) * 0.2;
    return sun * CSMQueryVisibility(data.worldPos) + diffuse;
}



vec3 getNormalLighting(vec2 uv, PostProceessingData data){
    if(length(data.normal) < 0.01){
        data.roughness = 0.0;
        vec3 dir = reconstructCameraSpaceDistance(uv, 1.0);
        return sampleAtmosphere(dir, 0.0, 1.0, 23);
    } else {
        return shadeFragment(data);
    }
}

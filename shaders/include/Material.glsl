layout(binding = 10) uniform sampler2D diffuseColorTex;
layout(binding = 11) uniform sampler2D normalTex;
layout(binding = 12) uniform sampler2D bumpTex;
layout(binding = 13) uniform sampler2D roughnessTex;
layout(binding = 14) uniform sampler2D metalnessTex;

uniform int useDiffuseColorTexInt;
uniform int useNormalTexInt;
uniform int useBumpTexInt;
uniform int useRoughnessTexInt;
uniform int useMetalnessTexInt;

uniform vec3 DiffuseColor;
uniform float Roughness;
uniform float Metalness;

uniform vec2 diffuseColorTexScale;
uniform vec2 normalTexScale;
uniform vec2 bumpTexScale;
uniform vec2 roughnessTexScale;
uniform vec2 metalnessTexScale;

float ParallaxHeightMultiplier = 1.03;
float newParallaxHeight = 0;
float parallaxScale = 0.02 * ParallaxHeightMultiplier;
// modded http://sunandblackcat.com/tipFullView.php?topicid=28
vec2 adjustParallaxUV(vec2 uv){
    vec3 twpos = quat_mul_vec(ModelInfos[Input.instanceId].Rotation, normalize(Input.Tangent.xyz));
    vec3 nwpos = quat_mul_vec(ModelInfos[Input.instanceId].Rotation, normalize(Input.Normal));
    vec3 bwpos =  normalize(cross(twpos, nwpos)) * Input.Tangent.w;
    vec3 eyevec = ((CameraPosition - Input.WorldPos));
    vec3 V = normalize(vec3(
        dot(eyevec, twpos),
        dot(eyevec, -bwpos),
        dot(eyevec, -nwpos)
    ));
    vec2 T = uv;
   const float minLayers = 6;
   const float maxLayersAngle = 11;
   const float maxLayersDistance = 24;
   float numLayers = mix(minLayers, maxLayersAngle, abs(dot(nwpos, V)));
   numLayers = mix(maxLayersDistance, numLayers, clamp(distance(CameraPosition, Input.WorldPos) * 1, 0.0, 1.0)) * ParallaxHeightMultiplier;
   float layerHeight = 1.0 / numLayers;
   float curLayerHeight = 0;
   vec2 dtex = parallaxScale * V.xy / V.z / numLayers;
   vec2 currentTextureCoords = T;
   float heightFromTexture = 1.0 - texture(bumpTex, currentTextureCoords * bumpTexScale).r;
   int cnt = int(numLayers);
   while(heightFromTexture > curLayerHeight && cnt-- >= 0)
   {
      curLayerHeight += layerHeight;
      currentTextureCoords -= dtex;
      heightFromTexture = 1.0 - texture(bumpTex, currentTextureCoords * bumpTexScale).r;
   }
   vec2 prevTCoords = currentTextureCoords + dtex;
   float nextH = heightFromTexture - curLayerHeight;
   float prevH = 1.0 - texture(bumpTex, prevTCoords * bumpTexScale).r - curLayerHeight + layerHeight;
   float weight = nextH / (nextH - prevH);
   vec2 finalTexCoords = prevTCoords * weight + currentTextureCoords * (1.0-weight);
   newParallaxHeight = curLayerHeight + prevH * weight + nextH * (1.0 - weight);
   return finalTexCoords;
}

vec3 examineBumpMap(vec2 iuv){
    float bc = texture(bumpTex, iuv).r;
    vec2 dsp = 1.0 / vec2(textureSize(bumpTex, 0)) * 1;
    float bdx = texture(bumpTex, iuv).r - texture(bumpTex, iuv+vec2(dsp.x, 0)).r;
    float bdy = texture(bumpTex, iuv).r - texture(bumpTex, iuv+vec2(0, dsp.y)).r;

    return normalize(vec3( bdx * 3.1415 * 1.0, bdy * 3.1415 * 1.0,max(0, 1.0 - bdx - bdy)));
}

float toLogDepth(float depth, float far){
    float badass_depth = log2(max(1e-6, 1.0 + depth)) / (log2(far));
    return badass_depth;
}

void outputMaterial(){
    vec2 UV = Input.TexCoord;
    vec3 diffuseColor = DiffuseColor;
    float metalness = Metalness;
    float roughness = Roughness;
    float normalfrequency = abs(1.0 - length(Input.Normal));
    vec3 normal = normalize(Input.Normal);
    vec3 normalmap = vec3(1.0);
    vec3 tangent = normalize(Input.Tangent.rgb);
    float tangentSign = Input.Tangent.w;
    mat3 TBN = mat3(
        normalize(tangent),
        normalize(cross(normal, tangent)) * tangentSign,
        normalize(normal)
    );
    if(useBumpTexInt > 0){
        UV = adjustParallaxUV(UV);
        normalmap *= examineBumpMap(UV);
    }
    if(useDiffuseColorTexInt > 0){
        diffuseColor = texture(diffuseColorTex, UV * diffuseColorTexScale).rgb;
    }
    if(useNormalTexInt > 0){
        vec3 normalmaptex = normalize(texture(normalTex, UV * normalTexScale).rgb * 2 - 1);
        normalmaptex.r *= -1.0;
        normalmaptex.g *= -1.0;
        normalmap *= normalmaptex;
    }
    if(useRoughnessTexInt > 0){
        roughness = texture(roughnessTex, UV * roughnessTexScale).r;
    }
    if(useMetalnessTexInt > 0){
        metalness = texture(metalnessTex, UV * metalnessTexScale).r;
    }
    if(useNormalTexInt > 0 || useBumpTexInt > 0){
        normal = TBN * normalmap;
    }
    normal = quat_mul_vec(ModelInfos[Input.instanceId].Rotation, normal);

    outAlbedoRoughness = vec4(diffuseColor, roughness);
    outNormalsMetalness = vec4(normal, metalness);

    outDistance = max(0.01, distance(CameraPosition, Input.WorldPos));
    //gl_FragDepth = toLogDepth(outDistance, 20000.0);
}

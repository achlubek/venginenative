#version 430 core

in Data {
#include InOutStageLayout.glsl
} Input;

layout(location = 0) out vec4 outAlbedoRoughness;
layout(location = 1) out vec4 outNormalsMetalness;
layout(location = 2) out float outDistance;

#include Quaternions.glsl
#include ModelBuffer.glsl
#include Mesh3dUniforms.glsl

#include Material.glsl

#define bitcheck(input,bit) ((input & bit) != 0)

vec3 saturation(vec3 rgb, float adjustment)
{
    // Algorithm from Chapter 16 of OpenGL Shading Language
    const vec3 W = vec3(0.2125, 0.7154, 0.0721);
    vec3 intensity = vec3(dot(rgb, W));
    return mix(intensity, rgb, adjustment);
}

vec3 rgb2hsv(vec3 c)
{
    vec4 K = vec4(0.0, -1.0 / 3.0, 2.0 / 3.0, -1.0);
    vec4 p = mix(vec4(c.bg, K.wz), vec4(c.gb, K.xy), step(c.b, c.g));
    vec4 q = mix(vec4(p.xyw, c.r), vec4(c.r, p.yzx), step(p.x, c.r));

    float d = q.x - min(q.w, q.y);
    float e = 1.0e-10;
    return vec3(abs(q.z + (q.w - q.y) / (6.0 * d + e)), d / (q.x + e), q.x);
}

vec3 hsv2rgb(vec3 c)
{
    vec4 K = vec4(1.0, 2.0 / 3.0, 1.0 / 3.0, 3.0);
    vec3 p = abs(fract(c.xxx + K.xyz) * 6.0 - K.www);
    return c.z * mix(K.xxx, clamp(p - K.xxx, 0.0, 1.0), c.y);
}

float toLogDepth(float depth, float far){
    float badass_depth = log2(max(1e-6, 1.0 + depth)) / (log2(far));
    return badass_depth;
}

float ParallaxHeightMultiplier = 1.03;
float newParallaxHeight = 0;
float parallaxScale = 0.02 * ParallaxHeightMultiplier;
vec2 adjustParallaxUV(int node, vec2 uv){

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
   // determine optimal number of layers
   const float minLayers = 6;
   const float maxLayersAngle = 11;
   const float maxLayersDistance = 24;
   float numLayers = mix(minLayers, maxLayersAngle, abs(dot(nwpos, V)));
   numLayers = mix(maxLayersDistance, numLayers, clamp(distance(CameraPosition, Input.WorldPos) * 1, 0.0, 1.0)) * ParallaxHeightMultiplier;
   //numLayers = 24;

   // height of each layer
   float layerHeight = 1.0 / numLayers;
   // current depth of the layer
   float curLayerHeight = 0;
   // shift of texture coordinates for each layer
   vec2 dtex = parallaxScale * V.xy / V.z / numLayers;

   // current texture coordinates
   vec2 currentTextureCoords = T;

   // depth from heightmap
   float heightFromTexture = 1.0 - sampleNode(node, currentTextureCoords).r;

   // while point is above the surface
   int cnt = int(numLayers);
   while(heightFromTexture > curLayerHeight && cnt-- >= 0)
   {
      // to the next layer
      curLayerHeight += layerHeight;
      // shift of texture coordinates
      currentTextureCoords -= dtex;
      // new depth from heightmap
      heightFromTexture = 1.0 - sampleNode(node, currentTextureCoords).r;
   }

   ///////////////////////////////////////////////////////////

   // previous texture coordinates
   vec2 prevTCoords = currentTextureCoords + dtex;

   // heights for linear interpolation
   float nextH    = heightFromTexture - curLayerHeight;
   float prevH    = 1.0 - sampleNode(node, prevTCoords).r
                           - curLayerHeight + layerHeight;

   // proportions for linear interpolation
   float weight = nextH / (nextH - prevH);

   // interpolation of texture coordinates
   vec2 finalTexCoords = prevTCoords * weight + currentTextureCoords * (1.0-weight);

   // interpolation of depth values
   newParallaxHeight = curLayerHeight + prevH * weight + nextH * (1.0 - weight);

   // return result
   return finalTexCoords;
}

void main(){
    //float bump = getBump(Input.TexCoord);
    //if(Input.Data.x < 1.0 && bump >= Input.Data.x) discard;
    vec3 diffuseColor = DiffuseColor;
    vec3 normal = normalize(Input.Normal);
    vec3 normalmap = vec3(0,0,1);
    float roughness = Roughness;
    float metalness = Metalness;

    vec2 UV = Input.TexCoord;

    vec3 tangent = normalize(Input.Tangent.rgb);

    float tangentSign = Input.Tangent.w;

    mat3 TBN = mat3(
        normalize(tangent),
        normalize(cross(normal, tangent)) * tangentSign,
        normalize(normal)
    );

    vec4 nodesdata[] = vec4[MAX_NODES](
        texture(texBind0 , UV * getModifier(0).uvScale).rgba,
        texture(texBind1 , UV * getModifier(1).uvScale).rgba,
        texture(texBind2 , UV * getModifier(2).uvScale).rgba,
        texture(texBind3 , UV * getModifier(3).uvScale).rgba,
        texture(texBind4 , UV * getModifier(4).uvScale).rgba,
        texture(texBind5 , UV * getModifier(5).uvScale).rgba,
        texture(texBind6 , UV * getModifier(6).uvScale).rgba,
        texture(texBind7 , UV * getModifier(7).uvScale).rgba,
        texture(texBind8 , UV * getModifier(8).uvScale).rgba,
        texture(texBind9 , UV * getModifier(9).uvScale).rgba
    );

    bool modifiedNormal = false;
    for(int i=0;i<NodesCount;i++){
        NodeImageModifier node = getModifier(i);
        vec4 data = node.soureColor;
        if(node.source == MODSOURCE_TEXTURE) data = nodesdata[i];

        if(bitcheck(node.modifier, MODMODIFIER_NEGATIVE) && node.target == MODTARGET_NORMAL)data.rgb = vec3(1.0 - data.r, 1.0 - data.g, data.b);
        if(bitcheck(node.modifier, MODMODIFIER_NEGATIVE) && node.target != MODTARGET_NORMAL)data = 1.0-data;
        if(bitcheck(node.modifier, MODMODIFIER_LINEARIZE))data = pow(data, vec4(2.4));
        if(bitcheck(node.modifier, MODMODIFIER_SATURATE))data.rgb = saturation(data.rgb, node.data.r);
        if(bitcheck(node.modifier, MODMODIFIER_HUE)){
            vec3 hsv = rgb2hsv(data.rgb);
            hsv.x = fract(hsv.x + node.data.r);
            data.rgb = hsv2rgb(hsv);
        }
        if(bitcheck(node.modifier, MODMODIFIER_BRIGHTNESS)){
            vec3 hsv = rgb2hsv(data.rgb);
            hsv.z = node.data.r;
            data.rgb = hsv2rgb(hsv);
        }
        if(bitcheck(node.modifier, MODMODIFIER_HSV)){
            vec3 hsv = rgb2hsv(data.rgb);
            hsv.x = fract(hsv.x + node.data.r);
            hsv.y *= node.data.g;
            hsv.z *= node.data.b;
        }
        if(bitcheck(node.modifier, MODMODIFIER_POWER))data = pow(data, node.data);

        if(node.target == MODTARGET_DIFFUSE){
            diffuseColor = nodeCombine(diffuseColor, data.rgb, node.mode, data.a);
        }
        if(node.target == MODTARGET_NORMAL){
            normalmap = nodeCombine(normalmap, node.source == MODSOURCE_TEXTURE ? data.rgb * 2 - 1 : data.rgb, node.mode, data.a);
            modifiedNormal = true;
        }
        if(node.target == MODTARGET_ROUGHNESS){
            roughness = nodeCombine(roughness, data.r, node.mode, data.a);
        }
        if(node.target == MODTARGET_METALNESS){
            metalness = nodeCombine(metalness, data.r, node.mode, data.a);
        }
        if(node.target == MODTARGET_BUMP){
            UV = adjustParallaxUV(node.samplerIndex, UV * node.uvScale) / node.uvScale;
            data.rgb = examineBumpMap(retrieveSampler(node.samplerIndex), UV * node.uvScale);
            normalmap = nodeCombine(normalmap, data.rgb, node.mode, data.a);
            modifiedNormal = true;
        }
    }
    if(modifiedNormal){
        float df = min(1.0, abs(length(vec3(1.0)) - length(normalmap)));
        roughness = mix(roughness, 1.0, df/length(vec3(1.0)));
        normalmap = normalize(normalmap);
        normalmap.r = - normalmap.r;
        normalmap.g = - normalmap.g;
        normal = TBN * normalmap;
    }
    normal = quat_mul_vec(ModelInfos[Input.instanceId].Rotation, normal);

  //  float LowFrequencyAO = 1.0 - newParallaxHeight*0.7;
    //diffuseColor *=  bump;

    outAlbedoRoughness = vec4(diffuseColor, roughness);
    outNormalsMetalness = vec4(normal, metalness);

    outDistance = max(0.01, distance(CameraPosition, Input.WorldPos));

    gl_FragDepth = toLogDepth(outDistance, 20000.0);
}

#version 450 core
layout( local_size_x = 32, local_size_y = 32, local_size_z = 1 ) in;

//grid I propose is 32 x 32 for a start it is 1024 lights spheres

layout(binding = 26) uniform sampler2D sunRSMTex;
layout(binding = 27) uniform sampler2D sunRSMWPosTex;
layout(binding = 28) uniform sampler2D sunRSMNormTex;

// 1 float is 4 bytes
// 4 floats are 16 bytes
// 3 * 4 floats is 48 bytes
// 48 bytes * 1024 = 49152 bytes = 48 kb
struct SingleLight{
    vec4 position;
    vec4 direction_roughness;
    vec4 color;
};

layout (std430, binding = 6) buffer lightsbuffer
{
    SingleLight[] lights;
};

void main(){
    vec2 iuv = vec2(gl_GlobalInvocationID.xy);
    vec2 uv = iuv / vec2(320.0) + 0.5 / 320.0;
    /*uv = uv * 2.0 - 1.0;
    uv.x = sign(uv.x) * pow(abs(uv.x), 6.0);
    uv.y = sign(uv.y) * pow(abs(uv.y), 6.0);
    uv = uv * 0.5 + 0.5;*/
    vec2 pixela = 1.0 / textureSize(sunRSMTex, 0);
    vec3 pixel = vec3(pixela, 0.0);
    vec3 albedo = textureLod(sunRSMTex, uv, 0.0).rgb;
    vec3 wpos = textureLod(sunRSMWPosTex, uv, 0.0).rgb;
    vec3 normal = textureLod(sunRSMNormTex, uv, 0.0).rgb;
    /*
    vec3 dfx = wpos - textureLod(sunRSMWPosTex, uv + pixel.xz, 0.0).rgb;
    vec3 dfy = wpos - textureLod(sunRSMWPosTex, uv + pixel.zy, 0.0).rgb;
    vec2 dfs = vec2(length(dfx), length(dfy));
    vec3 orignormal = normalize(cross(dfx, dfy));*/
    lights[gl_GlobalInvocationID.x * gl_NumWorkGroups.x * gl_WorkGroupSize.x + gl_GlobalInvocationID.y] = SingleLight(vec4(wpos, length(normal)), vec4(normal, 0.0), vec4(albedo, 1.0));
}

#version 450 core

in Data {
#include InOutStageLayout.glsl
} Input;

layout(location = 0) out vec4 Output;

#include Quaternions.glsl
#include ModelBuffer.glsl
#include Mesh3dUniforms.glsl

#include Material.glsl

layout(binding = 12) uniform sampler2DArray CSMTex;
float rand2s(vec2 co){
    return fract(sin(dot(co.xy,vec2(12.9898,78.233))) * 43758.5453);
}
float rand2sTime(vec2 co){
    co *= Time;
    return fract(sin(dot(co.xy,vec2(12.9898,78.233))) * 43758.5453);
}
vec2 UV = gl_FragCoord.xy / Resolution.xy;
#include CSM.glsl


layout (binding = 1, r32ui)  uniform coherent uimage3D VoxelsTextureRed;
layout (binding = 2, r32ui)  uniform coherent uimage3D VoxelsTextureGreen;
layout (binding = 3, r32ui)  uniform coherent uimage3D VoxelsTextureBlue;
layout (binding = 4, r32ui)  uniform coherent uimage3D VoxelsTextureCount;
uniform vec3 BoxSize;
uniform vec3 MapPosition;
uniform int GridSizeX;
uniform int GridSizeY;
uniform int GridSizeZ;

// xyz in range 0 -> 1
void WriteData3d(vec3 xyz, vec3 color){
    uint r = uint(color.r * 256.0);
    uint g = uint(color.g * 256.0);
    uint b = uint(color.b * 256.0);
    ivec3 iuv = ivec3(xyz * vec3(GridSizeX, GridSizeY, GridSizeZ));
    imageAtomicAdd(VoxelsTextureCount, iuv, 1);
    imageAtomicAdd(VoxelsTextureRed, iuv, r);
    imageAtomicAdd(VoxelsTextureGreen, iuv, g);
    imageAtomicAdd(VoxelsTextureBlue, iuv, b);
}

void main(){
    vec3 df = DiffuseColor;
    if(useDiffuseColorTexInt > 0){
        df = texture(diffuseColorTex, Input.TexCoord * diffuseColorTexScale).rgb;
    }
    df *= 0.1 + 0.9 * CSMQueryVisibilitySimple(Input.WorldPos);
    vec3 mp = floor(MapPosition * BoxSize) / BoxSize + (0.5 / BoxSize);
    vec3 hafbox = (((Input.WorldPos - mp) / BoxSize) * 0.5 + 0.5);
    if(hafbox.x > 0.0 && hafbox.x < 1.0 && hafbox.y > 0.0 && hafbox.y < 1.0 && hafbox.z > 0.0 && hafbox.z < 1.0){
        WriteData3d(hafbox, df);
    }
    Output = vec4(df, 0.0);
}

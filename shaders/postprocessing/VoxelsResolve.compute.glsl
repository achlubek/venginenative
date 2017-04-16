#version 450 core
layout( local_size_x = 1, local_size_y = 1, local_size_z = 1 ) in;


layout (binding = 1, r32ui)  uniform coherent uimage3D VoxelsTextureRed;
layout (binding = 2, r32ui)  uniform coherent uimage3D VoxelsTextureGreen;
layout (binding = 3, r32ui)  uniform coherent uimage3D VoxelsTextureBlue;
layout (binding = 4, r32ui)  uniform coherent uimage3D VoxelsTextureCount;
layout (binding = 5, rgba16f) uniform coherent image3D VoxelsTextureResult0;
layout (binding = 6, rgba16f) uniform coherent image3D VoxelsTextureResult1;
layout (binding = 7, rgba16f) uniform coherent image3D VoxelsTextureResult2;

uniform int mode;
#define MODE_LOD0 0
#define MODE_LOD1 1
#define MODE_LOD2 1

void lod0(){
    ivec3 ba = ivec3(gl_GlobalInvocationID.xyz);
    float c = float(imageLoad(VoxelsTextureCount, ba).r);
    float r = float(imageLoad(VoxelsTextureRed, ba).r) / 256.0;
    float g = float(imageLoad(VoxelsTextureGreen, ba).r) / 256.0;
    float b = float(imageLoad(VoxelsTextureBlue, ba).r) / 256.0;
    vec4 vox = vec4(r,g,b,c + 0.001);
    ivec3 isize = imageSize(VoxelsTextureResult0);
    imageStore(VoxelsTextureResult0, ba, vox);
}
// LOD 1 and LOD 2 should run with work groups - 1!!
void lod1(){
    ivec3 ba = ivec3(gl_GlobalInvocationID.xyz) + ivec3(1,0,0);
    vec4 data = imageLoad(VoxelsTextureResult0, ba);
    data += imageLoad(VoxelsTextureResult0, ba + ivec3(1,0,0));
    data += imageLoad(VoxelsTextureResult0, ba + ivec3(0,1,0));
    data += imageLoad(VoxelsTextureResult0, ba + ivec3(0,0,1));
    data += imageLoad(VoxelsTextureResult0, ba - ivec3(1,0,0));
    data += imageLoad(VoxelsTextureResult0, ba - ivec3(0,1,0));
    data += imageLoad(VoxelsTextureResult0, ba - ivec3(0,0,1));
    imageStore(VoxelsTextureResult1, ba, data * 0.125);
}
void lod2(){
    ivec3 ba = ivec3(gl_GlobalInvocationID.xyz) + ivec3(1,0,0);
    vec4 data = imageLoad(VoxelsTextureResult1, ba);
    data += imageLoad(VoxelsTextureResult1, ba + ivec3(1,0,0));
    data += imageLoad(VoxelsTextureResult1, ba + ivec3(0,1,0));
    data += imageLoad(VoxelsTextureResult1, ba + ivec3(0,0,1));
    data += imageLoad(VoxelsTextureResult1, ba - ivec3(1,0,0));
    data += imageLoad(VoxelsTextureResult1, ba - ivec3(0,1,0));
    data += imageLoad(VoxelsTextureResult1, ba - ivec3(0,0,1));
    imageStore(VoxelsTextureResult2, ba, data * 0.125);
}

void main(){
    if(mode == MODE_LOD0) lod0();
    if(mode == MODE_LOD1) lod1();
    if(mode == MODE_LOD2) lod2();
}

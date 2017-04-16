#version 450 core
layout( local_size_x = 1, local_size_y = 1, local_size_z = 1 ) in;


layout (binding = 1, r32ui)  uniform coherent uimage3D VoxelsTextureRed;
layout (binding = 2, r32ui)  uniform coherent uimage3D VoxelsTextureGreen;
layout (binding = 3, r32ui)  uniform coherent uimage3D VoxelsTextureBlue;
layout (binding = 4, r32ui)  uniform coherent uimage3D VoxelsTextureCount;
layout (binding = 5, rgba16f) uniform coherent image3D VoxelsTextureResult;


void main(){
    ivec3 ba = ivec3(gl_GlobalInvocationID.xyz);
    float c = float(imageLoad(VoxelsTextureCount, ba).r);
    float r = float(imageLoad(VoxelsTextureRed, ba).r) / 256.0;
    float g = float(imageLoad(VoxelsTextureGreen, ba).r) / 256.0;
    float b = float(imageLoad(VoxelsTextureBlue, ba).r) / 256.0;
    vec3 rgb = c < 0.01 ? vec3(0) : (vec3(r, g, b) / c);
    vec4 vox = vec4(r,g,b,c + 0.001);
    imageStore(VoxelsTextureResult, ba, vec4(vox.rgb , vox.a) );
}

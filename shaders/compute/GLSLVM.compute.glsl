#version 450 core

layout( local_size_x = 1, local_size_y = 1, local_size_z = 1 ) in;

layout(binding = 0)  uniform sampler2D texBind0;
layout(binding = 1)  uniform sampler2D texBind1;
layout(binding = 2)  uniform sampler2D texBind2;
layout(binding = 3)  uniform sampler2D texBind3;
layout(binding = 4)  uniform sampler2D texBind4;
layout(binding = 5)  uniform sampler2D texBind5;
layout(binding = 6)  uniform sampler2D texBind6;
layout(binding = 7)  uniform sampler2D texBind7;
layout(binding = 8)  uniform sampler2D texBind8;
layout(binding = 9)  uniform sampler2D texBind9;

vec4 sampleNodeLod0(int i, vec2 uv){
    if(i == 0)  return textureLod(texBind0 , uv, 0).rgba;
    if(i == 1)  return textureLod(texBind1 , uv, 0).rgba;
    if(i == 2)  return textureLod(texBind2 , uv, 0).rgba;
    if(i == 3)  return textureLod(texBind3 , uv, 0).rgba;
    if(i == 4)  return textureLod(texBind4 , uv, 0).rgba;
    if(i == 5)  return textureLod(texBind5 , uv, 0).rgba;
    if(i == 6)  return textureLod(texBind6 , uv, 0).rgba;
    if(i == 7)  return textureLod(texBind7 , uv, 0).rgba;
    if(i == 8)  return textureLod(texBind8 , uv, 0).rgba;
    if(i == 9)  return textureLod(texBind9 , uv, 0).rgba;
}

layout (std430, binding = 0) coherent buffer SharedMemoryIntBuffer
{
  int SharedMemoryInt[];
};
layout (std430, binding = 1) coherent buffer SharedMemoryFloatBuffer
{
  float SharedMemoryFloat[];
};
layout (std430, binding = 2) coherent buffer SharedMemoryVec2Buffer
{
  vec2 SharedMemoryVec2[];
};
layout (std430, binding = 3) coherent buffer SharedMemoryVec3Buffer
{
  vec3 SharedMemoryVec3[];
};
layout (std430, binding = 4) coherent buffer SharedMemoryVec4Buffer
{
  vec4 SharedMemoryVec4[];
};

#define MASM_DIRECTIVE_STORE_INT -1
#define MASM_DIRECTIVE_STORE_FLOAT 104
#define MASM_DIRECTIVE_STORE_VEC2 1
#define MASM_DIRECTIVE_STORE_VEC3 2
#define MASM_DIRECTIVE_STORE_VEC4 3
#define MASM_DIRECTIVE_LOAD_INT 4
#define MASM_DIRECTIVE_LOAD_FLOAT 5
#define MASM_DIRECTIVE_LOAD_VEC2 6
#define MASM_DIRECTIVE_LOAD_VEC3 7
#define MASM_DIRECTIVE_LOAD_VEC4 8
#define MASM_DIRECTIVE_REGMOV_INT 93
#define MASM_DIRECTIVE_REGMOV_FLOAT 9
#define MASM_DIRECTIVE_REGMOV_VEC2 10
#define MASM_DIRECTIVE_REGMOV_VEC3 11
#define MASM_DIRECTIVE_REGMOV_VEC4 12

#define MASM_DIRECTIVE_TEXTURE_R 13
#define MASM_DIRECTIVE_TEXTURE_G 14
#define MASM_DIRECTIVE_TEXTURE_B 15
#define MASM_DIRECTIVE_TEXTURE_A 16

#define MASM_DIRECTIVE_TEXTURE_RG 17
#define MASM_DIRECTIVE_TEXTURE_GB 18
#define MASM_DIRECTIVE_TEXTURE_BA 19

#define MASM_DIRECTIVE_TEXTURE_RGB 20
#define MASM_DIRECTIVE_TEXTURE_GBA 21

#define MASM_DIRECTIVE_TEXTURE_RGBA 22

#define MASM_DIRECTIVE_MIX_FLOAT_FLOAT 23
#define MASM_DIRECTIVE_MIX_VEC2_FLOAT 24
#define MASM_DIRECTIVE_MIX_VEC3_FLOAT 25
#define MASM_DIRECTIVE_MIX_VEC4_FLOAT 26
#define MASM_DIRECTIVE_POW_FLOAT_FLOAT 27
#define MASM_DIRECTIVE_POW_VEC2_FLOAT 28
#define MASM_DIRECTIVE_POW_VEC3_FLOAT 29
#define MASM_DIRECTIVE_POW_VEC4_FLOAT 30

#define MASM_DIRECTIVE_JUMP_ABSOLUTE 31
#define MASM_DIRECTIVE_JUMP_RELATIVE 32 // cool

#define MASM_DIRECTIVE_LONG_JUMP_ABSOLUTE 105
#define MASM_DIRECTIVE_LONG_JUMP_RELATIVE 106 // cool

#define MASM_DIRECTIVE_LONG_JUMP_BACK 33 // and i thought recursiveness wouldnt be possible

#define MASM_DIRECTIVE_JUMP_IF_EQUAL_INT 34
#define MASM_DIRECTIVE_JUMP_IF_EQUAL_FLOAT 35
#define MASM_DIRECTIVE_JUMP_IF_EQUAL_VEC2 36
#define MASM_DIRECTIVE_JUMP_IF_EQUAL_VEC3 37
#define MASM_DIRECTIVE_JUMP_IF_EQUAL_VEC4 38

#define MASM_DIRECTIVE_JUMP_IF_NOT_EQUAL_INT 39
#define MASM_DIRECTIVE_JUMP_IF_NOT_EQUAL_FLOAT 40
#define MASM_DIRECTIVE_JUMP_IF_NOT_EQUAL_VEC2 41
#define MASM_DIRECTIVE_JUMP_IF_NOT_EQUAL_VEC3 42
#define MASM_DIRECTIVE_JUMP_IF_NOT_EQUAL_VEC4 43

#define MASM_DIRECTIVE_JUMP_IF_HIGHER_INT 44
#define MASM_DIRECTIVE_JUMP_IF_HIGHER_FLOAT 45
#define MASM_DIRECTIVE_JUMP_IF_HIGHER_VEC2 46
#define MASM_DIRECTIVE_JUMP_IF_HIGHER_VEC3 47
#define MASM_DIRECTIVE_JUMP_IF_HIGHER_VEC4 48

#define MASM_DIRECTIVE_JUMP_IF_LOWER_INT 49
#define MASM_DIRECTIVE_JUMP_IF_LOWER_FLOAT 50
#define MASM_DIRECTIVE_JUMP_IF_LOWER_VEC2 51
#define MASM_DIRECTIVE_JUMP_IF_LOWER_VEC3 52
#define MASM_DIRECTIVE_JUMP_IF_LOWER_VEC4 53

#define MASM_DIRECTIVE_INC_INT 54
#define MASM_DIRECTIVE_DEC_INT 55

#define MASM_DIRECTIVE_ADD_INT 56
#define MASM_DIRECTIVE_ADD_FLOAT 57
#define MASM_DIRECTIVE_ADD_VEC2 58
#define MASM_DIRECTIVE_ADD_VEC2_FLOAT 59
#define MASM_DIRECTIVE_ADD_VEC3 60
#define MASM_DIRECTIVE_ADD_VEC3_FLOAT 61
#define MASM_DIRECTIVE_ADD_VEC4 62
#define MASM_DIRECTIVE_ADD_VEC4_FLOAT 63

#define MASM_DIRECTIVE_SUB_INT 64
#define MASM_DIRECTIVE_SUB_FLOAT 65
#define MASM_DIRECTIVE_SUB_VEC2 66
#define MASM_DIRECTIVE_SUB_VEC2_FLOAT 67
#define MASM_DIRECTIVE_SUB_VEC3 68
#define MASM_DIRECTIVE_SUB_VEC3_FLOAT 69
#define MASM_DIRECTIVE_SUB_VEC4 70
#define MASM_DIRECTIVE_SUB_VEC4_FLOAT 71

#define MASM_DIRECTIVE_MUL_INT 72
#define MASM_DIRECTIVE_MUL_FLOAT 73
#define MASM_DIRECTIVE_MUL_VEC2 74
#define MASM_DIRECTIVE_MUL_VEC2_FLOAT 75
#define MASM_DIRECTIVE_MUL_VEC3 76
#define MASM_DIRECTIVE_MUL_VEC3_FLOAT 77
#define MASM_DIRECTIVE_MUL_VEC4 78
#define MASM_DIRECTIVE_MUL_VEC4_FLOAT 79

#define MASM_DIRECTIVE_DIV_INT 80
#define MASM_DIRECTIVE_DIV_FLOAT 81
#define MASM_DIRECTIVE_DIV_VEC2 82
#define MASM_DIRECTIVE_DIV_VEC2_FLOAT 83
#define MASM_DIRECTIVE_DIV_VEC3 84
#define MASM_DIRECTIVE_DIV_VEC3_FLOAT 85
#define MASM_DIRECTIVE_DIV_VEC4 86
#define MASM_DIRECTIVE_DIV_VEC4_FLOAT 87

#define MASM_DIRECTIVE_MOV_INVOCATION_ID 88

#define MASM_DIRECTIVE_CAST_INT_FLOAT 89
#define MASM_DIRECTIVE_CAST_FLOAT_INT 90
#define MASM_DIRECTIVE_CAST_FLOAT_VEC2 91
#define MASM_DIRECTIVE_CAST_FLOAT_VEC3 92
#define MASM_DIRECTIVE_CAST_FLOAT_VEC4 94 // one reserved for missing REGMOV_INT

#define MASM_DIRECTIVE_LOAD_BY_POINTER_INT 95
#define MASM_DIRECTIVE_LOAD_BY_POINTER_FLOAT 96
#define MASM_DIRECTIVE_LOAD_BY_POINTER_VEC2 97
#define MASM_DIRECTIVE_LOAD_BY_POINTER_VEC3 98
#define MASM_DIRECTIVE_LOAD_BY_POINTER_VEC4 99

#define MASM_DIRECTIVE_STORE_BY_POINTER_INT 100
#define MASM_DIRECTIVE_STORE_BY_POINTER_FLOAT 101
#define MASM_DIRECTIVE_STORE_BY_POINTER_VEC2 102
#define MASM_DIRECTIVE_STORE_BY_POINTER_VEC3 103
#define MASM_DIRECTIVE_STORE_BY_POINTER_VEC4 107

#define MASM_DIRECTIVE_HALT 0

#define MASM_JUMP_STACK_SIZE 16
#define MASM_OUTPUT_CHUNK_SIZE 4
#define MASM_REGISTER_CHUNK_SIZE 32

#define MAX_PROGRAM_LENGTH 2048

#define MASM_MAX_TEXTURES 10

uniform vec2 TexturesScales[MASM_MAX_TEXTURES];
uniform int TexturesCount;

struct MaterialObject{
    vec3 diffuseColor;
    vec3 normal;
};

void main(){
    int i = 0;

    int memory_int[MASM_REGISTER_CHUNK_SIZE];
    float memory_float[MASM_REGISTER_CHUNK_SIZE];
    vec2 memory_vec2[MASM_REGISTER_CHUNK_SIZE];
    vec3 memory_vec3[MASM_REGISTER_CHUNK_SIZE];
    vec4 memory_vec4[MASM_REGISTER_CHUNK_SIZE];

    float output_float[MASM_OUTPUT_CHUNK_SIZE];
    vec3 output_vec3[MASM_OUTPUT_CHUNK_SIZE];
    vec4 output_vec4[MASM_OUTPUT_CHUNK_SIZE];

    int jump_stack[MASM_JUMP_STACK_SIZE];
    int jump_stack_pointer = 0;

    int target = 0;
    int source = 0;
    int temp1 = 0;
    int temp2 = 0;
    int temp3 = 0;
    while(true){
        int c = SharedMemoryInt[i++];
        switch(c){
            //################################################################//
            case MASM_DIRECTIVE_STORE_INT:
                target = SharedMemoryInt[i++];
                source = SharedMemoryInt[i++];
                SharedMemoryInt[target] = memory_int[source];
            break;
            case MASM_DIRECTIVE_STORE_FLOAT:
                target = SharedMemoryInt[i++];
                source = SharedMemoryInt[i++];
                SharedMemoryFloat[target] = memory_float[source];
            break;
            case MASM_DIRECTIVE_STORE_VEC2:
                target = SharedMemoryInt[i++];
                source = SharedMemoryInt[i++];
                SharedMemoryVec2[target] = memory_vec2[source];
            break;
            case MASM_DIRECTIVE_STORE_VEC3:
                target = SharedMemoryInt[i++];
                source = SharedMemoryInt[i++];
                SharedMemoryVec3[target] = memory_vec3[source];
            break;
            case MASM_DIRECTIVE_STORE_VEC4:
                target = SharedMemoryInt[i++];
                source = SharedMemoryInt[i++];
                SharedMemoryVec4[target] = memory_vec4[source];
            break;
            //################################################################//
            case MASM_DIRECTIVE_LOAD_INT:
                target = SharedMemoryInt[i++];
                source = SharedMemoryInt[i++];
                memory_int[target] = SharedMemoryInt[source];
            break;
            case MASM_DIRECTIVE_LOAD_FLOAT:
                target = SharedMemoryInt[i++];
                source = SharedMemoryInt[i++];
                memory_float[target] = SharedMemoryFloat[source];
            break;
            case MASM_DIRECTIVE_LOAD_VEC2:
                target = SharedMemoryInt[i++];
                source = SharedMemoryInt[i++];
                memory_vec2[target] = SharedMemoryVec2[source];
            break;
            case MASM_DIRECTIVE_LOAD_VEC3:
                target = SharedMemoryInt[i++];
                source = SharedMemoryInt[i++];
                memory_vec3[target] = SharedMemoryVec3[source];
            break;
            case MASM_DIRECTIVE_LOAD_VEC4:
                target = SharedMemoryInt[i++];
                source = SharedMemoryInt[i++];
                memory_vec4[target] = SharedMemoryVec4[source];
            break;
            //################################################################//
            case MASM_DIRECTIVE_REGMOV_INT:
                target = SharedMemoryInt[i++];
                source = SharedMemoryInt[i++];
                memory_int[target] = memory_int[source];
            break;
            case MASM_DIRECTIVE_REGMOV_FLOAT:
                target = SharedMemoryInt[i++];
                source = SharedMemoryInt[i++];
                memory_float[target] = memory_float[source];
            break;
            case MASM_DIRECTIVE_REGMOV_VEC2:
                target = SharedMemoryInt[i++];
                source = SharedMemoryInt[i++];
                memory_vec2[target] = memory_vec2[source];
            break;
            case MASM_DIRECTIVE_REGMOV_VEC3:
                target = SharedMemoryInt[i++];
                source = SharedMemoryInt[i++];
                memory_vec3[target] = memory_vec3[source];
            break;
            case MASM_DIRECTIVE_REGMOV_VEC4:
                target = SharedMemoryInt[i++];
                source = SharedMemoryInt[i++];
                memory_vec4[target] = memory_vec4[source];
            break;
            //################################################################//
            case MASM_DIRECTIVE_TEXTURE_R:
                target = SharedMemoryInt[i++];
                source = SharedMemoryInt[i++];
                temp1 = SharedMemoryInt[i++];
                memory_float[target] = sampleNodeLod0(source, memory_vec2[temp1]).r;
            break;
            case MASM_DIRECTIVE_TEXTURE_G:
                target = SharedMemoryInt[i++];
                source = SharedMemoryInt[i++];
                temp1 = SharedMemoryInt[i++];
                memory_float[target] = sampleNodeLod0(source, memory_vec2[temp1]).g;
            break;
            case MASM_DIRECTIVE_TEXTURE_B:
                target = SharedMemoryInt[i++];
                source = SharedMemoryInt[i++];
                temp1 = SharedMemoryInt[i++];
                memory_float[target] = sampleNodeLod0(source, memory_vec2[temp1]).b;
            break;
            case MASM_DIRECTIVE_TEXTURE_A:
                target = SharedMemoryInt[i++];
                source = SharedMemoryInt[i++];
                temp1 = SharedMemoryInt[i++];
                memory_float[target] = sampleNodeLod0(source, memory_vec2[temp1]).a;
            break;
            ////////////
            case MASM_DIRECTIVE_TEXTURE_RG:
                target = SharedMemoryInt[i++];
                source = SharedMemoryInt[i++];
                temp1 = SharedMemoryInt[i++];
                memory_vec2[target] = sampleNodeLod0(source, memory_vec2[temp1]).rg;
            break;
            case MASM_DIRECTIVE_TEXTURE_GB:
                target = SharedMemoryInt[i++];
                source = SharedMemoryInt[i++];
                temp1 = SharedMemoryInt[i++];
                memory_vec2[target] = sampleNodeLod0(source, memory_vec2[temp1]).gb;
            break;
            case MASM_DIRECTIVE_TEXTURE_BA:
                target = SharedMemoryInt[i++];
                source = SharedMemoryInt[i++];
                temp1 = SharedMemoryInt[i++];
                memory_vec2[target] = sampleNodeLod0(source, memory_vec2[temp1]).ba;
            break;
            ////////////
            case MASM_DIRECTIVE_TEXTURE_RGB:
                target = SharedMemoryInt[i++];
                source = SharedMemoryInt[i++];
                temp1 = SharedMemoryInt[i++];
                memory_vec3[target] = sampleNodeLod0(source, memory_vec2[temp1]).rgb;
            break;
            case MASM_DIRECTIVE_TEXTURE_GBA:
                target = SharedMemoryInt[i++];
                source = SharedMemoryInt[i++];
                temp1 = SharedMemoryInt[i++];
                memory_vec3[target] = sampleNodeLod0(source, memory_vec2[temp1]).gba;
            break;
            ////////////
            case MASM_DIRECTIVE_TEXTURE_RGBA:
                target = SharedMemoryInt[i++];
                source = SharedMemoryInt[i++];
                temp1 = SharedMemoryInt[i++];
                memory_vec4[target] = sampleNodeLod0(source, memory_vec2[temp1]).rgba;
            break;
            //################################################################//
            case MASM_DIRECTIVE_MIX_FLOAT_FLOAT:
                target = SharedMemoryInt[i++];
                temp1 = SharedMemoryInt[i++];
                temp2 = SharedMemoryInt[i++];
                temp3 = SharedMemoryInt[i++];
                memory_float[target] = mix(memory_float[temp1], memory_float[temp2], memory_float[temp3]);
            break;
            case MASM_DIRECTIVE_MIX_VEC2_FLOAT:
                target = SharedMemoryInt[i++];
                temp1 = SharedMemoryInt[i++];
                temp2 = SharedMemoryInt[i++];
                temp3 = SharedMemoryInt[i++];
                memory_vec2[target] = mix(memory_vec2[temp1], memory_vec2[temp2], memory_float[temp3]);
            break;
            case MASM_DIRECTIVE_MIX_VEC3_FLOAT:
                target = SharedMemoryInt[i++];
                temp1 = SharedMemoryInt[i++];
                temp2 = SharedMemoryInt[i++];
                temp3 = SharedMemoryInt[i++];
                memory_vec3[target] = mix(memory_vec3[temp1], memory_vec3[temp2], memory_float[temp3]);
            break;
            case MASM_DIRECTIVE_MIX_VEC4_FLOAT:
                target = SharedMemoryInt[i++];
                temp1 = SharedMemoryInt[i++];
                temp2 = SharedMemoryInt[i++];
                temp3 = SharedMemoryInt[i++];
                memory_vec4[target] = mix(memory_vec4[temp1], memory_vec4[temp2], memory_float[temp3]);
            break;
            //################################################################//
            case MASM_DIRECTIVE_POW_FLOAT_FLOAT:
                target = SharedMemoryInt[i++];
                temp1 = SharedMemoryInt[i++];
                temp2 = SharedMemoryInt[i++];
                memory_float[target] = pow(memory_float[temp1], memory_float[temp2]);
            break;
            case MASM_DIRECTIVE_POW_VEC2_FLOAT:
                target = SharedMemoryInt[i++];
                temp1 = SharedMemoryInt[i++];
                temp2 = SharedMemoryInt[i++];
                memory_vec2[target] = pow(memory_vec2[temp1], vec2(memory_float[temp2]));
            break;
            case MASM_DIRECTIVE_POW_VEC3_FLOAT:
                target = SharedMemoryInt[i++];
                temp1 = SharedMemoryInt[i++];
                temp2 = SharedMemoryInt[i++];
                memory_vec3[target] = pow(memory_vec3[temp1], vec3(memory_float[temp2]));
            break;
            case MASM_DIRECTIVE_POW_VEC4_FLOAT:
                target = SharedMemoryInt[i++];
                temp1 = SharedMemoryInt[i++];
                temp2 = SharedMemoryInt[i++];
                memory_vec4[target] = pow(memory_vec4[temp1], vec4(memory_float[temp2]));
            break;
            //################################################################//
            case MASM_DIRECTIVE_JUMP_ABSOLUTE:
                temp1 = SharedMemoryInt[i++]; // raw address into the assembly
                i = temp1;
            break;
            case MASM_DIRECTIVE_JUMP_RELATIVE:
                temp1 = SharedMemoryInt[i++];
                i += temp1;
            break;
            case MASM_DIRECTIVE_LONG_JUMP_ABSOLUTE:
                temp1 = SharedMemoryInt[i++];
                jump_stack[jump_stack_pointer++] = i;
                i = temp1;
            break;
            case MASM_DIRECTIVE_LONG_JUMP_RELATIVE:
                temp1 = SharedMemoryInt[i++];
                jump_stack[jump_stack_pointer++] = i;
                i += temp1;
            break;
            case MASM_DIRECTIVE_LONG_JUMP_BACK:
                i = jump_stack[--jump_stack_pointer]; // beautiful!
            break;
            //################################################################//
            case MASM_DIRECTIVE_JUMP_IF_EQUAL_INT:
                temp1 = SharedMemoryInt[i++];
                temp2 = SharedMemoryInt[i++];
                if(!(memory_int[temp1] == memory_int[temp2])) i++;// skips next instruction, jump for example
            break;
            case MASM_DIRECTIVE_JUMP_IF_EQUAL_FLOAT:
                temp1 = SharedMemoryInt[i++];
                temp2 = SharedMemoryInt[i++];
                if(!(memory_float[temp1] == memory_float[temp2])) i++;
            break;
            case MASM_DIRECTIVE_JUMP_IF_EQUAL_VEC2:
                temp1 = SharedMemoryInt[i++];
                temp2 = SharedMemoryInt[i++];
                if(!(memory_vec2[temp1] == memory_vec2[temp2])) i++;
            break;
            case MASM_DIRECTIVE_JUMP_IF_EQUAL_VEC3:
                temp1 = SharedMemoryInt[i++];
                temp2 = SharedMemoryInt[i++];
                if(!(memory_vec3[temp1] == memory_vec3[temp2])) i++;
            break;
            case MASM_DIRECTIVE_JUMP_IF_EQUAL_VEC4:
                temp1 = SharedMemoryInt[i++];
                temp2 = SharedMemoryInt[i++];
                if(!(memory_vec4[temp1] == memory_vec4[temp2])) i++;
            break;
            /////////////
            case MASM_DIRECTIVE_JUMP_IF_NOT_EQUAL_INT:
                temp1 = SharedMemoryInt[i++];
                temp2 = SharedMemoryInt[i++];
                if(!(memory_int[temp1] != memory_int[temp2])) i++;
            break;
            case MASM_DIRECTIVE_JUMP_IF_NOT_EQUAL_FLOAT:
                temp1 = SharedMemoryInt[i++];
                temp2 = SharedMemoryInt[i++];
                if(!(memory_float[temp1] != memory_float[temp2])) i++;
            break;
            case MASM_DIRECTIVE_JUMP_IF_NOT_EQUAL_VEC2:
                temp1 = SharedMemoryInt[i++];
                temp2 = SharedMemoryInt[i++];
                if(!(memory_vec2[temp1] != memory_vec2[temp2])) i++;
            break;
            case MASM_DIRECTIVE_JUMP_IF_NOT_EQUAL_VEC3:
                temp1 = SharedMemoryInt[i++];
                temp2 = SharedMemoryInt[i++];
                if(!(memory_vec3[temp1] != memory_vec3[temp2])) i++;
            break;
            case MASM_DIRECTIVE_JUMP_IF_NOT_EQUAL_VEC4:
                temp1 = SharedMemoryInt[i++];
                temp2 = SharedMemoryInt[i++];
                if(!(memory_vec4[temp1] != memory_vec4[temp2])) i++;
            break;
            /////////////
            case MASM_DIRECTIVE_JUMP_IF_HIGHER_INT:
                temp1 = SharedMemoryInt[i++];
                temp2 = SharedMemoryInt[i++];
                if(!(memory_int[temp1] > memory_int[temp2])) i++;
            break;
            case MASM_DIRECTIVE_JUMP_IF_HIGHER_FLOAT:
                temp1 = SharedMemoryInt[i++];
                temp2 = SharedMemoryInt[i++];
                if(!(memory_float[temp1] > memory_float[temp2])) i++;
            break;
            case MASM_DIRECTIVE_JUMP_IF_HIGHER_VEC2:
                temp1 = SharedMemoryInt[i++];
                temp2 = SharedMemoryInt[i++];
                if(!(length(memory_vec2[temp1]) > length(memory_vec2[temp2]))) i++;
            break;
            case MASM_DIRECTIVE_JUMP_IF_HIGHER_VEC3:
                temp1 = SharedMemoryInt[i++];
                temp2 = SharedMemoryInt[i++];
                if(!(length(memory_vec3[temp1]) > length(memory_vec3[temp2]))) i++;
            break;
            case MASM_DIRECTIVE_JUMP_IF_HIGHER_VEC4:
                temp1 = SharedMemoryInt[i++];
                temp2 = SharedMemoryInt[i++];
                if(!(length(memory_vec4[temp1]) > length(memory_vec4[temp2]))) i++;
            break;
            /////////////
            case MASM_DIRECTIVE_JUMP_IF_LOWER_INT:
                temp1 = SharedMemoryInt[i++];
                temp2 = SharedMemoryInt[i++];
                if(!(memory_int[temp1] < memory_int[temp2])) i++;
            break;
            case MASM_DIRECTIVE_JUMP_IF_LOWER_FLOAT:
                temp1 = SharedMemoryInt[i++];
                temp2 = SharedMemoryInt[i++];
                if(!(memory_float[temp1] < memory_float[temp2])) i++;
            break;
            case MASM_DIRECTIVE_JUMP_IF_LOWER_VEC2:
                temp1 = SharedMemoryInt[i++];
                temp2 = SharedMemoryInt[i++];
                if(!(length(memory_vec2[temp1]) < length(memory_vec2[temp2]))) i++;
            break;
            case MASM_DIRECTIVE_JUMP_IF_LOWER_VEC3:
                temp1 = SharedMemoryInt[i++];
                temp2 = SharedMemoryInt[i++];
                if(!(length(memory_vec3[temp1]) < length(memory_vec3[temp2]))) i++;
            break;
            case MASM_DIRECTIVE_JUMP_IF_LOWER_VEC4:
                temp1 = SharedMemoryInt[i++];
                temp2 = SharedMemoryInt[i++];
                if(!(length(memory_vec4[temp1]) < length(memory_vec4[temp2]))) i++;
            break;
            //################################################################//
            case MASM_DIRECTIVE_INC_INT:
                target = SharedMemoryInt[i++];
                memory_int[target]++;
            break;
            case MASM_DIRECTIVE_DEC_INT:
                target = SharedMemoryInt[i++];
                memory_int[target]--;
            break;
            //################################################################//
            case MASM_DIRECTIVE_ADD_INT:
                target = SharedMemoryInt[i++];
                source = SharedMemoryInt[i++];
                memory_int[target] += memory_int[source];
            break;
            case MASM_DIRECTIVE_ADD_FLOAT:
                target = SharedMemoryInt[i++];
                source = SharedMemoryInt[i++];
                memory_float[target] += memory_float[source];
            break;
            case MASM_DIRECTIVE_ADD_VEC2:
                target = SharedMemoryInt[i++];
                source = SharedMemoryInt[i++];
                memory_vec2[target] += memory_vec2[source];
            break;
            case MASM_DIRECTIVE_ADD_VEC2_FLOAT:
                target = SharedMemoryInt[i++];
                source = SharedMemoryInt[i++];
                memory_vec2[target] += memory_float[source];
            break;
            case MASM_DIRECTIVE_ADD_VEC3:
                target = SharedMemoryInt[i++];
                source = SharedMemoryInt[i++];
                memory_vec3[target] += memory_vec3[source];
            break;
            case MASM_DIRECTIVE_ADD_VEC3_FLOAT:
                target = SharedMemoryInt[i++];
                source = SharedMemoryInt[i++];
                memory_vec3[target] += memory_float[source];
            break;
            case MASM_DIRECTIVE_ADD_VEC4:
                target = SharedMemoryInt[i++];
                source = SharedMemoryInt[i++];
                memory_vec4[target] += memory_vec4[source];
            break;
            case MASM_DIRECTIVE_ADD_VEC4_FLOAT:
                target = SharedMemoryInt[i++];
                source = SharedMemoryInt[i++];
                memory_vec4[target] += memory_float[source];
            break;
            //################################################################//
            case MASM_DIRECTIVE_SUB_INT:
                target = SharedMemoryInt[i++];
                source = SharedMemoryInt[i++];
                memory_int[target] -= memory_int[source];
            break;
            case MASM_DIRECTIVE_SUB_FLOAT:
                target = SharedMemoryInt[i++];
                source = SharedMemoryInt[i++];
                memory_float[target] -= memory_float[source];
            break;
            case MASM_DIRECTIVE_SUB_VEC2:
                target = SharedMemoryInt[i++];
                source = SharedMemoryInt[i++];
                memory_vec2[target] -= memory_vec2[source];
            break;
            case MASM_DIRECTIVE_SUB_VEC2_FLOAT:
                target = SharedMemoryInt[i++];
                source = SharedMemoryInt[i++];
                memory_vec2[target] -= memory_float[source];
            break;
            case MASM_DIRECTIVE_SUB_VEC3:
                target = SharedMemoryInt[i++];
                source = SharedMemoryInt[i++];
                memory_vec3[target] -= memory_vec3[source];
            break;
            case MASM_DIRECTIVE_SUB_VEC3_FLOAT:
                target = SharedMemoryInt[i++];
                source = SharedMemoryInt[i++];
                memory_vec3[target] -= memory_float[source];
            break;
            case MASM_DIRECTIVE_SUB_VEC4:
                target = SharedMemoryInt[i++];
                source = SharedMemoryInt[i++];
                memory_vec4[target] -= memory_vec4[source];
            break;
            case MASM_DIRECTIVE_SUB_VEC4_FLOAT:
                target = SharedMemoryInt[i++];
                source = SharedMemoryInt[i++];
                memory_vec4[target] -= memory_float[source];
            break;
            //################################################################//
            case MASM_DIRECTIVE_MUL_INT:
                target = SharedMemoryInt[i++];
                source = SharedMemoryInt[i++];
                memory_int[target] *= memory_int[source];
            break;
            case MASM_DIRECTIVE_MUL_FLOAT:
                target = SharedMemoryInt[i++];
                source = SharedMemoryInt[i++];
                memory_float[target] *= memory_float[source];
            break;
            case MASM_DIRECTIVE_MUL_VEC2:
                target = SharedMemoryInt[i++];
                source = SharedMemoryInt[i++];
                memory_vec2[target] *= memory_vec2[source];
            break;
            case MASM_DIRECTIVE_MUL_VEC2_FLOAT:
                target = SharedMemoryInt[i++];
                source = SharedMemoryInt[i++];
                memory_vec2[target] *= memory_float[source];
            break;
            case MASM_DIRECTIVE_MUL_VEC3:
                target = SharedMemoryInt[i++];
                source = SharedMemoryInt[i++];
                memory_vec3[target] *= memory_vec3[source];
            break;
            case MASM_DIRECTIVE_MUL_VEC3_FLOAT:
                target = SharedMemoryInt[i++];
                source = SharedMemoryInt[i++];
                memory_vec3[target] *= memory_float[source];
            break;
            case MASM_DIRECTIVE_MUL_VEC4:
                target = SharedMemoryInt[i++];
                source = SharedMemoryInt[i++];
                memory_vec4[target] *= memory_vec4[source];
            break;
            case MASM_DIRECTIVE_MUL_VEC4_FLOAT:
                target = SharedMemoryInt[i++];
                source = SharedMemoryInt[i++];
                memory_vec4[target] *= memory_float[source];
            break;
            //################################################################//
            case MASM_DIRECTIVE_DIV_INT:
                target = SharedMemoryInt[i++];
                source = SharedMemoryInt[i++];
                memory_int[target] /= memory_int[source];
            break;
            case MASM_DIRECTIVE_DIV_FLOAT:
                target = SharedMemoryInt[i++];
                source = SharedMemoryInt[i++];
                memory_float[target] /= memory_float[source];
            break;
            case MASM_DIRECTIVE_DIV_VEC2:
                target = SharedMemoryInt[i++];
                source = SharedMemoryInt[i++];
                memory_vec2[target] /= memory_vec2[source];
            break;
            case MASM_DIRECTIVE_DIV_VEC2_FLOAT:
                target = SharedMemoryInt[i++];
                source = SharedMemoryInt[i++];
                memory_vec2[target] /= memory_float[source];
            break;
            case MASM_DIRECTIVE_DIV_VEC3:
                target = SharedMemoryInt[i++];
                source = SharedMemoryInt[i++];
                memory_vec3[target] /= memory_vec3[source];
            break;
            case MASM_DIRECTIVE_DIV_VEC3_FLOAT:
                target = SharedMemoryInt[i++];
                source = SharedMemoryInt[i++];
                memory_vec3[target] /= memory_float[source];
            break;
            case MASM_DIRECTIVE_DIV_VEC4:
                target = SharedMemoryInt[i++];
                source = SharedMemoryInt[i++];
                memory_vec4[target] /= memory_vec4[source];
            break;
            case MASM_DIRECTIVE_DIV_VEC4_FLOAT:
                target = SharedMemoryInt[i++];
                source = SharedMemoryInt[i++];
                memory_vec4[target] /= memory_float[source];
            break;
            //################################################################//
            case MASM_DIRECTIVE_MOV_INVOCATION_ID:
                target = SharedMemoryInt[i++];
                memory_int[target] = int(gl_GlobalInvocationID);
            break;
            //################################################################//
            case MASM_DIRECTIVE_CAST_INT_FLOAT:
                target = SharedMemoryInt[i++];
                source = SharedMemoryInt[i++];
                memory_float[target] = float(memory_int[source]);
            break;
            case MASM_DIRECTIVE_CAST_FLOAT_INT:
                target = SharedMemoryInt[i++];
                source = SharedMemoryInt[i++];
                memory_int[target] = int(memory_float[source]);
            break;
            case MASM_DIRECTIVE_CAST_FLOAT_VEC2:
                target = SharedMemoryInt[i++];
                source = SharedMemoryInt[i++];
                memory_vec2[target] = vec2(memory_float[source]);
            break;
            case MASM_DIRECTIVE_CAST_FLOAT_VEC3:
                target = SharedMemoryInt[i++];
                source = SharedMemoryInt[i++];
                memory_vec3[target] = vec3(memory_float[source]);
            break;
            case MASM_DIRECTIVE_CAST_FLOAT_VEC4:
                target = SharedMemoryInt[i++];
                source = SharedMemoryInt[i++];
                memory_vec4[target] = vec4(memory_float[source]);
            break;
            //################################################################//
            case MASM_DIRECTIVE_LOAD_BY_POINTER_INT:
                target = SharedMemoryInt[i++];
                source = SharedMemoryInt[i++];
                memory_int[target] = SharedMemoryInt[memory_int[source]];
            break;
            case MASM_DIRECTIVE_LOAD_BY_POINTER_FLOAT:
                target = SharedMemoryInt[i++];
                source = SharedMemoryInt[i++];
                memory_float[target] = SharedMemoryFloat[memory_int[source]];
            break;
            case MASM_DIRECTIVE_LOAD_BY_POINTER_VEC2:
                target = SharedMemoryInt[i++];
                source = SharedMemoryInt[i++];
                memory_vec2[target] = SharedMemoryVec2[memory_int[source]];
            break;
            case MASM_DIRECTIVE_LOAD_BY_POINTER_VEC3:
                target = SharedMemoryInt[i++];
                source = SharedMemoryInt[i++];
                memory_vec3[target] = SharedMemoryVec3[memory_int[source]];
            break;
            case MASM_DIRECTIVE_LOAD_BY_POINTER_VEC4:
                target = SharedMemoryInt[i++];
                source = SharedMemoryInt[i++];
                memory_vec4[target] = SharedMemoryVec4[memory_int[source]];
            break;
            //################################################################//
            case MASM_DIRECTIVE_STORE_BY_POINTER_INT:
                target = SharedMemoryInt[i++];
                source = SharedMemoryInt[i++];
                SharedMemoryInt[memory_int[target]] = memory_int[source];
            break;
            case MASM_DIRECTIVE_STORE_BY_POINTER_FLOAT:
                target = SharedMemoryInt[i++];
                source = SharedMemoryInt[i++];
                SharedMemoryFloat[memory_int[target]] = memory_float[source];
            break;
            case MASM_DIRECTIVE_STORE_BY_POINTER_VEC2:
                target = SharedMemoryInt[i++];
                source = SharedMemoryInt[i++];
                SharedMemoryVec2[memory_int[target]] = memory_vec2[source];
            break;
            case MASM_DIRECTIVE_STORE_BY_POINTER_VEC3:
                target = SharedMemoryInt[i++];
                source = SharedMemoryInt[i++];
                SharedMemoryVec3[memory_int[target]] = memory_vec3[source];
            break;
            case MASM_DIRECTIVE_STORE_BY_POINTER_VEC4:
                target = SharedMemoryInt[i++];
                source = SharedMemoryInt[i++];
                SharedMemoryVec4[memory_int[target]] = memory_vec4[source];
            break;
            //################################################################//
            case MASM_DIRECTIVE_HALT:
                barrier();
                memoryBarrier();
                return;
            break;
        }
    }
    barrier();
    memoryBarrier();
}


#extension GL_ARB_bindless_texture : require

layout(binding = 10)  uniform sampler2D texBind0 ;
layout(binding = 11)  uniform sampler2D texBind1 ;
layout(binding = 12)  uniform sampler2D texBind2 ;
layout(binding = 13)  uniform sampler2D texBind3 ;
layout(binding = 14)  uniform sampler2D texBind4 ;
layout(binding = 15)  uniform sampler2D texBind5 ;
layout(binding = 16)  uniform sampler2D texBind6 ;
layout(binding = 17)  uniform sampler2D texBind7 ;
layout(binding = 18)  uniform sampler2D texBind8 ;
layout(binding = 19)  uniform sampler2D texBind9 ;

vec4 sampleNode(int i, vec2 uv){
    if(i == 0)  return texture(texBind0 , uv).rgba;
    if(i == 1)  return texture(texBind1 , uv).rgba;
    if(i == 2)  return texture(texBind2 , uv).rgba;
    if(i == 3)  return texture(texBind3 , uv).rgba;
    if(i == 4)  return texture(texBind4 , uv).rgba;
    if(i == 5)  return texture(texBind5 , uv).rgba;
    if(i == 6)  return texture(texBind6 , uv).rgba;
    if(i == 7)  return texture(texBind7 , uv).rgba;
    if(i == 8)  return texture(texBind8 , uv).rgba;
    if(i == 9)  return texture(texBind9 , uv).rgba;
}

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

sampler2D retrieveSampler(int i){
    if(i == 0)  return texBind0;
    if(i == 1)  return texBind1;
    if(i == 2)  return texBind2;
    if(i == 3)  return texBind3;
    if(i == 4)  return texBind4;
    if(i == 5)  return texBind5;
    if(i == 6)  return texBind6;
    if(i == 7)  return texBind7;
    if(i == 8)  return texBind8;
    if(i == 9)  return texBind9;
}

uniform vec3 DiffuseColor;
uniform float Roughness;
uniform float Metalness;

#define MODMODE_ADD 0
#define MODMODE_MUL 1
#define MODMODE_AVERAGE 2
#define MODMODE_SUB 3
#define MODMODE_ALPHA 4
#define MODMODE_ONE_MINUS_ALPHA 5
#define MODMODE_REPLACE 6
#define MODMODE_MAX 7
#define MODMODE_MIN 8
#define MODMODE_DISTANCE 9

#define MODMODIFIER_ORIGINAL 0
#define MODMODIFIER_NEGATIVE 1
#define MODMODIFIER_LINEARIZE 2
#define MODMODIFIER_SATURATE 4
#define MODMODIFIER_HUE 8
#define MODMODIFIER_BRIGHTNESS 16
#define MODMODIFIER_POWER 32
#define MODMODIFIER_HSV 64

#define MODTARGET_DIFFUSE 0
#define MODTARGET_NORMAL 1
#define MODTARGET_ROUGHNESS 2
#define MODTARGET_METALNESS 3
#define MODTARGET_BUMP 4
#define MODTARGET_DISPLACEMENT 6

#define MODSOURCE_COLOR 0
#define MODSOURCE_TEXTURE 1

#define WRAP_REPEAT 0
#define WRAP_MIRRORED 1
#define WRAP_BORDER 2

struct NodeImageModifier{
    int samplerIndex;
    int mode;
    int target;
    int modifier;
    int source;
    int wrap;
    vec2 uvScale;
    vec4 data;
    vec4 soureColor;
};
#define MAX_NODES 10
uniform int NodesCount;
uniform int SamplerIndexArray[MAX_NODES];
uniform int ModeArray[MAX_NODES];
uniform int TargetArray[MAX_NODES];
uniform int SourcesArray[MAX_NODES];
uniform int ModifiersArray[MAX_NODES];
uniform int WrapModesArray[MAX_NODES];
uniform vec2 UVScaleArray[MAX_NODES];
uniform vec4 NodeDataArray[MAX_NODES];
uniform vec4 SourceColorsArray[MAX_NODES];

NodeImageModifier getModifier(int i){
    return NodeImageModifier(
        SamplerIndexArray[i],
        ModeArray[i],
        TargetArray[i],
        ModifiersArray[i],
        SourcesArray[i],
        WrapModesArray[i],
        UVScaleArray[i],
        NodeDataArray[i],
        SourceColorsArray[i]
    );
}

float nodeCombine(float v1, float v2, int mode, float dataAlpha){
    if(mode == MODMODE_REPLACE) return v2;
    if(mode == MODMODE_ADD) return v1 + v2;
    if(mode == MODMODE_MUL) return v1 * v2;
    if(mode == MODMODE_AVERAGE) return mix(v1, v2, 0.5);
    if(mode == MODMODE_SUB) return v1 - v2;
    if(mode == MODMODE_ALPHA) return mix(v1, v2, dataAlpha);
    if(mode == MODMODE_ONE_MINUS_ALPHA) return mix(v1, v2, 1.0 - dataAlpha);
    if(mode == MODMODE_MAX) return max(v1, v2);
    if(mode == MODMODE_MIN) return min(v1, v2);
    if(mode == MODMODE_DISTANCE) return distance(v1, v2);
    return mix(v1, v2, 0.5);
}

vec3 nodeCombine(vec3 v1, vec3 v2, int mode, float dataAlpha){
    if(mode == MODMODE_REPLACE) return v2;
    if(mode == MODMODE_ADD) return v1 + v2;
    if(mode == MODMODE_MUL) return v1 * v2;
    if(mode == MODMODE_AVERAGE) return mix(v1, v2, 0.5);
    if(mode == MODMODE_SUB) return v1 - v2;
    if(mode == MODMODE_ALPHA) return mix(v1, v2, dataAlpha);
    if(mode == MODMODE_ONE_MINUS_ALPHA) return mix(v1, v2, 1.0 - dataAlpha);
    if(mode == MODMODE_MAX) return max(v1, v2);
    if(mode == MODMODE_MIN) return min(v1, v2);
    if(mode == MODMODE_DISTANCE) return vec3(distance(v1, v2));
    return mix(v1, v2, 0.5);
}

bool RunParallax = false;


float getBump(vec2 uv){
    float bump = 0;
    for(int i=0;i<NodesCount;i++){
        NodeImageModifier node = getModifier(i);
        if(node.target == MODTARGET_DISPLACEMENT){
            vec4 data = sampleNodeLod0(node.samplerIndex, uv * node.uvScale);
            bump = nodeCombine(bump, data.r, node.mode, data.a);
            RunParallax = true;
        }
    }
    return bump;
}

vec3 examineBumpMap(sampler2D bumpTex, vec2 iuv){
    float bc = texture(bumpTex, iuv).r;
    vec2 dsp = 1.0 / vec2(textureSize(bumpTex, 0)) * 1;
    float bdx = texture(bumpTex, iuv).r - texture(bumpTex, iuv+vec2(dsp.x, 0)).r;
    float bdy = texture(bumpTex, iuv).r - texture(bumpTex, iuv+vec2(0, dsp.y)).r;


    return normalize(vec3( bdx * 3.1415 * 1.0, bdy * 3.1415 * 1.0,max(0, 1.0 - bdx - bdy)));
}

//############################################################################//

#define MASM_DIRECTIVE_STORE_FLOAT 1
#define MASM_DIRECTIVE_STORE_VEC3 2
#define MASM_DIRECTIVE_STORE_VEC4 3
#define MASM_DIRECTIVE_CONSTMOV_INT 4
#define MASM_DIRECTIVE_CONSTMOV_FLOAT 5
#define MASM_DIRECTIVE_CONSTMOV_VEC2 6
#define MASM_DIRECTIVE_CONSTMOV_VEC3 7
#define MASM_DIRECTIVE_CONSTMOV_VEC4 8
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
#define MASM_TARGET_DIFFUSECOLOR 0
#define MASM_TARGET_NORMAL 1

#define MASM_OUTPUT_CHUNK_SIZE 4
#define MASM_REGISTER_CHUNK_SIZE 16

#define MAX_PROGRAM_UNIFORMS_FLOAT 32
uniform float AsmUniformsFloat[MAX_PROGRAM_UNIFORMS_FLOAT];
#define MAX_PROGRAM_UNIFORMS_INT 16
uniform int AsmUniformsInt[MAX_PROGRAM_UNIFORMS_INT];
#define MAX_PROGRAM_UNIFORMS_VEC2 16
uniform vec2 AsmUniformsVec2[MAX_PROGRAM_UNIFORMS_VEC2];
#define MAX_PROGRAM_UNIFORMS_VEC3 32
uniform vec3 AsmUniformsVec3[MAX_PROGRAM_UNIFORMS_VEC3];
#define MAX_PROGRAM_UNIFORMS_VEC4 32
uniform vec4 AsmUniformsVec4[MAX_PROGRAM_UNIFORMS_VEC4];

#define MAX_PROGRAM_LENGTH 1024
uniform int AsmProgram[MAX_PROGRAM_LENGTH];
uniform int AsmProgramLength;

#define MASM_MAX_TEXTURES 10
uniform vec2 TexturesScales[MASM_MAX_TEXTURES];
uniform int TexturesCount;

struct MaterialObject{
    vec3 diffuseColor;
    vec3 normal;
};

MaterialObject runVm(vec2 UV){
    int i=0;
    MaterialObject mo = MaterialObject(vec3(0.0), vec3(0.0));

    vec4 texdata[] = vec4[MASM_MAX_TEXTURES](
        texture(texBind0 , UV * TexturesScales[0]).rgba,
        texture(texBind1 , UV * TexturesScales[1]).rgba,
        texture(texBind2 , UV * TexturesScales[2]).rgba,
        texture(texBind3 , UV * TexturesScales[3]).rgba,
        texture(texBind4 , UV * TexturesScales[4]).rgba,
        texture(texBind5 , UV * TexturesScales[5]).rgba,
        texture(texBind6 , UV * TexturesScales[6]).rgba,
        texture(texBind7 , UV * TexturesScales[7]).rgba,
        texture(texBind8 , UV * TexturesScales[8]).rgba,
        texture(texBind9 , UV * TexturesScales[9]).rgba
    );

    float memory_float[MASM_REGISTER_CHUNK_SIZE];
    vec2 memory_vec2[MASM_REGISTER_CHUNK_SIZE];
    vec3 memory_vec3[MASM_REGISTER_CHUNK_SIZE];
    vec4 memory_vec4[MASM_REGISTER_CHUNK_SIZE];

    float output_float[MASM_OUTPUT_CHUNK_SIZE];
    vec3 output_vec3[MASM_OUTPUT_CHUNK_SIZE];
    vec4 output_vec4[MASM_OUTPUT_CHUNK_SIZE];

    int target = 0;
    int source = 0;
    int temp1 = 0;
    int temp2 = 0;
    int temp3 = 0;
    while(i < AsmProgramLength){
        int c = AsmProgram[i++];
        switch(c){
            //################################################################//
            case MASM_DIRECTIVE_STORE_FLOAT:
                target = AsmProgram[i++];
                source = AsmProgram[i++];
                output_float[target] = memory_float[source];
            break;
            case MASM_DIRECTIVE_STORE_VEC3:
                target = AsmProgram[i++];
                source = AsmProgram[i++];
                output_vec3[target] = memory_vec3[source];
            break;
            case MASM_DIRECTIVE_STORE_VEC4:
                target = AsmProgram[i++];
                source = AsmProgram[i++];
                output_vec4[target] = memory_vec4[source];
            break;
            //################################################################//
            case MASM_DIRECTIVE_CONSTMOV_INT:
                // NOT IMPLEMENTED
            break;
            case MASM_DIRECTIVE_CONSTMOV_FLOAT:
                target = AsmProgram[i++];
                source = AsmProgram[i++];
                memory_float[target] = AsmUniformsFloat[source];
            break;
            case MASM_DIRECTIVE_CONSTMOV_VEC2:
                target = AsmProgram[i++];
                source = AsmProgram[i++];
                memory_vec2[target] = AsmUniformsVec2[source];
            break;
            case MASM_DIRECTIVE_CONSTMOV_VEC3:
                target = AsmProgram[i++];
                source = AsmProgram[i++];
                memory_vec3[target] = AsmUniformsVec3[source];
            break;
            case MASM_DIRECTIVE_CONSTMOV_VEC4:
                target = AsmProgram[i++];
                source = AsmProgram[i++];
                memory_vec4[target] = AsmUniformsVec4[source];
            break;
            //################################################################//
            case MASM_DIRECTIVE_REGMOV_FLOAT:
                target = AsmProgram[i++];
                source = AsmProgram[i++];
                memory_float[target] = memory_float[source];
            break;
            case MASM_DIRECTIVE_REGMOV_VEC2:
                target = AsmProgram[i++];
                source = AsmProgram[i++];
                memory_vec2[target] = memory_vec2[source];
            break;
            case MASM_DIRECTIVE_REGMOV_VEC3:
                target = AsmProgram[i++];
                source = AsmProgram[i++];
                memory_vec3[target] = memory_vec3[source];
            break;
            case MASM_DIRECTIVE_REGMOV_VEC4:
                target = AsmProgram[i++];
                source = AsmProgram[i++];
                memory_vec4[target] = memory_vec4[source];
            break;
            //################################################################//
            case MASM_DIRECTIVE_TEXTURE_R:
                target = AsmProgram[i++];
                source = AsmProgram[i++];
                memory_float[target] = texdata[source].r;
            break;
            case MASM_DIRECTIVE_TEXTURE_G:
                target = AsmProgram[i++];
                source = AsmProgram[i++];
                memory_float[target] = texdata[source].g;
            break;
            case MASM_DIRECTIVE_TEXTURE_B:
                target = AsmProgram[i++];
                source = AsmProgram[i++];
                memory_float[target] = texdata[source].b;
            break;
            case MASM_DIRECTIVE_TEXTURE_A:
                target = AsmProgram[i++];
                source = AsmProgram[i++];
                memory_float[target] = texdata[source].a;
            break;
            ////////////
            case MASM_DIRECTIVE_TEXTURE_RG:
                target = AsmProgram[i++];
                source = AsmProgram[i++];
                memory_vec2[target] = texdata[source].rg;
            break;
            case MASM_DIRECTIVE_TEXTURE_GB:
                target = AsmProgram[i++];
                source = AsmProgram[i++];
                memory_vec2[target] = texdata[source].gb;
            break;
            case MASM_DIRECTIVE_TEXTURE_BA:
                target = AsmProgram[i++];
                source = AsmProgram[i++];
                memory_vec2[target] = texdata[source].ba;
            break;
            ////////////
            case MASM_DIRECTIVE_TEXTURE_RGB:
                target = AsmProgram[i++];
                source = AsmProgram[i++];
                memory_vec3[target] = texdata[source].rgb;
            break;
            case MASM_DIRECTIVE_TEXTURE_GBA:
                target = AsmProgram[i++];
                source = AsmProgram[i++];
                memory_vec3[target] = texdata[source].gba;
            break;
            ////////////
            case MASM_DIRECTIVE_TEXTURE_RGBA:
                target = AsmProgram[i++];
                source = AsmProgram[i++];
                memory_vec4[target] = texdata[source].rgba;
            break;
            //################################################################//
            case MASM_DIRECTIVE_MIX_FLOAT_FLOAT:
                temp1 = AsmProgram[i++];
                temp2 = AsmProgram[i++];
                temp3 = AsmProgram[i++];
                target = AsmProgram[i++];
                memory_float[target] = mix(memory_float[temp1], memory_float[temp2], memory_float[temp3]);
            break;
            case MASM_DIRECTIVE_MIX_VEC2_FLOAT:
                temp1 = AsmProgram[i++];
                temp2 = AsmProgram[i++];
                temp3 = AsmProgram[i++];
                target = AsmProgram[i++];
                memory_vec2[target] = mix(memory_vec2[temp1], memory_vec2[temp2], memory_float[temp3]);
            break;
            case MASM_DIRECTIVE_MIX_VEC3_FLOAT:
                temp1 = AsmProgram[i++];
                temp2 = AsmProgram[i++];
                temp3 = AsmProgram[i++];
                target = AsmProgram[i++];
                memory_vec3[target] = mix(memory_vec3[temp1], memory_vec3[temp2], memory_float[temp3]);
            break;
            case MASM_DIRECTIVE_MIX_VEC4_FLOAT:
                temp1 = AsmProgram[i++];
                temp2 = AsmProgram[i++];
                temp3 = AsmProgram[i++];
                target = AsmProgram[i++];
                memory_vec4[target] = mix(memory_vec4[temp1], memory_vec4[temp2], memory_float[temp3]);
            break;
            //################################################################//
            case MASM_DIRECTIVE_POW_FLOAT_FLOAT:
                temp1 = AsmProgram[i++];
                temp2 = AsmProgram[i++];
                target = AsmProgram[i++];
                memory_float[target] = pow(memory_float[temp1], memory_float[temp2]);
            break;
            case MASM_DIRECTIVE_POW_VEC2_FLOAT:
                temp1 = AsmProgram[i++];
                temp2 = AsmProgram[i++];
                target = AsmProgram[i++];
                memory_vec2[target] = pow(memory_vec2[temp1], vec2(memory_float[temp2]));
            break;
            case MASM_DIRECTIVE_POW_VEC3_FLOAT:
                temp1 = AsmProgram[i++];
                temp2 = AsmProgram[i++];
                target = AsmProgram[i++];
                memory_vec3[target] = pow(memory_vec3[temp1], vec3(memory_float[temp2]));
            break;
            case MASM_DIRECTIVE_POW_VEC4_FLOAT:
                temp1 = AsmProgram[i++];
                temp2 = AsmProgram[i++];
                target = AsmProgram[i++];
                memory_vec4[target] = pow(memory_vec4[temp1], vec4(memory_float[temp2]));
            break;
        }
    }
    mo.diffuseColor = output_vec3[MASM_TARGET_DIFFUSECOLOR];
    mo.normal = output_vec3[MASM_TARGET_NORMAL];
    return mo;
}









//

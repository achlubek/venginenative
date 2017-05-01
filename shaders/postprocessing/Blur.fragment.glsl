#version 430 core

layout(binding = 14) uniform sampler2D inputTex;

in vec2 UV;
layout(location = 0) out vec4 outcolor;

uniform int Pass;
uniform float u;// = mouse.x * 2.0 - 1.0;
//float a = 1.0 - mouse.y;
uniform float coef1;// = 1.0 / (sqrt(2.0 * 3.1415 * a * a)); // can be precomputed
uniform float coef2;// = -1.0 / (2.0 * a * a); // can be precomputed
uniform float GaussianScale;
uniform int Range;
uniform int UseGaussian;

float gaussian(float x){
	return coef1 * pow(2.7182818, pow(x * GaussianScale, 2.0) * coef2 );
}

void main(){
    vec2 weighter = Pass == 0 ? vec2(1.0, 0.0) : vec2(0.0, 1.0);
    vec4 sum = vec4(0.0);
    float sumw = 0.001;
    vec2 imgSize = vec2(textureSize(inputTex, 0));
    vec2 pixel = weighter * (1.0 / imgSize);
    float rangef = float(Range);
    for(float i = -rangef; i <= rangef; i += 1.0){
        vec2 niuv = UV + pixel * i;
        float w = UseGaussian == 1 ? (coef1 * pow(2.7182818, pow(i * GaussianScale, 2.0) * coef2 )) : 1.0;
        sum += w * texture(inputTex, clamp(niuv, 0.0, 1.0));
        sumw += w;
    }
    outcolor = sum / sumw;
}

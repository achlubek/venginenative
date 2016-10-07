#version 430 core

#include PostProcessEffectBase.glsl

layout(binding = 16) uniform sampler2D inputTex;

uniform float Time;


float hashS( vec2 p ) {
	float h = dot(p,vec2(127.1,311.7));	
    return fract(sin(h)*43758.5453123);
}
float noiseS( in vec2 p ) {
    const vec2 modifier = vec2(1.0, 0.0);
    vec2 i = floor( p );
    vec2 f = fract( p );	
	vec2 u = f*f*(3.0-2.0*f);
    return -1.0+2.0*mix( mix( hashS( i + modifier.yy ), 
                     hashS( i + modifier.xy ), u.x),
                mix( hashS( i + modifier.yx ), 
                     hashS( i + modifier.xx ), u.x), u.y);
 }
                     
float sea_octave(vec2 uv, float choppy) {
    uv += noiseS(uv);        
    vec2 wv = 1.0-abs(sin(uv));
    vec2 swv = abs(cos(uv));    
    wv = mix(wv,swv,wv);
    return pow(1.0-pow(wv.x * wv.y,0.65),choppy);
}
float seatime = 1.0 + Time * 0.8;
mat2 octave_m = mat2(1.6,1.2,-1.2,1.6);
float heightwater(vec2 uv) {
    float freq = 0.16;
    float amp = 1.0;
    uv *= 92.1;
    float choppy =3.0;
    
    float d, h = 0.0; 
    float a = 0.0;
    for(int i = 0; i < 4; i++) {        
    	d = sea_octave((uv+seatime)*freq,choppy);
    	d += sea_octave((uv-seatime)*freq,choppy);
        h += d * amp;        
        a += amp * 2.0;
    	uv *= octave_m; freq *= 1.9; amp *= 0.22;
        choppy = mix(choppy,1.0,0.2);
    }
    return h / a;
}

vec4 shade(){    
    //vec3 color = fxaa(inputTex, UV).rgb;
    
    vec2 uv = UV;
    float c1 = heightwater(uv);
    //float c2 = heightwater(vec2(uv.x, 1.0 - uv.y));
    //float c3 = heightwater(vec2(1.0 - uv.x, uv.y));
    //float c4 = heightwater(vec2(1.0 - uv.x, 1.0 - uv.y));
    //float color = 0.25 * (c1 + c2 + c3 + c4);
    float color = c1;
    return vec4(color);
}
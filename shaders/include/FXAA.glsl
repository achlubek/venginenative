#ifndef FXAA_REDUCE_MIN
    #define FXAA_REDUCE_MIN   (1.0/ 128.0)
#endif
#ifndef FXAA_REDUCE_MUL
    #define FXAA_REDUCE_MUL   (1.0 / 8.0)
#endif
#ifndef FXAA_SPAN_MAX
    #define FXAA_SPAN_MAX     8.0
#endif

//optimized version for mobile, where dependent 
//texture reads can be a bottleneck
vec2 v_rgbNW;
vec2 v_rgbNE;
vec2 v_rgbSW;
vec2 v_rgbSE;
vec2 v_rgbM;

void texcoords(vec2 fragCoord) {
    vec2 inverseVP = 1.0 / Resolution.xy;
    v_rgbNW = (fragCoord + vec2(-1.0, -1.0)) * inverseVP;
    v_rgbNE = (fragCoord + vec2(1.0, -1.0)) * inverseVP;
    v_rgbSW = (fragCoord + vec2(-1.0, 1.0)) * inverseVP;
    v_rgbSE = (fragCoord + vec2(1.0, 1.0)) * inverseVP;
    v_rgbM = vec2(fragCoord * inverseVP);
}

vec4 fxaa(sampler2D tex, vec2 fragCoordF) {
    vec4 color;
    vec2 fragCoord = fragCoordF * Resolution;
    texcoords(fragCoord);
    vec2 inverseVP = vec2(1.0 / Resolution.x, 1.0 / Resolution.y);
    vec3 rgbNW = textureLod(tex, v_rgbNW, 0.0).xyz;
    vec3 rgbNE = textureLod(tex, v_rgbNE, 0.0).xyz;
    vec3 rgbSW = textureLod(tex, v_rgbSW, 0.0).xyz;
    vec3 rgbSE = textureLod(tex, v_rgbSE, 0.0).xyz;
    vec4 texColor = textureLod(tex, v_rgbM, 0.0);
    vec3 rgbM  = texColor.xyz;
    vec3 luma = vec3(0.299, 0.587, 0.114);
    float lumaNW = dot(rgbNW, luma);
    float lumaNE = dot(rgbNE, luma);
    float lumaSW = dot(rgbSW, luma);
    float lumaSE = dot(rgbSE, luma);
    float lumaM  = dot(rgbM,  luma);
    float lumaMin = min(lumaM, min(min(lumaNW, lumaNE), min(lumaSW, lumaSE)));
    float lumaMax = max(lumaM, max(max(lumaNW, lumaNE), max(lumaSW, lumaSE)));
    
    vec2 dir;
    dir.x = -((lumaNW + lumaNE) - (lumaSW + lumaSE));
    dir.y =  ((lumaNW + lumaSW) - (lumaNE + lumaSE));
    
    float dirReduce = max((lumaNW + lumaNE + lumaSW + lumaSE) *
                          (0.25 * FXAA_REDUCE_MUL), FXAA_REDUCE_MIN);
    
    float rcpDirMin = 1.0 / (min(abs(dir.x), abs(dir.y)) + dirReduce);
    dir = min(vec2(FXAA_SPAN_MAX, FXAA_SPAN_MAX),
              max(vec2(-FXAA_SPAN_MAX, -FXAA_SPAN_MAX),
              dir * rcpDirMin)) * inverseVP;
    
    vec3 rgbA = 0.5 * (
        textureLod(tex, fragCoord * inverseVP + dir * (1.0 / 3.0 - 0.5), 0.0).xyz +
        textureLod(tex, fragCoord * inverseVP + dir * (2.0 / 3.0 - 0.5), 0.0).xyz);
    vec3 rgbB = rgbA * 0.5 + 0.25 * (
        textureLod(tex, fragCoord * inverseVP + dir * -0.5, 0.0).xyz +
        textureLod(tex, fragCoord * inverseVP + dir * 0.5, 0.0).xyz);

    float lumaB = dot(rgbB, luma);
    if ((lumaB < lumaMin) || (lumaB > lumaMax))
        color = vec4(rgbA, texColor.a);
    else
        color = vec4(rgbB, texColor.a);
    return color;
}

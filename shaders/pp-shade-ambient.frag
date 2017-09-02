#version 450
#extension GL_ARB_separate_shader_objects : enable

layout(location = 0) out vec4 outColor;

#include postProcessingInputs.glsl

#include camera.glsl

//#########//

float fresnelf(vec3 direction, vec3 normal) {
    vec3 nDirection = normalize( direction );
    vec3 nNormal = normalize( normal );
    vec3 halfDirection = normalize( nNormal + nDirection );

    float cosine = dot( halfDirection, nDirection );
    float product = max( cosine, 0.0 );
    float factor = pow( product, 5.0 );

    return factor;
}

void main() {
    vec3 diffuse = texture(texDiffuse, UV).rgb;
    vec3 normal = normalize(texture(texNormal, UV).rgb);
    vec3 res = diffuse * (0.02 + 0.98 * max(0.0, dot(vec3(0.0, 1.0, 0.0), normal)));
    outColor = vec4(res, 1.0);
}

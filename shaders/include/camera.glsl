vec3 reconstructCameraSpaceDistance(vec2 uv, float dist){
    uv.y = 1.0 - uv.y;
    vec3 dir = normalize((FrustumConeLeftBottom + FrustumConeBottomLeftToBottomRight * uv.x
        + FrustumConeBottomLeftToTopLeft * uv.y));
    return dir * dist;
}

vec3 ToCameraSpace(vec3 position){
    return position + CameraPosition;
}

vec3 FromCameraSpace(vec3 position){
    return position - -CameraPosition;
}

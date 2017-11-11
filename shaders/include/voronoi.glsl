
#define UNIT 16.0

vec2 get_cell(vec2 p){
    float ix = floor(p.x/UNIT)*UNIT;
    float iy = floor(p.y/UNIT)*UNIT;
    float cx = cos(3.2543*ix*(iy+49.0)+2.8575*iy*(ix+16.0))*0.3;
    float cy = sin(2.1948*iy*(ix-91.0)+3.1947*ix*(iy-61.0))*0.3;
    return vec2(cx,cy)*UNIT+vec2(ix,iy);
}
vec2 get_cell3d(vec3 p){
    float ix = floor(p.x/UNIT)*UNIT;
    float iy = floor(p.y/UNIT)*UNIT;
    float cx = cos(3.2543*ix*(iy+49.0)+2.8575*iy*(ix+16.0)+p.z)*0.3;
    float cy = sin(2.1948*iy*(ix-91.0)+3.1947*ix*(iy-61.0)+p.z)*0.3;
    return vec2(cx,cy)*UNIT+vec2(ix,iy);
}

float voronoi(vec2 p){
    vec2 unit = vec2(UNIT);
    vec2 sp = floor(p/UNIT)*UNIT;
    vec2 oxo = vec2(1.0,0.0)*unit;
    vec2 ooy = vec2(0.0,1.0)*unit;
    vec2 oxy = vec2(1.0,1.0)*unit;
    float vor = 1000.0;
    vor = min(vor, length(p-get_cell(sp)));
    vor = min(vor, length(p-get_cell(sp+oxo)));
    vor = min(vor, length(p-get_cell(sp+ooy)));
    vor = min(vor, length(p-get_cell(sp+oxy)));
    return vor;
}

float voronoi3d(vec3 p){
    vec2 unit = vec2(UNIT);
    vec2 sp = floor(p/UNIT)*UNIT;
    vec2 oxo = vec2(1.0,0.0)*unit;
    vec2 ooy = vec2(0.0,1.0)*unit;
    vec2 oxy = vec2(1.0,1.0)*unit;
    float vor = 1000.0;
    vor = min(vor, length(p-get_cell3d(sp)));
    vor = min(vor, length(p-get_cell3d(sp+oxo)));
    vor = min(vor, length(p-get_cell3d(sp+ooy)));
    vor = min(vor, length(p-get_cell3d(sp+oxy)));
    return vor;
}

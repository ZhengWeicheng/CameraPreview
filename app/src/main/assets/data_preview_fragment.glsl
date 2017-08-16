#extension GL_OES_EGL_image_external : require
precision mediump float;
varying vec2 vTexCoord;
uniform sampler2D yTexture;
uniform sampler2D uvTexture;
uniform float distance;
uniform float currentFilter;

//0
const vec4 bkColor = vec4(0.5, 0.5, 0.5, 1.0);
const highp vec3 W = vec3(0.2125, 0.7154, 0.0721);
//1
const vec3 monoMultiplier = vec3(0.299, 0.587, 0.114);
const vec3 cyanFactor = vec3(0.8, 1.2, 1.2);

const vec3 sepiaToneFactor = vec3(1.2, 1.0, 0.8);

vec4 getBaseColor(in vec2 coord){
   float r,g,b,y,u,v;
   y = texture2D(yTexture, coord).r;
   vec4 uvColor = texture2D(uvTexture, coord);
   u = uvColor.a - 0.5;
   v = uvColor.r - 0.5;
   r = y + 1.13983*v;
   g = y - 0.39465*u - 0.58060*v;
   b = y + 2.03211*u;
   return vec4(r, g, b, 1.0);
}

vec4 getFilterColor(float filterId) {
    vec4 color;
    if (filterId == 1.f) {
        float luminance = dot(getBaseColor(vTexCoord).rgb, W);
        color = vec4(vec3(luminance), 0.0) + bkColor;
    } else if (filterId == 2.f) {
        float monoColor = dot(getBaseColor(vTexCoord).rgb, monoMultiplier);
        color = vec4(clamp(vec3(monoColor, monoColor, monoColor)*cyanFactor, 0.0, 1.0), 1.0);
    } else if (filterId == 3.f) {
        vec3 textureColor = getBaseColor(vTexCoord).rgb;
        color = vec4(1.0-textureColor.r,1.0-textureColor.g,1.0-textureColor.b,1.0);
    } else if (filterId == 4.f) {
        float monoColor = dot(getBaseColor(vTexCoord).rgb,monoMultiplier);
        color = vec4(clamp(vec3(monoColor, monoColor, monoColor)*sepiaToneFactor, 0.0, 1.0), 1.0);
    } else {
        color = getBaseColor(vTexCoord);
    }
    return color;
}

vec4 getColor() {
    vec4 color;
    vec2 tc = vTexCoord.xy;
    float dist = length(tc.y);
    if (dist > abs(distance)) {
        color = getFilterColor(distance >= 0.f ? currentFilter : currentFilter - 1.f);
    } else {
        color = getFilterColor(distance > 0.f ? currentFilter + 1.f : currentFilter);
    }
    return color;
}

void main() {
  vec4 color = getColor();
  gl_FragColor = color;
}
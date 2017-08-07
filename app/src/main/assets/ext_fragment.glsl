#extension GL_OES_EGL_image_external : require

precision mediump float;
varying vec2 vTextureCoord;
uniform samplerExternalOES texture;
const vec4 bkColor = vec4(0.5, 0.5, 0.5, 1.0);
const highp vec3 W = vec3(0.2125, 0.7154, 0.0721);
void main() {
    vec2 tc = vTextureCoord.xy;
    float dist = length(tc.y);
    if (dist > 0.5f) {
        gl_FragColor = texture2D(texture, vTextureCoord);
    } else {
        float luminance = dot(texture2D(texture, tc).rgb, W);
        gl_FragColor = vec4(vec3(luminance), 0.0) + bkColor;
    }

}


//const vec2 center = vec2(330.0, 185.0);
//const float radius = 165.0;
//const float angle = 0.8;
//void main()
//{
//     vec2 texSize = vec2(659, 369);
//     vec2 tc = vTextureCoord * texSize;
//     tc -= center;
//     float dist = length(tc);
//     if (dist < radius)
//      {
//        float percent = (radius - dist) / radius;
//        float theta = percent * percent * angle * 8.0;
//        float s = sin(theta);
//        float c = cos(theta);
//        tc = vec2(dot(tc, vec2(c, -s)), dot(tc, vec2(s, c)));
//      }
//      tc += center;
//      vec3 color = texture2D(texture, (tc) / texSize).rgb;
//      gl_FragColor = vec4(color, 1.0);
//}
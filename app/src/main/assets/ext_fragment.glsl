#extension GL_OES_EGL_image_external : require

precision mediump float;
varying vec2 vTextureCoord;
uniform samplerExternalOES texture;

const vec2 center = vec2(330.0, 185.0);
const float radius = 165.0;
const float angle = 0.8;
void main()
{
     vec2 texSize = vec2(659, 369);
     vec2 tc = vTextureCoord * texSize;
     tc -= center;
     float dist = length(tc);
     if (dist < radius)
      {
        float percent = (radius - dist) / radius;
        float theta = percent * percent * angle * 8.0;
        float s = sin(theta);
        float c = cos(theta);
        tc = vec2(dot(tc, vec2(c, -s)), dot(tc, vec2(s, c)));
      }
      tc += center;
      vec3 color = texture2D(texture, (tc) / texSize).rgb;
      gl_FragColor = vec4(color, 1.0);
}
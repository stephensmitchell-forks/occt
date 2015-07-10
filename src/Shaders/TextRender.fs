#extension GL_EXT_gpu_shader4 : enable

varying vec2 TexCoord;

uniform sampler2D ColorTexture;
uniform sampler2D DepthTexture;

uniform vec3 Pixel;

#define MAXFLOAT 1.0e15

#define ZERO vec3 (0.0, 0.0, 0.0)
#define ONES vec3 (1.0, 1.0, 1.0)

// =======================================================================
// function : getAlpha
// purpose  :
// =======================================================================
float getAlpha()
{
  return texture2D (ColorTexture, TexCoord.st).a;
}

#define EPSILON 2.0e-5

// =======================================================================
// function : main
// purpose  :
// =======================================================================
void main()
{
  vec2 aTexCoord = Pixel.xy / vec2 (textureSize2D (DepthTexture, 0));

  vec2 aTexCoord1 = gl_FragCoord.xy / vec2 (textureSize2D (DepthTexture, 0));

  vec4 aColor = vec4 (1.0);

  aColor.a *= getAlpha();

  if (aColor.a <= 0.285)
    discard;

  if (Pixel.z - EPSILON > texture2D (DepthTexture, aTexCoord).x)
    discard;

  gl_FragDepth = 0.0;

  gl_FragColor = vec4 (aColor.xyz, aColor.a);
}
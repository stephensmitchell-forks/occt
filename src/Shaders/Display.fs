//! Input image.
uniform sampler2D uInputTexture;

//! Ray tracing depth image.
uniform sampler2D uDepthTexture;

uniform int uBilateralEnabled;

//! Output pixel color.
out vec4 OutColor;

const float rI = 0.270 * 1.0f;  // The intensity radius (in pixels).
const float rL = 1.71 * 0.5f;   // The geometric radius (in pixels).
const int WindowSize = 8;       // The window size (in pixels).

float gaussian (float theL, float theR)
{
  return exp (-theL * theL / (2.0f * theR * theR));
}

vec4 bilateral()
{
  // Get the sizes
  int aWindow   = WindowSize / 2;
  vec4 anOutCol = vec4 (0.f, 0.f, 0.f, 0.f);
  vec4 aRefCol  = texelFetch (uInputTexture, ivec2 (gl_FragCoord.xy), 0);
  float aNorm   = 0.f;

  // Compute the kernel
  for (int i = -aWindow; i <= aWindow; i++)
  {
    for (int j = -aWindow; j <= aWindow; j++)
    {
      vec4 aCol = texelFetch (uInputTexture, ivec2 (gl_FragCoord.xy) + ivec2 (j, i), 0);
      float A = gaussian (distance (aCol, aRefCol), rI);
      float B = gaussian (length (vec2(j, i)), rL);
      anOutCol += aCol * A * B;
      aNorm += A * B;
    }
  }
  return anOutCol * (1.f / aNorm);
}

void main (void)
{
  vec4 aColor;

  if (bool (uBilateralEnabled))
  {
    aColor = bilateral();
  }
  else
  {
    aColor = texelFetch (uInputTexture, ivec2 (gl_FragCoord.xy), 0);
  }

  float aDepth = texelFetch (uDepthTexture, ivec2 (gl_FragCoord.xy), 0).r;
  gl_FragDepth = aDepth;

  // apply gamma correction (we use gamma = 2)
  OutColor = vec4 (sqrt (aColor.rgb), aColor.a);
}
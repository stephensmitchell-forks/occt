// Created on: 2014-12-18
// Created by: Ilya Sevrikov
// Copyright (c) 2013-2014 OPEN CASCADE SAS
//
// This file is part of Open CASCADE Technology software library.
//
// This library is free software; you can redistribute it and/or modify it under
// the terms of the GNU Lesser General Public License version 2.1 as published
// by the Free Software Foundation, with special exception defined in the file
// OCCT_LGPL_EXCEPTION.txt. Consult the file LICENSE_LGPL_21.txt included in OCCT
// distribution for complete text of the license and disclaimer of any warranty.
//
// Alternatively, this file may be used under the terms of Open CASCADE
// commercial license or contractual agreement.

//! Pixel coords.
varying vec2 vPixel;

// Viewing position.
uniform vec3 uOriginLT;
uniform vec3 uOriginLB;
uniform vec3 uOriginRT;
uniform vec3 uOriginRB;

// Viewing direction.
uniform vec3 uDirectLT;
uniform vec3 uDirectLB;
uniform vec3 uDirectRT;
uniform vec3 uDirectRB;

// 3D volume bounds.
uniform vec3 uBoxCornerMin;
uniform vec3 uBoxCornerMax;

// 3D volume and TF.
uniform sampler1D uTransferFunct;
uniform sampler3D uVolumeTexture;

// Rendering parameters.
uniform int   uNumOfSamples;
uniform int   uTriCubFilter;
uniform int   uApplyShading;
uniform int   uTraceShadows;
uniform int   uRandomShift;
uniform vec3  uVolTexelSize;
uniform float uOpacityScale;
uniform float uNormMinValue;
uniform float uNormMaxValue;
uniform vec4  uRandomSeed;

uniform int uIsoSurfaceMode;
uniform int uTraceSliceMode;

uniform float uIsoSurfaceLevel;
uniform float uTraceSliceLevel;

//! Stores ray parameters.
struct SRay
{
  vec3 Origin;
  vec3 Direct;
};

#define MAXFLOAT 1.0e15

#define ZERO vec3 (0.0, 0.0, 0.0)
#define ONES vec3 (1.0, 1.0, 1.0)

// =======================================================================
// function : GenerateRay
// purpose  :
// =======================================================================
SRay GenerateRay (in vec2 thePixel)
{
  vec3 aP0 = mix (uOriginLB, uOriginRB, thePixel.x);
  vec3 aP1 = mix (uOriginLT, uOriginRT, thePixel.x);

  vec3 aD0 = mix (uDirectLB, uDirectRB, thePixel.x);
  vec3 aD1 = mix (uDirectLT, uDirectRT, thePixel.x);

  vec3 aDirection = normalize (mix (aD0, aD1, thePixel.y));

  return SRay (mix (aP0, aP1, thePixel.y), aDirection);
}

// =======================================================================
// function : IntersectBox
// purpose  : Computes ray-box intersection
// =======================================================================
vec2 IntersectBox (in SRay theRay, in vec3 theMinPnt, in vec3 theMaxPnt)
{
  vec3 aTimeBoxMin = (theMinPnt - theRay.Origin) * (1.0 / theRay.Direct);
  vec3 aTimeBoxMax = (theMaxPnt - theRay.Origin) * (1.0 / theRay.Direct);

  vec3 aTimeMax = max (aTimeBoxMin, aTimeBoxMax);
  vec3 aTimeMin = min (aTimeBoxMin, aTimeBoxMax);

  float aTime1 = max (aTimeMin.x, max (aTimeMin.y, aTimeMin.z));
  float aTime2 = min (aTimeMax.x, min (aTimeMax.y, aTimeMax.z));

  return aTime1 > aTime2 || aTime2 < 0.0 ?
    vec2 (MAXFLOAT) : vec2 (max (aTime1, 0.0), aTime2);
}

//=======================================================================
// function : InterpolateCubic
// purpose  : Based on 2D cubic interpolation from the paper:
//            GPU Gems 2. Fast Third-Order Texture Filtering (Chapter 20)
//=======================================================================
float InterpolateCubic (in vec3 theCoord, in vec3 theCellSize)
{
  vec3 aCoordGrid = theCoord / theCellSize - vec3 (0.5);

  vec3 aIndex = floor (aCoordGrid);

  vec3 aFract = aCoordGrid - aIndex;
  vec3 aCompl = vec3 (1.0) - aFract;

  vec3 aCompl2 = aCompl * aCompl;
  vec3 aFract2 = aFract * aFract;

  vec3 aW0 = 1.0 / 6.0 * aCompl2 * aCompl;
  vec3 aW1 = vec3 (2.0 / 3.0) - 0.5 * aFract2 * (2.0 - aFract);
  vec3 aW2 = vec3 (2.0 / 3.0) - 0.5 * aCompl2 * (2.0 - aCompl);
  vec3 aW3 = 1.0 / 6.0 * aFract2 * aFract;

  vec3 aG0 = aW0 + aW1;
  vec3 aG1 = aW2 + aW3;

  vec3 aH0 = (aW1 / aG0) - vec3 (0.5) + aIndex;
  vec3 aH1 = (aW3 / aG1) + vec3 (1.5) + aIndex;

  aH0 *= theCellSize;
  aH1 *= theCellSize;

  // fetch the eight linear interpolations
  float aTex000 = texture3D (uVolumeTexture, aH0).x;
  float aTex100 = texture3D (uVolumeTexture, vec3 (aH1.x, aH0.y, aH0.z)).x;
  float aTex010 = texture3D (uVolumeTexture, vec3 (aH0.x, aH1.y, aH0.z)).x;
  float aTex110 = texture3D (uVolumeTexture, vec3 (aH1.x, aH1.y, aH0.z)).x;
  float aTex001 = texture3D (uVolumeTexture, vec3 (aH0.x, aH0.y, aH1.z)).x;
  float aTex101 = texture3D (uVolumeTexture, vec3 (aH1.x, aH0.y, aH1.z)).x;
  float aTex011 = texture3D (uVolumeTexture, vec3 (aH0.x, aH1.y, aH1.z)).x;
  float aTex111 = texture3D (uVolumeTexture, aH1).x;

  // weigh along the z-direction
  aTex000 = mix (aTex001, aTex000, aG0.z);
  aTex100 = mix (aTex101, aTex100, aG0.z);
  aTex010 = mix (aTex011, aTex010, aG0.z);
  aTex110 = mix (aTex111, aTex110, aG0.z);

  // weigh along the y-direction
  aTex000 = mix (aTex010, aTex000, aG0.y);
  aTex100 = mix (aTex110, aTex100, aG0.y);

  // weigh along the x-direction
  return mix (aTex100, aTex000, aG0.x);
}

//=======================================================================
// function : Field
// purpose  :
//=======================================================================
float Field (in vec3 thePoint)
{
  vec3 aTexCoord = (thePoint - uBoxCornerMin) / (uBoxCornerMax - uBoxCornerMin);

  if (uTriCubFilter == 0)
  {
    return texture3D (uVolumeTexture, aTexCoord).x;
  }

  return InterpolateCubic (aTexCoord, uVolTexelSize);
}

//=======================================================================
// function : TransferFunction
// purpose  :
//=======================================================================
vec4 TransferFunction (in float theValue)
{
  return texture1D (uTransferFunct, (theValue - uNormMinValue) / (uNormMaxValue - uNormMinValue));
}

//=======================================================================
// function : FieldNormal
// purpose  :
//=======================================================================
vec3 FieldNormal (in vec3 thePoint, in vec3 theView, in float theValue)
{
  vec3 aTexCoord = (thePoint - uBoxCornerMin) / (uBoxCornerMax - uBoxCornerMin);

  float aPosX = texture3D (uVolumeTexture,
    vec3 (aTexCoord.x + uVolTexelSize.x, aTexCoord.y, aTexCoord.z)).x;
  float aPosY = texture3D (uVolumeTexture,
    vec3 (aTexCoord.x, aTexCoord.y + uVolTexelSize.y, aTexCoord.z)).x;
  float aPosZ = texture3D (uVolumeTexture,
    vec3 (aTexCoord.x, aTexCoord.y, aTexCoord.z + uVolTexelSize.z)).x;

  vec3 aNormal = vec3 (aPosX - theValue,
                       aPosY - theValue,
                       aPosZ - theValue) / uVolTexelSize;

  float aLength = dot (aNormal, aNormal);

  return aLength > 0.0 ? aNormal * inversesqrt (aLength) : theView;
}

//=======================================================================
// function : Rand
// purpose  : 
//=======================================================================
float Rand (in vec4 theVec)
{
  return fract(sin(dot(theVec, uRandomSeed)) * 1000.0);
}

#define MIN_SAMPLES 25
#define MAX_OPACITY 0.99

//=======================================================================
// function : RaytraceVolume
// purpose  :
//=======================================================================
vec4 RaytraceVolume (in SRay theRay, in vec2 theRange)
{
  float aFraction = theRange.y /
    length (uBoxCornerMax - uBoxCornerMin);

  int aNbIterations = max (int (uNumOfSamples * aFraction), MIN_SAMPLES);

  float aOpacity = uOpacityScale * aFraction / aNbIterations;

  vec3 aLight = theRay.Direct;

  theRay.Origin += theRange.x * theRay.Direct;
  theRay.Direct *= theRange.y / aNbIterations;

  vec4 aResult = vec4 (0.0); // result color

  if (uRandomShift != 0)
  {
    theRay.Origin += theRay.Direct * (Rand (vec4 (gl_FragCoord.xy, theRay.Origin.xy)));
  }

  for (int aStep = 0; aStep < aNbIterations && aResult.w < MAX_OPACITY; ++aStep)
  {
    float aField = Field (theRay.Origin);

    vec4 aColor = TransferFunction (aField);

    if (uApplyShading != 0)
    {
      vec3 aNormal = FieldNormal (theRay.Origin, -theRay.Direct, aField);

      aColor.xyz *= max (abs (dot (aNormal, aLight)), 0.2);
    }

    float anAlpha = (1.0 - aResult.w) * (aColor.w * aOpacity);

    aResult = vec4 (aResult.x + anAlpha * aColor.x,
                    aResult.y + anAlpha * aColor.y,
                    aResult.z + anAlpha * aColor.z,
                    aResult.w + anAlpha);

    theRay.Origin += theRay.Direct;
  }

  return aResult;
}

//=======================================================================
// function : illumination
// purpose  :
//=======================================================================
vec3 illumination (in vec3 point, in vec3 view, in vec3 normal, in vec3 color)
{
  float diffuse = (1.0 + dot (view, normal)) / 2.0;
  
  float specular = pow (max (0.0, dot (view, normal)), 16.0);
  
  return (0.5 * color * (1.0 - diffuse) + color * diffuse) +
          0.1 * specular * vec3 (1.0);
}

//=======================================================================
// function : RaytraceIsosurface
// purpose  :
//=======================================================================
vec4 RaytraceIsosurface (in SRay theRay, in vec2 theRange)
{
  vec4 result = vec4 (0.0);

  float fraction = theRange.y /
    length (uBoxCornerMax - uBoxCornerMin);

  int intervals = max (int (uNumOfSamples * fraction), MIN_SAMPLES);

  float step = theRange.y / intervals;

  vec3 origin = theRay.Origin + theRange.x * theRay.Direct;

  float curr = Field (origin);

  float level = uNormMinValue + (uNormMaxValue - uNormMinValue) * uIsoSurfaceLevel;

  for (int s = 0; s < intervals; ++s)
  {
    origin += step * theRay.Direct;

    float next = Field (origin);

    if ((curr - level) * (next - level) < 0.0)
    {
      float time = step * (next - level) / (curr - next);

      vec3 point = origin + time * theRay.Direct;

      float aValue = Field (point);

      vec4 color = TransferFunction (aValue);

      vec3 normal = FieldNormal (point, -theRay.Direct, aValue);

      vec3 shaded = illumination (point, -theRay.Direct, normal, color.xyz);

      color = vec4 (shaded.x,
                    shaded.y,
                    shaded.z,
                    1.0);

      return color;
    }

    curr = next;
  }

  return result;
}

//=======================================================================
// function : RaytraceSlice
// purpose  :
//=======================================================================
vec4 RaytraceSlice (in SRay theRay)
{
  float aField = Field (theRay.Origin);

  vec4 aColor = TransferFunction (aField);
  aColor.w *= 2.0;

  bool aBorder = vPixel.x < 0.009 || vPixel.y < 0.009 || vPixel.x > 0.99 || vPixel.y > 0.99;

  return mix (aColor, vec4 (0.75, 0.75, 0.75, 1.0), float (aBorder));
}

// =======================================================================
// function : main
// purpose  :
// =======================================================================
void main()
{
  if (uTraceSliceMode == 1)
  {
    SRay aRay = SRay (uBoxCornerMin +
      (uBoxCornerMax - uBoxCornerMin) * vec3 (vPixel.xy, uTraceSliceLevel), vec3 (0.0, 0.0, -1.0));

    gl_FragColor = RaytraceSlice (aRay);
    return;
  }

  SRay aRay = GenerateRay (vPixel);

  vec3 aBoxMax = uBoxCornerMax;

  if (uTraceSliceMode == 2)
  {
    aBoxMax.z = uBoxCornerMin.z + (uBoxCornerMax.z - uBoxCornerMin.z) * uTraceSliceLevel;
  }

  vec2 aRange = IntersectBox (aRay, uBoxCornerMin, aBoxMax);

  vec4 aColor = vec4 (0.0);

  if (aRange.x == MAXFLOAT)
  {
    discard;
  }

  aRange.y -= aRange.x;

  if (uIsoSurfaceMode == 0)
  {
    aColor = RaytraceVolume (aRay, aRange);
  }
  else
  {
    aColor = RaytraceIsosurface (aRay, aRange);
  }

  gl_FragColor = aColor;
}
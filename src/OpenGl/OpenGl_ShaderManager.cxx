// Created on: 2013-09-26
// Created by: Denis BOGOLEPOV
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

#include <typeinfo>

#include <Graphic3d_TextureParams.hxx>
#include <OpenGl_AspectFace.hxx>
#include <OpenGl_AspectLine.hxx>
#include <OpenGl_AspectMarker.hxx>
#include <OpenGl_AspectText.hxx>
#include <OpenGl_Clipping.hxx>
#include <OpenGl_Context.hxx>
#include <OpenGl_ShaderManager.hxx>
#include <OpenGl_ShaderProgram.hxx>
#include <OpenGl_Workspace.hxx>

#include <TCollection_ExtendedString.hxx>

IMPLEMENT_STANDARD_RTTIEXT(OpenGl_ShaderManager,Standard_Transient)

namespace
{

  //! Clipping planes limit (see the same definition in Declarations.glsl).
  static const Standard_Size THE_MAX_CLIP_PLANES = 8;

#define EOL "\n"

//! Definition of TexCoord varying.
const char THE_VARY_TexCoord_OUT[] =
  EOL"THE_SHADER_OUT vec4 TexCoord;";
const char THE_VARY_TexCoord_IN[] =
  EOL"THE_SHADER_IN  vec4 TexCoord;";
//! Compute TexCoord value in Vertex Shader
const char THE_VARY_TexCoord_Trsf[] =
  EOL"  float aRotSin = occTextureTrsf_RotationSin();"
  EOL"  float aRotCos = occTextureTrsf_RotationCos();"
  EOL"  vec2  aTex2   = (occTexCoord.xy + occTextureTrsf_Translation()) * occTextureTrsf_Scale();"
  EOL"  vec2  aCopy   = aTex2;"
  EOL"  aTex2.x = aCopy.x * aRotCos - aCopy.y * aRotSin;"
  EOL"  aTex2.y = aCopy.x * aRotSin + aCopy.y * aRotCos;"
  EOL"  TexCoord = vec4(aTex2, occTexCoord.zw);";

//! Auxiliary function to flip gl_PointCoord vertically
#define THE_VEC2_glPointCoord "vec2 (gl_PointCoord.x, 1.0 - gl_PointCoord.y)"

//! Auxiliary function to transform normal
const char THE_FUNC_transformNormal[] =
  EOL"vec3 transformNormal (in vec3 theNormal)"
  EOL"{"
  EOL"  vec4 aResult = occWorldViewMatrixInverseTranspose"
  EOL"               * occModelWorldMatrixInverseTranspose"
  EOL"               * vec4 (theNormal, 0.0);"
  EOL"  return normalize (aResult.xyz);"
  EOL"}";

//! Global shader variable for color definition with lighting enabled.
const char THE_FUNC_lightDef[] =
  EOL"vec3 Ambient;"   //!< Ambient  contribution of light sources
  EOL"vec3 Diffuse;"   //!< Diffuse  contribution of light sources
  EOL"vec3 Specular;"; //!< Specular contribution of light sources

//! Function computes contribution of isotropic point light source
const char THE_FUNC_pointLight[] =
  EOL"void pointLight (in int  theId,"
  EOL"                 in vec3 theNormal,"
  EOL"                 in vec3 theView,"
  EOL"                 in vec3 thePoint,"
  EOL"                 in bool theIsFront)"
  EOL"{"
  EOL"  vec3 aLight = occLight_Position (theId).xyz;"
  EOL"  if (occLight_IsHeadlight (theId) == 0)"
  EOL"  {"
  EOL"    aLight = vec3 (occWorldViewMatrix * occModelWorldMatrix * vec4 (aLight, 1.0));"
  EOL"  }"
  EOL"  aLight -= thePoint;"
  EOL
  EOL"  float aDist = length (aLight);"
  EOL"  aLight = aLight * (1.0 / aDist);"
  EOL
  EOL"  float anAtten = 1.0 / (occLight_ConstAttenuation  (theId)"
  EOL"                       + occLight_LinearAttenuation (theId) * aDist);"
  EOL
  EOL"  vec3 aHalf = normalize (aLight + theView);"
  EOL
  EOL"  vec3  aFaceSideNormal = theIsFront ? theNormal : -theNormal;"
  EOL"  float aNdotL = max (0.0, dot (aFaceSideNormal, aLight));"
  EOL"  float aNdotH = max (0.0, dot (aFaceSideNormal, aHalf ));"
  EOL
  EOL"  float aSpecl = 0.0;"
  EOL"  if (aNdotL > 0.0)"
  EOL"  {"
  EOL"    aSpecl = pow (aNdotH, theIsFront ? occFrontMaterial_Shininess() : occBackMaterial_Shininess());"
  EOL"  }"
  EOL
  EOL"Diffuse  += occLight_Diffuse  (theId).rgb * aNdotL * anAtten;"
  EOL"Specular += occLight_Specular (theId).rgb * aSpecl * anAtten;"
  EOL"}";

//! Function computes contribution of spotlight source
const char THE_FUNC_spotLight[] =
  EOL"void spotLight (in int  theId,"
  EOL"                in vec3 theNormal,"
  EOL"                in vec3 theView,"
  EOL"                in vec3 thePoint,"
  EOL"                in bool theIsFront)"
  EOL"{"
  EOL"  vec3 aLight   = occLight_Position      (theId).xyz;"
  EOL"  vec3 aSpotDir = occLight_SpotDirection (theId).xyz;"
  EOL"  if (occLight_IsHeadlight (theId) == 0)"
  EOL"  {"
  EOL"    aLight   = vec3 (occWorldViewMatrix * occModelWorldMatrix * vec4 (aLight,   1.0));"
  EOL"    aSpotDir = vec3 (occWorldViewMatrix * occModelWorldMatrix * vec4 (aSpotDir, 0.0));"
  EOL"  }"
  EOL"  aLight -= thePoint;"
  EOL
  EOL"  float aDist = length (aLight);"
  EOL"  aLight = aLight * (1.0 / aDist);"
  EOL
  EOL"  aSpotDir = normalize (aSpotDir);"
  // light cone
  EOL"  float aCosA = dot (aSpotDir, -aLight);"
  EOL"  if (aCosA >= 1.0 || aCosA < cos (occLight_SpotCutOff (theId)))"
  EOL"  {"
  EOL"    return;"
  EOL"  }"
  EOL
  EOL"  float anExponent = occLight_SpotExponent (theId);"
  EOL"  float anAtten    = 1.0 / (occLight_ConstAttenuation  (theId)"
  EOL"                          + occLight_LinearAttenuation (theId) * aDist);"
  EOL"  if (anExponent > 0.0)"
  EOL"  {"
  EOL"    anAtten *= pow (aCosA, anExponent * 128.0);"
  EOL"  }"
  EOL
  EOL"  vec3 aHalf = normalize (aLight + theView);"
  EOL
  EOL"  vec3  aFaceSideNormal = theIsFront ? theNormal : -theNormal;"
  EOL"  float aNdotL = max (0.0, dot (aFaceSideNormal, aLight));"
  EOL"  float aNdotH = max (0.0, dot (aFaceSideNormal, aHalf ));"
  EOL
  EOL"  float aSpecl = 0.0;"
  EOL"  if (aNdotL > 0.0)"
  EOL"  {"
  EOL"    aSpecl = pow (aNdotH, theIsFront ? occFrontMaterial_Shininess() : occBackMaterial_Shininess());"
  EOL"  }"
  EOL
  EOL"  Diffuse  += occLight_Diffuse  (theId).rgb * aNdotL * anAtten;"
  EOL"  Specular += occLight_Specular (theId).rgb * aSpecl * anAtten;"
  EOL"}";

//! Function computes contribution of directional light source
const char THE_FUNC_directionalLight[] =
  EOL"void directionalLight (in int  theId,"
  EOL"                       in vec3 theNormal,"
  EOL"                       in vec3 theView,"
  EOL"                       in bool theIsFront)"
  EOL"{"
  EOL"  vec3 aLight = normalize (occLight_Position (theId).xyz);"
  EOL"  if (occLight_IsHeadlight (theId) == 0)"
  EOL"  {"
  EOL"    aLight = vec3 (occWorldViewMatrix * occModelWorldMatrix * vec4 (aLight, 0.0));"
  EOL"  }"
  EOL
  EOL"  vec3 aHalf = normalize (aLight + theView);"
  EOL
  EOL"  vec3  aFaceSideNormal = theIsFront ? theNormal : -theNormal;"
  EOL"  float aNdotL = max (0.0, dot (aFaceSideNormal, aLight));"
  EOL"  float aNdotH = max (0.0, dot (aFaceSideNormal, aHalf ));"
  EOL
  EOL"  float aSpecl = 0.0;"
  EOL"  if (aNdotL > 0.0)"
  EOL"  {"
  EOL"    aSpecl = pow (aNdotH, theIsFront ? occFrontMaterial_Shininess() : occBackMaterial_Shininess());"
  EOL"  }"
  EOL
  EOL"  Diffuse  += occLight_Diffuse  (theId).rgb * aNdotL;"
  EOL"  Specular += occLight_Specular (theId).rgb * aSpecl;"
  EOL"}";

//! The same as THE_FUNC_directionalLight but for the light with zero index
//! (avoids limitations on some mobile devices).
const char THE_FUNC_directionalLightFirst[] =
  EOL"void directionalLightFirst (in vec3 theNormal,"
  EOL"                            in vec3 theView,"
  EOL"                            in bool theIsFront)"
  EOL"{"
  EOL"  vec3 aLight = normalize (occLightSources[1].xyz);"
  EOL"  if (occLight_IsHeadlight (0) == 0)"
  EOL"  {"
  EOL"    aLight = vec3 (occWorldViewMatrix * occModelWorldMatrix * vec4 (aLight, 0.0));"
  EOL"  }"
  EOL
  EOL"  vec3 aHalf = normalize (aLight + theView);"
  EOL
  EOL"  vec3  aFaceSideNormal = theIsFront ? theNormal : -theNormal;"
  EOL"  float aNdotL = max (0.0, dot (aFaceSideNormal, aLight));"
  EOL"  float aNdotH = max (0.0, dot (aFaceSideNormal, aHalf ));"
  EOL
  EOL"  float aSpecl = 0.0;"
  EOL"  if (aNdotL > 0.0)"
  EOL"  {"
  EOL"    aSpecl = pow (aNdotH, theIsFront ? occFrontMaterial_Shininess() : occBackMaterial_Shininess());"
  EOL"  }"
  EOL
  EOL"  Diffuse  += occLightSources[0].rgb * aNdotL;"
  EOL"  Specular += occLightSources[0].rgb * aSpecl;"
  EOL"}";

//! Process clipping planes in Fragment Shader.
//! Should be added at the beginning of the main() function.
const char THE_FRAG_CLIP_PLANES_N[] =
  EOL"  for (int aPlaneIter = 0; aPlaneIter < occClipPlaneCount; ++aPlaneIter)"
  EOL"  {"
  EOL"    vec4 aClipEquation = occClipPlaneEquations[aPlaneIter];"
  EOL"    if (dot (aClipEquation.xyz, PositionWorld.xyz / PositionWorld.w) + aClipEquation.w < 0.0)"
  EOL"    {"
  EOL"      discard;"
  EOL"    }"
  EOL"  }";

//! Process 1 clipping plane in Fragment Shader.
const char THE_FRAG_CLIP_PLANES_1[] =
  EOL"  vec4 aClipEquation0 = occClipPlaneEquations[0];"
  EOL"  if (dot (aClipEquation0.xyz, PositionWorld.xyz / PositionWorld.w) + aClipEquation0.w < 0.0)"
  EOL"  {"
  EOL"    discard;"
  EOL"  }";

//! Process 2 clipping planes in Fragment Shader.
const char THE_FRAG_CLIP_PLANES_2[] =
  EOL"  vec4 aClipEquation0 = occClipPlaneEquations[0];"
  EOL"  vec4 aClipEquation1 = occClipPlaneEquations[1];"
  EOL"  if (dot (aClipEquation0.xyz, PositionWorld.xyz / PositionWorld.w) + aClipEquation0.w < 0.0"
  EOL"   || dot (aClipEquation1.xyz, PositionWorld.xyz / PositionWorld.w) + aClipEquation1.w < 0.0)"
  EOL"  {"
  EOL"    discard;"
  EOL"  }";

}

// =======================================================================
// function : OpenGl_ShaderManager
// purpose  : Creates new empty shader manager
// =======================================================================
OpenGl_ShaderManager::OpenGl_ShaderManager (OpenGl_Context* theContext)
: myShadingModel (Graphic3d_TOSM_VERTEX),
  myContext  (theContext),
  myLastView (NULL)
{
  //
}

// =======================================================================
// function : ~OpenGl_ShaderManager
// purpose  : Releases resources of shader manager
// =======================================================================
OpenGl_ShaderManager::~OpenGl_ShaderManager()
{
  myProgramList.Clear();
}

// =======================================================================
// function : clear
// purpose  :
// =======================================================================
void OpenGl_ShaderManager::clear()
{
  myProgramList.Clear();
  myLightPrograms.Nullify();
  myFlatPrograms = OpenGl_SetOfShaderPrograms();
  myMapOfLightPrograms.Clear();
  myFontProgram.Nullify();
  myBlitProgram.Nullify();
  for (Standard_Integer aModeIter = 0; aModeIter < Graphic3d_StereoMode_NB; ++aModeIter)
  {
    myStereoPrograms[aModeIter].Nullify();
  }
  switchLightPrograms();
}

// =======================================================================
// function : Create
// purpose  : Creates new shader program
// =======================================================================
Standard_Boolean OpenGl_ShaderManager::Create (const Handle(Graphic3d_ShaderProgram)& theProxy,
                                               TCollection_AsciiString&               theShareKey,
                                               Handle(OpenGl_ShaderProgram)&          theProgram)
{
  theProgram.Nullify();
  if (theProxy.IsNull())
  {
    return Standard_False;
  }

  theShareKey = theProxy->GetId();
  if (myContext->GetResource<Handle(OpenGl_ShaderProgram)> (theShareKey, theProgram))
  {
    if (theProgram->Share())
    {
      myProgramList.Append (theProgram);
    }
    return Standard_True;
  }

  theProgram = new OpenGl_ShaderProgram (theProxy);
  if (!theProgram->Initialize (myContext, theProxy->ShaderObjects()))
  {
    theProgram->Release (myContext);
    theShareKey.Clear();
    theProgram.Nullify();
    return Standard_False;
  }

  myProgramList.Append (theProgram);
  myContext->ShareResource (theShareKey, theProgram);
  return Standard_True;
}

// =======================================================================
// function : Unregister
// purpose  : Removes specified shader program from the manager
// =======================================================================
void OpenGl_ShaderManager::Unregister (TCollection_AsciiString&      theShareKey,
                                       Handle(OpenGl_ShaderProgram)& theProgram)
{
  for (OpenGl_ShaderProgramList::Iterator anIt (myProgramList); anIt.More(); anIt.Next())
  {
    if (anIt.Value() == theProgram)
    {
      if (!theProgram->UnShare())
      {
        theShareKey.Clear();
        theProgram.Nullify();
        return;
      }

      myProgramList.Remove (anIt);
      break;
    }
  }

  const TCollection_AsciiString anID = theProgram->myProxy->GetId();
  if (anID.IsEmpty())
  {
    myContext->DelayedRelease (theProgram);
    theProgram.Nullify();
  }
  else
  {
    theProgram.Nullify();
    myContext->ReleaseResource (anID, Standard_True);
  }
}

// =======================================================================
// function : ShaderPrograms
// purpose  : Returns list of registered shader programs
// =======================================================================
const OpenGl_ShaderProgramList& OpenGl_ShaderManager::ShaderPrograms() const
{
  return myProgramList;
}

// =======================================================================
// function : Empty
// purpose  : Returns true if no program objects are attached
// =======================================================================
Standard_Boolean OpenGl_ShaderManager::IsEmpty() const
{
  return myProgramList.IsEmpty();
}

// =======================================================================
// function : switchLightPrograms
// purpose  :
// =======================================================================
void OpenGl_ShaderManager::switchLightPrograms()
{
  TCollection_AsciiString aKey (myShadingModel == Graphic3d_TOSM_FRAGMENT ? "p_" : "g_");
  const OpenGl_ListOfLight* aLights = myLightSourceState.LightSources();
  if (aLights != NULL)
  {
    for (OpenGl_ListOfLight::Iterator aLightIter (*aLights); aLightIter.More(); aLightIter.Next())
    {
      switch (aLightIter.Value().Type)
      {
        case Graphic3d_TOLS_AMBIENT:
          break; // skip ambient
        case Graphic3d_TOLS_DIRECTIONAL:
          aKey += "d";
          break;
        case Graphic3d_TOLS_POSITIONAL:
          aKey += "p";
          break;
        case Graphic3d_TOLS_SPOT:
          aKey += "s";
          break;
      }
    }
  }

  if (!myMapOfLightPrograms.Find (aKey, myLightPrograms))
  {
    myLightPrograms = new OpenGl_SetOfShaderPrograms();
    myMapOfLightPrograms.Bind (aKey, myLightPrograms);
  }
}

// =======================================================================
// function : UpdateLightSourceStateTo
// purpose  : Updates state of OCCT light sources
// =======================================================================
void OpenGl_ShaderManager::UpdateLightSourceStateTo (const OpenGl_ListOfLight* theLights)
{
  myLightSourceState.Set (theLights);
  myLightSourceState.Update();
  switchLightPrograms();
}

// =======================================================================
// function : SetShadingModel
// purpose  :
// =======================================================================
void OpenGl_ShaderManager::SetShadingModel (const Graphic3d_TypeOfShadingModel theModel)
{
  myShadingModel = theModel;
  switchLightPrograms();
}

// =======================================================================
// function : SetProjectionState
// purpose  : Sets new state of OCCT projection transform
// =======================================================================
void OpenGl_ShaderManager::UpdateProjectionStateTo (const OpenGl_Mat4& theProjectionMatrix)
{
  myProjectionState.Set (theProjectionMatrix);
  myProjectionState.Update();
}

// =======================================================================
// function : SetModelWorldState
// purpose  : Sets new state of OCCT model-world transform
// =======================================================================
void OpenGl_ShaderManager::UpdateModelWorldStateTo (const OpenGl_Mat4& theModelWorldMatrix)
{
  myModelWorldState.Set (theModelWorldMatrix);
  myModelWorldState.Update();
}

// =======================================================================
// function : SetWorldViewState
// purpose  : Sets new state of OCCT world-view transform
// =======================================================================
void OpenGl_ShaderManager::UpdateWorldViewStateTo (const OpenGl_Mat4& theWorldViewMatrix)
{
  myWorldViewState.Set (theWorldViewMatrix);
  myWorldViewState.Update();
}

// =======================================================================
// function : LightSourceState
// purpose  : Returns current state of OCCT light sources
// =======================================================================
const OpenGl_LightSourceState& OpenGl_ShaderManager::LightSourceState() const
{
  return myLightSourceState;
}

// =======================================================================
// function : ProjectionState
// purpose  : Returns current state of OCCT projection transform
// =======================================================================
const OpenGl_ProjectionState& OpenGl_ShaderManager::ProjectionState() const
{
  return myProjectionState;
}

// =======================================================================
// function : ModelWorldState
// purpose  : Returns current state of OCCT model-world transform
// =======================================================================
const OpenGl_ModelWorldState& OpenGl_ShaderManager::ModelWorldState() const
{
  return myModelWorldState;
}

// =======================================================================
// function : WorldViewState
// purpose  : Returns current state of OCCT world-view transform
// =======================================================================
const OpenGl_WorldViewState& OpenGl_ShaderManager::WorldViewState() const
{
  return myWorldViewState;
}

//! Packed properties of light source
class OpenGl_ShaderLightParameters
{
public:

  OpenGl_Vec4 Color;
  OpenGl_Vec4 Position;
  OpenGl_Vec4 Direction;
  OpenGl_Vec4 Parameters;

  //! Returns packed (serialized) representation of light source properties
  const OpenGl_Vec4* Packed() const { return reinterpret_cast<const OpenGl_Vec4*> (this); }
  static Standard_Integer NbOfVec4() { return 4; }

};

//! Packed light source type information
class OpenGl_ShaderLightType
{
public:

  Standard_Integer Type;
  Standard_Integer IsHeadlight;

  //! Returns packed (serialized) representation of light source type
  const OpenGl_Vec2i* Packed() const { return reinterpret_cast<const OpenGl_Vec2i*> (this); }
  static Standard_Integer NbOfVec2i() { return 1; }

};

// =======================================================================
// function : PushLightSourceState
// purpose  : Pushes state of OCCT light sources to the program
// =======================================================================
void OpenGl_ShaderManager::PushLightSourceState (const Handle(OpenGl_ShaderProgram)& theProgram) const
{
  if (myLightSourceState.Index() == theProgram->ActiveState (OpenGl_LIGHT_SOURCES_STATE)
   || !theProgram->IsValid())
  {
    return;
  }

  OpenGl_ShaderLightType* aLightTypeArray = new OpenGl_ShaderLightType[OpenGLMaxLights];
  for (Standard_Integer aLightIt = 0; aLightIt < OpenGLMaxLights; ++aLightIt)
  {
    aLightTypeArray[aLightIt].Type = -1;
  }

  const Standard_Integer aLightsDefNb = Min (myLightSourceState.LightSources()->Size(), OpenGLMaxLights);
  if (aLightsDefNb < 1)
  {
    theProgram->SetUniform (myContext,
                            theProgram->GetStateLocation (OpenGl_OCC_LIGHT_SOURCE_COUNT),
                            0);
    theProgram->SetUniform (myContext,
                            theProgram->GetStateLocation (OpenGl_OCC_LIGHT_AMBIENT),
                            OpenGl_Vec4 (0.0f, 0.0f, 0.0f, 0.0f));
    theProgram->SetUniform (myContext,
                            theProgram->GetStateLocation (OpenGl_OCC_LIGHT_SOURCE_TYPES),
                            OpenGLMaxLights * OpenGl_ShaderLightType::NbOfVec2i(),
                            aLightTypeArray[0].Packed());
    theProgram->UpdateState (OpenGl_LIGHT_SOURCES_STATE, myLightSourceState.Index());
    delete[] aLightTypeArray;
    return;
  }

  OpenGl_ShaderLightParameters* aLightParamsArray = new OpenGl_ShaderLightParameters[aLightsDefNb];

  OpenGl_Vec4 anAmbient (0.0f, 0.0f, 0.0f, 0.0f);
  Standard_Integer aLightsNb = 0;
  for (OpenGl_ListOfLight::Iterator anIter (*myLightSourceState.LightSources()); anIter.More(); anIter.Next())
  {
    const OpenGl_Light& aLight = anIter.Value();
    if (aLight.Type == Graphic3d_TOLS_AMBIENT)
    {
      anAmbient += aLight.Color;
      continue;
    }
    else if (aLightsNb >= OpenGLMaxLights)
    {
      continue;
    }

    OpenGl_ShaderLightType& aLightType = aLightTypeArray[aLightsNb];
    aLightType.Type        = aLight.Type;
    aLightType.IsHeadlight = aLight.IsHeadlight;

    OpenGl_ShaderLightParameters& aLightParams = aLightParamsArray[aLightsNb];
    aLightParams.Color    = aLight.Color;
    aLightParams.Position = aLight.Type == Graphic3d_TOLS_DIRECTIONAL
                         ? -aLight.Direction
                         :  aLight.Position;
    if (aLight.Type == Graphic3d_TOLS_SPOT)
    {
      aLightParams.Direction = aLight.Direction;
    }
    aLightParams.Parameters = aLight.Params;
    ++aLightsNb;
  }

  theProgram->SetUniform (myContext,
                          theProgram->GetStateLocation (OpenGl_OCC_LIGHT_SOURCE_COUNT),
                          aLightsNb);
  theProgram->SetUniform (myContext,
                          theProgram->GetStateLocation (OpenGl_OCC_LIGHT_AMBIENT),
                          anAmbient);
  theProgram->SetUniform (myContext,
                          theProgram->GetStateLocation (OpenGl_OCC_LIGHT_SOURCE_TYPES),
                          OpenGLMaxLights * OpenGl_ShaderLightType::NbOfVec2i(),
                          aLightTypeArray[0].Packed());
  if (aLightsNb > 0)
  {
    theProgram->SetUniform (myContext,
                            theProgram->GetStateLocation (OpenGl_OCC_LIGHT_SOURCE_PARAMS),
                            aLightsNb * OpenGl_ShaderLightParameters::NbOfVec4(),
                            aLightParamsArray[0].Packed());
  }
  delete[] aLightParamsArray;
  delete[] aLightTypeArray;

  theProgram->UpdateState (OpenGl_LIGHT_SOURCES_STATE, myLightSourceState.Index());
}

// =======================================================================
// function : PushProjectionState
// purpose  : Pushes state of OCCT projection transform to the program
// =======================================================================
void OpenGl_ShaderManager::PushProjectionState (const Handle(OpenGl_ShaderProgram)& theProgram) const
{
  if (myProjectionState.Index() == theProgram->ActiveState (OpenGl_PROJECTION_STATE))
  {
    return;
  }

  theProgram->SetUniform (myContext,
                          theProgram->GetStateLocation (OpenGl_OCC_PROJECTION_MATRIX),
                          myProjectionState.ProjectionMatrix());

  GLint aLocation = theProgram->GetStateLocation (OpenGl_OCC_PROJECTION_MATRIX_INVERSE);
  if (aLocation != OpenGl_ShaderProgram::INVALID_LOCATION)
  {
    theProgram->SetUniform (myContext, aLocation, myProjectionState.ProjectionMatrixInverse());
  }

  theProgram->SetUniform (myContext,
                          theProgram->GetStateLocation (OpenGl_OCC_PROJECTION_MATRIX_TRANSPOSE),
                          myProjectionState.ProjectionMatrix(), true);

  aLocation = theProgram->GetStateLocation (OpenGl_OCC_PROJECTION_MATRIX_INVERSE_TRANSPOSE);
  if (aLocation != OpenGl_ShaderProgram::INVALID_LOCATION)
  {
    theProgram->SetUniform (myContext, aLocation, myProjectionState.ProjectionMatrixInverse(), true);
  }

  theProgram->UpdateState (OpenGl_PROJECTION_STATE, myProjectionState.Index());
}

// =======================================================================
// function : PushModelWorldState
// purpose  : Pushes state of OCCT model-world transform to the program
// =======================================================================
void OpenGl_ShaderManager::PushModelWorldState (const Handle(OpenGl_ShaderProgram)& theProgram) const
{
  if (myModelWorldState.Index() == theProgram->ActiveState (OpenGl_MODEL_WORLD_STATE))
  {
    return;
  }

  theProgram->SetUniform (myContext,
                          theProgram->GetStateLocation (OpenGl_OCC_MODEL_WORLD_MATRIX),
                          myModelWorldState.ModelWorldMatrix());

  GLint aLocation = theProgram->GetStateLocation (OpenGl_OCC_MODEL_WORLD_MATRIX_INVERSE);
  if (aLocation != OpenGl_ShaderProgram::INVALID_LOCATION)
  {
    theProgram->SetUniform (myContext, aLocation, myModelWorldState.ModelWorldMatrixInverse());
  }

  theProgram->SetUniform (myContext,
                          theProgram->GetStateLocation (OpenGl_OCC_MODEL_WORLD_MATRIX_TRANSPOSE),
                          myModelWorldState.ModelWorldMatrix(), true);

  aLocation = theProgram->GetStateLocation (OpenGl_OCC_MODEL_WORLD_MATRIX_INVERSE_TRANSPOSE);
  if (aLocation != OpenGl_ShaderProgram::INVALID_LOCATION)
  {
    theProgram->SetUniform (myContext, aLocation, myModelWorldState.ModelWorldMatrixInverse(), true);
  }

  theProgram->UpdateState (OpenGl_MODEL_WORLD_STATE, myModelWorldState.Index());
}

// =======================================================================
// function : PushWorldViewState
// purpose  : Pushes state of OCCT world-view transform to the program
// =======================================================================
void OpenGl_ShaderManager::PushWorldViewState (const Handle(OpenGl_ShaderProgram)& theProgram) const
{
  if (myWorldViewState.Index() == theProgram->ActiveState (OpenGl_WORLD_VIEW_STATE))
  {
    return;
  }

  theProgram->SetUniform (myContext,
                          theProgram->GetStateLocation (OpenGl_OCC_WORLD_VIEW_MATRIX),
                          myWorldViewState.WorldViewMatrix());

  GLint aLocation = theProgram->GetStateLocation (OpenGl_OCC_WORLD_VIEW_MATRIX_INVERSE);
  if (aLocation != OpenGl_ShaderProgram::INVALID_LOCATION)
  {
    theProgram->SetUniform (myContext, aLocation, myWorldViewState.WorldViewMatrixInverse());
  }

  theProgram->SetUniform (myContext,
                          theProgram->GetStateLocation (OpenGl_OCC_WORLD_VIEW_MATRIX_TRANSPOSE),
                          myWorldViewState.WorldViewMatrix(), true);

  aLocation = theProgram->GetStateLocation (OpenGl_OCC_WORLD_VIEW_MATRIX_INVERSE_TRANSPOSE);
  if (aLocation != OpenGl_ShaderProgram::INVALID_LOCATION)
  {
    theProgram->SetUniform (myContext, aLocation, myWorldViewState.WorldViewMatrixInverse(), true);
  }

  theProgram->UpdateState (OpenGl_WORLD_VIEW_STATE, myWorldViewState.Index());
}

// =======================================================================
// function : UpdateClippingState
// purpose  : Updates state of OCCT clipping planes
// =======================================================================
void OpenGl_ShaderManager::UpdateClippingState()
{
  myClippingState.Update();
}

// =======================================================================
// function : RevertClippingState
// purpose  : Reverts state of OCCT clipping planes
// =======================================================================
void OpenGl_ShaderManager::RevertClippingState()
{
  myClippingState.Revert();
}

// =======================================================================
// function : PushClippingState
// purpose  : Pushes state of OCCT clipping planes to the program
// =======================================================================
void OpenGl_ShaderManager::PushClippingState (const Handle(OpenGl_ShaderProgram)& theProgram) const
{
  if (myClippingState.Index() == theProgram->ActiveState (OpenGl_CLIP_PLANES_STATE))
  {
    return;
  }

  theProgram->UpdateState (OpenGl_CLIP_PLANES_STATE, myClippingState.Index());
  const GLint aLocEquations = theProgram->GetStateLocation (OpenGl_OCC_CLIP_PLANE_EQUATIONS);
  if (aLocEquations == OpenGl_ShaderProgram::INVALID_LOCATION)
  {
    return;
  }

  GLint aPlanesNb = 0;
  for (Graphic3d_SequenceOfHClipPlane::Iterator anIter (myContext->Clipping().Planes());
       anIter.More(); anIter.Next())
  {
    const Handle(Graphic3d_ClipPlane)& aPlane = anIter.Value();
    if (!myContext->Clipping().IsEnabled (aPlane))
    {
      continue;
    }

    ++aPlanesNb;
  }
  if (aPlanesNb < 1)
  {
    return;
  }

  OpenGl_Vec4 anEquations[THE_MAX_CLIP_PLANES];
  GLuint aPlaneId = 0;
  for (Graphic3d_SequenceOfHClipPlane::Iterator anIter (myContext->Clipping().Planes());
       anIter.More(); anIter.Next())
  {
    const Handle(Graphic3d_ClipPlane)& aPlane = anIter.Value();
    if (!myContext->Clipping().IsEnabled (aPlane))
    {
      continue;
    }
    else if (aPlaneId >= THE_MAX_CLIP_PLANES)
    {
      myContext->PushMessage (GL_DEBUG_SOURCE_APPLICATION,
        GL_DEBUG_TYPE_PORTABILITY, 0, GL_DEBUG_SEVERITY_MEDIUM,
        "Warning: clipping planes limit (8) has been exceeded.");
      break;
    }

    const Graphic3d_ClipPlane::Equation& anEquation = aPlane->GetEquation();
    anEquations[aPlaneId] = OpenGl_Vec4 ((float) anEquation.x(),
                                         (float) anEquation.y(),
                                         (float) anEquation.z(),
                                         (float) anEquation.w());
    ++aPlaneId;
  }

  theProgram->SetUniform (myContext,
                          theProgram->GetStateLocation (OpenGl_OCC_CLIP_PLANE_COUNT),
                          aPlanesNb);
  theProgram->SetUniform (myContext, aLocEquations, THE_MAX_CLIP_PLANES, anEquations);
}

// =======================================================================
// function : PushState
// purpose  : Pushes state of OCCT graphics parameters to the program
// =======================================================================
void OpenGl_ShaderManager::PushState (const Handle(OpenGl_ShaderProgram)& theProgram) const
{
  PushClippingState    (theProgram);
  PushWorldViewState   (theProgram);
  PushModelWorldState  (theProgram);
  PushProjectionState  (theProgram);
  PushLightSourceState (theProgram);
}

// =======================================================================
// function : prepareStdProgramFont
// purpose  :
// =======================================================================
Standard_Boolean OpenGl_ShaderManager::prepareStdProgramFont()
{
  Handle(Graphic3d_ShaderProgram) aProgramSrc = new Graphic3d_ShaderProgram();
  TCollection_AsciiString aSrcVert = TCollection_AsciiString()
     + EOL"THE_SHADER_OUT vec2 TexCoord;"
       EOL"void main()"
       EOL"{"
       EOL"  TexCoord = occTexCoord.st;"
       EOL"  gl_Position = occProjectionMatrix * occWorldViewMatrix * occModelWorldMatrix * occVertex;"
       EOL"}";

  TCollection_AsciiString
    aSrcGetAlpha = EOL"float getAlpha(void) { return occTexture2D(occActiveSampler, TexCoord.st).a; }";
#if !defined(GL_ES_VERSION_2_0)
  if (myContext->core11 == NULL)
  {
    aSrcGetAlpha = EOL"float getAlpha(void) { return occTexture2D(occActiveSampler, TexCoord.st).r; }";
  }
#endif

  TCollection_AsciiString aSrcFrag = TCollection_AsciiString() +
     + EOL"THE_SHADER_IN vec2 TexCoord;"
     + aSrcGetAlpha
     + EOL"void main()"
       EOL"{"
       EOL"  vec4 aColor = occColor;"
       EOL"  aColor.a *= getAlpha();"
       EOL"  if (aColor.a <= 0.285) discard;"
       EOL"  occFragColor = aColor;"
       EOL"}";

#if !defined(GL_ES_VERSION_2_0)
  if (myContext->core32 != NULL)
  {
    aProgramSrc->SetHeader ("#version 150");
  }
#endif
  aProgramSrc->AttachShader (Graphic3d_ShaderObject::CreateFromSource (Graphic3d_TOS_VERTEX,   aSrcVert));
  aProgramSrc->AttachShader (Graphic3d_ShaderObject::CreateFromSource (Graphic3d_TOS_FRAGMENT, aSrcFrag));
  TCollection_AsciiString aKey;
  if (!Create (aProgramSrc, aKey, myFontProgram))
  {
    myFontProgram = new OpenGl_ShaderProgram(); // just mark as invalid
    return Standard_False;
  }
  return Standard_True;
}

// =======================================================================
// function : prepareStdProgramFboBlit
// purpose  :
// =======================================================================
Standard_Boolean OpenGl_ShaderManager::prepareStdProgramFboBlit()
{
  Handle(Graphic3d_ShaderProgram) aProgramSrc = new Graphic3d_ShaderProgram();
  TCollection_AsciiString aSrcVert =
      EOL"THE_SHADER_OUT vec2 TexCoord;"
      EOL"void main()"
      EOL"{"
      EOL"  TexCoord    = occVertex.zw;"
      EOL"  gl_Position = vec4(occVertex.x, occVertex.y, 0.0, 1.0);"
      EOL"}";

  TCollection_AsciiString aSrcFrag =
      EOL"uniform sampler2D uColorSampler;"
      EOL"uniform sampler2D uDepthSampler;"
      EOL
      EOL"THE_SHADER_IN vec2 TexCoord;"
      EOL
      EOL"void main()"
      EOL"{"
      EOL"  gl_FragDepth = occTexture2D (uDepthSampler, TexCoord).r;"
      EOL"  occFragColor = occTexture2D (uColorSampler, TexCoord);"
      EOL"}";

#if defined(GL_ES_VERSION_2_0)
  if (myContext->IsGlGreaterEqual (3, 0))
  {
    aProgramSrc->SetHeader ("#version 300 es");
  }
  else if (myContext->extFragDepth)
  {
    aProgramSrc->SetHeader ("#extension GL_EXT_frag_depth : enable"
                         EOL"#define gl_FragDepth gl_FragDepthEXT");
  }
  else
  {
    // there is no way to draw into depth buffer
    aSrcFrag =
      EOL"uniform sampler2D uColorSampler;"
      EOL
      EOL"THE_SHADER_IN vec2 TexCoord;"
      EOL
      EOL"void main()"
      EOL"{"
      EOL"  occFragColor = occTexture2D (uColorSampler, TexCoord);"
      EOL"}";
  }
#else
  if (myContext->core32 != NULL)
  {
    aProgramSrc->SetHeader ("#version 150");
  }
#endif
  aProgramSrc->AttachShader (Graphic3d_ShaderObject::CreateFromSource (Graphic3d_TOS_VERTEX,   aSrcVert));
  aProgramSrc->AttachShader (Graphic3d_ShaderObject::CreateFromSource (Graphic3d_TOS_FRAGMENT, aSrcFrag));
  TCollection_AsciiString aKey;
  if (!Create (aProgramSrc, aKey, myBlitProgram))
  {
    myBlitProgram = new OpenGl_ShaderProgram(); // just mark as invalid
    return Standard_False;
  }

  myContext->BindProgram (myBlitProgram);
  myBlitProgram->SetSampler (myContext, "uColorSampler", 0);
  myBlitProgram->SetSampler (myContext, "uDepthSampler", 1);
  myContext->BindProgram (NULL);
  return Standard_True;
}

// =======================================================================
// function : pointSpriteAlphaSrc
// purpose  :
// =======================================================================
TCollection_AsciiString OpenGl_ShaderManager::pointSpriteAlphaSrc (const Standard_Integer theBits)
{
  TCollection_AsciiString aSrcGetAlpha = EOL"float getAlpha(void) { return occTexture2D(occActiveSampler, " THE_VEC2_glPointCoord ").a; }";
#if !defined(GL_ES_VERSION_2_0)
  if (myContext->core11 == NULL
   && (theBits & OpenGl_PO_TextureA) != 0)
  {
    aSrcGetAlpha = EOL"float getAlpha(void) { return occTexture2D(occActiveSampler, " THE_VEC2_glPointCoord ").r; }";
  }
#else
  (void )theBits;
#endif
  return aSrcGetAlpha;
}

namespace
{

  // =======================================================================
  // function : textureUsed
  // purpose  :
  // =======================================================================
  static bool textureUsed (const Standard_Integer theBits)
  {
    return (theBits & OpenGl_PO_TextureA) != 0 || (theBits & OpenGl_PO_TextureRGB) != 0;
  }

}

// =======================================================================
// function : prepareStdProgramFlat
// purpose  :
// =======================================================================
Standard_Boolean OpenGl_ShaderManager::prepareStdProgramFlat (Handle(OpenGl_ShaderProgram)& theProgram,
                                                              const Standard_Integer        theBits)
{
  Handle(Graphic3d_ShaderProgram) aProgramSrc = new Graphic3d_ShaderProgram();
  TCollection_AsciiString aSrcVert, aSrcVertExtraOut, aSrcVertExtraMain, aSrcVertExtraFunc, aSrcGetAlpha, aSrcFrag, aSrcFragExtraOut, aSrcFragExtraMain;
  TCollection_AsciiString aSrcFragGetColor     = EOL"vec4 getColor(void) { return occColor; }";
  TCollection_AsciiString aSrcFragMainGetColor = EOL"  occFragColor = getColor();";
  if ((theBits & OpenGl_PO_Point) != 0)
  {
  #if defined(GL_ES_VERSION_2_0)
    aSrcVertExtraMain += EOL"  gl_PointSize = occPointSize;";
  #endif

    if ((theBits & OpenGl_PO_TextureRGB) != 0)
    {
      aSrcFragGetColor =
        EOL"vec4 getColor(void) { return occTexture2D(occActiveSampler, " THE_VEC2_glPointCoord "); }";
    }

    if (textureUsed (theBits))
    {
      aSrcGetAlpha = pointSpriteAlphaSrc (theBits);

    #if !defined(GL_ES_VERSION_2_0)
      if (myContext->core11 != NULL
        && myContext->IsGlGreaterEqual (2, 1))
      {
        aProgramSrc->SetHeader ("#version 120"); // gl_PointCoord has been added since GLSL 1.2
      }
    #endif

      aSrcFragMainGetColor =
        EOL"  vec4 aColor = getColor();"
        EOL"  aColor.a = getAlpha();"
        EOL"  if (aColor.a <= 0.1) discard;"
        EOL"  occFragColor = aColor;";
    }
    else
    {
      aSrcFragMainGetColor =
        EOL"  vec4 aColor = getColor();"
        EOL"  if (aColor.a <= 0.1) discard;"
        EOL"  occFragColor = aColor;";
    }
  }
  else
  {
    if ((theBits & OpenGl_PO_TextureRGB) != 0)
    {
      aSrcVertExtraOut  += THE_VARY_TexCoord_OUT;
      aSrcFragExtraOut  += THE_VARY_TexCoord_IN;
      aSrcVertExtraMain += THE_VARY_TexCoord_Trsf;

      aSrcFragGetColor =
        EOL"vec4 getColor(void) { return occTexture2D(occActiveSampler, TexCoord.st / TexCoord.w); }";
    }
    else if ((theBits & OpenGl_PO_TextureEnv) != 0)
    {
      aSrcVertExtraOut += THE_VARY_TexCoord_OUT;
      aSrcFragExtraOut += THE_VARY_TexCoord_IN;

      aSrcVertExtraFunc = THE_FUNC_transformNormal;

      aSrcVertExtraMain +=
        EOL"  vec4 aPosition = occWorldViewMatrix * occModelWorldMatrix * occVertex;"
        EOL"  vec3 aNormal   = transformNormal (occNormal);"
        EOL"  vec3 aReflect  = reflect (normalize (aPosition.xyz), aNormal);"
        EOL"  aReflect.z += 1.0;"
        EOL"  TexCoord = vec4(aReflect.xy * inversesqrt (dot (aReflect, aReflect)) * 0.5 + vec2 (0.5), 0.0, 1.0);";

      aSrcFragGetColor =
        EOL"vec4 getColor(void) { return occTexture2D (occActiveSampler, TexCoord.st); }";
    }
  }
  if ((theBits & OpenGl_PO_VertColor) != 0)
  {
    aSrcVertExtraOut  += EOL"THE_SHADER_OUT vec4 VertColor;";
    aSrcVertExtraMain += EOL"  VertColor = occVertColor;";
    aSrcFragExtraOut  += EOL"THE_SHADER_IN  vec4 VertColor;";
    aSrcFragGetColor  =  EOL"vec4 getColor(void) { return VertColor; }";
  }
  if ((theBits & OpenGl_PO_ClipPlanesN) != 0)
  {
    aSrcVertExtraOut +=
      EOL"THE_SHADER_OUT vec4 PositionWorld;"
      EOL"THE_SHADER_OUT vec4 Position;";
    aSrcFragExtraOut +=
      EOL"THE_SHADER_IN  vec4 PositionWorld;"
      EOL"THE_SHADER_IN  vec4 Position;";
    aSrcVertExtraMain +=
      EOL"  PositionWorld = occModelWorldMatrix * occVertex;"
      EOL"  Position      = occWorldViewMatrix * PositionWorld;";

    if ((theBits & OpenGl_PO_ClipPlanes1) != 0)
    {
      aSrcFragExtraMain += THE_FRAG_CLIP_PLANES_1;
    }
    else if ((theBits & OpenGl_PO_ClipPlanes2) != 0)
    {
      aSrcFragExtraMain += THE_FRAG_CLIP_PLANES_2;
    }
    else
    {
      aSrcFragExtraMain += THE_FRAG_CLIP_PLANES_N;
    }
  }

  TCollection_AsciiString aSrcVertEndMain;
  if ((theBits & OpenGl_PO_StippleLine) != 0)
  {
    bool hasGlslBitOps = false;
  #if defined(GL_ES_VERSION_2_0)
    if (myContext->IsGlGreaterEqual (3, 0))
    {
      aProgramSrc->SetHeader ("#version 300 es");
      hasGlslBitOps = true;
    }
  #else
    if (myContext->IsGlGreaterEqual (3, 0))
    {
      aProgramSrc->SetHeader ("#version 130");
      hasGlslBitOps = true;
    }
    else if(myContext->CheckExtension("GL_EXT_gpu_shader4"))
    {
      aProgramSrc->SetHeader ("#extension GL_EXT_gpu_shader4 : enable");
      hasGlslBitOps = true;
    }
  #endif

    if (hasGlslBitOps)
    {
      aSrcVertExtraOut +=
        EOL"THE_SHADER_OUT vec2 ScreenSpaceCoord;";
      aSrcFragExtraOut +=
        EOL"THE_SHADER_IN  vec2 ScreenSpaceCoord;"
        EOL"uniform int   uPattern;"
        EOL"uniform float uFactor;";
      aSrcVertEndMain =
        EOL"  ScreenSpaceCoord = gl_Position.xy / gl_Position.w;";
      aSrcFragMainGetColor =
        EOL"  float anAngle      = atan (dFdx (ScreenSpaceCoord.x), dFdy (ScreenSpaceCoord.y));"
        EOL"  float aRotatePoint = gl_FragCoord.x * sin (anAngle) + gl_FragCoord.y * cos (anAngle);"
        EOL"  uint  aBit         = uint (floor (aRotatePoint / uFactor + 0.5)) & 15U;"
        EOL"  if ((uint (uPattern) & (1U << aBit)) == 0U) discard;"
        EOL"  vec4 aColor = getColor();"
        EOL"  if (aColor.a <= 0.1) discard;"
        EOL"  occFragColor = aColor;";
    }
    else
    {
      const TCollection_ExtendedString aWarnMessage =
        "Warning: stipple lines in GLSL will be ignored.";
      myContext->PushMessage (GL_DEBUG_SOURCE_APPLICATION,
        GL_DEBUG_TYPE_PORTABILITY, 0, GL_DEBUG_SEVERITY_HIGH, aWarnMessage);
    }
  }

  aSrcVert =
      aSrcVertExtraFunc
    + aSrcVertExtraOut
    + EOL"void main()"
      EOL"{"
    + aSrcVertExtraMain
    + EOL"  gl_Position = occProjectionMatrix * occWorldViewMatrix * occModelWorldMatrix * occVertex;"
    + aSrcVertEndMain
    + EOL"}";

  aSrcFrag =
      aSrcFragExtraOut
    + aSrcFragGetColor
    + aSrcGetAlpha
    + EOL"void main()"
      EOL"{"
    + aSrcFragExtraMain
    + aSrcFragMainGetColor
    + EOL"}";

#if !defined(GL_ES_VERSION_2_0)
  if (myContext->core32 != NULL)
  {
    aProgramSrc->SetHeader ("#version 150");
  }
#endif
  aProgramSrc->AttachShader (Graphic3d_ShaderObject::CreateFromSource (Graphic3d_TOS_VERTEX,   aSrcVert));
  aProgramSrc->AttachShader (Graphic3d_ShaderObject::CreateFromSource (Graphic3d_TOS_FRAGMENT, aSrcFrag));

  TCollection_AsciiString aKey;
  if (!Create (aProgramSrc, aKey, theProgram))
  {
    theProgram = new OpenGl_ShaderProgram(); // just mark as invalid
    return Standard_False;
  }
  return Standard_True;
}

// =======================================================================
// function : pointSpriteShadingSrc
// purpose  :
// =======================================================================
TCollection_AsciiString OpenGl_ShaderManager::pointSpriteShadingSrc (const TCollection_AsciiString theBaseColorSrc,
                                                                     const Standard_Integer theBits)
{
  TCollection_AsciiString aSrcFragGetColor;
  if ((theBits & OpenGl_PO_TextureA) != 0)
  {
    aSrcFragGetColor = pointSpriteAlphaSrc (theBits) +
      EOL"vec4 getColor(void)"
      EOL"{"
      EOL"  vec4 aColor = " + theBaseColorSrc + ";"
      EOL"  aColor.a = getAlpha();"
      EOL"  if (aColor.a <= 0.1) discard;"
      EOL"  return aColor;"
      EOL"}";
  }
  else if ((theBits & OpenGl_PO_TextureRGB) != 0)
  {
    aSrcFragGetColor = TCollection_AsciiString() +
      EOL"vec4 getColor(void)"
      EOL"{"
      EOL"  vec4 aColor = " + theBaseColorSrc + ";"
      EOL"  aColor = occTexture2D(occActiveSampler, " THE_VEC2_glPointCoord ") * aColor;"
      EOL"  if (aColor.a <= 0.1) discard;"
      EOL"  return aColor;"
      EOL"}";
  }

  return aSrcFragGetColor;
}

// =======================================================================
// function : stdComputeLighting
// purpose  :
// =======================================================================
TCollection_AsciiString OpenGl_ShaderManager::stdComputeLighting (const Standard_Boolean theHasVertColor)
{
  Standard_Integer aLightsMap[Graphic3d_TOLS_SPOT + 1] = { 0, 0, 0, 0 };
  TCollection_AsciiString aLightsFunc, aLightsLoop;
  const OpenGl_ListOfLight* aLights = myLightSourceState.LightSources();
  if (aLights != NULL)
  {
    Standard_Integer anIndex = 0;
    for (OpenGl_ListOfLight::Iterator aLightIter (*aLights); aLightIter.More(); aLightIter.Next(), ++anIndex)
    {
      switch (aLightIter.Value().Type)
      {
        case Graphic3d_TOLS_AMBIENT:
          --anIndex;
          break; // skip ambient
        case Graphic3d_TOLS_DIRECTIONAL:
          aLightsLoop = aLightsLoop + EOL"    directionalLight (" + anIndex + ", theNormal, theView, theIsFront);";
          break;
        case Graphic3d_TOLS_POSITIONAL:
          aLightsLoop = aLightsLoop + EOL"    pointLight (" + anIndex + ", theNormal, theView, aPoint, theIsFront);";
          break;
        case Graphic3d_TOLS_SPOT:
          aLightsLoop = aLightsLoop + EOL"    spotLight (" + anIndex + ", theNormal, theView, aPoint, theIsFront);";
          break;
      }
      aLightsMap[aLightIter.Value().Type] += 1;
    }
    const Standard_Integer aNbLoopLights = aLightsMap[Graphic3d_TOLS_DIRECTIONAL]
                                         + aLightsMap[Graphic3d_TOLS_POSITIONAL]
                                         + aLightsMap[Graphic3d_TOLS_SPOT];
    if (aLightsMap[Graphic3d_TOLS_DIRECTIONAL] == 1
     && aNbLoopLights == 1)
    {
      // use the version with hard-coded first index
      aLightsLoop = EOL"    directionalLightFirst(theNormal, theView, theIsFront);";
      aLightsFunc += THE_FUNC_directionalLightFirst;
    }
    else if (aLightsMap[Graphic3d_TOLS_DIRECTIONAL] > 0)
    {
      aLightsFunc += THE_FUNC_directionalLight;
    }
    if (aLightsMap[Graphic3d_TOLS_POSITIONAL] > 0)
    {
      aLightsFunc += THE_FUNC_pointLight;
    }
    if (aLightsMap[Graphic3d_TOLS_SPOT] > 0)
    {
      aLightsFunc += THE_FUNC_spotLight;
    }
  }

  TCollection_AsciiString aGetMatAmbient = "theIsFront ? occFrontMaterial_Ambient()  : occBackMaterial_Ambient();";
  TCollection_AsciiString aGetMatDiffuse = "theIsFront ? occFrontMaterial_Diffuse()  : occBackMaterial_Diffuse();";
  if (theHasVertColor)
  {
    aGetMatAmbient = "getVertColor();";
    aGetMatDiffuse = "getVertColor();";
  }

  return TCollection_AsciiString()
    + THE_FUNC_lightDef
    + aLightsFunc
    + EOL
      EOL"vec4 computeLighting (in vec3 theNormal,"
      EOL"                      in vec3 theView,"
      EOL"                      in vec4 thePoint,"
      EOL"                      in bool theIsFront)"
      EOL"{"
      EOL"  Ambient  = occLightAmbient.rgb;"
      EOL"  Diffuse  = vec3 (0.0);"
      EOL"  Specular = vec3 (0.0);"
      EOL"  vec3 aPoint = thePoint.xyz / thePoint.w;"
    + aLightsLoop
    + EOL"  vec4 aMatAmbient  = " + aGetMatAmbient
    + EOL"  vec4 aMatDiffuse  = " + aGetMatDiffuse
    + EOL"  vec4 aMatSpecular = theIsFront ? occFrontMaterial_Specular() : occBackMaterial_Specular();"
      EOL"  vec4 aMatEmission = theIsFront ? occFrontMaterial_Emission() : occBackMaterial_Emission();"
      EOL"  vec3 aColor = Ambient  * aMatAmbient.rgb"
      EOL"              + Diffuse  * aMatDiffuse.rgb"
      EOL"              + Specular * aMatSpecular.rgb"
      EOL"                         + aMatEmission.rgb;"
      EOL"  return vec4 (aColor, aMatDiffuse.a);"
      EOL"}";
}

// =======================================================================
// function : prepareStdProgramGouraud
// purpose  :
// =======================================================================
Standard_Boolean OpenGl_ShaderManager::prepareStdProgramGouraud (Handle(OpenGl_ShaderProgram)& theProgram,
                                                                 const Standard_Integer        theBits)
{
  Handle(Graphic3d_ShaderProgram) aProgramSrc = new Graphic3d_ShaderProgram();
  TCollection_AsciiString aSrcVert, aSrcVertColor, aSrcVertExtraOut, aSrcVertExtraMain, aSrcFrag, aSrcFragExtraOut, aSrcFragExtraMain;
  TCollection_AsciiString aSrcFragGetColor = EOL"vec4 getColor(void) { return gl_FrontFacing ? FrontColor : BackColor; }";
  if ((theBits & OpenGl_PO_Point) != 0)
  {
  #if defined(GL_ES_VERSION_2_0)
    aSrcVertExtraMain += EOL"  gl_PointSize = occPointSize;";
  #endif

    if (textureUsed (theBits))
    {
      #if !defined(GL_ES_VERSION_2_0)
        if (myContext->core11 != NULL
         && myContext->IsGlGreaterEqual (2, 1))
        {
          aProgramSrc->SetHeader ("#version 120"); // gl_PointCoord has been added since GLSL 1.2
        }
      #endif

      aSrcFragGetColor = pointSpriteShadingSrc ("gl_FrontFacing ? FrontColor : BackColor", theBits);
    }
  }
  else
  {
    if ((theBits & OpenGl_PO_TextureRGB) != 0)
    {
      aSrcVertExtraOut  += THE_VARY_TexCoord_OUT;
      aSrcFragExtraOut  += THE_VARY_TexCoord_IN;
      aSrcVertExtraMain += THE_VARY_TexCoord_Trsf;

      aSrcFragGetColor =
        EOL"vec4 getColor(void)"
        EOL"{"
        EOL"  vec4 aColor = gl_FrontFacing ? FrontColor : BackColor;"
        EOL"  return occTexture2D(occActiveSampler, TexCoord.st / TexCoord.w) * aColor;"
        EOL"}";
    }
  }

  if ((theBits & OpenGl_PO_VertColor) != 0)
  {
    aSrcVertColor = EOL"vec4 getVertColor(void) { return occVertColor; }";
  }

  if ((theBits & OpenGl_PO_ClipPlanesN) != 0)
  {
    aSrcVertExtraOut +=
      EOL"THE_SHADER_OUT vec4 PositionWorld;"
      EOL"THE_SHADER_OUT vec4 Position;";
    aSrcFragExtraOut +=
      EOL"THE_SHADER_IN  vec4 PositionWorld;"
      EOL"THE_SHADER_IN  vec4 Position;";
    aSrcVertExtraMain +=
      EOL"  PositionWorld = aPositionWorld;"
      EOL"  Position      = aPosition;";

    if ((theBits & OpenGl_PO_ClipPlanes1) != 0)
    {
      aSrcFragExtraMain += THE_FRAG_CLIP_PLANES_1;
    }
    else if ((theBits & OpenGl_PO_ClipPlanes2) != 0)
    {
      aSrcFragExtraMain += THE_FRAG_CLIP_PLANES_2;
    }
    else
    {
      aSrcFragExtraMain += THE_FRAG_CLIP_PLANES_N;
    }
  }

  const TCollection_AsciiString aLights = stdComputeLighting ((theBits & OpenGl_PO_VertColor) != 0);
  aSrcVert = TCollection_AsciiString()
    + THE_FUNC_transformNormal
    + EOL
    + aSrcVertColor
    + aLights
    + EOL
      EOL"THE_SHADER_OUT vec4 FrontColor;"
      EOL"THE_SHADER_OUT vec4 BackColor;"
      EOL
    + aSrcVertExtraOut
    + EOL"void main()"
      EOL"{"
      EOL"  vec4 aPositionWorld = occModelWorldMatrix * occVertex;"
      EOL"  vec4 aPosition      = occWorldViewMatrix * aPositionWorld;"
      EOL"  vec3 aNormal        = transformNormal (occNormal);"
      EOL"  vec3 aView          = vec3 (0.0, 0.0, 1.0);"
      EOL"  FrontColor  = computeLighting (normalize (aNormal), normalize (aView), aPosition, true);"
      EOL"  BackColor   = computeLighting (normalize (aNormal), normalize (aView), aPosition, false);"
    + aSrcVertExtraMain
    + EOL"  gl_Position = occProjectionMatrix * occWorldViewMatrix * occModelWorldMatrix * occVertex;"
      EOL"}";

  aSrcFrag = TCollection_AsciiString()
    + EOL"THE_SHADER_IN vec4 FrontColor;"
      EOL"THE_SHADER_IN vec4 BackColor;"
    + aSrcFragExtraOut
    + aSrcFragGetColor
    + EOL"void main()"
      EOL"{"
    + aSrcFragExtraMain
    + EOL"  occFragColor = getColor();"
      EOL"}";

#if !defined(GL_ES_VERSION_2_0)
  if (myContext->core32 != NULL)
  {
    aProgramSrc->SetHeader ("#version 150");
  }
#endif
  aProgramSrc->AttachShader (Graphic3d_ShaderObject::CreateFromSource (Graphic3d_TOS_VERTEX,   aSrcVert));
  aProgramSrc->AttachShader (Graphic3d_ShaderObject::CreateFromSource (Graphic3d_TOS_FRAGMENT, aSrcFrag));
  TCollection_AsciiString aKey;
  if (!Create (aProgramSrc, aKey, theProgram))
  {
    theProgram = new OpenGl_ShaderProgram(); // just mark as invalid
    return Standard_False;
  }
  return Standard_True;
}

// =======================================================================
// function : prepareStdProgramPhong
// purpose  :
// =======================================================================
Standard_Boolean OpenGl_ShaderManager::prepareStdProgramPhong (Handle(OpenGl_ShaderProgram)& theProgram,
                                                               const Standard_Integer        theBits)
{
  #define thePhongCompLight "computeLighting (normalize (Normal), normalize (View), Position, gl_FrontFacing)"

  Handle(Graphic3d_ShaderProgram) aProgramSrc = new Graphic3d_ShaderProgram();
  TCollection_AsciiString aSrcVert, aSrcVertExtraOut, aSrcVertExtraMain, aSrcFrag, aSrcFragExtraOut, aSrcFragGetVertColor, aSrcFragExtraMain;
  TCollection_AsciiString aSrcFragGetColor = EOL"vec4 getColor(void) { return " thePhongCompLight "; }";
  if ((theBits & OpenGl_PO_Point) != 0)
  {
  #if defined(GL_ES_VERSION_2_0)
    aSrcVertExtraMain += EOL"  gl_PointSize = occPointSize;";
  #endif

    if (textureUsed (theBits))
    {
      #if !defined(GL_ES_VERSION_2_0)
        if (myContext->core11 != NULL
         && myContext->IsGlGreaterEqual (2, 1))
        {
          aProgramSrc->SetHeader ("#version 120"); // gl_PointCoord has been added since GLSL 1.2
        }
      #endif

      aSrcFragGetColor = pointSpriteShadingSrc (thePhongCompLight, theBits);
    }
  }
  else
  {
    if ((theBits & OpenGl_PO_TextureRGB) != 0)
    {
      aSrcVertExtraOut  += THE_VARY_TexCoord_OUT;
      aSrcFragExtraOut  += THE_VARY_TexCoord_IN;
      aSrcVertExtraMain += THE_VARY_TexCoord_Trsf;

      aSrcFragGetColor =
        EOL"vec4 getColor(void)"
        EOL"{"
        EOL"  vec4 aColor = " thePhongCompLight ";"
        EOL"  return occTexture2D(occActiveSampler, TexCoord.st / TexCoord.w) * aColor;"
        EOL"}";
    }
  }

  if ((theBits & OpenGl_PO_VertColor) != 0)
  {
    aSrcVertExtraOut    += EOL"THE_SHADER_OUT vec4 VertColor;";
    aSrcVertExtraMain   += EOL"  VertColor = occVertColor;";
    aSrcFragGetVertColor = EOL"THE_SHADER_IN  vec4 VertColor;"
                           EOL"vec4 getVertColor(void) { return VertColor; }";
  }

  if ((theBits & OpenGl_PO_ClipPlanesN) != 0)
  {
    if ((theBits & OpenGl_PO_ClipPlanes1) != 0)
    {
      aSrcFragExtraMain += THE_FRAG_CLIP_PLANES_1;
    }
    else if ((theBits & OpenGl_PO_ClipPlanes2) != 0)
    {
      aSrcFragExtraMain += THE_FRAG_CLIP_PLANES_2;
    }
    else
    {
      aSrcFragExtraMain += THE_FRAG_CLIP_PLANES_N;
    }
  }

  aSrcVert = TCollection_AsciiString()
    + THE_FUNC_transformNormal
    + EOL
      EOL"THE_SHADER_OUT vec4 PositionWorld;"
      EOL"THE_SHADER_OUT vec4 Position;"
      EOL"THE_SHADER_OUT vec3 Normal;"
      EOL"THE_SHADER_OUT vec3 View;"
      EOL
    + aSrcVertExtraOut
    + EOL"void main()"
      EOL"{"
      EOL"  PositionWorld = occModelWorldMatrix * occVertex;"
      EOL"  Position      = occWorldViewMatrix * PositionWorld;"
      EOL"  Normal        = transformNormal (occNormal);"
      EOL"  View          = vec3 (0.0, 0.0, 1.0);"
    + aSrcVertExtraMain
    + EOL"  gl_Position = occProjectionMatrix * occWorldViewMatrix * occModelWorldMatrix * occVertex;"
      EOL"}";

  const TCollection_AsciiString aLights = stdComputeLighting ((theBits & OpenGl_PO_VertColor) != 0);
  aSrcFrag = TCollection_AsciiString()
    + EOL"THE_SHADER_IN vec4 PositionWorld;"
      EOL"THE_SHADER_IN vec4 Position;"
      EOL"THE_SHADER_IN vec3 Normal;"
      EOL"THE_SHADER_IN vec3 View;"
    + EOL
    + aSrcFragExtraOut
    + aSrcFragGetVertColor
    + aLights
    + aSrcFragGetColor
    + EOL
      EOL"void main()"
      EOL"{"
    + aSrcFragExtraMain
    + EOL"  occFragColor = getColor();"
      EOL"}";

#if !defined(GL_ES_VERSION_2_0)
  if (myContext->core32 != NULL)
  {
    aProgramSrc->SetHeader ("#version 150");
  }
#endif
  aProgramSrc->AttachShader (Graphic3d_ShaderObject::CreateFromSource (Graphic3d_TOS_VERTEX,   aSrcVert));
  aProgramSrc->AttachShader (Graphic3d_ShaderObject::CreateFromSource (Graphic3d_TOS_FRAGMENT, aSrcFrag));
  TCollection_AsciiString aKey;
  if (!Create (aProgramSrc, aKey, theProgram))
  {
    theProgram = new OpenGl_ShaderProgram(); // just mark as invalid
    return Standard_False;
  }
  return Standard_True;
}

// =======================================================================
// function : prepareStdProgramStereo
// purpose  :
// =======================================================================
Standard_Boolean OpenGl_ShaderManager::prepareStdProgramStereo (Handle(OpenGl_ShaderProgram)& theProgram,
                                                                const Graphic3d_StereoMode    theStereoMode)
{
  Handle(Graphic3d_ShaderProgram) aProgramSrc = new Graphic3d_ShaderProgram();
  TCollection_AsciiString aSrcVert =
      EOL"THE_SHADER_OUT vec2 TexCoord;"
      EOL"void main()"
      EOL"{"
      EOL"  TexCoord    = occVertex.zw;"
      EOL"  gl_Position = vec4(occVertex.x, occVertex.y, 0.0, 1.0);"
      EOL"}";

  TCollection_AsciiString aSrcFrag;
  switch (theStereoMode)
  {
    case Graphic3d_StereoMode_Anaglyph:
    {
      aSrcFrag =
          EOL"uniform sampler2D uLeftSampler;"
          EOL"uniform sampler2D uRightSampler;"
          EOL
          EOL"uniform mat4 uMultL;"
          EOL"uniform mat4 uMultR;"
          EOL
          EOL"const vec4 THE_POW_UP   =       vec4 (2.2, 2.2, 2.2, 1.0);"
          EOL"const vec4 THE_POW_DOWN = 1.0 / vec4 (2.2, 2.2, 2.2, 1.0);"
          EOL
          EOL"THE_SHADER_IN vec2 TexCoord;"
          EOL
          EOL"void main()"
          EOL"{"
          EOL"  vec4 aColorL = occTexture2D (uLeftSampler,  TexCoord);"
          EOL"  vec4 aColorR = occTexture2D (uRightSampler, TexCoord);"
          EOL"  aColorL = pow (aColorL, THE_POW_UP);" // normalize
          EOL"  aColorR = pow (aColorR, THE_POW_UP);"
          EOL"  vec4 aColor = uMultR * aColorR + uMultL * aColorL;"
          EOL"  occFragColor = pow (aColor, THE_POW_DOWN);"
          EOL"}";
      break;
    }
    case Graphic3d_StereoMode_RowInterlaced:
    {
      aSrcFrag =
          EOL"uniform sampler2D uLeftSampler;"
          EOL"uniform sampler2D uRightSampler;"
          EOL
          EOL"THE_SHADER_IN vec2 TexCoord;"
          EOL
          EOL"void main()"
          EOL"{"
          EOL"  vec4 aColorL = occTexture2D (uLeftSampler,  TexCoord);"
          EOL"  vec4 aColorR = occTexture2D (uRightSampler, TexCoord);"
          EOL"  if (int (mod (gl_FragCoord.y - 1023.5, 2.0)) != 1)"
          EOL"  {"
          EOL"    occFragColor = aColorL;"
          EOL"  }"
          EOL"  else"
          EOL"  {"
          EOL"    occFragColor = aColorR;"
          EOL"  }"
          EOL"}";
      break;
    }
    case Graphic3d_StereoMode_ColumnInterlaced:
    {
      aSrcFrag =
          EOL"uniform sampler2D uLeftSampler;"
          EOL"uniform sampler2D uRightSampler;"
          EOL
          EOL"THE_SHADER_IN vec2 TexCoord;"
          EOL
          EOL"void main()"
          EOL"{"
          EOL"  vec4 aColorL = occTexture2D (uLeftSampler,  TexCoord);"
          EOL"  vec4 aColorR = occTexture2D (uRightSampler, TexCoord);"
          EOL"  if (int (mod (gl_FragCoord.x - 1023.5, 2.0)) == 1)"
          EOL"  {"
          EOL"    occFragColor = aColorL;"
          EOL"  }"
          EOL"  else"
          EOL"  {"
          EOL"    occFragColor = aColorR;"
          EOL"  }"
          EOL"}";
      break;
    }
    case Graphic3d_StereoMode_ChessBoard:
    {
      aSrcFrag =
          EOL"uniform sampler2D uLeftSampler;"
          EOL"uniform sampler2D uRightSampler;"
          EOL
          EOL"THE_SHADER_IN vec2 TexCoord;"
          EOL
          EOL"void main()"
          EOL"{"
          EOL"  vec4 aColorL = occTexture2D (uLeftSampler,  TexCoord);"
          EOL"  vec4 aColorR = occTexture2D (uRightSampler, TexCoord);"
          EOL"  bool isEvenX = int(mod(floor(gl_FragCoord.x - 1023.5), 2.0)) != 1;"
          EOL"  bool isEvenY = int(mod(floor(gl_FragCoord.y - 1023.5), 2.0)) == 1;"
          EOL"  if ((isEvenX && isEvenY) || (!isEvenX && !isEvenY))"
          EOL"  {"
          EOL"    occFragColor = aColorL;"
          EOL"  }"
          EOL"  else"
          EOL"  {"
          EOL"    occFragColor = aColorR;"
          EOL"  }"
          EOL"}";
      break;
    }
    case Graphic3d_StereoMode_SideBySide:
    {
      aSrcFrag =
          EOL"uniform sampler2D uLeftSampler;"
          EOL"uniform sampler2D uRightSampler;"
          EOL
          EOL"THE_SHADER_IN vec2 TexCoord;"
          EOL
          EOL"void main()"
          EOL"{"
          EOL"  vec2 aTexCoord = vec2 (TexCoord.x * 2.0, TexCoord.y);"
          EOL"  if (TexCoord.x > 0.5)"
          EOL"  {"
          EOL"    aTexCoord.x -= 1.0;"
          EOL"  }"
          EOL"  vec4 aColorL = occTexture2D (uLeftSampler,  aTexCoord);"
          EOL"  vec4 aColorR = occTexture2D (uRightSampler, aTexCoord);"
          EOL"  if (TexCoord.x <= 0.5)"
          EOL"  {"
          EOL"    occFragColor = aColorL;"
          EOL"  }"
          EOL"  else"
          EOL"  {"
          EOL"    occFragColor = aColorR;"
          EOL"  }"
          EOL"}";
      break;
    }
    case Graphic3d_StereoMode_OverUnder:
    {
      aSrcFrag =
          EOL"uniform sampler2D uLeftSampler;"
          EOL"uniform sampler2D uRightSampler;"
          EOL
          EOL"THE_SHADER_IN vec2 TexCoord;"
          EOL
          EOL"void main()"
          EOL"{"
          EOL"  vec2 aTexCoord = vec2 (TexCoord.x, TexCoord.y * 2.0);"
          EOL"  if (TexCoord.y > 0.5)"
          EOL"  {"
          EOL"    aTexCoord.y -= 1.0;"
          EOL"  }"
          EOL"  vec4 aColorL = occTexture2D (uLeftSampler,  aTexCoord);"
          EOL"  vec4 aColorR = occTexture2D (uRightSampler, aTexCoord);"
          EOL"  if (TexCoord.y <= 0.5)"
          EOL"  {"
          EOL"    occFragColor = aColorL;"
          EOL"  }"
          EOL"  else"
          EOL"  {"
          EOL"    occFragColor = aColorR;"
          EOL"  }"
          EOL"}";
      break;
    }
    case Graphic3d_StereoMode_QuadBuffer:
    case Graphic3d_StereoMode_SoftPageFlip:
    default:
    {
      /*const Handle(OpenGl_ShaderProgram)& aProgram = myStereoPrograms[Graphic3d_StereoMode_QuadBuffer];
      if (!aProgram.IsNull())
      {
        return aProgram->IsValid();
      }*/
      aSrcFrag =
          EOL"uniform sampler2D uLeftSampler;"
          EOL"uniform sampler2D uRightSampler;"
          EOL
          EOL"THE_SHADER_IN vec2 TexCoord;"
          EOL
          EOL"void main()"
          EOL"{"
          EOL"  vec4 aColorL = occTexture2D (uLeftSampler,  TexCoord);"
          EOL"  vec4 aColorR = occTexture2D (uRightSampler, TexCoord);"
          EOL"  aColorL.b = 0.0;"
          EOL"  aColorL.g = 0.0;"
          EOL"  aColorR.r = 0.0;"
          EOL"  occFragColor = aColorL + aColorR;"
          EOL"}";
      break;
    }
  }

#if !defined(GL_ES_VERSION_2_0)
  if (myContext->core32 != NULL)
  {
    aProgramSrc->SetHeader ("#version 150");
  }
#endif

  aProgramSrc->AttachShader (Graphic3d_ShaderObject::CreateFromSource (Graphic3d_TOS_VERTEX,   aSrcVert));
  aProgramSrc->AttachShader (Graphic3d_ShaderObject::CreateFromSource (Graphic3d_TOS_FRAGMENT, aSrcFrag));
  TCollection_AsciiString aKey;
  if (!Create (aProgramSrc, aKey, theProgram))
  {
    theProgram = new OpenGl_ShaderProgram(); // just mark as invalid
    return Standard_False;
  }

  myContext->BindProgram (theProgram);
  theProgram->SetSampler (myContext, "uLeftSampler",  0);
  theProgram->SetSampler (myContext, "uRightSampler", 1);
  myContext->BindProgram (NULL);
  return Standard_True;
}

// =======================================================================
// function : bindProgramWithState
// purpose  :
// =======================================================================
Standard_Boolean OpenGl_ShaderManager::bindProgramWithState (const Handle(OpenGl_ShaderProgram)& theProgram)
{
  if (!myContext->BindProgram (theProgram))
  {
    return Standard_False;
  }
  theProgram->ApplyVariables (myContext);

  PushState (theProgram);
  return Standard_True;
}

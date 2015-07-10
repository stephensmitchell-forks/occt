// Created on: 2014-12-10
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

#include <OpenGl_View.hxx>
#include <OpenGl_Volume.hxx>
#include <OpenGl_ShaderManager.hxx>
#include <Graphic3d_TextureParams.hxx>
#include <Graphic3d_DenseVolumeData.hxx>
#include <Graphic3d_RgbaTransferFunction.hxx>

// Use this macro to output debug info
#define VOLUME_RENDER_PRINT_INFO

// =======================================================================
// function : OpenGl_Volume
// purpose  :
// =======================================================================
OpenGl_Volume::OpenGl_Volume (const Handle(Graphic3d_Volume)& theVolume)
: OpenGl_Element()
{
  // Set invalid states
  myVolumeDataState = 0;
  myTransFunctState = 0;

  Standard_ASSERT_RAISE (!theVolume.IsNull(),
    "Error: Graphic 3D volume object is NULL!");

  myVolumeSource = theVolume;

  srand (static_cast<unsigned int> (time (NULL)));
}

// =======================================================================
// function : ~OpenGl_Volume
// purpose  :
// =======================================================================
OpenGl_Volume::~OpenGl_Volume()
{
  //
}

// =======================================================================
// function : Release
// purpose  :
// =======================================================================
void OpenGl_Volume::Release (OpenGl_Context* theContext)
{
  if (!myVolumeDataTex.IsNull())
  {
    myVolumeDataTex->Release (theContext);
  }

  if (!myTransFunctTex.IsNull())
  {
    myTransFunctTex->Release (theContext);
  }

  if (myDummyQuad.IsValid())
  {
    myDummyQuad.Release (theContext);
  }
}

// =======================================================================
// function : BuildProgram
// purpose  :
// =======================================================================
Standard_Boolean OpenGl_Volume::BuildProgram (const Handle(OpenGl_Context)& theContext) const
{
  if (myIsProgramReady)
  {
    return Standard_True;
  }

  myIsProgramReady = Standard_True;

  if (myVolumeProgGL.IsNull())
  {
    TCollection_AsciiString aKey;

    myIsProgramReady &= theContext->ShaderManager()->Create (
      myVolumeSource->ShaderProgram(), aKey, myVolumeProgGL);
  }

  if (myIsProgramReady)
  {
    GetUniformLocations (theContext);
  }

  const GLfloat aVertices[] = { -1.f, -1.f,  0.f,
                                -1.f,  1.f,  0.f,
                                 1.f,  1.f,  0.f,
                                 1.f,  1.f,  0.f,
                                 1.f, -1.f,  0.f,
                                -1.f, -1.f,  0.f };

  myDummyQuad.Init (theContext, 3, 6, aVertices);

  return myIsProgramReady = Standard_True;
}

// =======================================================================
// function : GetUniformLocations
// purpose  :
// =======================================================================
void OpenGl_Volume::GetUniformLocations (const Handle(OpenGl_Context)& theContext) const
{
  theContext->BindProgram (myVolumeProgGL);

  myLocation.CameraOriginLT = myVolumeProgGL->GetUniformLocation (theContext, "uOriginLT");
  myLocation.CameraOriginLB = myVolumeProgGL->GetUniformLocation (theContext, "uOriginLB");
  myLocation.CameraOriginRT = myVolumeProgGL->GetUniformLocation (theContext, "uOriginRT");
  myLocation.CameraOriginRB = myVolumeProgGL->GetUniformLocation (theContext, "uOriginRB");
  myLocation.CameraDirectLT = myVolumeProgGL->GetUniformLocation (theContext, "uDirectLT");
  myLocation.CameraDirectLB = myVolumeProgGL->GetUniformLocation (theContext, "uDirectLB");
  myLocation.CameraDirectRT = myVolumeProgGL->GetUniformLocation (theContext, "uDirectRT");
  myLocation.CameraDirectRB = myVolumeProgGL->GetUniformLocation (theContext, "uDirectRB");

  myLocation.NumOfSamples  = myVolumeProgGL->GetUniformLocation (theContext, "uNumOfSamples");
  myLocation.TriCubFilter  = myVolumeProgGL->GetUniformLocation (theContext, "uTriCubFilter");
  myLocation.ApplyShading  = myVolumeProgGL->GetUniformLocation (theContext, "uApplyShading");
  myLocation.TraceShadows  = myVolumeProgGL->GetUniformLocation (theContext, "uTraceShadows");
  myLocation.RandomShift   = myVolumeProgGL->GetUniformLocation (theContext, "uRandomShift");
  myLocation.VolTexelSize  = myVolumeProgGL->GetUniformLocation (theContext, "uVolTexelSize");
  myLocation.NormMinValue  = myVolumeProgGL->GetUniformLocation (theContext, "uNormMinValue");
  myLocation.NormMaxValue  = myVolumeProgGL->GetUniformLocation (theContext, "uNormMaxValue");
  myLocation.OpacityScale  = myVolumeProgGL->GetUniformLocation (theContext, "uOpacityScale");
  myLocation.BoxCornerMin  = myVolumeProgGL->GetUniformLocation (theContext, "uBoxCornerMin");
  myLocation.BoxCornerMax  = myVolumeProgGL->GetUniformLocation (theContext, "uBoxCornerMax");
  myLocation.TransferFunct = myVolumeProgGL->GetUniformLocation (theContext, "uTransferFunct");
  myLocation.VolumeTexture = myVolumeProgGL->GetUniformLocation (theContext, "uVolumeTexture");
  myLocation.RandomSeed    = myVolumeProgGL->GetUniformLocation (theContext, "uRandomSeed");

  myLocation.IsoSurfaceMode = myVolumeProgGL->GetUniformLocation (theContext, "uIsoSurfaceMode");
  myLocation.TraceSliceMode = myVolumeProgGL->GetUniformLocation (theContext, "uTraceSliceMode");

  myLocation.IsoSurfaceLevel = myVolumeProgGL->GetUniformLocation (theContext, "uIsoSurfaceLevel");
  myLocation.TraceSliceLevel = myVolumeProgGL->GetUniformLocation (theContext, "uTraceSliceLevel");
}

// =======================================================================
// function : SetUniformValues
// purpose  :
// =======================================================================
void OpenGl_Volume::SetUniformValues (const Handle(OpenGl_Context)& theContext,
                                      const OpenGl_Vec3*            theOrigins,
                                      const OpenGl_Vec3*            theDirects) const
{
  // Update state of volume and transfer function textures
  if (!UpdateTextures (theContext))
  {
    return;
  }

  const Handle(Graphic3d_VolumeData)& aVolumeData = myVolumeSource->VolumeData();

  // Set bounding box
  myVolumeProgGL->SetUniform (theContext,
    myLocation.BoxCornerMin, aVolumeData->Bounds().CornerMin().Convert<GLfloat>());
  myVolumeProgGL->SetUniform (theContext,
    myLocation.BoxCornerMax, aVolumeData->Bounds().CornerMax().Convert<GLfloat>());

  // Set texture samplers
  myVolumeProgGL->SetUniform (theContext, myLocation.VolumeTexture, 0);
  myVolumeProgGL->SetUniform (theContext, myLocation.TransferFunct, 1);

  // Set rendering options
  myVolumeProgGL->SetUniform (theContext, myLocation.NumOfSamples, myVolumeSource->NumOfSamples());
  myVolumeProgGL->SetUniform (theContext, myLocation.OpacityScale, myVolumeSource->OpacityScale());
  myVolumeProgGL->SetUniform (theContext, myLocation.NormMinValue, static_cast<Standard_ShortReal> (myNormMinValue));
  myVolumeProgGL->SetUniform (theContext, myLocation.NormMaxValue, static_cast<Standard_ShortReal> (myNormMaxValue));
  myVolumeProgGL->SetUniform (theContext, myLocation.TriCubFilter, static_cast<GLint> (myVolumeSource->TriCubFilter()));
  myVolumeProgGL->SetUniform (theContext, myLocation.ApplyShading, static_cast<GLint> (myVolumeSource->ApplyShading()));
  myVolumeProgGL->SetUniform (theContext, myLocation.TraceShadows, static_cast<GLint> (myVolumeSource->TraceShadows()));
  myVolumeProgGL->SetUniform (theContext, myLocation.RandomShift,  static_cast<GLint> (myVolumeSource->RandomShift()));

  myVolumeProgGL->SetUniform (theContext, myLocation.IsoSurfaceMode, static_cast<GLint> (myVolumeSource->IsoSurfaceMode()));
  //myVolumeProgGL->SetUniform (theContext, myLocation.TraceSliceMode, static_cast<GLint> (myVolumeSource->SliceMode()));

  myVolumeProgGL->SetUniform (theContext, myLocation.IsoSurfaceLevel, myVolumeSource->IsoSurfaceLevel());
  myVolumeProgGL->SetUniform (theContext, myLocation.TraceSliceLevel,  myVolumeSource->SliceLevel());

  // Set random seed for to reduce graphic artifacts.
  const OpenGl_Vec4 aRandomSeed = OpenGl_Vec4 (static_cast<GLfloat> (rand()),
                                               static_cast<GLfloat> (rand()),
                                               static_cast<GLfloat> (rand()),
                                               static_cast<GLfloat> (rand()));

  myVolumeProgGL->SetUniform (theContext, myLocation.RandomSeed, aRandomSeed);

  // Set 3D size of single voxel
  const BVH_Vec3f aTexelSize = BVH_Vec3f (1.f / aVolumeData->NbVoxelX(),
                                          1.f / aVolumeData->NbVoxelX(),
                                          1.f / aVolumeData->NbVoxelZ());

  myVolumeProgGL->SetUniform (theContext, myLocation.VolTexelSize, aTexelSize);

  // Set viewing position and orientation
  myVolumeProgGL->SetUniform (theContext, myLocation.CameraOriginLB, theOrigins[0]);
  myVolumeProgGL->SetUniform (theContext, myLocation.CameraOriginRB, theOrigins[1]);
  myVolumeProgGL->SetUniform (theContext, myLocation.CameraOriginLT, theOrigins[2]);
  myVolumeProgGL->SetUniform (theContext, myLocation.CameraOriginRT, theOrigins[3]);
  myVolumeProgGL->SetUniform (theContext, myLocation.CameraDirectLB, theDirects[0]);
  myVolumeProgGL->SetUniform (theContext, myLocation.CameraDirectRB, theDirects[1]);
  myVolumeProgGL->SetUniform (theContext, myLocation.CameraDirectLT, theDirects[2]);
  myVolumeProgGL->SetUniform (theContext, myLocation.CameraDirectRT, theDirects[3]);
}

namespace
{
  struct OpenGl_VolumeDataWrapper
  {
    //! Returns OpenGL texture format.
    virtual OpenGl_TextureFormat Format() const = 0;

    //! Returns raw volumetric data.
    virtual const GLvoid* Voxels() const = 0;
  };

  template<class T>
  class OpenGl_DenseVolumeDataWrapper : public OpenGl_VolumeDataWrapper
  {
  private:

    const Graphic3d_DenseVolumeData<T>* myData;

  public:

    //! Creates new wrapper for dense volumetric data.
    OpenGl_DenseVolumeDataWrapper (const Graphic3d_VolumeData& theData)
    {
      myData = static_cast<const Graphic3d_DenseVolumeData<T>* > (&theData);
    }

    //! Returns OpenGL texture format.
    virtual OpenGl_TextureFormat Format() const
    {
      return OpenGl_TextureFormat::Create<T, 1>();
    }

    //! Returns raw volumetric data.
    virtual const GLvoid* Voxels() const
    {
      return &myData->Data()->First();
    }
  };
}

// =======================================================================
// function : UpdateTextures
// purpose  :
// =======================================================================
Standard_Boolean OpenGl_Volume::UpdateTextures (const Handle(OpenGl_Context)& theContext) const
{
  const Handle(Graphic3d_VolumeData)& aVolumeData = myVolumeSource->VolumeData();

  if (aVolumeData.IsNull())
  {
    return Standard_False;
  }

  Handle(Graphic3d_TextureParams) aParams = new Graphic3d_TextureParams;

  // Set linear filtering of TF and volume data
  aParams->SetFilter (Graphic3d_TOTF_TRILINEAR);

  if (myTransFunctTex.IsNull())
    myTransFunctTex = new OpenGl_Texture (aParams);

  if (myVolumeDataTex.IsNull())
    myVolumeDataTex = new OpenGl_Texture (aParams);

  if (myVolumeDataState != aVolumeData->State())
  {
#ifdef VOLUME_RENDER_PRINT_INFO
    std::cout << "Info: Updating 3D volume texture" << std::endl;
#endif

    if (aVolumeData->IsDense())
    {
      std::auto_ptr<OpenGl_VolumeDataWrapper> aWrapper;

      if (typeid (*aVolumeData) == typeid (Graphic3d_DenseVolumeData<GLfloat>))
      {
        aWrapper.reset (new OpenGl_DenseVolumeDataWrapper<GLfloat> (*aVolumeData));
      }
      else if (typeid (*aVolumeData) == typeid (Graphic3d_DenseVolumeData<GLubyte>))
      {
        aWrapper.reset (new OpenGl_DenseVolumeDataWrapper<GLubyte> (*aVolumeData));
      }
      else if (typeid (*aVolumeData) == typeid (Graphic3d_DenseVolumeData<GLushort>))
      {
        aWrapper.reset (new OpenGl_DenseVolumeDataWrapper<GLushort> (*aVolumeData));
      }

      if (aWrapper.get() == NULL)
      {
        return Standard_False;
      }

      Standard_Boolean aResult = myVolumeDataTex->Init3D (theContext,
                                                          aVolumeData->NbVoxelX(),
                                                          aVolumeData->NbVoxelY(),
                                                          aVolumeData->NbVoxelZ(),
                                                          aWrapper->Format(),
                                                          aWrapper->Voxels());
      if (!aResult)
      {
        return Standard_False;
      }
    }
    else
    {
      // Sparse data are not supported
    }

    myVolumeDataState = aVolumeData->State();
  }

  if (myTransFunctState != myVolumeSource->TransferFunction()->State())
  {
#ifdef VOLUME_RENDER_PRINT_INFO
    std::cout << "Info: Updating transfer function texture" << std::endl;
#endif

    Handle(Image_PixMap) anImage = myVolumeSource->TransferFunction()->ImageTable (myNormMinValue,
                                                                                   myNormMaxValue);

    if (!myTransFunctTex->Init (theContext, *anImage, Graphic3d_TOT_1D))
    {
      return Standard_False;
    }

    myTransFunctState = myVolumeSource->TransferFunction()->State();
  }

  myVolumeDataTex->Bind (theContext, GL_TEXTURE0);
  myTransFunctTex->Bind (theContext, GL_TEXTURE1);

  return Standard_True;
}

// =======================================================================
// function : UpdateCamera
// purpose  :
// =======================================================================
void OpenGl_Volume::UpdateCamera (const OpenGl_Mat4& theOrientation,
                                  const OpenGl_Mat4& theViewMapping,
                                  OpenGl_Vec3        theOrigins[4],
                                  OpenGl_Vec3        theDirects[4]) const
{
  OpenGl_Mat4 theUnviewMat;

  // compute inverse model-view-projection matrix
  (theViewMapping * theOrientation).Inverted (theUnviewMat);

  Standard_Integer aOriginIndex = 0;
  Standard_Integer aDirectIndex = 0;

  for (Standard_Integer aY = -1; aY <= 1; aY += 2)
  {
    for (Standard_Integer aX = -1; aX <= 1; aX += 2)
    {
      OpenGl_Vec4 aOrigin (GLfloat(aX),
                           GLfloat(aY),
                          -1.0f,
                           1.0f);

      aOrigin = theUnviewMat * aOrigin;

      aOrigin.x() = aOrigin.x() / aOrigin.w();
      aOrigin.y() = aOrigin.y() / aOrigin.w();
      aOrigin.z() = aOrigin.z() / aOrigin.w();

      OpenGl_Vec4 aDirect (GLfloat(aX),
                           GLfloat(aY),
                           1.0f,
                           1.0f);

      aDirect = theUnviewMat * aDirect;

      aDirect.x() = aDirect.x() / aDirect.w();
      aDirect.y() = aDirect.y() / aDirect.w();
      aDirect.z() = aDirect.z() / aDirect.w();

      aDirect = aDirect - aOrigin;

      GLdouble aInvLen = 1.0 / sqrt (aDirect.x() * aDirect.x() +
                                     aDirect.y() * aDirect.y() +
                                     aDirect.z() * aDirect.z());

      theOrigins[aOriginIndex++] = OpenGl_Vec3 (static_cast<GLfloat> (aOrigin.x()),
                                                static_cast<GLfloat> (aOrigin.y()),
                                                static_cast<GLfloat> (aOrigin.z()));

      theDirects[aDirectIndex++] = OpenGl_Vec3 (static_cast<GLfloat> (aDirect.x() * aInvLen),
                                                static_cast<GLfloat> (aDirect.y() * aInvLen),
                                                static_cast<GLfloat> (aDirect.z() * aInvLen));
    }
  }
}

// =======================================================================
// function : Render
// purpose  :
// =======================================================================
void OpenGl_Volume::Render (const Handle(OpenGl_Workspace)& theWorkspace) const
{
  Standard_Boolean anIsGL20Supproted =
    atof (reinterpret_cast<const char *> (glGetString(GL_VERSION))) >= 2.0;

  if (!anIsGL20Supproted)
  {
    std::cerr << "Error: The current OpenGL version doesn't support Volume Rendering." 
      << std::endl << "Info: OpenGL version is must be not lower 2.0." << std::endl;
    return ;
  }

  const Handle(OpenGl_Context)& aContext = theWorkspace->GetGlContext();

  if (!myIsProgramReady)
  {
    BuildProgram (aContext);
  }

  const Standard_Integer aSizeX = theWorkspace->Width();
  const Standard_Integer aSizeY = theWorkspace->Height();

  glDisable (GL_DEPTH_TEST);

  myDummyQuad.Bind (aContext);

  if (myIsProgramReady)
  {
    aContext->BindProgram (myVolumeProgGL);

    OpenGl_Mat4 aOrientationMatrix = aContext->WorldViewState.Current();
    OpenGl_Mat4 aViewMappingMatrix = aContext->ProjectionState.Current();

    OpenGl_Vec3 aOrigins[4];
    OpenGl_Vec3 aDirects[4];

    UpdateCamera (aOrientationMatrix,
                  aViewMappingMatrix,
                  aOrigins,
                  aDirects);

    SetUniformValues (aContext,
                      aOrigins,
                      aDirects);

    myVolumeProgGL->SetUniform (theWorkspace->GetGlContext(), myLocation.TraceSliceMode, myVolumeSource->SliceMode() ?  2 : 0);

    aContext->ModelWorldState.Push();
    aContext->ProjectionState.Push();

    aContext->ModelWorldState.SetIdentity();
    aContext->ProjectionState.SetIdentity();

    aContext->ApplyModelWorldMatrix();
    aContext->ApplyProjectionMatrix();

    glEnable (GL_BLEND);
    glBlendFunc (GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

    myDummyQuad.BindVertexAttrib (aContext, Graphic3d_TOA_POS);
    {
      aContext->core20fwd->glVertexAttribPointer (
        Graphic3d_TOA_POS, 3, GL_FLOAT, GL_FALSE, 0, NULL);

      aContext->core15fwd->glDrawArrays (GL_TRIANGLES, 0, 6);
    }
    myDummyQuad.UnbindAttribute (aContext, Graphic3d_TOA_POS);

    if (myVolumeSource->SliceMode())
    {
      glViewport (16,
                  16,
                  Min (250, aSizeX - 16),
                  Min (250, aSizeY - 16));

      myVolumeProgGL->SetUniform (theWorkspace->GetGlContext(), myLocation.TraceSliceMode, 1);

      myDummyQuad.BindVertexAttrib (aContext, Graphic3d_TOA_POS);
      {
        aContext->core20fwd->glVertexAttribPointer (
          Graphic3d_TOA_POS, 3, GL_FLOAT, GL_FALSE, 0, NULL);

        aContext->core15fwd->glDrawArrays (GL_TRIANGLES, 0, 6);
      }
      myDummyQuad.UnbindAttribute (aContext, Graphic3d_TOA_POS);

      glViewport (0, 0, aSizeX, aSizeY);
    }

    glDisable (GL_BLEND);

    aContext->ModelWorldState.Pop();
    aContext->ProjectionState.Pop();

    aContext->ApplyModelWorldMatrix();
    aContext->ApplyProjectionMatrix();

    aContext->BindProgram (NULL);
  }

  myDummyQuad.Unbind (aContext);
}

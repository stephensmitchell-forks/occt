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

#ifndef OpenGl_Volume_Header
#define OpenGl_Volume_Header

#include <OpenGl_Texture.hxx>
#include <OpenGl_Workspace.hxx>
#include <OpenGl_VertexBuffer.hxx>
#include <OpenGl_ShaderProgram.hxx>

#include <Graphic3d_Volume.hxx>
#include <Graphic3d_ArrayOfTriangles.hxx>

//! Tool object for visualizing volumetric data.
class OpenGl_Volume : public OpenGl_Element
{
public:

  //! Creates new 3D volume from the base object.
  Standard_EXPORT OpenGl_Volume (const Handle(Graphic3d_Volume)& theVolume);

  //! Releases resources of 3D volume.
  Standard_EXPORT virtual ~OpenGl_Volume();

public:

  //! Releases GPU resources.
  Standard_EXPORT virtual void Release (OpenGl_Context* theContext);

  //! Performs volumetric rendering with direct ray-casting.
  Standard_EXPORT virtual void Render (const Handle(OpenGl_Workspace)& theWorkspace) const;

private:

  //! Retrieves uniform locations from the shader program.
  void GetUniformLocations (const Handle(OpenGl_Context)& theContext) const;

  //! Updates volume data and transfer function textures.
  Standard_Boolean UpdateTextures (const Handle(OpenGl_Context)& theContext) const;

  //! Sets uniform value for active shader program.
  void SetUniformValues (const Handle(OpenGl_Context)& theContext,
                         const OpenGl_Vec3*            theOrigins,
                         const OpenGl_Vec3*            theDirects) const;

  //! Builds shader program for volume ray-casting.
  Standard_Boolean BuildProgram (const Handle(OpenGl_Context)& theContext) const;

  //! Updates viewing parameters used in volume ray-casting.
  void UpdateCamera (const OpenGl_Mat4& theOrientation,
                     const OpenGl_Mat4& theViewMapping,
                     OpenGl_Vec3        theOrigins[4],
                     OpenGl_Vec3        theDirects[4]) const;

protected:

  //! 3D volume source.
  Handle(Graphic3d_Volume) myVolumeSource;

  //! Ready-to-use state of volumetric data.
  mutable Standard_Size myVolumeDataState;

  //! Ready-to-use state of transfer function.
  mutable Standard_Size myTransFunctState;

  //! Is shader program ready?
  mutable Standard_Boolean myIsProgramReady;

  //! OpenGL 3D volumetric data texture.
  mutable Handle(OpenGl_Texture) myVolumeDataTex;

  //! OpenGL 1D transfer function texture.
  mutable Handle(OpenGl_Texture) myTransFunctTex;

  //! Shader program used for volume rendering.
  mutable Handle(OpenGl_ShaderProgram) myVolumeProgGL;

  //! Stores location of uniform variables.
  mutable struct Location
  {
    GLint BoxCornerMin;
    GLint BoxCornerMax;
    GLint NumOfSamples;
    GLint TriCubFilter;
    GLint ApplyShading;
    GLint TraceShadows;
    GLint RandomShift;
    GLint OpacityScale;
    GLint VolTexelSize;
    GLint NormMinValue;
    GLint NormMaxValue;
    GLint TransferFunct;
    GLint VolumeTexture;
    GLint RandomSeed;
    GLint CameraOriginLT;
    GLint CameraOriginLB;
    GLint CameraOriginRT;
    GLint CameraOriginRB;
    GLint CameraDirectLT;
    GLint CameraDirectLB;
    GLint CameraDirectRT;
    GLint CameraDirectRB;
    GLint IsoSurfaceMode;
    GLint TraceSliceMode;
    GLint IsoSurfaceLevel;
    GLint TraceSliceLevel;

    Location()
    : BoxCornerMin   (-1),
      BoxCornerMax   (-1),
      NumOfSamples   (-1),
      TriCubFilter   (-1),
      ApplyShading   (-1),
      TraceShadows   (-1),
      RandomShift    (-1),
      OpacityScale   (-1),
      VolTexelSize   (-1),
      TransferFunct  (-1),
      VolumeTexture  (-1),
      RandomSeed     (-1),
      CameraOriginLT (-1),
      CameraOriginLB (-1),
      CameraOriginRT (-1),
      CameraOriginRB (-1),
      CameraDirectLT (-1),
      CameraDirectLB (-1),
      CameraDirectRT (-1),
      CameraDirectRB (-1),
      IsoSurfaceMode (-1),
      TraceSliceMode (-1),
      IsoSurfaceLevel (-1),
      TraceSliceLevel (-1)
    {
      //
    }

  } myLocation;

  //! Screen-size quad for volume ray-casting.
  mutable OpenGl_VertexBuffer myDummyQuad;

  //! Normalized min value of the scalar field.
  mutable Standard_Real myNormMinValue;

  //! Normalized max value of the scalar field.
  mutable Standard_Real myNormMaxValue;

public:

  DEFINE_STANDARD_ALLOC
};

#endif // OpenGl_Volume_Header

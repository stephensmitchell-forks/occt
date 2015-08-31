// Created on: 2014-12-12
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

#ifndef _Graphic3d_Volume_HeaderFile
#define _Graphic3d_Volume_HeaderFile

#include <Graphic3d_VolumeData.hxx>
#include <Graphic3d_ShaderProgram.hxx>
#include <Graphic3d_TransferFunction.hxx>

//! Contains definition of 3D volume object.
class Graphic3d_Volume : public Standard_Transient
{
public:

  //! Creates uninitialized volume object.
  Standard_EXPORT Graphic3d_Volume();

  //! Release resources of volume object.
  Standard_EXPORT virtual ~Graphic3d_Volume();

public:

  //! Returns volumetric data attached to the volume.
  const Handle(Graphic3d_VolumeData)& VolumeData() const
  {
    return myVolumeData;
  }

  //! Attaches the given volumetric data to the volume.
  void AttachVolumeData (const Handle(Graphic3d_VolumeData)& theData)
  {
    myVolumeData = theData;
  }

  //! Returns transfer function attached to the volume.
  const Handle(Graphic3d_TransferFunction)& TransferFunction() const
  {
    return myTransFunction;
  }

  //! Attaches the given transfer function to the volume.
  void AttachTransferFunction (const Handle(Graphic3d_TransferFunction)& theTransFunc)
  {
    myTransFunction = theTransFunc;
  }

  //! Returns maximum number of samples per ray.
  Standard_Integer NumOfSamples() const
  {
    return myNumOfSamples;
  }

  //! Sets maximum number of samples per ray.
  void SetNumOfSamples (const Standard_Integer theNbOfSamples)
  {
    myNumOfSamples = theNbOfSamples;
  }

  //! Returns scaling factor for volume opacity.
  Standard_ShortReal OpacityScale() const
  {
    return myOpacityScale;
  }

  //! Sets scaling factor for volume opacity.
  void SetOpacityScale (const Standard_ShortReal theOpacityScale)
  {
    myOpacityScale = theOpacityScale;
  }

  //! Checks if tri-cubic volume filtering is enabled.
  Standard_Boolean TriCubFilter() const
  {
    return myTriCubFilter;
  }

  //! Enables/disables tri-cubic volume filtering.
  void SetTriCubFilter (const Standard_Boolean theIsEnabled)
  {
    myTriCubFilter = theIsEnabled;
  }

  //! Checks if volume shading is enabled.
  Standard_Boolean ApplyShading() const
  {
    return myApplyShading;
  }

  //! Enables/disables volume shading.
  void SetApplyShading (const Standard_Boolean theIsEnabled)
  {
    myApplyShading = theIsEnabled;
  }

  //! Checks if tracing of shadow rays is enabled.
  Standard_Boolean TraceShadows() const
  {
    return myTraceShadows;
  }

  //! Enables/disables tracing of shadow rays.
  void SetTraceShadows (const Standard_Boolean theIsEnabled)
  {
    myTraceShadows = theIsEnabled;
  }

  //! Checks if random shift of rays is enabled.
  Standard_Boolean RandomShift() const
  {
    return myRandomShift;
  }

  //! Enables/disables random shift of rays.
  void SetRandomShift (const Standard_Boolean theIsEnabled)
  {
    myRandomShift = theIsEnabled;
  }

  //! Checks if iso-surface rendering is enabled.
  Standard_Boolean IsoSurfaceMode() const
  {
    return myIsoSurfaceMode;
  }

  //! Enables/disables iso-surface rendering.
  void SetIsoSurfaceMode (const Standard_Boolean theIsEnabled)
  {
    myIsoSurfaceMode = theIsEnabled;
  }

  //! Checks if 2D slice rendering is enabled.
  Standard_Boolean SliceMode() const
  {
    return mySliceMode;
  }

  //! Enables/disables 2D slice rendering.
  void SetSliceMode (const Standard_Boolean theIsEnabled)
  {
    mySliceMode = theIsEnabled;
  }

  //! Normalized [0, 1].
  Standard_ShortReal IsoSurfaceLevel() const
  {
    return myIsoSurfaceLevel;
  }

  //! Normalized [0, 1].
  void SetIsoSurfaceLevel (const Standard_ShortReal theValue)
  {
    myIsoSurfaceLevel = theValue;
  }

  //! Normalized [0, 1].
  Standard_ShortReal SliceLevel() const
  {
    return mySliceLevel;
  }

  //! Normalized [0, 1].
  void SetSliceLevel (const Standard_ShortReal theValue)
  {
    mySliceLevel = theValue;
  }

  //! Returns shader program used for volume visualization.
  Handle(Graphic3d_ShaderProgram) ShaderProgram()
  {
    return myShaderProgram;
  }

protected:

  //! Initializes volume rendering options.
  Standard_EXPORT virtual Standard_Boolean Init();

protected:

  //! Maximum number of samples per ray.
  Standard_Integer myNumOfSamples;

  //! Scaling factor for volume opacity.
  Standard_ShortReal myOpacityScale;

  //! Enables/disables volume shading.
  Standard_Boolean myApplyShading;

  //! Enables/disables tri-cubic filtering.
  Standard_Boolean myTriCubFilter;

  //! Enables/disables tracing of shadow rays.
  Standard_Boolean myTraceShadows;

  //! Enables/disables random shift of rays.
  Standard_Boolean myRandomShift;

  //! Enables/disables iso-surface rendering.
  Standard_Boolean myIsoSurfaceMode;

  //! Enables/disables 2D slice rendering.
  Standard_Boolean mySliceMode;

  Standard_ShortReal myIsoSurfaceLevel;
  Standard_ShortReal mySliceLevel;

protected:

  // Volumetric data for visualization.
  Handle(Graphic3d_VolumeData) myVolumeData;

  //! Shader program used for volume visualization.
  Handle(Graphic3d_ShaderProgram) myShaderProgram;

  //! Transfer function used for colorizing volume.
  Handle(Graphic3d_TransferFunction) myTransFunction;

public:

  DEFINE_STANDARD_RTTI (Graphic3d_Volume, Standard_Transient)

};

DEFINE_STANDARD_HANDLE (Graphic3d_Volume, Standard_Transient)

#endif // _Graphic3d_Volume_HeaderFile

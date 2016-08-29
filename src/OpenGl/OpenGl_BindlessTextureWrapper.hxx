// Created on: 2016-08-05
// Created by: Ilya SEVRIKOV
// Copyright (c) 2016 OPEN CASCADE SAS
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

#ifndef _OpenGl_BindlessTextureWrapper_HeaderFile
#define _OpenGl_BindlessTextureWrapper_HeaderFile

#include <OpenGl_Context.hxx>

//! ARB texture bindless wrapper.
//!
//! To access texture or image resources using handles, the handles must first be made resident.
//! Accessing a texture or image by handle without first making it resident can result in undefined results,
//! including program termination.
class OpenGl_BindlessTextureWrapper : public Standard_Transient
{
public:
  OpenGl_BindlessTextureWrapper (const Handle(OpenGl_Context)& theContext)
  : myContext (theContext)
  {
    //
  }

  //! Create a texture handle using the current non-sampler state from the texture named <texture>
  //! and the sampler state from the sampler object <sampler>.
  //! Returns a 64-bit unsigned integer handle.
  //! The error INVALID_VALUE is generated if <texture> is zero or is not the name of an existing texture object
  //! or if <sampler> is zero or is not the name of an existing sampler object.
  //! The error INVALID_OPERATION is generated if the texture object <texture> is not complete (section 3.9.14).
  //! If an error occurs, a handle of zero is returned.
  //!
  //! The error INVALID_OPERATION is generated if the border color (taken from the <sampler>)
  //! is not one of the following allowed values. If the texture's base internal format is signed
  //! or unsigned integer, allowed values are (0,0,0,0), (0,0,0,1), (1,1,1,0), and (1,1,1,1).
  //! If the base internal format is not integer, allowed values are (0.0, 0.0, 0.0, 0.0),
  //! (0.0, 0.0, 0.0, 1.0), (1.0, 1.0, 1.0, 0.0), and (1.0, 1.0, 1.0, 1.0).
  //!
  //! The handle for each texture or texture/sampler pair is unique;
  //! the same handle will if GetTextureSamplerHandleARB is called multiple times for the same texture/sampler pair.
  Standard_EXPORT GLuint64 GetTextureSamplerHandle (const GLuint theTextureId, const GLuint theSamplerId);

  //! Makes a texture handle accessible to shaders for texture mapping operations.
  //!
  //! While the texture handle is resident, it may be used in texture mapping operations.
  //! If a shader attempts to perform a texture mapping operation using a handle that is not resident,
  //! the results of that operation are undefined and may lead to application termination.
  //! When a texture handle is resident, the texture it references is also considered resident for the
  //! purposes of the AreTexturesResident command. The error INVALID_OPERATION is generated
  //! if <handle> is not a valid texture handle, or if <handle> is already resident in the current GL context.
  Standard_EXPORT GLvoid MakeTextureHandleResident (const GLuint64 theHandle);

  //! Makes a texture handle inaccessible to shaders for texture mapping operations.
  //!
  //! The error INVALID_OPERATION is generated if <handle> is not a valid
  //! texture handle, or if <handle> is not resident in the current GL context.
  Standard_EXPORT GLvoid MakeTextureHandleNonResident (const GLuint64 theHandle);

  //!
  Standard_EXPORT GLboolean IsTextureHandleResident (const GLuint64 theHandle);

protected:
  Handle(OpenGl_Context) myContext;

public:
  DEFINE_STANDARD_RTTIEXT (OpenGl_BindlessTextureWrapper, Standard_Transient)
};

DEFINE_STANDARD_HANDLE (OpenGl_BindlessTextureWrapper, Standard_Transient)

#endif //_OpenGl_BindlessTextureWrapper_HeaderFile

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

#include <OpenGl_ArbTexBindless.hxx>
#include <OpenGl_BindlessTextureWrapper.hxx>

IMPLEMENT_STANDARD_RTTIEXT (OpenGl_BindlessTextureWrapper, Standard_Transient)

// =======================================================================
// function : GetTextureSamplerHandle
// purpose  :
// =======================================================================
GLuint64 OpenGl_BindlessTextureWrapper::GetTextureSamplerHandle (const GLuint theTextureId, const GLuint theSamplerId)
{
  GLuint64 aHandle = myContext->arbTexBindless->glGetTextureSamplerHandleARB (theTextureId, theSamplerId);

#ifdef OCCT_DEBUG
  GLenum anError = glGetError();

  if (anError != GL_NO_ERROR)
  {
    Message::DefaultMessenger()->Send ("Error: Can not get texture sampler handle.", Message_Fail);
  }
#endif

  return aHandle;
}

// =======================================================================
// function : MakeTextureHandleResident
// purpose  :
// =======================================================================
GLvoid OpenGl_BindlessTextureWrapper::MakeTextureHandleResident (const GLuint64 theHandle)
{
  myContext->arbTexBindless->glMakeTextureHandleResidentARB (theHandle);

#ifdef OCCT_DEBUG
  GLenum anError = glGetError();

  if (anError != GL_NO_ERROR)
  {
    Message::DefaultMessenger()->Send ("Error: Can not make texture handle as resident.", Message_Fail);
  }
#endif
}

// =======================================================================
// function : MakeTextureHandleNonResident
// purpose  :
// =======================================================================
GLvoid OpenGl_BindlessTextureWrapper::MakeTextureHandleNonResident (const GLuint64 theHandle)
{
  myContext->arbTexBindless->glMakeTextureHandleNonResidentARB (theHandle);

#ifdef OCCT_DEBUG
  GLenum anError = glGetError();

  if (anError != GL_NO_ERROR)
  {
    Message::DefaultMessenger()->Send("Error: Can not make texture handle as non-resident.", Message_Fail);
  }
#endif
}

// =======================================================================
// function : IsTextureHandleResident
// purpose  :
// =======================================================================
GLboolean OpenGl_BindlessTextureWrapper::IsTextureHandleResident (const GLuint64 theHandle)
{
  return myContext->arbTexBindless->glIsTextureHandleResidentARB (theHandle);
}

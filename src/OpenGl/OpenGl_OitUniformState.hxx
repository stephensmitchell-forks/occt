// Created on: 2017-01-26
// Created by: Anton POLETAEV
// Copyright (c) 2017 OPEN CASCADE SAS
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

#ifndef _OpenGl_OitUniformState_HeaderFile
#define _OpenGl_OitUniformState_HeaderFile

#include <OpenGl_ShaderStates.hxx>

//! Defines generic state of order-independent transparency rendering properties.
class OpenGl_OitUniformState : public OpenGl_StateInterface
{
public:

  //! Creates new uniform state.
  OpenGl_OitUniformState() : myToEnableWrite (false), myDepthWeight (0.5f) {}

  //! Sets the uniform values.
  //! @param theToEnableWrite [in] flag indicating whether color and coverage
  //!  values for OIT processing should be written by shader program.
  //! @param theDepthWeight [in] scalar factor [0-1] defining influence of depth
  //!  component of a fragment to its final coverage coefficient.
  void Set (const bool  theToEnableWrite,
            const float theDepthWeight)
  {
    myToEnableWrite = theToEnableWrite;
    myDepthWeight   = Max (0.f, Min (1.f, theDepthWeight));
  }

  //! Returns flag indicating whether writing of output for OIT processing
  //! should be enabled/disabled.
  bool ToEnableWrite() const { return myToEnableWrite; }

  //! Returns factor defining influence of depth component of a fragment
  //! to its final coverage coefficient.
  float DepthWeight() const { return myDepthWeight; }

private:

  bool  myToEnableWrite; //!< writing color and coverage.
  float myDepthWeight;   //!< factor of depth influence to coverage.
};

#endif // _OpenGl_OitUniformState_HeaderFile

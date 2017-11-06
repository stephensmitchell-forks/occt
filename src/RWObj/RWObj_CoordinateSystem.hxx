// Author: Kirill Gavrilov
// Copyright: Open CASCADE 2015-2017
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

#ifndef _RWObj_CoordinateSystem_HeaderFile
#define _RWObj_CoordinateSystem_HeaderFile

//! Coordinate system definition for OBJ reader.
enum RWObj_CoordinateSystem
{
  RWObj_CoordinateSystem_posYfwd_posZup = 0, //!< +YForward+Zup
  RWObj_CoordinateSystem_negZfwd_posYup,     //!< -Zforward+Yup

  RWObj_CoordinateSystem_OCCT    = RWObj_CoordinateSystem_posYfwd_posZup, //!< coordinate system used by OCCT,  +YForward+Zup
  RWObj_CoordinateSystem_Blender = RWObj_CoordinateSystem_OCCT,           //!< coordinate system used by Blender (same as OCCT)
  RWObj_CoordinateSystem_Unity   = RWObj_CoordinateSystem_negZfwd_posYup  //!< coordinate system used by Unity, -Zforward+Yup
};
enum
{
  RWObj_CoordinateSystem_LOWER = RWObj_CoordinateSystem_posYfwd_posZup,
  RWObj_CoordinateSystem_UPPER = RWObj_CoordinateSystem_negZfwd_posYup,
  RWObj_CoordinateSystem_NB    = RWObj_CoordinateSystem_UPPER + 1
};

#endif // _RWObj_CoordinateSystem_HeaderFile

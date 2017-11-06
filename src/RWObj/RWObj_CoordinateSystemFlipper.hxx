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

#ifndef _RWObj_CoordinateSystemFlipper_HeaderFile
#define _RWObj_CoordinateSystemFlipper_HeaderFile

#include <RWObj_CoordinateSystem.hxx>

#include <gp.hxx>
#include <gp_XYZ.hxx>
#include <Graphic3d_Vec3.hxx>

//! Coordinate system flipper for OBJ reader.
class RWObj_CoordinateSystemFlipper
{
public:

  //! Empty constructor.
  RWObj_CoordinateSystemFlipper()
  : mySign_lf (1, 1, 1),
    myTrsf_lf (1, 1, 1),
    myXyzMap  (0, 1, 2),
    mySign_f  (1, 1, 1),
    myUnitFactor (1),
    myHasModif(false)
  {
    //
  }

  //! Initialize transformation for importing external data.
  void InitToOcct (RWObj_CoordinateSystem theSysFrom,
                   Standard_Real theUnitFactor)
  {
    Init (theSysFrom, RWObj_CoordinateSystem_OCCT, theUnitFactor);
  }

  //! Initialize transformation for exporting into external format.
  void InitFromOcct (RWObj_CoordinateSystem theSysTo,
                     Standard_Real theUnitFactor)
  {
    Init (RWObj_CoordinateSystem_OCCT, theSysTo, theUnitFactor);
  }

  //! Initialize transformation.
  void Init (RWObj_CoordinateSystem theSysFrom,
             RWObj_CoordinateSystem theSysTo,
             Standard_Real theUnitFactor)
  {
    myUnitFactor = theUnitFactor;
    mySign_lf.SetCoord (1, 1, 1);
    myXyzMap .SetValues(0, 1, 2);
    myHasModif = Abs(myUnitFactor - 1.0) > gp::Resolution();
    if (theSysFrom != theSysTo)
    {
      switch (theSysFrom)
      {
        case RWObj_CoordinateSystem_posYfwd_posZup:
        {
          switch (theSysTo)
          {
            case RWObj_CoordinateSystem_posYfwd_posZup:
            {
              break;
            }
            case RWObj_CoordinateSystem_negZfwd_posYup:
            {
              myHasModif = true;
              myXyzMap .SetValues(0, 2,  1);
              mySign_lf.SetCoord (1, 1, -1);
              break;
            }
          }
          break;
        }
        case RWObj_CoordinateSystem_negZfwd_posYup:
        {
          switch (theSysTo)
          {
            case RWObj_CoordinateSystem_posYfwd_posZup:
            {
              myHasModif = true;
              myXyzMap .SetValues(0,  2, 1);
              mySign_lf.SetCoord (1, -1, 1);
              break;
            }
            case RWObj_CoordinateSystem_negZfwd_posYup:
            {
              break;
            }
          }
          break;
        }
      }
    }
    myTrsf_lf = mySign_lf * myUnitFactor;
    mySign_f.SetValues (float(mySign_lf.X()), float(mySign_lf.Y()), float(mySign_lf.Z()));
  }

  //! Transform position.
  void TransformPosition (gp_XYZ& thePos)
  {
    if (!myHasModif)
    {
      return;
    }

    thePos.SetCoord (thePos.Coord (myXyzMap[0] + 1),
                     thePos.Coord (myXyzMap[1] + 1),
                     thePos.Coord (myXyzMap[2] + 1));
    thePos *= myTrsf_lf;
  }

  //! Transform normal.
  void TransformNormal (Graphic3d_Vec3& theNorm)
  {
    if (!myHasModif)
    {
      return;
    }

    theNorm.SetValues (theNorm[myXyzMap[0]],
                       theNorm[myXyzMap[1]],
                       theNorm[myXyzMap[2]]);
    theNorm *= mySign_f;
  }

private:

  gp_XYZ          mySign_lf;    //!< vec3 defining Sign of each component; (1,1,1) by default
  gp_XYZ          myTrsf_lf;    //!< vec3 defining XYZ transformation (including Unit scale Factor and Sign)
  Graphic3d_Vec3i myXyzMap;     //!< vec3 defining indices of each component for remapping; (0,1,2) by default
  Graphic3d_Vec3  mySign_f;     //!< same as mySign_lf but in single floating point precision
  Standard_Real   myUnitFactor; //!< unit scale factor
  bool            myHasModif;   //!< flag indicating that transformation is not empty

};

#endif // _RWObj_CoordinateSystemFlipper_HeaderFile

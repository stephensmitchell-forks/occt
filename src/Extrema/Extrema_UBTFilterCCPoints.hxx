// Created on: 2016-07-20
// Created by: OPEN CASCADE Support
// Copyright (c) 2005-2016 OPEN CASCADE SAS
//
// This file is part of Open CASCADE Technology software library.
//
// This library is free software; you can redistribute it and / or modify it
// under the terms of the GNU Lesser General Public version 2.1 as published
// by the Free Software Foundation, with special exception defined in the file
// OCCT_LGPL_EXCEPTION.txt. Consult the file LICENSE_LGPL_21.txt included in OCCT
// distribution for complete text of the license and disclaimer of any warranty.
//
// Alternatively, this file may be used under the terms of Open CASCADE
// commercial license or contractual agreement.

#ifndef _Extrema_UBTFilterCCPoints_HeaderFile
#define _Extrema_UBTFilterCCPoints_HeaderFile

#include <NCollection_CellFilter.hxx>

class Extrema_CCPointsInspector : public NCollection_CellFilter_InspectorXY
{
public:
  typedef gp_XY Target;
  //! Constructor; remembers the tolerance
  Extrema_CCPointsInspector (const Standard_Real theTol[2])
  {
    myTol[0] = theTol[0];
    myTol[1] = theTol[1];
    myIsFind = Standard_False;
  }

  void ClearFind()
  {
    myIsFind = Standard_False;
  }

  Standard_Boolean isFind()
  {
    return myIsFind;
  }

  //! Set current point to search for coincidence
  void SetCurrent (const gp_XY& theCurPnt) 
  { 
    myCurrent = theCurPnt;
  }

  //! Implementation of inspection method
  NCollection_CellFilter_Action Inspect (const Target& theObject)
  {
    gp_XY aPt = myCurrent.Subtracted(theObject);
    if((Abs(aPt.X()) < myTol[0]) && (Abs(aPt.Y()) < myTol[1]))
    {
      myIsFind = Standard_True;
    }

    return CellFilter_Keep;
  }

private:
  Standard_Real myTol[2];
  gp_XY myCurrent;
  Standard_Boolean myIsFind;
};

typedef NCollection_CellFilter<Extrema_CCPointsInspector> Extrema_UBTFilterCCPoints;

#endif //_Extrema_UBTFilterCCPoints_HeaderFile

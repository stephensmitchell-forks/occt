// Created on: 1993-09-23
// Created by: Didier PIFFAULT
// Copyright (c) 1993-1999 Matra Datavision
// Copyright (c) 1999-2014 OPEN CASCADE SAS
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

#ifndef _BRepMesh_Triangle_HeaderFile
#define _BRepMesh_Triangle_HeaderFile

#include <Standard.hxx>
#include <Standard_DefineAlloc.hxx>
#include <Standard_Macro.hxx>

#include <BRepMesh_DegreeOfFreedom.hxx>


//! Light weighted structure representing triangle 
//! of mesh consisting of oriented links.
class BRepMesh_Triangle
{
public:

  DEFINE_STANDARD_ALLOC

  //! Default constructor.
  BRepMesh_Triangle()
    : myEdges{0, 0, 0},
      myOrientations{ Standard_False, Standard_False, Standard_False },
      myMovability  (BRepMesh_Free)
  {
  }

  //! Constructor.
  //! @param theEdges array of edges of triangle.
  //! @param theOrientations array of edge's orientations.
  //! @param theMovability movability of triangle.
  BRepMesh_Triangle(
    const Standard_Integer          (&theEdges)[3],
    const Standard_Boolean          (&theOrientations)[3],
    const BRepMesh_DegreeOfFreedom  theMovability)
  {
    Initialize(theEdges, theOrientations, theMovability);
  }
  
  //! Initializes the triangle by the given parameters.
  //! @param theEdges array of edges of triangle.
  //! @param theOrientations array of edge's orientations.
  //! @param theMovability movability of triangle.
  inline void Initialize(
    const Standard_Integer          (&theEdges)[3],
    const Standard_Boolean          (&theOrientations)[3],
    const BRepMesh_DegreeOfFreedom  theMovability)
  {
    memcpy(myEdges, theEdges, sizeof(theEdges));
    memcpy(myOrientations, theOrientations, sizeof(theOrientations));
    myMovability   = theMovability;
  }
  
  //! Gets edges with orientations composing the triangle.
  //! @param[out] theEdges array edges are stored to.
  //! @param[out] theOrientations array orientations are stored to.
  inline void Edges(Standard_Integer (&theEdges)[3],
                    Standard_Boolean (&theOrientations)[3]) const
  {
    memcpy(theEdges, myEdges, sizeof(myEdges));
    memcpy(theOrientations, myOrientations, sizeof(myOrientations));
  }
  
  //! Returns movability of the triangle.
  inline BRepMesh_DegreeOfFreedom Movability() const 
  {
    return myMovability;
  }
  
  //! Sets movability of the triangle.
  inline void SetMovability(const BRepMesh_DegreeOfFreedom theMovability)
  {
    myMovability = theMovability;
  }
  
  //! Returns hash code for this triangle.
  //! @param theUpper upper index in the container.
  //! @return hash code.
  inline Standard_Integer HashCode(const Standard_Integer theUpper) const
  {
    return ::HashCode(myEdges[0] + myEdges[1] + myEdges[2], theUpper);
  }
  
  //! Checks for equality with another triangle.
  //! @param theOther triangle to be checked against this one.
  //! @return TRUE if equal, FALSE if not.
  inline Standard_Boolean IsEqual(const BRepMesh_Triangle& theOther) const
  {
    if (myMovability == BRepMesh_Deleted || theOther.myMovability == BRepMesh_Deleted)
      return Standard_False;

    if (myEdges[0] == theOther.myEdges[0] &&
        myEdges[1] == theOther.myEdges[1] &&
        myEdges[2] == theOther.myEdges[2])
    {
      return Standard_True;
    }

    if (myEdges[0] == theOther.myEdges[1] &&
        myEdges[1] == theOther.myEdges[2] &&
        myEdges[2] == theOther.myEdges[0])
    {
      return Standard_True;
    }

    if (myEdges[0] == theOther.myEdges[2] &&
        myEdges[1] == theOther.myEdges[0] &&
        myEdges[2] == theOther.myEdges[1])
    {
      return Standard_True;
    }

    return Standard_False;
  }
  
  //! Alias for IsEqual.
  inline Standard_Boolean operator ==(const BRepMesh_Triangle& theOther) const
  {
    return IsEqual(theOther);
  }

  Standard_Integer          myEdges[3];
  Standard_Boolean          myOrientations[3];
  BRepMesh_DegreeOfFreedom  myMovability;
};

inline Standard_Integer HashCode(const BRepMesh_Triangle& theTriangle,
                                 const Standard_Integer   theUpper)
{
 return theTriangle.HashCode(theUpper);
}

#endif

// Created on: 2017-02-17
// Created by: Nikolai BUKHALOV
// Copyright (c) 1999-2017 OPEN CASCADE SAS
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

#ifndef _TopClass_GeomVertex_HeaderFile
#define _TopClass_GeomVertex_HeaderFile

#include <Standard_Type.hxx>

#include <gp_Pnt.hxx>

class TopoDS_Edge;
class TopoDS_Vertex;

//! Stores geometrical information about TopoDS_Vertex,
//! which is useful for TopClass_GeomEdge class only.
//! ATTENTION!!! This geometrical representation
//! does not store any information about sharing this
//! vertex between several edges. I.e. one geom vertex
//! is in single geom-edge only.

class TopClass_GeomVertex
{
public:
  //! Default constructor
  TopClass_GeomVertex() : myToler(-1.0), myEdgeParameter(0.0){}

  //! It is main constructor for initialization of
  //! this class.
  TopClass_GeomVertex(const TopoDS_Vertex &theV,
                      const TopoDS_Edge   &theE)
  {
    Init(theV, theE);
  }

  //! This constructor is used for pseudo-edges (which do not have
  //! any 3D-representations).
  TopClass_GeomVertex(const TopoDS_Vertex &theV,
                      const Standard_Real theParameterOnEdge)
  {
    Init(theV, theParameterOnEdge);
  }

  //! It is main method for initialization of
  //! this class.
  Standard_EXPORT void Init(const TopoDS_Vertex &theV,
                            const TopoDS_Edge   &theE);

  //! This method is used for initialization of pseudo-edges
  //! (which do not have any 3D-representations).
  Standard_EXPORT void Init(const TopoDS_Vertex &theV,
                            const Standard_Real theParameterOnEdge);

  //! Returns TRUE for not initialized object
  Standard_Boolean IsNull() const
  {
    return (myToler < 0.0);
  }

  //! Returns TRUE if theP is covered by *this
  Standard_Boolean IsSame(const gp_Pnt& theP) const
  {
    if (IsNull())
      return Standard_False;

    return (theP.SquareDistance(myPoint) < myToler*myToler);
  }

  //! Returns the parameter of the vertex on the edge.
  const Standard_Real ParameterOnEdge() const
  {
    return myEdgeParameter;
  }

protected:

private:
  //! 3D-point of the vertex
  gp_Pnt myPoint;
  
  //! Tolerance of the vertex
  Standard_Real myToler;

  //! Parameter of the vertex on the edge.
  Standard_Real myEdgeParameter;
};

#endif
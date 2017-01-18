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

#ifndef _TopClass_GeomEdge_HeaderFile
#define _TopClass_GeomEdge_HeaderFile

#include <Standard_Type.hxx>

#include <Geom2dAdaptor_Curve.hxx>
#include <GeomAdaptor_Curve.hxx>
#include <GeomAdaptor_Surface.hxx>
#include <Precision.hxx>
#include <TopClass_GeomVertex.hxx>
#include <TopoDS_Edge.hxx>

class Geom_Curve;
class Geom_Surface;
class Geom2d_Curve;
class TopoDS_Face;

//! Stores information about 3D-curve and
//! correspond 2D-curve on the face for some edge
//! (i.e. geometrical representation of the edge).
//! This info is used for classifier algorithm only.
//! 3D-curve can be Null-curve (for pseudo- or 
//! degenerated-edge).
//! Also, this class can store 2d-curve only (without 3D-curve
//! and face). It makes this class compatible with
//! Geom2dHatch_* classes.
//! ATTENTIONS!!!
//!   1. The information about 2D-curve has
//! priority over the information about 3D one
//! (e.g. First/Last-parameter are taken from 2D-curve).
//!   2. This geometrical representation does not store any
//! information about sharing of several edges. I.e. even if two
//! edges are shared topologically, their geometrical representations
//! will be two independent (to each other) edges. Every such edge will
//! have two independent geometrical vertices (one vertex of
//! 1st edge will be identical of the vertex of 2nd edge but
//! these vertices will be different and independent to each other).

class TopClass_GeomEdge
{
public:
  //! Default constructor
  Standard_EXPORT TopClass_GeomEdge() : myOri(TopAbs_EXTERNAL),
                                        myIsSameParameter(Standard_False)
  {};

  //! Constructor. Creates edge, which is bounded by the vertices theVf theVl
  //! and has orientation theOri. 3D- (theC3D) and 2D- (theC2D) curve on
  //! the surface theS have range [theFirstParam, theLastParam].
  //! theToler is 3D-tolerance of the edge (is used for check ON-status).
  Standard_EXPORT TopClass_GeomEdge(const Handle(Geom_Curve)& theC3D,
                                    const Handle(Geom2d_Curve)& theC2D,
                                    const Handle(Geom_Surface)& theS,
                                    const Standard_Real theFirstParam,
                                    const Standard_Real theLastParam,
                                    const TopAbs_Orientation theOri,
                                    const TopClass_GeomVertex &theVf,
                                    const TopClass_GeomVertex &theVl,
                                    const Standard_Boolean theIsSameParameter);
  
  //! Constructor. Excludes necessary geometrical information from topological
  //! edge theE lied in the face theF.
  //! ATENTION!!!
  //!   1. theE must have 2D-curve on theF. Otherwise, no object will be created.
  //!   2. Orientation of *this (geometrical edge) will always match
  //!     the orientation of theE. So, the application must set
  //!     correct edge orientation before calling this constructor.
  Standard_EXPORT TopClass_GeomEdge(const TopoDS_Edge& theE,
                                    const TopoDS_Face& theF);

  //! Replaces 2d-curve with theC.
  void Replace2DCurve(const Geom2dAdaptor_Curve& theC,
                      const TopAbs_Orientation theOri)
  {
    myC2D = theC;
    myOri = theOri;
  }

  //! Recomputes thePt on the surface mySurf to 3D-point and
  //! looks for thePOnC, which is nearest point in
  //! the 3D-curve myC3D to thePt. If (thePOnC != 0)
  //! the found value will be assigned to thePOnC.
  //! If (theParOnC != 0) the method will assign to it the
  //! parameter on myC3D respected to thePOnC.
  //! Returns the distance between thePt and thePOnC.
  //! Returns negative value if computation is not possible.
  //! ATTENTION!!!
  //!   The final result depends on the value, which theParOnC
  //!   is initialized by before calling the method.
  //!   If *theParOnC = DBL_MAX (or DBL_MIN) or does not exist then
  //!   the result of global extrema algorithm will be returned.
  //!   Otherwise, the local extrema will be used with initial
  //!   parameter "*theParOnC".
  Standard_EXPORT Standard_Real Distance3D(const gp_Pnt2d &thePt,
                                           gp_Pnt *thePOnC = 0,
                                           Standard_Real *theParOnC = 0) const;
 

  //! Looks for thePOnC, which is nearest point in
  //! the 2D-curve myC2D to thePt. If thePOnC != 0
  //! the found value will be assigned to thePOnC.
  //! If (theParOnC != 0) the method will assign to it the
  //! parameter on myC2D respected to thePOnC.
  //! Returns the distance between thePt and thePOnC.
  //! Returns negative value if computation is not possible.
  //! ATTENTION!!!
  //!   The final result depends on the value, which theParOnC
  //!   is initialized by before calling the method.
  //!   If *theParOnC = DBL_MAX (or DBL_MIN) or does not exist then
  //!   the result of global extrema algorithm will be returned.
  //!   Otherwise, the local extrema will be used with initial
  //!   parameter "*theParOnC".
  Standard_EXPORT Standard_Real Distance2D(const gp_Pnt2d &thePt,
                                           gp_Pnt2d *thePOnC = 0,
                                           Standard_Real *theParOnC = 0) const;

  //! Returns 3D-curve of the edge
  const GeomAdaptor_Curve& Get3DCurve() const
  {
    return myC3D;
  }

  //! Returns orientation of the edge
  TopAbs_Orientation GetOrientation() const
  {
    return myOri;
  }

  //! Returns 2D-curve of the edge on mySurf
  const Geom2dAdaptor_Curve& Get2DCurve() const
  {
    return myC2D;
  }

  //! Returns surface
  const GeomAdaptor_Surface& GetSurface() const
  {
    return mySurf;
  }

  //! Returns TRUE if the edge is pseudo
  Standard_Boolean IsPseudo() const
  {
    return (myC3D.GetType() == GeomAbs_OtherCurve);
  }

  //! Returns TRUE if the edge has not been
  //! initialized yet.
  Standard_Boolean IsNull() const
  {
    return (myC2D.GetType() == GeomAbs_OtherCurve);
  }

  //! Returns first parameter of the curve of the edge
  Standard_Real FirstParameter() const
  {
    return myC2D.FirstParameter();
  }

  //! Returns last parameter of the curve of the edge
  Standard_Real LastParameter() const
  {
    return myC2D.LastParameter();
  }

  //! Returns vertex in the start position of the edge
  const TopClass_GeomVertex& FirstVertex() const
  {
    return myVertF;
  }

  //! Returns vertex in the end of the edge
  const TopClass_GeomVertex& LastVertex() const
  {
    return myVertL;
  }

  //! Returns topological representation
  //! of the edge. Uses only in TopOpeBRep classes.
  const TopoDS_Edge& GetTopoEdge() const
  {
    return myTopoEdge;
  }

  Standard_Boolean IsSameParameter() const
  {
    return myIsSameParameter;
  }

protected:

  //! Initializes of fields
  void Init(const Handle(Geom_Curve)& theC3D,
            const Handle(Geom2d_Curve)& theC2D,
            const Handle(Geom_Surface)& theS,
            const Standard_Real theFirstParam,
            const Standard_Real theLastParam);

private:
  //! This field is used for compatibility with TopOpeBRep classes
  //! and must be deleted after these classes will be eliminated.
  TopoDS_Edge myTopoEdge;

  //! 3D-curve of the edge
  GeomAdaptor_Curve myC3D;

  //! 2D-curve of the edge
  Geom2dAdaptor_Curve myC2D;

  //! Surface, which lies 2D-curve on
  GeomAdaptor_Surface mySurf;

  //! Orientation of the edge
  TopAbs_Orientation myOri;
  
  //! Vertex in the start position of the edge
  TopClass_GeomVertex myVertF;

  //! Vertex in the end of the edge
  TopClass_GeomVertex myVertL;

  //! Is equal to TRUE if myC3D and myC2D
  //! are same-parameter.
  Standard_Boolean myIsSameParameter;
};

#endif
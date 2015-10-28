// Created on: 1995-01-27
// Created by: Jacques GOUSSARD
// Copyright (c) 1995-1999 Matra Datavision
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

#ifndef _GeomInt_TheMultiLineOfWLApprox_HeaderFile
#define _GeomInt_TheMultiLineOfWLApprox_HeaderFile

#include <Standard.hxx>
#include <Standard_DefineAlloc.hxx>
#include <Standard_Handle.hxx>

#include <Standard_Address.hxx>
#include <Standard_Integer.hxx>
#include <Standard_Boolean.hxx>
#include <Standard_Real.hxx>
#include <Approx_Status.hxx>
#include <TColgp_Array1OfPnt.hxx>
#include <TColgp_Array1OfPnt2d.hxx>
#include <TColgp_Array1OfVec.hxx>
#include <TColgp_Array1OfVec2d.hxx>
#include <IntPatch_WLine.hxx>


class ApproxInt_SvSurfaces;



class GeomInt_TheMultiLineOfWLApprox 
{
public:

  DEFINE_STANDARD_ALLOC

  //! The class SvSurfaces is used when the approximation algorithm 
  //! needs some extra points on the line <line>. 
  //! A New line  is then created which shares the same surfaces and functions.
  //! SvSurfaces is a deferred class which allows several implementations of
  //! this  algorithm with different surfaces (bi-parametric ones, or 
  //! implicit and biparametric ones)
  Standard_EXPORT GeomInt_TheMultiLineOfWLApprox( const Handle(IntPatch_WLine)& line,
                                                  const Standard_Address PtrSvSurfaces,
                                                  const Standard_Integer NbP3d,
                                                  const Standard_Integer NbP2d,
                                                  const Standard_Boolean P2DOnFirst,
                                                  const Standard_Integer IndMin = 0,
                                                  const Standard_Integer IndMax = 0);
  
  //! No Extra points will be added on the current line
  Standard_EXPORT GeomInt_TheMultiLineOfWLApprox( const Handle(IntPatch_WLine)& line,
                                                  const Standard_Integer NbP3d,
                                                  const Standard_Integer NbP2d,
                                                  const Standard_Boolean P2DOnFirst,
                                                  const Standard_Integer IndMin = 0,
                                                  const Standard_Integer IndMax = 0);
  
  Standard_EXPORT Standard_Integer FirstPoint() const
  {
    return indicemin;
  }
  
  Standard_EXPORT Standard_Integer LastPoint() const
  {
    return indicemax;
  }

  Standard_EXPORT Approx_Status WhatStatus() const
  {
    if(PtrOnmySvSurfaces)
      return(Approx_PointsAdded);
    else 
      return(Approx_NoPointsAdded);
  }

  //! Returns the number of 3d points of a TheLine.
  Standard_EXPORT Standard_Integer NbP3d() const
  {
    return nbp3d;
  }
  
  //! Returns the number of 2d points of a TheLine.
  Standard_EXPORT Standard_Integer NbP2d() const
  {
    return nbp2d;
  }
  
  //! Returns the 3d points of the multipoint <MPointIndex> when only 3d points exist.
  Standard_EXPORT void Value (const Standard_Integer MPointIndex,
                              TColgp_Array1OfPnt& tabPt) const
  {
    tabPt(1) = myLine->Point(MPointIndex).Value();
  }
  
  //! returns the 3d and 2d points of the multipoint <MPointIndex>.
  Standard_EXPORT void Value (const Standard_Integer MPointIndex,
                              TColgp_Array1OfPnt& tabPt, TColgp_Array1OfPnt2d& tabPt2d) const
  {
    Value(MPointIndex, tabPt);
    Value(MPointIndex, tabPt2d);
  }
  
  //! Returns the 2d points of the multipoint <MPointIndex> when only 2d points exist.
  Standard_EXPORT void Value (const Standard_Integer MPointIndex,
                              TColgp_Array1OfPnt2d& tabPt2d) const;  
  
  //! Returns the 3d tangency points of the multipoint <MPointIndex> only
  //! when 3d points exist.
  Standard_EXPORT Standard_Boolean Tangency ( const Standard_Integer MPointIndex,
                                              TColgp_Array1OfVec& tabV) const;
  
  //! Returns the 2d tangency points of the multipoint <MPointIndex> only
  //! when 2d points exist.
  Standard_EXPORT Standard_Boolean Tangency ( const Standard_Integer MPointIndex,
                                              TColgp_Array1OfVec2d& tabV2d) const;
  
  //! Returns the 3d and 2d points of the multipoint <MPointIndex>.
  Standard_EXPORT Standard_Boolean Tangency ( const Standard_Integer MPointIndex,
                                              TColgp_Array1OfVec& tabV,
                                              TColgp_Array1OfVec2d& tabV2d) const
  {
    return (Tangency(MPointIndex, tabV) && Tangency(MPointIndex, tabV2d));
  }
  
  Standard_EXPORT GeomInt_TheMultiLineOfWLApprox
                          MakeMLBetween ( const Standard_Integer Low,
                                          const Standard_Integer High,
                                          const Standard_Integer NbPointsToInsert) const;
  
  //! Dump of the current multi-line.
  Standard_EXPORT void Dump() const;




protected:
  GeomInt_TheMultiLineOfWLApprox operator=(GeomInt_TheMultiLineOfWLApprox&);

private:
  const Standard_Address PtrOnmySvSurfaces;
  const Handle(IntPatch_WLine) myLine;
  const Standard_Integer indicemin;
  const Standard_Integer indicemax;
  const Standard_Integer nbp3d;
  const Standard_Integer nbp2d;
  const Standard_Boolean p2donfirst;
};

#endif // _GeomInt_TheMultiLineOfWLApprox_HeaderFile

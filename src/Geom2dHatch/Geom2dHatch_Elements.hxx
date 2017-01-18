// Created on: 1994-12-16
// Created by: Laurent BUCHARD
// Copyright (c) 1994-1999 Matra Datavision
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

#ifndef _Geom2dHatch_Elements_HeaderFile
#define _Geom2dHatch_Elements_HeaderFile

#include <Geom2dHatch_MapOfElements.hxx>
#include <NCollection_List.hxx>

class TopClass_GeomEdge;
class TopClass_RayInfo;

class Geom2dHatch_Elements 
{
public:

  DEFINE_STANDARD_ALLOC

  
  Standard_EXPORT Geom2dHatch_Elements();
  
  Standard_EXPORT Geom2dHatch_Elements(const Geom2dHatch_Elements& Other);
  
  Standard_EXPORT void Clear();
  ~Geom2dHatch_Elements()
  {
    Clear();
  }
  
  Standard_EXPORT Standard_Boolean Bind (const Standard_Integer K,
                                         const Geom2dHatch_Element& I);
  
  Standard_EXPORT Standard_Boolean IsBound (const Standard_Integer K) const;
  
  Standard_EXPORT Standard_Boolean UnBind (const Standard_Integer K);
  
  Standard_EXPORT const Geom2dHatch_Element& Find(const Standard_Integer K) const;
  const Geom2dHatch_Element& operator() (const Standard_Integer K) const
  {
    return Find(K);
  }
  
  Standard_EXPORT Geom2dHatch_Element& ChangeFind(const Standard_Integer K);
  Geom2dHatch_Element& operator() (const Standard_Integer K)
  {
    return ChangeFind(K);
  }

  Standard_EXPORT Standard_Boolean CheckPoint (gp_Pnt2d& P);

  //! Returns ray to process.
  Standard_EXPORT Standard_Boolean OtherRay(const gp_Pnt2d& P,
                                            TopClass_RayInfo& theRay,
                                            const unsigned int theMaxRating);

  Standard_EXPORT void InitWires();
  
  Standard_EXPORT Standard_Boolean MoreWires() const;
  
  Standard_EXPORT void NextWire();
  
  Standard_EXPORT void InitEdges();
  
  Standard_EXPORT Standard_Boolean MoreEdges() const;
  
  Standard_EXPORT void NextEdge();
  
  //! Returns analyzed edge.
  Standard_EXPORT void CurrentEdge(TopClass_GeomEdge& E) const;

  //! Returns TRUE if thePnt is covered by the tolerance of at least one
  //! vertex of the analyzed edge.
  //! If is true then stores the parameter of the vertex on the curve.
  //! Otherwise, returns *theParameter = 0.0.
  //! This parameter is used in TopOpeBRep class
  //! and must be deleted after this class will be eliminated.
  Standard_EXPORT 
    Standard_Boolean IsPointInEdgeVertex(const gp_Pnt2d &thePnt,
                                         Standard_Real* const theParameter = 0) const;

  Standard_Boolean FirstRay(const gp_Pnt2d& theP,
                            TopClass_RayInfo& theRay,
                            const unsigned int theMaxRating = UINT_MAX)
  {
    myCurEdge = 1;

    return OtherRay(theP, theRay, theMaxRating);
  }



protected:
  

private:
  Geom2dHatch_MapOfElements myMap;
  Geom2dHatch_DataMapIteratorOfMapOfElements myIter;
  Standard_Integer myCurrentWire;
  Standard_Integer myCurEdge;
};

#endif // _Geom2dHatch_Elements_HeaderFile

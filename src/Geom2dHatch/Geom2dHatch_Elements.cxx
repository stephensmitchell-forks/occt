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

//  Modified by skv - Fri Jul 14 17:03:47 2006 OCC12627

#include <Geom2dHatch_Elements.hxx>

#include <TopClass_GeomEdge.hxx>
#include <TopClass_RayInfo.hxx>

Geom2dHatch_Elements::Geom2dHatch_Elements(const Geom2dHatch_Elements& )
{
#ifdef OCCT_DEBUG
  cout<<" Magic Constructor in Geom2dHatch_Elements:: "<<endl;
#endif
}

Geom2dHatch_Elements::Geom2dHatch_Elements() :myCurrentWire(0)
{
}

void Geom2dHatch_Elements::Clear()
{
  myMap.Clear();
}

Standard_Boolean Geom2dHatch_Elements::IsBound(const Standard_Integer K) const
{
  return(myMap.IsBound(K));
}

Standard_Boolean Geom2dHatch_Elements::UnBind(const Standard_Integer K)
{
  return(myMap.UnBind(K));
}

Standard_Boolean Geom2dHatch_Elements::Bind(const Standard_Integer K,const Geom2dHatch_Element& I)
{
  return(myMap.Bind(K,I));
}

const Geom2dHatch_Element& Geom2dHatch_Elements::Find(const Standard_Integer K) const
{
  return(myMap.Find(K));
}

Geom2dHatch_Element& Geom2dHatch_Elements::ChangeFind(const Standard_Integer K)
{
  return(myMap.ChangeFind(K));
}

//=======================================================================
//function : CheckPoint
//purpose  : 
//=======================================================================

Standard_Boolean  Geom2dHatch_Elements::CheckPoint(gp_Pnt2d&)
{
  return Standard_True;
}

//=======================================================================
//function : ListOfRays
//purpose  : 
//=======================================================================
void Geom2dHatch_Elements::ListOfRays(const gp_Pnt2d& thePnt,
                                      NCollection_List<TopClass_RayInfo>& theList)
{
  Geom2dHatch_DataMapIteratorOfMapOfElements Itertemp(myMap);

  for (Itertemp.Initialize(myMap); Itertemp.More(); Itertemp.Next())
  {
    const Standard_Integer anIdx = Itertemp.Key();
    const Geom2dHatch_Element& anItem = myMap.Find(anIdx);
    const Geom2dAdaptor_Curve& E = anItem.Curve();
    const TopAbs_Orientation anOri = anItem.Orientation();
    if ((anOri != TopAbs_FORWARD) && (anOri != TopAbs_REVERSED))
    {
      continue;
    }

    TopClass_RayInfo aRI;
    const gp_Pnt2d aPE = E.Value((E.FirstParameter() + E.LastParameter()) *0.5);
    const gp_Vec2d aV(thePnt, aPE);
    aRI.SetParam(aV.Magnitude());
    if (aRI.GetParameter() < gp::Resolution())
    {
      continue;
    }

    aRI.SetLin(gp_Lin2d(thePnt, aV));
    theList.Append(aRI);
  }
}

//=======================================================================
//function : InitWires
//purpose  : 
//=======================================================================

void  Geom2dHatch_Elements::InitWires()
{
  myCurrentWire = 0;
}

//=======================================================================
//function : InitEdges
//purpose  : 
//=======================================================================

void  Geom2dHatch_Elements::InitEdges()
{
  myIter.Initialize(myMap);
}

//=======================================================================
//function : CurrentEdge
//purpose  : 
//=======================================================================

void  Geom2dHatch_Elements::CurrentEdge(TopClass_GeomEdge& theGE) const 
{
  const Geom2dHatch_Element& anItem = myMap.Find(myIter.Key());
  theGE.Replace2DCurve(anItem.Curve(), anItem.Orientation());
}


//=======================================================================
//function : MoreWires
//purpose  : 
//=======================================================================

Standard_Boolean  Geom2dHatch_Elements::MoreWires() const 
{
  return (myCurrentWire == 0);
}

//=======================================================================
//function : NextWire
//purpose  : 
//=======================================================================

void Geom2dHatch_Elements::NextWire()
{
  myCurrentWire++;
}

//=======================================================================
//function : MoreEdges
//purpose  : 
//=======================================================================

Standard_Boolean  Geom2dHatch_Elements::MoreEdges() const
{
  return(myIter.More());
}

//=======================================================================
//function : NextEdge
//purpose  : 
//=======================================================================

void Geom2dHatch_Elements::NextEdge()
{
  myIter.Next();
}

//=======================================================================
//function : IsPointInEdgeVertex
//purpose  : 
//=======================================================================
Standard_Boolean
  Geom2dHatch_Elements::IsPointInEdgeVertex(const gp_Pnt2d &thePnt,
                                            Standard_Real* const theParameter) const
{
  const Geom2dAdaptor_Curve& aCurv = myMap.Find(myIter.Key()).Curve();
  const gp_Pnt2d aPntF(aCurv.Value(aCurv.FirstParameter())),
                 aPntL(aCurv.Value(aCurv.LastParameter()));
  const Standard_Real aSqTol = Precision::SquarePConfusion();

  if (thePnt.SquareDistance(aPntF) < aSqTol)
  {
    if (theParameter)
      *theParameter = aCurv.FirstParameter();

    return Standard_True;
  }

  if (thePnt.SquareDistance(aPntL) < aSqTol)
  {
    if (theParameter)
      *theParameter = aCurv.LastParameter();

    return Standard_True;
  }

  if (theParameter)
    *theParameter = 0.0;

  return Standard_False;
}



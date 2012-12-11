// Created on: 1995-12-08
// Created by: Jacques GOUSSARD
// Copyright (c) 1995-1999 Matra Datavision
// Copyright (c) 1999-2012 OPEN CASCADE SAS
//
// The content of this file is subject to the Open CASCADE Technology Public
// License Version 6.5 (the "License"). You may not use the content of this file
// except in compliance with the License. Please obtain a copy of the License
// at http://www.opencascade.org and read it completely before using this file.
//
// The Initial Developer of the Original Code is Open CASCADE S.A.S., having its
// main offices at: 1, place des Freres Montgolfier, 78280 Guyancourt, France.
//
// The Original Code and all software distributed under the License is
// distributed on an "AS IS" basis, without warranty of any kind, and the
// Initial Developer hereby disclaims all such warranties, including without
// limitation, any warranties of merchantability, fitness for a particular
// purpose or non-infringement. Please see the License for the specific terms
// and conditions governing the rights and limitations under the License.



#include <BRepCheck.ixx>

#include <BRepCheck_ListIteratorOfListOfStatus.hxx>
#include <BRepCheck_Wire.hxx>

#include <BRep_Tool.hxx>

#include <TopoDS_Iterator.hxx>
#include <TopoDS.hxx>


//=======================================================================
//function : Add
//purpose  : 
//=======================================================================

void BRepCheck::Add(BRepCheck_ListOfStatus& lst, const BRepCheck_Status stat)
{
  BRepCheck_ListIteratorOfListOfStatus it(lst);
  while (it.More()) {
    if (it.Value() == BRepCheck_NoError && stat != BRepCheck_NoError) {
      lst.Remove(it);
    }
    else {
      if (it.Value() == stat) {
	return;
      }
      it.Next();
    }
  }
  lst.Append(stat);
}



//=======================================================================
//function : SelfIntersection
//purpose  : 
//=======================================================================

Standard_Boolean BRepCheck::SelfIntersection(const TopoDS_Wire& W,
					     const TopoDS_Face& myFace,
					     TopoDS_Edge& RetE1,
					     TopoDS_Edge& RetE2)
{

  Handle(BRepCheck_Wire) chkw = new BRepCheck_Wire(W);
  BRepCheck_Status stat = chkw->SelfIntersect(myFace,RetE1,RetE2);
  return (stat == BRepCheck_SelfIntersectingWire);
}
				  

//=======================================================================
//function : Print
//purpose  : 
//=======================================================================

void BRepCheck::Print(const BRepCheck_Status stat,
		      Standard_OStream& OS)
{
#define PRINT_CASE(stat) case stat: OS << #stat "\n"; break

  switch (stat) {
  PRINT_CASE(BRepCheck_NoError);
  PRINT_CASE(BRepCheck_InvalidPointOnCurve);
  PRINT_CASE(BRepCheck_InvalidPointOnCurveOnSurface);
  PRINT_CASE(BRepCheck_InvalidPointOnSurface);
  PRINT_CASE(BRepCheck_No3DCurve);
  PRINT_CASE(BRepCheck_Multiple3DCurve);
  PRINT_CASE(BRepCheck_Invalid3DCurve);
  PRINT_CASE(BRepCheck_NoCurveOnSurface);
  PRINT_CASE(BRepCheck_InvalidCurveOnSurface);
  PRINT_CASE(BRepCheck_InvalidCurveOnClosedSurface);
  PRINT_CASE(BRepCheck_InvalidSameRangeFlag);
  PRINT_CASE(BRepCheck_InvalidSameParameterFlag);
  PRINT_CASE(BRepCheck_InvalidDegeneratedFlag);
  PRINT_CASE(BRepCheck_FreeEdge);
  PRINT_CASE(BRepCheck_InvalidMultiConnexity);
  PRINT_CASE(BRepCheck_InvalidRange);
  PRINT_CASE(BRepCheck_EmptyWire);
  PRINT_CASE(BRepCheck_RedundantEdge);
  PRINT_CASE(BRepCheck_SelfIntersectingWire);
  PRINT_CASE(BRepCheck_NoSurface);
  PRINT_CASE(BRepCheck_InvalidWire);
  PRINT_CASE(BRepCheck_RedundantWire);
  PRINT_CASE(BRepCheck_IntersectingWires);
  PRINT_CASE(BRepCheck_InvalidImbricationOfWires);
  PRINT_CASE(BRepCheck_EmptyShell);
  PRINT_CASE(BRepCheck_RedundantFace);
  PRINT_CASE(BRepCheck_UnorientableShape);
  PRINT_CASE(BRepCheck_NotClosed);
  PRINT_CASE(BRepCheck_NotConnected);
  PRINT_CASE(BRepCheck_SubshapeNotInShape);
  PRINT_CASE(BRepCheck_BadOrientation);
  PRINT_CASE(BRepCheck_BadOrientationOfSubshape);
  PRINT_CASE(BRepCheck_InvalidToleranceValue);
  PRINT_CASE(BRepCheck_CheckFail);
  default:
    break;
  }
}

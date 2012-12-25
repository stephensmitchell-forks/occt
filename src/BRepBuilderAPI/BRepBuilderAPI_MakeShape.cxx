// Created on: 1993-07-23
// Created by: Remi LEQUETTE
// Copyright (c) 1993-1999 Matra Datavision
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



#include <BRepBuilderAPI_MakeShape.ixx>
#include <BRep_TEdge.hxx>
#include <BRep_TFace.hxx>
#include <BRep_TVertex.hxx>
#include <TopExp_Explorer.hxx>
#include <TopoDS.hxx>
#include <TopoDS_Shape.hxx>
#include <TopoDS_Face.hxx>
#include <TopoDS_Edge.hxx>
#include <TopoDS_Vertex.hxx>


//=======================================================================
//function : BRepBuilderAPI_MakeShape
//purpose  : 
//=======================================================================

BRepBuilderAPI_MakeShape::BRepBuilderAPI_MakeShape()
{
}

void BRepBuilderAPI_MakeShape::Delete()
{}


//=======================================================================
//function : Build
//purpose  : 
//=======================================================================

void BRepBuilderAPI_MakeShape::Build()
{
}

//=======================================================================
//function : Shape
//purpose  : 
//=======================================================================

const TopoDS_Shape&  BRepBuilderAPI_MakeShape::Shape() const
{
  if (!IsDone()) {
    // the following is const cast away
    ((BRepBuilderAPI_MakeShape*) (void*) this)->Build();
    Check();
  }
  return myShape;
}


//=======================================================================
//function : operator
//purpose  : 
//=======================================================================

BRepBuilderAPI_MakeShape::operator TopoDS_Shape() const
{
  return Shape();
}


//=======================================================================
//function : Generated
//purpose  : 
//=======================================================================

const TopTools_ListOfShape& BRepBuilderAPI_MakeShape::Generated (const TopoDS_Shape& S) 

{
  myGenerated.Clear();
  return myGenerated;
}


//=======================================================================
//function : Modified
//purpose  : 
//=======================================================================

const TopTools_ListOfShape& BRepBuilderAPI_MakeShape::Modified (const TopoDS_Shape& S) 

{
  myGenerated.Clear();
  return myGenerated;
}


//=======================================================================
//function : IsDeleted
//purpose  : 
//=======================================================================

Standard_Boolean BRepBuilderAPI_MakeShape::IsDeleted (const TopoDS_Shape& S) 

{
  return Standard_False;
}



//=======================================================================
//function : EnsureToleranceRule
//purpose  : 
//=======================================================================

void BRepBuilderAPI_MakeShape::EnsureToleranceRule(const TopoDS_Shape & theS)
{
  if (theS.IsNull())
  {
    return;
  }
  //
  for (TopExp_Explorer aFE(theS, TopAbs_FACE); aFE.More(); aFE.Next())
  {
    TopoDS_Face aF = TopoDS::Face(aFE.Current());
    Standard_Real aFT = (*((Handle_BRep_TFace *)&aF.TShape()))->Tolerance();
    //
    for (TopExp_Explorer anEE(aF, TopAbs_EDGE); anEE.More(); anEE.Next())
    {
      TopoDS_Edge anES = TopoDS::Edge(anEE.Current());
      Handle_BRep_TEdge & anEG = *(Handle_BRep_TEdge *)&anES.TShape();
      Standard_Real anET = anEG->Tolerance();
      if (anET < aFT)
      {
        anET = aFT;
        anEG->Tolerance(anET);
      }
      for (TopExp_Explorer aVE(anES, TopAbs_VERTEX); aVE.More(); aVE.Next())
      {
        TopoDS_Vertex aVS = TopoDS::Vertex(aVE.Current());
        Handle_BRep_TVertex & aVG = *(Handle_BRep_TVertex *)&aVS.TShape();
        aVG->UpdateTolerance(anET);
      }
    }
    //
    for (TopExp_Explorer aVE(aF, TopAbs_VERTEX, TopAbs_EDGE);
      aVE.More(); aVE.Next())
    {
      TopoDS_Vertex aVS = TopoDS::Vertex(aVE.Current());
      Handle_BRep_TVertex & aVG = *(Handle_BRep_TVertex *)&aVS.TShape();
      aVG->UpdateTolerance(aFT);
    }
  }
  //
  for (TopExp_Explorer anEE(theS, TopAbs_EDGE, TopAbs_FACE);
    anEE.More(); anEE.Next())
  {
    TopoDS_Edge anES = TopoDS::Edge(anEE.Current());
    Handle_BRep_TEdge & anEG = *(Handle_BRep_TEdge *)&anES.TShape();
    Standard_Real anET = anEG->Tolerance();
    for (TopExp_Explorer aVE(anES, TopAbs_VERTEX); aVE.More(); aVE.Next())
    {
      TopoDS_Vertex aVS = TopoDS::Vertex(aVE.Current());
      Handle_BRep_TVertex & aVG = *(Handle_BRep_TVertex *)&aVS.TShape();
      aVG->UpdateTolerance(anET);
    }
  }
}



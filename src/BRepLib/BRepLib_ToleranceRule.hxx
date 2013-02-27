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

#ifndef _BRepLib_ToleranceRule_HeaderFile
#define _BRepLib_ToleranceRule_HeaderFile

#include <BRep_TEdge.hxx>
#include <BRep_TFace.hxx>
#include <BRep_TVertex.hxx>
#include <NCollection_DataMap.hxx>
#include <TopTools_ShapeMapHasher.hxx>

//! The type is intended to work with shape tolerances by tolerance rule.<br>
class BRepLib_ToleranceRule
{
  public:
    DEFINE_STANDARD_ALLOC
    //!  In shape theS context calculates proper tolerance theTs(S) for every
    //!  shape theS shape S so that
    //!  - S is edge contained by a face of shape theS or
    //!  - S is vertex contained by an edge or a face of shape theS.
    //!  Shape S tolerance t(S) may be only increased:
    //!  t(S) <= theTs(S).
    //!  Calculation is made by the tolerance rule:
    //!  face F contains edge   E ==> t(F)     <= theTs(E),
    //!  edge E contains vertex V ==> theTs(E) <= theTs(V),
    //!  face F contains vertex V ==> t(F)     <= theTs(V).
    //!
    //!  If flag theMinimizeTolerances is Standard_True then
    //!  - theTs(F) is calculated for every face F of shape theS as
    //!    only face F type and size dependent value,
    //!  - theTs(F) is used instead of t(F),
    //!  - tolerance of vertex V representations is used instead of t(V).
    Standard_EXPORT static void ProperTolerances(const TopoDS_Shape & theS,
      NCollection_DataMap<TopoDS_Shape, Standard_Real,
        TopTools_ShapeMapHasher> & theTs,
      Standard_Boolean theMinimizeTolerances = Standard_False);
    //!  Auxiliary method. Returns Standard_False.
    Standard_Boolean IsProtectedFromModification(
      const TopoDS_Shape & theS) const;
    //!  The method is intended for any class T which contains method
    //!  IsProtectedFromModification with the following definition:
    //!  Standard_Boolean IsProtectedFromModification(
    //!    const TopoDS_Shape & theShape) const.
    //!  Gets proper tolerances by
    //!  ProperTolerances(theS, aTs, theMinimizeTolerances).
    //!  Toleranse t(S) is set to aTs(S) for every shape S so that
    //!  aTs.IsBound(S) is Standard_True and
    //!  theObj.IsProtectedFromModification(S) is Standard_False.
    template <typename T> static void SetProperTolerances(
      const TopoDS_Shape & theS, const T & theObj,
      Standard_Boolean theMinimizeTolerances = Standard_False);
    //!  Calls SetProperTolerances(
    //!  theS, BRepLib_ToleranceRule(), theMinimizeTolerances).
    static void SetProperTolerances(const TopoDS_Shape & theS,
      Standard_Boolean theMinimizeTolerances = Standard_False);
};

#include <BRepLib_ToleranceRule.lxx>

#endif

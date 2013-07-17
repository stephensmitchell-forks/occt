// Copyright (c) 2013 OPEN CASCADE SAS
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

#ifndef _XSControl_ConnectedShapes_HeaderFile
#define _XSControl_ConnectedShapes_HeaderFile

#include <Standard.hxx>
#include <Standard_DefineHandle.hxx>
#include <XSControl_TransferReader.hxx>
#include <IFSelect_SelectExplore.hxx>
#include <Standard_Boolean.hxx>
#include <Standard_Integer.hxx>
#include <Handle_Standard_Transient.hxx>
#include <Handle_TColStd_HSequenceOfTransient.hxx>
#include <Transfer_TransientProcess.hxx>
#include <TopAbs_ShapeEnum.hxx>

class Standard_Transient;
class Interface_Graph;
class Interface_EntityIterator;
class TCollection_AsciiString;
class TColStd_HSequenceOfTransient;
class Handle(Standard_Type);
class Handle(IFSelect_SelectExplore);
class TopoDS_Shape;

DEFINE_STANDARD_HANDLE(XSControl_ConnectedShapes,IFSelect_SelectExplore)

//! From a TopoDS_Shape, or from the entity which has produced it, <br>
//!           searches for the shapes, and the entities which have produced <br>
//!           them in last transfer, which are adjacent to it by VERTICES <br>
class XSControl_ConnectedShapes : public IFSelect_SelectExplore
{
public:

  //! Creates a Selection ConnectedShapes. It remains to be set a <br>
  //!           TransferReader <br>
  Standard_EXPORT   XSControl_ConnectedShapes();

  //! Creates a Selection ConnectedShapes, which will work with the <br>
  //!           current TransferProcess brought by the TransferReader <br>
  Standard_EXPORT   XSControl_ConnectedShapes(const Handle(XSControl_TransferReader)& TR);

  //! Sets a TransferReader to sort entities : it brings the <br>
  //!           TransferProcess which may change, while the TransferReader does not <br>
  Standard_EXPORT     void SetReader(const Handle(XSControl_TransferReader)& TR);

  //! Explores an entity : entities from which are connected to that <br>
  //!           produced by this entity, including itself <br>
  Standard_EXPORT     Standard_Boolean Explore(const Standard_Integer level,
                                               const Handle(Standard_Transient)& ent,
                                               const Interface_Graph& G,
                                               Interface_EntityIterator& explored) const;

  //! Returns a text defining the criterium. <br>
  //!           "Connected Entities through produced Shapes" <br>
  Standard_EXPORT     TCollection_AsciiString ExploreLabel() const;

  //! This functions considers a shape from a transfer and performs <br>
  //!           the search function explained above <br>
  Standard_EXPORT   static  Handle_TColStd_HSequenceOfTransient AdjacentEntities(const TopoDS_Shape& ashape,
                                                                                 const Handle(Transfer_TransientProcess)& TP,
                                                                                 const TopAbs_ShapeEnum type) ;

  DEFINE_STANDARD_RTTI(XSControl_ConnectedShapes)

private: 
  Handle(XSControl_TransferReader) theTR;

};
#endif

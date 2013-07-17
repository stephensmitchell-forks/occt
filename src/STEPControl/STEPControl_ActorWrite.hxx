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

#ifndef _STEPControl_ActorWrite_HeaderFile
#define _STEPControl_ActorWrite_HeaderFile

#include <Standard.hxx>
#include <Standard_DefineHandle.hxx>
#include <Standard_Integer.hxx>
#include <Standard_Real.hxx>
#include <STEPConstruct_ContextTool.hxx>
#include <Transfer_ActorOfFinderProcess.hxx>
#include <Standard_Boolean.hxx>
#include <Handle_Transfer_Finder.hxx>
#include <Handle_Transfer_Binder.hxx>
#include <Transfer_FinderProcess.hxx>
#include <Handle_StepShape_ShapeDefinitionRepresentation.hxx>
#include <Handle_StepGeom_Axis2Placement3d.hxx>
#include <Handle_TopTools_HSequenceOfShape.hxx>
#include <STEPControl_StepModelType.hxx>
#include <Handle_StepShape_NonManifoldSurfaceShapeRepresentation.hxx>

class Transfer_Finder;
class Transfer_Binder;
class Transfer_FinderProcess;
class StepShape_ShapeDefinitionRepresentation;
class StepGeom_Axis2Placement3d;
class TopTools_HSequenceOfShape;
class TopoDS_Shape;
class StepShape_NonManifoldSurfaceShapeRepresentation;
class Standard_Transient;
class Handle(Standard_Type);
class Handle(Transfer_ActorOfFinderProcess);
class STEPControl_ActorWrite;

DEFINE_STANDARD_HANDLE(STEPControl_ActorWrite,Transfer_ActorOfFinderProcess)

//! This class performs the transfer of a Shape from TopoDS <br>
//!           to AP203 or AP214 (CD2 or DIS) <br>
class STEPControl_ActorWrite : public Transfer_ActorOfFinderProcess
{

public:

  
  Standard_EXPORT   STEPControl_ActorWrite();
  
  Standard_EXPORT   virtual  Standard_Boolean Recognize(const Handle(Transfer_Finder)& start) ;
  
  Standard_EXPORT   virtual  Handle_Transfer_Binder Transfer(const Handle(Transfer_Finder)& start,
                                                             const Handle(Transfer_FinderProcess)& FP) ;
  
  Standard_EXPORT     Handle_Transfer_Binder TransferSubShape(const Handle(Transfer_Finder)& start,
                                                              const Handle(StepShape_ShapeDefinitionRepresentation)& SDR,
                                                              Handle(StepGeom_Axis2Placement3d)& AX1,
                                                              const Handle(Transfer_FinderProcess)& FP,
                                                              const Handle(TopTools_HSequenceOfShape)& shapeGroup = NULL,
                                                              const Standard_Boolean isManifold = Standard_True) ;
  
  Standard_EXPORT     Handle_Transfer_Binder TransferShape(const Handle(Transfer_Finder)& start,
                                                           const Handle(StepShape_ShapeDefinitionRepresentation)& SDR,
                                                           const Handle(Transfer_FinderProcess)& FP,
                                                           const Handle(TopTools_HSequenceOfShape)& shapeGroup = NULL,
                                                           const Standard_Boolean isManifold = Standard_True) ;
  
  Standard_EXPORT     Handle_Transfer_Binder TransferCompound(const Handle(Transfer_Finder)& start,
                                                              const Handle(StepShape_ShapeDefinitionRepresentation)& SDR,
                                                              const Handle(Transfer_FinderProcess)& FP) ;
  
  Standard_EXPORT     void SetMode(const STEPControl_StepModelType M) ;
  
  Standard_EXPORT     STEPControl_StepModelType Mode() const;
  
  Standard_EXPORT     void SetGroupMode(const Standard_Integer mode) ;
  
  Standard_EXPORT     Standard_Integer GroupMode() const;
  
  Standard_EXPORT     void SetTolerance(const Standard_Real Tol) ;
  
  //! Customizable method to check whether shape S should <br>
  //!          be written as assembly or not <br>
  //!          Default implementation uses flag GroupMode and analyses <br>
  //!          the shape itself <br>
  //!          NOTE: this method can modify shape <br>
  Standard_EXPORT   virtual  Standard_Boolean IsAssembly(TopoDS_Shape& S) const;

  DEFINE_STANDARD_RTTI(STEPControl_ActorWrite)

private: 

  //! Non-manifold shapes are stored in NMSSR group <br>
  //!          (NON_MANIFOLD_SURFACE_SHAPE_REPRESENTATION). <br>
  //!          Use this method to get the corresponding NMSSR (or <br>
  //!          to create a new one if doesn't exist yet) <br>
  //!          (ssv; 13.11.2010) <br>
  Standard_EXPORT  Handle(StepShape_NonManifoldSurfaceShapeRepresentation) getNMSSRForGroup(const Handle(TopTools_HSequenceOfShape)& shapeGroup,
                                                                                              const Handle(Transfer_FinderProcess)& FP,
                                                                                              Standard_Boolean& isNMSSRCreated) const;

  Standard_Integer mygroup;
  Standard_Real mytoler;
  STEPConstruct_ContextTool myContext;
};
#endif

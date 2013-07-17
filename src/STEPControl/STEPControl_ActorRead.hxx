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

#ifndef _STEPControl_ActorRead_HeaderFile
#define _STEPControl_ActorRead_HeaderFile

#include <Standard.hxx>
#include <Standard_DefineHandle.hxx>
#include <StepToTopoDS_NMTool.hxx>
#include <Standard_Real.hxx>
#include <Handle_StepRepr_Representation.hxx>
#include <Transfer_ActorOfTransientProcess.hxx>
#include <Standard_Boolean.hxx>
#include <Handle_Standard_Transient.hxx>
#include <Handle_Transfer_Binder.hxx>
#include <Transfer_TransientProcess.hxx>
#include <Handle_StepGeom_Axis2Placement3d.hxx>
#include <Handle_StepRepr_RepresentationRelationship.hxx>
#include <TransferBRep_ShapeBinder.hxx>
#include <Handle_StepBasic_ProductDefinition.hxx>
#include <Handle_StepRepr_NextAssemblyUsageOccurrence.hxx>
#include <Handle_StepShape_ShapeRepresentation.hxx>
#include <Handle_StepShape_ContextDependentShapeRepresentation.hxx>
#include <Handle_StepRepr_ShapeRepresentationRelationship.hxx>
#include <Standard_Integer.hxx>
#include <Handle_StepGeom_GeometricRepresentationItem.hxx>
#include <Handle_StepRepr_MappedItem.hxx>
#include <Handle_StepShape_FaceSurface.hxx>

class StepRepr_Representation;
class Standard_Transient;
class Transfer_Binder;
class Transfer_TransientProcess;
class StepGeom_Axis2Placement3d;
class gp_Trsf;
class StepRepr_RepresentationRelationship;
class TransferBRep_ShapeBinder;
class StepBasic_ProductDefinition;
class StepRepr_NextAssemblyUsageOccurrence;
class StepShape_ShapeRepresentation;
class StepShape_ContextDependentShapeRepresentation;
class StepRepr_ShapeRepresentationRelationship;
class StepGeom_GeometricRepresentationItem;
class StepRepr_MappedItem;
class StepShape_FaceSurface;
class TopoDS_Shell;
class TopTools_ListOfShape;
class TopoDS_Compound;
class TopTools_IndexedDataMapOfShapeListOfShape;
class Standard_Transient;
class Handle(Standard_Type);
class Handle(Transfer_ActorOfTransientProcess);
class STEPControl_ActorRead;

DEFINE_STANDARD_HANDLE(STEPControl_ActorRead,Transfer_ActorOfTransientProcess)


//! This class performs the transfer of an Entity from <br>
//!           AP214 and AP203, either Geometric or Topologic. <br>
//! <br>
//!           I.E. for each type of Entity, it invokes the appropriate Tool <br>
//!           then returns the Binder which contains the Result <br>
class STEPControl_ActorRead : public Transfer_ActorOfTransientProcess {

public:

  
  Standard_EXPORT   STEPControl_ActorRead();
  
  Standard_EXPORT   virtual  Standard_Boolean Recognize(const Handle(Standard_Transient)& start) ;
  
  Standard_EXPORT   virtual  Handle_Transfer_Binder Transfer(const Handle(Standard_Transient)& start,const Handle(Transfer_TransientProcess)& TP) ;
  
  Standard_EXPORT     Handle_Transfer_Binder TransferShape(const Handle(Standard_Transient)& start,const Handle(Transfer_TransientProcess)& TP,const Standard_Boolean isManifold = Standard_True) ;
  //! set units and tolerances context by given ShapeRepresentation <br>
  Standard_EXPORT     void PrepareUnits(const Handle(StepRepr_Representation)& rep,const Handle(Transfer_TransientProcess)& TP) ;
  //! reset units and tolerances context to default <br>
//!         (mm, radians, read.precision.val, etc.) <br>
  Standard_EXPORT     void ResetUnits() ;
  //! Computes transformation defined by two axis placements (in MAPPED_ITEM <br>
//!          or ITEM_DEFINED_TRANSFORMATION) taking into account their <br>
//!          representation contexts (i.e. units, which may be different) <br>
//!          Returns True if transformation is computed and is not an identity. <br>
  Standard_EXPORT     Standard_Boolean ComputeTransformation(const Handle(StepGeom_Axis2Placement3d)& Origin,const Handle(StepGeom_Axis2Placement3d)& Target,const Handle(StepRepr_Representation)& OrigContext,const Handle(StepRepr_Representation)& TargContext,const Handle(Transfer_TransientProcess)& TP,gp_Trsf& Trsf) ;
  //! Computes transformation defined by given <br>
//!          REPRESENTATION_RELATIONSHIP_WITH_TRANSFORMATION <br>
  Standard_EXPORT     Standard_Boolean ComputeSRRWT(const Handle(StepRepr_RepresentationRelationship)& SRR,const Handle(Transfer_TransientProcess)& TP,gp_Trsf& Trsf) ;




  DEFINE_STANDARD_RTTI(STEPControl_ActorRead)

protected:

  //!Transfers product definition entity <br>
  Standard_EXPORT     Handle_TransferBRep_ShapeBinder TransferEntity(const Handle(StepBasic_ProductDefinition)& PD,const Handle(Transfer_TransientProcess)& TP) ;
  //!Transfers next assembly usage occurence entity <br>
  Standard_EXPORT     Handle_TransferBRep_ShapeBinder TransferEntity(const Handle(StepRepr_NextAssemblyUsageOccurrence)& NAUO,const Handle(Transfer_TransientProcess)& TP) ;
  //!Transfers shape representation entity <br>
  Standard_EXPORT     Handle_TransferBRep_ShapeBinder TransferEntity(const Handle(StepShape_ShapeRepresentation)& sr,const Handle(Transfer_TransientProcess)& TP,Standard_Boolean& isBound) ;
  //!Transfers context dependent shape representation entity <br>
  Standard_EXPORT     Handle_TransferBRep_ShapeBinder TransferEntity(const Handle(StepShape_ContextDependentShapeRepresentation)& CDSR,const Handle(Transfer_TransientProcess)& TP) ;
  //!Transfers  shape representation relationship entity <br>
  Standard_EXPORT     Handle_TransferBRep_ShapeBinder TransferEntity(const Handle(StepRepr_ShapeRepresentationRelationship)& und,const Handle(Transfer_TransientProcess)& TP,const Standard_Integer nbrep = 0) ;
  //!Transfers  geometric representation item entity such as ManifoldSolidBRep ,...etc <br>
  Standard_EXPORT     Handle_TransferBRep_ShapeBinder TransferEntity(const Handle(StepGeom_GeometricRepresentationItem)& git,const Handle(Transfer_TransientProcess)& TP,const Standard_Boolean isManifold) ;
  //!Transfers  mapped item <br>
  Standard_EXPORT     Handle_TransferBRep_ShapeBinder TransferEntity(const Handle(StepRepr_MappedItem)& mapit,const Handle(Transfer_TransientProcess)& TP) ;
  //!Transfers  FaceSurface entity <br>
  Standard_EXPORT     Handle_TransferBRep_ShapeBinder TransferEntity(const Handle(StepShape_FaceSurface)& fs,const Handle(Transfer_TransientProcess)& TP) ;
  //!Tranlates file by old way when CDSR are roots . Acts only if "read.step.product_mode" is equal Off. <br>
  Standard_EXPORT     Handle_TransferBRep_ShapeBinder OldWay(const Handle(Standard_Transient)& start,const Handle(Transfer_TransientProcess)& TP) ;



private: 

  
  Standard_EXPORT     TopoDS_Shell closeIDEASShell(const TopoDS_Shell& shell,const TopTools_ListOfShape& closingShells) ;
  
  Standard_EXPORT     void computeIDEASClosings(const TopoDS_Compound& comp,TopTools_IndexedDataMapOfShapeListOfShape& shellClosingMap) ;

StepToTopoDS_NMTool myNMTool;
Standard_Real myPrecision;
Standard_Real myMaxTol;
Handle_StepRepr_Representation mySRContext;


};





// other Inline functions and methods (like "C++: function call" methods)


#endif

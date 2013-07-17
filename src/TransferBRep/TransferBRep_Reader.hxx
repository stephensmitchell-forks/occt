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

#ifndef _TransferBRep_Reader_HeaderFile
#define _TransferBRep_Reader_HeaderFile

#include <Standard.hxx>
#include <Standard_DefineAlloc.hxx>
#include <Standard_Macro.hxx>
#include <Handle_Interface_Protocol.hxx>
#include <Transfer_ActorOfTransientProcess.hxx>
#include <Handle_Interface_InterfaceModel.hxx>
#include <Standard_Integer.hxx>
#include <Standard_Boolean.hxx>
#include <Transfer_TransientProcess.hxx>
#include <Handle_TopTools_HSequenceOfShape.hxx>
#include <Handle_TColStd_HSequenceOfTransient.hxx>
#include <Handle_Standard_Transient.hxx>

class Interface_Protocol;
class Transfer_ActorOfTransientProcess;
class Interface_InterfaceModel;
class Transfer_TransientProcess;
class TopTools_HSequenceOfShape;
class TColStd_HSequenceOfTransient;
class Standard_OutOfRange;
class Interface_CheckIterator;
class TopoDS_Shape;
class Standard_Transient;


//! This class offers a simple, easy to call, way of transferring <br>
//!           data from interface files to Shapes from CasCade <br>
//!           It must be specialized according to each norm/protocol, by : <br>
//!           - defining how to read a file (specific method with protocol) <br>
//!           - definig transfer, by providing an Actor <br>
class TransferBRep_Reader
{
public:

  DEFINE_STANDARD_ALLOC

  //! Initializes a non-specialised Reader. Typically, for each norm <br>
//!           or protocol, is will be required to define a specific Create <br>
//!           to load a file and transfer it <br>
  Standard_EXPORT   TransferBRep_Reader();
  //! Records the protocol to be used for read and transfer roots <br>
  Standard_EXPORT     void SetProtocol(const Handle(Interface_Protocol)& protocol) ;
  //! Returns the recorded Protocol <br>
  Standard_EXPORT   virtual  Handle_Interface_Protocol Protocol() const;
  //! Records the actor to be used for transfers <br>
  Standard_EXPORT     void SetActor(const Handle(Transfer_ActorOfTransientProcess)& actor) ;
  //! Returns the recorded Actor <br>
  Standard_EXPORT   virtual  Handle_Transfer_ActorOfTransientProcess Actor() const;
  //! Sets File Status to be interpreted as follows : <br>
//!           = 0 OK <br>
//!           < 0 file not found <br>
//!           > 0 read error, no Model could be created <br>
  Standard_EXPORT     void SetFileStatus(const Standard_Integer status) ;
  //! Returns the File Status <br>
  Standard_EXPORT     Standard_Integer FileStatus() const;
  //! Returns True if FileStatus is for FileNotFound <br>
  Standard_EXPORT     Standard_Boolean FileNotFound() const;
  //! Returns True if FileStatus is for Error during read <br>
//!           (major error; for local error, see CheckModel) <br>
  Standard_EXPORT     Standard_Boolean SyntaxError() const;
  //! Specifies a Model to work on <br>
//!           Also clears the result and Done status <br>
  Standard_EXPORT     void SetModel(const Handle(Interface_InterfaceModel)& model) ;
  //! Returns the Model to be worked on <br>
  Standard_EXPORT     Handle_Interface_InterfaceModel Model() const;
  //! clears the result and Done status. But not the Model. <br>
  Standard_EXPORT     void Clear() ;
  //! Checks the Model. Returns True if there is NO FAIL at all <br>
//!           (regardless Warnings) <br>
//!           If <withprint> is True, also sends Checks on standard output <br>
  Standard_EXPORT     Standard_Boolean CheckStatusModel(const Standard_Boolean withprint) const;
  //! Checks the Model (complete : syntax + semantic) and returns <br>
//!           the produced Check List <br>
  Standard_EXPORT     Interface_CheckIterator CheckListModel() const;
  //! Returns (by Reference, hence can be changed) the Mode for new <br>
//!           Transfer : True (D) means that each new Transfer produces a <br>
//!           new TransferProcess. Else keeps the original one but each <br>
//!           Transfer clears its (former results are not kept) <br>
  Standard_EXPORT     Standard_Boolean& ModeNewTransfer() ;
  //! Initializes the Reader for a Transfer (one,roots, or list) <br>
//!           Also calls PrepareTransfer <br>
//!           Returns True when done, False if could not be done <br>
  Standard_EXPORT     Standard_Boolean BeginTransfer() ;
  //! Ebds a Transfer (one, roots or list) by recording its result <br>
  Standard_EXPORT     void EndTransfer() ;
  //! Prepares the Transfer. Also can act on the Actor or change the <br>
//!           TransientProcess if required. <br>
//!           Should not set the Actor into the TransientProcess, it is done <br>
//!           by caller. The provided default does nothing. <br>
  Standard_EXPORT   virtual  void PrepareTransfer() ;
  //! Transfers all Root Entities which are recognized as Geom-Topol <br>
//!           The result will be a list of Shapes. <br>
//!           This method calls user redefinable PrepareTransfer <br>
//!           Remark : former result is cleared <br>
  Standard_EXPORT   virtual  void TransferRoots() ;
  //! Transfers an Entity given its rank in the Model (Root or not) <br>
//!           Returns True if it is recognized as Geom-Topol. <br>
//!           (But it can have failed : see IsDone) <br>
  Standard_EXPORT   virtual  Standard_Boolean Transfer(const Standard_Integer num) ;
  //! Transfers a list of Entities (only the ones also in the Model) <br>
//!           Remark : former result is cleared <br>
  Standard_EXPORT   virtual  void TransferList(const Handle(TColStd_HSequenceOfTransient)& list) ;
  //! Returns True if the LAST Transfer/TransferRoots was a success <br>
  Standard_EXPORT     Standard_Boolean IsDone() const;
  //! Returns the count of produced Shapes (roots) <br>
  Standard_EXPORT     Standard_Integer NbShapes() const;
  //! Returns the complete list of produced Shapes <br>
  Standard_EXPORT     Handle_TopTools_HSequenceOfShape Shapes() const;
  //! Returns a Shape given its rank, by default the first one <br>
  Standard_EXPORT    const TopoDS_Shape& Shape(const Standard_Integer num = 1) const;
  //! Returns a Shape produced from a given entity (if it was <br>
//!           individually transferred or if an intermediate result is <br>
//!           known). If no Shape is bound with <ent>, returns a Null Shape <br>
//!  Warning : Runs on the last call to Transfer,TransferRoots,TransferList <br>
  Standard_EXPORT     TopoDS_Shape ShapeResult(const Handle(Standard_Transient)& ent) const;
  //! Returns a unique Shape for the result : <br>
//!           - a void Shape (type = SHAPE) if result is empty <br>
//!           - a simple Shape if result has only one : returns this one <br>
//!           - a Compound if result has more than one Shape <br>
  Standard_EXPORT     TopoDS_Shape OneShape() const;
  //! Returns the count of produced Transient Results (roots) <br>
  Standard_EXPORT     Standard_Integer NbTransients() const;
  //! Returns the complete list of produced Transient Results <br>
  Standard_EXPORT     Handle_TColStd_HSequenceOfTransient Transients() const;
  //! Returns a Transient Root Result, given its rank (by default <br>
//!           the first one) <br>
  Standard_EXPORT     Handle_Standard_Transient Transient(const Standard_Integer num = 1) const;
  //! Checks the Result of last Transfer (individual or roots, no <br>
//!           cumulation on several transfers). Returns True if NO fail <br>
//!           occured during Transfer (queries the TransientProcess) <br>
  Standard_EXPORT     Standard_Boolean CheckStatusResult(const Standard_Boolean withprints) const;
  //! Checks the Result of last Transfer (individual or roots, no <br>
//!           cumulation on several transfers) and returns the produced list <br>
  Standard_EXPORT     Interface_CheckIterator CheckListResult() const;
  //! Returns the TransientProcess. It records informations about <br>
//!           the very last transfer done. Null if no transfer yet done. <br>
//!           Can be used for queries more accurate than the default ones. <br>
  Standard_EXPORT     Handle_Transfer_TransientProcess TransientProcess() const;
  
  Standard_EXPORT   virtual  void Destroy() ;
Standard_EXPORT virtual ~TransferBRep_Reader() { Destroy(); }





protected:



Standard_Boolean theDone;
Handle_Transfer_TransientProcess theProc;


private:



Handle_Interface_Protocol theProto;
Handle_Transfer_ActorOfTransientProcess theActor;
Handle_Interface_InterfaceModel theModel;
Standard_Integer theFilest;
Standard_Boolean theNewpr;
Handle_TopTools_HSequenceOfShape theShapes;
Handle_TColStd_HSequenceOfTransient theTransi;


};
#endif

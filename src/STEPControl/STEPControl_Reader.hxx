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

#ifndef _STEPControl_Reader_HeaderFile
#define _STEPControl_Reader_HeaderFile

#include <Standard.hxx>
#include <Standard_DefineAlloc.hxx>
#include <Standard_Macro.hxx>
#include <XSControl_Reader.hxx>
#include <XSControl_WorkSession.hxx>
#include <Standard_Boolean.hxx>
#include <Handle_StepData_StepModel.hxx>
#include <Standard_Integer.hxx>
#include <Handle_StepRepr_RepresentationContext.hxx>

class XSControl_WorkSession;
class StepData_StepModel;
class TColStd_SequenceOfAsciiString;
class StepRepr_RepresentationContext;
class TColStd_Array1OfAsciiString;
class TColStd_Array1OfReal;


//! Reads STEP files, checks them and translates their contents <br>
//! into Open CASCADE models. The STEP data can be that of <br>
//! a whole model or that of a specific list of entities in the model. <br>
//! As in XSControl_Reader, you specify the list using a selection. <br>
//! For the translation of iges files it is possible to use next sequence: <br>
//! To change translation parameters <br>
//! class Interface_Static should be used before beginning of <br>
//! translation  (see STEP Parameters and General Parameters) <br>
//!     Creation of reader - STEPControl_Reader reader; <br>
//!  To load s file in a model use method reader.ReadFile("filename.stp") <br>
//! To print load results reader.PrintCheckLoad(failsonly,mode) <br>
//! where mode is equal to the value of enumeration IFSelect_PrintCount <br>
//!   For definition number of candidates : <br>
//! Standard_Integer nbroots = reader. NbRootsForTransfer(); <br>
//! To transfer entities from a model the following methods can be used: <br>
//! for the whole model - reader.TransferRoots(); <br>
//! to transfer a list of entities: reader.TransferList(list); <br>
//! to transfer one entity Handle(Standard_Transient) <br>
//! ent = reader.RootForTransfer(num); <br>
//! reader.TransferEntity(ent), or <br>
//! reader.TransferOneRoot(num), or <br>
//! reader.TransferOne(num), or <br>
//! reader.TransferRoot(num) <br>
//!   To obtain the result the following method can be used: <br>
//! reader.NbShapes() and reader.Shape(num); or reader.OneShape(); <br>
//! To print the results of transfer use method: <br>
//! reader.PrintCheckTransfer(failwarn,mode); <br>
//! where printfail is equal to the value of enumeration <br>
//! IFSelect_PrintFail, mode see above; or reader.PrintStatsTransfer(); <br>
//! Gets correspondence between a STEP entity and a result <br>
//! shape obtained from it. <br>
//! Handle(XSControl_WorkSession) <br>
//! WS = reader.WS(); <br>
//! if ( WS->TransferReader()->HasResult(ent) ) <br>
//! TopoDS_Shape shape = WS->TransferReader()->ShapeResult(ent); <br>
class STEPControl_Reader  : public XSControl_Reader {
public:

  DEFINE_STANDARD_ALLOC

  //! Creates a reader object with an empty STEP model. <br>
  Standard_EXPORT   STEPControl_Reader();
  //! Creates a Reader for STEP from an already existing Session <br>
//!           Clears the session if it was not yet set for STEP <br>
  Standard_EXPORT   STEPControl_Reader(const Handle(XSControl_WorkSession)& WS,const Standard_Boolean scratch = Standard_True);
  //! Returns the model as a StepModel. <br>
//!           It can then be consulted (header, product) <br>
  Standard_EXPORT     Handle_StepData_StepModel StepModel() const;
  //! Transfers a root given its rank in the list of candidate roots <br>
//!           Default is the first one <br>
//!           Returns True if a shape has resulted, false else <br>
//!           Same as inherited TransferOneRoot, kept for compatibility <br>
  Standard_EXPORT     Standard_Boolean TransferRoot(const Standard_Integer num = 1) ;
  //! Determines the list of root entities from Model which are candidate for <br>
//!           a transfer to a Shape (type of entities is PRODUCT) <br>
  Standard_EXPORT   virtual  Standard_Integer NbRootsForTransfer() ;
  //! Returns sequence of all unit names for shape representations <br>
//!           found in file <br>
  Standard_EXPORT     void FileUnits(TColStd_SequenceOfAsciiString& theUnitLengthNames,TColStd_SequenceOfAsciiString& theUnitAngleNames,TColStd_SequenceOfAsciiString& theUnitSolidAngleNames) ;





protected:





private:

  //! Returns  units for length , angle and solidangle for shape representations <br>
  Standard_EXPORT     Standard_Boolean findUnits(const Handle(StepRepr_RepresentationContext)& theReprContext,TColStd_Array1OfAsciiString& theNameUnits,TColStd_Array1OfReal& theFactorUnits) ;




};





// other Inline functions and methods (like "C++: function call" methods)


#endif

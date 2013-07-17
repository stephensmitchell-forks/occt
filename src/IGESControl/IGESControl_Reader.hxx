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

#ifndef _IGESControl_Reader_HeaderFile
#define _IGESControl_Reader_HeaderFile

#include <Standard.hxx>
#include <Standard_DefineAlloc.hxx>
#include <Standard_Macro.hxx>
#include <Standard_Boolean.hxx>
#include <XSControl_Reader.hxx>
#include <XSControl_WorkSession.hxx>
#include <Handle_IGESData_IGESModel.hxx>
#include <Standard_Integer.hxx>
#include <IFSelect_PrintFail.hxx>
#include <IFSelect_PrintCount.hxx>

class XSControl_WorkSession;
class IGESData_IGESModel;



//! Reads IGES files, checks them and translates their contents into Open CASCADE models. <br>
//! The IGES data can be that of a whole model or that of a specific list of entities in the model. <br>
//! As in XSControl_Reader, you specify the list using a selection. <br>
//! For translation of iges files it is possible to use the following sequence: <br>
//! To change parameters of translation <br>
//! class Interface_Static should be used before the beginning of translation <br>
//! (see IGES Parameters and General Parameters) <br>
//! Creation of reader <br>
//!      IGESControl_Reader reader; <br>
//! To load a file in a model use method: <br>
//!      reader.ReadFile("filename.igs") <br>
//! To check a loading file use method Check: <br>
//!      reader.Check(failsonly); where failsonly is equal to Standard_True or <br>
//!      Standard_False; <br>
//! To print the results of load: <br>
//!      reader.PrintCheckLoad(failsonly,mode) where mode is equal to the value of <br>
//!      enumeration IFSelect_PrintCount <br>
//! To transfer entities from a model the following methods can be used: <br>
//! for the whole model <br>
//!      reader.TransferRoots(onlyvisible); where onlyvisible is equal to <br>
//!      Standard_True or Standard_False; <br>
//! To transfer a list of entities: <br>
//!      reader.TransferList(list); <br>
//! To transfer one entity <br>
//!      reader.TransferEntity(ent) or reader.Transfer(num); <br>
//! To obtain a result the following method can be used: <br>
//!      reader.IsDone() <br>
//!      reader.NbShapes() and reader.Shape(num); or reader.OneShape(); <br>
//! To print the results of transfer use method: <br>
//!      reader.PrintTransferInfo(failwarn,mode); where printfail is equal to the <br>
//!      value of enumeration IFSelect_PrintFail, mode see above. <br>
//! Gets correspondence between an IGES entity and a result shape obtained therefrom. <br>
//!      reader.TransientProcess(); <br>
//! TopoDS_Shape shape = <br>
//! TransferBRep::ShapeResult(reader.TransientProcess(),ent); <br>
class IGESControl_Reader  : public XSControl_Reader
{
public:

  DEFINE_STANDARD_ALLOC

  //! Creates a Reader from scratch <br>
  Standard_EXPORT   IGESControl_Reader();
  
  //! Creates a Reader from an already existing Session <br>
  Standard_EXPORT   IGESControl_Reader(const Handle(XSControl_WorkSession)& WS,
                                       const Standard_Boolean scratch = Standard_True);
                                       
  //! Set the transion of ALL Roots (if myReadOnlyVisible is False) <br>
  //!           or of Visible Roots (if myReadOnlyVisible is True) <br>
  void SetReadVisible(const Standard_Boolean ReadRoot) ;
  
  Standard_Boolean GetReadVisible() const;
  
  //! Returns the model as a IGESModel. <br>
  //!           It can then be consulted (header, product) <br>
  Standard_EXPORT     Handle_IGESData_IGESModel IGESModel() const;
  
  //! Determines the list of root entities from Model which are candidate for <br>
  //!           a transfer to a Shape (type of entities is PRODUCT) <br>
  //!           <myReadOnlyVisible> is taken into account to define roots <br>
  Standard_EXPORT   virtual  Standard_Integer NbRootsForTransfer();
  
  //! Prints Statistics and check list for Transfer <br>
  Standard_EXPORT     void PrintTransferInfo (const IFSelect_PrintFail failwarn,
                                              const IFSelect_PrintCount mode) const;


private:
  Standard_Boolean myReadOnlyVisible;
};

#include <IGESControl_Reader.lxx>

#endif

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

#ifndef _StepData_StepReaderTool_HeaderFile
#define _StepData_StepReaderTool_HeaderFile

#include <Standard.hxx>
#include <Standard_DefineAlloc.hxx>
#include <Standard_Macro.hxx>
#include <StepData_FileRecognizer.hxx>
#include <Interface_GeneralLib.hxx>
#include <Interface_ReaderLib.hxx>
#include <Interface_FileReaderTool.hxx>
#include <Handle_StepData_StepReaderData.hxx>
#include <Handle_StepData_Protocol.hxx>
#include <Standard_Boolean.hxx>
#include <Standard_Integer.hxx>
#include <Handle_Interface_Check.hxx>
#include <Handle_Standard_Transient.hxx>
#include <Handle_Interface_InterfaceModel.hxx>

class StepData_FileRecognizer;
class StepData_StepReaderData;
class StepData_Protocol;
class Interface_Check;
class Standard_Transient;
class Interface_InterfaceModel;


//! Specific FileReaderTool for Step; works with FileReaderData <br>
//!           provides references evaluation, plus access to litteral data <br>
//!           and specific methods defined by FileReaderTool <br>
//!           Remarks : works with a ReaderLib to load Entities <br>
class StepData_StepReaderTool  : public Interface_FileReaderTool
{
public:

  DEFINE_STANDARD_ALLOC

  //! creates StepReaderTool to work with a StepReaderData according <br>
//!           to a Step Protocol. Defines the ReaderLib at this time <br>
  Standard_EXPORT   StepData_StepReaderTool(const Handle(StepData_StepReaderData)& reader,const Handle(StepData_Protocol)& protocol);
  //! Bounds empty entities to records, uses default Recognition <br>
//!           provided by ReaderLib and ReaderModule. Also calls computation <br>
//!           of references (SetEntityNumbers from StepReaderData) <br>
//!           Works only on data entities (skips header) <br>
//!           <optimize> given False allows to test some internal algorithms <br>
//!           which are normally avoided (see also StepReaderData) <br>
  Standard_EXPORT     void Prepare(const Standard_Boolean optimize = Standard_True) ;
  //! Bounds empty entities to records, works with a specific <br>
//!           FileRecognizer, stored and later used in Recognize <br>
//!           Works only on data entities (skips header) <br>
//!           <optimize : same as above <br>
  Standard_EXPORT     void Prepare(const Handle(StepData_FileRecognizer)& reco,const Standard_Boolean optimize = Standard_True) ;
  //! recognizes records, by asking either ReaderLib (default) or <br>
//!           FileRecognizer (if defined) to do so. <ach> is to call <br>
//!           RecognizeByLib <br>
  Standard_EXPORT     Standard_Boolean Recognize(const Standard_Integer num,Handle(Interface_Check)& ach,Handle(Standard_Transient)& ent) ;
  //! bounds empty entities and sub-lists to header records <br>
//!           works like Prepare + SetEntityNumbers, but for header <br>
//!           (N.B.: in Header, no Ident and no reference) <br>
//!           FileRecognizer is to specify Entities which are allowed to be <br>
//!           defined in the Header (not every type can be) <br>
  Standard_EXPORT     void PrepareHeader(const Handle(StepData_FileRecognizer)& reco) ;
  //! fills model's header; that is, gives to it Header entities <br>
//!           and commands their loading. Also fills StepModel's Global <br>
//!           Check from StepReaderData's GlobalCheck <br>
  Standard_EXPORT     void BeginRead(const Handle(Interface_InterfaceModel)& amodel) ;
  //! fills an entity, given record no; works by using a ReaderLib <br>
//!           to load each entity, which must be a Transient <br>
//!           Actually, returned value is True if no fail, False else <br>
  Standard_EXPORT     Standard_Boolean AnalyseRecord(const Standard_Integer num,const Handle(Standard_Transient)& anent,Handle(Interface_Check)& acheck) ;
  //! Ends file reading after reading all the entities <br>
//!           Here, it binds in the model, Idents to Entities (for checks) <br>
  Standard_EXPORT   virtual  void EndRead(const Handle(Interface_InterfaceModel)& amodel) ;

private:

Handle_StepData_FileRecognizer thereco;
Interface_GeneralLib theglib;
Interface_ReaderLib therlib;


};

#endif

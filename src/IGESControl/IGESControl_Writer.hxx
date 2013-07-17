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

#ifndef _IGESControl_Writer_HeaderFile
#define _IGESControl_Writer_HeaderFile

#include <Standard.hxx>
#include <Standard_DefineAlloc.hxx>
#include <Standard_Macro.hxx>
#include <Transfer_FinderProcess.hxx>
#include <Handle_IGESData_IGESModel.hxx>
#include <IGESData_BasicEditor.hxx>
#include <Standard_Integer.hxx>
#include <Standard_Boolean.hxx>
#include <Standard_CString.hxx>
#include <Handle_Standard_Transient.hxx>
#include <Handle_IGESData_IGESEntity.hxx>
#include <Standard_OStream.hxx>

class Transfer_FinderProcess;
class IGESData_IGESModel;
class TopoDS_Shape;
class Standard_Transient;
class IGESData_IGESEntity;


//! This class creates and writes <br>
//! IGES files from CAS.CADE models. An IGES file can be written to <br>
//! an existing IGES file or to a new one. <br>
//! The translation can be performed in one or several <br>
//! operations. Each translation operation <br>
//! outputs a distinct root entity in the IGES file. <br>
//!   To write an IGES file it is possible to use the following sequence: <br>
//! To modify the IGES file header or to change translation <br>
//! parameters it is necessary to use class Interface_Static (see <br>
//! IGESParameters and GeneralParameters). <br>
class IGESControl_Writer  {
public:

  DEFINE_STANDARD_ALLOC

  //! Creates a writer object with the <br>
  //! default unit (millimeters) and write mode (Face). <br>
  //! IGESControl_Writer (const Standard_CString unit, <br>
  //! const Standard_Integer modecr = 0); <br>
  Standard_EXPORT   IGESControl_Writer();
  //! Creates a writer with given <br>
  //! values for units and for write mode. <br>
  //! unit may be any unit that is accepted by the IGES standard. <br>
  //! By default, it is the millimeter. <br>
  //! modecr defines the write mode and may be: <br>
  //! - 0: Faces (default) <br>
  //! - 1: BRep. <br>
  Standard_EXPORT   IGESControl_Writer(const Standard_CString unit,const Standard_Integer modecr = 0);
  //! Creates a writer object with the <br>
  //! prepared IGES model model in write mode. <br>
  //! modecr defines the write mode and may be: <br>
  //! - 0: Faces (default) <br>
  //! - 1: BRep. <br>
  Standard_EXPORT   IGESControl_Writer(const Handle(IGESData_IGESModel)& model,const Standard_Integer modecr = 0);
  //! Returns the IGES model to be written in output. <br>
  Standard_EXPORT     Handle_IGESData_IGESModel Model() const;
  
  Standard_EXPORT     Handle_Transfer_FinderProcess TransferProcess() const;
  //! Returns/Sets the TransferProcess : it contains final results <br>
  //!           and if some, check messages <br>
  Standard_EXPORT     void SetTransferProcess(const Handle(Transfer_FinderProcess)& TP) ;
  //! Translates a Shape to IGES Entities and adds them to the model <br>
  //!           Returns True if done, False if Shape not suitable for IGES or null <br>
  Standard_EXPORT     Standard_Boolean AddShape(const TopoDS_Shape& sh) ;
  //! Translates a Geometry (Surface or Curve) to IGES Entities and <br>
  //!           adds them to the model <br>
  //!           Returns True if done, False if geom is neither a Surface or <br>
  //!           a Curve suitable for IGES or is null <br>
  Standard_EXPORT     Standard_Boolean AddGeom(const Handle(Standard_Transient)& geom) ;
  //! Adds an IGES entity (and the ones it references) to the model <br>
  Standard_EXPORT     Standard_Boolean AddEntity(const Handle(IGESData_IGESEntity)& ent) ;
  //! Computes the entities found in <br>
  //! the model, which is ready to be written. <br>
  //! This contrasts with the default computation of headers only. <br>
  Standard_EXPORT     void ComputeModel() ;
  //! Computes then writes the model to an OStream <br>
  //!           Returns True when done, false in case of error <br>
  Standard_EXPORT     Standard_Boolean Write(Standard_OStream& S,const Standard_Boolean fnes = Standard_False) ;
  //! Prepares and writes an IGES model <br>
  //! either to an OStream, S or to a file name,CString. <br>
  //! Returns True if the operation was performed correctly and <br>
  //! False if an error occurred (for instance, <br>
  //! if the processor could not create the file). <br>
  Standard_EXPORT     Standard_Boolean Write(const Standard_CString file,const Standard_Boolean fnes = Standard_False) ;
  //! Prints Statistics about Transfer <br>
  Standard_EXPORT     void PrintStatsTransfer(const Standard_Integer what,const Standard_Integer mode = 0) const;

private:

  Handle_Transfer_FinderProcess theTP;
  Handle_IGESData_IGESModel themod;
  IGESData_BasicEditor thedit;
  Standard_Integer thecr;
  Standard_Boolean thest;
};
#endif

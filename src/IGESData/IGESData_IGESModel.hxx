// Created on: 1992-04-06
// Created by: Christian CAILLET
// Copyright (c) 1992-1999 Matra Datavision
// Copyright (c) 1999-2014 OPEN CASCADE SAS
//
// This file is part of Open CASCADE Technology software library.
//
// This library is free software; you can redistribute it and/or modify it under
// the terms of the GNU Lesser General Public License version 2.1 as published
// by the Free Software Foundation, with special exception defined in the file
// OCCT_LGPL_EXCEPTION.txt. Consult the file LICENSE_LGPL_21.txt included in OCCT
// distribution for complete text of the license and disclaimer of any warranty.
//
// Alternatively, this file may be used under the terms of Open CASCADE
// commercial license or contractual agreement.

#ifndef _IGESData_IGESModel_HeaderFile
#define _IGESData_IGESModel_HeaderFile

#include <Standard.hxx>
#include <Standard_Type.hxx>

#include <TColStd_HSequenceOfHAsciiString.hxx>
#include <IGESData_GlobalSection.hxx>
#include <Interface_InterfaceModel.hxx>
#include <Standard_Integer.hxx>
#include <Standard_CString.hxx>
#include <Standard_Boolean.hxx>
#include <Standard_Real.hxx>
class Interface_InterfaceError;
class Message_Messenger;
class IGESData_GlobalSection;
class IGESData_IGESEntity;
class Interface_InterfaceModel;
class Interface_Check;
class Standard_Transient;
class TCollection_HAsciiString;


class IGESData_IGESModel;
DEFINE_STANDARD_HANDLE(IGESData_IGESModel, Interface_InterfaceModel)

//! Defines the file header and
//! entities for IGES files. These headers and entities result from
//! a complete data translation using the IGES data exchange processor.
//! Each entity is contained in a single model only and has a
//! unique identifier. You can access this identifier using the method Number.
//! Gives an access to the general data in the Start and the Global
//! sections of an IGES file.
//! The IGES file includes the following sections:
//! -Start,
//! -Global,
//! -Directory Entry,
//! -Parameter Data,
//! -Terminate
class IGESData_IGESModel : public Interface_InterfaceModel
{
 public:

  IGESData_IGESModel() : thestart(new TColStd_HSequenceOfHAsciiString()) {}

  //! Erases all data specific to IGES file Header (Start + Global)
  Standard_EXPORT void ClearHeader() Standard_OVERRIDE;
  
  //! Returns Model's Start Section (list of comment lines)
  const Handle(TColStd_HSequenceOfHAsciiString) & StartSection() const { return thestart; }
  
  //! Sets a new Start section from a list of strings. The Start section will be shared.
  //!  Any modifications made to the strings later on, will have an effect on the Start section.
  void SetStartSection (const Handle(TColStd_HSequenceOfHAsciiString)& list)
  {
    if (list.IsNull()) thestart = new TColStd_HSequenceOfHAsciiString();
    else thestart = list;
  }
  
  //! Returns the Global section of the IGES file.
  const IGESData_GlobalSection& GlobalSection() const { return theheader; }

  //! Sets the Global section of the IGES file.
  void SetGlobalSection (const IGESData_GlobalSection& header) { theheader = header; }
  
  //! Prints the IGES file header
  //! (Start and Global Sections) to the log file. The integer
  //! parameter is intended to be used as a level indicator but is not used at present.
  Standard_EXPORT virtual void DumpHeader (const Handle(Message_Messenger)& S, const Standard_Integer level = 0) const Standard_OVERRIDE;
  
  //! Returns an IGES entity given by its rank number.
  Standard_EXPORT Handle(IGESData_IGESEntity) Entity (const Standard_Integer num) const;
  
  //! Returns the equivalent DE Number for an Entity, i.e.
  //! 2*Number(ent)-1 , or 0 if <ent> is unknown from <me>
  //! This DE Number is used for File Writing for instance
  Standard_EXPORT Standard_Integer DNum (const Handle(IGESData_IGESEntity)& ent) const;
  
  //! gets Header (GlobalSection) from another Model
  Standard_EXPORT void GetFromAnother (const Handle(IGESData_IGESModel)& other);
  
  //! Checks that the IGES file Global
  //! section contains valid data that conforms to the IGES specifications.
  Standard_EXPORT virtual void VerifyCheck (Handle(Interface_Check)& ach) const Standard_OVERRIDE;
  
  //! erases specific labels, i.e. does nothing
  Standard_EXPORT virtual void ClearLabels() Standard_OVERRIDE;
  
  //! Prints label specific to IGES norm for a given entity, i.e.
  //! its directory entry number (2*Number-1)
  Standard_EXPORT virtual void PrintLabel (const Handle(Standard_Transient)& ent, const Handle(Message_Messenger)& S) const Standard_OVERRIDE;
  
  //! Prints label specific to IGES norm  for a given -- --
  //! entity,  i.e.  its directory entry number (2*Number-1)
  //! in the log file format.
  Standard_EXPORT virtual void PrintToLog (const Handle(Standard_Transient)& ent, const Handle(Message_Messenger)& S) const Standard_OVERRIDE;
  
  //! Returns a string with the label attached to a given entity,
  //! i.e. a string "Dnn" with nn = directory entry number (2*N-1)
  Standard_EXPORT virtual Handle(TCollection_HAsciiString) StringLabel (const Handle(Standard_Transient)& ent) const Standard_OVERRIDE;

  DEFINE_STANDARD_RTTIEXT(IGESData_IGESModel,Interface_InterfaceModel)

 private:

  Handle(TColStd_HSequenceOfHAsciiString) thestart;
  IGESData_GlobalSection theheader;
};

#endif // _IGESData_IGESModel_HeaderFile

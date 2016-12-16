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

#ifndef _IGESData_IGESReaderData_HeaderFile
#define _IGESData_IGESReaderData_HeaderFile

#include <Standard.hxx>
#include <Standard_Type.hxx>

#include <IGESData_IGESType.hxx>
#include <TColStd_HArray1OfInteger.hxx>
#include <TColStd_HArray1OfTransient.hxx>
#include <TColStd_HSequenceOfHAsciiString.hxx>
#include <IGESData_GlobalSection.hxx>
#include <IGESData_Array1OfDirPart.hxx>
//#include <IGESData_ReadStage.hxx>
#include <Interface_ParamType.hxx>
class Interface_ParamList;
class Interface_ParamSet;
class Interface_Check;
class IGESData_GlobalSection;
class IGESData_DirPart;
class IGESData_IGESType;

#include <NCollection_DefineHArray1.hxx>
DEFINE_HARRAY1(IGESData_HArray1OfDirPart,IGESData_Array1OfDirPart)

class IGESData_IGESReaderData;
DEFINE_STANDARD_HANDLE(IGESData_IGESReaderData, MMgt_TShared)

//! specific FileReaderData for IGES
//! contains header as GlobalSection, and for each Entity, its
//! directory part as DirPart, list of Parameters as ParamSet
//! Each Item has a DirPart, plus classically a ParamSet and the
//! correspondant recognized Entity (inherited from FileReaderData)
//! Parameters are accessed through specific objects, ParamReaders
class IGESData_IGESReaderData : public MMgt_TShared //Interface_FileReaderData
{
 public:

  //! Default constructor
  Standard_EXPORT IGESData_IGESReaderData();

  Standard_EXPORT Standard_Integer Read (const Standard_CString theFileName, const Standard_Boolean theModeFnes = Standard_False);

  //! Returns the Start Section in once
  const Handle(TColStd_HSequenceOfHAsciiString) & StartSection() const { return myStartSection; }
  
  //! returns header as GlobalSection
  const IGESData_GlobalSection & GlobalSection() const { return thehead; }
  
  //! returns DirPart identified by record no (half Dsect number)
  const IGESData_DirPart & DirPart (const Standard_Integer num) const;

  Standard_EXPORT virtual Standard_Integer NbRecords () const Standard_OVERRIDE;

  //! Returns count of recorded Entities (i.e. size of Directory)
  Standard_EXPORT virtual Standard_Integer NbEntities() const Standard_OVERRIDE;
  
  //! determines next suitable record from num; that is num+1 except
  //! for last one which gives 0
  Standard_EXPORT virtual Standard_Integer FindNextRecord (const Standard_Integer num) const Standard_OVERRIDE;
  
  const Handle(Standard_Transient)& BoundEntity (const Standard_Integer num) const { return theents->Value(num); }

  void BindEntity (const Standard_Integer num, const Handle(Standard_Transient)& ent) { theents->SetValue(num,ent); }

  //! Returns count of parameters attached to record "num"
  //! If <num> = 0, returns the total recorded count of parameters
  Standard_EXPORT Standard_Integer NbParams (const Standard_Integer num) const;

  Standard_EXPORT Handle(Interface_ParamList) Params () const;

  Standard_Integer ParamFirstRank (const Standard_Integer num) const { return thenumpar->Value(num); }

  //! Returns the recorded Global Check
  const Handle(Interface_Check) & GlobalCheck() const { return thechk; }
  
  //! Returns the recorded Default Line Weight, if there is (else, returns 0)
  Standard_Real DefaultLineWeight() const { return 0.; }

  DEFINE_STANDARD_RTTIEXT(IGESData_IGESReaderData,MMgt_TShared)

 private:

  Standard_Integer ReadLine (Standard_Integer &theNumLine);

  Handle(Interface_ParamSet) theparams;
  Handle(TColStd_HArray1OfInteger) thenumpar;
  Handle(TColStd_HArray1OfTransient) theents;

  Handle(TColStd_HSequenceOfHAsciiString) myStartSection;
  Handle(Interface_ParamSet) theparh;
  IGESData_GlobalSection thehead;
  Handle(IGESData_HArray1OfDirPart) thedirs;
  Handle(Interface_Check) thechk;

  Standard_Boolean myModeFnes;
  FILE* myFile;
  char myBuffer[100];
};

#endif // _IGESData_IGESReaderData_HeaderFile

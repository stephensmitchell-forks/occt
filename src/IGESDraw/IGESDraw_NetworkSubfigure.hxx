// Created on: 1993-01-11
// Created by: CKY / Contract Toubro-Larsen ( TCD )
// Copyright (c) 1993-1999 Matra Datavision
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

#ifndef _IGESDraw_NetworkSubfigure_HeaderFile
#define _IGESDraw_NetworkSubfigure_HeaderFile

#include <gp_XYZ.hxx>
#include <IGESDraw_HArray1OfConnectPoint.hxx>
#include <IGESData_IGESEntity.hxx>
class IGESDraw_NetworkSubfigureDef;
class TCollection_HAsciiString;
class IGESGraph_TextDisplayTemplate;
class IGESDraw_ConnectPoint;


//! defines IGES Network Subfigure Instance Entity,
//! Type <420> Form Number <0> in package IGESDraw
//!
//! Used to specify each instance of Network Subfigure
//! Definition Entity (Type 320, Form 0).
class IGESDraw_NetworkSubfigure : public IGESData_IGESEntity
{
 public:

  Standard_EXPORT virtual Standard_Integer TypeNumber() const Standard_OVERRIDE { return 420; }

  Standard_EXPORT virtual Standard_Integer FormNumber() const Standard_OVERRIDE { return 0; }

  IGESDraw_NetworkSubfigure() {}

  //! returns Network Subfigure Definition Entity specified by this entity
  const Handle(IGESDraw_NetworkSubfigureDef) & SubfigureDefinition() const { return theSubfigureDefinition; }

  //! returns Translation Data relative to either model space or to
  //! the definition space of a referring entity
  const gp_XYZ & Translation() const { return theTranslation; }

  //! returns the Transformed Translation Data relative to either model
  //! space or to the definition space of a referring entity
  Standard_EXPORT gp_XYZ TransformedTranslation() const;
  
  //! returns Scale factor in definition space(x, y, z axes)
  const gp_XYZ & ScaleFactors() const { return theScaleFactor; }

  //! returns Type Flag which implements the distinction between Logical
  //! design and Physical design data,and is required if both are present.
  //! Type Flag = 0 : Not specified (default)
  //! = 1 : Logical
  //! = 2 : Physical
  Standard_Integer TypeFlag() const { return theTypeFlag; }

  //! returns the primary reference designator
  const Handle(TCollection_HAsciiString) & ReferenceDesignator() const { return theDesignator; }

  //! returns True if Text Display Template Entity is specified,
  //! else False
  Standard_Boolean HasDesignatorTemplate() const { return (! theDesignatorTemplate.IsNull() ); }

  //! returns primary reference designator Text Display Template Entity,
  //! or null. If null, no Text Display Template Entity specified
  const Handle(IGESGraph_TextDisplayTemplate) & DesignatorTemplate() const { return theDesignatorTemplate; }

  //! returns the number of associated Connect Point Entities
  Standard_EXPORT Standard_Integer NbConnectPoints() const;
  
  //! returns the Index'th  associated Connect point Entity
  //! raises exception if Index <= 0 or Index > NbConnectPoints()
  Standard_EXPORT const Handle(IGESDraw_ConnectPoint) & ConnectPoint (const Standard_Integer Index) const;

  Standard_EXPORT virtual void OwnRead (IGESFile_Reader &) Standard_OVERRIDE;
  
  Standard_EXPORT virtual void OwnWrite (IGESData_IGESWriter &) const Standard_OVERRIDE;

  Standard_EXPORT virtual void OwnShared (Interface_EntityIterator &) const Standard_OVERRIDE;

  Standard_EXPORT virtual IGESData_DirChecker DirChecker () const Standard_OVERRIDE;

  Standard_EXPORT virtual void OwnCheck (const Interface_ShareTool &, Handle(Interface_Check) &) const Standard_OVERRIDE;

  Standard_EXPORT virtual void OwnDump (const IGESData_IGESDumper &, const Handle(Message_Messenger) &, const Standard_Integer) const Standard_OVERRIDE;

  DEFINE_STANDARD_RTTIEXT(IGESDraw_NetworkSubfigure,IGESData_IGESEntity)

 private:

  Interface_Pointer<IGESDraw_NetworkSubfigureDef> theSubfigureDefinition;
  gp_XYZ theTranslation;
  gp_XYZ theScaleFactor;
  Standard_Integer theTypeFlag;
  Handle(TCollection_HAsciiString) theDesignator;
  Interface_Pointer<IGESGraph_TextDisplayTemplate> theDesignatorTemplate;
  Handle(IGESDraw_HArray1OfConnectPoint) theConnectPoints;
};

#endif // _IGESDraw_NetworkSubfigure_HeaderFile

// Created on: 1993-01-11
// Created by: CKY / Contract Toubro-Larsen ( Niraj RANGWALA )
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

#ifndef _IGESDraw_NetworkSubfigureDef_HeaderFile
#define _IGESDraw_NetworkSubfigureDef_HeaderFile

#include <IGESData_HArray1OfIGESEntity.hxx>
#include <IGESDraw_HArray1OfConnectPoint.hxx>
#include <IGESData_IGESEntity.hxx>
class TCollection_HAsciiString;
class IGESGraph_TextDisplayTemplate;
class IGESDraw_ConnectPoint;


//! defines IGESNetworkSubfigureDef,
//! Type <320> Form Number <0> in package IGESDraw
//!
//! This class differs from the ordinary subfigure definition
//! in that it defines a specialized subfigure, one whose
//! instances may participate in networks.
//!
//! The Number of associated(child) Connect Point Entities
//! in the Network Subfigure Instance must match the number
//! in the Network Subfigure Definition, their order must
//! be identical, and any unused points of connection in
//! the instance must be indicated by a null(zero) pointer.
class IGESDraw_NetworkSubfigureDef : public IGESData_IGESEntity
{
 public:

  Standard_EXPORT virtual Standard_Integer TypeNumber() const Standard_OVERRIDE { return 320; }

  Standard_EXPORT virtual Standard_Integer FormNumber() const Standard_OVERRIDE { return 0; }

  IGESDraw_NetworkSubfigureDef() {}
  
  //! returns Depth of Subfigure(indication the amount of nesting)
  //! Note : The Depth is inclusive of both Network Subfigure Definition
  //! Entity and the Ordinary Subfigure Definition Entity.
  //! Thus, the two may be nested.
  Standard_Integer Depth() const { return theDepth; }

  //! returns the Subfigure Name
  const Handle(TCollection_HAsciiString) & Name() const { return theName; }

  //! returns Number of Associated(child) entries in subfigure exclusive
  //! of primary reference designator and Control Points
  Standard_EXPORT Standard_Integer NbEntities() const;
  
  //! returns the Index'th IGESEntity in subfigure exclusive of primary
  //! reference designator and Control Points
  //! raises exception if Index  <=0 or Index > NbEntities()
  Standard_EXPORT const Handle(IGESData_IGESEntity) & Entity (const Standard_Integer Index) const;
  
  //! return value = 0 : Not Specified
  //! = 1 : Logical  design
  //! = 2 : Physical design
  Standard_Integer TypeFlag() const { return theTypeFlag; }

  //! returns Primary Reference Designator
  const Handle(TCollection_HAsciiString) & Designator() const { return theDesignator; }

  //! returns True if Text Display Template is specified for
  //! primary designator else returns False
  Standard_Boolean HasDesignatorTemplate() const { return (! theDesignatorTemplate.IsNull()); }

  //! if Text Display Template specified then return TextDisplayTemplate
  //! else return NULL Handle
  const Handle(IGESGraph_TextDisplayTemplate) & DesignatorTemplate() const { return theDesignatorTemplate; }

  //! returns the Number Of Associated(child) Connect Point Entities
  Standard_EXPORT Standard_Integer NbPointEntities() const;
  
  //! returns True is Index'th Associated Connect Point Entity is present
  //! else returns False
  //! raises exception if Index is out of bound
  Standard_EXPORT Standard_Boolean HasPointEntity (const Standard_Integer Index) const;
  
  //! returns the Index'th Associated Connect Point Entity
  //! raises exception if Index <= 0 or Index > NbPointEntities()
  Standard_EXPORT const Handle(IGESDraw_ConnectPoint) & PointEntity (const Standard_Integer Index) const;

  Standard_EXPORT virtual void OwnRead (IGESFile_Reader &) Standard_OVERRIDE;
  
  Standard_EXPORT virtual void OwnWrite (IGESData_IGESWriter &) const Standard_OVERRIDE;

  Standard_EXPORT virtual void OwnShared (Interface_EntityIterator &) const Standard_OVERRIDE;

  Standard_EXPORT virtual IGESData_DirChecker DirChecker () const Standard_OVERRIDE;

  Standard_EXPORT virtual void OwnCheck (const Interface_ShareTool &, Handle(Interface_Check) &) const Standard_OVERRIDE;

  Standard_EXPORT virtual void OwnDump (const IGESData_IGESDumper &, const Handle(Message_Messenger) &, const Standard_Integer) const Standard_OVERRIDE;

  DEFINE_STANDARD_RTTIEXT(IGESDraw_NetworkSubfigureDef,IGESData_IGESEntity)

 private:

  Standard_Integer theDepth;
  Handle(TCollection_HAsciiString) theName;
  Handle(IGESData_HArray1OfIGESEntity) theEntities;
  Standard_Integer theTypeFlag;
  Handle(TCollection_HAsciiString) theDesignator;
  Interface_Pointer<IGESGraph_TextDisplayTemplate> theDesignatorTemplate;
  Handle(IGESDraw_HArray1OfConnectPoint) thePointEntities;
};

#endif // _IGESDraw_NetworkSubfigureDef_HeaderFile

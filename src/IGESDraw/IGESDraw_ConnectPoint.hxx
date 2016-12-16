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

#ifndef _IGESDraw_ConnectPoint_HeaderFile
#define _IGESDraw_ConnectPoint_HeaderFile

#include <IGESData_IGESEntity.hxx>
#include <gp_XYZ.hxx>
class TCollection_HAsciiString;
class IGESGraph_TextDisplayTemplate;
class gp_Pnt;


//! defines IGESConnectPoint, Type <132> Form Number <0>
//! in package IGESDraw
//!
//! Connect Point Entity describes a point of connection for
//! zero, one or more entities. Its referenced from Composite
//! curve, or Network Subfigure Definition/Instance, or Flow
//! Associative Instance, or it may stand alone.
class IGESDraw_ConnectPoint : public IGESData_IGESEntity
{
 public:

  Standard_EXPORT virtual Standard_Integer TypeNumber() const Standard_OVERRIDE { return 132; }

  Standard_EXPORT virtual Standard_Integer FormNumber() const Standard_OVERRIDE { return 0; }

  IGESDraw_ConnectPoint() {}
  
  //! returns the coordinate of the connection point
  Standard_EXPORT gp_Pnt Point() const;
  
  //! returns the Transformed coordinate of the connection point
  Standard_EXPORT gp_Pnt TransformedPoint() const;
  
  //! returns True if Display symbol is specified
  //! else returns False
  Standard_Boolean HasDisplaySymbol() const { return (! theDisplaySymbol.IsNull()); }

  //! if display symbol specified returns display symbol geometric entity
  //! else returns NULL Handle
  const Handle(IGESData_IGESEntity) & DisplaySymbol() const { return theDisplaySymbol; }

  //! return value specifies a particular type of connection :
  //! Type Flag = 0   : Not Specified(default)
  //! 1   : Nonspecific logical  point of connection
  //! 2   : Nonspecific physical point of connection
  //! 101 : Logical component pin
  //! 102 : Logical part connector
  //! 103 : Logical offpage connector
  //! 104 : Logical global signal connector
  //! 201 : Physical PWA surface mount pin
  //! 202 : Physical PWA blind pin
  //! 203 : Physical PWA thru-pin
  //! 5001-9999 : Implementor defined.
  Standard_Integer TypeFlag() const { return theTypeFlag; }

  //! returns Function Code that specifies a particular function for the
  //! ECO576 connection :
  //! e.g.,        Function Flag = 0 : Unspecified(default)
  //! = 1 : Electrical Signal
  //! = 2 : Fluid flow Signal
  Standard_Integer FunctionFlag() const { return theFunctionFlag; }

  //! return HAsciiString identifying Pin Number or Nozzle Label etc.
  const Handle(TCollection_HAsciiString) & FunctionIdentifier() const { return theFunctionIdentifier; }

  //! returns True if Text Display Template is specified for Identifier
  //! else returns False
  Standard_Boolean HasIdentifierTemplate() const { return (! theIdentifierTemplate.IsNull()); }

  //! if Text Display Template for the Function Identifier is defined,
  //! returns TestDisplayTemplate
  //! else returns NULL Handle
  const Handle(IGESGraph_TextDisplayTemplate) & IdentifierTemplate() const { return theIdentifierTemplate; }

  //! returns Connection Point Function Name
  const Handle(TCollection_HAsciiString) & FunctionName() const { return theFunctionName; }

  //! returns True if Text Display Template is specified for Function Name
  //! else returns False
  Standard_Boolean HasFunctionTemplate() const { return (! theFunctionTemplate.IsNull()); }

  //! if Text Display Template for the Function Name is defined,
  //! returns TestDisplayTemplate
  //! else returns NULL Handle
  const Handle(IGESGraph_TextDisplayTemplate) & FunctionTemplate() const { return theFunctionTemplate; }

  //! returns the Unique Connect Point Identifier
  Standard_Integer PointIdentifier() const { return thePointIdentifier; }

  //! returns the Connect Point Function Code
  Standard_Integer FunctionCode() const { return theFunctionCode; }

  //! return value = 0 : Connect point may be swapped(default)
  //! = 1 : Connect point may not be swapped
  Standard_Boolean SwapFlag() const { return (theSwapFlag != 0); }

  //! returns True if Network Subfigure Instance/Definition Entity
  //! is specified
  //! else returns False
  Standard_Boolean HasOwnerSubfigure() const { return (! theOwnerSubfigure.IsNull()); }

  //! returns "owner" Network Subfigure Instance Entity,
  //! or Network Subfigure Definition Entity, or NULL Handle.
  const Handle(IGESData_IGESEntity) & OwnerSubfigure() const { return theOwnerSubfigure; }

  Standard_EXPORT virtual void OwnRead (IGESFile_Reader &) Standard_OVERRIDE;
  
  Standard_EXPORT virtual void OwnWrite (IGESData_IGESWriter &) const Standard_OVERRIDE;

  Standard_EXPORT virtual void OwnShared (Interface_EntityIterator &) const Standard_OVERRIDE;

  Standard_EXPORT virtual IGESData_DirChecker DirChecker () const Standard_OVERRIDE;

  Standard_EXPORT virtual void OwnCheck (const Interface_ShareTool &, Handle(Interface_Check) &) const Standard_OVERRIDE;

  Standard_EXPORT virtual void OwnDump (const IGESData_IGESDumper &, const Handle(Message_Messenger) &, const Standard_Integer) const Standard_OVERRIDE;

  DEFINE_STANDARD_RTTIEXT(IGESDraw_ConnectPoint,IGESData_IGESEntity)

 private:

  gp_XYZ thePoint;
  Interface_Pointer<IGESData_IGESEntity> theDisplaySymbol;
  Standard_Integer theTypeFlag;
  Standard_Integer theFunctionFlag;
  Handle(TCollection_HAsciiString) theFunctionIdentifier;
  Interface_Pointer<IGESGraph_TextDisplayTemplate> theIdentifierTemplate;
  Handle(TCollection_HAsciiString) theFunctionName;
  Interface_Pointer<IGESGraph_TextDisplayTemplate> theFunctionTemplate;
  Standard_Integer thePointIdentifier;
  Standard_Integer theFunctionCode;
  Standard_Integer theSwapFlag;
  Interface_Pointer<IGESData_IGESEntity> theOwnerSubfigure;
};

#endif // _IGESDraw_ConnectPoint_HeaderFile

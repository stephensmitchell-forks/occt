// Created on: 1993-01-11
// Created by: CKY / Contract Toubro-Larsen ( Anand NATRAJAN )
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

#ifndef _IGESAppli_Flow_HeaderFile
#define _IGESAppli_Flow_HeaderFile

#include <IGESData_HArray1OfIGESEntity.hxx>
#include <IGESDraw_HArray1OfConnectPoint.hxx>
#include <Interface_HArray1OfHAsciiString.hxx>
#include <IGESGraph_HArray1OfTextDisplayTemplate.hxx>
#include <IGESData_IGESEntity.hxx>
class IGESData_IGESEntity;
class IGESDraw_ConnectPoint;
class TCollection_HAsciiString;
class IGESGraph_TextDisplayTemplate;


class IGESAppli_Flow;
DEFINE_STANDARD_HANDLE(IGESAppli_Flow, IGESData_IGESEntity)

//! defines Flow, Type <402> Form <18>
//! in package IGESAppli
//! Represents a single signal or a single fluid flow path
//! starting from a starting Connect Point Entity and
//! including additional intermediate connect points.
class IGESAppli_Flow : public IGESData_IGESEntity
{
 public:

  Standard_EXPORT virtual Standard_Integer TypeNumber() const Standard_OVERRIDE { return 402; }

  Standard_EXPORT virtual Standard_Integer FormNumber() const Standard_OVERRIDE { return 18; }

  IGESAppli_Flow()
  : myTypeOfFlow(0),
    myFunctionFlag(0)
  {}
  
  //! returns number of Flow Associativity Entities
  Standard_EXPORT Standard_Integer NbFlowAssociativities() const;
  
  //! returns number of Connect Point Entities
  Standard_EXPORT Standard_Integer NbConnectPoints() const;
  
  //! returns number of Join Entities
  Standard_EXPORT Standard_Integer NbJoins() const;
  
  //! returns number of Flow Names
  Standard_EXPORT Standard_Integer NbFlowNames() const;
  
  //! returns number of Text Display Template Entities
  Standard_EXPORT Standard_Integer NbTextDisplayTemplates() const;
  
  //! returns number of Continuation Flow Associativity Entities
  Standard_EXPORT Standard_Integer NbContFlowAssociativities() const;
  
  //! returns Type of Flow = 0 : Not Specified (default)
  //! 1 : Logical
  //! 2 : Physical
  Standard_Integer TypeOfFlow() const { return myTypeOfFlow; }

  //! returns Function Flag = 0 : Not Specified (default)
  //! 1 : Electrical Signal
  //! 2 : Fluid Flow Path
  Standard_Integer FunctionFlag() const { return myFunctionFlag; }

  //! returns Flow Associativity Entity
  //! raises exception if Index <= 0 or Index > NbFlowAssociativities()
  Standard_EXPORT const Handle(IGESData_IGESEntity) & FlowAssociativity (const Standard_Integer Index) const;
  
  //! returns Connect Point Entity
  //! raises exception if Index <= 0 or Index > NbConnectPoints()
  Standard_EXPORT const Handle(IGESDraw_ConnectPoint) & ConnectPoint (const Standard_Integer Index) const;
  
  //! returns Join Entity
  //! raises exception if Index <= 0 or Index > NbJoins()
  Standard_EXPORT const Handle(IGESData_IGESEntity) & Join (const Standard_Integer Index) const;
  
  //! returns Flow Name
  //! raises exception if Index <= 0 or Index > NbFlowNames()
  Standard_EXPORT const Handle(TCollection_HAsciiString) & FlowName (const Standard_Integer Index) const;
  
  //! returns Text Display Template Entity
  //! raises exception if Index <= 0 or Index > NbTextDisplayTemplates()
  Standard_EXPORT const Handle(IGESGraph_TextDisplayTemplate) & TextDisplayTemplate (const Standard_Integer Index) const;
  
  //! returns Continuation Flow Associativity Entity
  //! raises exception if Index <= 0 or Index > NbContFlowAssociativities()
  Standard_EXPORT const Handle(IGESData_IGESEntity) & ContFlowAssociativity (const Standard_Integer Index) const;

  Standard_EXPORT virtual void OwnRead (IGESFile_Reader &) Standard_OVERRIDE;
  
  Standard_EXPORT virtual void OwnWrite (IGESData_IGESWriter &) const Standard_OVERRIDE;

  Standard_EXPORT virtual void OwnShared(Interface_EntityIterator &theIter) const Standard_OVERRIDE;

  Standard_EXPORT virtual IGESData_DirChecker DirChecker () const Standard_OVERRIDE;

  Standard_EXPORT virtual void OwnCheck (const Interface_ShareTool &, const Handle(Interface_Check) &) const Standard_OVERRIDE;

  Standard_EXPORT virtual void OwnDump (const IGESData_IGESDumper &, const Handle(Message_Messenger) &, const Standard_Integer) const Standard_OVERRIDE;

  DEFINE_STANDARD_RTTIEXT(IGESAppli_Flow,IGESData_IGESEntity)

 private:

  Standard_Integer myTypeOfFlow;
  Standard_Integer myFunctionFlag;
  Handle(IGESData_HArray1OfIGESEntity) myFlowAssociativities;
  Handle(IGESDraw_HArray1OfConnectPoint) myConnectPoints;
  Handle(IGESData_HArray1OfIGESEntity) myJoins;
  Handle(Interface_HArray1OfHAsciiString) myFlowNames;
  Handle(IGESGraph_HArray1OfTextDisplayTemplate) myTextDisplayTemplates;
  Handle(IGESData_HArray1OfIGESEntity) myContFlowAssociativities;
};

#endif // _IGESAppli_Flow_HeaderFile

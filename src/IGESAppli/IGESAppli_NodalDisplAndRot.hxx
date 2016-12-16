// Created on: 1993-01-11
// Created by: CKY / Contract Toubro-Larsen ( Arun MENON )
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

#ifndef _IGESAppli_NodalDisplAndRot_HeaderFile
#define _IGESAppli_NodalDisplAndRot_HeaderFile

#include <IGESDimen_HArray1OfGeneralNote.hxx>
#include <TColStd_HArray1OfInteger.hxx>
#include <IGESAppli_HArray1OfNode.hxx>
#include <IGESData_IGESEntity.hxx>
class IGESBasic_HArray1OfHArray1OfXYZ;
class IGESDimen_GeneralNote;
class IGESAppli_Node;
class gp_XYZ;


class IGESAppli_NodalDisplAndRot;
DEFINE_STANDARD_HANDLE(IGESAppli_NodalDisplAndRot, IGESData_IGESEntity)

//! defines NodalDisplAndRot, Type <138> Form <0>
//! in package IGESAppli
//! Used to communicate finite element post processing
//! data.
class IGESAppli_NodalDisplAndRot : public IGESData_IGESEntity
{
 public:

  Standard_EXPORT virtual Standard_Integer TypeNumber() const Standard_OVERRIDE { return 138; }

  Standard_EXPORT virtual Standard_Integer FormNumber() const Standard_OVERRIDE { return 0; }

  IGESAppli_NodalDisplAndRot() {}
  
  //! returns the number of analysis cases
  Standard_EXPORT Standard_Integer NbCases() const;
  
  //! returns the number of nodes
  Standard_EXPORT Standard_Integer NbNodes() const;
  
  //! returns the General Note that describes the Index analysis case
  //! raises exception if Index <= 0 or Index > NbCases
  Standard_EXPORT const Handle(IGESDimen_GeneralNote) & Note (const Standard_Integer Index) const;
  
  //! returns the node identifier as specified by the Index
  //! raises exception if Index <= 0 or Index > NbNodes
  Standard_EXPORT Standard_Integer NodeIdentifier (const Standard_Integer Index) const;
  
  //! returns the node as specified by the Index
  //! raises exception if Index <= 0 or Index > NbNodes
  Standard_EXPORT const Handle(IGESAppli_Node) & Node (const Standard_Integer Index) const;
  
  //! returns the Translational Parameters for the particular Index
  //! Exception raised if NodeNum <= 0 or NodeNum > NbNodes()
  //! or CaseNum <= 0 or CaseNum > NbCases()
  Standard_EXPORT const gp_XYZ & TranslationParameter (const Standard_Integer NodeNum, const Standard_Integer CaseNum) const;
  
  //! returns the Rotational Parameters for Index
  //! Exception raised if NodeNum <= 0 or NodeNum > NbNodes()
  //! or CaseNum <= 0 or CaseNum > NbCases()
  Standard_EXPORT const gp_XYZ & RotationalParameter (const Standard_Integer NodeNum, const Standard_Integer CaseNum) const;

  Standard_EXPORT virtual void OwnRead (IGESFile_Reader &) Standard_OVERRIDE;
  
  Standard_EXPORT virtual void OwnWrite (IGESData_IGESWriter &) const Standard_OVERRIDE;

  Standard_EXPORT virtual void OwnShared(Interface_EntityIterator &theIter) const Standard_OVERRIDE;

  Standard_EXPORT virtual IGESData_DirChecker DirChecker () const Standard_OVERRIDE;

  Standard_EXPORT virtual void OwnDump (const IGESData_IGESDumper &, const Handle(Message_Messenger) &, const Standard_Integer) const Standard_OVERRIDE;

  DEFINE_STANDARD_RTTIEXT(IGESAppli_NodalDisplAndRot,IGESData_IGESEntity)

 private:

  Handle(IGESDimen_HArray1OfGeneralNote) myNotes;
  Handle(TColStd_HArray1OfInteger) myNodeIdentifiers;
  Handle(IGESAppli_HArray1OfNode) myNodes;
  Handle(IGESBasic_HArray1OfHArray1OfXYZ) myTransParam;
  Handle(IGESBasic_HArray1OfHArray1OfXYZ) myRotParam;
};

#endif // _IGESAppli_NodalDisplAndRot_HeaderFile

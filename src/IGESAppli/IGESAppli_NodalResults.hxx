// Created on: 1993-01-11
// Created by: CKY / Contract Toubro-Larsen ( Arun  MENON )
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

#ifndef _IGESAppli_NodalResults_HeaderFile
#define _IGESAppli_NodalResults_HeaderFile

#include <TColStd_HArray1OfInteger.hxx>
#include <IGESAppli_HArray1OfNode.hxx>
#include <TColStd_HArray2OfReal.hxx>
#include <IGESData_IGESEntity.hxx>
class IGESDimen_GeneralNote;
class IGESAppli_Node;

//! defines NodalResults, Type <146>
//! in package IGESAppli
//! Used to store the Analysis Data results per FEM Node
class IGESAppli_NodalResults : public IGESData_IGESEntity
{
 public:

  Standard_EXPORT virtual Standard_Integer TypeNumber() const Standard_OVERRIDE { return 146; }

  Standard_EXPORT virtual Standard_Integer FormNumber() const Standard_OVERRIDE { return myForm; }

  IGESAppli_NodalResults(const Standard_Integer theForm)
  : myForm(theForm),
    mySubCaseNum(0),
    myTime(0.)
  {}
  
  //! returns the General Note Entity that describes the analysis case
  const Handle(IGESDimen_GeneralNote) & Note() const { return myNote; }

  //! returns zero if there is no subcase
  Standard_Integer SubCaseNumber() const { return mySubCaseNum; }

  //! returns the Analysis time value for this subcase. It is the time
  //! at which transient analysis results occur in the mathematical
  //! FEM model.
  Standard_Real Time() const { return myTime; }

  //! returns number of real values in array V for a FEM node
  Standard_EXPORT Standard_Integer NbData() const;
  
  //! returns number of FEM nodes for which data is to be read.
  Standard_EXPORT Standard_Integer NbNodes() const;
  
  //! returns FEM node number identifier for the (Index)th node
  //! raises exception if Index <= 0 or Index > NbNodes
  Standard_EXPORT Standard_Integer NodeIdentifier (const Standard_Integer Index) const;
  
  //! returns the node as specified by the Index
  //! raises exception if Index <= 0 or Index > NbNodes
  Standard_EXPORT const Handle(IGESAppli_Node) & Node (const Standard_Integer Index) const;
  
  //! returns the finite element analysis result value
  //! raises exception if (NodeNum <= 0 or NodeNum > NbNodes()) or
  //! if (DataNum <=0 or DataNum > NbData())
  Standard_EXPORT Standard_Real Data (const Standard_Integer NodeNum, const Standard_Integer DataNum) const;

  Standard_EXPORT virtual void OwnRead (IGESFile_Reader &) Standard_OVERRIDE;
  
  Standard_EXPORT virtual void OwnWrite (IGESData_IGESWriter &) const Standard_OVERRIDE;

  Standard_EXPORT virtual void OwnShared(Interface_EntityIterator &theIter) const Standard_OVERRIDE;

  Standard_EXPORT virtual IGESData_DirChecker DirChecker () const Standard_OVERRIDE;

  Standard_EXPORT virtual void OwnCheck (const Interface_ShareTool &, const Handle(Interface_Check) &) const Standard_OVERRIDE;

  Standard_EXPORT virtual void OwnDump (const IGESData_IGESDumper &, const Handle(Message_Messenger) &, const Standard_Integer) const Standard_OVERRIDE;

  DEFINE_STANDARD_RTTIEXT(IGESAppli_NodalResults,IGESData_IGESEntity)

 private:

  Standard_Integer myForm;
  Interface_Pointer<IGESDimen_GeneralNote> myNote;
  Standard_Integer mySubCaseNum;
  Standard_Real myTime;
  Handle(TColStd_HArray1OfInteger) myNodeIdentifiers;
  Handle(IGESAppli_HArray1OfNode) myNodes;
  Handle(TColStd_HArray2OfReal) myData;
};

#endif // _IGESAppli_NodalResults_HeaderFile

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

#ifndef _IGESAppli_ElementResults_HeaderFile
#define _IGESAppli_ElementResults_HeaderFile

#include <IGESAppli_HArray1OfFiniteElement.hxx>
#include <IGESData_IGESEntity.hxx>
class TColStd_HArray1OfInteger;
class TColStd_HArray1OfReal;
class IGESDimen_GeneralNote;
class IGESBasic_HArray1OfHArray1OfInteger;
class IGESBasic_HArray1OfHArray1OfReal;
class IGESAppli_FiniteElement;


class IGESAppli_ElementResults;
DEFINE_STANDARD_HANDLE(IGESAppli_ElementResults, IGESData_IGESEntity)

//! defines ElementResults, Type <148>
//! in package IGESAppli
//! Used to find the results of FEM analysis
class IGESAppli_ElementResults : public IGESData_IGESEntity
{
 public:

  Standard_EXPORT virtual Standard_Integer TypeNumber() const Standard_OVERRIDE { return 148; }

  Standard_EXPORT virtual Standard_Integer FormNumber() const Standard_OVERRIDE { return myForm; }

  IGESAppli_ElementResults(const Standard_Integer theForm)
  : myForm(theForm),
    mySubcaseNumber(0),
    myTime(0.),
    myNbResultValues(0),
    myResultReportFlag(0)
  {}
  
  //! returns General Note Entity describing analysis case
  const Handle(IGESDimen_GeneralNote) & Note() const { return myNote; }

  //! returns analysis Subcase number
  Standard_Integer SubCaseNumber() const { return mySubcaseNumber; }

  //! returns analysis time value
  Standard_Real Time() const { return myTime; }

  //! returns number of result values per FEM
  Standard_Integer NbResultValues() const { return myNbResultValues; }

  //! returns Results Reporting Flag
  Standard_Integer ResultReportFlag() const { return myResultReportFlag; }

  //! returns number of FEM elements
  Standard_EXPORT Standard_Integer NbElements() const;

  //! returns FEM element number for elements
  Standard_EXPORT Standard_Integer ElementIdentifier (const Standard_Integer Index) const;
  
  //! returns FEM element
  Standard_EXPORT const Handle(IGESAppli_FiniteElement) & Element (const Standard_Integer Index) const;
  
  //! returns element Topology Types
  Standard_EXPORT Standard_Integer ElementTopologyType (const Standard_Integer Index) const;
  
  //! returns number of layers per result data location
  Standard_EXPORT Standard_Integer NbLayers (const Standard_Integer Index) const;
  
  //! returns Data Layer Flags
  Standard_EXPORT Standard_Integer DataLayerFlag (const Standard_Integer Index) const;
  
  //! returns number of result data report locations
  Standard_EXPORT Standard_Integer NbResultDataLocs (const Standard_Integer Index) const;
  
  //! returns Result Data Report Locations
  //! UNFINISHED
  Standard_EXPORT Standard_Integer ResultDataLoc (const Standard_Integer NElem, const Standard_Integer NLoc) const;
  
  //! returns total number of results
  Standard_EXPORT Standard_Integer NbResults (const Standard_Integer Index) const;
  
  //! returns Result data value for an Element, given its
  //! order between 1 and <NbResults(NElem)> (direct access)
  //! For a more comprehensive access, see below
  Standard_EXPORT Standard_Real ResultData (const Standard_Integer NElem, const Standard_Integer num) const;
  
  //! Computes, for a given Element <NElem>, the rank of a
  //! individual Result Data, given <NVal>,<NLay>,<NLoc>
  Standard_EXPORT Standard_Integer ResultRank (const Standard_Integer NElem, const Standard_Integer NVal, const Standard_Integer NLay, const Standard_Integer NLoc) const;
  
  //! returns Result data values of FEM analysis, according this
  //! definition :
  //! - <NElem> : n0 of the Element to be considered
  //! - <NVal> : n0 of the Value between 1 and NbResultValues
  //! - <NLay> : n0 of the Layer for this Element
  //! - <NLoc> : n0 of the Data Location for this Element
  //! This gives for each Element, the corresponding rank
  //! computed by ResultRank, in which the leftmost subscript
  //! changes most rapidly
  Standard_EXPORT Standard_Real ResultData (const Standard_Integer NElem, const Standard_Integer NVal, const Standard_Integer NLay, const Standard_Integer NLoc) const;
  
  //! Returns in once the entire list of data for an Element,
  //! addressed as by ResultRank (See above)
  Standard_EXPORT Handle(TColStd_HArray1OfReal) ResultList (const Standard_Integer NElem) const;

  Standard_EXPORT virtual void OwnRead (IGESFile_Reader &) Standard_OVERRIDE;
  
  Standard_EXPORT virtual void OwnWrite (IGESData_IGESWriter &) const Standard_OVERRIDE;

  Standard_EXPORT virtual void OwnShared(Interface_EntityIterator &theIter) const Standard_OVERRIDE;

  Standard_EXPORT virtual IGESData_DirChecker DirChecker () const Standard_OVERRIDE;

  Standard_EXPORT virtual void OwnCheck (const Interface_ShareTool &, const Handle(Interface_Check) &) const Standard_OVERRIDE;
  
  Standard_EXPORT virtual void OwnDump (const IGESData_IGESDumper &, const Handle(Message_Messenger) &, const Standard_Integer) const Standard_OVERRIDE;

  DEFINE_STANDARD_RTTIEXT(IGESAppli_ElementResults,IGESData_IGESEntity)

 private:

  Standard_Integer myForm;
  //Handle(IGESDimen_GeneralNote) myNote;
  Interface_Pointer<IGESDimen_GeneralNote> myNote;
  Standard_Integer mySubcaseNumber;
  Standard_Real myTime;
  Standard_Integer myNbResultValues;
  Standard_Integer myResultReportFlag;
  Handle(TColStd_HArray1OfInteger) myElementIdentifiers;
  Handle(IGESAppli_HArray1OfFiniteElement) myElements;
  Handle(TColStd_HArray1OfInteger) myElementTopologyTypes;
  Handle(TColStd_HArray1OfInteger) myNbLayers;
  Handle(TColStd_HArray1OfInteger) myDataLayerFlags;
  Handle(TColStd_HArray1OfInteger) myNbResultDataLocs;
  Handle(IGESBasic_HArray1OfHArray1OfInteger) myResultDataLocs;
  Handle(IGESBasic_HArray1OfHArray1OfReal) myResultData;
};

#endif // _IGESAppli_ElementResults_HeaderFile

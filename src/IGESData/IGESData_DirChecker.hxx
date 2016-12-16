// Created on: 1992-11-10
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

#ifndef _IGESData_DirChecker_HeaderFile
#define _IGESData_DirChecker_HeaderFile

#include <Standard.hxx>
#include <Standard_DefineAlloc.hxx>
#include <Standard_Handle.hxx>

#include <IGESData_DefType.hxx>
class Interface_Check;
class IGESData_IGESEntity;


//! This class centralizes general Checks upon an IGES Entity's
//! Directory Part. That is : such field Ignored or Required,
//! or Required with a given Value (for an Integer field)
//! More precise checks can be performed as necessary, by each
//! Entity (method OwnCheck).
//!
//! Each class of Entity defines its DirChecker (method DirChecker)
//! and the DirChecker is able to perform its Checks on an Entity
//!
//! A Required Value or presence of a field causes a Fail Message
//! if criterium is not satisfied
//! An Ignored field causes a Correction Message if the field is
//! not null/zero
class IGESData_DirChecker 
{
public:

  DEFINE_STANDARD_ALLOC

  
  //! Returns a DirChecker, with no criterium at all to be checked
  IGESData_DirChecker()
  : thetype(0), theform1(0), theform2(0),
    thestructure(IGESData_ErrorRef),
    thelinefont(IGESData_ErrorRef),
    thelineweig(IGESData_ErrorRef),
    thecolor(IGESData_ErrorRef),
    thegraphier(-100), // ne pas tester GraphicsIgnored
    theblankst(-100), thesubordst(-100), theuseflag(-100), thehierst(-100) // ne pas tester
  {}
  
  //! Returns a DirChecker, with no criterium except Required Type
  IGESData_DirChecker(const Standard_Integer atype)
  : thetype(atype), theform1(0), theform2(-1), // test de forme inhibe
    thestructure(IGESData_ErrorRef),
    thelinefont(IGESData_ErrorRef),
    thelineweig(IGESData_ErrorRef),
    thecolor(IGESData_ErrorRef),
    thegraphier(-100), // ne pas tester GraphicsIgnored
    theblankst(-100), thesubordst(-100), theuseflag(-100), thehierst(-100) // ne pas tester
  {}

  //! Returns a DirChecker, with no criterium except Required values
  //! for Type and Form numbers
  IGESData_DirChecker(const Standard_Integer atype, const Standard_Integer aform)
  : thetype(atype), theform1(aform), theform2(aform), // forme : valeur requise
    thestructure(IGESData_ErrorRef),
    thelinefont(IGESData_ErrorRef),
    thelineweig(IGESData_ErrorRef),
    thecolor(IGESData_ErrorRef),
    thegraphier(-100), // ne pas tester GraphicsIgnored
    theblankst(-100), thesubordst(-100), theuseflag(-100), thehierst(-100) // ne pas tester
  {}

  //! Returns a DirChecker, with no criterium except Required values
  //! for Type number (atype), and Required Range for Form number
  //! (which must be between aform1 and aform2 included)
  IGESData_DirChecker(const Standard_Integer atype, const Standard_Integer aform1, const Standard_Integer aform2)
  : thetype(atype), theform1(aform1), theform2(aform2), // forme : [...]
    thestructure(IGESData_ErrorRef),
    thelinefont(IGESData_ErrorRef),
    thelineweig(IGESData_ErrorRef),
    thecolor(IGESData_ErrorRef),
    thegraphier(-100), // ne pas tester GraphicsIgnored
    theblankst(-100), thesubordst(-100), theuseflag(-100), thehierst(-100) // ne pas tester
  {}

  //! Sets Structure criterium.
  //! If crit is DefVoid, Ignored : should not be defined
  //! If crit is DefReference, Required : must be defined
  //! Other values are not taken in account
  void Structure (const IGESData_DefType crit) { thestructure = crit; }

  //! Sets LineFont criterium
  //! If crit is DefVoid, Ignored : should not be defined
  //! If crit is DefAny, Required : must be defined (value or ref)
  //! If crit is DefValue, Required as a Value (error if Reference)
  //! Other values are not taken in account
  void LineFont (const IGESData_DefType crit) { thelinefont = crit; }

  //! Sets LineWeight criterium
  //! If crit is DefVoid, Ignored : should not be defined
  //! If crit is DefValue, Required
  //! Other values are not taken in account
  void LineWeight (const IGESData_DefType crit) { thelineweig = crit; }

  //! Sets Color criterium
  //! If crit is DefVoid, Ignored : should not be defined
  //! If crit is DefAny, Required : must be defined (value or ref)
  //! Other values are not taken in account
  void Color (const IGESData_DefType crit) { thecolor = crit; }

  //! Sets Graphics data (LineFont, LineWeight, Color, Level, View)
  //! to be ignored according value of Hierarchy status :
  //! If hierarchy is not given, they are Ignored any way
  //! (that is, they should not be defined)
  //! If hierarchy is given, Graphics are Ignored if the Hierarchy
  //! status has the value given in argument "hierarchy"
  void GraphicsIgnored (const Standard_Integer hierarchy = -1) { thegraphier = hierarchy; }

  //! Sets Blank Status to be ignored
  //! (should not be defined, or its value should be 0)
  void BlankStatusIgnored() { theblankst = -10; }
  
  //! Sets Blank Status to be required at a given value
  void BlankStatusRequired (const Standard_Integer val) { theblankst = val; }

  //! Sets Subordinate Status to be ignored
  //! (should not be defined, or its value should be 0)
  void SubordinateStatusIgnored() { thesubordst = -10; }

  //! Sets Subordinate Status to be required at a given value
  void SubordinateStatusRequired (const Standard_Integer val) { thesubordst = val; }

  //! Sets Blank Status to be ignored
  //! (should not be defined, or its value should be 0)
  void UseFlagIgnored() { theuseflag = -10; }

  //! Sets Blank Status to be required at a given value
  //! Give -1 to demand UseFlag not zero (but no precise value req.)
  void UseFlagRequired (const Standard_Integer val) { theuseflag = val; }

  //! Sets Hierarchy Status to be ignored
  //! (should not be defined, or its value should be 0)
  void HierarchyStatusIgnored() { thehierst = -10; }

  //! Sets Hierarchy Status to be required at a given value
  void HierarchyStatusRequired (const Standard_Integer val) { thehierst = val; }

  //! Performs the Checks on an IGESEntity, according to the
  //! recorded criteria
  //! In addition, does minimal Checks, such as admitted range for
  //! Status, or presence of Error status in some data (Color, ...)
  //szv_c1:Standard_EXPORT void Check (Handle(Interface_Check)& ach, const Handle(IGESData_IGESEntity)& ent) const;
  
  //! Performs a Check only on Values of Type Number and Form Number
  //! This allows to do a check on an Entity not yet completely
  //! filled but of which Type and Form Number have been already set
  Standard_EXPORT void CheckTypeAndForm (Handle(Interface_Check)& ach, const Handle(IGESData_IGESEntity)& ent) const;
  
  //! Corrects the Directory Entry of an IGES Entity as far as it is
  //! possible according recorded criteria without any ambiguity :
  //! - if a numeric Status is required a given value, this value is
  //! enforced
  //! - if an item is required to be Void, or if it recorded as
  //! Erroneous, it is cleared (set to Void)
  //! - Type Number is enforced
  //! - finally Form Number is enforced only if one and only Value
  //! is admitted (no range, see Constructors of DirChecker)
  //szv_c1:Standard_EXPORT Standard_Boolean Correct (const Handle(IGESData_IGESEntity)& ent) const;

 //private:

  Standard_Integer thetype;
  Standard_Integer theform1;
  Standard_Integer theform2;
  IGESData_DefType thestructure;
  IGESData_DefType thelinefont;
  IGESData_DefType thelineweig;
  IGESData_DefType thecolor;
  Standard_Integer thegraphier;
  Standard_Integer theblankst;
  Standard_Integer thesubordst;
  Standard_Integer theuseflag;
  Standard_Integer thehierst;
};

#endif // _IGESData_DirChecker_HeaderFile

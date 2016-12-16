// Created on: 1993-01-09
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

#ifndef _IGESDraw_LabelDisplay_HeaderFile
#define _IGESDraw_LabelDisplay_HeaderFile

#include <IGESDraw_HArray1OfViewKindEntity.hxx>
#include <TColgp_HArray1OfXYZ.hxx>
#include <IGESDimen_HArray1OfLeaderArrow.hxx>
#include <TColStd_HArray1OfInteger.hxx>
#include <IGESData_HArray1OfIGESEntity.hxx>
#include <IGESData_LabelDisplayEntity.hxx>
class IGESData_ViewKindEntity;
class gp_Pnt;
class IGESDimen_LeaderArrow;


//! defines IGESLabelDisplay, Type <402> Form <5>
//! in package IGESDraw
//!
//! Permits one or more displays for the
//! entity labels of an entity
class IGESDraw_LabelDisplay : public IGESData_LabelDisplayEntity
{
 public:

  Standard_EXPORT virtual Standard_Integer TypeNumber() const Standard_OVERRIDE { return 402; }

  Standard_EXPORT virtual Standard_Integer FormNumber() const Standard_OVERRIDE { return 5; }

  IGESDraw_LabelDisplay() {}
  
  //! returns the number of label placements in <me>
  Standard_EXPORT Standard_Integer NbLabels() const;
  
  //! returns the View entity indicated by ViewIndex
  //! raises an exception if ViewIndex <= 0 or ViewIndex > NbLabels().
  Standard_EXPORT const Handle(IGESData_ViewKindEntity) & ViewItem (const Standard_Integer ViewIndex) const;
  
  //! returns the 3d-Point coordinates of the text location, in the
  //! view indicated by ViewIndex
  //! raises an exception if ViewIndex <= 0 or ViewIndex > NbLabels().
  Standard_EXPORT gp_Pnt TextLocation (const Standard_Integer ViewIndex) const;
  
  //! returns the Leader entity in the view indicated by ViewIndex
  //! raises an exception if ViewIndex <= 0 or ViewIndex > NbLabels().
  Standard_EXPORT const Handle(IGESDimen_LeaderArrow) & LeaderEntity (const Standard_Integer ViewIndex) const;
  
  //! returns the Entity label level number in the view indicated
  //! by ViewIndex
  //! raises an exception if ViewIndex <= 0 or ViewIndex > NbLabels().
  Standard_EXPORT Standard_Integer LabelLevel (const Standard_Integer ViewIndex) const;
  
  //! returns the entity indicated by EntityIndex
  //! raises an exception if EntityIndex <= 0 or EntityIndex > NbLabels().
  Standard_EXPORT const Handle(IGESData_IGESEntity) & DisplayedEntity (const Standard_Integer EntityIndex) const;
  
  //! returns the transformed 3d-Point coordinates of the text
  //! location, in the view indicated by ViewIndex
  //! raises an exception if ViewIndex <= 0 or ViewIndex > NbLabels().
  Standard_EXPORT gp_Pnt TransformedTextLocation (const Standard_Integer ViewIndex) const;

  Standard_EXPORT virtual void OwnRead (IGESFile_Reader &) Standard_OVERRIDE;
  
  Standard_EXPORT virtual void OwnWrite (IGESData_IGESWriter &) const Standard_OVERRIDE;

  Standard_EXPORT virtual void OwnShared (Interface_EntityIterator &) const Standard_OVERRIDE;

  Standard_EXPORT virtual IGESData_DirChecker DirChecker () const Standard_OVERRIDE;

  Standard_EXPORT virtual void OwnDump (const IGESData_IGESDumper &, const Handle(Message_Messenger) &, const Standard_Integer) const Standard_OVERRIDE;

  DEFINE_STANDARD_RTTIEXT(IGESDraw_LabelDisplay,IGESData_LabelDisplayEntity)

 private:

  Handle(IGESDraw_HArray1OfViewKindEntity) theViews;
  Handle(TColgp_HArray1OfXYZ) theTextLocations;
  Handle(IGESDimen_HArray1OfLeaderArrow) theLeaderEntities;
  Handle(TColStd_HArray1OfInteger) theLabelLevels;
  Handle(IGESData_HArray1OfIGESEntity) theDisplayedEntities;
};

#endif // _IGESDraw_LabelDisplay_HeaderFile

// Created on: 2009-04-06
// Created by: Sergey ZARITCHNY
// Copyright (c) 2009-2014 OPEN CASCADE SAS
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

#ifndef _TDataXtd_Presentation_HeaderFile
#define _TDataXtd_Presentation_HeaderFile

#include <Standard.hxx>
#include <Standard_Type.hxx>
#include <Standard_GUID.hxx>

#include <gp_Pnt.hxx>
#include <TDF_Attribute.hxx>
#include <Standard_Boolean.hxx>
class TDF_Label;
class gp_Pnt;
class Standard_GUID;
class TDF_Attribute;
class TDF_RelocationTable;


class TDataXtd_Presentation;
DEFINE_STANDARD_HANDLE(TDataXtd_Presentation, TDF_Attribute)

//! Presentation of a Label
class TDataXtd_Presentation : public TDF_Attribute
{

public:

  
  //! Create if not found the TDataXtd_Presentation attribute set its position to <aPos>
  Standard_EXPORT static Handle(TDataXtd_Presentation) Set(const TDF_Label& theLabel, const Standard_GUID& theDriverId);
  
  //! Find an existing, or create an empty, Presentation
  //! the Position attribute is returned.
  Standard_EXPORT static Handle(TDataXtd_Presentation) Set (const Handle(TDF_Attribute)& theMaster);


  Standard_EXPORT void Unset(const TDF_Label& theLabel);
  
  //! Search label <aLabel) for the TDataXtd_Presentation attribute and get its position
  //! if found returns True
  Standard_EXPORT static Standard_Boolean Get (const TDF_Label& aLabel, gp_Pnt& aPos);
  
  Standard_EXPORT TDataXtd_Presentation();
  
  //! Returns the ID of the attribute.
  Standard_EXPORT const Standard_GUID& ID() const Standard_OVERRIDE;

  Standard_EXPORT Standard_GUID GetDriverGUID() const;

  Standard_EXPORT void SetDriverGUID(const Standard_GUID& theGUID);
  
  //! Returns the ID of the attribute.
  Standard_EXPORT static const Standard_GUID& GetID();
  
  //! Restores the contents from <anAttribute> into this
  //! one. It is used when aborting a transaction.
  Standard_EXPORT virtual void Restore (const Handle(TDF_Attribute)& anAttribute) Standard_OVERRIDE;
  
  //! Returns an new empty attribute from the good end
  //! type. It is used by the copy algorithm.
  Standard_EXPORT virtual Handle(TDF_Attribute) NewEmpty() const Standard_OVERRIDE;
  
  //! This method is different from the "Copy" one,
  //! because it is used when copying an attribute from
  //! a source structure into a target structure. This
  //! method pastes the current attribute to the label
  //! corresponding to the insertor. The pasted
  //! attribute may be a brand new one or a new version
  //! of the previous one.
  Standard_EXPORT virtual void Paste (const Handle(TDF_Attribute)& intoAttribute, const Handle(TDF_RelocationTable)& aRelocTationable) const Standard_OVERRIDE;
  
  Standard_EXPORT Standard_Boolean IsDisplayed() const;

  Standard_EXPORT Standard_Boolean IsHasOwnMaterial() const;

  Standard_EXPORT Standard_Boolean IsHasOwnTransparency() const;

  Standard_EXPORT Standard_Boolean IsHasOwnColor() const;

  Standard_EXPORT Standard_Boolean IsHasOwnWidth() const;

  Standard_EXPORT Standard_Boolean IsHasOwnMode() const;

  Standard_EXPORT Standard_Boolean IsHasOwnSelectionMode() const;

  Standard_EXPORT void SetDisplayed(const Standard_Boolean theIsDisplayed);

  Standard_EXPORT void SetMaterial(const Standard_Integer theName);

  Standard_EXPORT void SetTransparency(const Standard_Real theValue);

  Standard_EXPORT void SetColor(const Standard_Integer theColor);

  Standard_EXPORT void SetWidth(const Standard_Real theWidth);

  Standard_EXPORT void SetMode(const Standard_Integer theMode);

  Standard_EXPORT void SetSelectionMode(const Standard_Integer theSelectionMode);

  Standard_EXPORT Standard_Integer Material() const;

  Standard_EXPORT Standard_Real Transparency() const;

  Standard_EXPORT Standard_Integer Color() const;

  Standard_EXPORT Standard_Real Width() const;

  Standard_EXPORT Standard_Integer Mode() const;

  Standard_EXPORT Standard_Integer SelectionMode() const;

  Standard_EXPORT void UnsetMaterial();

  Standard_EXPORT void UnsetTransparency();

  Standard_EXPORT void UnsetColor();

  Standard_EXPORT void UnsetWidth();

  Standard_EXPORT void UnsetMode();

  Standard_EXPORT void UnsetSelectionMode();

  Standard_EXPORT Handle(TDF_Attribute) BackupCopy() const;




  DEFINE_STANDARD_RTTI(TDataXtd_Presentation,TDF_Attribute)

protected:




private:


    Standard_GUID  myDriverGUID;
    Standard_Integer myColor;
    Standard_Integer myMaterial;
    Standard_Integer myMode;
    Standard_Integer mySelectionMode;
    Standard_Real myTransparency;
    Standard_Real myWidth;
    Standard_Boolean myIsDisplayed;
    Standard_Boolean myIsHasOwnColor;
    Standard_Boolean myIsHasOwnMaterial;
    Standard_Boolean myIsHasOwnTransparency;
    Standard_Boolean myIsHasOwnWidth;
    Standard_Boolean myIsHasOwnMode;
    Standard_Boolean myIsHasOwnSelectionMode;


};







#endif // _TDataXtd_Presentation_HeaderFile

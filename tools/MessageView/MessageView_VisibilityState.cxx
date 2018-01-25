// Created on: 2017-06-16
// Created by: Natalia ERMOLAEVA
// Copyright (c) 2017 OPEN CASCADE SAS
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

#include <inspector/MessageView_VisibilityState.hxx>
#include <inspector/MessageModel_ItemAlert.hxx>

#include <TopoDS_AlertWithShape.hxx>

// =======================================================================
// function : CanBeVisible
// purpose :
// =======================================================================
bool MessageView_VisibilityState::CanBeVisible (const QModelIndex& theIndex) const
{
  return !getShape (theIndex).IsNull();
}

// =======================================================================
// function : SetVisible
// purpose :
// =======================================================================
bool MessageView_VisibilityState::SetVisible (const QModelIndex& theIndex, const bool theState)
{
  TopoDS_Shape aShape = getShape (theIndex);
  if (aShape.IsNull())
    return false;

  myDisplayer->SetVisible (aShape, theState, myPresentationType);
  return true;
}

// =======================================================================
// function : IsVisible
// purpose :
// =======================================================================
bool MessageView_VisibilityState::IsVisible (const QModelIndex& theIndex) const
{
  return myDisplayer->IsVisible (getShape (theIndex), myPresentationType);
}

// =======================================================================
// function : getShape
// purpose :
// =======================================================================
TopoDS_Shape MessageView_VisibilityState::getShape (const QModelIndex& theIndex) const
{
  TreeModel_ItemBasePtr anItemBase = TreeModel_ModelBase::GetItemByIndex (theIndex);
  if (!anItemBase)
    return TopoDS_Shape();

  MessageModel_ItemAlertPtr anAlertItem = itemDynamicCast<MessageModel_ItemAlert>(anItemBase);
  if (!anAlertItem)
    return TopoDS_Shape();

  Handle(Message_Alert) anAlert = anAlertItem->GetAlert();
  if (anAlert.IsNull())
    return TopoDS_Shape();

  Handle(TopoDS_AlertWithShape) aShapeAlert = Handle(TopoDS_AlertWithShape)::DownCast (anAlert);
  if (aShapeAlert.IsNull() || aShapeAlert->GetShape().IsNull())
    return TopoDS_Shape();

  return aShapeAlert->GetShape();
}

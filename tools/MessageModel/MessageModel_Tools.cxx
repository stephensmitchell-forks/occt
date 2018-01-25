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

#include <inspector/MessageModel_Tools.hxx>

#include <BRep_Builder.hxx>
#include <BRepTools.hxx>
#include <Message_AlertWithObject.hxx>
#include <TopoDS_AlertWithShape.hxx>

// =======================================================================
// function : GetPointerInfo
// purpose :
// =======================================================================
TCollection_AsciiString MessageModel_Tools::GetPointerInfo (const Handle(Standard_Transient)& thePointer, const bool isShortInfo)
{
  if (thePointer.IsNull())
    return TCollection_AsciiString();

  std::ostringstream aPtrStr;
  aPtrStr << thePointer.operator->();
  if (!isShortInfo)
    return aPtrStr.str().c_str();

  TCollection_AsciiString anInfoPtr (aPtrStr.str().c_str());
  for (int aSymbolId = 1; aSymbolId < anInfoPtr.Length(); aSymbolId++)
  {
    if (anInfoPtr.Value(aSymbolId) != '0')
    {
      anInfoPtr = anInfoPtr.SubString (aSymbolId, anInfoPtr.Length());
      anInfoPtr.Prepend("0x");
      return anInfoPtr;
    }
  }
  return aPtrStr.str().c_str();
}

// =======================================================================
// function : GetPointerAlertInfo
// purpose :
// =======================================================================
TCollection_AsciiString MessageModel_Tools::GetPointerAlertInfo (const Handle(Message_Alert)& theAlert)
{
  if (theAlert->IsKind (STANDARD_TYPE (TopoDS_AlertWithShape)))
    return GetPointerInfo (Handle(TopoDS_AlertWithShape)::DownCast (theAlert)->GetShape().TShape());
  else if (theAlert->IsKind (STANDARD_TYPE (Message_AlertWithObject)))
    return GetPointerInfo (Handle(Message_AlertWithObject)::DownCast (theAlert)->GetObject());
  return TCollection_AsciiString();
}

// =======================================================================
// function : GetShapeTypeAlertInfo
// purpose :
// =======================================================================
TCollection_AsciiString MessageModel_Tools::GetShapeTypeAlertInfo (const Handle(Message_Alert)& theAlert)
{
  TopoDS_Shape aShape;

  if (theAlert->IsKind (STANDARD_TYPE (TopoDS_AlertWithShape)))
    aShape = Handle(TopoDS_AlertWithShape)::DownCast (theAlert)->GetShape();

  if (aShape.IsNull())
    return TCollection_AsciiString();

  TopAbs_ShapeEnum aShapeType = aShape.ShapeType();
  Standard_SStream aSStream;
  TopAbs::Print ((TopAbs_ShapeEnum)aShapeType, aSStream);
  return aSStream.str().c_str();
}

// =======================================================================
// function : GetStandardTypeAlertInfo
// purpose :
// =======================================================================
TCollection_AsciiString MessageModel_Tools::GetStandardTypeAlertInfo (const Handle(Message_Alert)& theAlert)
{
  Handle(Standard_Transient) aPointer;
  if (theAlert->IsKind (STANDARD_TYPE (TopoDS_AlertWithShape)))
    aPointer = Handle(TopoDS_AlertWithShape)::DownCast (theAlert)->GetShape().TShape();
  else if (theAlert->IsKind (STANDARD_TYPE (Message_AlertWithObject)))
    aPointer = Handle(Message_AlertWithObject)::DownCast (theAlert)->GetObject();

  if (aPointer.IsNull())
    return TCollection_AsciiString();

  return aPointer->DynamicType()->Name();
}

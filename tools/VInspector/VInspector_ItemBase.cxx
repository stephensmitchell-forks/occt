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


#include <VInspector_ItemBase.hxx>
#include <VInspector_ItemContext.hxx>

// =======================================================================
// function : GetContext
// purpose :
// =======================================================================
const Handle(AIS_InteractiveContext)& VInspector_ItemBase::GetContext() const
{
  if (!myContext.IsNull())
    return myContext;
  
  Handle(AIS_InteractiveContext) aContext;
  if (const VInspector_ItemContext* aThisContextItem = dynamic_cast<const VInspector_ItemContext*> (this))
  {
    aContext = aThisContextItem->GetContext();
  }
  else
  {
    TreeModel_ItemBasePtr anItem = Parent();
    while (anItem)
    {
      if (VInspector_ItemContextPtr aContextItem = itemDynamicCast<VInspector_ItemContext>(anItem))
      {
        aContext = aContextItem->GetContext();
        break;
      }
      anItem = anItem->Parent();
    }
  }
  if (!aContext.IsNull())
    const_cast<VInspector_ItemBase*>(this)->SetContext (aContext);

  return myContext;
}

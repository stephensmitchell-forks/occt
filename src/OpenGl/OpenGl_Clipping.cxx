// Created on: 2013-09-05
// Created by: Anton POLETAEV
// Copyright (c) 2013-2014 OPEN CASCADE SAS
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

#include <OpenGl_Clipping.hxx>

#include <OpenGl_GlCore11.hxx>
#include <OpenGl_Workspace.hxx>
#include <OpenGl_Context.hxx>

// =======================================================================
// function : OpenGl_ClippingIterator
// purpose  :
// =======================================================================
OpenGl_ClippingIterator::OpenGl_ClippingIterator (const OpenGl_Clipping& theClipping)
: myDisabled  (&theClipping.myDisabledPlanes),
  myCurrIndex (1)
{
  if (!theClipping.myPlanesGlobal.IsNull())
  {
    myIter1.Init (*theClipping.myPlanesGlobal);
  }
  if (!theClipping.myPlanesLocal.IsNull())
  {
    myIter2.Init (*theClipping.myPlanesLocal);
  }
}

// =======================================================================
// function : OpenGl_Clipping
// purpose  :
// =======================================================================
OpenGl_Clipping::OpenGl_Clipping()
{
  memset (myCounters, 0, sizeof(myCounters));
}

// =======================================================================
// function : Init
// purpose  :
// =======================================================================
void OpenGl_Clipping::Init()
{
  myPlanesGlobal.Nullify();
  myPlanesLocal.Nullify();
  memset (myCounters, 0, sizeof(myCounters));
}

// =======================================================================
// function : Reset
// purpose  :
// =======================================================================
void OpenGl_Clipping::Reset (const Handle(OpenGl_Context)& theGlCtx,
                             const Handle(Graphic3d_SequenceOfHClipPlane)& thePlanes)
{
  const Standard_Integer aStartIndex = myPlanesGlobal.IsNull() ? 1 : myPlanesGlobal->Size() + 1;
  remove (theGlCtx, myPlanesLocal,  aStartIndex);
  remove (theGlCtx, myPlanesGlobal, 1);

  myPlanesGlobal = thePlanes;
  myPlanesLocal.Nullify();

  add (theGlCtx, thePlanes, 1);
  myCounters[Counter_Disabled] = 0;

  // Method ::add() implicitly extends myDisabledPlanes (NCollection_Vector::SetValue()),
  // however we do not reset myDisabledPlanes and mySkipFilter beforehand to avoid redundant memory re-allocations.
  // So once extended, they will never reduce their size to lower values.
  // This should not be a problem since overall number of clipping planes is expected to be quite small.
}

// =======================================================================
// function : SetLocalPlanes
// purpose  :
// =======================================================================
void OpenGl_Clipping::SetLocalPlanes (const Handle(OpenGl_Context)& theGlCtx,
                                      const Handle(Graphic3d_SequenceOfHClipPlane)& thePlanes)
{
  const Standard_Integer aStartIndex = myPlanesGlobal.IsNull() ? 1 : myPlanesGlobal->Size() + 1;
  remove (theGlCtx, myPlanesLocal, aStartIndex);

  myPlanesLocal = thePlanes;

  add (theGlCtx, thePlanes, aStartIndex);
}

// =======================================================================
// function : add
// purpose  :
// =======================================================================
void OpenGl_Clipping::add (const Handle(OpenGl_Context)& ,
                           const Handle(Graphic3d_SequenceOfHClipPlane)& thePlanes,
                           const Standard_Integer theStartIndex)
{
  if (thePlanes.IsNull())
  {
    return;
  }

  Standard_Integer aPlaneId = theStartIndex;
  for (Graphic3d_SequenceOfHClipPlane::Iterator aPlaneIt (*thePlanes); aPlaneIt.More(); aPlaneIt.Next())
  {
    const Handle(Graphic3d_ClipPlane)& aPlane = aPlaneIt.Value();
    if (aPlane->IsOn() && aPlane->IsChain())
    {
      ++myCounters[Counter_EnabledChains];
    }
    for (const Graphic3d_ClipPlane* aSubPlaneIter = aPlane.get(); aSubPlaneIter != NULL; aSubPlaneIter = aSubPlaneIter->NextPlaneInChain().get(), ++aPlaneId)
    {
      myDisabledPlanes.SetValue (aPlaneId, Standard_False); // automatically resizes the vector
      if (aPlane->IsOn())
      {
        ++myCounters[aSubPlaneIter->IsCapping() ? Counter_EnabledCapping : Counter_EnabledClipping];
      }
    }
  }
}

// =======================================================================
// function : remove
// purpose  :
// =======================================================================
void OpenGl_Clipping::remove (const Handle(OpenGl_Context)& ,
                              const Handle(Graphic3d_SequenceOfHClipPlane)& thePlanes,
                              const Standard_Integer theStartIndex)
{
  if (thePlanes.IsNull())
  {
    return;
  }

  Standard_Integer aPlaneIndex = theStartIndex;
  for (Graphic3d_SequenceOfHClipPlane::Iterator aPlaneIt (*thePlanes); aPlaneIt.More(); aPlaneIt.Next())
  {
    const Handle(Graphic3d_ClipPlane)& aPlane = aPlaneIt.Value();
    if (!aPlane->IsOn())
    {
      aPlaneIndex += aPlane->NbForwardUnionChains();
      continue;
    }

    bool hasSubEnabled = false;
    for (const Graphic3d_ClipPlane* aSubPlaneIter = aPlane.get(); aSubPlaneIter != NULL; aSubPlaneIter = aSubPlaneIter->NextPlaneInChain().get(), ++aPlaneIndex)
    {
      if (!myDisabledPlanes.Value (aPlaneIndex))
      {
        hasSubEnabled = true;
        --myCounters[aSubPlaneIter->IsCapping() ? Counter_EnabledCapping : Counter_EnabledClipping];
      }
    }
    if (hasSubEnabled && aPlane->IsChain())
    {
      --myCounters[Counter_EnabledChains];
    }
  }
}

// =======================================================================
// function : SetEnabled
// purpose  :
// =======================================================================
Standard_Boolean OpenGl_Clipping::SetEnabled (const Handle(OpenGl_Context)&  ,
                                              const OpenGl_ClippingIterator& thePlane,
                                              const Standard_Integer         theSubPlane,
                                              const Standard_Boolean         theIsEnabled)
{
  Standard_Integer aPlaneIndex = thePlane.PlaneIndex();
  const Standard_Integer aTargetPlaneIndex = thePlane.PlaneIndex() + theSubPlane;
  Standard_Integer aNbDisabled = 0;
  bool hasChanged = false;
  for (const Graphic3d_ClipPlane* aSubPlaneIter = thePlane.Value().get(); aSubPlaneIter != NULL; aSubPlaneIter = aSubPlaneIter->NextPlaneInChain().get(), ++aPlaneIndex)
  {
    Standard_Boolean& isDisabled = myDisabledPlanes.ChangeValue (aPlaneIndex);
    if (theSubPlane != -1)
    {
      if (aPlaneIndex != aTargetPlaneIndex)
      {
        if (isDisabled)
        {
          ++aNbDisabled;
        }
        continue;
      }
    }

    if (!theIsEnabled)
    {
      ++aNbDisabled;
    }
    if (isDisabled == !theIsEnabled)
    {
      continue;
    }

    hasChanged = true;
    isDisabled = !theIsEnabled;
    const Standard_Integer aNbSubPlanes = thePlane.Value()->NbForwardUnionChains();

    myCounters[aSubPlaneIter->IsCapping() ? Counter_EnabledCapping : Counter_EnabledClipping] += (theIsEnabled ? 1 : -1);
    myCounters[Counter_Disabled] += (theIsEnabled ? -1 : 1);
  }
  if (!hasChanged)
  {
    return Standard_False;
  }

  if (thePlane.Value()->IsChain())
  {
    if (aNbDisabled == thePlane.Value()->NbForwardUnionChains())
    {
      --myCounters[Counter_EnabledChains];
    }
    else if (aNbDisabled == 0)
    {
      ++myCounters[Counter_EnabledChains];
    }
  }
  return Standard_True;
}

// =======================================================================
// function : RestoreDisabled
// purpose  :
// =======================================================================
void OpenGl_Clipping::RestoreDisabled (const Handle(OpenGl_Context)& )
{
  if (myCounters[Counter_Disabled] == 0)
  {
    return;
  }

  myCounters[Counter_Disabled] = 0;
  for (OpenGl_ClippingIterator aPlaneIter (*this); aPlaneIter.More(); aPlaneIter.Next())
  {
    Standard_Integer aPlaneIndex = aPlaneIter.PlaneIndex();
    bool wasSomeEnabled = false;
    for (const Graphic3d_ClipPlane* aSubPlaneIter = aPlaneIter.Value().get(); aSubPlaneIter != NULL; aSubPlaneIter = aSubPlaneIter->NextPlaneInChain().get(), ++aPlaneIndex)
    {
      Standard_Boolean& isDisabled = myDisabledPlanes.ChangeValue (aPlaneIndex);
      if (!isDisabled)
      {
        wasSomeEnabled = true;
        continue;
      }

      isDisabled = Standard_False;
      ++myCounters[aSubPlaneIter->IsCapping() ? Counter_EnabledCapping : Counter_EnabledClipping];
    }
    if (!wasSomeEnabled
      && aPlaneIter.Value()->IsChain())
    {
      ++myCounters[Counter_EnabledChains];
    }
  }
}

// =======================================================================
// function : DisableGlobal
// purpose  :
// =======================================================================
void OpenGl_Clipping::DisableGlobal (const Handle(OpenGl_Context)& theGlCtx)
{
  for (OpenGl_ClippingIterator aPlaneIter (*this); aPlaneIter.More(); aPlaneIter.Next())
  {
    if (!aPlaneIter.IsGlobal())
    {
      // local planes always follow global ones in iterator
      return;
    }

    SetEnabled (theGlCtx, aPlaneIter, -1, Standard_False);
  }
}

// =======================================================================
// function : DisableAllExcept
// purpose  :
// =======================================================================
void OpenGl_Clipping::DisableAllExcept (const Handle(OpenGl_Context)&  theGlCtx,
                                        const OpenGl_ClippingIterator& thePlane,
                                        const Standard_Integer         theSubPlane)
{
  const Standard_Integer aTargetPlaneIndex = thePlane.PlaneIndex() + theSubPlane;
  for (OpenGl_ClippingIterator aPlaneIter (*this); aPlaneIter.More(); aPlaneIter.Next())
  {
    Standard_Integer aPlaneIndex = aPlaneIter.PlaneIndex();
    for (const Graphic3d_ClipPlane* aSubPlaneIter = aPlaneIter.Value().get(); aSubPlaneIter != NULL; aSubPlaneIter = aSubPlaneIter->NextPlaneInChain().get(), ++aPlaneIndex)
    {
      if (myDisabledPlanes.Value (aPlaneIndex) || !aPlaneIter.Value()->IsOn())
      {
        mySkipFilter.SetValue (aPlaneIndex, Standard_True);
        continue;
      }

      const Standard_Boolean isOn = (aPlaneIndex == aTargetPlaneIndex);
      SetEnabled (theGlCtx, aPlaneIter, aPlaneIndex - aPlaneIter.PlaneIndex(), isOn);
      mySkipFilter.SetValue (aPlaneIndex, Standard_False);
    }
  }
}

// =======================================================================
// function : EnableAllExcept
// purpose  :
// =======================================================================
void OpenGl_Clipping::EnableAllExcept (const Handle(OpenGl_Context)&  theGlCtx,
                                       const OpenGl_ClippingIterator& thePlane,
                                       const Standard_Integer         theSubPlane)
{
  const Standard_Integer aTargetPlaneIndex = thePlane.PlaneIndex() + theSubPlane;
  for (OpenGl_ClippingIterator aPlaneIter (*this); aPlaneIter.More(); aPlaneIter.Next())
  {
    Standard_Integer aPlaneIndex = aPlaneIter.PlaneIndex();
    for (const Graphic3d_ClipPlane* aSubPlaneIter = aPlaneIter.Value().get(); aSubPlaneIter != NULL; aSubPlaneIter = aSubPlaneIter->NextPlaneInChain().get(), ++aPlaneIndex)
    {
      if (mySkipFilter.Value (aPlaneIndex))
      {
        continue;
      }

      const Standard_Boolean isOn = (aPlaneIndex != aTargetPlaneIndex);
      SetEnabled (theGlCtx, aPlaneIter, aPlaneIndex - aPlaneIter.PlaneIndex(), isOn);
    }
  }
}

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

#include <XCAFDimTolObjects_GeomToleranceObject.hxx>

//=======================================================================
//function : XCAFDimTolObjects_GeomTolerance
//purpose  : 
//=======================================================================

XCAFDimTolObjects_GeomToleranceObject::XCAFDimTolObjects_GeomToleranceObject()
{
  myHasAxis = Standard_False;
  myHasPlane = Standard_False;
  myHasPnt = Standard_False;
}

//=======================================================================
//function : XCAFDimTolObjects_GeomTolerance
//purpose  : 
//=======================================================================

XCAFDimTolObjects_GeomToleranceObject::XCAFDimTolObjects_GeomToleranceObject(const Handle(XCAFDimTolObjects_GeomToleranceObject)& theObj)
{
  myType = theObj->myType;
  myTypeOfValue = theObj->myTypeOfValue;
  myValue = theObj->myValue;
  myMatReqModif = theObj->myMatReqModif;
  myZoneModif = theObj->myZoneModif;
  myValueOfZoneModif = theObj->myValueOfZoneModif;
  myModifiers = theObj->myModifiers;
  myMaxValueModif = theObj->myMaxValueModif;
  myAxis = theObj->myAxis;
  myPlane = theObj->myPlane;
  myPnt= theObj->myPnt;
  myHasAxis = theObj->myHasAxis;
  myHasPlane = theObj->myHasPlane;
  myHasPnt = theObj->myHasPnt;
}

//=======================================================================
//function :
//purpose  : 
//=======================================================================

void XCAFDimTolObjects_GeomToleranceObject::SetType (const XCAFDimTolObjects_GeomToleranceType theType) 
{
  myType = theType;
}

//=======================================================================
//function :
//purpose  : 
//=======================================================================

XCAFDimTolObjects_GeomToleranceType XCAFDimTolObjects_GeomToleranceObject::GetType()  const
{
  return myType;
}

//=======================================================================
//function :
//purpose  : 
//=======================================================================

void XCAFDimTolObjects_GeomToleranceObject::SetTypeOfValue (const XCAFDimTolObjects_GeomToleranceTypeValue theTypeOfValue) 
{
  myTypeOfValue = theTypeOfValue;
}

//=======================================================================
//function :
//purpose  : 
//=======================================================================

XCAFDimTolObjects_GeomToleranceTypeValue XCAFDimTolObjects_GeomToleranceObject::GetTypeOfValue()  const
{
  return myTypeOfValue;
}

//=======================================================================
//function :
//purpose  : 
//=======================================================================

void XCAFDimTolObjects_GeomToleranceObject::SetValue (const Standard_Real theValue) 
{
  myValue = theValue;
}

//=======================================================================
//function :
//purpose  : 
//=======================================================================

Standard_Real XCAFDimTolObjects_GeomToleranceObject::GetValue()  const
{
  return myValue;
}

//=======================================================================
//function :
//purpose  : 
//=======================================================================

void XCAFDimTolObjects_GeomToleranceObject::SetMaterialRequirementModifier (const XCAFDimTolObjects_GeomToleranceMatReqModif theMatReqModif) 
{
  myMatReqModif = theMatReqModif;
}

//=======================================================================
//function :
//purpose  : 
//=======================================================================

XCAFDimTolObjects_GeomToleranceMatReqModif XCAFDimTolObjects_GeomToleranceObject::GetMaterialRequirementModifier()  const
{
  return myMatReqModif;
}

//=======================================================================
//function :
//purpose  : 
//=======================================================================

void XCAFDimTolObjects_GeomToleranceObject::SetZoneModifier (const XCAFDimTolObjects_GeomToleranceZoneModif theZoneModif) 
{
  myZoneModif = theZoneModif;
}

//=======================================================================
//function :
//purpose  : 
//=======================================================================

XCAFDimTolObjects_GeomToleranceZoneModif XCAFDimTolObjects_GeomToleranceObject::GetZoneModifier()  const
{
  return myZoneModif;
}

//=======================================================================
//function :
//purpose  : 
//=======================================================================

void XCAFDimTolObjects_GeomToleranceObject::SetValueOfZoneModifier (const Standard_Real theValue) 
{
  myValueOfZoneModif = theValue;
}

//=======================================================================
//function :
//purpose  : 
//=======================================================================

Standard_Real XCAFDimTolObjects_GeomToleranceObject::GetValueOfZoneModifier()  const
{
  return myValueOfZoneModif;
}

//=======================================================================
//function :
//purpose  : 
//=======================================================================

void XCAFDimTolObjects_GeomToleranceObject::SetModifiers (const XCAFDimTolObjects_GeomToleranceModifiersSequence& theModifiers) 
{
  myModifiers = theModifiers;
}

//=======================================================================
//function :
//purpose  : 
//=======================================================================

void XCAFDimTolObjects_GeomToleranceObject::AddModifier (const XCAFDimTolObjects_GeomToleranceModif theModifier) 
{
  myModifiers.Append(theModifier);
}

//=======================================================================
//function :
//purpose  : 
//=======================================================================

XCAFDimTolObjects_GeomToleranceModifiersSequence XCAFDimTolObjects_GeomToleranceObject::GetModifiers()  const
{
  return myModifiers;
}

//=======================================================================
//function :
//purpose  : 
//=======================================================================

void XCAFDimTolObjects_GeomToleranceObject::SetMaxValueModifier (const Standard_Real theModifier) 
{
  myMaxValueModif = theModifier;
}

//=======================================================================
//function :
//purpose  : 
//=======================================================================

Standard_Real XCAFDimTolObjects_GeomToleranceObject::GetMaxValueModifier()  const
{
  return myMaxValueModif;
}

//=======================================================================
//function :
//purpose  : 
//=======================================================================

void XCAFDimTolObjects_GeomToleranceObject::SetAxis (const gp_Ax2 theAxis) 
{
  myAxis = theAxis;
  myHasAxis = Standard_True;
}

//=======================================================================
//function :
//purpose  : 
//=======================================================================

gp_Ax2 XCAFDimTolObjects_GeomToleranceObject::GetAxis()  const
{
  return myAxis;
}

//=======================================================================
//function :
//purpose  : 
//=======================================================================

Standard_Boolean XCAFDimTolObjects_GeomToleranceObject::HasAxis () const 
{
  return myHasAxis;
}

//=======================================================================
//function : GetPlane
//purpose  : 
//=======================================================================

gp_Ax2 XCAFDimTolObjects_GeomToleranceObject::GetPlane() const
{
  return myPlane;
}

//=======================================================================
//function : SetPlane
//purpose  : 
//=======================================================================

void XCAFDimTolObjects_GeomToleranceObject::SetPlane(const gp_Ax2& thePlane)
{
  myPlane = thePlane;
  myHasPlane = Standard_True;
}

//=======================================================================
//function : GetPoint
//purpose  : 
//=======================================================================

gp_Pnt XCAFDimTolObjects_GeomToleranceObject::GetPoint() const
{
  return myPnt;
}

//=======================================================================
//function : SetPoint
//purpose  : 
//=======================================================================

void XCAFDimTolObjects_GeomToleranceObject::SetPoint(const gp_Pnt& thePnt)
{
  myPnt = thePnt;
  myHasPlane = Standard_True;
}

//=======================================================================
//function :
//purpose  : 
//=======================================================================

Standard_Boolean XCAFDimTolObjects_GeomToleranceObject::HasPlane () const 
{
  return myHasPlane;
}

//=======================================================================
//function :
//purpose  : 
//=======================================================================

Standard_Boolean XCAFDimTolObjects_GeomToleranceObject::HasPoint () const 
{
  return myHasPnt;
}
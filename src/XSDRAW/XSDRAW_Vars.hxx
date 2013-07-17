// Copyright (c) 2013 OPEN CASCADE SAS
//
// The content of this file is subject to the Open CASCADE Technology Public
// License Version 6.5 (the "License"). You may not use the content of this file
// except in compliance with the License. Please obtain a copy of the License
// at http://www.opencascade.org and read it completely before using this file.
//
// The Initial Developer of the Original Code is Open CASCADE S.A.S., having its
// main offices at: 1, place des Freres Montgolfier, 78280 Guyancourt, France.
//
// The Original Code and all software distributed under the License is
// distributed on an "AS IS" basis, without warranty of any kind, and the
// Initial Developer hereby disclaims all such warranties, including without
// limitation, any warranties of merchantability, fitness for a particular
// purpose or non-infringement. Please see the License for the specific terms
// and conditions governing the rights and limitations under the License.

#ifndef _XSDRAW_Vars_HeaderFile
#define _XSDRAW_Vars_HeaderFile

#include <Standard.hxx>
#include <Standard_DefineHandle.hxx>
#include <XSControl_Vars.hxx>
#include <Standard_CString.hxx>
#include <Handle_Standard_Transient.hxx>
#include <Handle_Geom_Geometry.hxx>
#include <Handle_Geom2d_Curve.hxx>
#include <Handle_Geom_Curve.hxx>
#include <Handle_Geom_Surface.hxx>
#include <Standard_Boolean.hxx>

class Standard_Transient;
class Geom_Geometry;
class Geom2d_Curve;
class Geom_Curve;
class Geom_Surface;
class gp_Pnt;
class gp_Pnt2d;
class TopoDS_Shape;
class Standard_Transient;
class Handle(Standard_Type);
class Handle(XSControl_Vars);
class XSDRAW_Vars;

DEFINE_STANDARD_HANDLE(XSDRAW_Vars,XSControl_Vars)

//! Vars for DRAW session (i.e. DBRep and DrawTrSurf) <br>
class XSDRAW_Vars : public XSControl_Vars {

public:
  
  Standard_EXPORT   XSDRAW_Vars();
  
  Standard_EXPORT   virtual  void Set(const Standard_CString name,const Handle(Standard_Transient)& val) ;
  
  Standard_EXPORT   virtual  Handle_Geom_Geometry GetGeom(Standard_CString& name) const;
  
  Standard_EXPORT   virtual  Handle_Geom2d_Curve GetCurve2d(Standard_CString& name) const;
  
  Standard_EXPORT   virtual  Handle_Geom_Curve GetCurve(Standard_CString& name) const;
  
  Standard_EXPORT   virtual  Handle_Geom_Surface GetSurface(Standard_CString& name) const;
  
  Standard_EXPORT   virtual  void SetPoint(const Standard_CString name,const gp_Pnt& val) ;
  
  Standard_EXPORT   virtual  void SetPoint2d(const Standard_CString name,const gp_Pnt2d& val) ;
  
  Standard_EXPORT   virtual  Standard_Boolean GetPoint(Standard_CString& name,gp_Pnt& pnt) const;
  
  Standard_EXPORT   virtual  Standard_Boolean GetPoint2d(Standard_CString& name,gp_Pnt2d& pnt) const;
  
  Standard_EXPORT   virtual  void SetShape(const Standard_CString name,const TopoDS_Shape& val) ;
  
  Standard_EXPORT   virtual  TopoDS_Shape GetShape(Standard_CString& name) const;

  DEFINE_STANDARD_RTTI(XSDRAW_Vars)

};

#endif

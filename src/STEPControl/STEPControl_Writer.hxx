// Created on: 1996-07-08
// Created by: Christian CAILLET
// Copyright (c) 1996-1999 Matra Datavision
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

#ifndef _STEPControl_Writer_HeaderFile
#define _STEPControl_Writer_HeaderFile

#include <Standard.hxx>
#include <Standard_DefineAlloc.hxx>
#include <Standard_Handle.hxx>

#include <IFSelect_ReturnStatus.hxx>
#include <STEPControl_StepModelType.hxx>
#include <Standard_CString.hxx>
class XSControl_WorkSession;
class StepData_StepModel;
class TopoDS_Shape;

//! This class creates and writes
//! STEP files from Open CASCADE models. A STEP file can be
//! written to an existing STEP file or to a new one.
//! Translation can be performed in one or several operations. Each
//! translation operation outputs a distinct root entity in the STEP file.
class STEPControl_Writer 
{
 public:

  DEFINE_STANDARD_ALLOC
  
  //! Creates a Writer from scratch
  Standard_EXPORT STEPControl_Writer();
  
  //! Creates a Writer from an already existing Session
  //! If <scratch> is True (D), clears already recorded data
  Standard_EXPORT STEPControl_Writer(const Handle(XSControl_WorkSession)& WS, const Standard_Boolean scratch = Standard_True);
  
  //! Sets a length-measure value that
  //! will be written to uncertainty-measure-with-unit
  //! when the next shape is translated.
  Standard_EXPORT void SetTolerance (const Standard_Real Tol);
  
  //! Unsets the tolerance formerly forced by SetTolerance
  void UnsetTolerance() { SetTolerance (-1.); }
  
  //! Sets a specific session to <me>
  Standard_EXPORT void SetWS (const Handle(XSControl_WorkSession)& WS, const Standard_Boolean scratch = Standard_True);
  
  //! Returns the session used in <me>
  const Handle(XSControl_WorkSession) & WS() const { return thesession; }
  
  //! Returns the produced model. Produces a new one if not yet done
  //! or if <newone> is True
  //! This method allows for instance to edit product or header
  //! data before writing.
  Standard_EXPORT Handle(StepData_StepModel) Model (const Standard_Boolean newone = Standard_False);
  
  //! Translates shape sh to a STEP
  //! entity. mode defines the STEP entity type to be output:
  //! - STEPControlStd_AsIs translates a shape to its highest possible
  //! STEP representation.
  //! - STEPControlStd_ManifoldSolidBrep translates a shape to a STEP
  //! manifold_solid_brep or brep_with_voids entity.
  //! - STEPControlStd_FacetedBrep translates a shape into a STEP
  //! faceted_brep entity.
  //! - STEPControlStd_ShellBasedSurfaceModel translates a shape into a STEP
  //! shell_based_surface_model entity.
  //! - STEPControlStd_GeometricCurveSet translates a shape into a STEP
  //! geometric_curve_set entity.
  Standard_EXPORT IFSelect_ReturnStatus Transfer (const TopoDS_Shape& sh, const STEPControl_StepModelType mode, const Standard_Boolean compgraph = Standard_True);
  
  //! Writes a STEP model in the file identified by filename.
  Standard_EXPORT IFSelect_ReturnStatus Write (const Standard_CString filename);

 private:

  Handle(XSControl_WorkSession) thesession;
};

#endif // _STEPControl_Writer_HeaderFile

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

#ifndef _STEPControl_Writer_HeaderFile
#define _STEPControl_Writer_HeaderFile

#include <Standard_DefineAlloc.hxx>
#include <Standard_Macro.hxx>
#include <XSControl_WorkSession.hxx>
#include <Standard_Boolean.hxx>
#include <Standard_Real.hxx>
#include <Handle_StepData_StepModel.hxx>
#include <IFSelect_ReturnStatus.hxx>
#include <STEPControl_StepModelType.hxx>
#include <Standard_CString.hxx>
#include <Standard_Integer.hxx>

class XSControl_WorkSession;
class StepData_StepModel;
class TopoDS_Shape;


//! This class creates and writes <br>
//! STEP files from Open CASCADE models. A STEP file can be <br>
//! written to an existing STEP file or to a new one. <br>
//! Translation can be performed in one or several operations. Each <br>
//! translation operation outputs a distinct root entity in the STEP file. <br>
class STEPControl_Writer  {
public:

  DEFINE_STANDARD_ALLOC

  //! Creates a Writer from scratch <br>
  Standard_EXPORT   STEPControl_Writer();
  //! Creates a Writer from an already existing Session <br>
//!           If <scratch> is True (D), clears already recorded data <br>
  Standard_EXPORT   STEPControl_Writer(const Handle(XSControl_WorkSession)& WS,const Standard_Boolean scratch = Standard_True);
  //! Sets a length-measure value that <br>
//!           will be written to uncertainty-measure-with-unit <br>
//!           when the next shape is translated. <br>
  Standard_EXPORT     void SetTolerance(const Standard_Real Tol) ;
  //! Unsets the tolerance formerly forced by SetTolerance <br>
  Standard_EXPORT     void UnsetTolerance() ;
  //! Sets a specific session to <me> <br>
  Standard_EXPORT     void SetWS(const Handle(XSControl_WorkSession)& WS,const Standard_Boolean scratch = Standard_True) ;
  //! Returns the session used in <me> <br>
  Standard_EXPORT     Handle_XSControl_WorkSession WS() const;
  //! Returns the produced model. Produces a new one if not yet done <br>
//!           or if <newone> is True <br>
//!           This method allows for instance to edit product or header <br>
//!           data before writing. <br>
  Standard_EXPORT     Handle_StepData_StepModel Model(const Standard_Boolean newone = Standard_False) ;
  //! Translates shape sh to a STEP <br>
//! entity. mode defines the STEP entity type to be output: <br>
//! - STEPControlStd_AsIs translates a shape to its highest possible <br>
//!   STEP representation. <br>
//! - STEPControlStd_ManifoldSolidBrep translates a shape to a STEP <br>
//!   manifold_solid_brep or brep_with_voids entity. <br>
//! - STEPControlStd_FacetedBrep translates a shape into a STEP <br>
//!   faceted_brep entity. <br>
//! - STEPControlStd_ShellBasedSurfaceModel translates a shape into a STEP <br>
//!   shell_based_surface_model entity. <br>
//! - STEPControlStd_GeometricCurveSet translates a shape into a STEP <br>
//!   geometric_curve_set entity. <br>
  Standard_EXPORT     IFSelect_ReturnStatus Transfer(const TopoDS_Shape& sh,const STEPControl_StepModelType mode,const Standard_Boolean compgraph = Standard_True) ;
  //! Writes a STEP model in the file identified by filename. <br>
  Standard_EXPORT     IFSelect_ReturnStatus Write(const Standard_CString filename) ;
  //!  Displays the statistics for the <br>
//! last translation. what defines the kind of statistics that are displayed: <br>
//! - 0 gives general statistics   (number of translated roots, <br>
//!   number of warnings, number of   fail messages), <br>
//! - 1 gives root results, <br>
//! - 2 gives statistics for all checked entities, <br>
//! - 3 gives the list of translated entities, <br>
//! - 4 gives warning and fail messages, <br>
//! - 5 gives fail messages only. <br>
//!   mode is used according to the use of what. If what is 0, mode is <br>
//! ignored. If what is 1, 2 or 3, mode defines the following: <br>
//! - 0 lists the numbers of STEP entities in a STEP model, <br>
//! - 1 gives the number, identifier, type and result type for each <br>
//!   STEP entity and/or its status (fail, warning, etc.), <br>
//! - 2 gives maximum information for each STEP entity (i.e. checks), <br>
//! - 3 gives the number of entities by the type of a STEP entity, <br>
//! - 4 gives the number of of STEP entities per result type and/or status, <br>
//! - 5 gives the number of pairs (STEP or result type and status), <br>
//! - 6 gives the number of pairs (STEP or result type and status) <br>
//!   AND the list of entity numbers in the STEP model. <br>
  Standard_EXPORT     void PrintStatsTransfer(const Standard_Integer what,const Standard_Integer mode = 0) const;





protected:





private:



Handle_XSControl_WorkSession thesession;


};





// other Inline functions and methods (like "C++: function call" methods)


#endif

// Created on: 2014-08-13
// Created by: Maxim GLIBIN
// Copyright (c) 2014 OPEN CASCADE SAS
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

#ifndef _AIS_PointCloud_HeaderFile
#define _AIS_PointCloud_HeaderFile

#include <AIS.hxx>
#include <AIS_InteractiveObject.hxx>
#include <gp_Pnt.hxx>
#include <Standard.hxx>
#include <Standard_Macro.hxx>
#include <Graphic3d_ArrayOfPoints.hxx>
#include <NCollection_Handle.hxx>
#include <NCollection_HArray1.hxx>

DEFINE_STANDARD_HANDLE (AIS_PointCloud, AIS_InteractiveObject)

NCOLLECTION_HARRAY1(AIS_ArrayOfPnt, gp_Pnt)

//! Interactive object for set of points.
class AIS_PointCloud : public AIS_InteractiveObject
{

public:

  //! Create point cloud.
  Standard_EXPORT AIS_PointCloud ();

  //! Initializes construction of the point cloud from points.
  //! @param thePoints [in] the points.
  Standard_EXPORT bool SetPoints (const Handle(Graphic3d_ArrayOfPoints)& thePoints);

  //! Sets the points.
  //! @param theCoords [in] the array of coordinates.
  //! @param theColors [in] the array of RGB colors.
  Standard_EXPORT bool SetPoints (const Handle(AIS_ArrayOfPnt)& theCoords,
                                  const Handle(AIS_ArrayOfPnt)& theColors,
                                  const Standard_Boolean hasColors = true);

  //! Sets the color theColor in the reconstructed point cloud
  Standard_EXPORT void SetColor (const Quantity_NameOfColor theColor);
  Standard_EXPORT void SetColor (const Quantity_Color &theColor);

public:

  DEFINE_STANDARD_RTTI (AIS_PointCloud)

protected:

  Standard_EXPORT virtual void Compute (const Handle(PrsMgr_PresentationManager3d)& thePresentationManager,
                                        const Handle(Prs3d_Presentation)& thePresentation,
                                        const Standard_Integer theMode = 0);

  Standard_EXPORT virtual void ComputeSelection(const Handle(SelectMgr_Selection)& aSelection,
                                                const Standard_Integer aMode);

private:

  Handle(Graphic3d_ArrayOfPoints) myPoints;
};

#endif // _AIS_PointCloud_HeaderFile

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

#include <Graphic3d_ArrayOfPoints.hxx>

#include <Quantity_HArray1OfColor.hxx>

#include <Standard.hxx>
#include <Standard_DefineHandle.hxx>
#include <Standard_Macro.hxx>

#include <TColgp_HArray1OfDir.hxx>
#include <TColgp_HArray1OfPnt.hxx>

DEFINE_STANDARD_HANDLE (AIS_PointCloud, AIS_InteractiveObject)

//! Interactive object for set of points.
//! Selection services are not provided by this class.
class AIS_PointCloud : public AIS_InteractiveObject
{

public:

  //! Constructor
  Standard_EXPORT AIS_PointCloud ();

  //! Sets the points from array of points.
  //! @detailed This function allows to avoid data duplication.
  //! @param    thePoints [in] the array of points.
  Standard_EXPORT virtual void SetPoints (const Handle(Graphic3d_ArrayOfPoints)& thePoints);

  //! Sets the points with optional colors.
  //! @param theCoords [in] the array of coordinates.
  //! @param theColors [in] optional array of colors.
  Standard_EXPORT virtual void SetPoints (const Handle(TColgp_HArray1OfPnt)&     theCoords,
                                          const Handle(Quantity_HArray1OfColor)& theColors = NULL);

  //! Get the points.
  //! @return the array of points.
  Standard_EXPORT virtual const Handle(Graphic3d_ArrayOfPoints) GetPoints() const;

  //! Redefined method implemets the standard behavior.
  Standard_EXPORT virtual void SetColor (const Quantity_NameOfColor theColor);

  //! Redefined method implemets the standard behavior.
  Standard_EXPORT virtual void SetColor (const Quantity_Color& theColor);

public:

  DEFINE_STANDARD_RTTI (AIS_PointCloud)

protected:

  Standard_EXPORT virtual void Compute (const Handle(PrsMgr_PresentationManager3d)& thePresentationManager,
                                        const Handle(Prs3d_Presentation)& thePresentation,
                                        const Standard_Integer theMode = 0);

  Standard_EXPORT virtual void ComputeSelection(const Handle(SelectMgr_Selection)& theSelection,
                                                const Standard_Integer theMode);

private:

  Handle(Graphic3d_ArrayOfPoints) myPoints;
};

#endif // _AIS_PointCloud_HeaderFile

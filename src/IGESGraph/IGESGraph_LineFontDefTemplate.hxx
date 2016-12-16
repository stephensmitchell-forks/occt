// Created on: 1993-01-11
// Created by: CKY / Contract Toubro-Larsen ( Niraj RANGWALA )
// Copyright (c) 1993-1999 Matra Datavision
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

#ifndef _IGESGraph_LineFontDefTemplate_HeaderFile
#define _IGESGraph_LineFontDefTemplate_HeaderFile

#include <IGESData_LineFontEntity.hxx>
class IGESBasic_SubfigureDef;


//! defines IGESLineFontDefTemplate, Type <304> Form <1>
//! in package IGESGraph
//!
//! Line Font can be defined as a repetition od Template figure
//! that is displayed at regularly spaced locations along a
//! planer anchoring curve. The anchoring curve itself has
//! no visual purpose.
class IGESGraph_LineFontDefTemplate : public IGESData_LineFontEntity
{
 public:

  Standard_EXPORT virtual Standard_Integer TypeNumber() const Standard_OVERRIDE { return 304; }

  Standard_EXPORT virtual Standard_Integer FormNumber() const Standard_OVERRIDE { return 1; }

  IGESGraph_LineFontDefTemplate()
  : myOrientation(0),
    myDistance(0.),
    myScale(1.)
  {}
  
  //! if return value = 0, Each Template display is oriented by aligning
  //! the axis of the SubfigureDef with the axis of
  //! the definition space of the anchoring curve.
  //! = 1, Each Template display is oriented by aligning
  //! X-axis of the SubfigureDef with the tangent
  //! vector of the anchoring curve at the point of
  //! incidence of the curve and the origin of
  //! subfigure.
  //! Similarly Z-axis is aligned.
  Standard_Integer Orientation() const { return myOrientation; }

  //! returns SubfigureDef as the Entity used as Template figure.
  const Handle(IGESBasic_SubfigureDef) & TemplateEntity() const { return myTemplateEntity; }

  //! returns the Distance between any two Template figures on the
  //! anchoring curve.
  Standard_Real Distance() const { return myDistance; }

  //! returns the Scaling factor applied to SubfigureDef to form
  //! Template figure.
  Standard_Real Scale() const { return myScale; }

  Standard_EXPORT virtual void OwnRead (IGESFile_Reader &) Standard_OVERRIDE;
  
  Standard_EXPORT virtual void OwnWrite (IGESData_IGESWriter &) const Standard_OVERRIDE;

  Standard_EXPORT virtual void OwnShared (Interface_EntityIterator &) const Standard_OVERRIDE;

  Standard_EXPORT virtual IGESData_DirChecker DirChecker () const Standard_OVERRIDE;

  Standard_EXPORT virtual void OwnCheck (const Interface_ShareTool &, Handle(Interface_Check) &) const Standard_OVERRIDE;

  Standard_EXPORT virtual void OwnDump (const IGESData_IGESDumper &, const Handle(Message_Messenger) &, const Standard_Integer) const Standard_OVERRIDE;

  DEFINE_STANDARD_RTTIEXT(IGESGraph_LineFontDefTemplate,IGESData_LineFontEntity)

 private:

  Standard_Integer myOrientation;
  Interface_Pointer<IGESBasic_SubfigureDef> myTemplateEntity;
  Standard_Real myDistance;
  Standard_Real myScale;
};

#endif // _IGESGraph_LineFontDefTemplate_HeaderFile

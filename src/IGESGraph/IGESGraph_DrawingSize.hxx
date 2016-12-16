// Created on: 1993-01-09
// Created by: CKY / Contract Toubro-Larsen ( TCD )
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

#ifndef _IGESGraph_DrawingSize_HeaderFile
#define _IGESGraph_DrawingSize_HeaderFile

#include <IGESData_IGESEntity.hxx>


//! defines IGESDrawingSize, Type <406> Form <16>
//! in package IGESGraph
//!
//! Specifies the drawing size in drawing units. The
//! origin of the drawing is defined to be (0,0) in
//! drawing space
class IGESGraph_DrawingSize : public IGESData_IGESEntity
{
 public:

  Standard_EXPORT virtual Standard_Integer TypeNumber() const Standard_OVERRIDE { return 406; }

  Standard_EXPORT virtual Standard_Integer FormNumber() const Standard_OVERRIDE { return 16; }

  IGESGraph_DrawingSize()
  : myXSize(0.),
    myYSize(0.)
  {}

  //! returns the extent of Drawing along positive XD axis
  Standard_Real XSize() const { return myXSize; }

  //! returns the extent of Drawing along positive YD axis
  Standard_Real YSize() const { return myYSize; }
  
  Standard_EXPORT virtual void OwnRead (IGESFile_Reader &) Standard_OVERRIDE;
  
  Standard_EXPORT virtual void OwnWrite (IGESData_IGESWriter &) const Standard_OVERRIDE;

  Standard_EXPORT virtual IGESData_DirChecker DirChecker () const Standard_OVERRIDE;

  Standard_EXPORT virtual void OwnDump (const IGESData_IGESDumper &, const Handle(Message_Messenger) &, const Standard_Integer) const Standard_OVERRIDE;

  DEFINE_STANDARD_RTTIEXT(IGESGraph_DrawingSize,IGESData_IGESEntity)

 private:

  Standard_Real myXSize;
  Standard_Real myYSize;
};

#endif // _IGESGraph_DrawingSize_HeaderFile

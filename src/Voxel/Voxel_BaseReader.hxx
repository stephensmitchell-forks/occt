// Created on: 2015-01-29
// Created by: Ilya Sevrikov
// Copyright (c) 2015 OPEN CASCADE SAS
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

#ifndef _Voxel_BaseReader_HeaderFile
#define _Voxel_BaseReader_HeaderFile

#include <Standard.hxx>

#include <TCollection_AsciiString.hxx>

#include <Graphic3d_DenseVolumeData.hxx>

//! Defines abstract reader for read volume data and transfer function
class Voxel_BaseReader
{
public:

  //! Creates empty reader.
  Voxel_BaseReader()
  {
    //
  }

  //! Releases resources.
  virtual ~Voxel_BaseReader()
  {
    //
  }

  //! Reads volume data from file and returns it.
  Standard_EXPORT virtual Standard_Boolean ReadDenseVolumeData (Handle(Graphic3d_VolumeData)&   theVolume,
                                                        const TCollection_AsciiString&  thePath) const = 0;

  //! Reads transfer function from file and returns it.
  //Standard_EXPORT virtual Handle(Graphic3d_ParentTransferFunction) ReadTransFunc (const TCollection_AsciiString& thePath) const = 0;

};

#endif //_Voxel_BaseReader_HeaderFile

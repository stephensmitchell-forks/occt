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

#ifndef _Voxel_DicomReader_HeaderFile
#define _Voxel_DicomReader_HeaderFile

#include <Voxel_BaseReader.hxx>
#include <OSD_OpenFile.hxx>

#include <Standard.hxx>
#include <TCollection_AsciiString.hxx>

#include <gdcmImageReader.h>
#include <gdcmImage.h>
#include <gdcmPhotometricInterpretation.h>
#include <gdcmDirectory.h>

//! Defines reader class for reading dicom files.
class Voxel_DicomReader : public Voxel_BaseReader
{
public:
  //! Creates dicom reader and fills dictionaries.
  Standard_EXPORT Voxel_DicomReader();

  //! Reads volume data from directory and returns it.
  Standard_EXPORT Standard_Boolean ReadDenseVolumeData (Handle(Graphic3d_VolumeData)&   theVolume,
                                                        const TCollection_AsciiString&  thePath) const;
};

#endif //_Voxel_DicomReader_HeaderFile

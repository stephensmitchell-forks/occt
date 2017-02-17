// Created on: 2017-02-16
// Created by: Sergey NIKONOV
// Copyright (c) 2000-2017 OPEN CASCADE SAS
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

#ifndef _XCAFDoc_AssemblyItemId_HeaderFile
#define _XCAFDoc_AssemblyItemId_HeaderFile

#include <Standard_GUID.hxx>
#include <TColStd_ListOfAsciiString.hxx>

class XCAFDoc_AssemblyItemId
{

public:

  Standard_EXPORT XCAFDoc_AssemblyItemId();
  Standard_EXPORT XCAFDoc_AssemblyItemId(const TColStd_ListOfAsciiString& thePath);
  Standard_EXPORT XCAFDoc_AssemblyItemId(const TCollection_AsciiString& theString);

  Standard_EXPORT void Init(const TColStd_ListOfAsciiString& thePath);
  Standard_EXPORT void Init(const TCollection_AsciiString& theString);

  Standard_EXPORT Standard_Boolean IsNull() const;
  Standard_EXPORT void Nullify();

  Standard_EXPORT Standard_Boolean IsChild(const XCAFDoc_AssemblyItemId& theOther) const;
  Standard_EXPORT Standard_Boolean IsDirectChild(const XCAFDoc_AssemblyItemId& theOther) const;

  Standard_EXPORT Standard_Boolean IsEqual(const XCAFDoc_AssemblyItemId& theOther) const;

  Standard_EXPORT const TColStd_ListOfAsciiString& GetPath() const;

  Standard_EXPORT TCollection_AsciiString ToString() const;

  struct Hasher
  {
    static int HashCode(const XCAFDoc_AssemblyItemId& theItem, 
                        const int                     upper)
    {
      return ::HashCode(theItem.ToString(), upper);
    }

    static int IsEqual(const XCAFDoc_AssemblyItemId& theItem1,
                       const XCAFDoc_AssemblyItemId& theItem2)
    {
      return theItem1.IsEqual(theItem2);
    }
  };

private:

  TColStd_ListOfAsciiString myPath;

};

#endif // _XCAFDoc_AssemblyItemId_HeaderFile

// Author: Kirill Gavrilov
// Copyright: Open CASCADE 2015-2017
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

#ifndef _RWObj_Tools_HeaderFile
#define _RWObj_Tools_HeaderFile

#include <gp_XYZ.hxx>
#include <Graphic3d_Vec3.hxx>
#include <TCollection_AsciiString.hxx>

//! Auxiliary tools for OBJ format parser.
namespace RWObj_Tools
{
  //! Read 3 float values.
  inline bool ReadVec3 (const char*     thePos,
                        char*&          theNext,
                        Graphic3d_Vec3& theVec)
  {
    const char* aPos = thePos;
    theVec.x() = (float )Strtod (aPos, &theNext);
    aPos = theNext;
    theVec.y() = (float )Strtod (aPos, &theNext);
    aPos = theNext;
    theVec.z() = (float )Strtod (aPos, &theNext);
    return aPos != theNext;
  }

  //! Read 3 double values.
  inline bool ReadVec3 (const char* thePos,
                        char*&      theNext,
                        gp_XYZ&     theVec)
  {
    const char* aPos = thePos;
    theVec.SetX (Strtod (aPos, &theNext));
    aPos = theNext;
    theVec.SetY (Strtod (aPos, &theNext));
    aPos = theNext;
    theVec.SetZ (Strtod (aPos, &theNext));
    return aPos != theNext;
  }

  //! Read string.
  inline bool ReadName (const char*              thePos,
                        TCollection_AsciiString& theName)
  {
    TCollection_AsciiString aName = TCollection_AsciiString (thePos);
    Standard_Integer aTail = aName.Length();
    if (aTail > 0
     && aName.Value (aTail) == '\n')
    {
      aName.SetValue (aTail--, '\0'); // replace '\n'
    }
    if (aTail > 0
     && aName.Value (aTail) == '\r')
    {
      aName.SetValue (aTail--, '\0'); // replace '\r'
    }
    theName = aName.ToCString();
    theName.LeftAdjust();
    theName.RightAdjust();
    return !theName.IsEmpty();
  }

  //! Return true if specified char is a white space.
  inline bool isSpaceChar (const char theChar)
  {
    return theChar == ' '
        || theChar == '\t';
    //return IsSpace (theChar);
  }
}

#endif // _RWObj_Tools_HeaderFile

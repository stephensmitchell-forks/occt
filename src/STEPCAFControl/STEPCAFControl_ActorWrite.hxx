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

#ifndef _STEPCAFControl_ActorWrite_HeaderFile
#define _STEPCAFControl_ActorWrite_HeaderFile

#include <Standard.hxx>
#include <Standard_DefineHandle.hxx>
#include <Standard_Boolean.hxx>
#include <TopTools_MapOfShape.hxx>
#include <STEPControl_ActorWrite.hxx>

class TopoDS_Shape;
class Standard_Transient;
class Handle(Standard_Type);
class Handle(STEPControl_ActorWrite);
class STEPCAFControl_ActorWrite;

DEFINE_STANDARD_HANDLE(STEPCAFControl_ActorWrite,STEPControl_ActorWrite)


//! Extends ActorWrite from STEPControl by analysis of <br>
//!          whether shape is assembly (based on information from DECAF) <br>
class STEPCAFControl_ActorWrite : public STEPControl_ActorWrite
{

public:

  Standard_EXPORT   STEPCAFControl_ActorWrite();
  
  //! Check whether shape S is assembly <br>
  //!          Returns True if shape is registered in assemblies map <br>
  Standard_EXPORT   virtual  Standard_Boolean IsAssembly(TopoDS_Shape& S) const;
  
  //! Set standard mode of work <br>
  //!          In standard mode Actor (default) behaves exactly as its <br>
  //!          ancestor, also map is cleared <br>
  Standard_EXPORT     void SetStdMode(const Standard_Boolean stdmode = Standard_True);
  
  //! Clears map of shapes registered as assemblies <br>
  Standard_EXPORT     void ClearMap();
  
  //! Registers shape to be written as assembly <br>
  //!          The shape should be TopoDS_Compound (else does nothing) <br>
  Standard_EXPORT     void RegisterAssembly(const TopoDS_Shape& S);

  DEFINE_STANDARD_RTTI(STEPCAFControl_ActorWrite)

private: 

 Standard_Boolean myStdMode;
 TopTools_MapOfShape myMap;
};
#endif

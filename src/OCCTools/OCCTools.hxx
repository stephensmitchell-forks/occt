// File: OCCTools.hxx
// Created: 21.02.2017
// Author: Mikhail Sazonov

#ifndef OCCTools_hxx
#define OCCTools_hxx

#include <Standard_TypeDef.hxx>

class TopoDS_Shape;

//! This class provides some useful static methods
class OCCTools
{
public:

  //! Reads STL file and creates a shape, in which each triangle from STL is 
  //! represented by a single TopoDS_Face.
  //! Returns null shape in the case of failure.
  Standard_EXPORT static TopoDS_Shape ReadStl(const char* theFName);

  //! Reads STEP file and creates a shape from its contents.
  //! Returns null shape in the case of failure.
  Standard_EXPORT static TopoDS_Shape ReadStep(const char* theFName);
};

#endif

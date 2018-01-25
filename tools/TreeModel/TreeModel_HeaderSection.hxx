// Created on: 2017-06-16
// Created by: Natalia ERMOLAEVA
// Copyright (c) 2017 OPEN CASCADE SAS
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

#ifndef TreeModel_HeaderSection_H
#define TreeModel_HeaderSection_H

#include <Standard.hxx>
#include <Standard_Macro.hxx>

#include <QString>

//! \class TreeModel_HeaderSection
//! \brief Container of tree view header sections, like width, visibility, text value
class TreeModel_HeaderSection
{
public:
  //! Constructor
  TreeModel_HeaderSection() {}

  //! Constructor
  TreeModel_HeaderSection (const QString& theName, const int theWidth, const bool theIsHidden = false)
  : myName (theName), myWidth (theWidth), myIsHidden (theIsHidden) {}

  //! Destructor
  ~TreeModel_HeaderSection() {}

  //! Sets text value
  //! \theName text value
  void SetName (const QString& theName) { myName = theName; }

  //! Returns text value
  QString GetName() const { return myName; }

  //! Sets section width
  //! \param theValue width value
  void SetWidth (const int theWidth) { myWidth = theWidth; }

  //! Returns section width
  int GetWidth() const { return myWidth; }

  //! Sets section width
  void SetIsHidden (bool isHidden) { myIsHidden = isHidden; }

  //! Returns if the section is visiblt
  bool IsHidden() const { return myIsHidden; }

private:
  QString myName;  //! text value
  int myWidth; //! section width
  bool myIsHidden; //! visibility
};

#endif

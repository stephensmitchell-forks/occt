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

#include <inspector/ViewControl_TableModelValues.hxx>

#include <QApplication>
#include <QFont>

// =======================================================================
// function : data
// purpose :
// =======================================================================
QVariant ViewControl_TableModelValues::data (const QModelIndex& theIndex, int theRole) const
{
  if (theRole == Qt::DisplayRole)
    return myValues.at (getPosition (theIndex.row(), theIndex.column()));

  if ((theRole == Qt::FontRole || theRole == Qt::ForegroundRole) && isItalicHeader(theIndex))
  {
    if (theRole == Qt::FontRole)
    {
      QFont aFont = qApp->font();
      aFont.setItalic (true);
      return aFont;
    }
    else
      QColor (Qt::darkGray).darker(150);
  }
  return QVariant();
}

// =======================================================================
// function : headerData
// purpose :
// =======================================================================
QVariant ViewControl_TableModelValues::headerData (int theSection, Qt::Orientation theOrientation, int theRole) const
{
   if (!myHeaderValues.contains (theOrientation))
     return QVariant();

  if (theRole == Qt::DisplayRole)
    return myHeaderValues[theOrientation][theSection].GetName();
  else if (theRole == Qt::ForegroundRole)
    return QColor (Qt::darkGray);

  return QVariant();
}

// =======================================================================
// function : isItalicHeader
// purpose :
// =======================================================================
bool ViewControl_TableModelValues::isItalicHeader (const QModelIndex& theIndex) const
{
  Qt::Orientation anOrientation = myOrientation == Qt::Vertical ? Qt::Horizontal : Qt::Vertical;
  int aCell = anOrientation == Qt::Horizontal ? theIndex.column() : theIndex.row();

  return GetHeaderItem (anOrientation, aCell).IsItalic();
}
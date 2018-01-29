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

#ifndef ViewControl_TableModelValues_H
#define ViewControl_TableModelValues_H

#include <Standard.hxx>

#include <inspector/TreeModel_HeaderSection.hxx>

#ifdef _MSC_VER
#pragma warning(disable : 4127) // conditional expression is constant
#endif
#include <QAbstractTableModel>
#include <QList>
#include <QVariant>

//! \class ViewControl_TableModelValues
//! \brief This is an interace for ViewControl_TableModel to give real values of the model
//! It should be filled or redefined.
class ViewControl_TableModelValues
{
public:

  //! Constructor
  Standard_EXPORT ViewControl_TableModelValues (const Qt::Orientation& theOrientation = Qt::Vertical)
  { SetOrientation (theOrientation); }

  //! Destructor
  virtual ~ViewControl_TableModelValues() {}

  //! Sets direction of the values applying, whether it should be placed by rows or by columns
  //! \param theOrientation if horizontal, the values are applyed by rows, otherwise by columns
  void SetOrientation (const Qt::Orientation& theOrientation) { myOrientation = theOrientation; }

  //! Fills the model values.
  //! \param theValues a container of table model values
  void SetValues (const QVector<QVariant>& theValues) { myValues = theValues; }

  //! Fills the model header values for orientation.
  //! \param theValues a container of header text values
  //! \param theOrientation an orientation of header
  void SetHeaderValues (const QList<TreeModel_HeaderSection>& theValues, const Qt::Orientation theOrientation)
  { myHeaderValues.insert (theOrientation, theValues); }

  //! Returns whether the column is hidden by default
  //! \param theColumnId a column index
  //! \return header section values container
  TreeModel_HeaderSection GetHeaderItem (const Qt::Orientation theOrientation, const int theColumnId) const
  { return myHeaderValues.contains(theOrientation) ? myHeaderValues[theOrientation][theColumnId] : TreeModel_HeaderSection(); }

  //! Stores information about table view header visibility
  //! \param theOrientation an orientation of header
  //! \param theVisibility if true, header is visible
  void SetHeaderVisible (const Qt::Orientation theOrientation, const bool theVisibility)
  { myVisibleHeader.insert (theOrientation, theVisibility); }

  //! Stores information about table view header visibility
  //! \param theOrientation an orientation of header
  //! \param theVisibility if true, header is visible
  bool IsHeaderVisible (const Qt::Orientation theOrientation) const
  { return myVisibleHeader[theOrientation]; }

  //! Returns number of columns, size of header values
  //! \param theParent an index of the parent item
  //! \return an integer value
  virtual int columnCount (const QModelIndex& theParent = QModelIndex()) const
  { (void)theParent; return myOrientation == Qt::Vertical ? myHeaderValues[Qt::Horizontal].size() : myHeaderValues[Qt::Vertical].size(); }

  //! Returns number of rows, depending on orientation: myColumnCount or size of values container
  //! \param theParent an index of the parent item
  //! \return an integer value
  virtual int rowCount (const QModelIndex& theParent = QModelIndex()) const
  { return columnCount (theParent) > 0 ? myValues.size() / columnCount (theParent) : 0; }

  //! Returns content of the model index for the given role, it is obtained from internal container of values
  //! It returns value only for DisplayRole.
  //! \param theIndex a model index
  //! \param theRole a view role
  //! \return value intepreted depending on the given role
  Standard_EXPORT virtual QVariant data (const QModelIndex& theIndex, int theRole = Qt::DisplayRole) const;

  //! Returns content of the model index for the given role, it is obtainer from internal container of header values
  //! It returns value only for DisplayRole.
  //! \param theSection an index of value in the container 
  //! \param theIndex a model index
  //! \param theRole a view role
  //! \return value intepreted depending on the given role
  Standard_EXPORT virtual QVariant headerData (int theSection, Qt::Orientation theOrientation, int theRole = Qt::DisplayRole) const;

  //! Returns flags for the item: ItemIsEnabled | Qt::ItemIsSelectable
  //! \param theIndex a model index
  //! \return flags
  virtual Qt::ItemFlags flags (const QModelIndex& theIndex) const
  { return theIndex.isValid() ? Qt::ItemIsEnabled | Qt::ItemIsSelectable : Qt::NoItemFlags; }

protected:
  //! Finds position in internal vector of values using the table column/row count
  //! \param theRow a row of a table cell
  //! \param theColumn a column of a table cell
  size_t getPosition (const int theRow, const int theColumn) const { return columnCount() * theRow + theColumn; }

  //! Returns true if the header item is italic of the parameter index
  //! \param theIndex a model index
  //! \param boolean value
  bool isItalicHeader (const QModelIndex& theIndex) const;

protected:

  Qt::Orientation myOrientation; //!< orientation how the values should fill the current table view
  QMap<Qt::Orientation, QList<TreeModel_HeaderSection> > myHeaderValues; //!< table header values
  QMap<Qt::Orientation, bool> myVisibleHeader; //! table header visibility
  QVector<QVariant> myValues; //! cached container of table values
};

#endif

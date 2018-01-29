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

#ifndef ViewControl_Table_H
#define ViewControl_Table_H

#include <Standard.hxx>
#include <Standard_Macro.hxx>

#ifdef _MSC_VER
#pragma warning(disable : 4127) // conditional expression is constant
#endif

#include <QObject>

class QAbstractTableModel;
//class QAction;
class QContextMenuEvent;
//class QLabel;
//class QPlainTextEdit;
//class QPushButton;
//class QSpinBox;
class QTableView;
class QWidget;

//! \class ViewControl_Table
//! \brief View to display table values with possibility to change table columns
//! if the table has 1D dimension and Horizontal orientation
class ViewControl_Table : public QObject
{
  Q_OBJECT
public:

  //! Constructor
  Standard_EXPORT ViewControl_Table (QWidget* theParent);

  //! Destructor
  virtual ~ViewControl_Table() Standard_OVERRIDE {}

  //! Fills table view and table size control by the model
  //! \param theModel values model
  Standard_EXPORT void Init (QAbstractTableModel* theModel);

  //! \return the text edit control
  QWidget* GetControl() const { return myMainWidget; }

  //! \return the table view
  QTableView* GetTableView() const { return myTableView; }

  //! Shows context menu for tree view selected item. It contains export action.
  //! \param theEvent a context menu event
  //void ProcessContextMenuRequsted (QContextMenuEvent* theEvent);

signals:
  //! Signals about header cell is clicked
  //! \param theEvent context menu event
  //void headerContextMenuRequsted (QContextMenuEvent* theEvent);

protected slots:
  //! Reacts to the spin box value change, it divides table model values to has given number of columns
  //! \param theValue a new value of spin box
  //void onSeparateSizeChanged (int theValue);

  //! Reacts to the tolerance value change, it sets it into table model
  //! \param theValue a new value of spin box
  //void onToleranceSizeChanged (int theValue);

  //! Updates controls by selection change in table view
  //! \param theSelected container of selected table cells
  //! \param theDeselected container of selected table cells
  //void onTableSelectionChanged(const QItemSelection& theSelected, const QItemSelection& theDeselected);

private:
  QWidget* myMainWidget; //!< parent of all controls
  //QLabel* mySize; //!< control to visualize initial values size
  //QSpinBox* mySeparateSize; //!< control to divide table by given size if thable has only 1 dimension
  //QLabel* myVisualizedSize; //!< control to visualize current table size
  //QLabel* myTolerance; //!< control to visualize tolerance size
  //QSpinBox* myToleranceSize; //!< control to change table values tolerance

  QTableView* myTableView; //!< table view
  //QPlainTextEdit* myModelInformation; //!< control to visualize current table size
  //QPushButton* myMinValue; //!< minimum table value
  //QPushButton* myMaxValue; //!< maximum table value
  //QPushButton* myMinSelectedValue; //! minimum value of selected table cells
  //QPushButton* myMaxSelectedValue; //! maximum value of selected table cells
};
#endif

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

#ifndef TreeModel_PropertyView_H
#define TreeModel_PropertyView_H

#include <Standard.hxx>
#include <Standard_Macro.hxx>

#include <inspector/ViewControl_Table.hxx>

#ifdef _MSC_VER
#pragma warning(disable : 4127) // conditional expression is constant
#endif
#include <QObject>
#include <QList>

class QAbstractTableModel;
class QStackedWidget;
class QWidget;
class QVBoxLayout;

class ViewControl_TableModelValues;

//! \class ViewControl_PropertyView
//! \brief View widget where several tables are visualized in vertical layout.
class ViewControl_PropertyView : public QObject
{
  Q_OBJECT
public:

  //! Constructor
  Standard_EXPORT ViewControl_PropertyView (QWidget* theParent);

  //! Destructor
  virtual ~ViewControl_PropertyView() Standard_OVERRIDE {}

  //! Fills the view content with values. Number of visible tables is size of container,
  //! Each element of container is values of the corresponded table
  //! \param theTableValues values
  Standard_EXPORT void Init (const QList<ViewControl_TableModelValues*>& theTableValues);

  //! Fills the view content with the parameter custom widget.
  //! \param theWidget control
  Standard_EXPORT void Init (QWidget* theWidget);

  //! Clears layout of the view and tables models.
  Standard_EXPORT void Clear();

  //! \return the text edit control
  QWidget* GetControl() const { return myMainWidget; }

protected:
  //! Returns table instance or create if it was not created ealier
  //! \param theTableId an index in internal container of tables
  //! \param isToCreate if true, the table is created if not exists
  ViewControl_Table* findTable (const int theTableId, const bool isToCreate = true);

private:
  QWidget* myMainWidget; //! parent of all controls

  QStackedWidget* myAttributesStack; //!< container of already created panes
  QWidget* myEmptyWidget; //!< an empty widget when nothing is selected in tree view

  QWidget* myTableWidget; //!< widget of tables in vertical layout
  QVBoxLayout* myTableWidgetLayout; //! main view layout where tables or custom widgets are presented
  QList<ViewControl_Table*> myTables; //!< table view, shown only first tables filled in Init method
  QWidget* myCustomWidget; //!< custom view widget
};
#endif

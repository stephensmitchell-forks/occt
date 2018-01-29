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

#include <inspector/ViewControl_PropertyView.hxx>
#include <inspector/ViewControl_Table.hxx>
#include <inspector/ViewControl_TableModel.hxx>
#include <inspector/ViewControl_TableModelValues.hxx>
#include <inspector/ViewControl_Tools.hxx>

#include <QAbstractTableModel>
#include <QStackedWidget>
#include <QTableView>
#include <QVBoxLayout>
#include <QWidget>

// =======================================================================
// function : Constructor
// purpose :
// =======================================================================
ViewControl_PropertyView::ViewControl_PropertyView (QWidget* theParent)
: QObject (theParent)
{
  myMainWidget = new QWidget (theParent);
  myMainWidget->setMinimumSize (QSize (1, 1));
  ViewControl_Tools::SetWhiteBackground (myMainWidget);

  QGridLayout* aLayout = new QGridLayout (myMainWidget);
  aLayout->setContentsMargins (0, 0, 0, 0);

  //myAttributesStack = new DFBrowser_AttributePaneStack (this);
  //myAttributesStack->CreateWidget (myMainWidget);
  myAttributesStack = new QStackedWidget (myMainWidget);
  ViewControl_Tools::SetWhiteBackground (myAttributesStack);
  aLayout->addWidget (myAttributesStack, 0, 0);

  myEmptyWidget = new QWidget (myAttributesStack);
  ViewControl_Tools::SetWhiteBackground (myEmptyWidget);
  myAttributesStack->addWidget (myEmptyWidget);

  myTableWidget = new QWidget (myAttributesStack);
  myTableWidgetLayout = new QVBoxLayout (myTableWidget);
  myTableWidgetLayout->setContentsMargins (0, 0, 0, 0);
  myAttributesStack->addWidget (myTableWidget);

  //myTreeLevelView = new DFBrowser_TreeLevelView (theParent);
  //myAttributesStack->addWidget (myTreeLevelView->GetControl());

  //mySearchView = new DFBrowser_SearchView (theParent);
  //myAttributesStack->addWidget (mySearchView->GetControl());

  myAttributesStack->setCurrentWidget (myEmptyWidget);

  //myMainWidget = new QWidget (theParent);
  //ViewControl_Tools::SetWhiteBackground (myMainWidget);

  //myMainLayout = new QVBoxLayout (myMainWidget);
  //myMainLayout->setContentsMargins (0, 0, 0, 0);

  //QWidget* anEmptyWidget = new QWidget (myMainWidget);
  //anEmptyWidget->setMinimumSize (40, 40);
  //ViewControl_Tools::SetWhiteBackground (anEmptyWidget);

  //myMainLayout->addWidget(anEmptyWidget);

  //QList<ViewControl_Table*> myTables; //!< table view, shown only first tables filled in Init method
  //QWidget* myCustomWidget; //!< custom view widget

  //myTable (new ViewControl_Table (theParent, theModel))
  //aLayout->addWidget (myTable->GetControl());
  //myTable->GetControl()->setVisible (false);
}

// =======================================================================
// function : Init
// purpose :
// =======================================================================
void ViewControl_PropertyView::Init (const QList<ViewControl_TableModelValues*>& theTableValues)
{
  for (int aTableId = 0; aTableId < theTableValues.size(); aTableId++)
  {
    ViewControl_TableModelValues* aValues = theTableValues[aTableId];

    ViewControl_Table* aTable = findTable (aTableId);
    ViewControl_TableModel* aModel = dynamic_cast<ViewControl_TableModel*> (aTable->GetTableView()->model());
    aModel->SetModelValues (aValues);
    ViewControl_Tools::SetDefaultHeaderSections (aTable->GetTableView(), Qt::Horizontal);

    aTable->GetTableView()->setVisible (true);
  }
  // hide not used tables
  for (int aTableId = theTableValues.size(); aTableId < myTables.size(); aTableId++)
  {
    ViewControl_Table* aTable = findTable (aTableId, false);
    if (!aTable)
      continue;

    ViewControl_TableModel* aModel = dynamic_cast<ViewControl_TableModel*> (aTable->GetTableView()->model());
    aModel->SetModelValues (0);
    aTable->GetTableView()->setVisible (false);
  }

  if (theTableValues.size() > 0)
    myAttributesStack->setCurrentWidget (myTableWidget);
  else
    myAttributesStack->setCurrentWidget (myEmptyWidget);
}

// =======================================================================
// function : Init
// purpose :
// =======================================================================
void ViewControl_PropertyView::Init (QWidget* theWidget)
{
}

// =======================================================================
// function : Clear
// purpose :
// =======================================================================
void ViewControl_PropertyView::Clear()
{
  for (int aTableId = 0; aTableId < myTables.size(); aTableId++)
  {
    ViewControl_Table* aTable = findTable (aTableId, false);
    if (!aTable)
      continue;

    ViewControl_TableModel* aModel = dynamic_cast<ViewControl_TableModel*> (aTable->GetTableView()->model());
    aModel->SetModelValues (0);

    aTable->GetTableView()->setVisible (true);
  }
  myAttributesStack->setCurrentWidget (myEmptyWidget);
}

// =======================================================================
// function : findTable
// purpose :
// =======================================================================
ViewControl_Table* ViewControl_PropertyView::findTable (const int theTableId, const bool isToCreate)
{
  if (!isToCreate && theTableId >= myTables.size())
    return 0;

  if (theTableId < myTables.size())
    return myTables[theTableId];

  ViewControl_Table* aTable = new ViewControl_Table (myMainWidget);
  ViewControl_TableModel* aModel = new ViewControl_TableModel(aTable->GetTableView());
  aTable->GetTableView()->setModel (aModel);
  myTableWidgetLayout->addWidget (aTable->GetControl());

  myTables.insert (theTableId, aTable);

  return myTables[theTableId];
}

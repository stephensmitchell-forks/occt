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

#include <inspector/ViewControl_Tools.hxx>
#include <inspector/ViewControl_TableModel.hxx>

#include <QAction>
#include <QObject>
#include <QPalette>
#include <QTableView>
#include <QWidget>

// =======================================================================
// function : CreateAction
// purpose :
// =======================================================================
QAction* ViewControl_Tools::CreateAction (const QString& theText, const char* theSlot, QObject* theParent, QObject* theContext)
{
  QAction* anAction = new QAction (theText, theParent);
  QObject::connect (anAction, SIGNAL (triggered (bool)), theContext, theSlot);
  return anAction;
}

// =======================================================================
// function : SetWhiteBackground
// purpose :
// =======================================================================
void ViewControl_Tools::SetWhiteBackground (QWidget* theControl)
{
  QPalette aPalette = theControl->palette();
  aPalette.setColor (QPalette::All, QPalette::Foreground, Qt::white);
  theControl->setPalette (aPalette);
}

// =======================================================================
// function : SetDefaultHeaderSections
// purpose :
// =======================================================================
void ViewControl_Tools::SetDefaultHeaderSections(QTableView* theTableView, const Qt::Orientation theOrientation)
{
  ViewControl_TableModel* aTableModel = dynamic_cast<ViewControl_TableModel*> (theTableView->model());
  ViewControl_TableModelValues* aModelValues = aTableModel->GetModelValues();
  if (!aModelValues)
    return;

  for (int aColumnId = 0, aNbColumns = aTableModel->columnCount(); aColumnId < aNbColumns; aColumnId++)
  {
    TreeModel_HeaderSection aSection = aModelValues->GetHeaderItem (theOrientation, aColumnId);

    theTableView->setColumnWidth (aColumnId, aSection.GetWidth());
    theTableView->setColumnHidden (aColumnId, aSection.IsHidden());
  }
}

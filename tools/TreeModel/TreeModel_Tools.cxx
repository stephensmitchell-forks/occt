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

#include <inspector/TreeModel_Tools.hxx>
#include <inspector/TreeModel_ModelBase.hxx>
#include <inspector/TreeModel_ColumnType.hxx>
#include <inspector/TreeModel_VisibilityState.hxx>

#include <QAction>
#include <QApplication>
#include <QFont>
#include <QFontMetrics>
#include <QHeaderView>
#include <QObject>
#include <QRegExp>
#include <QStringList>
#include <QStyle>
#include <QTreeView>

// =======================================================================
// function : ToString
// purpose :
// =======================================================================
QString TreeModel_Tools::ToString (const QByteArray& theValue)
{
  char aBuffer[8];
  QStringList aBytesList;
  for (int aByteId = 0; aByteId < theValue.size();  aByteId++)
  {
    ::sprintf (aBuffer, "#%02X", (unsigned char)theValue.at (aByteId));
    aBytesList.append (QString (aBuffer));
  }
  return QString ("@ByteArray[%1]").arg (aBytesList.join (" "));
}

// =======================================================================
// function : ToByteArray
// purpose :
// =======================================================================
QByteArray TreeModel_Tools::ToByteArray (const QString& theValue)
{
  QByteArray aStateArray;
  if (!theValue.startsWith ("@ByteArray[") || !theValue.endsWith (']'))
    return aStateArray;

  QString aValue = theValue.mid (11, theValue.size() - 12);
  QStringList lst = aValue.split (QRegExp ("[\\s|,]"), QString::SkipEmptyParts);
  for (QStringList::ConstIterator aByteId = lst.begin(); aByteId != lst.end(); ++aByteId)
  {
    int aBase = 10;
    QString aString = *aByteId;
    if (aString.startsWith ("#"))
    {
      aBase = 16;
      aString = aString.mid (1);
    }
    bool isOk = false;
    int aNum = aString.toInt (&isOk, aBase);
    if (!isOk || aNum < 0 || aNum > 255)
      continue;
    aStateArray.append ((char)aNum);
  }
  return aStateArray;
}

// =======================================================================
// function : CreateAction
// purpose :
// =======================================================================
QAction* TreeModel_Tools::CreateAction (const QString& theText, const char* theSlot, QObject* theParent, QObject* theContext)
{
  QAction* anAction = new QAction (theText, theParent);
  QObject::connect (anAction, SIGNAL (triggered (bool)), theContext, theSlot);
  return anAction;
}

// =======================================================================
// function : SaveState
// purpose :
// =======================================================================
void TreeModel_Tools::SaveState (QTreeView* theTreeView, QMap<QString, QString>& theItems,
                                 const QString& thePrefix)
{
  QStringList aColumnWidths, aHiddenColumns;
  for (int aColumnId = 0; aColumnId < theTreeView->model()->columnCount(); aColumnId++)
  {
    if (theTreeView->isColumnHidden (aColumnId))
    {
      aHiddenColumns.append (QString::number (aColumnId));
      aColumnWidths.append (QString());
    }
    else
      aColumnWidths.append (QString::number (theTreeView->columnWidth (aColumnId)));
  }
  theItems[thePrefix + "columns_width"] = aColumnWidths.join (",");
  theItems[thePrefix + "columns_hidden"] = aHiddenColumns.join (",");
}

// =======================================================================
// function : RestoreState
// purpose :
// =======================================================================
bool TreeModel_Tools::RestoreState (QTreeView* theTreeView, const QString& theKey, const QString& theValue,
                                    const QString& thePrefix)
{
  if (theKey == thePrefix + "columns_width")
  {
    QStringList aValues = theValue.split (",");
    for (int aColumnId = 0; aColumnId < theTreeView->model()->columnCount() && aColumnId < aValues.size(); aColumnId++)
    {
      bool isOk;
      int aWidth = aValues.at (aColumnId).toInt (&isOk);
      if (isOk && !theTreeView->isColumnHidden (aColumnId)) // do not resize hidden columnsa
        theTreeView->setColumnWidth (aColumnId, aWidth);
    }
  }
  else if (theKey == thePrefix + "columns_hidden")
  {
    int aColumnSize = theTreeView->model()->columnCount();
    QStringList aValues = theValue.split (",", QString::SplitBehavior::SkipEmptyParts);
    QList<int> aColumnIds;
    for (int aValueId = 0; aValueId < aValues.size(); aValueId++)
    {
      if (aValueId < aColumnSize)
        aColumnIds.append (aValues.at (aValueId).toInt());
    }
    for (int aColumnId = 0; aColumnId < aColumnSize; aColumnId++)
      theTreeView->setColumnHidden (aColumnId, aColumnIds.contains (aColumnId));
  }
  else
    return false;
  return true;
}

// =======================================================================
// function : SetDefaultHeaderSections
// purpose :
// =======================================================================
void TreeModel_Tools::SetDefaultHeaderSections(QTreeView* theTreeView)
{
  TreeModel_ModelBase* aTreeModel = dynamic_cast<TreeModel_ModelBase*> (theTreeView->model());

  for (int aColumnId = 0, aNbColumns = aTreeModel->columnCount(); aColumnId < aNbColumns; aColumnId++)
  {
    theTreeView->setColumnWidth (aColumnId, aTreeModel->ColumnWidth (aColumnId));
    theTreeView->setColumnHidden (aColumnId, aTreeModel->GetHeaderItem (aColumnId).IsHidden());
  }
}

// =======================================================================
// function : UseVisibilityColumn
// purpose :
// =======================================================================
void TreeModel_Tools::UseVisibilityColumn (QTreeView* theTreeView, const bool theActive)
{
  QHeaderView* aHeader = theTreeView->header();
#if QT_VERSION < 0x050000
  aHeader->setResizeMode (TreeModel_ColumnType_Visibility, QHeaderView::Fixed);
#else
  aHeader->setSectionResizeMode (TreeModel_ColumnType_Visibility, QHeaderView::Fixed);
#endif
  aHeader->moveSection (TreeModel_ColumnType_Name, TreeModel_ColumnType_Visibility);

  TreeModel_ModelBase* aModel = dynamic_cast<TreeModel_ModelBase*> (theTreeView->model());
  aModel->SetHeaderItem (TreeModel_ColumnType_Visibility,
    TreeModel_HeaderSection ("Visibility", TreeModel_ModelBase::ColumnVisibilityWidth()));

  TreeModel_VisibilityState* aVisibilityState = aModel->GetVisibilityState ();

  aModel->SetUseVisibilityColumn (true);
  if (theActive && aVisibilityState)
    QObject::connect (theTreeView, SIGNAL (clicked (const QModelIndex&)), 
                      aVisibilityState, SLOT (OnClicked(const QModelIndex&)));
}

// =======================================================================
// function : GetTextWidth
// purpose :
// =======================================================================
int TreeModel_Tools::GetTextWidth (const QString& theText, QObject*)
{
  // TODO: find margins like QApplication::style()->pixelMetric(QStyle::PM_FocusFrameHMargin, 0, (QWidget*)theParent);
  int aTextMargin = 10;
  QFontMetrics aFontMetrics (QApplication::font());
  QRect aBoundingRect = aFontMetrics.boundingRect (theText);
  return qMax (aBoundingRect.width(), aFontMetrics.width (theText)) + aTextMargin * 2;
}

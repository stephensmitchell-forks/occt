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

#include <inspector/MessageModel_ItemAlert.hxx>

#include <inspector/MessageModel_ItemRoot.hxx>
#include <inspector/MessageModel_ItemReport.hxx>
#include <inspector/MessageModel_Tools.hxx>

#include <TCollection_AsciiString.hxx>

#include <QColor>

// =======================================================================
// function : initValue
// purpose :
// =======================================================================
QVariant MessageModel_ItemAlert::initValue (const int theRole) const
{
  if (theRole == Qt::ForegroundRole)
  {
    Handle(Message_Report) aReport = findReport();
    if (aReport.IsNull())
      return QVariant();

    if (!aReport->GetAlerts (Message_Fail).IsEmpty())
      return QColor(Qt::darkRed);

    for (int aGravityId = (int)Message_Trace; aGravityId <= (int)Message_Fail; aGravityId++)
    {
      if (!aReport->IsActive ((Message_Gravity)aGravityId))
        return QColor(Qt::darkGray);
    }
    return QVariant();
  }

  if (theRole != Qt::DisplayRole && theRole != Qt::ToolTipRole)
    return QVariant();

  Handle(Message_Alert) anAlert = getAlert();
  const Message_ListOfAlert& aUnitedAlerts = GetUnitedAlerts();
  if (anAlert.IsNull() && !aUnitedAlerts.IsEmpty()) // give information about last united alert
    anAlert = !isReversed() ? aUnitedAlerts.Last() : aUnitedAlerts.First();
  if (anAlert.IsNull())
    return QVariant();

  switch (Column())
  {
    case 0:
      return theRole == Qt::DisplayRole ? anAlert->GetMessageKey() : anAlert->DynamicType()->Name();
    case 1: return QVariant(); // visibility state
    case 2: return rowCount() > 0 ? QVariant (rowCount()) : QVariant();
    case 3: return anAlert->ElapsedTime() < 0 ? QVariant() : anAlert->ElapsedTime();
    case 4: return AmountElapsedTime (anAlert);
    case 5:
    {
      TreeModel_ItemBasePtr aParentItem = Parent();
      MessageModel_ItemReportPtr aReportItem = itemDynamicCast<MessageModel_ItemReport> (aParentItem);
      while (!aReportItem)
      {
        aParentItem = aParentItem->Parent();
        aReportItem = itemDynamicCast<MessageModel_ItemReport> (aParentItem);
      }
      double aDivideTo = MessageModel_ItemReport::AmountElapsedTime (aReportItem->GetReport());
      return aDivideTo == 0 ? QVariant() : 100. * AmountElapsedTime (anAlert) / aDivideTo;
    }
    case 6: return MessageModel_Tools::GetPointerAlertInfo (anAlert).ToCString();
    case 7: return MessageModel_Tools::GetShapeTypeAlertInfo (anAlert).ToCString();
    case 8: return MessageModel_Tools::GetStandardTypeAlertInfo (anAlert).ToCString();
    default: break;
  }
  return QVariant();
}

// =======================================================================
// function : initRowCount
// purpose :
// =======================================================================
int MessageModel_ItemAlert::initRowCount() const
{
  const Handle(Message_Alert)& anAlert = getAlert();
  if (anAlert.IsNull())
    return GetUnitedAlerts().Size();

  int aRowCount = 0;
  NCollection_Vector<Message_ListOfAlert> aUnitedAlerts;
  for (int aGravityId = Message_Trace; aGravityId <= Message_Fail; aGravityId++)
  {
    const Message_ListOfAlert& anAlerts  = anAlert->GetAlerts ((Message_Gravity)aGravityId);
    if (isUniteAlerts())
      GetUnitedAlerts (anAlerts, aUnitedAlerts);
    else
      aRowCount += anAlerts.Size();
  }
  return isUniteAlerts() ? aUnitedAlerts.Size() : aRowCount;
}

// =======================================================================
// function : createChild
// purpose :
// =======================================================================
TreeModel_ItemBasePtr MessageModel_ItemAlert::createChild (int theRow, int theColumn)
{
  return MessageModel_ItemAlert::CreateItem (currentItem(), theRow, theColumn);
}

// =======================================================================
// function : Init
// purpose :
// =======================================================================
void MessageModel_ItemAlert::Init()
{
  MessageModel_ItemReportPtr aReportItem = itemDynamicCast<MessageModel_ItemReport> (Parent());
  MessageModel_ItemAlertPtr anAlertItem;
  Handle(Message_Report) aReport;
  Handle(Message_Alert) anAlert;
  if (aReportItem)
    aReport = aReportItem->GetReport();
  else
  {
    anAlertItem = itemDynamicCast<MessageModel_ItemAlert> (Parent());
    if (anAlertItem)
      anAlert = anAlertItem->GetAlert();
  }
  if (aReport.IsNull() && anAlert.IsNull() && !anAlertItem)
    return;

  if (anAlert.IsNull() && anAlertItem) // union folder item
  {
    int aCurrentSubId = 0;
    for (Message_ListOfAlert::Iterator anAlertsIt (anAlertItem->GetUnitedAlerts()); anAlertsIt.More();
         anAlertsIt.Next(), aCurrentSubId++)
    {
      if (aCurrentSubId != Row())
        continue;
      myAlert = anAlertsIt.Value();
      MessageModel_ItemBase::Init();
      return;
    }
    return;
  }

  //// iterates through all gravity types, skip types where report is empty, if report is not empty, increment
  //// current index until it equal to the current row index
  Message_ListOfAlert anAlerts;
  NCollection_Vector<Message_ListOfAlert> aUnitedAlerts;
  int aRowId = Row();
  int aPreviousAlertsCount = 0;
  for (int aGravityId = Message_Trace; aGravityId <= Message_Fail; aGravityId++)
  {
    if (!aReport.IsNull())
      anAlerts = aReport->GetAlerts ((Message_Gravity)aGravityId);
    else if (!anAlert.IsNull())
      anAlerts = anAlert->GetAlerts ((Message_Gravity)aGravityId);

    if (isReversed())
      anAlerts.Reverse();

    if (isUniteAlerts())
    {
      GetUnitedAlerts (anAlerts, aUnitedAlerts);
      if (aRowId < aUnitedAlerts.Size())
      {
        anAlerts = aUnitedAlerts.Value (aRowId);

        if (anAlerts.Size() == 1)
          myAlert = anAlerts.First();
        else
          myUnitedAlerts = anAlerts;

        MessageModel_ItemBase::Init();
        return;
      }
    }
    else
    {
      if (aRowId < aPreviousAlertsCount + anAlerts.Size())
      {
        aRowId = aRowId - aPreviousAlertsCount;
        int aCurrentId = 0;
        for (Message_ListOfAlert::Iterator anAlertsIt (anAlerts); anAlertsIt.More(); anAlertsIt.Next(), aCurrentId++)
        {
          if (aCurrentId != Row())
            continue;
          myAlert = anAlertsIt.Value();
          MessageModel_ItemBase::Init();
          return;
        }
      }
      aPreviousAlertsCount += anAlerts.Size();
    }
  }
}

// =======================================================================
// function : Reset
// purpose :
// =======================================================================
void MessageModel_ItemAlert::Reset()
{
  MessageModel_ItemBase::Reset();
  myAlert = Handle(Message_Alert)();
  myUnitedAlerts.Clear();
}

// =======================================================================
// function : initItem
// purpose :
// =======================================================================
void MessageModel_ItemAlert::initItem() const
{
  if (IsInitialized())
    return;
  const_cast<MessageModel_ItemAlert*>(this)->Init();
}

// =======================================================================
// function : getAlert
// purpose :
// =======================================================================
const Handle(Message_Alert)& MessageModel_ItemAlert::getAlert() const
{
  initItem();
  return myAlert;
}

// =======================================================================
// function : getUnitedAlerts
// purpose :
// =======================================================================
const Message_ListOfAlert& MessageModel_ItemAlert::getUnitedAlerts() const
{
  initItem();
  return myUnitedAlerts;
}

// =======================================================================
// function : AmountElapsedTime
// purpose :
// =======================================================================
double MessageModel_ItemAlert::AmountElapsedTime(const Handle(Message_Alert)& theAlert)
{
  if (theAlert.IsNull())
    return 0;

  double anAmountTime = 0;
  if (theAlert->ElapsedTime() > 0)
    anAmountTime = theAlert->ElapsedTime();

  for (int aGravityId = Message_Trace; aGravityId <= Message_Fail; aGravityId++)
  {
    for (Message_ListOfAlert::Iterator anAlertsIt(theAlert->GetAlerts((Message_Gravity)aGravityId)); anAlertsIt.More(); anAlertsIt.Next())
    {
      double aSubItemTime = AmountElapsedTime(anAlertsIt.Value());
      if (aSubItemTime > 0)
        anAmountTime += aSubItemTime;
    }
  }
  return anAmountTime;
}

// =======================================================================
// function : GetUnitedAlerts
// purpose :
// =======================================================================
void MessageModel_ItemAlert::GetUnitedAlerts(const Message_ListOfAlert& theAlerts,
                       NCollection_Vector<Message_ListOfAlert>& theUnitedAlerts)
{
  //theUnitedAlerts.Clear();
  TCollection_AsciiString anAlertMessageKey;
  for (Message_ListOfAlert::Iterator anAlertsIt (theAlerts); anAlertsIt.More(); anAlertsIt.Next())
  {
    Handle(Message_Alert) anAlert = anAlertsIt.Value();
    if (anAlertMessageKey.IsEqual (anAlert->GetMessageKey())) {
      Message_ListOfAlert anAlerts = theUnitedAlerts.Last();
      anAlerts.Append (anAlert);
      theUnitedAlerts.SetValue(theUnitedAlerts.Size()-1, anAlerts);
    }
    else {
      Message_ListOfAlert anAlerts;
      anAlerts.Append (anAlert);
      theUnitedAlerts.Append (anAlerts);
      anAlertMessageKey = anAlert->GetMessageKey();
    }
  }
}

// =======================================================================
// function : findReportItem
// purpose :
// =======================================================================
Handle(Message_Report) MessageModel_ItemAlert::findReport() const
{
  TreeModel_ItemBasePtr anItem = Parent();
  while (anItem)
  {
    if (MessageModel_ItemReportPtr aReportItem = itemDynamicCast<MessageModel_ItemReport>(anItem))
      return aReportItem->GetReport();

    anItem = anItem->Parent();
  }
  return Handle(Message_Report)();
}

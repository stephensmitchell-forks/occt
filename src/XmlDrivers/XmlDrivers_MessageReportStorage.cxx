// Created on:
// Created by:
// Copyright (c) 2001-2014 OPEN CASCADE SAS
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

#include <XmlDrivers_MessageReportStorage.hxx>

#include <Message.hxx>
#include <Message_Report.hxx>

#include <TDataStd_AsciiString.hxx>
#include <TDataStd_Real.hxx>
#include <TDataStd_Name.hxx>
#include <TDataStd_RealArray.hxx>
#include <TDF_ChildIterator.hxx>
#include <TDocStd_Application.hxx>
#include <TDocStd_Document.hxx>
#include <XmlDrivers.hxx>

// =======================================================================
// function : GetApplication
// purpose :
// =======================================================================
const Handle(TDocStd_Application)& GetApplication()
{
  static Handle(TDocStd_Application) anApp;
  if (anApp.IsNull())
  {
    anApp = new TDocStd_Application;
    XmlDrivers::DefineFormat (anApp);
  }
  return anApp;
}

// =======================================================================
// function : ExportReport
// purpose :
// =======================================================================
void XmlDrivers_MessageReportStorage::ExportReport (const Handle(Message_Report)& theReport,
                                                    const TCollection_AsciiString& theFileName)
{
  if (theReport.IsNull())
    return;

  Handle(TDocStd_Document) aDocument;
  GetApplication()->NewDocument (TCollection_ExtendedString("XmlOcaf"), aDocument);

  TDF_Label aMainLabel = aDocument->Main();

  for (int aGravityId = Message_Trace; aGravityId <= Message_Fail; aGravityId++)
  {
    Message_Gravity aGravity = (Message_Gravity)aGravityId;

    if (theReport->GetAlerts (aGravity).Size() == 0)
      continue;
    // Gravity Label
    TDF_Label aGravityLabel = aMainLabel.NewChild();
    // set gravity kind string
    TCollection_ExtendedString aName (Message::GravityToString ((Message_Gravity)aGravity));
    if (!aName.IsEmpty())
      TDataStd_Name::Set (aGravityLabel, aName);

    /// reserved label to store gravity information
    //TDF_Label aFirstAlertLabel = aGravityLabel.NewChild();

    // Alert Labels: labels are started from the second index
    const Message_ListOfAlert& anAlerts = theReport->GetAlerts(aGravity);
    for (Message_ListOfAlert::Iterator anAlertsIt (anAlerts); anAlertsIt.More(); anAlertsIt.Next())
    {
      exportAlert (anAlertsIt.Value(), aGravityLabel);
    }
  }
  GetApplication()->SaveAs (aDocument, theFileName);
}

// =======================================================================
// function : ImportReport
// purpose :
// =======================================================================
Handle(Message_Report) XmlDrivers_MessageReportStorage::ImportReport (const TCollection_AsciiString& theFileName)
{
  Handle(TDocStd_Document) aDocument;
  GetApplication()->Open (theFileName, aDocument);
  if (aDocument.IsNull())
    return Handle(Message_Report)();

  TDF_Label aMainLabel = aDocument->Main();
  if (aMainLabel.IsNull())
    return Handle(Message_Report)();

  TDF_Label aLabel;
  Handle(Message_Report) aReport = new Message_Report();
  for (TDF_ChildIterator aLabelsIt(aMainLabel); aLabelsIt.More(); aLabelsIt.Next())
  {
    TDF_Label aGravityLabel = aLabelsIt.Value();
    if (aGravityLabel.IsNull())
      continue;
    Handle(TDF_Attribute) anAttribute;
    if (!aGravityLabel.FindAttribute (TDataStd_Name::GetID(), anAttribute))
      continue;
    Handle(TDataStd_Name) aNameAttribute = Handle(TDataStd_Name)::DownCast (anAttribute);
    if (aNameAttribute.IsNull())
      continue;

    // get gravity type
    const TCollection_ExtendedString& aGravityName = aNameAttribute->Get();
    Message_Gravity aGravity = Message::GravityFromString (TCollection_AsciiString (aGravityName).ToCString());

    /// reserved label to store gravity information
    //TDF_Label aFirstAlertLabel = aGravityLabel.FindChild (1, Standard_False);

    // find alerts information, add corresponded alerts to the report
    for (TDF_ChildIterator anAlertLabelsIt (aGravityLabel); anAlertLabelsIt.More(); anAlertLabelsIt.Next())
    {
      TDF_Label anAlertLabel = anAlertLabelsIt.Value();
      if (anAlertLabel.IsNull())
        continue;

      importAlert (anAlertLabel, aGravity, aReport, Handle(Message_Alert)());
    }
  }
  return aReport;
}

// =======================================================================
// function : exportAlert
// purpose :
// =======================================================================
void XmlDrivers_MessageReportStorage::exportAlert (const Handle(Message_Alert)& theAlert, const TDF_Label& theParentLabel)
{
  TDF_Label anAlertLabel = theParentLabel.NewChild();
  TDataStd_Name::Set (anAlertLabel, theAlert->DynamicType()->Name());

  /// reserved label to store parameters of the current label
  TDF_Label anAlertParmetersLabel = anAlertLabel.NewChild();
  TDataStd_Name::Set (anAlertParmetersLabel, theAlert->DynamicType()->Name());
  exportAlertParameters (theAlert, anAlertParmetersLabel);

  for (int aGravityId = Message_Trace; aGravityId <= Message_Fail; aGravityId++)
  {
    // Gravity Label
    TDF_Label aGravityLabel = anAlertLabel.NewChild();
    // set gravity kind string
    TDataStd_Name::Set (aGravityLabel, Message::GravityToString ((Message_Gravity)aGravityId));

    const Message_ListOfAlert& anAlerts = theAlert->GetAlerts ((Message_Gravity)aGravityId);
    for (Message_ListOfAlert::Iterator anAlertsIt (anAlerts); anAlertsIt.More(); anAlertsIt.Next())
      exportAlert (anAlertsIt.Value(), aGravityLabel);
  }
}

// =======================================================================
// function : importAlert
// purpose :
// =======================================================================
void XmlDrivers_MessageReportStorage::importAlert (const TDF_Label& theAlertLabel, Message_Gravity theGravity,
  Handle(Message_Report)& theReport, const Handle(Message_Alert)& theParentAlert)
{
  TDF_Label aParametersLabel = theAlertLabel.FindChild (1, Standard_False);
  Handle(Message_Alert) anAlert = importAlertParameters (aParametersLabel);
  if (anAlert.IsNull())
    return;

  TDF_Label aLabel;
  TDF_ChildIterator aSubAlertsLabelsIt (theAlertLabel);
  aSubAlertsLabelsIt.Next(); // do not processing the first (parameters) label
  for (; aSubAlertsLabelsIt.More(); aSubAlertsLabelsIt.Next())
  {
    TDF_Label aGravityLabel = aSubAlertsLabelsIt.Value();
    if (aGravityLabel.IsNull())
      continue;
    Handle(TDF_Attribute) anAttribute;
    if (!aGravityLabel.FindAttribute (TDataStd_Name::GetID(), anAttribute))
      continue;
    Handle(TDataStd_Name) aNameAttribute = Handle(TDataStd_Name)::DownCast (anAttribute);
    if (aNameAttribute.IsNull())
      continue;

    // get gravity type
    Message_Gravity aGravity = Message::GravityFromString (TCollection_AsciiString (aNameAttribute->Get()).ToCString());
    // find alerts information, add corresponded alerts to the report
    for (TDF_ChildIterator anAlertLabelsIt (aGravityLabel); anAlertLabelsIt.More(); anAlertLabelsIt.Next())
    {
      TDF_Label anAlertLabel = anAlertLabelsIt.Value();
      if (anAlertLabel.IsNull())
        continue;

      importAlert (anAlertLabel, aGravity, theReport, anAlert);
    }
  }
  theReport->AddAlert (theGravity, anAlert, theParentAlert);
}

// =======================================================================
// function : exportAlertParameters
// purpose :
// =======================================================================
void XmlDrivers_MessageReportStorage::exportAlertParameters (const Handle(Message_Alert)& theAlert, const TDF_Label& theAlertLabel)
{
  // store elapsed time
  TDataStd_Real::Set (theAlertLabel, theAlert->ElapsedTime());

  TDataStd_AsciiString::Set (theAlertLabel, theAlert->GetName());
}

// =======================================================================
// function : importAlertParameters
// purpose :
// =======================================================================
Handle(Message_Alert) XmlDrivers_MessageReportStorage::importAlertParameters (const TDF_Label& aParametersLabel)
{
  Handle(TDF_Attribute) anAttribute;
  if (!aParametersLabel.FindAttribute (TDataStd_Name::GetID(), anAttribute))
    return Handle(Message_Alert)();

  Handle(TDataStd_Name) aNameAttribute = Handle(TDataStd_Name)::DownCast (anAttribute);
  if (aNameAttribute.IsNull())
    return Handle(Message_Alert)();

  // elapsed time
  Standard_Real anElapsedTime = -1;
  Handle(TDataStd_Real) aTimeAttribute;
  if (aParametersLabel.FindAttribute (TDataStd_Real::GetID(), aTimeAttribute))
    anElapsedTime = aTimeAttribute->Get();

  if (!aParametersLabel.FindAttribute (TDataStd_AsciiString::GetID(), anAttribute))
    return Handle(Message_Alert)();

  Handle(TDataStd_AsciiString) aValueAttribute = Handle(TDataStd_AsciiString)::DownCast (anAttribute);
  if (aValueAttribute.IsNull())
    return Handle(Message_Alert)();

  Handle(Message_Alert) anAlert = new Message_Alert (aValueAttribute->Get());
  anAlert->SetElapsedTime (anElapsedTime);
  return anAlert;
}

// Copyright (c) 1999-2014 OPEN CASCADE SAS
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


#include <Interface_Check.hxx>
#include <Interface_InterfaceModel.hxx>
#include <Transfer_Binder.hxx>
#include <Transfer_TransientProcess.hxx>
#include <XSSelect_SignTransferStatus.hxx>

IMPLEMENT_STANDARD_RTTIEXT(XSSelect_SignTransferStatus,IFSelect_Signature)

// si resultat avec type: a exploiter tout de suite !

// BinderStatus retourne une valeur :
// 0 Binder Null.   1 void  2 Warning seul  3 Fail seul
// 11 Resultat OK. 12 Resultat+Warning. 13 Resultat+Fail
// 20 Abnormal (Interrupted)

static Standard_Integer BinderStatus (const Handle(Transfer_Binder)& binder)
{
  Standard_Integer stat = 0;
  if (binder.IsNull())  return 0;
  Interface_CheckStatus cst = binder->Check()->Status();
  Transfer_StatusExec est = binder->StatusExec ();
  Standard_Boolean res = binder->HasResult();
  if (est == Transfer_StatusRun || est == Transfer_StatusLoop) return 20;
  if      (cst == Interface_CheckOK)      stat = (res ? 11 : 1);
  else if (cst == Interface_CheckWarning) stat = (res ? 12 : 2);
  else if (cst == Interface_CheckFail)    stat = (res ? 13 : 3);

  return stat;
}

Standard_CString  XSSelect_SignTransferStatus::Value
  (const Handle(Standard_Transient)& ent,
   const Handle(Interface_InterfaceModel)& /*model*/) const
{
  if (ent.IsNull()) return "";
  if (theTP.IsNull()) return "";

  Handle(Transfer_Binder) binder = theTP->Find(ent);

  Standard_Integer stat = BinderStatus (binder);

  if (stat <= 1) return "";
  if (stat == 2) return "Warning";
  if (stat == 3) return "Fail";
  if (stat == 20) return "Fail on run";

  theMes.Clear();
  if (stat > 10) {
//  Y a un resultat : donner son type
    Handle(Transfer_Binder) bnd = binder;
    Standard_Integer hasres = Standard_False;
    while (!bnd.IsNull()) {
      if (bnd->Status() != Transfer_StatusVoid) {
        theMes.AssignCat(hasres? "," : "Result:");
        theMes.AssignCat(bnd->ResultTypeName());
        hasres = Standard_True;
      }
      bnd = bnd->NextResult();
    }
    if (stat == 12) theMes.AssignCat("/Warning");
    if (stat == 13) theMes.AssignCat("/Fail");
  }
  return theMes.ToCString();
}

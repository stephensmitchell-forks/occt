// Created on: 2001-08-24
// Created by: Alexnder GRIGORIEV
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


#include <Message_Messenger.hxx>
#include <Standard_Type.hxx>
#include <TDataStd_NoteBook.hxx>
#include <TDF_Attribute.hxx>
#include <XmlMDataStd_NoteBookDriver.hxx>
#include <XmlObjMgt_Persistent.hxx>

IMPLEMENT_STANDARD_RTTIEXT(XmlMDataStd_NoteBookDriver,XmlMDF_ADriver)

//=======================================================================
//function : XmlMDataStd_NoteBookDriver
//purpose  : Constructor
//=======================================================================
XmlMDataStd_NoteBookDriver::XmlMDataStd_NoteBookDriver
                        (const Handle(Message_Messenger)& theMsgDriver)
      : XmlMDF_ADriver (theMsgDriver, NULL)
{}

//=======================================================================
//function : NewEmpty
//purpose  : 
//=======================================================================

 Handle(TDF_Attribute) XmlMDataStd_NoteBookDriver::NewEmpty() const
{
  return (new TDataStd_NoteBook());
}

//=======================================================================
//function : Paste
//purpose  : 
//=======================================================================

 Standard_Boolean XmlMDataStd_NoteBookDriver::Paste
                                        (const XmlObjMgt_Persistent&,
                                         const Handle(TDF_Attribute)&,
                                         XmlObjMgt_RRelocationTable& ) const
{
  return Standard_True;
}

//=======================================================================
//function : Paste
//purpose  : 
//=======================================================================

 void XmlMDataStd_NoteBookDriver::Paste (const Handle(TDF_Attribute)&,
                                         XmlObjMgt_Persistent&,
                                         XmlObjMgt_SRelocationTable&  ) const
{
}

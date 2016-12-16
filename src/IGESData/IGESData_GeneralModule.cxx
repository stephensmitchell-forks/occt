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


#include <IGESData.hxx>
#include <IGESData_ColorEntity.hxx>
#include <IGESData_DirChecker.hxx>
#include <IGESData_GeneralModule.hxx>
#include <IGESData_IGESEntity.hxx>
#include <IGESData_LabelDisplayEntity.hxx>
#include <IGESData_LevelListEntity.hxx>
#include <IGESData_LineFontEntity.hxx>
#include <IGESData_Protocol.hxx>
#include <IGESData_TransfEntity.hxx>
#include <IGESData_ViewKindEntity.hxx>
#include <Interface_Check.hxx>
#include <Interface_EntityIterator.hxx>
#include <Interface_GeneralLib.hxx>
#include <Interface_Macros.hxx>
//#include <Interface_ShareTool.hxx>
#include <Standard_Type.hxx>
#include <TCollection_HAsciiString.hxx>

#include <IGESDraw_ViewsVisible.hxx>
#include <IGESDraw_ViewsVisibleWithAttr.hxx>

IMPLEMENT_STANDARD_RTTIEXT(IGESData_GeneralModule,Interface_GeneralModule)

void IGESData_GeneralModule::FillSharedCase (const Standard_Integer, const Handle(Standard_Transient)& ent, Interface_EntityIterator& iter) const
{
  DeclareAndCast(IGESData_IGESEntity,anent,ent);
  if (anent.IsNull()) return;

//  .... Directory Part
  iter.AddItem (anent->DirFieldEntity (3));
  iter.AddItem (anent->DirFieldEntity (4));
  iter.AddItem (anent->DirFieldEntity (5));
  iter.AddItem (anent->DirFieldEntity (6));
  iter.AddItem (anent->DirFieldEntity (7));
  iter.AddItem (anent->DirFieldEntity (8));
  iter.AddItem (anent->DirFieldEntity (13));

//  .... Own Parameters
  anent->OwnShared (iter);

//  .... Properties
  Interface_EntityIterator assocs = anent->Properties();
  for (; assocs.More(); assocs.Next())  iter.AddItem (assocs.Value());
}

void  IGESData_GeneralModule::ListImpliedCase
  (const Standard_Integer CN, const Handle(Standard_Transient)& ent,
   Interface_EntityIterator& iter) const
{
  DeclareAndCast(IGESData_IGESEntity,aent,ent);
  if (aent.IsNull()) return;
  switch (CN ) {
    case 13 : {
      DeclareAndCast(IGESDraw_ViewsVisible,anent,aent);
      if (anent.IsNull()) break;
      const Standard_Integer nb  = anent->NbDisplayedEntities();
      for (Standard_Integer i = 1; i <= nb; i++)
        iter.GetOneItem(anent->DisplayedEntity(i));
      break;
    }
    case 14 : {
      DeclareAndCast(IGESDraw_ViewsVisibleWithAttr,anent,aent);
      if (anent.IsNull()) break;
      const Standard_Integer nb  = anent->NbDisplayedEntities();
      for (Standard_Integer i = 1; i <= nb; i++)
        iter.GetOneItem(anent->DisplayedEntity(i));
      break;
    }
    default : break;
  }
  Interface_EntityIterator assocs = aent->Associativities();
  for (; assocs.More(); assocs.Next())  iter.AddItem (assocs.Value());
}


void IGESData_GeneralModule::CheckCase
  (const Standard_Integer, const Handle(Standard_Transient)& ent,
   const Interface_ShareTool& shares, Handle(Interface_Check)& ach) const
{
  DeclareAndCast(IGESData_IGESEntity,anent,ent);

  //szv_c1:anent->DirChecker().Check(ach,anent); should be called from OwnCheck

  anent->OwnCheck (shares,ach);
}


    Handle(TCollection_HAsciiString)  IGESData_GeneralModule::Name
  (const Standard_Integer , const Handle(Standard_Transient)& ent,
   const Interface_ShareTool& ) const
{
  Handle(TCollection_HAsciiString) name;
  DeclareAndCast(IGESData_IGESEntity,anent,ent);
  if (anent.IsNull()) return name;
  name = anent->NameValue ();
  return name;
}

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
#include <Interface_ShareTool.hxx>
#include <Standard_Transient.hxx>
#include <Standard_Type.hxx>
#include <TCollection_HAsciiString.hxx>

IMPLEMENT_STANDARD_RTTIEXT(IGESData_GeneralModule,Interface_GeneralModule)

void  IGESData_GeneralModule::FillSharedCase
  (const Standard_Integer CN, const Handle(Standard_Transient)& ent,
   Interface_EntityIterator& iter) const 
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
  OwnSharedCase (CN,anent,iter);

//  .... Properties
  Interface_EntityIterator assocs = anent->Properties();
  for (; assocs.More(); assocs.Next())  iter.AddItem (assocs.Value());
}

    void  IGESData_GeneralModule::ListImpliedCase
  (const Standard_Integer CN, const Handle(Standard_Transient)& ent,
   Interface_EntityIterator& iter) const
{
  DeclareAndCast(IGESData_IGESEntity,anent,ent);
  if (anent.IsNull()) return;
  OwnImpliedCase (CN,anent,iter);
  Interface_EntityIterator assocs = anent->Associativities();
  for (; assocs.More(); assocs.Next())  iter.AddItem (assocs.Value());
}


    void  IGESData_GeneralModule::OwnImpliedCase
  (const Standard_Integer , const Handle(IGESData_IGESEntity)& ,
   Interface_EntityIterator& ) const 
      {  }  // par defaut, rien  (redefinissable)



    void  IGESData_GeneralModule::CheckCase
  (const Standard_Integer CN, const Handle(Standard_Transient)& ent,
   const Interface_ShareTool& shares, Handle(Interface_Check)& ach) const
{
  DeclareAndCast(IGESData_IGESEntity,anent,ent);

  IGESData_DirChecker dc = DirChecker(CN,anent);
  dc.Check(ach,anent);
  OwnCheckCase (CN,anent,shares,ach);
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

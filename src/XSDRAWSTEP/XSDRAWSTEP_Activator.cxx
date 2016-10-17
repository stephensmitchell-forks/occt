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


#include <Interface_Macros.hxx>
#include <Standard_ErrorHandler.hxx>
#include <Standard_Failure.hxx>
#include <Standard_Type.hxx>
#include <StepData_Plex.hxx>
#include <StepData_Simple.hxx>
#include <StepData_UndefinedEntity.hxx>
#include <IFSelect_WorkSession.hxx>
#include <XSDRAWSTEP_Activator.hxx>
#include <XSDRAW_SessionPilot.hxx>

IMPLEMENT_STANDARD_RTTIEXT(XSDRAWSTEP_Activator,XSDRAW_Activator)

XSDRAWSTEP_Activator::XSDRAWSTEP_Activator ()
{
  static int initActivator = 0;
  if (initActivator) return;  initActivator = 1;

  Add    ( 1,"stepschema");
}


Interface_ReturnStatus  XSDRAWSTEP_Activator::Do
  (const Standard_Integer number,
   const Handle(XSDRAW_SessionPilot)& pilot)
{
  Standard_Integer argc = pilot->NbWords();
  const Standard_CString arg1 = pilot->Word(1).ToCString();
  const Standard_CString arg2 = pilot->Word(2).ToCString();

  switch (number) {

    case  1 : {   //        ****    StepSchema
      if (argc < 2) {
        cout<<"Identify an entity"<<endl;
        return Interface_RetError;
      }
      Standard_Integer num = pilot->Number(arg1);
      if (num <= 0) {
        cout<<"Not an entity : "<<arg2<<endl;
        return Interface_RetError;
      }
      Handle(Standard_Transient) ent = pilot->Session()->StartingEntity(num);
      DeclareAndCast(StepData_UndefinedEntity,und,ent);
      if (!und.IsNull()) {
	cout<<"Entity "<<arg2<<" : No Binding known"<<endl;
	return Interface_RetVoid;
      }
      DeclareAndCast(StepData_Simple,sim,ent);
      if (!sim.IsNull()) {
	cout<<"Entity "<<arg2<<" : Late Binding"<<endl;
	cout<<"Simple Type : "<<sim->StepType()<<endl;
	return Interface_RetVoid;
      }
      DeclareAndCast(StepData_Plex,plx,ent);
      if (!plx.IsNull()) {
	cout<<"Entity "<<arg2<<" : Late Binding"<<endl;
	cout<<"Complex Type"<<endl;
      }
//       reste Early Binding
      cout<<"Entity "<<arg2<<" : Early Binding"<<endl;
      cout<<"CDL Type : "<<ent->DynamicType()->Name()<<endl;
      return Interface_RetVoid;
    }

    default : break;
  }
  return Interface_RetVoid;

}


Standard_CString  XSDRAWSTEP_Activator::Help
  (const Standard_Integer) const
{
  return "";
}

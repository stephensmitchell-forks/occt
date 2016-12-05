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


#include <IGESBasic_AssocGroupType.hxx>
#include <IGESBasic_ExternalReferenceFile.hxx>
#include <IGESBasic_ExternalRefFile.hxx>
#include <IGESBasic_ExternalRefFileIndex.hxx>
#include <IGESBasic_ExternalRefFileName.hxx>
#include <IGESBasic_ExternalRefLibName.hxx>
#include <IGESBasic_ExternalRefName.hxx>
#include <IGESBasic_GeneralModule.hxx>
#include <IGESBasic_Group.hxx>
#include <IGESBasic_GroupWithoutBackP.hxx>
#include <IGESBasic_Hierarchy.hxx>
#include <IGESBasic_Name.hxx>
#include <IGESBasic_OrderedGroup.hxx>
#include <IGESBasic_OrderedGroupWithoutBackP.hxx>
#include <IGESBasic_SingleParent.hxx>
#include <IGESBasic_SingularSubfigure.hxx>
#include <IGESBasic_SubfigureDef.hxx>
#include <IGESBasic_ToolAssocGroupType.hxx>
#include <IGESBasic_ToolExternalReferenceFile.hxx>
#include <IGESBasic_ToolExternalRefFile.hxx>
#include <IGESBasic_ToolExternalRefFileIndex.hxx>
#include <IGESBasic_ToolExternalRefFileName.hxx>
#include <IGESBasic_ToolExternalRefLibName.hxx>
#include <IGESBasic_ToolExternalRefName.hxx>
#include <IGESBasic_ToolGroup.hxx>
#include <IGESBasic_ToolGroupWithoutBackP.hxx>
#include <IGESBasic_ToolHierarchy.hxx>
#include <IGESBasic_ToolName.hxx>
#include <IGESBasic_ToolOrderedGroup.hxx>
#include <IGESBasic_ToolOrderedGroupWithoutBackP.hxx>
#include <IGESBasic_ToolSingleParent.hxx>
#include <IGESBasic_ToolSingularSubfigure.hxx>
#include <IGESBasic_ToolSubfigureDef.hxx>
#include <IGESData_DirChecker.hxx>
#include <IGESData_IGESEntity.hxx>
#include <Interface_Category.hxx>
#include <Interface_Check.hxx>
#include <Interface_EntityIterator.hxx>
#include <Interface_Macros.hxx>
#include <Interface_ShareTool.hxx>

IMPLEMENT_STANDARD_RTTIEXT(IGESBasic_GeneralModule,IGESData_GeneralModule)


    void  IGESBasic_GeneralModule::OwnSharedCase
  (const Standard_Integer CN, const Handle(IGESData_IGESEntity)& ent,
   Interface_EntityIterator& iter) const 
{
  switch (CN) {
    case  3 : {
      DeclareAndCast(IGESBasic_ExternalRefFileIndex,anent,ent);
      if (anent.IsNull()) return;
      IGESBasic_ToolExternalRefFileIndex tool;
      tool.OwnShared(anent,iter);
    }
      break;
    case  8 : {
      DeclareAndCast(IGESBasic_Group,anent,ent);
      if (anent.IsNull()) return;
      IGESBasic_ToolGroup tool;
      tool.OwnShared(anent,iter);
    }
      break;
    case  9 : {
      DeclareAndCast(IGESBasic_GroupWithoutBackP,anent,ent);
      if (anent.IsNull()) return;
      IGESBasic_ToolGroupWithoutBackP tool;
      tool.OwnShared(anent,iter);
    }
      break;
    case 12 : {
      DeclareAndCast(IGESBasic_OrderedGroup,anent,ent);
      if (anent.IsNull()) return;
      IGESBasic_ToolOrderedGroup tool;
      tool.OwnShared(anent,iter);
    }
      break;
    case 13 : {
      DeclareAndCast(IGESBasic_OrderedGroupWithoutBackP,anent,ent);
      if (anent.IsNull()) return;
      IGESBasic_ToolOrderedGroupWithoutBackP tool;
      tool.OwnShared(anent,iter);
    }
      break;
    case 14 : {
      DeclareAndCast(IGESBasic_SingleParent,anent,ent);
      if (anent.IsNull()) return;
      IGESBasic_ToolSingleParent tool;
      tool.OwnShared(anent,iter);
    }
      break;
    case 15 : {
      DeclareAndCast(IGESBasic_SingularSubfigure,anent,ent);
      if (anent.IsNull()) return;
      IGESBasic_ToolSingularSubfigure tool;
      tool.OwnShared(anent,iter);
    }
      break;
    case 16 : {
      DeclareAndCast(IGESBasic_SubfigureDef,anent,ent);
      if (anent.IsNull()) return;
      IGESBasic_ToolSubfigureDef tool;
      tool.OwnShared(anent,iter);
    }
      break;
    default : break;
  }
}


    IGESData_DirChecker  IGESBasic_GeneralModule::DirChecker
  (const Standard_Integer CN, const Handle(IGESData_IGESEntity)& ent) const 
{
  switch (CN) {
    case  1 : {
      DeclareAndCast(IGESBasic_AssocGroupType,anent,ent);
      if (anent.IsNull()) break;
      IGESBasic_ToolAssocGroupType tool;
      return tool.DirChecker(anent);
    }
    case  2 : {
      DeclareAndCast(IGESBasic_ExternalRefFile,anent,ent);
      if (anent.IsNull()) break;
      IGESBasic_ToolExternalRefFile tool;
      return tool.DirChecker(anent);
    }
    case  3 : {
      DeclareAndCast(IGESBasic_ExternalRefFileIndex,anent,ent);
      if (anent.IsNull()) break;
      IGESBasic_ToolExternalRefFileIndex tool;
      return tool.DirChecker(anent);
    }
    case  4 : {
      DeclareAndCast(IGESBasic_ExternalRefFileName,anent,ent);
      if (anent.IsNull()) break;
      IGESBasic_ToolExternalRefFileName tool;
      return tool.DirChecker(anent);
    }
    case  5 : {
      DeclareAndCast(IGESBasic_ExternalRefLibName,anent,ent);
      if (anent.IsNull()) break;
      IGESBasic_ToolExternalRefLibName tool;
      return tool.DirChecker(anent);
    }
    case  6 : {
      DeclareAndCast(IGESBasic_ExternalRefName,anent,ent);
      if (anent.IsNull()) break;
      IGESBasic_ToolExternalRefName tool;
      return tool.DirChecker(anent);
    }
    case  7 : {
      DeclareAndCast(IGESBasic_ExternalReferenceFile,anent,ent);
      if (anent.IsNull()) break;
      IGESBasic_ToolExternalReferenceFile tool;
      return tool.DirChecker(anent);
    }
    case  8 : {
      DeclareAndCast(IGESBasic_Group,anent,ent);
      if (anent.IsNull()) break;
      IGESBasic_ToolGroup tool;
      return tool.DirChecker(anent);
    }
    case  9 : {
      DeclareAndCast(IGESBasic_GroupWithoutBackP,anent,ent);
      if (anent.IsNull()) break;
      IGESBasic_ToolGroupWithoutBackP tool;
      return tool.DirChecker(anent);
    }
    case 10 : {
      DeclareAndCast(IGESBasic_Hierarchy,anent,ent);
      if (anent.IsNull()) break;
      IGESBasic_ToolHierarchy tool;
      return tool.DirChecker(anent);
    }
    case 11 : {
      DeclareAndCast(IGESBasic_Name,anent,ent);
      if (anent.IsNull()) break;
      IGESBasic_ToolName tool;
      return tool.DirChecker(anent);
    }
    case 12 : {
      DeclareAndCast(IGESBasic_OrderedGroup,anent,ent);
      if (anent.IsNull()) break;
      IGESBasic_ToolOrderedGroup tool;
      return tool.DirChecker(anent);
    }
    case 13 : {
      DeclareAndCast(IGESBasic_OrderedGroupWithoutBackP,anent,ent);
      if (anent.IsNull()) break;
      IGESBasic_ToolOrderedGroupWithoutBackP tool;
      return tool.DirChecker(anent);
    }
    case 14 : {
      DeclareAndCast(IGESBasic_SingleParent,anent,ent);
      if (anent.IsNull()) break;
      IGESBasic_ToolSingleParent tool;
      return tool.DirChecker(anent);
    }
    case 15 : {
      DeclareAndCast(IGESBasic_SingularSubfigure,anent,ent);
      if (anent.IsNull()) break;
      IGESBasic_ToolSingularSubfigure tool;
      return tool.DirChecker(anent);
    }
    case 16 : {
      DeclareAndCast(IGESBasic_SubfigureDef,anent,ent);
      if (anent.IsNull()) break;
      IGESBasic_ToolSubfigureDef tool;
      return tool.DirChecker(anent);
    }
    default : break;
  }
  return IGESData_DirChecker();    // by default, no specific criterium
}


    void  IGESBasic_GeneralModule::OwnCheckCase
  (const Standard_Integer CN, const Handle(IGESData_IGESEntity)& ent,
   const Interface_ShareTool& shares, Handle(Interface_Check)& ach) const 
{
  switch (CN) {
    case  1 : {
      DeclareAndCast(IGESBasic_AssocGroupType,anent,ent);
      if (anent.IsNull()) return;
      IGESBasic_ToolAssocGroupType tool;
      tool.OwnCheck(anent,shares,ach);
    }
      break;
    case  4 : {
      DeclareAndCast(IGESBasic_ExternalRefFileName,anent,ent);
      if (anent.IsNull()) return;
      IGESBasic_ToolExternalRefFileName tool;
      tool.OwnCheck(anent,shares,ach);
    }
      break;
    case  7 : {
      DeclareAndCast(IGESBasic_ExternalReferenceFile,anent,ent);
      if (anent.IsNull()) return;
      IGESBasic_ToolExternalReferenceFile tool;
      tool.OwnCheck(anent,shares,ach);
    }
      break;
    case  8 : {
      DeclareAndCast(IGESBasic_Group,anent,ent);
      if (anent.IsNull()) return;
      IGESBasic_ToolGroup tool;
      tool.OwnCheck(anent,shares,ach);
    }
      break;
    case  9 : {
      DeclareAndCast(IGESBasic_GroupWithoutBackP,anent,ent);
      if (anent.IsNull()) return;
      IGESBasic_ToolGroupWithoutBackP tool;
      tool.OwnCheck(anent,shares,ach);
    }
      break;
    case 10 : {
      DeclareAndCast(IGESBasic_Hierarchy,anent,ent);
      if (anent.IsNull()) return;
      IGESBasic_ToolHierarchy tool;
      tool.OwnCheck(anent,shares,ach);
    }
      break;
    case 11 : {
      DeclareAndCast(IGESBasic_Name,anent,ent);
      if (anent.IsNull()) return;
      IGESBasic_ToolName tool;
      tool.OwnCheck(anent,shares,ach);
    }
      break;
    case 12 : {
      DeclareAndCast(IGESBasic_OrderedGroup,anent,ent);
      if (anent.IsNull()) return;
      IGESBasic_ToolOrderedGroup tool;
      tool.OwnCheck(anent,shares,ach);
    }
      break;
    case 13 : {
      DeclareAndCast(IGESBasic_OrderedGroupWithoutBackP,anent,ent);
      if (anent.IsNull()) return;
      IGESBasic_ToolOrderedGroupWithoutBackP tool;
      tool.OwnCheck(anent,shares,ach);
    }
      break;
    case 14 : {
      DeclareAndCast(IGESBasic_SingleParent,anent,ent);
      if (anent.IsNull()) return;
      IGESBasic_ToolSingleParent tool;
      tool.OwnCheck(anent,shares,ach);
    }
      break;
    case 16 : {
      DeclareAndCast(IGESBasic_SubfigureDef,anent,ent);
      if (anent.IsNull()) return;
      IGESBasic_ToolSubfigureDef tool;
      tool.OwnCheck(anent,shares,ach);
    }
      break;
    default : break;
  }
}


    Standard_Boolean  IGESBasic_GeneralModule::NewVoid
  (const Standard_Integer CN, Handle(Standard_Transient)& ent) const 
{
  switch (CN) {
    case  1 : ent = new IGESBasic_AssocGroupType;	break;
    case  2 : ent = new IGESBasic_ExternalRefFile;	break;
    case  3 : ent = new IGESBasic_ExternalRefFileIndex;	break;
    case  4 : ent = new IGESBasic_ExternalRefFileName;	break;
    case  5 : ent = new IGESBasic_ExternalRefLibName;	break;
    case  6 : ent = new IGESBasic_ExternalRefName;	break;
    case  7 : ent = new IGESBasic_ExternalReferenceFile;	break;
    case  8 : ent = new IGESBasic_Group;		break;
    case  9 : ent = new IGESBasic_GroupWithoutBackP;	break;
    case 10 : ent = new IGESBasic_Hierarchy;		break;
    case 11 : ent = new IGESBasic_Name; 		break;
    case 12 : ent = new IGESBasic_OrderedGroup; 	break;
    case 13 : ent = new IGESBasic_OrderedGroupWithoutBackP;	break;
    case 14 : ent = new IGESBasic_SingleParent; 	break;
    case 15 : ent = new IGESBasic_SingularSubfigure;	break;
    case 16 : ent = new IGESBasic_SubfigureDef; 	break;
    default : return Standard_False;    // by default, Failure on Recognize
  }
  return Standard_True;
}

    Standard_Integer  IGESBasic_GeneralModule::CategoryNumber
  (const Standard_Integer CN, const Handle(Standard_Transient)& ,
   const Interface_ShareTool& ) const
{
  if (CN == 10 || CN == 11) return Interface_Category::Number("Auxiliary");
  if (CN >=  2 && CN <=  6) return Interface_Category::Number("Description");
  return Interface_Category::Number("Structure");
}

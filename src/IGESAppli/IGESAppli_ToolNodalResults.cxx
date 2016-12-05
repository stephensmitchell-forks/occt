// Created by: CKY / Contract Toubro-Larsen
// Copyright (c) 1993-1999 Matra Datavision
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

//--------------------------------------------------------------------
//--------------------------------------------------------------------

#include <IGESAppli_HArray1OfNode.hxx>
#include <IGESAppli_NodalResults.hxx>
#include <IGESAppli_Node.hxx>
#include <IGESAppli_ToolNodalResults.hxx>
#include <IGESData_DirChecker.hxx>
#include <IGESData_Dump.hxx>
#include <IGESData_IGESDumper.hxx>
#include <IGESData_IGESReaderData.hxx>
#include <IGESData_IGESWriter.hxx>
#include <IGESData_ParamCursor.hxx>
#include <IGESData_ParamReader.hxx>
#include <IGESDimen_GeneralNote.hxx>
#include <Interface_Check.hxx>
#include <Interface_EntityIterator.hxx>
#include <Interface_Macros.hxx>
#include <Interface_ShareTool.hxx>
#include <Message_Messenger.hxx>
#include <Standard_DomainError.hxx>
#include <TColStd_HArray1OfInteger.hxx>
#include <TColStd_HArray1OfReal.hxx>
#include <TColStd_HArray2OfReal.hxx>


void  IGESAppli_ToolNodalResults::ReadOwnParams
  (const Handle(IGESAppli_NodalResults)& ent,
   const Handle(IGESData_IGESReaderData)& IR, IGESData_ParamReader& PR) const
{
  Standard_Integer tempSubCaseNum = 0;
  Standard_Real tempTime;
  Standard_Integer nbval = 0;
  Standard_Integer nbnodes = 0;
  //Standard_Boolean st; //szv#4:S4163:12Mar99 not needed
  Handle(IGESDimen_GeneralNote) tempNote;
  Handle(TColStd_HArray2OfReal) tempData ;
  Handle(IGESAppli_HArray1OfNode) tempNodes ;
  Handle(TColStd_HArray1OfInteger) tempNodeIdentifiers ;

  //szv#4:S4163:12Mar99 `st=` not needed
  PR.ReadEntity(IR,PR.Current(),"General Note describing the analysis case",
		STANDARD_TYPE(IGESDimen_GeneralNote), tempNote);
  PR.ReadInteger (PR.Current(),"Subcase number",tempSubCaseNum);
  PR.ReadReal(PR.Current(),"Analysis time used",tempTime);
  Standard_Boolean tempFlag = PR.ReadInteger(PR.Current(),"No. of values",nbval);
  //szv#4:S4163:12Mar99 moved in if
  if (PR.ReadInteger(PR.Current(),"No. of nodes",nbnodes)) {
    tempData  = new TColStd_HArray2OfReal(1,nbnodes,1,nbval);
    tempNodes = new IGESAppli_HArray1OfNode(1,nbnodes);
    tempNodeIdentifiers = new TColStd_HArray1OfInteger(1,nbnodes);
    for (Standard_Integer i = 1; i <= nbnodes; i ++) {
      Standard_Integer aitem;
      //Check  whether nbval properly read or not.
      Handle(IGESAppli_Node) aNode ;

      if (PR.ReadInteger(PR.Current(),"Node no. identifier",aitem))
	tempNodeIdentifiers->SetValue(i,aitem);
      if (PR.ReadEntity(IR,PR.Current(),"FEM Node", STANDARD_TYPE(IGESAppli_Node), aNode))
	tempNodes->SetValue(i,aNode);
      if (tempFlag)
	//Check  whether nbval properly read or not.
	for (Standard_Integer j = 1; j <= nbval; j ++) {
	  Standard_Real aval;
	  if (PR.ReadReal(PR.Current(),"Value",aval))
	    tempData->SetValue(i,j,aval);
	}
    }
  }
  DirChecker(ent).CheckTypeAndForm(PR.CCheck(),ent);
  ent->Init(tempNote,tempSubCaseNum,tempTime,tempNodeIdentifiers,tempNodes,tempData);
}

void  IGESAppli_ToolNodalResults::WriteOwnParams
  (const Handle(IGESAppli_NodalResults)& ent, IGESData_IGESWriter& IW) const
{
  Standard_Integer nbnodes = ent->NbNodes();
  Standard_Integer nbdata  = ent->NbData ();
  IW.Send(ent->Note());
  IW.Send(ent->SubCaseNumber());
  IW.Send(ent->Time());
  IW.Send(nbdata);
  IW.Send(nbnodes);
  for (Standard_Integer i = 1; i <= nbnodes; i++)
    {
      IW.Send(ent->NodeIdentifier(i));
      IW.Send(ent->Node(i));
      for (Standard_Integer j = 1; j <= nbdata; j++)
	IW.Send(ent->Data(i,j));
    }
}

IGESData_DirChecker  IGESAppli_ToolNodalResults::DirChecker
  (const Handle(IGESAppli_NodalResults)& /* ent */ ) const
{
  IGESData_DirChecker DC(146,0,34);  // Type = 146 Form No. = 0 to 34
  DC.Structure(IGESData_DefVoid);
  DC.LineFont(IGESData_DefVoid);
  DC.LineWeight(IGESData_DefVoid);
  DC.Color(IGESData_DefAny);
  DC.BlankStatusIgnored();
  DC.UseFlagRequired(03);
  DC.HierarchyStatusIgnored();
  return DC;
}

void  IGESAppli_ToolNodalResults::OwnDump
  (const Handle(IGESAppli_NodalResults)& ent, const IGESData_IGESDumper& dumper,
   const Handle(Message_Messenger)& S, const Standard_Integer level) const
{
//  Standard_Integer nbnodes = ent->NbNodes();
//  Standard_Integer nbdata  = ent->NbData ();
  S << "IGESAppli_NodalResults" << endl;

  S << "General Note : ";
  dumper.Dump(ent->Note(),S, (level <= 4) ? 0 : 1);
  S << endl;
  S << "Analysis subcase number : " << ent->SubCaseNumber() << "  ";
  S << "Time used : " << ent->Time() << endl;
  S << "No. of nodes : " << ent->NbNodes() << "  ";
  S << "No. of values for a node : " << ent->NbData() << endl;
  S << "Node Identifiers : " << endl;
  S << "Nodes : " << endl;
  S << "Data : ";  if (level < 6) S << " [ask level > 5]";
//  IGESData_DumpRectVals(S ,-level,1, ent->NbData(),ent->Data);
  S << endl;
  if (level > 4)
    {
      for (Standard_Integer i=1; i <= ent->NbNodes(); i++)
	{
          S << "[" << i << "]: ";
          S << "NodeIdentifier : " << ent->NodeIdentifier(i) << "  ";
          S << "Node : ";
          dumper.Dump (ent->Node(i),S, 1);
          S << endl;
	  if (level < 6) continue;
          S << "Data : [ ";
          for (Standard_Integer j = 1; j <= ent->NbData(); j ++)
              S << "  " << ent->Data(i,j);
	  S << " ]" << endl;
	}
    }
}

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

#include <gp_XYZ.hxx>
#include <IGESAppli_NodalDisplAndRot.hxx>
#include <IGESAppli_Node.hxx>
#include <IGESBasic_HArray1OfHArray1OfXYZ.hxx>
#include <IGESDimen_GeneralNote.hxx>
#include <Standard_DimensionMismatch.hxx>
#include <Standard_OutOfRange.hxx>
#include <Interface_EntityIterator.hxx>
#include <TColgp_HArray1OfXYZ.hxx>
#include <IGESFile_Reader.hxx>
#include <IGESData_IGESWriter.hxx>
#include <Message_Messenger.hxx>
#include <IGESData_DirChecker.hxx>
#include <IGESData_IGESDumper.hxx>
#include <IGESData_Dump.hxx>

IMPLEMENT_STANDARD_RTTIEXT(IGESAppli_NodalDisplAndRot,IGESData_IGESEntity)

Standard_Integer IGESAppli_NodalDisplAndRot::NbCases () const
{
  return myNotes->Length();
}

Standard_Integer IGESAppli_NodalDisplAndRot::NbNodes () const
{
  return myNodes->Length();
}

const Handle(IGESDimen_GeneralNote) & IGESAppli_NodalDisplAndRot::Note (const Standard_Integer Index) const
{
  return myNotes->Value(Index);
}

Standard_Integer IGESAppli_NodalDisplAndRot::NodeIdentifier (const Standard_Integer Index) const
{
  return myNodeIdentifiers->Value(Index);
}

const Handle(IGESAppli_Node) & IGESAppli_NodalDisplAndRot::Node (const Standard_Integer Index) const
{
  return myNodes->Value(Index);
}

const gp_XYZ & IGESAppli_NodalDisplAndRot::TranslationParameter (const Standard_Integer NodeNum, const Standard_Integer CaseNum) const
{
  return myTransParam->Value(NodeNum)->Value(CaseNum);
}

const gp_XYZ & IGESAppli_NodalDisplAndRot::RotationalParameter (const Standard_Integer NodeNum, const Standard_Integer CaseNum) const
{
  return myRotParam->Value(NodeNum)->Value(CaseNum);
}

void IGESAppli_NodalDisplAndRot::OwnRead (IGESFile_Reader &theReader)
{
  Standard_Integer nbcases = 0;
  theReader.ReadInteger(nbcases,"No. of analysis cases");
  if (nbcases > 0)
  {
    myNotes = new IGESDimen_HArray1OfGeneralNote(1,nbcases);
    for (Standard_Integer i=1; i <= nbcases; i++)
      theReader.ReadPointer(myNotes->ChangeValue(i),"General Note");
  }

  Standard_Integer nbnodes = 0;
  theReader.ReadInteger(nbnodes,"No. of nodes");
  if (nbnodes > 0)
  {
    myNodeIdentifiers = new TColStd_HArray1OfInteger(1,nbnodes);
    myNodes = new IGESAppli_HArray1OfNode(1,nbnodes);
    myTransParam = new IGESBasic_HArray1OfHArray1OfXYZ(1, nbnodes);
    myRotParam = new IGESBasic_HArray1OfHArray1OfXYZ(1, nbnodes);

    for (Standard_Integer j=1; j <=nbnodes; j++)
    {
      theReader.ReadInteger(myNodeIdentifiers->ChangeValue(j),"Node number");
      theReader.ReadPointer(myNodes->ChangeValue(j),"Node Directory Entry");

      if (nbcases > 0)
      {
        Handle(TColgp_HArray1OfXYZ) arr1 = new TColgp_HArray1OfXYZ(1,nbcases);
        Handle(TColgp_HArray1OfXYZ) arr2 = new TColgp_HArray1OfXYZ(1,nbcases);
        for (Standard_Integer k = 1; k <= nbcases; k++)
        {
          theReader.ReadXYZ(arr1->ChangeValue(k),"Translation XYZ");
          theReader.ReadXYZ(arr2->ChangeValue(k),"Rotational XYZ");
        }
        myTransParam->SetValue(j,arr1);
        myRotParam->SetValue(j,arr2);
      }
    }
  }
}

void IGESAppli_NodalDisplAndRot::OwnWrite (IGESData_IGESWriter &IW) const
{
  const Standard_Integer nbcases = myNotes->Length();
  const Standard_Integer nbnodes = myNodes->Length();

  IW.Send(nbcases);
  for (Standard_Integer i = 1; i <= nbcases; i ++)
    IW.Send(myNotes->Value(i));
  IW.Send(nbnodes);
  for (Standard_Integer j = 1; j <= nbnodes; j ++)
  {
    IW.Send(myNodeIdentifiers->Value(j));
    IW.Send(myNodes->Value(j));
    for (Standard_Integer k = 1; k <= nbcases; k ++)
	{
      IW.Send(TranslationParameter(j,k).X());
      IW.Send(TranslationParameter(j,k).Y());
      IW.Send(TranslationParameter(j,k).Z());
      IW.Send(RotationalParameter(j,k).X());
      IW.Send(RotationalParameter(j,k).Y());
      IW.Send(RotationalParameter(j,k).Z());
    }
  }
}

void IGESAppli_NodalDisplAndRot::OwnShared(Interface_EntityIterator &theIter) const
{
  const Standard_Integer nbcases = myNotes->Length();
  const Standard_Integer nbnodes = myNodes->Length();

  for (Standard_Integer i = 1; i <= nbcases; i ++)
    theIter.GetOneItem(myNotes->Value(i));
  for (Standard_Integer j = 1; j <= nbnodes; j ++)
    theIter.GetOneItem(myNodes->Value(j));
}

IGESData_DirChecker IGESAppli_NodalDisplAndRot::DirChecker () const
{
  IGESData_DirChecker DC(138,0);  //Form no = 0 & Type = 138
  DC.Structure(IGESData_DefVoid);
  DC.GraphicsIgnored();
  DC.HierarchyStatusIgnored();
  return DC;
}

void IGESAppli_NodalDisplAndRot::OwnDump (const IGESData_IGESDumper &dumper, const Handle(Message_Messenger) &S, const Standard_Integer level) const
{
  S << "IGESAppli_NodalDisplAndRot" << endl;

  const Standard_Integer nbcases = myNotes->Length();
  const Standard_Integer nbnodes = myNodes->Length();

  S << "No. of analysis cases : " << nbcases << endl;
  S << "General Notes : ";
  IGESData_DumpEntities(S,dumper,level,1,nbcases,Note);
  S << endl;

  switch (level)
  {
    case 4:
      S << "Nodes : ";
      IGESData_DumpEntities(S,dumper,-level,1,nbnodes,Node);
      S << "NodeIdentifiers : ";
      IGESData_DumpVals(S,-level,1,nbnodes,NodeIdentifier);
      S << "Translation Parameters : ";
      S << "Rotational Parameters : ";
      S << "  TO BE DONE" << endl;
      break;
    case 5:
    case 6: {
      for (Standard_Integer i = 1; i <= nbcases; i ++)
      {
        S << "[" << i << "]: ";
        dumper.Dump (Note(i),S, 1);
        S << endl;
      }
      S << "Nodes : " << endl;
      S << "Node Identifiers : "  << endl;
      S << "Translation Parameters : " << endl;
      S << "Rotational Parameters : Count = " << nbcases;
      S << endl;
      for (Standard_Integer j = 1; j <= nbnodes; j ++)
      {
        S << "[" << j << "]:  -  NodeIdentifier : "
          << NodeIdentifier(j) << "  -  Node :"<<endl; ;
        S << "Node : ";
        dumper.Dump (Node(j),S, 1);
        S << "  -  Parameters : " << endl;
        for (Standard_Integer k = 1; k <= nbcases; k ++)
        {
          S << " [" << k << "]: "  << endl;
          S << "Translational Parameter : X="
            << TranslationParameter(j,k).X() << ", Y="
            << TranslationParameter(j,k).Y() << ", Z="
            << TranslationParameter(j,k).Z() << endl;
          S << "Rotational Parameter : X="
            << RotationalParameter(j,k).X() << ", Y="
            << RotationalParameter(j,k).Y() << ", Z="
            << RotationalParameter(j,k).Z();
	      if (k == nbcases) S << "] ";
	      S << endl;
        }
      }
    }
    break;
    default: break;
  }
  S << endl;
}

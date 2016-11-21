// Created on: 2015-05-14
// Created by: data exchange team
// Copyright (c) 2000-2015 OPEN CASCADE SAS
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

#include <XCAFDoc_Editor.hxx>
#include <XCAFDoc.hxx>

#include <BRep_Builder.hxx>
#include <TopoDS.hxx>

#include <Quantity_Color.hxx>
#include <TDF_AttributeIterator.hxx>
#include <TDF_ChildIterator.hxx>
#include <TDF_Label.hxx>
#include <TDF_LabelSequence.hxx>
#include <TDF_RelocationTable.hxx>
#include <TDataStd_Name.hxx>
#include <TDataStd_UAttribute.hxx>
#include <TDataStd_TreeNode.hxx>
#include <TDocStd_Document.hxx>
#include <TopLoc_Location.hxx>
#include <TopoDS_Shape.hxx>

#include <XCAFDoc_ColorTool.hxx>
#include <XCAFDoc_DocumentTool.hxx>
#include <XCAFDoc_GraphNode.hxx>
#include <XCAFDoc_LayerTool.hxx>
#include <XCAFDoc_MaterialTool.hxx>
#include <XCAFDoc_ShapeMapTool.hxx>
#include <XCAFDoc_ShapeTool.hxx>
#include <XCAFDoc_Location.hxx>
#include <TNaming_NamedShape.hxx>

//=======================================================================
//function : Expand
//purpose  : Convert Shape(compound) to assambly
//=======================================================================

Standard_Boolean XCAFDoc_Editor::Expand (const TDF_Label& Doc, const TDF_Label& Shape,
  const Standard_Boolean recursively)
{
  if(Doc.IsNull() || Shape.IsNull())
    return Standard_False;

  Handle(XCAFDoc_ColorTool) aColorTool = XCAFDoc_DocumentTool::ColorTool(Doc);
  Handle(XCAFDoc_LayerTool) aLayerTool = XCAFDoc_DocumentTool::LayerTool(Doc);
  Handle(XCAFDoc_ShapeTool) aShapeTool = XCAFDoc_DocumentTool::ShapeTool(Doc);

  TopoDS_Shape aS = aShapeTool->GetShape(Shape);
  if (!aS.IsNull() && aS.ShapeType() == TopAbs_COMPOUND && !aShapeTool->IsAssembly(Shape))
  {
    //convert compound to assembly(without attributes)
    aShapeTool->Expand(Shape);
    //move attrributes
    TDF_ChildIterator anIter(Shape, Standard_True);
    for(; anIter.More(); anIter.Next())
    {
      TDF_Label aChild = anIter.Value();

      TDF_LabelSequence aLayers;
      TDF_LabelSequence aColors;
      Handle(TDataStd_Name) aName;
      getParams(Doc, aChild, aColors, aLayers, aName);
      //get part
      TDF_Label aPart;
      if(aShapeTool->GetReferredShape(aChild, aPart))
      {
        setParams(Doc, aPart, aColors, aLayers, aName);
        //remove unnecessary links
        TopoDS_Shape aShape = aShapeTool->GetShape(aChild);
        if(!aShapeTool->GetShape(aPart.Father()).IsNull())
        {
          TopLoc_Location nulloc;
          {
            aPart.ForgetAttribute(XCAFDoc::ShapeRefGUID());
            if(aShapeTool->GetShape(aPart.Father()).ShapeType() == TopAbs_COMPOUND)
            {
              aShapeTool->SetShape(aPart, aShape);
            }
            aPart.ForgetAttribute(XCAFDoc_ShapeMapTool::GetID());
            aChild.ForgetAllAttributes(Standard_False);
          }
        }
        aChild.ForgetAttribute(TNaming_NamedShape::GetID());
        aChild.ForgetAttribute(XCAFDoc_ShapeMapTool::GetID());
      }
    }
    //if assembly contains compound, expand it recursively(if flag recursively is true)
    if(recursively)
    {
      anIter.Initialize(Shape);
      for(; anIter.More(); anIter.Next())
      {
        TDF_Label aChild = anIter.Value();
        TDF_Label aPart;
        if(aShapeTool->GetReferredShape(aChild, aPart))
        {
          Expand(Doc, aPart, recursively);
        }
      }
    }
    return Standard_True;
  }
  return Standard_False;
}

//=======================================================================
//function : Expand
//purpose  : Convert all compounds in Doc to assambly
//=======================================================================

Standard_Boolean XCAFDoc_Editor::Expand (const TDF_Label& Doc, const Standard_Boolean recursively)
{
  Standard_Boolean result = Standard_False;
  TDF_LabelSequence aLabels;
  Handle(XCAFDoc_ShapeTool) aShapeTool = XCAFDoc_DocumentTool::ShapeTool(Doc);
  aShapeTool->GetFreeShapes(aLabels);
  for(Standard_Integer i = 1; i <= aLabels.Length(); i++)
  {
    TopoDS_Shape aS = aShapeTool->GetShape(aLabels(i));
    if (!aS.IsNull() && aS.ShapeType() == TopAbs_COMPOUND && !aShapeTool->IsAssembly(aLabels(i)))
      if (Expand(Doc, aLabels(i), recursively))
      {
        result = Standard_True;
      }
  }
  return result;
}

//=======================================================================
//function : ExtractAttributes
//purpose  : 
//=======================================================================

void XCAFDoc_Editor::ExtractAttributes(const TDF_Label theOld, TDF_Label& theNew)
{
  if (theOld.IsEqual(theNew))
    return;

  // Color
  Handle(XCAFDoc_ColorTool) aNewColorTool = XCAFDoc_DocumentTool::ColorTool(theNew);
  Handle(XCAFDoc_ColorTool) aColorTool = XCAFDoc_DocumentTool::ColorTool(theOld);
  Quantity_Color aColor;
  if (aColorTool->GetColor(theOld, XCAFDoc_ColorSurf, aColor))
    aNewColorTool->SetColor(theNew, aColor, XCAFDoc_ColorSurf);
  if (aColorTool->GetColor(theOld, XCAFDoc_ColorCurv, aColor))
    aNewColorTool->SetColor(theNew, aColor, XCAFDoc_ColorCurv);
  if (aColorTool->GetColor(theOld, XCAFDoc_ColorGen, aColor))
    aNewColorTool->SetColor(theNew, aColor, XCAFDoc_ColorGen);
  if (!aColorTool->IsVisible(theOld))
    aNewColorTool->SetVisibility(theNew, Standard_False);

  // Layer
  Handle(XCAFDoc_LayerTool) aNewLayerTool = XCAFDoc_DocumentTool::LayerTool(theNew);
  Handle(XCAFDoc_LayerTool) aLayerTool = XCAFDoc_DocumentTool::LayerTool(theOld);
  Handle(TColStd_HSequenceOfExtendedString) aLayers;
  aLayerTool->GetLayers(theOld, aLayers);
  for (int j = 1; j <= aLayers->Length(); j++)
  {
    aNewLayerTool->SetLayer(theNew, aLayers->Value(j));
  }

  // Material
  Handle(XCAFDoc_MaterialTool) aNewMatTool = XCAFDoc_DocumentTool::MaterialTool(theNew);
  Handle(XCAFDoc_MaterialTool) aMatTool = XCAFDoc_DocumentTool::MaterialTool(theOld);
  Handle(TDataStd_TreeNode) aMatNode;
  if (theOld.FindAttribute(XCAFDoc::MaterialRefGUID(), aMatNode) && aMatNode->HasFather())
  {
    TDF_Label aMatL = aMatNode->Father()->Label();
    Handle(TCollection_HAsciiString) aName;
    Handle(TCollection_HAsciiString) aDescription;
    Standard_Real aDensity;
    Handle(TCollection_HAsciiString) aDensName;
    Handle(TCollection_HAsciiString) aDensValType;
    if (aMatTool->GetMaterial(aMatL, aName, aDescription, aDensity, aDensName, aDensValType))
    {
      if (aName->Length() != 0)
        aNewMatTool->SetMaterial(theNew, aName, aDescription, aDensity, aDensName, aDensValType);
    }
  }

  // Attributes
  Handle(TDF_Attribute) tAtt;
  // Finds the target attributes or creates them empty.
  for (TDF_AttributeIterator attItr(theOld); attItr.More(); attItr.Next())
  {
    const Handle(TDF_Attribute) sAtt = attItr.Value();
    // protect against color and layer coping without link to colors and layers.
    if (sAtt->IsKind(STANDARD_TYPE(TDataStd_TreeNode)) || sAtt->IsKind(STANDARD_TYPE(XCAFDoc_GraphNode)))
      continue;
    // do not copy shape, it is already copied
    if (sAtt->IsKind(STANDARD_TYPE(TNaming_NamedShape)))
      continue;
    const Standard_GUID& id = sAtt->ID();
    if (!theNew.FindAttribute(id, tAtt))
    {
      tAtt = sAtt->NewEmpty();
      theNew.AddAttribute(tAtt);
    }
    Handle(TDF_RelocationTable) aRT = new TDF_RelocationTable();
    sAtt->Paste(tAtt, aRT);
  }
}

//=======================================================================
//function : ExtractSubAssembly
//purpose  : 
//=======================================================================

void XCAFDoc_Editor::ExtractSubAssembly (const TDF_LabelSequence& theSrcLabels, const TDF_Label theDstLabel)
{
  Handle(XCAFDoc_ShapeTool) aNewShapeTool = XCAFDoc_DocumentTool::ShapeTool(theDstLabel);

  // Get original label
  TDF_Label aDstAssembly;
  if (!aNewShapeTool->GetReferredShape(theDstLabel, aDstAssembly))
    aDstAssembly = theDstLabel;

  // Copy an each original shape to the new document
  TDF_LabelSequence::Iterator it(theSrcLabels);
  for (; it.More(); it.Next())
  {
    TDF_Label aRefLabel, aLabel = it.Value();
    Handle(XCAFDoc_ShapeTool) aShapeTool = XCAFDoc_DocumentTool::ShapeTool(aLabel);
    if (!aShapeTool->GetReferredShape(aLabel, aRefLabel))
      aRefLabel = aLabel;

    TopoDS_Shape anOriginalShape = aShapeTool->GetShape(aRefLabel);
    Standard_Boolean isAssembly = aShapeTool->IsAssembly(aRefLabel);

    TDF_Label aNewLabel = aNewShapeTool->AddShape(anOriginalShape, isAssembly);
    TDF_LabelMap aMap;
    copyLabel(aRefLabel, aNewLabel, aMap);

    TopLoc_Location aLoc;
    if (aLabel != aRefLabel)
      aLoc = aShapeTool->GetLocation(aLabel);
    aNewShapeTool->AddComponent(aDstAssembly, aNewLabel, aLoc);
  }
}

//=======================================================================
//function : getParams
//purpose  : Get colors layers and name
//=======================================================================

Standard_Boolean XCAFDoc_Editor::getParams (const TDF_Label& Doc, const TDF_Label& Label,
                                            TDF_LabelSequence& Colors, TDF_LabelSequence& Layers,
                                            Handle(TDataStd_Name)& Name)
{
  if(Doc.IsNull() || Label.IsNull())
    return Standard_False;

  Handle(XCAFDoc_ColorTool) aColorTool = XCAFDoc_DocumentTool::ColorTool(Doc);
  Handle(XCAFDoc_LayerTool) aLayerTool = XCAFDoc_DocumentTool::LayerTool(Doc);

  //get colors
  XCAFDoc_ColorType aTypeColor = XCAFDoc_ColorGen;
  for(;;)
  {
    TDF_Label aColor;
    aColorTool->GetColor(Label, aTypeColor, aColor);
    Colors.Append(aColor);
    if(aTypeColor == XCAFDoc_ColorCurv)
      break;
    aTypeColor = (XCAFDoc_ColorType)(aTypeColor + 1);
  }

  //get layers
  aLayerTool->GetLayers(Label, Layers);

  //get name
  Label.FindAttribute(TDataStd_Name::GetID(), Name);
  return Standard_True;
}

//=======================================================================
//function : setParams
//purpose  : set colors layers and name
//=======================================================================

Standard_Boolean XCAFDoc_Editor::setParams (const TDF_Label& Doc, const TDF_Label& Label,
                                            const TDF_LabelSequence& Colors, const TDF_LabelSequence& Layers,
                                            const Handle(TDataStd_Name)& Name)
{
  if(Doc.IsNull() || Label.IsNull())
    return Standard_False;

  Handle(XCAFDoc_ColorTool) aColorTool = XCAFDoc_DocumentTool::ColorTool(Doc);
  Handle(XCAFDoc_LayerTool) aLayerTool = XCAFDoc_DocumentTool::LayerTool(Doc);
  Handle(XCAFDoc_ShapeTool) aShapeTool = XCAFDoc_DocumentTool::ShapeTool(Doc);

  //set layers
  if(!Layers.IsEmpty())
  {
    for(Standard_Integer i = 1; i <= Layers.Length(); i++)
    {
      aLayerTool->SetLayer(Label, Layers.Value(i));
    }
  }
  //set colors
  if(!Colors.IsEmpty())
  {
    XCAFDoc_ColorType aTypeColor = XCAFDoc_ColorGen;
    for(Standard_Integer i = 1; i <= Colors.Length(); i++)
    {
      if(!Colors.Value(i).IsNull())
        aColorTool->SetColor(Label, Colors.Value(i), aTypeColor);
      aTypeColor = (XCAFDoc_ColorType)(aTypeColor + 1);
    }
  }
  //set name
  if(!Name.IsNull())
  {
    if(Name->Get().Search("=>") < 0)
      TDataStd_Name::Set(Label, Name->Get());
  }
  else
  {
    Standard_SStream Stream;
    TopoDS_Shape aShape = aShapeTool->GetShape(Label);
    TopAbs::Print(aShape.ShapeType(), Stream);
    TCollection_AsciiString aName (Stream.str().c_str());
    TDataStd_Name::Set(Label, TCollection_ExtendedString(aName));
  }
  return Standard_True;
}

//=======================================================================
//function : copyLabel
//purpose  : 
//=======================================================================

void XCAFDoc_Editor::copyLabel(const TDF_Label& theOld, TDF_Label& theNew, TDF_LabelMap& theMap)
{
  Handle(XCAFDoc_ShapeTool) aShapeTool = XCAFDoc_DocumentTool::ShapeTool(theOld);
  Handle(XCAFDoc_ShapeTool) aNewShapeTool = XCAFDoc_DocumentTool::ShapeTool(theNew);
  ExtractAttributes(theOld, theNew);

  TDF_Label anOld = theOld;
  if (aShapeTool->IsReference(anOld))
    aShapeTool->GetReferredShape(anOld, anOld);
  if (!theMap.Add(anOld))
    return;
  TDF_Label aNew = theNew;
  if (aNewShapeTool->IsReference(aNew))
    aNewShapeTool->GetReferredShape(aNew, aNew);

  TDF_LabelSequence anOldComp, aNewComp;
  if (aShapeTool->IsAssembly(anOld))
    aShapeTool->GetComponents(anOld, anOldComp);
  else
    aShapeTool->GetSubShapes(anOld, anOldComp);
  if (aShapeTool->IsAssembly(anOld) && !aNewShapeTool->IsAssembly(aNew))
  {
    aNewShapeTool->Expand(aNew);
  }
  else if (!aShapeTool->IsAssembly(anOld))
  {
    if (aNewShapeTool->IsAssembly(aNew))
    {
      // clear assembly structure
      TDF_LabelSequence aSeq;
      aNewShapeTool->GetComponents(aNew, aSeq);
      for (Standard_Integer i = aSeq.Lower(); i <= aSeq.Upper(); i++)
      {
        TDF_Label aCompLabel;
        aNewShapeTool->GetReferredShape(aSeq.Value(i), aCompLabel);
        aNewShapeTool->RemoveComponent(aSeq.Value(i));
        aNewShapeTool->RemoveShape(aCompLabel);
      }
      Handle(TDataStd_UAttribute) anUattr;
      aNew.FindAttribute(XCAFDoc::AssemblyGUID(), anUattr);
      aNew.ForgetAttribute(anUattr->ID());
      aNewShapeTool->SetShape(aNew, aShapeTool->GetShape(anOld));
    }

    for (int i = anOldComp.Lower(); i <= anOldComp.Upper(); i++)
    {
      aNewShapeTool->AddSubShape(aNew, aShapeTool->GetShape(anOldComp.Value(i)));
    }
  }
  
  if (aNewShapeTool->IsAssembly(aNew))
    aNewShapeTool->GetComponents(aNew, aNewComp);
  else
    aNewShapeTool->GetSubShapes(aNew, aNewComp);

  if (anOldComp.Length() == aNewComp.Length())
  {
    for (Standard_Integer i = 1; i <= anOldComp.Length(); i++)
    {
      TDF_Label aNewL = aNewComp.Value(i);
      TDF_Label anOldL = anOldComp.Value(i);
      copyLabel(anOldL, aNewL, theMap);
    }
  }
  ExtractAttributes(anOld, aNew);
}

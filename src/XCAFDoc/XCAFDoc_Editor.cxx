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

#include <TDF_Label.hxx>
#include <TDF_LabelSequence.hxx>
#include <TDF_ChildIterator.hxx>
#include <TDataStd_Name.hxx>
#include <TDataStd_UAttribute.hxx>
#include <TopLoc_Location.hxx>
#include <TopoDS_Shape.hxx>

#include <XCAFDoc_DocumentTool.hxx>
#include <XCAFDoc_ColorTool.hxx>
#include <XCAFDoc_LayerTool.hxx>
#include <XCAFDoc_ShapeTool.hxx>
#include <XCAFDoc_ShapeMapTool.hxx>
#include <XCAFDoc_Location.hxx>
#include <TNaming_NamedShape.hxx>
#include <BRep_Builder.hxx>

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
//function : Compress
//purpose  : Convert assembly to compound
//=======================================================================

Standard_Boolean XCAFDoc_Editor::Compact (const TDF_Label& Doc, const TDF_Label& Shape)
{
  if(Doc.IsNull() || Shape.IsNull())
    return Standard_False;

  Handle(XCAFDoc_ShapeTool) aShapeTool = XCAFDoc_DocumentTool::ShapeTool(Doc);

  TopoDS_Shape aS = aShapeTool->GetShape(Shape);
  if (!aS.IsNull() && aS.ShapeType() == TopAbs_COMPOUND && aShapeTool->IsAssembly(Shape))
  {
    Shape.ForgetAttribute(XCAFDoc::AssemblyGUID());

    TopoDS_Compound Comp;//new compound for Shape label
    BRep_Builder B;
    B.MakeCompound(Comp);

    //convert assembly to compound 
    TDF_ChildIterator anIter(Shape);
    for(; anIter.More(); anIter.Next())
    {
      TDF_Label aChild = anIter.Value();
      TopoDS_Shape aChildShape = aShapeTool->GetShape(aChild);

      TDF_Label aPart;
      if(aShapeTool->GetReferredShape(aChild, aPart))
      {
        if (aChildShape.ShapeType() == TopAbs_COMPOUND && aShapeTool->IsAssembly(aPart))
        {
          //iterate next level if it needed
          Compact(Doc, aPart);
        }
        //get location
        Handle(XCAFDoc_Location) LocationAttribute;
        aChild.FindAttribute(XCAFDoc_Location::GetID(), LocationAttribute);

        TopLoc_Location aLoc;
        if(!LocationAttribute.IsNull())
        {
          aLoc = LocationAttribute->Get();
        }

        if(aChildShape.ShapeType() != TopAbs_COMPOUND)
        {
          TDF_LabelSequence aLayers;
          TDF_LabelSequence aColors;
          Handle(TDataStd_Name) aName;
          getParams(Doc, aPart, aColors, aLayers, aName);

          aChild.ForgetAllAttributes(Standard_False);
          //move shape
          aShapeTool->SetShape(aChild, aChildShape.Located(aLoc), Standard_False);

          aChildShape.Free(Standard_True);
          B.Add(Comp, aChildShape.Located(aLoc));

          setParams(Doc, aChild, aColors, aLayers, aName);
        }
        else
        {
          aChild.ForgetAllAttributes(Standard_False);
        }

        //move subshapes
        TDF_LabelSequence aSub;
        aShapeTool->GetSubShapes(aPart,aSub);
        for(Standard_Integer i = 1; i <= aSub.Length(); i++)
        {
          TopoDS_Shape aShapeSub = aShapeTool->GetShape(aSub(i));

          TDF_LabelSequence aLayers;
          TDF_LabelSequence aColors;
          Handle(TDataStd_Name) aName;
          getParams(Doc, aSub(i), aColors, aLayers, aName);

          aSub(i).ForgetAllAttributes(Standard_False);

          TDF_TagSource aTag;
          TDF_Label aSubC = aTag.NewChild(Shape);

          TopLoc_Location loc = aLoc;
          if(aChildShape.ShapeType() == TopAbs_COMPOUND)
          {
            loc = aShapeSub.Location().Multiplied(aLoc);
          }

          //set shape
          aShapeTool->SetShape(aSubC, aShapeSub.Located(loc), Standard_False);
          aSubC.ForgetAttribute(XCAFDoc_ShapeMapTool::GetID() );
          if(aChildShape.ShapeType() == TopAbs_COMPOUND)
          {
            aShapeSub.Free(Standard_True);
            B.Add(Comp, aShapeSub.Located(loc));
          }
          setParams(Doc, aSubC, aColors, aLayers, aName);
        }
        //remove part
        aPart.ForgetAllAttributes();
      }
    }
    aShapeTool->SetShape(Shape, Comp);
    return Standard_True;
  }
  return Standard_False;
}

//=======================================================================
//function : Compress
//purpose  : Convert all assambly in Doc to compound
//=======================================================================

Standard_Boolean XCAFDoc_Editor::Compact (const TDF_Label& Doc)
{
  Standard_Boolean result = Standard_False;
  TDF_LabelSequence aLabels;
  Handle(XCAFDoc_ShapeTool) aShapeTool = XCAFDoc_DocumentTool::ShapeTool(Doc);
  aShapeTool->GetFreeShapes(aLabels);
  for(Standard_Integer i = 1; i <= aLabels.Length(); i++)
  {
    TopoDS_Shape aS = aShapeTool->GetShape(aLabels(i));
    if (!aS.IsNull() && aS.ShapeType() == TopAbs_COMPOUND && aShapeTool->IsAssembly(aLabels(i)))
      if (Compact(Doc, aLabels(i)))
      {
        result = Standard_True;
      }
  }
  return result;
}

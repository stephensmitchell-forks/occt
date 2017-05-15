// File: OCCTools_ComputeExposedArea.cxx
// Created: 20.02.2017
// Author: Mikhail Sazonov

#include "OCCTools_ComputeExposedArea.hxx"
#include <BRepAlgoAPI_Fuse.hxx>
#include <BRepGProp.hxx>
#include <BOPAlgo_MakerVolume.hxx>
#include <TopExp_Explorer.hxx>
#include <GProp_GProps.hxx>

#pragma comment(lib,"TKernel")
#pragma comment(lib,"TKMath")
#pragma comment(lib,"TKG2d")
#pragma comment(lib,"TKBRep")
#pragma comment(lib,"TKTopAlgo")
#pragma comment(lib,"TKBO")

//=======================================================================
//function : Constructor
//purpose  : 
//=======================================================================
OCCTools_ComputeExposedArea::OCCTools_ComputeExposedArea()
  : myFuzzyValue(0.),
    mySumArea(0.),
    myArea(0.),
    myIsGluing(Standard_False)
{
}

//=======================================================================
//function : Constructor
//purpose  : 
//=======================================================================
OCCTools_ComputeExposedArea::OCCTools_ComputeExposedArea(const TopTools_ListOfShape& theShapes)
  : myFuzzyValue(0.),
    mySumArea(0.),
    myArea(0.),
    myIsGluing(Standard_False)
{
  TopTools_ListOfShape::Iterator anIt(theShapes);
  for (; anIt.More(); anIt.Next())
  {
    AddShape(anIt.Value());
  }
}

//=======================================================================
//function : Constructor
//purpose  : 
//=======================================================================
Standard_Boolean OCCTools_ComputeExposedArea::AddShape(const TopoDS_Shape& theShape)
{
  TopExp_Explorer anExp(theShape, TopAbs_SOLID);
  if (anExp.More())
  {
    // Shape has solids
    for (; anExp.More(); anExp.Next())
    {
      const TopoDS_Shape& aShape = anExp.Current();
      myShapes.Append(aShape);
      GProp_GProps aProps;
      BRepGProp::SurfaceProperties(aShape, aProps);
      mySumArea += aProps.Mass();
    }
  }
  else
  {
    // No solids, so try to make them from faces
    BOPAlgo_MakerVolume aMkVol;
    aMkVol.SetRunParallel(Standard_True);
    anExp.Init(theShape, TopAbs_FACE);
    for (; anExp.More(); anExp.Next())
    {
      aMkVol.AddArgument(anExp.Current());
    }
    aMkVol.Perform();
    if (aMkVol.ErrorStatus() != 0)
      return Standard_False;
    const TopoDS_Shape& aShape = aMkVol.Shape();
    return AddShape(aShape);
  }
  return Standard_True;
}

//=======================================================================
//function : Constructor
//purpose  : 
//=======================================================================
Standard_Integer OCCTools_ComputeExposedArea::Perform(const Handle(Message_ProgressIndicator)& theProgress)
{
  if (myShapes.Extent() == 0)
  {
    // no solid shapes
    return Status_BadInput;
  }
  else if (myShapes.Extent() == 1)
  {
    // one solid - nothing to fuse
    myFusedShape = myShapes.First();
  }
  else
  {
    // create fuse algorithm
    BRepAlgoAPI_Fuse aBOP;
    aBOP.SetProgressIndicator(theProgress);
    aBOP.SetRunParallel(Standard_True);
    aBOP.SetFuzzyValue(myFuzzyValue);
    //aBOP.SetNonDestructive(Standard_True);
    if (myIsGluing)
      aBOP.SetGlue(BOPAlgo_GlueShift);

    // prepare arguments for fusion
    TopTools_ListOfShape anArgs, aTools;
    anArgs = myShapes;
    anArgs.RemoveFirst();
    aTools.Append(myShapes.First());
    aBOP.SetArguments(anArgs);
    aBOP.SetTools(aTools);

    // perform fusion
    aBOP.Build();
    if (!aBOP.IsDone())
    {
      return Status_FuseError;
    }

    myFusedShape = aBOP.Shape();
  }

  // compute area of fused shape
  GProp_GProps aProps;
  BRepGProp::SurfaceProperties(myFusedShape, aProps);
  myArea = aProps.Mass();
  return Status_OK;
}

// File: OCCTools_ComputeExposedArea.cxx
// Created: 20.02.2017
// Author: Mikhail Sazonov

#include "OCCTools_ComputeExposedArea.hxx"

#include <BRep_Builder.hxx>
#include <BRepAlgoAPI_BuilderAlgo.hxx>
#include <BRepAlgoAPI_Fuse.hxx>
#include <BRepGProp.hxx>
#include <BOPAlgo_MakerVolume.hxx>
#include <GProp_GProps.hxx>
#include <TopExp_Explorer.hxx>
#include <TopoDS_Compound.hxx>
#include <TopTools_MapOfShape.hxx>

#pragma comment(lib,"TKernel")
#pragma comment(lib,"TKMath")
#pragma comment(lib,"TKG2d")
#pragma comment(lib,"TKBRep")
#pragma comment(lib,"TKTopAlgo")
#pragma comment(lib,"TKBO")

static
  void MakeCompound(const TopoDS_Shape& theShape,
                    TopoDS_Shape& theComp,
                    const TopAbs_ShapeEnum theType = TopAbs_FACE);

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
//function : AddShape
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
    // For the case of gluing there is no need to perform operation on
    // the solids, it is enough to fuse only the faces, and remove all
    // coinciding splits from the result.
    // Without gluing we need to perform real Boolean Fuse operation
    // to avoid common part in the result

    BRepAlgoAPI_BuilderAlgo *pBuilder = myIsGluing ?
      new BRepAlgoAPI_BuilderAlgo() : new BRepAlgoAPI_Fuse();
    pBuilder->SetProgressIndicator(theProgress);
    pBuilder->SetRunParallel(Standard_True);
    pBuilder->SetFuzzyValue(myFuzzyValue);
    if (myIsGluing)
    {
      pBuilder->SetGlue(BOPAlgo_GlueShift);
      // prepare arguments - remove solids and make compound of faces
      TopTools_ListOfShape aLFaces;
      TopTools_ListIteratorOfListOfShape aItLS(myShapes);
      for (; aItLS.More(); aItLS.Next())
      {
        TopoDS_Shape aFaces;
        MakeCompound(aItLS.Value(), aFaces);
        aLFaces.Append(aFaces);
      }
      pBuilder->SetArguments(aLFaces);
    }
    else
    {
      // For the fuse one of the shapes should be an object
      // and other the tools
      TopTools_ListOfShape anObjects, aTools;
      anObjects = myShapes;
      anObjects.RemoveFirst();
      aTools.Append(myShapes.First());
      pBuilder->SetArguments(anObjects);
      ((BRepAlgoAPI_Fuse*)pBuilder)->SetTools(aTools);
    }

    // perform fusion
    pBuilder->Build();
    if (!pBuilder->IsDone())
    {
      return Status_FuseError;
    }

    myFusedShape = pBuilder->Shape();

    if (myIsGluing)
    {
      // Analyze the result of fuse - the fused result should contain only unique faces

      // Result compound
      TopoDS_Compound aResult;
      BRep_Builder().MakeCompound(aResult);

      // Make a map from the result faces to original ones
      BOPCol_IndexedDataMapOfShapeListOfShape aFaceLOrigins;

      TopTools_ListIteratorOfListOfShape aItLS(myShapes);
      for (; aItLS.More(); aItLS.Next())
      {
        const TopoDS_Shape& aS = aItLS.Value();
        TopExp_Explorer anExp(aS, TopAbs_FACE);
        for (; anExp.More(); anExp.Next())
        {
          const TopoDS_Shape& aF = anExp.Current();
          const TopTools_ListOfShape& aLFIm = pBuilder->Modified(aF);
          if (aLFIm.IsEmpty())
          {
            // the face has not been even touched
            BRep_Builder().Add(aResult, aF);
            continue;
          }

          TopTools_ListIteratorOfListOfShape aItLFIm(aLFIm);
          for (; aItLFIm.More(); aItLFIm.Next()) 
          {
            const TopoDS_Shape& aFIm = aItLFIm.Value();
            TopTools_ListOfShape *pLOrigins = aFaceLOrigins.ChangeSeek(aFIm);
            if (!pLOrigins)
              pLOrigins = &aFaceLOrigins(aFaceLOrigins.Add(aFIm, TopTools_ListOfShape()));
            pLOrigins->Append(aF);
          }
        }
      }

      Standard_Integer i, aNb = aFaceLOrigins.Extent();
      for (i = 1; i <= aNb; ++i)
      {
        if (aFaceLOrigins(i).Extent() == 1)
          BRep_Builder().Add(aResult, aFaceLOrigins.FindKey(i));
      }

      myFusedShape = aResult;
    }
  }

  // compute area of fused shape
  GProp_GProps aProps;
  BRepGProp::SurfaceProperties(myFusedShape, aProps);
  myArea = aProps.Mass();
  return Status_OK;
}

//=======================================================================
//function : MakeCompound
//purpose  : 
//=======================================================================
void MakeCompound(const TopoDS_Shape& theShape,
                  TopoDS_Shape& theComp,
                  const TopAbs_ShapeEnum theType)
{
  TopoDS_Compound aComp;
  BRep_Builder().MakeCompound(aComp);
  TopExp_Explorer anExp(theShape, theType);
  for (; anExp.More(); anExp.Next())
    BRep_Builder().Add(aComp, anExp.Current());
  theComp = aComp;
}

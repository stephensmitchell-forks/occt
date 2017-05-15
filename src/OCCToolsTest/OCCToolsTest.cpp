// File: OCCToolsTest.cpp

#include "OCCToolsTest.hxx"
#include <Draw_PluginMacro.hxx>
#include <Draw_Interpretor.hxx>
#include <Draw.hxx>
#include <DBRep.hxx>
#include <BRepGProp.hxx>
#include <GProp_GProps.hxx>
#include <OCCTools_ComputeExposedArea.hxx>
#include <OCCTools.hxx>

#pragma comment(lib,"TKernel")
#pragma comment(lib,"TKMath")
#pragma comment(lib,"TKDraw")
#pragma comment(lib,"TKG2d")
#pragma comment(lib,"TKTopAlgo")

//=======================================================================
//  taExposedArea
//=======================================================================
Standard_Integer taExposedArea(Draw_Interpretor& di,
                               Standard_Integer n, const char** a)
{
  if (n < 3)
  {
    di << "Invalid usage, use help";
    return 1;
  }

  // algorithm instance
  OCCTools_ComputeExposedArea aCompEA;

  // read options
  Standard_Integer anInd = 2;
  while (a[anInd][0] == '-')
  {
    if (strcmp(a[anInd], "-fuz") == 0)
    {
      anInd++;
      Standard_Real aValue = Draw::Atof(a[anInd++]);
      aCompEA.SetFuzzyValue(aValue);
    }
    else if (strcmp(a[anInd], "-glue") == 0)
    {
      anInd++;
      aCompEA.SetIsGluing(Standard_True);
    }
    else
    {
      di << "Invalid option " << a[anInd];
      return 1;
    }
  }

  // get shapes
  for (; anInd < n; anInd++)
  {
    TopoDS_Shape aShape = DBRep::Get(a[anInd]);
    if (aShape.IsNull())
    {
      di << "No such shape " << a[anInd];
      return 1;
    }
    aCompEA.AddShape(aShape);
  }
  di << "Nb solids is " << aCompEA.GetNbOfSolids() << "\n";
  di << "Sum area is " << aCompEA.GetSumArea() << "\n";

  // perform algo
  Standard_Integer anError = aCompEA.Perform();
  if (anError != OCCTools_ComputeExposedArea::Status_OK)
  {
    // process error statuses
    if (anError & OCCTools_ComputeExposedArea::Status_BadInput)
    {
      di << "Bad input, no solids are found\n";
    }
    if (anError & OCCTools_ComputeExposedArea::Status_FuseError)
    {
      di << "Failure of computation of fused shape\n";
    }
  }
  else
  {
    // show result
    const TopoDS_Shape& aFusedShape = aCompEA.GetFusedShape();
    Standard_Real anArea = aCompEA.GetArea();
    DBRep::Set(a[1], aFusedShape);
    di << "Area is " << anArea;
  }

  return 0;
}

//=======================================================================
//  taReadStep
//=======================================================================
Standard_Integer taReadFile(Draw_Interpretor& di,
                            Standard_Integer n, const char** a)
{
  if (n < 3)
  {
    di << "Invalid usage, use help";
    return 1;
  }

  const char* aFName = a[2];
  TopoDS_Shape aShape;
  if (strcmp(a[0], "taReadStep") == 0)
    aShape = OCCTools::ReadStep(aFName);
  else //if (strcmp(a[0], "taReadStl") == 0)
    aShape = OCCTools::ReadStl(aFName);
  if (aShape.IsNull())
  {
    di << "Read error";
  }
  else
  {
    DBRep::Set(a[1], aShape);
  }
  return 0;
}

//=======================================================================
//  Commands
//=======================================================================
void OCCToolsTest::Commands(Draw_Interpretor& theCommands) 
{
  static Standard_Boolean done = Standard_False;
  if (done) return;
  done = Standard_True;

  char* g = "OCCToolsTest commands";

  theCommands.Add("taExposedArea",
    "taExposedArea res_name [-fuz <val>] [-glue] shape [shape ...]",
    __FILE__, taExposedArea, g);
  theCommands.Add("taReadStep",
    "taReadStep res_name file_name",
    __FILE__, taReadFile, g);
  theCommands.Add("taReadStl",
    "taReadStl res_name file_name",
    __FILE__, taReadFile, g);
}

//=======================================================================
//  Factory
//=======================================================================
void OCCToolsTest::Factory(Draw_Interpretor& theDI)
{
  static Standard_Boolean FactoryDone = Standard_False;
  if (FactoryDone) return;

  FactoryDone = Standard_True;

  OCCToolsTest::Commands(theDI);
}
// Declare entry point PLUGINFACTORY
DPLUGIN(OCCToolsTest)

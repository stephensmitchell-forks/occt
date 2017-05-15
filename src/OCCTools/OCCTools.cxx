// File: OCCTools.cxx
// Created: 21.02.2017
// Author: Mikhail Sazonov

#include "OCCTools.hxx"
#include <TopoDS_Shape.hxx>
#include <StlAPI.hxx>
#include <STEPControl_Reader.hxx>
#include <Standard_ErrorHandler.hxx>

#pragma comment(lib,"TKSTL")
#pragma comment(lib,"TKXSBase")
#pragma comment(lib,"TKSTEP")

//=======================================================================
//function : ReadStl
//purpose  : 
//=======================================================================
TopoDS_Shape OCCTools::ReadStl(const char* theFName)
{
  TopoDS_Shape aShape;
  try
  {
    OCC_CATCH_SIGNALS
    StlAPI::Read(aShape, theFName);
  }
  catch (Standard_Failure)
  {
    aShape.Nullify();
  }
  return aShape;
}

//=======================================================================
//function : ReadStep
//purpose  : 
//=======================================================================
TopoDS_Shape OCCTools::ReadStep(const char* theFName)
{
  TopoDS_Shape aShape;
  try
  {
    OCC_CATCH_SIGNALS
    STEPControl_Reader aReader;
    IFSelect_ReturnStatus aStatus = aReader.ReadFile(theFName);
    if (aStatus == IFSelect_RetDone)
    {
      aReader.TransferRoots();
      aShape = aReader.OneShape();
    }
  }
  catch (Standard_Failure)
  {
    aShape.Nullify();
  }
  return aShape;
}

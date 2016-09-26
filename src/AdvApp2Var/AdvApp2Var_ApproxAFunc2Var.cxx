// Created on: 1996-07-03
// Created by: Joelle CHAUVET
// Copyright (c) 1996-1999 Matra Datavision
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

#include <AdvApp2Var_ApproxAFunc2Var.hxx>
#include <AdvApp2Var_EvaluatorFunc2Var.hxx>
#include <AdvApp2Var_Criterion.hxx>
#include <AdvApp2Var_Context.hxx>
#include <AdvApp2Var_Patch.hxx>
#include <AdvApp2Var_Network.hxx>
#include <AdvApp2Var_Node.hxx>
#include <AdvApp2Var_Iso.hxx>
#include <AdvApp2Var_Strip.hxx>
#include <AdvApp2Var_Framework.hxx>
#include <AdvApprox_Cutting.hxx>

#include <Standard_ConstructionError.hxx>
#include <Standard_OutOfRange.hxx>
#include <TColStd_HArray1OfInteger.hxx>
#include <TColStd_HArray2OfInteger.hxx>
#include <TColStd_HArray1OfReal.hxx>
#include <TColStd_HArray2OfReal.hxx>

#include <gp_XY.hxx>
#include <gp_Pnt2d.hxx>
#include <gp_Pnt.hxx>
#include <TColgp_HArray2OfPnt.hxx>

#include <Convert_GridPolynomialToPoles.hxx>

#include <Geom_BezierSurface.hxx>
#include <Geom_BSplineSurface.hxx>

AdvApp2Var_ApproxAFunc2Var::AdvApp2Var_ApproxAFunc2Var
(
  const Parameters & parameters,
  const AdvApp2Var_EvaluatorFunc2Var& Func,
  AdvApprox_Cutting& UChoice,
  AdvApprox_Cutting& VChoice
):
  myParameters    (parameters),
  myDone          (Standard_False),
  myHasResult     (Standard_False),
  myDegreeInU     (0),
  myDegreeInV     (0),
  myCriterionError(0.0)
{
  Init();
  Perform(UChoice, VChoice, Func);
  ConvertBS();
}

AdvApp2Var_ApproxAFunc2Var::AdvApp2Var_ApproxAFunc2Var
(
  const Parameters & parameters,
  const AdvApp2Var_EvaluatorFunc2Var& Func,
  const AdvApp2Var_Criterion& Crit,
  AdvApprox_Cutting& UChoice,
  AdvApprox_Cutting& VChoice
):
  myParameters    (parameters),
  myDone          (Standard_False),
  myHasResult     (Standard_False),
  myDegreeInU     (0),
  myDegreeInV     (0),
  myCriterionError(0.0)
{
  Init();
  Perform(UChoice, VChoice, Func, Crit);
  ConvertBS();
}

//=======================================================================
//function : AdvApp2Var_ApproxAFunc2Var
//purpose  : 
//=======================================================================

AdvApp2Var_ApproxAFunc2Var::AdvApp2Var_ApproxAFunc2Var
(
  const Standard_Integer Num1DSS,
  const Standard_Integer Num2DSS,
  const Standard_Integer Num3DSS,
  const Handle(TColStd_HArray1OfReal)& OneDTol,
  const Handle(TColStd_HArray1OfReal)& TwoDTol,
  const Handle(TColStd_HArray1OfReal)& ThreeDTol,
  const Handle(TColStd_HArray2OfReal)& OneDTolFr,
  const Handle(TColStd_HArray2OfReal)& TwoDTolFr,
  const Handle(TColStd_HArray2OfReal)& ThreeDTolFr,
  const Standard_Real FirstInU,
  const Standard_Real LastInU,
  const Standard_Real FirstInV,
  const Standard_Real LastInV,
  const GeomAbs_IsoType FavorIso,
  const GeomAbs_Shape ContInU,
  const GeomAbs_Shape ContInV,
  const Standard_Integer PrecisCode,
  const Standard_Integer MaxDegInU,
  const Standard_Integer MaxDegInV,
  const Standard_Integer MaxPatch,
  const AdvApp2Var_EvaluatorFunc2Var& Func,
  AdvApprox_Cutting& UChoice,
  AdvApprox_Cutting& VChoice
)
: myDone          (Standard_False),
  myHasResult     (Standard_False),
  myDegreeInU     (0),
  myDegreeInV     (0),
  myCriterionError(0.0)
{
  myParameters.NumberSubSpaces[0] = Num1DSS;
  myParameters.NumberSubSpaces[1] = Num2DSS;
  myParameters.NumberSubSpaces[2] = Num3DSS;

  myParameters.Tolerances1D = OneDTol;
  myParameters.Tolerances2D = TwoDTol;
  myParameters.Tolerances3D = ThreeDTol;

  myParameters.TolerancesOnFrontier1D = OneDTolFr;
  myParameters.TolerancesOnFrontier2D = TwoDTolFr;
  myParameters.TolerancesOnFrontier3D = ThreeDTolFr;

  myParameters.FirstParamU = FirstInU;
  myParameters.FirstParamV = FirstInV;
  myParameters.LastParamU = LastInU;
  myParameters.LastParamV = LastInV;

  myParameters.FavouriteIso = FavorIso;

  myParameters.ContinuityU = ContInU;
  myParameters.ContinuityV = ContInV;

  myParameters.PrecisionCode = PrecisCode;
  myParameters.DegreeU = MaxDegInU;
  myParameters.DegreeV = MaxDegInV;

  myParameters.MaxPatchesU = myParameters.MaxPatchesV = IntegerLast();

  myParameters.TotalPatches = MaxPatch;

  Init();
  Perform(UChoice, VChoice, Func);
  ConvertBS();
}

//=======================================================================
//function : AdvApp2Var_ApproxAFunc2Var
//purpose  : 
//=======================================================================
AdvApp2Var_ApproxAFunc2Var::AdvApp2Var_ApproxAFunc2Var
(
  const Standard_Integer Num1DSS,
  const Standard_Integer Num2DSS,
  const Standard_Integer Num3DSS,
  const Handle(TColStd_HArray1OfReal)& OneDTol,
  const Handle(TColStd_HArray1OfReal)& TwoDTol,
  const Handle(TColStd_HArray1OfReal)& ThreeDTol,
  const Handle(TColStd_HArray2OfReal)& OneDTolFr,
  const Handle(TColStd_HArray2OfReal)& TwoDTolFr,
  const Handle(TColStd_HArray2OfReal)& ThreeDTolFr,
  const Standard_Real FirstInU,
  const Standard_Real LastInU,
  const Standard_Real FirstInV,
  const Standard_Real LastInV,
  const GeomAbs_IsoType FavorIso,
  const GeomAbs_Shape ContInU,
  const GeomAbs_Shape ContInV,
  const Standard_Integer PrecisCode,
  const Standard_Integer MaxDegInU,
  const Standard_Integer MaxDegInV,
  const Standard_Integer MaxPatch,
  const AdvApp2Var_EvaluatorFunc2Var& Func,
  const AdvApp2Var_Criterion& Crit,
  AdvApprox_Cutting& UChoice,
  AdvApprox_Cutting& VChoice
)
: myDone          (Standard_False),
  myHasResult     (Standard_False),
  myDegreeInU     (0),
  myDegreeInV     (0),
  myCriterionError(0.0)
{
  myParameters.NumberSubSpaces[0] = Num1DSS;
  myParameters.NumberSubSpaces[1] = Num2DSS;
  myParameters.NumberSubSpaces[2] = Num3DSS;

  myParameters.Tolerances1D = OneDTol;
  myParameters.Tolerances2D = TwoDTol;
  myParameters.Tolerances3D = ThreeDTol;

  myParameters.TolerancesOnFrontier1D = OneDTolFr;
  myParameters.TolerancesOnFrontier2D = TwoDTolFr;
  myParameters.TolerancesOnFrontier3D = ThreeDTolFr;

  myParameters.FirstParamU = FirstInU;
  myParameters.FirstParamV = FirstInV;
  myParameters.LastParamU = LastInU;
  myParameters.LastParamV = LastInV;

  myParameters.FavouriteIso = FavorIso;

  myParameters.ContinuityU = ContInU;
  myParameters.ContinuityV = ContInV;

  myParameters.PrecisionCode = PrecisCode;
  myParameters.DegreeU = MaxDegInU;
  myParameters.DegreeV = MaxDegInV;

  myParameters.MaxPatchesU = myParameters.MaxPatchesV = IntegerLast();

  myParameters.TotalPatches = MaxPatch;

  Init();
  Perform(UChoice, VChoice, Func, Crit);
  ConvertBS();
}

//=======================================================================
//function : Init
//purpose  : Initialisation of the approximation
//=======================================================================

void AdvApp2Var_ApproxAFunc2Var::Init()
{
  Standard_Integer idxFavourite;
  Standard_Integer idxContU=0, idxContV=0;

  switch (myParameters.FavouriteIso) {
  case GeomAbs_IsoU :
    idxFavourite = 1;
    break;
  case GeomAbs_IsoV :
    idxFavourite = 2;
    break;
  default :
    idxFavourite = 2;
    break;
  }

  switch (myParameters.ContinuityU) {
  case GeomAbs_C0 :
    idxContU = 0;
    break;
  case GeomAbs_C1 :
    idxContU = 1;
    break;
  case GeomAbs_C2 :
    idxContU = 2;
    break;
  default :
    Standard_ConstructionError::Raise("AdvApp2Var_ApproxAFunc2Var : UContinuity Error");
  }

  switch (myParameters.ContinuityV) {
  case GeomAbs_C0 :
    idxContV = 0;
    break;
  case GeomAbs_C1 :
    idxContV = 1;
    break;
  case GeomAbs_C2 :
    idxContV = 2;
    break;
  default :
    Standard_ConstructionError::Raise("AdvApp2Var_ApproxAFunc2Var : VContinuity Error");
  }

  Standard_Integer degreeU = Max(myParameters.DegreeU + 1, 2 * idxContU + 2);
  Standard_Integer degreeV = Max(myParameters.DegreeV + 1, 2 * idxContV + 2);

  myParameters.PrecisionCode = Max (0, Min (myParameters.PrecisionCode, 3));

  AdvApp2Var_Context Conditions
    (
      idxFavourite,
      idxContU,
      idxContV,
      degreeU,
      degreeV,
      myParameters.PrecisionCode,
      myParameters.NumberSubSpaces[0],
      myParameters.NumberSubSpaces[1],
      myParameters.NumberSubSpaces[2],
      myParameters.Tolerances1D,
      myParameters.Tolerances2D,
      myParameters.Tolerances3D,
      myParameters.TolerancesOnFrontier1D,
      myParameters.TolerancesOnFrontier2D,
      myParameters.TolerancesOnFrontier3D
    );

  myConditions = Conditions;
  InitGrid(1, 1);
}


//=======================================================================
//function : InitGrid
//purpose  : Initialisation of the approximation with regular cuttings
//=======================================================================

void AdvApp2Var_ApproxAFunc2Var::InitGrid(const Standard_Integer sizeU, const Standard_Integer sizeV)
{
  Standard_Integer orderU = myConditions.UOrder(), orderV = myConditions.VOrder();

  AdvApp2Var_Patch M0(myParameters.FirstParamU, myParameters.LastParamU, myParameters.FirstParamV, myParameters.LastParamV, orderU, orderV);

  AdvApp2Var_SequenceOfPatch Net;
  Net.Append(M0);

  TColStd_SequenceOfReal TheU,TheV;
  TheU.Append(myParameters.FirstParamU);
  TheV.Append(myParameters.FirstParamV);
  TheU.Append(myParameters.LastParamU);
  TheV.Append(myParameters.LastParamV);

  AdvApp2Var_Network Result(Net,TheU,TheV);

  gp_XY UV1 (myParameters.FirstParamU, myParameters.FirstParamV);
  AdvApp2Var_Node C1 (UV1, orderU, orderV);
  gp_XY UV2 (myParameters.LastParamU, myParameters.FirstParamV);
  AdvApp2Var_Node C2 (UV2, orderU, orderV);
  gp_XY UV3 (myParameters.FirstParamU, myParameters.LastParamV);
  AdvApp2Var_Node C3 (UV3, orderU, orderV);
  gp_XY UV4 (myParameters.LastParamU, myParameters.LastParamV);
  AdvApp2Var_Node C4 (UV4, orderU, orderV);

  AdvApp2Var_SequenceOfNode Bag;
  Bag.Append(C1);
  Bag.Append(C2);
  Bag.Append(C3);
  Bag.Append(C4);

  AdvApp2Var_Iso V0(GeomAbs_IsoV,myParameters.FirstParamV,
        myParameters.FirstParamU,myParameters.LastParamU,myParameters.FirstParamV,myParameters.LastParamV,
        1,orderU,orderV);
  AdvApp2Var_Iso V1(GeomAbs_IsoV,myParameters.LastParamV,
        myParameters.FirstParamU,myParameters.LastParamU,myParameters.FirstParamV,myParameters.LastParamV,
        2,orderU,orderV);
  AdvApp2Var_Iso U0(GeomAbs_IsoU,myParameters.FirstParamU,
        myParameters.FirstParamU,myParameters.LastParamU,myParameters.FirstParamV,myParameters.LastParamV,
        3,orderU,orderV);
  AdvApp2Var_Iso U1(GeomAbs_IsoU,myParameters.LastParamU,
        myParameters.FirstParamU,myParameters.LastParamU,myParameters.FirstParamV,myParameters.LastParamV,
        4,orderU,orderV);

  AdvApp2Var_Strip BU0, BV0;
  BU0.Append(V0);
  BU0.Append(V1);
  BV0.Append(U0);
  BV0.Append(U1);

  AdvApp2Var_SequenceOfStrip UStrip, VStrip;
  UStrip.Append(BU0);
  VStrip.Append(BV0);

  AdvApp2Var_Framework Constraints(Bag, UStrip, VStrip);

  {
    Standard_Real deltaU = (myParameters.LastParamU - myParameters.FirstParamU)/sizeU;
    for (Standard_Integer i = 1; i < sizeU; ++i)
    {
      Result.UpdateInU(myParameters.FirstParamU + i * deltaU);
      Constraints.UpdateInU(myParameters.FirstParamU + i * deltaU);
    }
  }

  {
    Standard_Real deltaV = (myParameters.LastParamV - myParameters.FirstParamV)/sizeV;
    for (Standard_Integer i = 1; i < sizeV; ++i)
    {
      Result.UpdateInV(myParameters.FirstParamV + i * deltaV);
      Constraints.UpdateInV(myParameters.FirstParamV + i * deltaV);
    }
  }

  myResult = Result;
  myConstraints = Constraints;
}

//=======================================================================
//function : Perform
//purpose  : Computation of the approximation
//=======================================================================
void AdvApp2Var_ApproxAFunc2Var::Perform
(
  const AdvApprox_Cutting& UChoice,
  const AdvApprox_Cutting& VChoice,
  const AdvApp2Var_EvaluatorFunc2Var& Func
)
{
  ComputePatches(UChoice, VChoice, Func);
  myHasResult = myDone = Standard_True;
  Compute3DErrors();
}

//=======================================================================
//function : Perform
//purpose  : Computation of the approximation
//=======================================================================
void AdvApp2Var_ApproxAFunc2Var::Perform
(
  const AdvApprox_Cutting& UChoice,
  const AdvApprox_Cutting& VChoice,
  const AdvApp2Var_EvaluatorFunc2Var& Func,
  const AdvApp2Var_Criterion& Crit
)
{
  ComputePatches(UChoice, VChoice, Func, Crit);
  myHasResult = myDone = Standard_True;
  Compute3DErrors();
  ComputeCritError();
}

enum CuttingAllowance
{
  NoCuttingPossible = 0,
  UCuttingPossibleAndAllowed = 1,
  VCuttingPossibleAndAllowed = 2,
  UCuttingPossibleAndUVAllowed = 3,
  VCuttingPossibleAndUVAllowed = 4,
  UVCuttingPossibleAndAllowed = 5,
};

//=======================================================================
//function : ComputePatches
//purpose  : Computation of the polynomial approximations
//=======================================================================

void AdvApp2Var_ApproxAFunc2Var::ComputePatches
(
  const AdvApprox_Cutting& UChoice,
  const AdvApprox_Cutting& VChoice,
  const AdvApp2Var_EvaluatorFunc2Var& Func
)
{
  Standard_Integer currentIdx;

  while (myResult.FirstNotApprox(currentIdx))
  {
    // complete the set of constraints 
    ComputeConstraints(UChoice, VChoice, Func);

    // discretization of constraints relative to the square
    myResult(currentIdx).Discretise(myConditions, myConstraints, Func);
    if ( ! myResult(currentIdx).IsDiscretised() )
    {
      myHasResult = myDone = Standard_False;
      Standard_ConstructionError::Raise ("AdvApp2Var_ApproxAFunc2Var : Surface Discretisation Error");
    }

    // calculate the number and the type of authorized cuts
    // depending on the max number of squares and the validity of next cuts.
    Standard_Integer NbU = myResult.NbPatchInU();
    Standard_Integer NbV = myResult.NbPatchInV();

    Standard_Real Udec, Vdec;
    Standard_Boolean isUPossible = UChoice.Value(myResult(currentIdx).U0(), myResult(currentIdx).U1(), Udec);
    Standard_Boolean isVPossible = VChoice.Value(myResult(currentIdx).V0(), myResult(currentIdx).V1(), Vdec);

    if (NbU >= myParameters.MaxPatchesU)
    {
      isUPossible = Standard_False;
    }
    if (NbV >= myParameters.MaxPatchesV)
    {
      isVPossible = Standard_False;
    }

    int mode = NoCuttingPossible;

    if ( (NbU + 1) * (NbV + 1) <= myParameters.TotalPatches ) 
    {
      if ( isUPossible && isVPossible )
      {
        mode = UVCuttingPossibleAndAllowed;
      }
      else if ( isUPossible )
      {
        mode = UCuttingPossibleAndUVAllowed;
      }
      else if ( isVPossible )
      {
        mode = VCuttingPossibleAndUVAllowed;
      }
    }
    else
    {
      Standard_Boolean isUAllowed = (((NbU + 1) * NbV) <= myParameters.TotalPatches);
      Standard_Boolean isVAllowed = ((NbU * (NbV + 1)) <= myParameters.TotalPatches);
      if (isUAllowed && isVAllowed)
      {
        if ( isUPossible )
        {
          mode = UCuttingPossibleAndUVAllowed;
        }
        if ( isVPossible && (NbV > NbU) )
        {
          mode = VCuttingPossibleAndUVAllowed;
        }
      }
      else if ( isUAllowed && isUPossible)
      {
        mode = UCuttingPossibleAndAllowed;
      }
      else if ( isVAllowed && isVPossible)
      {
        mode = VCuttingPossibleAndAllowed;
      }
    }

    // approximation of the square
    myResult(currentIdx).MakeApprox(myConditions, myConstraints, mode);

    if ( myResult(currentIdx).IsApproximated() )
      continue;

    switch (myResult(currentIdx).CutSense()) {
    case 0 :
      {
        //  It is not possible to cut : the result is preserved
        if ( myResult(currentIdx).HasResult()) 
        {
          myResult(currentIdx).OverwriteApprox();
        }
        else 
        {
          myHasResult =  myDone = Standard_False;
          Standard_ConstructionError::Raise ("AdvApp2Var_ApproxAFunc2Var : Surface Approximation Error");
        }
        break;
      }
    case 1 :
      {
        // It is necessary to cut in U
        myResult.UpdateInU(Udec);
        myConstraints.UpdateInU(Udec);
        break;
      }
    case 2 :
      {
        // It is necessary to cut in V
        myResult.UpdateInV(Vdec);
        myConstraints.UpdateInV(Vdec);
        break;
      }
    case 3 :
      {
        // It is necessary to cut in U and V
        myResult.UpdateInU(Udec);
        myConstraints.UpdateInU(Udec);
        myResult.UpdateInV(Vdec);
        myConstraints.UpdateInV(Vdec);
        break;
      }
    default :
      {
        myHasResult =  myDone = Standard_False;
        Standard_ConstructionError::Raise ("AdvApp2Var_ApproxAFunc2Var : Surface Approximation Error");
      }
    }
  }
}

//=======================================================================
//function : ComputePatches
//purpose  : Computation of the polynomial approximations
//=======================================================================

void AdvApp2Var_ApproxAFunc2Var::ComputePatches
(
  const AdvApprox_Cutting& UChoice,
  const AdvApprox_Cutting& VChoice,
  const AdvApp2Var_EvaluatorFunc2Var& Func,
  const AdvApp2Var_Criterion& Crit
)
{
  Standard_Integer currentIdx;
  Standard_Integer decision=0;

  while (myResult.FirstNotApprox(currentIdx)) 
  {
    // complete the set of constraints 
    ComputeConstraints(UChoice, VChoice, Func, Crit);

    // discretize the constraints relative to the square
    myResult(currentIdx).Discretise (myConditions, myConstraints, Func);
    if ( ! myResult(currentIdx).IsDiscretised() )
    {
      myHasResult =  myDone = Standard_False;
      Standard_ConstructionError::Raise ("AdvApp2Var_ApproxAFunc2Var : Surface Discretisation Error");
    }

    // calculate the number and type of autorized cuts
    // depending on the max number of squares and the validity of next cuts
    Standard_Integer NbU = myResult.NbPatchInU();
    Standard_Integer NbV = myResult.NbPatchInV();

    Standard_Real Udec, Vdec;
    Standard_Boolean isUPossible = UChoice.Value(myResult(currentIdx).U0(), myResult(currentIdx).U1(), Udec);
    Standard_Boolean isVPossible = VChoice.Value(myResult(currentIdx).V0(), myResult(currentIdx).V1(), Vdec);

    if (NbU >= myParameters.MaxPatchesU)
    {
      isUPossible = Standard_False;
    }
    if (NbV >= myParameters.MaxPatchesV)
    {
      isVPossible = Standard_False;
    }

    int mode = NoCuttingPossible;

    if ( (NbU + 1) * (NbV + 1) <= myParameters.TotalPatches ) 
    {
      if ( isUPossible && isVPossible )
      {
        mode = UVCuttingPossibleAndAllowed;
      }
      else if ( isUPossible )
      {
        mode = UCuttingPossibleAndAllowed;
      }
      else if ( isVPossible )
      {
        mode = VCuttingPossibleAndAllowed;
      }
    }
    else
    {
      Standard_Boolean isUAllowed = (((NbU + 1) * NbV) <= myParameters.TotalPatches);
      Standard_Boolean isVAllowed = ((NbU * (NbV + 1)) <= myParameters.TotalPatches);
      if (isUAllowed && isVAllowed)
      {
        if ( isUPossible )
        {
          mode = UCuttingPossibleAndUVAllowed;
        }
        if ( isVPossible && (NbV > NbU) )
        {
          mode = VCuttingPossibleAndUVAllowed;
        }
      }
      else if ( isUAllowed && isUPossible)
      {
        mode = UCuttingPossibleAndAllowed;
      }
      else if ( isVAllowed && isVPossible)
      {
        mode = VCuttingPossibleAndAllowed;
      }
    }

    // approximation of the square
    if ( Crit.Type() == AdvApp2Var_Absolute )
    {
      myResult(currentIdx).MakeApprox(myConditions, myConstraints, 0);
    }
    else
    {
      myResult(currentIdx).MakeApprox(myConditions, myConstraints, mode);
    }

    if (mode >= 3) mode = mode - 2;

    // evaluation of the criterion on the square
    if ( myResult(currentIdx).HasResult() )
    {
      Crit.Value (myResult(currentIdx), myConditions);
    }

    // is it necessary to cut ?
    decision = myResult(currentIdx).CutSense (Crit, mode);
    Standard_Boolean Regular = (Crit.Repartition() ==  AdvApp2Var_Regular);

    if (Regular && decision > 0)
    {
      switch (decision)
      {
      case 1: ++NbU; break;
      case 2: ++NbV; break;
      case 3: ++NbU; ++NbV; break;
      default:
        {
          myHasResult =  myDone = Standard_False;
          Standard_ConstructionError::Raise ("AdvApp2Var_ApproxAFunc2Var : Surface Approximation Error");
        }
      }

      InitGrid(NbU, NbV);
    }
    else 
    {
      switch (decision) {
      case 0 :
        {
          //  It is not possible to cut : the result is preserved
          if ( myResult(currentIdx).HasResult()) 
          {
            myResult(currentIdx).OverwriteApprox();
          }
          else 
          {
            myHasResult =  myDone = Standard_False;
            Standard_ConstructionError::Raise ("AdvApp2Var_ApproxAFunc2Var : Surface Approximation Error");
          }
          break;
        }
      case 1 :
        {
          // It is necessary to cut in U
          myResult.UpdateInU(Udec);
          myConstraints.UpdateInU(Udec);
          break;
        }
      case 2 :
        {
          // It is necessary to cut in V
          myResult.UpdateInV(Vdec);
          myConstraints.UpdateInV(Vdec);
          break;
        }
      case 3 :
        {
          // It is necessary to cut in U and V
          myResult.UpdateInU(Udec);
          myConstraints.UpdateInU(Udec);
          myResult.UpdateInV(Vdec);
          myConstraints.UpdateInV(Vdec);
          break;
        }
      default :
        {
          myHasResult =  myDone = Standard_False;
          Standard_ConstructionError::Raise ("AdvApp2Var_ApproxAFunc2Var : Surface Approximation Error");
        }
      }
    }
  }
}

//=======================================================================
//function : ComputeConstraints without Criterion
//purpose  : Approximation of the constraints
//=======================================================================

void AdvApp2Var_ApproxAFunc2Var::ComputeConstraints
(
  const AdvApprox_Cutting& UChoice,
  const AdvApprox_Cutting& VChoice,
  const AdvApp2Var_EvaluatorFunc2Var& Func
)
{
  Standard_Integer ind1, ind2;
  AdvApp2Var_Iso Is;

  while ( myConstraints.FirstNotApprox(ind1, ind2, Is) )
  {
    AdvApp2Var_Node N1, N2;

    // approximation of iso and calculation of constraints at extremities
    Standard_Integer indN1 = myConstraints.FirstNode(Is.Type(), ind1, ind2);
    N1 = myConstraints.Node(indN1);
    Standard_Integer indN2 = myConstraints.LastNode(Is.Type(), ind1, ind2);
    N2 = myConstraints.Node(indN2);

    Is.MakeApprox(myConditions,
      myParameters.FirstParamU, myParameters.LastParamU,
      myParameters.FirstParamV, myParameters.LastParamV,
      Func, N1 , N2);

    if (Is.IsApproximated())
    {
      // iso is approached at the required tolerance
      myConstraints.ChangeIso(ind1,ind2,Is);
      myConstraints.ChangeNode(indN1) = N1;
      myConstraints.ChangeNode(indN2) = N2;
      continue;
    }

    // Approximation is not satisfactory

    Standard_Real param = 0.;
    Standard_Integer NbPatch = 0;
    Standard_Boolean isAddingPossible = Standard_False;

    Standard_Integer NbU = myResult.NbPatchInU();
    Standard_Integer NbV = myResult.NbPatchInV();
    if (Is.Type()==GeomAbs_IsoV)
    {
      NbPatch = (NbU+1) * NbV;
      isAddingPossible = UChoice.Value(Is.T0(),Is.T1(),param);
      isAddingPossible = isAddingPossible && (NbU < myParameters.MaxPatchesU);
    }
    else {
      NbPatch = NbU * (NbV+1);
      isAddingPossible = VChoice.Value(Is.T0(),Is.T1(),param);
      isAddingPossible = isAddingPossible && (NbV < myParameters.MaxPatchesV);
    }

    if (NbPatch <= myParameters.TotalPatches && isAddingPossible)
    {
      //  It is possible to cut iso
      if (Is.Type()==GeomAbs_IsoV)
      {
        myResult.UpdateInU(param);
        myConstraints.UpdateInU(param);
      }
      else {
        myResult.UpdateInV(param);
        myConstraints.UpdateInV(param);
      }
    }
    else 
    {
      //  It is not possible to cut : the result is preserved
      if (Is.HasResult()) 
      {
        Is.OverwriteApprox();
        myConstraints.ChangeIso(ind1,ind2,Is);
        myConstraints.ChangeNode(indN1) = N1;
        myConstraints.ChangeNode(indN2) = N2;  
      }
      else 
      {
        myHasResult =  myDone = Standard_False;
        Standard_ConstructionError::Raise ("AdvApp2Var_ApproxAFunc2Var : Curve Approximation Error");
      }
    }
  }
}


//=======================================================================
//function : ComputeConstraints with Criterion
//purpose  : Approximation of the constraints
//=======================================================================

void AdvApp2Var_ApproxAFunc2Var::ComputeConstraints
(
  const AdvApprox_Cutting& UChoice,
  const AdvApprox_Cutting& VChoice,
  const AdvApp2Var_EvaluatorFunc2Var& Func,
  const AdvApp2Var_Criterion& Crit
)
{
  Standard_Integer ind1, ind2;
  AdvApp2Var_Iso Is;

  while ( myConstraints.FirstNotApprox(ind1, ind2, Is) ) 
  {
    AdvApp2Var_Node N1, N2;

    // approximation of the iso and calculation of constraints at the extremities
    Standard_Integer indN1 = myConstraints.FirstNode(Is.Type(), ind1, ind2);
    N1 = myConstraints.Node(indN1);
    Standard_Integer indN2 = myConstraints.LastNode(Is.Type(), ind1, ind2);
    N2 = myConstraints.Node(indN2);

    Is.MakeApprox(myConditions,
                  myParameters.FirstParamU, myParameters.LastParamU,
                  myParameters.FirstParamV, myParameters.LastParamV,
                  Func, N1 , N2);

    if (Is.IsApproximated())
    {
      // iso is approached at the required tolerance
      myConstraints.ChangeIso(ind1,ind2,Is);
      myConstraints.ChangeNode(indN1) = N1;
      myConstraints.ChangeNode(indN2) = N2;
      continue;
    }

    // Approximation is not satisfactory
    Standard_Real param = 0.;
    Standard_Integer NbPatch = 0;
    Standard_Boolean isAddingPossible = Standard_False;
    
    Standard_Integer NbU = myResult.NbPatchInU();
    Standard_Integer NbV = myResult.NbPatchInV();
    if (Is.Type()==GeomAbs_IsoV)
    {
      NbPatch = (NbU+1) * NbV;
      isAddingPossible = UChoice.Value(Is.T0(),Is.T1(),param);
      isAddingPossible = isAddingPossible && (NbU < myParameters.MaxPatchesU);
    }
    else {
      NbPatch = NbU * (NbV+1);
      isAddingPossible = VChoice.Value(Is.T0(),Is.T1(),param);
      isAddingPossible = isAddingPossible && (NbV < myParameters.MaxPatchesV);
    }

    // To force Overwrite if the criterion is Absolute
    isAddingPossible = isAddingPossible && (Crit.Type() == AdvApp2Var_Relative);

    if (NbPatch <= myParameters.TotalPatches && isAddingPossible)
    {
      //  It is possible to cut iso
      if (Is.Type()==GeomAbs_IsoV)
      {
        myResult.UpdateInU(param);
        myConstraints.UpdateInU(param);
      }
      else 
      {
        myResult.UpdateInV(param);
        myConstraints.UpdateInV(param);
      }
    }
    else 
    {
      //  It is not possible to cut: the result is preserved
      if (Is.HasResult()) 
      {
        Is.OverwriteApprox();
        myConstraints.ChangeIso(ind1, ind2, Is);
        myConstraints.ChangeNode(indN1) = N1;
        myConstraints.ChangeNode(indN2) = N2;  
      }
      else
      {
        myHasResult =  myDone = Standard_False;
        Standard_ConstructionError::Raise ("AdvApp2Var_ApproxAFunc2Var : Curve Approximation Error");
      }
    }
  }
}

//=======================================================================
//function : Compute3DErrors
//purpose  : Computation of the 3D errors
//=======================================================================
void AdvApp2Var_ApproxAFunc2Var::Compute3DErrors()
{
  Standard_Real error_max,error_moy,error_U0,error_V0,error_U1,error_V1;
  Standard_Real Tol,F1Tol,F2Tol,F3Tol,F4Tol;
  if ( myParameters.NumberSubSpaces[2] > 0 )
  {
    my3DMaxError = new (TColStd_HArray1OfReal) (1,myParameters.NumberSubSpaces[2]);
    my3DAverageError = new (TColStd_HArray1OfReal) (1,myParameters.NumberSubSpaces[2]);
    my3DUFrontError = new (TColStd_HArray1OfReal) (1,myParameters.NumberSubSpaces[2]);
    my3DVFrontError = new (TColStd_HArray1OfReal) (1,myParameters.NumberSubSpaces[2]);
    for (Standard_Integer iesp = 1; iesp <= myParameters.NumberSubSpaces[2]; ++iesp)
    {
      error_max = 0;
      error_moy = 0.;
      error_U0 = 0.;
      error_V0 = 0.;
      error_U1 = 0.;
      error_V1 = 0.;
      Tol = myParameters.Tolerances3D->Value(iesp);
      F1Tol = myParameters.TolerancesOnFrontier3D->Value(iesp,1);
      F2Tol = myParameters.TolerancesOnFrontier3D->Value(iesp,2);
      F3Tol = myParameters.TolerancesOnFrontier3D->Value(iesp,3);
      F4Tol = myParameters.TolerancesOnFrontier3D->Value(iesp,4);

      for (Standard_Integer ipat = 1; ipat <= myResult.NbPatch(); ++ipat)
      {
        error_max = Max((myResult(ipat).MaxErrors())->Value(iesp),error_max);
        error_U0 = Max((myResult(ipat).IsoErrors())->Value(iesp,3),error_U0);
        error_U1 = Max((myResult(ipat).IsoErrors())->Value(iesp,4),error_U1);
        error_V0 = Max((myResult(ipat).IsoErrors())->Value(iesp,1),error_V0);
        error_V1 = Max((myResult(ipat).IsoErrors())->Value(iesp,2),error_V1);
        error_moy += (myResult(ipat).AverageErrors())->Value(iesp);
      }

      my3DMaxError->SetValue(iesp,error_max);
      my3DUFrontError->SetValue(iesp,Max(error_U0, error_U1));
      my3DVFrontError->SetValue(iesp,Max(error_V0, error_V1));
      error_moy /= (Standard_Real) myResult.NbPatch();
      my3DAverageError->SetValue(iesp,error_moy);

      if ( error_max>Tol 
        || error_U0>F3Tol || error_U1>F4Tol 
        || error_V0>F1Tol || error_V1>F2Tol) 
      {
        myDone = Standard_False;
      }
    }
  }
}


//=======================================================================
//function : ComputeCritError
//purpose  : Computation of the max value of the Criterion
//=======================================================================
void AdvApp2Var_ApproxAFunc2Var::ComputeCritError()
{
  if ( myParameters.NumberSubSpaces[2] > 0 )
  {
    for (Standard_Integer iesp=1; iesp <= myParameters.NumberSubSpaces[2]; ++iesp)
    {
      Standard_Real crit_max = 0.;
      for (Standard_Integer ipat=1; ipat <= myResult.NbPatch(); ++ipat) 
      {
        crit_max = Max((myResult(ipat).CritValue()),crit_max);
      }

      myCriterionError = crit_max;
    }
  }
}

//=======================================================================
//function : ConvertBS
//purpose  : Convertion of the approximation in BSpline Surface
//=======================================================================
void AdvApp2Var_ApproxAFunc2Var::ConvertBS ()
{
  // Homogeneization of degrees
  Standard_Integer orderU = myConditions.UOrder(), orderV = myConditions.VOrder();
  Standard_Integer limitU = myConditions.ULimit(), limitV = myConditions.VLimit();

  myResult.SameDegree(orderU,orderV,limitU,limitV);
  myDegreeInU = limitU - 1;
  myDegreeInV = limitV - 1;

  // Calculate resulting surfaces 
  mySurfaces = new ( TColGeom_HArray1OfSurface) (1,  myParameters.NumberSubSpaces[2]);

  TColStd_Array1OfReal UKnots (1, myResult.NbPatchInU() + 1);
  for (Standard_Integer j=1; j<=UKnots.Length(); ++j)
  {
    UKnots.SetValue(j, myResult.UParameter(j));
  } 
 
  TColStd_Array1OfReal VKnots (1, myResult.NbPatchInV() + 1);
  for (Standard_Integer j=1; j<=VKnots.Length(); ++j)
  {
    VKnots.SetValue(j, myResult.VParameter(j));
  }

 // Prepare data for conversion grid of polynoms --> poles
  Handle(TColStd_HArray1OfReal) Uint1 = new (TColStd_HArray1OfReal) (1,2);
  Uint1->SetValue(1, -1);
  Uint1->SetValue(2, 1);
  Handle(TColStd_HArray1OfReal) Vint1 = new (TColStd_HArray1OfReal) (1,2);
  Vint1->SetValue(1, -1);
  Vint1->SetValue(2, 1);

  Handle(TColStd_HArray1OfReal) Uint2 = new (TColStd_HArray1OfReal) (1, myResult.NbPatchInU() + 1);
  for (Standard_Integer j=1; j<=Uint2->Length(); ++j)
  {
    Uint2->SetValue(j, myResult.UParameter(j));
  } 

  Handle(TColStd_HArray1OfReal) Vint2 = new (TColStd_HArray1OfReal) (1,myResult.NbPatchInV()+1);
  for (Standard_Integer j=1; j<=Vint2->Length(); ++j)
  {
    Vint2->SetValue(j, myResult.VParameter(j));
  } 

  Standard_Integer nmax = myResult.NbPatchInU()*myResult.NbPatchInV();
  Standard_Integer Size_eq = myConditions.ULimit() * myConditions.VLimit() * 3;

  Handle(TColStd_HArray2OfInteger) NbCoeff = new (TColStd_HArray2OfInteger) (1, nmax, 1, 2);
  Handle(TColStd_HArray1OfReal) Poly = new (TColStd_HArray1OfReal) (1, nmax * Size_eq);

  for (Standard_Integer SSP=1; SSP <= myParameters.NumberSubSpaces[2]; SSP++)
  { 
    // Creation of the grid of polynoms
    Standard_Integer n=0, icf=1;

    for (Standard_Integer j = 1; j <= myResult.NbPatchInV(); ++j)
    {
      for (Standard_Integer i = 1; i <= myResult.NbPatchInU(); ++i)
      {
        n++;
        NbCoeff->SetValue(n,1, myResult.Patch(i,j).NbCoeffInU());
        NbCoeff->SetValue(n,2, myResult.Patch(i,j).NbCoeffInV());
        Handle(TColStd_HArray1OfReal) coefficients = myResult.Patch (i, j).Coefficients (SSP, myConditions);

        for (Standard_Integer ieq = 1; ieq <= Size_eq; ++ieq, ++icf)
        {
          Poly->SetValue(icf, coefficients->Value(ieq));
        }
      }
    }
  
    // Conversion into poles
    Convert_GridPolynomialToPoles CvP 
      (
        myResult.NbPatchInU(),
        myResult.NbPatchInV(),
        orderU,
        orderV,
        myParameters.DegreeU,
        myParameters.DegreeV,
        NbCoeff,
        Poly,
        Uint1,
        Vint1,
        Uint2,
        Vint2
      );

    if ( !CvP.IsDone() ) { myDone = Standard_False; }
   
    // Conversion into BSpline
    mySurfaces->ChangeValue(SSP) = new (Geom_BSplineSurface) 
      (
        CvP.Poles()->Array2(),
        CvP.UKnots()->Array1(),
        CvP.VKnots()->Array1(),
        CvP.UMultiplicities()->Array1(),
        CvP.VMultiplicities()->Array1(),
        CvP.UDegree(),
        CvP.VDegree()
      );
  }
}

//=======================================================================
//function : NumSubSpaces
//purpose  : 
//=======================================================================

Standard_Integer
AdvApp2Var_ApproxAFunc2Var::NumSubSpaces(const Standard_Integer Dimension) const 
{
  if (Dimension < 1 || Dimension > 3)
  {
    Standard_OutOfRange::Raise ("AdvApp2Var_ApproxAFunc2Var::MaxError : Dimension must be equal to 1,2 or 3 !");
  }
  return myParameters.NumberSubSpaces[Dimension-1];
}

//=======================================================================
//function : MaxError
//purpose  : 
//=======================================================================

Handle(TColStd_HArray1OfReal)
AdvApp2Var_ApproxAFunc2Var::MaxError(const Standard_Integer Dimension) const
{
  Handle (TColStd_HArray1OfReal) EPtr;
  if (Dimension < 1 || Dimension > 3)
  {
    Standard_OutOfRange::Raise ("AdvApp2Var_ApproxAFunc2Var::MaxError : Dimension must be equal to 1,2 or 3 !");
  }
  switch (Dimension) {
  case 1:
    EPtr = my1DMaxError;
    break;
  case 2:
    EPtr = my2DMaxError;
    break;
  case 3:
    EPtr = my3DMaxError;
    break;
  }
  return EPtr;
}

//=======================================================================
//function : AverageError
//purpose  : 
//=======================================================================

Handle(TColStd_HArray1OfReal)
AdvApp2Var_ApproxAFunc2Var::AverageError(const Standard_Integer Dimension) const 
{
  Handle (TColStd_HArray1OfReal) EPtr;
  if (Dimension < 1 || Dimension > 3)
  {
    Standard_OutOfRange::Raise ("AdvApp2Var_ApproxAFunc2Var::AverageError : Dimension must be equal to 1,2 or 3 !");
  }
  switch (Dimension) {
  case 1:
    EPtr = my1DAverageError;
    break;
  case 2:
    EPtr = my2DAverageError;
    break;
  case 3:
    EPtr = my3DAverageError;
    break;
  }
  return EPtr;
}

//=======================================================================
//function : UFrontError
//purpose  : 
//=======================================================================

Handle(TColStd_HArray1OfReal)
AdvApp2Var_ApproxAFunc2Var::UFrontError(const Standard_Integer Dimension) const 
{
  Handle (TColStd_HArray1OfReal) EPtr;
  if (Dimension < 1 || Dimension > 3)
  {
    Standard_OutOfRange::Raise ("AdvApp2Var_ApproxAFunc2Var::UFrontError : Dimension must be equal to 1,2 or 3 !");
  }
  switch (Dimension) {
  case 1:
    EPtr = my1DUFrontError;
    break;
  case 2:
    EPtr = my2DUFrontError;
    break;
  case 3:
    EPtr = my3DUFrontError;
    break;
  }
  return EPtr;
}

//=======================================================================
//function : VFrontError
//purpose  : 
//=======================================================================

Handle(TColStd_HArray1OfReal)
AdvApp2Var_ApproxAFunc2Var::VFrontError(const Standard_Integer Dimension) const 
{
  Handle (TColStd_HArray1OfReal) EPtr;
  if (Dimension < 1 || Dimension > 3)
  {
    Standard_OutOfRange::Raise ("AdvApp2Var_ApproxAFunc2Var::VFrontError : Dimension must be equal to 1,2 or 3 !");
  }
  switch (Dimension) {
  case 1:
    EPtr = my1DVFrontError;
    break;
  case 2:
    EPtr = my2DVFrontError;
    break;
  case 3:
    EPtr = my3DVFrontError;
    break;
  }
  return EPtr;
}

//=======================================================================
//function : MaxError
//purpose  : 
//=======================================================================

Standard_Real
AdvApp2Var_ApproxAFunc2Var::MaxError
(
  const Standard_Integer Dimension,
  const Standard_Integer SSPIndex
) const 
{
  if (Dimension != 3 || SSPIndex != 1)
  {
    Standard_OutOfRange::Raise ("AdvApp2Var_ApproxAFunc2Var::MaxError: ONE Surface 3D only !");
  }
  Handle (TColStd_HArray1OfReal) EPtr = MaxError(Dimension);
  return EPtr->Value(SSPIndex);
}

//=======================================================================
//function : AverageError
//purpose  : 
//=======================================================================
Standard_Real
AdvApp2Var_ApproxAFunc2Var::AverageError
(
  const Standard_Integer Dimension,
  const Standard_Integer SSPIndex
) const 
{
  if (Dimension != 3 || SSPIndex != 1)
  {
    Standard_OutOfRange::Raise ("AdvApp2Var_ApproxAFunc2Var::AverageError : ONE Surface 3D only !");
  }
  Handle (TColStd_HArray1OfReal) EPtr = AverageError(Dimension);
  return EPtr->Value(SSPIndex);
}

//=======================================================================
//function : UFrontError
//purpose  : 
//=======================================================================
Standard_Real
AdvApp2Var_ApproxAFunc2Var::UFrontError
(
  const Standard_Integer Dimension,
  const Standard_Integer SSPIndex
) const 
{
  if (Dimension != 3 || SSPIndex != 1)
  {
    Standard_OutOfRange::Raise ("AdvApp2Var_ApproxAFunc2Var::UFrontError : ONE Surface 3D only !");
  }
  Handle (TColStd_HArray1OfReal) EPtr = UFrontError(Dimension);
  return EPtr->Value(SSPIndex);
}

//=======================================================================
//function : VFrontError
//purpose  : 
//=======================================================================
Standard_Real
AdvApp2Var_ApproxAFunc2Var::VFrontError
(
  const Standard_Integer Dimension,
  const Standard_Integer SSPIndex
) const 
{
  if (Dimension != 3 || SSPIndex != 1) {
    Standard_OutOfRange::Raise ("AdvApp2Var_ApproxAFunc2Var::VFrontError : ONE Surface 3D only !");
  }
  Handle (TColStd_HArray1OfReal) EPtr = VFrontError(Dimension);
  return EPtr->Value(SSPIndex);
}


//=======================================================================
//function : CritError
//purpose  : 
//=======================================================================
Standard_Real
AdvApp2Var_ApproxAFunc2Var::CritError
(
  const Standard_Integer Dimension,
  const Standard_Integer SSPIndex
) const 
{
  if (Dimension != 3 || SSPIndex != 1) {
    Standard_OutOfRange::Raise ("AdvApp2Var_ApproxAFunc2Var::CritError: ONE Surface 3D only !");
  }
  return myCriterionError;
}

//=======================================================================
//function : Dump
//purpose  : 
//=======================================================================

void AdvApp2Var_ApproxAFunc2Var::Dump(Standard_OStream& o) const 
{
  Standard_Integer iesp=1,NbKU,NbKV,ik;
  o<<endl;
  if (!myHasResult) { o<<"No result"<<endl; }
  else {
    o<<"There is a result";
    if (myDone) {
      o<<" within the requested tolerance "<<myParameters.Tolerances3D->Value(iesp)<<endl;
    }
    else if (my3DMaxError->Value(iesp)>myParameters.Tolerances3D->Value(iesp)) {
      o<<" WITHOUT the requested tolerance "<<myParameters.Tolerances3D->Value(iesp)<<endl;
    }
    else {
      o<<" WITHOUT the requested continuities "<<endl;
    }
    o<<endl;
    o<<"Result max error :"<<my3DMaxError->Value(iesp)<<endl;
    o<<"Result average error :"<<my3DAverageError->Value(iesp)<<endl;
    o<<"Result max error on U frontiers :"<<my3DUFrontError->Value(iesp)<<endl;
    o<<"Result max error on V frontiers :"<<my3DVFrontError->Value(iesp)<<endl;
    o<<endl;
    o<<"Degree of Bezier patches in U : "<<myDegreeInU
      <<"  in V : "<<myDegreeInV<<endl;
    o<<endl;
    Handle(Geom_BSplineSurface) S
      = Handle(Geom_BSplineSurface)::DownCast(mySurfaces->Value(iesp));
    o<<"Number of poles in U : "<<S->NbUPoles()
      <<"  in V : "<<S->NbVPoles()<<endl;
    o<<endl;
    NbKU = S->NbUKnots();
    NbKV = S->NbVKnots();
    o<<"Number of knots in U : "<<NbKU<<endl;
    for (ik=1;ik<=NbKU;ik++) {
      o<<"   "<<ik<<" : "<<S->UKnot(ik)<<"   mult : "<<S->UMultiplicity(ik)<<endl;
    }
    o<<endl;
    o<<"Number of knots in V : "<<NbKV<<endl;
    for (ik=1;ik<=NbKV;ik++) {
      o<<"   "<<ik<<" : "<<S->VKnot(ik)<<"   mult : "<<S->VMultiplicity(ik)<<endl;
    }
    o<<endl;
  }
}

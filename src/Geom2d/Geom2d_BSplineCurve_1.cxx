// Created on: 1991-07-05
// Created by: JCV
// Copyright (c) 1991-1999 Matra Datavision
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

// 03-02-97 : pmn ->LocateU sur Periodic (PRO6963), 
//            bon appel a LocateParameter (PRO6973) et mise en conformite avec
//            le cdl de LocateU, lorsque U est un noeud (PRO6988)

#define No_Standard_OutOfRange
#define No_Standard_DimensionError

#include <Geom2d_BSplineCurve.jxx>
#include <BSplCLib.hxx>
#include <gp.hxx>

#include <Geom2d_UndefinedDerivative.hxx>
#include <Standard_DimensionError.hxx>
#include <Standard_OutOfRange.hxx>
#include <Standard_DomainError.hxx>
#include <Standard_RangeError.hxx>
#include <Standard_Mutex.hxx>

#define  POLES    (poles->Array1())
#define  KNOTS    (knots->Array1())
#define  FKNOTS   (flatknots->Array1())
#define  FMULTS   (BSplCLib::NoMults())

//=======================================================================
//function : IsCN
//purpose  : 
//=======================================================================

Standard_Boolean Geom2d_BSplineCurve::IsCN ( const Standard_Integer N) const
{
  Standard_RangeError_Raise_if
    (N < 0, "Geom2d_BSplineCurve::IsCN");

  switch (smooth) {
  case GeomAbs_CN : return Standard_True;
  case GeomAbs_C0 : return N <= 0;
  case GeomAbs_G1 : return N <= 0;
  case GeomAbs_C1 : return N <= 1;
  case GeomAbs_G2 : return N <= 1;
  case GeomAbs_C2 : return N <= 2;
  case GeomAbs_C3 : 
    return N <= 3 ? Standard_True :
           N <= deg - BSplCLib::MaxKnotMult (mults->Array1(), mults->Lower() + 1, mults->Upper() - 1);
  default:
    return Standard_False;
  }
}
//=======================================================================
//function : IsG1
//purpose  : 
//=======================================================================

Standard_Boolean Geom2d_BSplineCurve::IsG1 ( const Standard_Real theTf,
                                           const Standard_Real theTl,
                                           const Standard_Real theAngTol) const
{

  if(IsCN(1))
  {
    return Standard_True;
  }

  Standard_Integer  start = FirstUKnotIndex()+1,
                    finish = LastUKnotIndex()-1;
  Standard_Integer aDeg = Degree();
  for(Standard_Integer aNKnot = start; aNKnot <= finish; aNKnot++)
  {
    const Standard_Real aTpar = Knot(aNKnot);

    if(aTpar < theTf)
      continue;
    if(aTpar > theTl)
      break;

    Standard_Integer mult = Multiplicity(aNKnot);
    if (mult < aDeg)
      continue;

    gp_Pnt2d aP1, aP2;
    gp_Vec2d aV1, aV2;
    LocalD1(aTpar, aNKnot-1, aNKnot, aP1, aV1);
    LocalD1(aTpar, aNKnot, aNKnot+1, aP2, aV2);

    if((aV1.SquareMagnitude() <= gp::Resolution()) ||
        aV2.SquareMagnitude() <= gp::Resolution())
    {
      return Standard_False;
    }

    if(Abs(aV1.Angle(aV2)) > theAngTol)
      return Standard_False;
  }

  if(!IsPeriodic())
    return Standard_True;

  const Standard_Real aFirstParam = FirstParameter(),
                      aLastParam = LastParameter();

  if( ((aFirstParam - theTf)*(theTl - aFirstParam) < 0.0) &&
      ((aLastParam - theTf)*(theTl - aLastParam) < 0.0))
  {
    //Range [theTf, theTl] does not intersect curve bounadries
    return Standard_True;
  }

  //Curve is closed or periodic and range [theTf, theTl]
  //intersect curve boundary. Therefore, it is necessary to 
  //check if curve is smooth in its first and last point.

  gp_Pnt2d aP;
  gp_Vec2d aV1, aV2;
  D1(Knot(FirstUKnotIndex()), aP, aV1);
  D1(Knot(LastUKnotIndex()), aP, aV2);

  if((aV1.SquareMagnitude() <= gp::Resolution()) ||
      aV2.SquareMagnitude() <= gp::Resolution())
  {
    return Standard_False;
  }

  if(Abs(aV1.Angle(aV2)) > theAngTol)
    return Standard_False;


  return Standard_True;

}

//=======================================================================
//function : IsClosed
//purpose  : 
//=======================================================================

Standard_Boolean Geom2d_BSplineCurve::IsClosed () const
{ return (StartPoint().Distance (EndPoint())) <= gp::Resolution (); }



//=======================================================================
//function : IsPeriodic
//purpose  : 
//=======================================================================

Standard_Boolean Geom2d_BSplineCurve::IsPeriodic () const
{ return periodic; }

//=======================================================================
//function : Continuity
//purpose  : 
//=======================================================================

GeomAbs_Shape Geom2d_BSplineCurve::Continuity () const
{ return smooth; }

//=======================================================================
//function : Degree
//purpose  : 
//=======================================================================

Standard_Integer Geom2d_BSplineCurve::Degree () const
{ return deg; }


//=======================================================================
//function : D0
//purpose  : 
//=======================================================================

void Geom2d_BSplineCurve::D0 ( const Standard_Real U, 
			      gp_Pnt2d& P) const
{
  Standard_Real NewU(U);
  PeriodicNormalization(NewU);

  Geom2d_BSplineCurve* MyCurve = (Geom2d_BSplineCurve *) this;
  Standard_Mutex::Sentry aSentry(MyCurve->myMutex);

  if (!IsCacheValid(NewU))
    MyCurve->ValidateCache(NewU);
  
  if(rational)
  {
    BSplCLib::CacheD0(NewU,
		      deg,
		      parametercache,
		      spanlenghtcache,
		      (cachepoles->Array1()),
		      cacheweights->Array1(),
		      P) ;
  }
  else {
    BSplCLib::CacheD0(NewU,
		      deg,
		      parametercache,
		      spanlenghtcache,
		      (cachepoles->Array1()),
		      BSplCLib::NoWeights(),
		      P) ;
  }
}


//=======================================================================
//function : D1
//purpose  : 
//=======================================================================

void Geom2d_BSplineCurve::D1 (const Standard_Real U,
			      gp_Pnt2d& P,
			      gp_Vec2d& V1) const
{
  Standard_Real NewU(U);
  PeriodicNormalization(NewU);

  Geom2d_BSplineCurve* MyCurve = (Geom2d_BSplineCurve *) this;
  Standard_Mutex::Sentry aSentry(MyCurve->myMutex);

  if (!IsCacheValid(NewU))
    MyCurve->ValidateCache(NewU);
  
  if(rational)
  {
    BSplCLib::CacheD1(NewU,
		      deg,
		      parametercache,
		      spanlenghtcache,
		      (cachepoles->Array1()),
		      cacheweights->Array1(),
		      P,
		      V1) ;
  }
  else {
    BSplCLib::CacheD1(NewU,
		      deg,
		      parametercache,
		      spanlenghtcache,
		      (cachepoles->Array1()),
		      BSplCLib::NoWeights(),
		      P,
		      V1) ;
  }
}

//=======================================================================
//function : D2
//purpose  : 
//=======================================================================

void Geom2d_BSplineCurve::D2 (const Standard_Real U ,
			      gp_Pnt2d& P ,
			      gp_Vec2d& V1,
			      gp_Vec2d& V2 ) const
{
  Standard_Real NewU(U);
  PeriodicNormalization(NewU);

  Geom2d_BSplineCurve* MyCurve = (Geom2d_BSplineCurve *) this;
  Standard_Mutex::Sentry aSentry(MyCurve->myMutex);

  if (!IsCacheValid(NewU))
    MyCurve->ValidateCache(NewU);
  
  if(rational)
  {
    BSplCLib::CacheD2(NewU,
		      deg,
		      parametercache,
		      spanlenghtcache,
		      (cachepoles->Array1()),
		      cacheweights->Array1(),
		      P,
		      V1,
		      V2) ;
  }
  else {
    BSplCLib::CacheD2(NewU,
		      deg,
		      parametercache,
		      spanlenghtcache,
		      (cachepoles->Array1()),
		      BSplCLib::NoWeights(),
		      P,
		      V1,
		      V2) ;
  }
}

//=======================================================================
//function : D3
//purpose  : 
//=======================================================================

void Geom2d_BSplineCurve::D3  (const Standard_Real U ,
			       gp_Pnt2d& P ,
			       gp_Vec2d& V1,
			       gp_Vec2d& V2,
			       gp_Vec2d& V3 ) const
{
  Standard_Real NewU(U);
  PeriodicNormalization(NewU);

  Geom2d_BSplineCurve* MyCurve = (Geom2d_BSplineCurve *) this;
  Standard_Mutex::Sentry aSentry(MyCurve->myMutex);

  if (!IsCacheValid(NewU))
    MyCurve->ValidateCache(NewU);
  
  if(rational)
  {
    BSplCLib::CacheD3(NewU,
		      deg,
		      parametercache,
		      spanlenghtcache,
		      (cachepoles->Array1()),
		      cacheweights->Array1(),
		      P,
		      V1,
		      V2,
		      V3) ;
  }
  else {
    BSplCLib::CacheD3(NewU,
		      deg,
		      parametercache,
		      spanlenghtcache,
		      (cachepoles->Array1()),
		      BSplCLib::NoWeights(),
		      P,
		      V1,
		      V2,
		      V3) ;
  }
}

//=======================================================================
//function : DN
//purpose  : 
//=======================================================================

gp_Vec2d Geom2d_BSplineCurve::DN  (const Standard_Real    U,
				   const Standard_Integer N ) const
{
  gp_Vec2d V;

  if ( rational ) {
    BSplCLib::DN(U,N,0,deg,periodic,POLES,
		 weights->Array1(),
		 FKNOTS,FMULTS,V);
  }
  else {  
    BSplCLib::DN(U,N,0,deg,periodic,POLES,
		 *((TColStd_Array1OfReal*) NULL),
		 FKNOTS,FMULTS,V);
  }
  return V;
}

//=======================================================================
//function : EndPoint
//purpose  : 
//=======================================================================

gp_Pnt2d Geom2d_BSplineCurve::EndPoint () const
{ 
  if (mults->Value (knots->Upper ()) == deg + 1) 
    return poles->Value (poles->Upper());
  else
    return Value(LastParameter());
}

//=======================================================================
//function : FirstUKnotIndex
//purpose  : 
//=======================================================================

Standard_Integer Geom2d_BSplineCurve::FirstUKnotIndex () const
{ 
  if (periodic) return 1;
  else return BSplCLib::FirstUKnotIndex (deg, mults->Array1()); 
}

//=======================================================================
//function : FirstParameter
//purpose  : 
//=======================================================================

Standard_Real Geom2d_BSplineCurve::FirstParameter () const
{
  return flatknots->Value (deg+1); 
}

//=======================================================================
//function : Knot
//purpose  : 
//=======================================================================

Standard_Real Geom2d_BSplineCurve::Knot (const Standard_Integer Index) const
{
  Standard_OutOfRange_Raise_if
    (Index < 1 || Index > knots->Length(), "Geom2d_BSplineCurve::Knot");
  return knots->Value (Index);
}

//=======================================================================
//function : KnotDistribution
//purpose  : 
//=======================================================================

GeomAbs_BSplKnotDistribution Geom2d_BSplineCurve::KnotDistribution () const
{ 
  return knotSet; 
}

//=======================================================================
//function : Knots
//purpose  : 
//=======================================================================

void Geom2d_BSplineCurve::Knots (TColStd_Array1OfReal& K) const
{
  Standard_DimensionError_Raise_if
    (K.Length() != knots->Length(), "Geom2d_BSplineCurve::Knots");
  K = knots->Array1();
}

const TColStd_Array1OfReal& Geom2d_BSplineCurve::Knots() const
{
  return knots->Array1();
}

//=======================================================================
//function : KnotSequence
//purpose  : 
//=======================================================================

void Geom2d_BSplineCurve::KnotSequence (TColStd_Array1OfReal& K) const
{
  Standard_DimensionError_Raise_if
    (K.Length() != flatknots->Length(), "Geom2d_BSplineCurve::KnotSequence");
  K = flatknots->Array1();
}

const TColStd_Array1OfReal& Geom2d_BSplineCurve::KnotSequence() const
{
  return flatknots->Array1();
}

//=======================================================================
//function : LastUKnotIndex
//purpose  : 
//=======================================================================

Standard_Integer Geom2d_BSplineCurve::LastUKnotIndex() const
{
  if (periodic) return knots->Length();
  else return BSplCLib::LastUKnotIndex (deg, mults->Array1()); 
}

//=======================================================================
//function : LastParameter
//purpose  : 
//=======================================================================

Standard_Real Geom2d_BSplineCurve::LastParameter () const
{
  return flatknots->Value (flatknots->Upper()-deg); 
}

//=======================================================================
//function : LocalValue
//purpose  : 
//=======================================================================

gp_Pnt2d Geom2d_BSplineCurve::LocalValue
(const Standard_Real    U,
 const Standard_Integer FromK1,
 const Standard_Integer ToK2)   const
{
  gp_Pnt2d P;
  LocalD0(U,FromK1,ToK2,P);
  return P;
}

//=======================================================================
//function : LocalD0
//purpose  : 
//=======================================================================

void  Geom2d_BSplineCurve::LocalD0
(const Standard_Real    U,
 const Standard_Integer FromK1,
 const Standard_Integer ToK2,
 gp_Pnt2d& P)   const
{
  Standard_DomainError_Raise_if (FromK1 == ToK2,
				 "Geom2d_BSplineCurve::LocalValue");

  Standard_Real u = U;
  Standard_Integer index = 0;
  BSplCLib::LocateParameter(deg, FKNOTS, U, periodic,FromK1,ToK2, index,u);
  index = BSplCLib::FlatIndex(deg,index,mults->Array1(),periodic);

  if ( rational ) {
    BSplCLib::D0(u,index,deg,periodic,POLES,
		 weights->Array1(),
		 FKNOTS,FMULTS,P);
  }
  else {
    BSplCLib::D0(u,index,deg,periodic,POLES,
		 *((TColStd_Array1OfReal*) NULL),
		 FKNOTS,FMULTS,P);
  }
}

//=======================================================================
//function : LocalD1
//purpose  : 
//=======================================================================

void Geom2d_BSplineCurve::LocalD1 (const Standard_Real    U,
				   const Standard_Integer FromK1,
				   const Standard_Integer ToK2,
				   gp_Pnt2d&    P, 
				   gp_Vec2d&    V1)    const
{
  Standard_DomainError_Raise_if (FromK1 == ToK2,
				 "Geom2d_BSplineCurve::LocalD1");
  
  Standard_Real u = U;
  Standard_Integer index = 0;
  BSplCLib::LocateParameter(deg, FKNOTS, U, periodic, FromK1,ToK2, index, u);
  index = BSplCLib::FlatIndex(deg,index,mults->Array1(),periodic);
  
  if (rational) {
    BSplCLib::D1(u,index,deg,periodic,POLES,
		 weights->Array1(),
		 FKNOTS,FMULTS,P,V1);
  }
  else {
    BSplCLib::D1(u,index,deg,periodic,POLES,
		 *((TColStd_Array1OfReal*) NULL),
		 FKNOTS,FMULTS,P,V1);
  }
}

//=======================================================================
//function : LocalD2
//purpose  : 
//=======================================================================

void Geom2d_BSplineCurve::LocalD2
(const Standard_Real    U,
 const Standard_Integer FromK1,
 const Standard_Integer ToK2, 
 gp_Pnt2d&    P,
 gp_Vec2d&    V1,
 gp_Vec2d&    V2) const
{
  Standard_DomainError_Raise_if (FromK1 == ToK2,
				 "Geom2d_BSplineCurve::LocalD2");
  
  Standard_Real u = U;
  Standard_Integer index = 0;
  BSplCLib::LocateParameter(deg, FKNOTS, U, periodic, FromK1,ToK2, index, u);
  index = BSplCLib::FlatIndex(deg,index,mults->Array1(),periodic);
  
  if ( rational ) {
    BSplCLib::D2(u,index,deg,periodic,POLES,
		 weights->Array1(),
		 FKNOTS,FMULTS,P,V1,V2);
  }
  else {
    BSplCLib::D2(u,index,deg,periodic,POLES,
		 *((TColStd_Array1OfReal*) NULL),
		 FKNOTS,FMULTS,P,V1,V2);
  }
}

//=======================================================================
//function : LocalD3
//purpose  : 
//=======================================================================

void Geom2d_BSplineCurve::LocalD3
(const Standard_Real    U,
 const Standard_Integer FromK1,
 const Standard_Integer ToK2, 
 gp_Pnt2d&    P,
 gp_Vec2d&    V1,
 gp_Vec2d&    V2,
 gp_Vec2d&    V3) const
{
  Standard_DomainError_Raise_if (FromK1 == ToK2,
				 "Geom2d_BSplineCurve::LocalD3");
  
  Standard_Real u = U;
  Standard_Integer index = 0;
  BSplCLib::LocateParameter(deg, FKNOTS, U, periodic, FromK1,ToK2, index, u);
  index = BSplCLib::FlatIndex(deg,index,mults->Array1(),periodic);
  
  if ( rational ) {
    BSplCLib::D3(u,index,deg,periodic,POLES,
		 weights->Array1(),
		 FKNOTS,FMULTS,P,V1,V2,V3);
  }
  else {
    BSplCLib::D3(u,index,deg,periodic,POLES,
		 *((TColStd_Array1OfReal*) NULL),
		 FKNOTS,FMULTS,P,V1,V2,V3);
  }
}

//=======================================================================
//function : LocalDN
//purpose  : 
//=======================================================================

gp_Vec2d Geom2d_BSplineCurve::LocalDN
(const Standard_Real    U,
 const Standard_Integer FromK1,
 const Standard_Integer ToK2,
 const Standard_Integer N      ) const
{
  Standard_DomainError_Raise_if (FromK1 == ToK2,
				 "Geom2d_BSplineCurve::LocalD3");
  
  Standard_Real u = U;
  Standard_Integer index = 0;
  BSplCLib::LocateParameter(deg, FKNOTS, U, periodic, FromK1,ToK2, index, u);
  index = BSplCLib::FlatIndex(deg,index,mults->Array1(),periodic);
  
  gp_Vec2d V;

  if ( rational ) {
    BSplCLib::DN(u,N,index,deg,periodic,POLES,
		 weights->Array1(),
		 FKNOTS,FMULTS,V);
  }
  else {
    BSplCLib::DN(u,N,index,deg,periodic,POLES,
		 *((TColStd_Array1OfReal*) NULL),
		 FKNOTS,FMULTS,V);
  }
  return V;
}

//=======================================================================
//function : Multiplicity
//purpose  : 
//=======================================================================

Standard_Integer Geom2d_BSplineCurve::Multiplicity 
(const Standard_Integer Index) const
{
  Standard_OutOfRange_Raise_if (Index < 1 || Index > mults->Length(),
				"Geom2d_BSplineCurve::Multiplicity");
  return mults->Value (Index);
}

//=======================================================================
//function : Multiplicities
//purpose  : 
//=======================================================================

void Geom2d_BSplineCurve::Multiplicities (TColStd_Array1OfInteger& M) const
{
  Standard_DimensionError_Raise_if (M.Length() != mults->Length(),
				    "Geom2d_BSplineCurve::Multiplicities");
  M = mults->Array1();
}

const TColStd_Array1OfInteger& Geom2d_BSplineCurve::Multiplicities() const
{
  return mults->Array1();
}

//=======================================================================
//function : NbKnots
//purpose  : 
//=======================================================================

Standard_Integer Geom2d_BSplineCurve::NbKnots () const
{ return knots->Length(); }

//=======================================================================
//function : NbPoles
//purpose  : 
//=======================================================================

Standard_Integer Geom2d_BSplineCurve::NbPoles () const
{ return poles->Length(); }

//=======================================================================
//function : Pole
//purpose  : 
//=======================================================================

gp_Pnt2d Geom2d_BSplineCurve::Pole (const Standard_Integer Index) const
{
  Standard_OutOfRange_Raise_if (Index < 1 || Index > poles->Length(),
				"Geom2d_BSplineCurve::Pole");
  return poles->Value (Index);      
}

//=======================================================================
//function : Poles
//purpose  : 
//=======================================================================

void Geom2d_BSplineCurve::Poles (TColgp_Array1OfPnt2d& P) const
{
  Standard_DimensionError_Raise_if (P.Length() != poles->Length(),
				    "Geom2d_BSplineCurve::Poles");
  P = poles->Array1();
}

const TColgp_Array1OfPnt2d& Geom2d_BSplineCurve::Poles() const
{
  return poles->Array1();
}

//=======================================================================
//function : StartPoint
//purpose  : 
//=======================================================================

gp_Pnt2d Geom2d_BSplineCurve::StartPoint () const
{
  if (mults->Value (1) == deg + 1)  
    return poles->Value (1);
  else 
    return Value(FirstParameter());
}

//=======================================================================
//function : Weight
//purpose  : 
//=======================================================================

Standard_Real Geom2d_BSplineCurve::Weight
(const Standard_Integer Index) const
{
  Standard_OutOfRange_Raise_if (Index < 1 || Index > poles->Length(),
				"Geom2d_BSplineCurve::Weight");
  if (IsRational())
    return weights->Value (Index);
  else
    return 1.;
}

//=======================================================================
//function : Weights
//purpose  : 
//=======================================================================

void Geom2d_BSplineCurve::Weights
(TColStd_Array1OfReal& W) const
{
  Standard_DimensionError_Raise_if (W.Length() != poles->Length(),
				    "Geom2d_BSplineCurve::Weights");
  if (IsRational())
    W = weights->Array1();
  else {
    Standard_Integer i;
    for (i = W.Lower(); i <= W.Upper(); i++)
      W(i) = 1.;
  }
}

const TColStd_Array1OfReal& Geom2d_BSplineCurve::Weights() const
{
  if (IsRational())
    return weights->Array1();
  return BSplCLib::NoWeights();
}

//=======================================================================
//function : IsRational
//purpose  : 
//=======================================================================

Standard_Boolean Geom2d_BSplineCurve::IsRational () const
{ 
  return !weights.IsNull(); 
} 

//=======================================================================
//function : Transform
//purpose  : 
//=======================================================================

void Geom2d_BSplineCurve::Transform
(const gp_Trsf2d& T)
{
  TColgp_Array1OfPnt2d & CPoles = poles->ChangeArray1();
  for (Standard_Integer I = 1; I <= CPoles.Length(); I++)  
    CPoles (I).Transform (T);

  InvalidateCache();
  //  maxderivinvok = 0;
}

//=======================================================================
//function : LocateU
//purpose  : 
//=======================================================================

void Geom2d_BSplineCurve::LocateU
(const Standard_Real    U, 
 const Standard_Real    ParametricTolerance, 
 Standard_Integer&      I1,
 Standard_Integer&      I2,
 const Standard_Boolean WithKnotRepetition) const
{
  Standard_Real NewU = U;
  Handle(TColStd_HArray1OfReal) TheKnots;
  if (WithKnotRepetition)  TheKnots = flatknots;
  else                     TheKnots = knots;
  
  const TColStd_Array1OfReal & CKnots = TheKnots->Array1();

  PeriodicNormalization(NewU); //Attention a la periode
  Standard_Real UFirst = CKnots (1);
  Standard_Real ULast  = CKnots (CKnots.Length());
  Standard_Real PParametricTolerance = Abs(ParametricTolerance);
  if (Abs (NewU - UFirst) <= PParametricTolerance) { I1 = I2 = 1; }
  else if (Abs (NewU - ULast) <= PParametricTolerance) { 
    I1 = I2 = CKnots.Length();
  }
  else if (NewU < UFirst) {
    I2 = 1;
    I1 = 0;
  }
  else if (NewU > ULast) {
    I1 = CKnots.Length();
    I2 = I1 + 1;
  }
  else {
    I1 = 1;
    BSplCLib::Hunt (CKnots, NewU, I1);
    while ( Abs( CKnots(I1+1) - NewU) <= PParametricTolerance) I1++;
    if ( Abs( CKnots(I1) - NewU) <= PParametricTolerance) {
      I2 = I1;
    }
    else {
      I2 = I1 + 1;
    }
  }
}

//=======================================================================
//function : Resolution
//purpose  : 
//=======================================================================

void Geom2d_BSplineCurve::Resolution(const Standard_Real ToleranceUV,
				     Standard_Real &     UTolerance) 
{
  Standard_Integer ii ;  
  if(!maxderivinvok){
    if ( periodic) {
      Standard_Integer NbKnots, NbPoles;
      BSplCLib::PrepareUnperiodize( deg, 
				   mults->Array1(),
				   NbKnots,
				   NbPoles);
      TColgp_Array1OfPnt2d    new_poles(1,NbPoles) ;
      TColStd_Array1OfReal  new_weights(1,NbPoles) ;
      for(ii = 1 ; ii <= NbPoles ; ii++) {
	new_poles(ii) = poles->Array1()(((ii-1) % poles->Length()) + 1) ;
      }
      if (rational) {
	for(ii = 1 ; ii <= NbPoles ; ii++) {
	  new_weights(ii) = weights->Array1()(((ii-1) % poles->Length()) + 1) ;
	}
	BSplCLib::Resolution(new_poles,
			     new_weights,
			     new_poles.Length(),
			     flatknots->Array1(),
			     deg,
			     1.,
			     maxderivinv) ;
      }
      else {
	BSplCLib::Resolution(new_poles,
			     *((TColStd_Array1OfReal*) NULL),
			     new_poles.Length(),
			     flatknots->Array1(),
			     deg,
			     1.,
			     maxderivinv) ;
      }
    }
    else {
      if (rational) {
	BSplCLib::Resolution(poles->Array1(),
			     weights->Array1(),
			     poles->Length(),
			     flatknots->Array1(),
			     deg,
			     1.,
			     maxderivinv) ;
      }
      else {
	BSplCLib::Resolution(poles->Array1(),
			     *((TColStd_Array1OfReal*) NULL),
			     poles->Length(),
			     flatknots->Array1(),
			     deg,
			     1.,
			     maxderivinv) ;
      }
    }
    maxderivinvok = 1;
  } 
  UTolerance = ToleranceUV * maxderivinv; 
}


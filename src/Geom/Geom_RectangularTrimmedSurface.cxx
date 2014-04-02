// Created on: 1993-03-10
// Created by: JCV
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

// *******************************************************************
// *******************************************************************



#include <Geom_RectangularTrimmedSurface.ixx>


#include <Standard_ConstructionError.hxx>
#include <Standard_RangeError.hxx>

#include <Geom_TrimmedCurve.hxx>
#include <Geom_Geometry.hxx>
#include <Geom_Surface.hxx>
#include <Geom_Plane.hxx>
#include <Geom_CylindricalSurface.hxx>
#include <Geom_ConicalSurface.hxx>
#include <Geom_SphericalSurface.hxx>
#include <Geom_ToroidalSurface.hxx>
#include <Geom_BezierSurface.hxx>
#include <Geom_BSplineSurface.hxx>
#include <Geom_SurfaceOfRevolution.hxx>
#include <Geom_SurfaceOfLinearExtrusion.hxx>
#include <Geom_OffsetSurface.hxx>

#include <Precision.hxx>
#include <ElCLib.hxx>

typedef Handle(Geom_Surface)           Handle(Surface);
typedef Handle(Geom_Geometry)          Handle(Geometry);
typedef Geom_RectangularTrimmedSurface RectangularTrimmedSurface;
typedef Handle(Geom_RectangularTrimmedSurface) Handle(RectangularTrimmedSurface);

typedef gp_Ax1  Ax1;
typedef gp_Ax2  Ax2;
typedef gp_Pnt  Pnt;
typedef gp_Trsf Trsf;
typedef gp_Vec  Vec;





//=======================================================================
//function : Copy
//purpose  : 
//=======================================================================

Handle(Geom_Geometry) Geom_RectangularTrimmedSurface::Copy () const {

  Handle(RectangularTrimmedSurface) S;

  if ( isutrimmed && isvtrimmed ) 
    S = new RectangularTrimmedSurface (basisSurf,
				       utrim1   , utrim2,
				       vtrim1   , vtrim2,
				       Standard_True     , Standard_True   );
  else if ( isutrimmed)
    S = new RectangularTrimmedSurface (basisSurf,
				       utrim1   , utrim2,
				       Standard_True,      Standard_True   );
  else if (isvtrimmed)
    S = new RectangularTrimmedSurface (basisSurf,
				       vtrim1   , vtrim2,
				       Standard_False    , Standard_True   );

  return S;
}

//=======================================================================
//function : Geom_RectangularTrimmedSurface
//purpose  : 
//=======================================================================

Geom_RectangularTrimmedSurface::Geom_RectangularTrimmedSurface(
                          const Handle(Geom_Surface)& S,
                          const Standard_Boolean isUTrim,
                          const Standard_Boolean isVTrim,
                          const Standard_Real U1,
                          const Standard_Real U2,
                          const Standard_Real V1,
                          const Standard_Real V2):  utrim1(U1),
                                                    vtrim1(V1),
                                                    utrim2(U2),
                                                    vtrim2(V2),
                                                    isutrimmed (isUTrim),
                                                    isvtrimmed (isVTrim)
{
  // kill trimmed basis surfaces
  Handle(Geom_RectangularTrimmedSurface) T =
    Handle(Geom_RectangularTrimmedSurface)::DownCast(S);
  if (!T.IsNull())
    basisSurf = Handle(Surface)::DownCast(T->BasisSurface()->Copy());
  else
    basisSurf = Handle(Surface)::DownCast(S->Copy());

  Handle(Geom_OffsetSurface) O =
    Handle(Geom_OffsetSurface)::DownCast(basisSurf);
  if (!O.IsNull()) 
  {
    Handle(Geom_RectangularTrimmedSurface) S2 = 
           new Geom_RectangularTrimmedSurface( O->BasisSurface(),U1,U2, V1, V2, isUTrim, isVTrim);
    Handle(Geom_OffsetSurface) OS = new Geom_OffsetSurface(S2, O->Offset());
    basisSurf = Handle(Surface)::DownCast(OS);
  }  

  ForceTrim( U1, U2, V1, V2, isUTrim, isVTrim);
}

//=======================================================================
//function : Geom_RectangularTrimmedSurface
//purpose  : 
//=======================================================================

Geom_RectangularTrimmedSurface::Geom_RectangularTrimmedSurface (

const Handle(Surface)& S, 
const Standard_Real             U1, 
const Standard_Real             U2, 
const Standard_Real             V1,
const Standard_Real             V2,
const Standard_Boolean          USense,
const Standard_Boolean          VSense)

: utrim1 (U1),
  vtrim1(V1),
  utrim2 (U2),
  vtrim2 (V2),
  isutrimmed (Standard_True),
  isvtrimmed (Standard_True)
{

  // kill trimmed basis surfaces
  Handle(Geom_RectangularTrimmedSurface) T =
    Handle(Geom_RectangularTrimmedSurface)::DownCast(S);
  if (!T.IsNull())
    basisSurf = Handle(Surface)::DownCast(T->BasisSurface()->Copy());
  else
    basisSurf = Handle(Surface)::DownCast(S->Copy());

  Handle(Geom_OffsetSurface) O =
    Handle(Geom_OffsetSurface)::DownCast(basisSurf);
  if (!O.IsNull()) 
  {
    Handle(Geom_RectangularTrimmedSurface) S2 = 
           new Geom_RectangularTrimmedSurface( O->BasisSurface(),U1,U2, V1, V2, USense, VSense);
    Handle(Geom_OffsetSurface) OS = new Geom_OffsetSurface(S2, O->Offset());
    basisSurf = Handle(Surface)::DownCast(OS);
  }  

  SetTrim( U1, U2, V1, V2, USense, VSense);
}


//=======================================================================
//function : Geom_RectangularTrimmedSurface
//purpose  : 
//=======================================================================

Geom_RectangularTrimmedSurface::Geom_RectangularTrimmedSurface (

 const Handle(Geom_Surface)& S,
 const Standard_Real                  Param1, 
 const Standard_Real                  Param2,
 const Standard_Boolean               UTrim,
 const Standard_Boolean               Sense
) {

  // kill trimmed basis surfaces
  Handle(Geom_RectangularTrimmedSurface) T =
    Handle(Geom_RectangularTrimmedSurface)::DownCast(S);
  if (!T.IsNull())
    basisSurf = Handle(Surface)::DownCast(T->BasisSurface()->Copy());
  else
    basisSurf = Handle(Surface)::DownCast(S->Copy());

  Handle(Geom_OffsetSurface) O =
    Handle(Geom_OffsetSurface)::DownCast(basisSurf);
  if (!O.IsNull()) 
  {
    Handle(Geom_RectangularTrimmedSurface) S2 = 
           new Geom_RectangularTrimmedSurface( O->BasisSurface(),Param1,Param2, UTrim, Sense);
    Handle(Geom_OffsetSurface) OS = new Geom_OffsetSurface(S2, O->Offset());
    basisSurf = Handle(Surface)::DownCast(OS);
  }  

  SetTrim(Param1, Param2, UTrim, Sense);
}

//=======================================================================
//function : SetUTrim
//purpose  : 
//=======================================================================
void Geom_RectangularTrimmedSurface::SetUTrim(const Standard_Real theU1,
                                              const Standard_Real theU2)
{
  const Standard_Real Udeb = utrim1, Ufin = utrim2;

  utrim1 = theU1;
  utrim2 = theU2;

  if ( utrim1 == utrim2)
    Standard_ConstructionError::Raise
                  ("Geom_RectangularTrimmedSurface::U1==U2");

  if (basisSurf->IsUPeriodic())
  {
    ElCLib::AdjustPeriodic(Udeb, Ufin, 
      Min(Abs(utrim2-utrim1)/2,Precision::PConfusion()), utrim1, utrim2); 
  }//if (basisSurf->IsUPeriodic())
  else
  {
    if (utrim1 > utrim2)
    {
      //change some places of theUTrim1 and theUTrim2
      Standard_Real ut = utrim1;
      utrim1 = utrim2;
      utrim2 = ut;
    }

    if (  (Udeb-utrim1 > Precision::PConfusion()) ||
          (utrim2-Ufin > Precision::PConfusion()))
    {
      Standard_ConstructionError::Raise
        ("Geom_RectangularTrimmedSurface::Uparameters out of range");
    }
  }
}

//=======================================================================
//function : SetVTrim
//purpose  : 
//=======================================================================
void Geom_RectangularTrimmedSurface::SetVTrim(const Standard_Real theV1,
                                              const Standard_Real theV2)
{
  const Standard_Real Vdeb = vtrim1, Vfin = vtrim2;

  vtrim1 = theV1;
  vtrim2 = theV2;

  if ( vtrim1 == vtrim2)
    Standard_ConstructionError::Raise
                      ("Geom_RectangularTrimmedSurface::V1==V2");

  if (basisSurf->IsVPeriodic())
  {
    ElCLib::AdjustPeriodic(Vdeb, Vfin,
      Min(Abs(vtrim2-vtrim1)/2,Precision::PConfusion()),vtrim1, vtrim2);
  }//if (basisSurf->IsVPeriodic())
  else
  {
    if (vtrim1 > vtrim2)
    {
      Standard_Real vt = vtrim1;
      vtrim1 = vtrim2;
      vtrim2 = vt;
    }

    if (  (Vdeb-vtrim1 > Precision::PConfusion()) ||
          (vtrim2-Vfin > Precision::PConfusion()))
    {
      Standard_ConstructionError::Raise
         ("Geom_RectangularTrimmedSurface::V parameters out of range");
    }
  }
}

//=======================================================================
//function : ForceTrim
//purpose  : 
//=======================================================================
void Geom_RectangularTrimmedSurface::ForceTrim(const Standard_Real theU1,
                                               const Standard_Real theU2,
                                               const Standard_Real theV1,
                                               const Standard_Real theV2,
                                               const Standard_Boolean isUTrim,
                                               const Standard_Boolean isVTrim)
{
  basisSurf->Bounds(utrim1, utrim2, vtrim1, vtrim2);

  isutrimmed = isUTrim;
  isvtrimmed = isVTrim;

  // Trimming along U-Direction
  if (isutrimmed)
  {
    utrim1 = theU1;
    utrim2 = theU2;

    if((utrim1 > utrim2) || !(basisSurf->IsUPeriodic()))
    {
      //Standard_ConstructionError::Raise
      //  ("Geom_RectangularTrimmedSurface::ForceTrim(...). "
      //                        "utrim1 > utrim2");

      SetUTrim(theU1,theU2);
    }
    else
    {
      const Standard_Real aTolPeriodicFactor = 1.0e-7;
      const Standard_Real aT = basisSurf->UPeriod();
      const Standard_Real aTol = aTolPeriodicFactor * aT;
      if(utrim2 - utrim1 - aT > 2.0*aTol)
      {
        Standard_Integer n = RealToInt((utrim2 - utrim1)/aT);
        utrim2 -= (n * aT);
        
        if(utrim2 - utrim1 < aTol)
          utrim2 += aT;
      }

      //if(utrim2 - utrim1 < aTol)
      //  Standard_ConstructionError::Raise
      //        ("Geom_RectangularTrimmedSurface::SetTrim(...)."
      //              "ERROR in adjust U-parameter!");
    }
  }

  // Trimming along V-Direction
  if (isvtrimmed)
  {
    vtrim1 = theV1;
    vtrim2 = theV2;

    if ((vtrim1 > vtrim2) || !(basisSurf->IsVPeriodic()))
    {
      //Standard_ConstructionError::Raise
      //  ("Geom_RectangularTrimmedSurface::ForceTrim(...). "
      //                        "vtrim1 > vtrim2");

      SetVTrim(theV1,theV2);
    }
    else
    {
      const Standard_Real aTolPeriodicFactor = 1.0e-7;
      const Standard_Real aT = basisSurf->VPeriod();
      const Standard_Real aTol = aTolPeriodicFactor * aT;

      if(vtrim2 - vtrim1 - aT > 2.0*aTol)
      {
        Standard_Integer n = RealToInt((vtrim2 - vtrim1)/aT);
        vtrim2 -= (n * aT);

        if(vtrim2 - vtrim1 < aTol)
          vtrim2 += aT;
      }

      //if(vtrim2 - vtrim1 < aTol)
      //  Standard_ConstructionError::Raise
      //        ("Geom_RectangularTrimmedSurface::SetTrim(...)."
      //              "ERROR in adjust V-parameter!");
    }
  }
}

//=======================================================================
//function : SetTrim
//purpose  : 
//=======================================================================

void Geom_RectangularTrimmedSurface::SetTrim (const Standard_Real    U1, 
					      const Standard_Real    U2, 
					      const Standard_Real    V1,
					      const Standard_Real    V2, 
					      const Standard_Boolean USense, 
					      const Standard_Boolean VSense ) {

  SetTrim( U1, U2, V1, V2, Standard_True, Standard_True, USense, VSense);
}



//=======================================================================
//function : SetTrim
//purpose  : 
//=======================================================================

void Geom_RectangularTrimmedSurface::SetTrim (const Standard_Real    Param1,
					      const Standard_Real    Param2,
					      const Standard_Boolean UTrim, 
					      const Standard_Boolean Sense  ) {

  // dummy arguments to call general SetTrim
  Standard_Real dummy_a = 0.;
  Standard_Real dummy_b = 0.;
  Standard_Boolean dummy_Sense = Standard_True;

  if ( UTrim) {
    SetTrim( Param1        , Param2        , 
	     dummy_a       , dummy_b       ,
	     Standard_True , Standard_False,
	     Sense         , dummy_Sense    );
  }
  else {
    SetTrim( dummy_a       , dummy_b      ,
	     Param1        , Param2       ,
	     Standard_False, Standard_True,
	     dummy_Sense   , Sense         );
  }
}


//=======================================================================
//function : SetTrim
//purpose  : 
//=======================================================================

void Geom_RectangularTrimmedSurface::SetTrim(
                                          const Standard_Real theU1,
                                          const Standard_Real theU2,
                                          const Standard_Real theV1,
                                          const Standard_Real theV2,
                                          const Standard_Boolean isUTrim,
                                          const Standard_Boolean isVTrim,
                                          const Standard_Boolean isUSense,
                                          const Standard_Boolean isVSense)
{
  Standard_Boolean UsameSense = !(isUSense && (theU1 > theU2));
  Standard_Boolean VsameSense = !(isVSense && (theV1 > theV2));

  basisSurf->Bounds(utrim1, utrim2, vtrim1, vtrim2);

  isutrimmed = isUTrim;
  isvtrimmed = isVTrim;

  // Trimming the U-Direction
  if (isutrimmed)
    SetUTrim(theU1,theU2);

  // Trimming the V-Direction
  if (isvtrimmed)
    SetVTrim(theV1,theV2);

  if (!UsameSense)
    UReverse();
  if (!VsameSense)
    VReverse();
}


//=======================================================================
//function : UReverse
//purpose  : 
//=======================================================================

void Geom_RectangularTrimmedSurface::UReverse () 
{
  Standard_Real U1 = basisSurf->UReversedParameter(utrim2);
  Standard_Real U2 = basisSurf->UReversedParameter(utrim1);
  basisSurf->UReverse();
  SetTrim(U1,U2,vtrim1,vtrim2,
	  isutrimmed,isvtrimmed,
	  Standard_True,Standard_True);
}


//=======================================================================
//function : UReversedParameter
//purpose  : 
//=======================================================================

Standard_Real Geom_RectangularTrimmedSurface::UReversedParameter( const Standard_Real U) const {

  return basisSurf->UReversedParameter(U);
}


//=======================================================================
//function : VReverse
//purpose  : 
//=======================================================================

void Geom_RectangularTrimmedSurface::VReverse () 
{
  Standard_Real V1 = basisSurf->VReversedParameter(vtrim2);
  Standard_Real V2 = basisSurf->VReversedParameter(vtrim1);
  basisSurf->VReverse();
  SetTrim(utrim1,utrim2,V1,V2,
	  isutrimmed,isvtrimmed,
	  Standard_True,Standard_True);
}


//=======================================================================
//function : VReversedParameter
//purpose  : 
//=======================================================================

Standard_Real Geom_RectangularTrimmedSurface::VReversedParameter( const Standard_Real V) const {

  return basisSurf->VReversedParameter( V);
}


//=======================================================================
//function : BasisSurface
//purpose  : 
//=======================================================================

Handle(Surface) Geom_RectangularTrimmedSurface::BasisSurface () const
{
  return basisSurf;
}


//=======================================================================
//function : Continuity
//purpose  : 
//=======================================================================

GeomAbs_Shape Geom_RectangularTrimmedSurface::Continuity () const {

  return basisSurf->Continuity();
}


//=======================================================================
//function : D0
//purpose  : 
//=======================================================================

void Geom_RectangularTrimmedSurface::D0
  (const Standard_Real U, const Standard_Real V,
         Pnt& P ) const { 
      
   basisSurf->D0 (U, V, P);
}


//=======================================================================
//function : D1
//purpose  : 
//=======================================================================

void Geom_RectangularTrimmedSurface::D1 
  (const Standard_Real U, const Standard_Real V, 
         Pnt& P, 
         Vec& D1U, Vec& D1V) const {

  basisSurf->D1 (U, V, P, D1U, D1V);
}


//=======================================================================
//function : D2
//purpose  : 
//=======================================================================

void Geom_RectangularTrimmedSurface::D2 
  (const Standard_Real U, const Standard_Real V,
         Pnt& P, 
         Vec& D1U, Vec& D1V, 
         Vec& D2U, Vec& D2V, Vec& D2UV) const {

  basisSurf->D2 (U, V, P, D1U, D1V, D2U, D2V, D2UV);
}


//=======================================================================
//function : D3
//purpose  : 
//=======================================================================

void Geom_RectangularTrimmedSurface::D3 
  (const Standard_Real U, const Standard_Real V, 
   Pnt& P, 
   Vec& D1U, Vec& D1V, 
   Vec& D2U, Vec& D2V, Vec& D2UV, 
   Vec& D3U, Vec& D3V, Vec& D3UUV, Vec& D3UVV) const {

  basisSurf->D3 (U, V, P, D1U, D1V, D2U, D2V, D2UV, D3U, D3V, D3UUV, D3UVV);
}


//=======================================================================
//function : DN
//purpose  : 
//=======================================================================

Vec Geom_RectangularTrimmedSurface::DN 
  (const Standard_Real    U , const Standard_Real    V,
   const Standard_Integer Nu, const Standard_Integer Nv) const {

  return basisSurf->DN (U, V, Nu, Nv);
}


//=======================================================================
//function : Bounds
//purpose  : 
//=======================================================================

void Geom_RectangularTrimmedSurface::Bounds (Standard_Real& U1,
					     Standard_Real& U2, 
					     Standard_Real& V1, 
					     Standard_Real& V2) const {

  U1 = utrim1;  
  U2 = utrim2;  
  V1 = vtrim1; 
  V2 = vtrim2;
}


//=======================================================================
//function : UIso
//purpose  : 
//=======================================================================

Handle(Geom_Curve) Geom_RectangularTrimmedSurface::UIso (const Standard_Real U) const {
  
  Handle(Geom_Curve) C = basisSurf->UIso (U);

  if ( isvtrimmed) {
    Handle(Geom_TrimmedCurve) Ct;
    Ct = new Geom_TrimmedCurve (C, vtrim1, vtrim2, Standard_True);
    return Ct;
  }
  else {
    return C;
  }
}


//=======================================================================
//function : VIso
//purpose  : 
//=======================================================================

Handle(Geom_Curve) Geom_RectangularTrimmedSurface::VIso (const Standard_Real V) const {
 
  Handle(Geom_Curve) C = basisSurf->VIso (V);
  
  if ( isutrimmed) {
    Handle(Geom_TrimmedCurve) Ct;
    Ct = new Geom_TrimmedCurve (C, utrim1, utrim2, Standard_True);
    return Ct;
  }
  else {
    return C;
  }
}


//=======================================================================
//function : IsCNu
//purpose  : 
//=======================================================================

Standard_Boolean Geom_RectangularTrimmedSurface::IsCNu (const Standard_Integer N) const {

  Standard_RangeError_Raise_if (N < 0," ");
  return basisSurf->IsCNu (N);  
}


//=======================================================================
//function : IsCNv
//purpose  : 
//=======================================================================

Standard_Boolean Geom_RectangularTrimmedSurface::IsCNv (const Standard_Integer N) const {

  Standard_RangeError_Raise_if (N < 0," ");
  return basisSurf->IsCNv (N);  
}


//=======================================================================
//function : Transform
//purpose  : 
//=======================================================================

void Geom_RectangularTrimmedSurface::Transform (const Trsf& T) 
{
  basisSurf->Transform (T);
  basisSurf->TransformParameters(utrim1,vtrim1,T);
  basisSurf->TransformParameters(utrim2,vtrim2,T);
}


//=======================================================================
//function : IsUPeriodic
//purpose  : 
// 24/11/98: pmn : Compare la periode a la longeur de l'intervalle
//=======================================================================

Standard_Boolean Geom_RectangularTrimmedSurface::IsUPeriodic () const 
{
  if (basisSurf->IsUPeriodic() &&  !isutrimmed) 
    return Standard_True;
  return Standard_False;
}


//=======================================================================
//function : UPeriod
//purpose  : 
//=======================================================================

Standard_Real Geom_RectangularTrimmedSurface::UPeriod() const
{
  return basisSurf->UPeriod();
}


//=======================================================================
//function : IsVPeriodic
//purpose  : 
//=======================================================================

Standard_Boolean Geom_RectangularTrimmedSurface::IsVPeriodic () const 
{ 
  if (basisSurf->IsVPeriodic() && !isvtrimmed)
    return Standard_True;
  return Standard_False;
}


//=======================================================================
//function : VPeriod
//purpose  : 
//=======================================================================

Standard_Real Geom_RectangularTrimmedSurface::VPeriod() const
{
   return basisSurf->VPeriod(); 
}


//=======================================================================
//function : IsUClosed
//purpose  : 
//=======================================================================

Standard_Boolean Geom_RectangularTrimmedSurface::IsUClosed () const { 

  if (isutrimmed)  
    return Standard_False;
  else             
    return basisSurf->IsUClosed();
}


//=======================================================================
//function : IsVClosed
//purpose  : 
//=======================================================================

Standard_Boolean Geom_RectangularTrimmedSurface::IsVClosed () const { 

  if (isvtrimmed) 
    return Standard_False;
  else   
    return basisSurf->IsVClosed();
}

//=======================================================================
//function : TransformParameters
//purpose  : 
//=======================================================================

void Geom_RectangularTrimmedSurface::TransformParameters(Standard_Real& U,
							 Standard_Real& V,
							 const gp_Trsf& T) 
const
{
  basisSurf->TransformParameters(U,V,T);
}

//=======================================================================
//function : ParametricTransformation
//purpose  : 
//=======================================================================

gp_GTrsf2d Geom_RectangularTrimmedSurface::ParametricTransformation
(const gp_Trsf& T) const
{
  return basisSurf->ParametricTransformation(T);
}

//=======================================================================
//function : GetTrimmedFlags
//purpose  : 
//=======================================================================
void Geom_RectangularTrimmedSurface::GetTrimmedFlags(Standard_Boolean& isU, Standard_Boolean& isV) const
{
  isU = isutrimmed;
  isV = isvtrimmed;
}

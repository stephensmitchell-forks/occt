// Created on: 1997-05-28
// Created by: Xavier BENVENISTE
// Copyright (c) 1997-1999 Matra Datavision
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


#include <Geom2d_BSplineCurve.hxx>
#include <Geom_BSplineCurve.hxx>
#include <GeomLib_Check2dBSplineCurve.hxx>
#include <gp_Pnt2d.hxx>
#include <gp_Vec2d.hxx>
#include <Standard_OutOfRange.hxx>
#include <StdFail_NotDone.hxx>

//=======================================================================
//function : GeomLib_Check2dBSplineCurve
//purpose  : 
//=======================================================================
GeomLib_Check2dBSplineCurve::GeomLib_Check2dBSplineCurve(const Handle(Geom2d_BSplineCurve)& Curve,
                                                         const Standard_Real Tolerance,
                                                         const Standard_Real AngularTolerance,
                                                         const gp_Pnt2d& FirstPnt,
                                                         const gp_Pnt2d& SecondPnt,
                                                         const gp_Pnt2d& PrelastPnt,
                                                         const gp_Pnt2d& LastPnt)
:myCurve(Curve),
myDone(Standard_False),
myFixFirstTangent(Standard_False),
myFixLastTangent(Standard_False),
myAngularTolerance(Abs(AngularTolerance)),
myTolerance(Abs(Tolerance))
{
  const Standard_Real CrossProdTol = 1.e-6;
  
  myIndSecondPole = myIndPrelastPole = -1;
  
  Standard_Integer ii,
    num_poles ;

  num_poles = myCurve->NbPoles() ;
  
  if (!myCurve->IsPeriodic() && num_poles >= 4)
  {
    //Around first point
    gp_Vec2d FirstVec(FirstPnt, SecondPnt), FirstNormVec;
    Standard_Real FirstVecLength = FirstVec.Magnitude();
    if (FirstVecLength > gp::Resolution())
      FirstNormVec = FirstVec / FirstVecLength;
    Standard_Boolean AreParallel = Standard_True;

    for (ii = 2; ii <= num_poles-1; ii++)
    {
      gp_Pnt2d aPnt = myCurve->Pole(ii);
      gp_Vec2d aVec(FirstPnt, aPnt);
      Standard_Real ScalProd = FirstVec * aVec;
      if (ScalProd > 0.)
      {
        myIndSecondPole = ii;
        break;
      }
      else
      {
        Standard_Real aVecLength = aVec.Magnitude();
        if (FirstVecLength > gp::Resolution() &&
            aVecLength > gp::Resolution())
        {
          aVec /= aVecLength;
          Standard_Real CrossProd = FirstNormVec ^ aVec;
          if (Abs(CrossProd) > CrossProdTol)
          {
            AreParallel = Standard_False;
            break;
          }
        }
      }
    }

    if (myIndSecondPole > 2 && AreParallel)
      myFixFirstTangent = Standard_True;

    //Around last point;
    gp_Vec2d LastVec(LastPnt, PrelastPnt), LastNormVec;
    Standard_Real LastVecLength = LastVec.Magnitude();
    if (LastVecLength > gp::Resolution())
      LastNormVec = LastVec / LastVecLength;
    AreParallel = Standard_True;

    for (ii = num_poles-1; ii >= 2; ii--)
    {
      gp_Pnt2d aPnt = myCurve->Pole(ii);
      gp_Vec2d aVec(LastPnt, aPnt);
      Standard_Real ScalProd = LastVec * aVec;
      if (ScalProd > 0.)
      {
        myIndPrelastPole = ii;
        break;
      }
      else
      {
        Standard_Real aVecLength = aVec.Magnitude();
        if (LastVecLength > gp::Resolution() &&
            aVecLength > gp::Resolution())
        {
          aVec /= aVecLength;
          Standard_Real CrossProd = LastNormVec ^ aVec;
          if (Abs(CrossProd) > CrossProdTol)
          {
            AreParallel = Standard_False;
            break;
          }
        }
      }
    }

    if (myIndPrelastPole < num_poles-1 && AreParallel)
      myFixLastTangent = Standard_True;
  }
}

//=======================================================================
//function : GeomLib_Check2dBSplineCurve
//purpose  : 
//=======================================================================
GeomLib_Check2dBSplineCurve::GeomLib_Check2dBSplineCurve(const Handle(Geom2d_BSplineCurve)& Curve,
                                                         const Standard_Real Tolerance,
                                                         const Standard_Real AngularTolerance)
:myCurve(Curve),
myDone(Standard_False),
myFixFirstTangent(Standard_False),
myFixLastTangent(Standard_False),
myAngularTolerance(Abs(AngularTolerance)),
myTolerance(Abs(Tolerance)),
myFirstPole(1.0,0.0e0),
myLastPole(1.0e0,0.0e0)
{
  myIndSecondPole = myIndPrelastPole = -1;

  Standard_Integer ii,
    num_poles ;
  Standard_Real tangent_magnitude,
    value,
    angular_value,
    factor,
    vector_magnitude ;
  num_poles = Curve->NbPoles() ;
  if (( ! myCurve->IsPeriodic() )&& num_poles >= 4) {
    
    gp_Vec2d tangent,
      diff,
      a_vector;
    for (ii = 1 ; ii <= 2 ; ii++) {
      tangent.SetCoord(ii,myCurve->Pole(2).Coord(ii) - myCurve->Pole(1).Coord(ii))  ;
      a_vector.SetCoord(ii, myCurve->Pole(3).Coord(ii) - myCurve->Pole(1).Coord(ii)) ;
    }
    tangent_magnitude = tangent.Magnitude() ;
    vector_magnitude = a_vector.Magnitude() ;
    if (tangent_magnitude > myTolerance &&
	vector_magnitude > myTolerance)
      {
        value = tangent.Dot(a_vector) ;
        if ( value < 0.0e0) {
	  for (ii = 1 ; ii <= 2 ; ii++) {
	    diff.SetCoord(ii, (tangent.Coord(ii) / tangent_magnitude) + (a_vector.Coord(ii) / vector_magnitude)) ; 
          } 
	  angular_value = 
	    diff.Magnitude() ;
          if (angular_value < myAngularTolerance) {
	    myFixFirstTangent = Standard_True ;
	    factor = 1.0e0 ;
	    if (tangent_magnitude > 0.5e0 * vector_magnitude) {
	      factor = 0.5e0 *  vector_magnitude / tangent_magnitude ;
	    }
	    for (ii = 1 ; ii <= 2 ; ii++) {
	      myFirstPole.SetCoord(ii, myCurve->Pole(1).Coord(ii)  - factor * tangent.Coord(ii))  ;
	    }
          }
	  
	}
      }
    for (ii = 1 ; ii <= 2 ; ii++) {
      tangent.SetCoord(ii,myCurve->Pole(num_poles-1).Coord(ii) - myCurve->Pole(num_poles).Coord(ii))  ;
      a_vector.SetCoord(ii, myCurve->Pole(num_poles-2).Coord(ii) - myCurve->Pole(num_poles).Coord(ii)) ;
    }
    tangent_magnitude = tangent.Magnitude() ;
    vector_magnitude = a_vector.Magnitude() ;
    
    if (tangent_magnitude > myTolerance &&
	vector_magnitude > myTolerance)
      {
        value = tangent.Dot(a_vector) ;
	if (value < 0.0e0) {
	  for (ii = 1 ; ii <= 2 ; ii++) {
	    diff.SetCoord(ii, (tangent.Coord(ii) / tangent_magnitude) + (a_vector.Coord(ii) / vector_magnitude)) ; 
          } 
	  angular_value = 
	    diff.Magnitude() ;
	  if ( angular_value < myAngularTolerance) {
	    myFixLastTangent = Standard_True ;
	    factor = 1.0e0 ;
	    if (tangent_magnitude > 0.5e0 * vector_magnitude) {
	      factor = 0.5e0 *  vector_magnitude / tangent_magnitude ;
	    }
	    for (ii = 1 ; ii <= 2 ; ii++) {
	      myLastPole.SetCoord(ii, myCurve->Pole(num_poles).Coord(ii)  - factor * tangent.Coord(ii))  ;
	    }
	  }
	  
        }
      }
    
  }
  else {
    myDone = Standard_True ;
  }
}
  
//=======================================================================
//function : NeedTangentFix
//purpose  : 
//=======================================================================

void GeomLib_Check2dBSplineCurve::NeedTangentFix(Standard_Boolean & FirstFlag,
					       Standard_Boolean & LastFlag) const 
{
  FirstFlag = myFixFirstTangent ;
  LastFlag = myFixLastTangent ;
}
//=======================================================================
//function : FixTangent
//purpose  : 
//=======================================================================

Handle(Geom2d_BSplineCurve)  GeomLib_Check2dBSplineCurve::FixedTangent(const Standard_Boolean FirstFlag,
								 const Standard_Boolean LastFlag)
{ 
  Handle(Geom2d_BSplineCurve) new_curve ;
  if ((myFixFirstTangent && FirstFlag) ||(myFixLastTangent && LastFlag)) {
    new_curve =
      Handle(Geom2d_BSplineCurve)::DownCast(myCurve->Copy()) ;
    
  }
  if (myFixFirstTangent && FirstFlag) {
    new_curve->SetPole(2,
		     myFirstPole) ;
  }
  if (myFixLastTangent && LastFlag) {
    Standard_Integer num_poles = myCurve->NbPoles() ;
    new_curve->SetPole(num_poles-1,
		     myLastPole) ;
  }
  
  myDone = Standard_True ;
  return new_curve ;
}				   
//=======================================================================
//function : FixTangent
//purpose  : 
//=======================================================================

void GeomLib_Check2dBSplineCurve::FixTangent(const Standard_Boolean FirstFlag,
								 const Standard_Boolean LastFlag)
{ 

  if (myFixFirstTangent && FirstFlag) {
    if (myIndSecondPole == -1)
      myCurve->SetPole(2,
                       myFirstPole) ;
    else
    {
      gp_XY XY1 = myCurve->Pole(1).XY();
      gp_XY XY2 = myCurve->Pole(myIndSecondPole).XY();
      Standard_Real NbSamples = myIndSecondPole - 1;
      for (Standard_Integer i = 2; i < myIndSecondPole; i++)
      {
        Standard_Real ii = i-1;
        gp_Pnt2d aNewPole((1. - ii/NbSamples)*XY1 + ii/NbSamples*XY2);
        myCurve->SetPole(i, aNewPole);
      }
    }
  }
  if (myFixLastTangent && LastFlag) {
    Standard_Integer num_poles = myCurve->NbPoles() ;

    if (myIndPrelastPole == -1)
      myCurve->SetPole(num_poles-1,
                       myLastPole) ;
    else
    {
      gp_XY XY1 = myCurve->Pole(num_poles).XY();
      gp_XY XY2 = myCurve->Pole(myIndPrelastPole).XY();
      Standard_Real NbSamples = num_poles - myIndPrelastPole;
      for (Standard_Integer i = num_poles-1; i > myIndPrelastPole; i--)
      {
        Standard_Real ii = num_poles-i;
        gp_Pnt2d aNewPole((1. - ii/NbSamples)*XY1 + ii/NbSamples*XY2);
        myCurve->SetPole(i, aNewPole);
      }
    }
  }
  
  myDone = Standard_True ;
}				   

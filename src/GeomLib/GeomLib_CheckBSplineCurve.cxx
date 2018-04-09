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


#include <Geom_BSplineCurve.hxx>
#include <GeomLib_CheckBSplineCurve.hxx>
#include <gp_Pnt.hxx>
#include <gp_Vec.hxx>
#include <Standard_OutOfRange.hxx>
#include <StdFail_NotDone.hxx>

//=======================================================================
//function : GeomLib_CheckBSplineCurve
//purpose  : 
//=======================================================================
GeomLib_CheckBSplineCurve::GeomLib_CheckBSplineCurve(const Handle(Geom_BSplineCurve)& Curve,
						     const Standard_Real Tolerance,
						     const Standard_Real AngularTolerance,
                                                     const gp_Pnt& FirstPnt,
                                                     const gp_Pnt& SecondPnt,
                                                     const gp_Pnt& PrelastPnt,
                                                     const gp_Pnt& LastPnt)
 :myCurve(Curve),
  myDone(Standard_False),
  myFixFirstTangent(Standard_False),
  myFixLastTangent(Standard_False),
  myAngularTolerance(Abs(AngularTolerance)),
  myTolerance(Abs(Tolerance))
{
  const Standard_Real CrossProdSqTol = 1.e-12;
  
  myIndSecondPole = myIndPrelastPole = -1;
  
  Standard_Integer ii,
    num_poles ;

  num_poles = myCurve->NbPoles() ;
  
  if (!myCurve->IsPeriodic() && num_poles >= 4)
  {
    //Around first point
    gp_Vec FirstVec(FirstPnt, SecondPnt), FirstNormVec;
    Standard_Real FirstVecLength = FirstVec.Magnitude();
    if (FirstVecLength > gp::Resolution())
      FirstNormVec = FirstVec / FirstVecLength;
    Standard_Boolean AreParallel = Standard_True;

    for (ii = 2; ii <= num_poles-1; ii++)
    {
      gp_Pnt aPnt = myCurve->Pole(ii);
      gp_Vec aVec(FirstPnt, aPnt);
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
          gp_Vec CrossProd = FirstNormVec ^ aVec;
          Standard_Real CrossProdSqLength = CrossProd.SquareMagnitude();
          if (CrossProdSqLength > CrossProdSqTol)
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
    gp_Vec LastVec(LastPnt, PrelastPnt), LastNormVec;
    Standard_Real LastVecLength = LastVec.Magnitude();
    if (LastVecLength > gp::Resolution())
      LastNormVec = LastVec / LastVecLength;
    AreParallel = Standard_True;

    for (ii = num_poles-1; ii >= 2; ii--)
    {
      gp_Pnt aPnt = myCurve->Pole(ii);
      gp_Vec aVec(LastPnt, aPnt);
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
          gp_Vec CrossProd = LastNormVec ^ aVec;
          Standard_Real CrossProdSqLength = CrossProd.SquareMagnitude();
          if (CrossProdSqLength > CrossProdSqTol)
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
//function : GeomLib_CheckBSplineCurve
//purpose  : 
//=======================================================================
GeomLib_CheckBSplineCurve::GeomLib_CheckBSplineCurve(const Handle(Geom_BSplineCurve)& Curve,
						     const Standard_Real Tolerance,
						     const Standard_Real AngularTolerance)
:myCurve(Curve),
myDone(Standard_False),
myFixFirstTangent(Standard_False),
myFixLastTangent(Standard_False),
myAngularTolerance(Abs(AngularTolerance)),
myTolerance(Abs(Tolerance)),
myFirstPole(1.0,0.0e0,0.e0),
myLastPole(1.0e0,0.0e0,0.e0)
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
    
    gp_Vec tangent,
      diff,
      a_vector;
    for (ii = 1 ; ii <= 3 ; ii++) {
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
	  for (ii = 1 ; ii <= 3 ; ii++) {
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
	    for (ii = 1 ; ii <= 3 ; ii++) {
	      myFirstPole.SetCoord(ii, myCurve->Pole(1).Coord(ii)  - factor * tangent.Coord(ii))  ;
	    }
          }
	  
	}
      }
    for (ii = 1 ; ii <= 3 ; ii++) {
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
	  for (ii = 1 ; ii <= 3 ; ii++) {
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
	    for (ii = 1 ; ii <= 3 ; ii++) {
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

void GeomLib_CheckBSplineCurve::NeedTangentFix(Standard_Boolean & FirstFlag,
					       Standard_Boolean & LastFlag) const 
{
  FirstFlag = myFixFirstTangent ;
  LastFlag = myFixLastTangent ;
}
//=======================================================================
//function : FixTangent
//purpose  : 
//=======================================================================

Handle(Geom_BSplineCurve)  GeomLib_CheckBSplineCurve::FixedTangent(const Standard_Boolean FirstFlag,
								 const Standard_Boolean LastFlag)
{ 
  Handle(Geom_BSplineCurve) new_curve ;
  if ((myFixFirstTangent && FirstFlag) ||(myFixLastTangent && LastFlag)) {
    new_curve =
      Handle(Geom_BSplineCurve)::DownCast(myCurve->Copy()) ;
    
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

void GeomLib_CheckBSplineCurve::FixTangent(const Standard_Boolean FirstFlag,
					   const Standard_Boolean LastFlag)
{ 
  
  if (myFixFirstTangent && FirstFlag) {
    if (myIndSecondPole == -1)
      myCurve->SetPole(2,
                       myFirstPole) ;
    else
    {
      gp_XYZ XYZ1 = myCurve->Pole(1).XYZ();
      gp_XYZ XYZ2 = myCurve->Pole(myIndSecondPole).XYZ();
      Standard_Real NbSamples = myIndSecondPole - 1;
      for (Standard_Integer i = 2; i < myIndSecondPole; i++)
      {
        Standard_Real ii = i-1;
        gp_Pnt aNewPole((1. - ii/NbSamples)*XYZ1 + ii/NbSamples*XYZ2);
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
      gp_XYZ XYZ1 = myCurve->Pole(num_poles).XYZ();
      gp_XYZ XYZ2 = myCurve->Pole(myIndPrelastPole).XYZ();
      Standard_Real NbSamples = num_poles - myIndPrelastPole;
      for (Standard_Integer i = num_poles-1; i > myIndPrelastPole; i--)
      {
        Standard_Real ii = num_poles-i;
        gp_Pnt aNewPole((1. - ii/NbSamples)*XYZ1 + ii/NbSamples*XYZ2);
        myCurve->SetPole(i, aNewPole);
      }
    }
  }
  
  myDone = Standard_True ;
}				   

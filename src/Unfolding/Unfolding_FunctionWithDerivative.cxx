// File:	Unfolding_FunctionWithDerivative.cxx
// Created:	Fri Sep  5 17:51:58 2008
// Author:	Mikhail KLOKOV
//		<mkk@kurox>


#include <gp_Dir2d.hxx>
#include <gp_Trsf2d.hxx>
#include <TColgp_Array1OfXY.hxx>
#include <Unfolding_FunctionWithDerivative.hxx>

static Standard_Boolean ComputeArea(const TColgp_Array1OfXY& theMasterPolyLine,
				    const TColgp_Array1OfXY& theSlavePolyLine,
				    const gp_Dir2d&            theShiftDir,
				    const Standard_Real&     theShift,
				    const gp_Trsf2d          theTrsf,
				    Standard_Real&           theArea);


static Standard_Boolean ComputeAreaDeriv(const TColgp_Array1OfXY& theMasterPolyLine,
					 const TColgp_Array1OfXY& theSlavePolyLine,
					 const gp_Dir2d&          theShiftDir,
					 const Standard_Real&     theShift,
					 const gp_Trsf2d          theTrsf,
					 Standard_Real&           theArea,
					 Standard_Real&           theDeriv);


//=============================================================================
// function: Constructor
//=============================================================================
Unfolding_FunctionWithDerivative::Unfolding_FunctionWithDerivative(const TColgp_Array1OfXY& theMaster,
								   const TColgp_Array1OfXY& theSlave,
								   const gp_Dir2d& theDir,
								   const gp_Trsf2d& theTrsf):
myMasterPolyLine(theMaster.Lower(), theMaster.Upper()),
mySlavePolyLine(theSlave.Lower(), theSlave.Upper())
{
 myMasterPolyLine.Assign(theMaster);
 mySlavePolyLine.Assign(theSlave);
 myTrsf = theTrsf;
 myShiftDir = theDir;
 myShift = 0.;
 myArea = 0.;
}


//=============================================================================
// function: Value
//=============================================================================
Standard_Boolean Unfolding_FunctionWithDerivative::Value(const Standard_Real X,Standard_Real& F) 
{
  myShift = X;
  F = 0.;
  Standard_Real A = 0.;
  Standard_Real D = 0.;
  if ( !ComputeAreaDeriv(myMasterPolyLine, mySlavePolyLine, myShiftDir, myShift, myTrsf, A, D) )
    return Standard_False;

  myArea = A;

  gp_XY aP1P2(0, A);
  gp_XY vecD(1,D);
  Standard_Real modD = vecD.Modulus();
  if ( modD < gp::Resolution() ) {
    Standard_Real delta = 1.e-7;
    if ( !ComputeArea(myMasterPolyLine, mySlavePolyLine, myShiftDir, myShift - delta, myTrsf, A) )
      return Standard_False;
    gp_XY P1(0,A);
    if ( !ComputeArea(myMasterPolyLine, mySlavePolyLine, myShiftDir, myShift + delta, myTrsf, A) )
      return Standard_False;
    gp_XY P2(0,A);
    gp_XY V = P2-P1;
    modD = V.Modulus();
    if ( modD < gp::Resolution() )
      return Standard_False;
  }
  F = aP1P2 * vecD;
  F /= modD;

  return Standard_True;
}

//=============================================================================
// function: Derivative
//=============================================================================
Standard_Boolean Unfolding_FunctionWithDerivative::Derivative(const Standard_Real X,Standard_Real& D) 
{
  Standard_Real F = 0.;
  return Values(X,F,D);
}

//=============================================================================
// function: Values
//=============================================================================
Standard_Boolean Unfolding_FunctionWithDerivative::Values(const Standard_Real X,Standard_Real& F,Standard_Real& D) 
{
  Standard_Real F1 = 0., F2 = 0.;
  Standard_Real aDelta = 1.e-05;
  if ( fabs(X) > aDelta )
    aDelta = fabs(X) * aDelta;
  if ( !Value(X + aDelta, F2) )
    return Standard_False;

  if ( !Value(X, F1) )
    return Standard_False;

  myShift = X;
  F = F1;
  D = F2 - F1;
  D /= aDelta;

  return Standard_True;
}

//=============================================================================
// function: GetStateNumber
//=============================================================================
Standard_Integer Unfolding_FunctionWithDerivative::GetStateNumber() 
{
  ComputeArea(myMasterPolyLine, mySlavePolyLine, myShiftDir, myShift, myTrsf, myArea);
  return 0;
}

//=============================================================================
// function: Area
//=============================================================================
Standard_Real Unfolding_FunctionWithDerivative::Area() const
{
  return myArea;
}

//-----------------------------------------------------------------------------
// function: ComputeAreaDeriv
//-----------------------------------------------------------------------------
static Standard_Boolean ComputeAreaDeriv(const TColgp_Array1OfXY& theMasterPolyLine,
					 const TColgp_Array1OfXY& theSlavePolyLine,
					 const gp_Dir2d&          theShiftDir,
					 const Standard_Real&     theShift,
					 const gp_Trsf2d          theTrsf,
					 Standard_Real&           theArea,
					 Standard_Real&           theDeriv)
{
  theArea = 0.;
  theDeriv = 0.;
  Standard_Real X = theShift;
  Standard_Real F1 = 0., F2 = 0.;
  if ( !ComputeArea(theMasterPolyLine, theSlavePolyLine, theShiftDir, X, theTrsf, F1) )
    return Standard_False;
  theArea = F1;
  Standard_Real aDelta = 1.e-05;
  if ( fabs(X) > aDelta )
    aDelta = fabs(X) * aDelta;
  if ( !ComputeArea(theMasterPolyLine, theSlavePolyLine, theShiftDir, X + aDelta, theTrsf, F2) )
    return Standard_False;

  theDeriv = F2 - F1;
  theDeriv /= aDelta;
  return Standard_True;
}

//-----------------------------------------------------------------------------
// function: ComputeArea
//-----------------------------------------------------------------------------
static Standard_Boolean ComputeArea(const TColgp_Array1OfXY& theMasterPolyLine,
				    const TColgp_Array1OfXY& theSlavePolyLine,
				    const gp_Dir2d&            theShiftDir,
				    const Standard_Real&     theShift,
				    const gp_Trsf2d          theTrsf,
				    Standard_Real&           theArea)
{
  if ( ( theMasterPolyLine.Length() < 2 ) || ( theSlavePolyLine.Length() < 2 ) ||
      ( theSlavePolyLine.Length() != theMasterPolyLine.Length() ) )
    return Standard_False;
  theArea = 0.;

  gp_Vec2d aTranslation( theShiftDir.XY() * theShift );

  gp_Trsf2d aTransTrsf;
  aTransTrsf.SetTranslation( aTranslation );
  gp_Trsf2d aTrans;
  aTrans = aTransTrsf * theTrsf;

  //
  TColgp_Array1OfXY aTransformedSlave(theSlavePolyLine.Lower(), theSlavePolyLine.Upper());
  Standard_Integer i = 0;
  for ( i = aTransformedSlave.Lower(); i <= aTransformedSlave.Upper(); i++ ) {
    gp_Pnt2d aP( theSlavePolyLine.Value( i ) );
    aP.Transform( aTrans );
    aTransformedSlave.SetValue( i, aP.XY() );
  }
  //

  // compute area
  // This method assumes that the polylines shape is similar
  // if the shape is completely different, the compute area will be wrong.
  // But it will grow, that can be considered as a condition of distortion.
  // So, the method can be acceptable.
  gp_XY aFirstPoint = aTransformedSlave( aTransformedSlave.Lower() );
  aFirstPoint += theMasterPolyLine( theMasterPolyLine.Lower() );
  aFirstPoint *= 0.5;

  gp_XY aLastPoint = aTransformedSlave( aTransformedSlave.Upper() );
  aLastPoint += theMasterPolyLine( theMasterPolyLine.Upper() );
  aLastPoint *= 0.5;
  Standard_Integer nIndexDif = theMasterPolyLine.Lower() - aTransformedSlave.Lower();

  gp_XY aP1 = aFirstPoint;
  gp_XY aP3 = aFirstPoint;

  for ( i = theMasterPolyLine.Lower() + 1; i <= theMasterPolyLine.Upper(); i++ ) {
    gp_XY aP2 = theMasterPolyLine(i);
    gp_XY aP4 = aTransformedSlave(i-nIndexDif);
    if ( i == theMasterPolyLine.Upper() ) {
      aP2 = aLastPoint;
      aP4 = aLastPoint;
    }
    gp_XY aP1P4 = aP4 - aP1;
    gp_XY aP1P2 = aP2 - aP1;
    gp_XY aP1P3 = aP3 - aP1;
    Standard_Real aprod1 = aP1P4 ^ aP1P3;
    Standard_Real aprod2 = aP1P4 ^ aP1P2;
    if ( aprod1 * aprod2 <= 0. ) {
      theArea += 0.5 * ( fabs( aprod1 ) + fabs( aprod2 ) );
    }
    else {
      gp_XY aP3P2 = aP2 - aP3;
      gp_XY aP3P4 = aP4 - aP3;
      gp_XY aP3P1 = -aP1P3;

      Standard_Real asum = 0.5 * ( fabs( aprod1 ) + fabs( aprod2 ) );
      aprod1 = aP3P2 ^ aP3P4;
      aprod2 = aP3P2 ^ aP3P1;
      if ( aprod1 * aprod2 <= 0. ) {
	theArea += 0.5 * ( fabs( aprod1 ) + fabs( aprod2 ) );
      }
      else {
	asum += 0.5 * ( fabs( aprod1 ) + fabs( aprod2 ) );
	asum *= 0.5;
	theArea += asum;
      }
    }

    aP1 = aP2;
    aP3 = aP4;
  }

  return Standard_True;
}

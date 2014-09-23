// File:        Poly_Segment2d.cxx
// Created:     01.07.2010
// Author:      Sergey KUUL
// Copyright:   Open Cascade 2010

#include <Poly_Segment2d.hxx>
#include <Poly_PolyCurve2d.hxx>
#include <Precision.hxx>

//=======================================================================
//function : Dir
//purpose  : 
//=======================================================================

gp_XY Poly_Segment2d::Dir() const
{
  gp_XY aResult = myPnt[1] - myPnt[0];
  const Standard_Real aMagnitude = aResult.Modulus();
  if ( aMagnitude > Precision::Confusion() )
    aResult /= aMagnitude;
  return aResult;
}


//=======================================================================
//function : Dir
//purpose  : 
//=======================================================================
Standard_Boolean Poly_Segment2d::GetNormal(gp_XYZ& theDir) const
{
  theDir = myNormal;
  return theDir.SquareModulus() > Precision::Confusion()*Precision::Confusion();
}


//=======================================================================
//function : Restore3D
//purpose  : 
//=======================================================================
/*
void Poly_Segment::Restore3d (const Poly_SectionType theType,
                              const Standard_Real theAbsc,
                              gp_XYZ& theP0,
                              gp_XYZ& theP1) const
{
  if (theType == Poly_ST_YZ)
  {
    theP0.SetCoord (theAbsc, myPnt[0].X(), myPnt[0].Y());
    theP1.SetCoord (theAbsc, myPnt[1].X(), myPnt[1].Y());
  }
  else if (theType == Poly_ST_ZX)
  {
    theP0.SetCoord (myPnt[0].Y(), theAbsc, myPnt[0].X());
    theP1.SetCoord (myPnt[1].Y(), theAbsc, myPnt[1].X());
  }
  else
  {
    theP0.SetCoord (myPnt[0].X(), myPnt[0].Y(), theAbsc);
    theP1.SetCoord (myPnt[1].X(), myPnt[1].Y(), theAbsc);
  }
}
*/

//=======================================================================
//function : Interpolate
//purpose  : 
//=======================================================================

void Poly_Segment2d::Interpolate(const Standard_Real theParam,
                                 gp_XY& thePoint) const
{
  //if (IsPositive(theParam) == Standard_False)
  if ( theParam < Precision::Confusion() )
    thePoint = myPnt[0];
  //else if (IsPositive(1.-theParam) == Standard_False)
  else if ( (theParam - 1.) > -Precision::Confusion() )
    thePoint = myPnt[1];
  else
    thePoint = myPnt[0] * (1.-theParam) + myPnt[1] * theParam;
}

//=======================================================================
//function : Split
//purpose  : 
//=======================================================================
/*
Standard_Boolean Poly_Segment2d::Split(Poly_Segment2d&        theRes,
                                       const Standard_Real    theAbsc,
                                       const Standard_Boolean isFirst,
                                       const Standard_Integer theTimer) const
{
  theRes = * this;
  theRes.SetTimer (theTimer);
  return theRes.Delimit (theAbsc, isFirst);
}
*/

//=======================================================================
//function : Delimit
//purpose  : Similar to Split() but the result consists in modification of
//           the current Segment instance, rather than creation of a new one.
//=======================================================================
 /*
Standard_Boolean Poly_Segment2d::Delimit(const Standard_Real theAbsc,
                                         const Standard_Boolean isFirst)
{
  Standard_Boolean aResult (Standard_True);
  if (!IsPositive(theAbsc)) {
    if (isFirst)
      aResult = Standard_False;
  } else if (!IsPositive(1.-theAbsc)) {
    if (isFirst == Standard_False)
      aResult = Standard_False;
  } else {
    if (isFirst)
    {
      myPnt[1] = (1.-theAbsc) * myPnt[0] + theAbsc * myPnt[1];
      SetVertex (1, NULL);
    }
    else
    {
      myPnt[0] = (1.-theAbsc) * myPnt[0] + theAbsc * myPnt[1];
      SetVertex (0, NULL);
    }
  }
  return aResult;
}
*/

//=======================================================================
//function : Divide
//purpose  : 
//=======================================================================

Standard_Boolean Poly_Segment2d::Divide (const Standard_Real theParam)
{
  if (myCurve == NULL ||
      (theParam < Precision::Confusion()) || 
      ((1.-theParam)< Precision::Confusion()) )
    return Standard_False;

  // compute middle point
  gp_XY aPnt = (1.-theParam) * myPnt[0] + theParam * myPnt[1];

  // create new segment between the middle and the end
  Poly_Segment2d* aNewSeg =
    new (myCurve->Allocator()) Poly_Segment2d (aPnt, myPnt[1]);
  aNewSeg->SetCurve (myCurve);
  aNewSeg->CopyNormal (*this);
  aNewSeg->myInternal = myInternal;

  // make connections to insert in the curve
  aNewSeg->myPrevious = this;
  aNewSeg->myNext = myNext;
  const_cast<Poly_Segment2d*>(myNext)->myPrevious = aNewSeg;
  myNext = aNewSeg;
  myCurve->myNSegments++;
  myCurve->myIsModified = Standard_True;
  return Standard_True;
}


//=======================================================================
//function : Project
//purpose  : Projection of a point on the Segment.
//=======================================================================

Standard_Real Poly_Segment2d::Project(const gp_XY& thePnt,
                                      const Standard_Boolean theRestrict) const
{
  Standard_Real aResult (0.);
  const gp_XY aDelta = myPnt[1] - myPnt[0];
  const Standard_Real aLen2 = aDelta.SquareModulus();
  if ( aLen2 > Precision::Confusion() ) {
    aResult = (aDelta * (thePnt - myPnt[0])) / aLen2;
    if (theRestrict) {
      if (aResult < 0.)
        aResult = 0.;
      else if (aResult > 1.)
        aResult = 1.;
    }
  }
  return aResult;
}

//=======================================================================
//function : ~Poly_Segment
//purpose  : 
//=======================================================================

Poly_Segment2d::~Poly_Segment2d()
{
  //SetVertex (0, NULL);
  //SetVertex (1, NULL);
}

//=======================================================================
//function : SetVertex
//purpose  : 
//=======================================================================
/*
void Poly_Segment::SetVertex (const int theI,
                                 Poly_Vertex* theVertex)
{
  Poly_Vertex*& aVert = myVertex[theI & 0x1];
  if (aVert == theVertex)
    return; // no changes
  if (aVert)
  {
    aVert->removeSegment (this);
    aVert = NULL;
  }
  if (theVertex)
  {
    Standard_Boolean isError = theVertex == myVertex[1 - (theI & 0x1)];
    if (isError)
    {
      Poly_ASSERT (!isError, "Both ends of segment refer to the same vertex");
      return;
    }
    if (theVertex->addSegment (this))
      aVert = theVertex;
  }
}
*/

//=======================================================================
//function : SetFeaturePoint
//purpose  : 
//=======================================================================
/*
void Poly_Segment::SetFeaturePoint (const Standard_Real theParam)
{
  myFeatParam = -1;
  if (theParam < 0. || theParam > 1.)
    return;

  // myFeatParam is 8-bit integer, so we code a positive integer from 0 to 127
  myFeatParam = int(theParam * 127.) & 0xff;
}
*/

//=======================================================================
//function : Dump
//purpose  : 
//=======================================================================

void Poly_Segment2d::Dump (Standard_OStream& theStream) const
{
  theStream << this << " ";
  /*
  if (myCurve) {
    theStream << myCurve << " ";
    if (myCurve->myZBuffer) {
      Poly_ZBuffer* aZBuf = myCurve->myZBuffer;
      const char* aAbscName[] = {"Undef", "Z", "X", "Y"};
      Standard_Integer aType = (Standard_Integer) aZBuf->GetType();
      theStream << aAbscName[aType] << "=" << aZBuf->GetY() << " ";
    }
  }
  */
  theStream << myPnt[0].X() << " " << myPnt[0].Y() << " - "
    << myPnt[1].X() << " " << myPnt[1].Y();
  //if (myVertex[0])
  //  theStream << " v0=" << myVertex[0];
  //if (myVertex[1])
  //  theStream << " v1=" << myVertex[1];
  if (myInternal)
    theStream << " Int";
  theStream << endl;
}


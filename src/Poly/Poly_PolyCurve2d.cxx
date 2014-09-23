// File:        Poly_PolyCurve2d.cxx
// Created:     01.07.2010
// Author:      Sergey KUUL
// Copyright:   Open Cascade 2010

#include <Poly_PolyCurve2d.hxx>
#include <Poly_Segment2d.hxx>

#include <Bnd_B2d.hxx>
#include <NCollection_Vector.hxx>
#include <Precision.hxx>

#ifdef WNT
#pragma warning(disable:4996)
#endif

IMPLEMENT_STANDARD_HANDLE (Poly_PolyCurve2d, Standard_Transient)
IMPLEMENT_STANDARD_RTTIEXT(Poly_PolyCurve2d, Standard_Transient)

//=======================================================================
//function : Create
//purpose  : Method replacing both "operator new" and constructor.
//=======================================================================

Poly_PolyCurve2d* Poly_PolyCurve2d::Create
        (const Handle_NCollection_BaseAllocator& theAlloc,
         const CurveType                         theType)
{
  Handle(NCollection_BaseAllocator) anAlloc = theAlloc;
  if (theAlloc.IsNull())
    anAlloc = NCollection_BaseAllocator::CommonBaseAllocator();
  return new (anAlloc) Poly_PolyCurve2d(anAlloc, theType);
}

//=======================================================================
//function : Delete
//purpose  : 
//=======================================================================

void Poly_PolyCurve2d::Delete () const
{
  Poly_PolyCurve2d * meCurve = const_cast<Poly_PolyCurve2d*>(this);
  const Handle(NCollection_BaseAllocator) anAlloc = myAllocator;
  meCurve->~Poly_PolyCurve2d();
  anAlloc->Free(meCurve);
}

//=======================================================================
//function : ~Poly_PolyCurve2d
//purpose  : Destructor
//=======================================================================

Poly_PolyCurve2d::~Poly_PolyCurve2d ()
{
  if (mySegments != 0L) {
    const Poly_Segment2d* aLast = mySegments;
    do {
      const Poly_Segment2d* aSeg = mySegments->myNext;
      mySegments->~Poly_Segment2d();
      myAllocator->Free (mySegments);
      mySegments = const_cast<Poly_Segment2d*> (aSeg);
    } while (mySegments != aLast);
    mySegments = 0L;
  }
}


//=======================================================================
//function : IsNegative
//purpose  : Auxilary
//=======================================================================
inline Standard_Boolean IsNegative(const Standard_Real theVal)
{
  return (theVal < -Precision::Confusion());
}

//=======================================================================
//function : IsNegative2
//purpose  : Auxilary
//=======================================================================
inline Standard_Boolean IsNegative2(const Standard_Real theVal)
{
  return (theVal < -Precision::Confusion()*Precision::Confusion());
}

//=======================================================================
//function : IsPositive
//purpose  : Auxilary
//=======================================================================
inline Standard_Boolean IsPositive(const Standard_Real theVal)
{
  return (theVal > Precision::Confusion());
}


//=======================================================================
//function : Area
//purpose  : 
//=======================================================================

Standard_Real Poly_PolyCurve2d::Area ()
{
  if (myType != CurveType_SetOfBoundaries) {
    if (myIsModified)
      ComputeProperties();
    return myArea;
  }

  Standard_Real aSumArea = 0.;
  Poly_ListOfCurve2d::Iterator aChildIter (myChildren);
  for (; aChildIter.More(); aChildIter.Next()) {
    const Handle(Poly_PolyCurve2d)& aChild = aChildIter.Value();
    Standard_Real anArea = aChild->Area();
    if (aChild->IsCounterClockwise())
      aSumArea += anArea;
    else
      aSumArea -= anArea;
  }
  return aSumArea;
}

//=======================================================================
//function : Perimeter
//purpose  : 
//=======================================================================

Standard_Real Poly_PolyCurve2d::Perimeter ()
{
  if (myType != CurveType_SetOfBoundaries) {
    if (myIsModified)
      ComputeProperties();
    return myPerimeter;
  }

  Standard_Real aSumPeri = 0.;
  Poly_ListOfCurve2d::Iterator aChildIter (myChildren);
  for (; aChildIter.More(); aChildIter.Next())
    aSumPeri += aChildIter.Value()->Perimeter();
  return aSumPeri;
}

//=======================================================================
//function : Point
//purpose  : 
//=======================================================================

gp_XY Poly_PolyCurve2d::Point (const Standard_Real theAbsc) const
{
  gp_XY aResult (RealLast(), RealLast());
  Standard_Real aTotalLen (0.);
  Poly_Seg2dIterator anIterE (* mySegments);
  if (IsNegative(theAbsc) == Standard_False)
    while (anIterE.More()) {
      const Poly_Segment2d& aSeg = anIterE.Value();
      const Standard_Real aSegLen = aSeg.Length();
      const Standard_Real aTLen (aTotalLen + aSegLen);
      if ( (aTLen - theAbsc) > Precision::Confusion() ) {
        const Standard_Real aLocLen (theAbsc - aTotalLen);
        aSeg.Interpolate (aLocLen / aSegLen, aResult);
        break;
      }
      aTotalLen = aTLen;
      anIterE.Next();
    }
  return aResult;
}

//=======================================================================
//function : IsClosed
//purpose  : 
//=======================================================================

Standard_Boolean Poly_PolyCurve2d::IsClosed (const Standard_Real theTol) const
{
  Standard_Boolean aResult (Standard_False);
  if (mySegments != 0L) {
    Standard_Real aTol;
    if (theTol < Precision::Confusion())
      aTol = Precision::Confusion();
    else
      aTol = theTol;
    aResult = ((mySegments->Point(0) -
                mySegments->Previous().Point(1)).SquareModulus()
               < aTol*aTol);
  }
  return aResult;
}

//=======================================================================
//function : CloseCurve
//purpose  : 
//=======================================================================

Standard_Boolean Poly_PolyCurve2d::CloseCurve (const Standard_Real theTolMove,
                                               const Standard_Real theTolClose)
{
  Standard_Boolean aResult (Standard_False);
  if (mySegments != 0L) {
    const gp_XY& aLastPnt = mySegments->Previous().Point(1);
    const Standard_Real aDist = (mySegments->Point(0) - aLastPnt).Modulus();
    if (aDist < Precision::Confusion())
      aResult = Standard_True;
    else if (aDist < theTolMove && mySegments != &mySegments->Previous()) {
      mySegments->myPnt[0] = aLastPnt;
      aResult = Standard_True;
    }
    else if (aDist < theTolClose) {
      Poly_Segment2d * aSeg =
        new (myAllocator) Poly_Segment2d (aLastPnt, mySegments->Point(0));
      appendSegment (aSeg);
      aResult = Standard_True;
    }
  }
  return aResult;
}

//=======================================================================
//function : IsCounterClockwise
//purpose  : 
//=======================================================================

Standard_Boolean Poly_PolyCurve2d::IsCounterClockwise ()
{
  if (myIsModified)
    ComputeProperties();
  return myIsCCW;
}

//=======================================================================
//function : RemoveChildCurve
//purpose  : 
//=======================================================================

Standard_Boolean Poly_PolyCurve2d::RemoveChildCurve
    (const Handle_Poly_PolyCurve2d& theCurve)
{
  Standard_Boolean aResult (Standard_False);
  Poly_ListOfCurve2d::Iterator aChildIter (myChildren);
  while (aChildIter.More()) {
    if (aChildIter.Value() == theCurve) {
      myChildren.Remove (aChildIter);
      aResult = Standard_True;
      break;
    }
    aChildIter.Next();
  }
  return aResult;
}

//=======================================================================
//function : RemoveChildren
//purpose  : 
//=======================================================================

Standard_Boolean Poly_PolyCurve2d::RemoveChildren
                                        (const Standard_Integer theNbSeg)
{
  Poly_ListOfCurve2d::Iterator aChildIter (myChildren);
  while (aChildIter.More()) {
    if (aChildIter.Value()->NSegments() < theNbSeg)
      myChildren.Remove (aChildIter);
    else
      aChildIter.Next();
  }
  return myNSegments < theNbSeg && myChildren.IsEmpty();
}

//=======================================================================
//function : RemoveHoles
//purpose  : 
//=======================================================================

Standard_Boolean Poly_PolyCurve2d::RemoveHoles()
{
  Poly_ListOfCurve2d::Iterator aChildIter (myChildren);
  while (aChildIter.More()) {
    if (!aChildIter.Value()->IsCounterClockwise())
      myChildren.Remove (aChildIter);
    else
      aChildIter.Next();
  }
  return myChildren.IsEmpty();
}

//=======================================================================
//function : CreateSegment
//purpose  : 
//=======================================================================

Poly_Segment2d& Poly_PolyCurve2d::CreateSegment
                                             (const gp_XY& theOrigin,
                                              const gp_XY& theExtremity,
                                              const Standard_Boolean isAppend)
{
  Poly_Segment2d * aSeg = new (myAllocator) Poly_Segment2d (theOrigin,
                                                            theExtremity);
  appendSegment (aSeg);
  if (isAppend == Standard_False) { /// prepend rather than append
    mySegments = aSeg;
  }
  return * aSeg;
}

//=======================================================================
//function : CreateSegment
//purpose  : 
//=======================================================================

Poly_Segment2d& Poly_PolyCurve2d::CreateSegment(const Poly_Segment2d& theSeg,
                                                const Standard_Boolean isReverse,
                                                const Standard_Boolean isAppend)
{
  Poly_Segment2d * aNewSeg = new (myAllocator) Poly_Segment2d (theSeg);
  if (isReverse)
  {
    gp_XY aTmp = aNewSeg->myPnt[0];
    aNewSeg->myPnt[0] = aNewSeg->myPnt[1];
    aNewSeg->myPnt[1] = aTmp;
    //Poly_Vertex* aVTmp = aNewSeg->myVertex[0];
    //aNewSeg->myVertex[0] = aNewSeg->myVertex[1];
    //aNewSeg->myVertex[1] = aVTmp;
    aNewSeg->myNormal.Reverse();
  }

  appendSegment (aNewSeg);
  if (isAppend == Standard_False) { /// prepend rather than append
    mySegments = aNewSeg;
  }
  return * aNewSeg;
}

//=======================================================================
//function : ComputeProperties
//purpose  : 
//=======================================================================

void Poly_PolyCurve2d::ComputeProperties ()
{
  myArea = 0.;
  myPerimeter = 0.;
  myIsCCW = Standard_True;
  Poly_Seg2dIterator anIterE (* mySegments);
  if (anIterE.More()) {
    const gp_XY& aPnt0 = anIterE.Value().Point(0);
    for (; anIterE.More(); anIterE.Next()) {
      const Poly_Segment2d& aSeg = anIterE.Value();
      myPerimeter += static_cast <Standard_ShortReal> (aSeg.Length());
      myArea += static_cast <Standard_ShortReal> (aSeg.Area(aPnt0));
    }
  }
  if (myArea < -Precision::Confusion()*Precision::Confusion()) {
    myArea = -myArea;
    myIsCCW = Standard_False;
  }
  myIsModified = Standard_False;
}

//=======================================================================
//function : Clone
//purpose  : 
//=======================================================================

Poly_PolyCurve2d * Poly_PolyCurve2d::Clone
                (const Handle_NCollection_BaseAllocator& theAlloc,
                 const Standard_Boolean                  isReverse) const
{
  Poly_PolyCurve2d * aNewCurve = Create(theAlloc, myType);

  Poly_Seg2dIterator::Sense aSense(Poly_Seg2dIterator::Forward);
  if (isReverse)
    aSense = Poly_Seg2dIterator::Reverse;

  Poly_Seg2dIterator  anIterE (* mySegments, aSense);
  for (; anIterE.More(); anIterE.Next())
    aNewCurve->CreateSegment (anIterE.Value(), isReverse);

  // Clone children.
  NCollection_List<Handle(Poly_PolyCurve2d)>::Iterator anIter(myChildren);
  Handle(Poly_PolyCurve2d) aChildCurve;
  Handle(Poly_PolyCurve2d) aNewChildCurve;

  for (; anIter.More(); anIter.Next()) {
    aChildCurve    = anIter.Value();
    aNewChildCurve = aChildCurve->Clone(theAlloc, isReverse);
    aNewCurve->AddChildCurve(aNewChildCurve);
  }

  return aNewCurve;
}

//=======================================================================
//function : GetContour2d
//purpose  : 
//=======================================================================

void Poly_PolyCurve2d::GetContour2d (NCollection_Vector<gp_XY>& theVec) const
{
  theVec.Clear();
  Poly_Seg2dIterator anIterE (* mySegments);
  if (anIterE.More()) {
    gp_XY aPnt, * pLastPnt = 0L;
    pLastPnt = & theVec.Append (anIterE.Value().Point(0));
    for (; anIterE.More(); anIterE.Next()) {
      const Poly_Segment2d& aSeg = anIterE.Value();
      if ((aSeg.Point(0)- *pLastPnt).SquareModulus() >
          Precision::Confusion()*Precision::Confusion())
      {
        theVec.Append (aSeg.Point(0));
      }
      pLastPnt = & theVec.Append (aSeg.Point(1));
    }
  }
}

//=======================================================================
//function : Transform
//purpose  : 
//=======================================================================

void Poly_PolyCurve2d::Transform (const gp_Trsf2d& theTrsf)
{
  Poly_Seg2dIterator anIterE (* mySegments);
  for (; anIterE.More(); anIterE.Next())
    anIterE.ChangeValue().Transform (theTrsf);

  if (myIsModified == Standard_False) {
    const Standard_ShortReal aScaleFactor =
      static_cast<Standard_ShortReal>(theTrsf.ScaleFactor());
    myArea *= aScaleFactor * aScaleFactor;
    myPerimeter *= aScaleFactor;
  }
  // Transform children.
  NCollection_List<Handle(Poly_PolyCurve2d)>::Iterator anIter(myChildren);
  Handle(Poly_PolyCurve2d) aChildCurve;
  for (; anIter.More(); anIter.Next()) {
    aChildCurve = anIter.Value();
    aChildCurve->Transform(theTrsf);
  }
}

//=======================================================================
//function : appendSegment
//purpose  : 
//=======================================================================

void Poly_PolyCurve2d::appendSegment (Poly_Segment2d* theSeg)
{
  if (mySegments == 0L) {
    theSeg->myPrevious = theSeg;
    theSeg->myNext = theSeg;
    mySegments = theSeg;
    myNSegments = 1;
  }
  else {
    const_cast<Poly_Segment2d*>(mySegments->myPrevious)->myNext = theSeg;
    theSeg->myPrevious = mySegments->myPrevious;
    mySegments->myPrevious = theSeg;
    theSeg->myNext = mySegments;
    myNSegments++;
  }
  myIsModified = Standard_True;
  theSeg->SetCurve (this);
}

//=======================================================================
//function : RemoveSegment
//purpose  : 
//=======================================================================

void Poly_PolyCurve2d::RemoveSegment (Poly_Segment2d* thepSeg)
{
  if (mySegments != 0L) {
    if (mySegments == thepSeg)
      mySegments = const_cast<Poly_Segment2d*>(thepSeg->myNext);
    if (mySegments != thepSeg)
      myNSegments--;
    else {
      mySegments = 0L;
      myNSegments = 0;
    }

    const_cast<Poly_Segment2d*>(thepSeg->myNext)->myPrevious = thepSeg->myPrevious;
    const_cast<Poly_Segment2d*>(thepSeg->myPrevious)->myNext = thepSeg->myNext;
    thepSeg->myPrevious = thepSeg;
    thepSeg->myNext = thepSeg;
    thepSeg->~Poly_Segment2d();
    myAllocator->Free (thepSeg);

    myIsModified = Standard_True;
  }
}

//=======================================================================
//function : ClassifyPoint
//purpose  : Classification of a point (IN/ON/OUT)
//=======================================================================

Poly_PolyCurve2d::Class2d Poly_PolyCurve2d::ClassifyPoint
                                        (const gp_XY&        thePnt,
                                         const Standard_Real theTol,
                                         Standard_Integer *  theCounter) const
{
  Class2d aResult(Class2d_Indefinite);
  //TODO: Now this method deals only with polygons (without arcs).
  gp_XY aPnt;
  Standard_Integer aCounter(0); // intersections counter
  const Standard_Real aTol2(theTol * theTol);
  Poly_Seg2dIterator anIter(* mySegments);
  for (; anIter.More(); anIter.Next()) {
    const Poly_Segment2d& aSeg = anIter.Value();
    // Find the projection on the contour 
    const Standard_Real aProjAbs = aSeg.Project(thePnt);
    aSeg.Interpolate(aProjAbs, aPnt);
    if ((aPnt - thePnt).SquareModulus() < aTol2) {
      aResult = Class2d_ON;
      break;
    }
    const Standard_Real aProd((thePnt - aSeg.Point(0)) ^
                              (aSeg.Point(1) - aSeg.Point(0)));
    if ( (thePnt.X() - aSeg.Point(0).X()) > Precision::Confusion() ) {
      if ( (thePnt.X() - aSeg.Point(1).X()) < Precision::Confusion() ) {
        if (aProd > 0.)
          aCounter++;
      }
    }
    else {
      if ( (thePnt.X() - aSeg.Point(1).X()) > Precision::Confusion() ) {
        if (aProd < 0.)
          aCounter++;
      }
    }
  }
  if (aResult == Class2d_Indefinite) {
    if (aCounter & 0x1)
      aResult = Class2d_IN;
    else
      aResult = Class2d_OUT;
  }
  if (theCounter)
    theCounter[0] += aCounter;
  return aResult;
}

//=======================================================================
//function : Dump
//purpose  : 
//=======================================================================

void Poly_PolyCurve2d::Dump (Standard_OStream& theStream,
                             Standard_Integer theCnt) const
{
  if (theCnt == 0)
    theStream << "==> Poly_Curve: " << myNSegments << " segments, "
              << myChildren.Extent() << " children " << endl;
  //#ifdef _DEBUG
  //theStream << " ... Count = " << myCount << endl;
  //#endif
  Poly_Seg2dIterator anIter (* mySegments);
  if (anIter.More()) {
    theStream << "  > curve " << theCnt;
    if (const_cast<Poly_PolyCurve2d*>(this)->IsCounterClockwise())
      theStream << " trigo (CCW)" << endl;
    else
      theStream << " opposite (CW)" << endl;
    Standard_Integer aCount(0);
    while (anIter.More()) {
      char buf[128];
      const Poly_Segment2d& anElem = anIter.Value();
      sprintf (buf, "\t%4d: [%8.4f %8.4f] [%8.4f %8.4f]",
               aCount++, anElem.Point(0).X(), anElem.Point(0).Y(),
               anElem.Point(1).X(), anElem.Point(1).Y());
      theStream << buf << endl;
      anIter.Next();
    }
    // - end curve
  }
  Standard_Integer aCount = theCnt;
  NCollection_List<Handle(Poly_PolyCurve2d)>::Iterator aChildIter (myChildren);
  while (aChildIter.More()) {
    aChildIter.Value()->Dump(theStream, ++aCount);
    aChildIter.Next();
  }
}

//=======================================================================
//function : CheckSelfIntersection
//purpose  : 
//=======================================================================

Standard_Boolean Poly_PolyCurve2d::CheckSelfIntersection
                        (gp_XY& outPnt, const Standard_Real theTol) const
{
  Standard_Boolean aResult(Standard_False);
  NCollection_List<Handle(Poly_PolyCurve2d)>::Iterator aChildIter (myChildren);
  for (; !aResult && aChildIter.More(); aChildIter.Next())
    aResult = aChildIter.Value()->CheckSelfIntersection(outPnt, theTol);
  if (aResult)
    return Standard_True;

  if (myNSegments < 4)
    return Standard_False;

  const Standard_Real aTol2 = theTol*theTol;
  Poly_Seg2dIterator anIterS0(* mySegments, Poly_Seg2dIterator::Forward);
  for (; anIterS0.More(); anIterS0.Next()) {
    const Poly_Segment2d& aSeg0 = anIterS0.Value();
    const gp_XY aDirSeg0 (aSeg0.Point(1) - aSeg0.Point(0));
    Standard_Real aLen0 = aDirSeg0.Modulus();
    if ( aLen0 < Precision::Confusion() )
      return Standard_True;
    const gp_XY aDir0 = aDirSeg0 / aLen0;
    Poly_Seg2dIterator anIterS1(aSeg0.Previous(),
                                Poly_Seg2dIterator::Forward);
    anIterS1.Next(); // come to this
    anIterS1.Next(); // come to next
    for (anIterS1.Next(); anIterS1.More(); anIterS1.Next()) {
      const Poly_Segment2d& aSeg1 = anIterS1.Value();
      const gp_XY aDirSeg1 (aSeg1.Point(1) - aSeg1.Point(0));
      Standard_Real aLen1 = aDirSeg1.Modulus();
      if ( aLen1 < Precision::Confusion() )
        return Standard_True;
      // Bounding box rejection
      if (fabs(aDirSeg0.X()) + fabs(aDirSeg1.X()) + 2.5 * theTol <
          fabs(aSeg0.Point(0).X() + aSeg0.Point(1).X() -
               aSeg1.Point(0).X() - aSeg1.Point(1).X()) ||
          fabs(aDirSeg0.Y()) + fabs(aDirSeg1.Y()) + 2.5 * theTol <
          fabs(aSeg0.Point(0).Y() + aSeg0.Point(1).Y() -
               aSeg1.Point(0).Y() - aSeg1.Point(1).Y()))
        continue;

      const gp_XY aDir1 = aDirSeg1 / aLen1;
      const gp_XY aLink = aSeg1.Point(0) - aSeg0.Point(0);
      const Standard_Real aProdDir = aDir0 ^ aDir1;
      // Check exact intersection
      if (aProdDir > Precision::Confusion() ||
          aProdDir < -Precision::Confusion())
      {
        const Standard_Real aCoeff [2] = {
          (aLink ^ aDir1) / (aProdDir * aLen0),
          (aLink ^ aDir0) / (aProdDir * aLen1)
        };
        if ((aCoeff[0]*(1.-aCoeff[0]) > -Precision::Confusion()) &&
            (aCoeff[1]*(1.-aCoeff[1]) > -Precision::Confusion()))
        {
          aResult = Standard_True;
          outPnt = ((1. - aCoeff[0]) * aSeg0.Point(0) +
                    aCoeff[0] * aSeg0.Point(1));
          break;
        }
      }
      // Check the proximity that could be better than Tol
      Standard_Real aCoord[4] = {
        (aLink * aDir0) / aLen0,
        ((aSeg1.Point(1) - aSeg0.Point(0)) * aDir0) / aLen0,
        -(aLink * aDir1) / aLen1,
        ((aSeg0.Point(1) - aSeg1.Point(0)) * aDir1) / aLen1
      };
      Standard_Real aDist(1.01 * aTol2);
      const gp_XY * aPnt = 0L;
      if (!IsNegative(aCoord[0] * (1. - aCoord[0]))) {
        const Standard_Real aD = aLink ^ aDir0;
        if (aD * aD < aDist) {
          if ( (((aSeg1.Point(1) - aSeg0.Point(0)) ^ aDir0) * aD)
               < Precision::Confusion() ||  aD * aD < 0.0625 * aTol2)
          {
            aDist = aD * aD;
            aPnt = &aSeg1.Point(0);
          }
        }
      }
      if (!IsNegative(aCoord[1] * (1. - aCoord[1]))) {
        const Standard_Real aD = (aSeg1.Point(1) - aSeg0.Point(0)) ^ aDir0;
        if (aD * aD < aDist) {
          if ( ((aLink ^ aDir0) * aD) < Precision::Confusion() ||
               aD * aD < 0.0625 * aTol2 )
          {
            aDist =  aD * aD;
            aPnt = &aSeg1.Point(1);
          }
        }
      }
      if (!IsNegative(aCoord[2] * (1. - aCoord[2]))) {
        const Standard_Real aD = - (aLink ^ aDir1);
        if (aD * aD < aDist) {
          if ( (((aSeg0.Point(1) - aSeg1.Point(0)) ^ aDir1) * aD)
               < Precision::Confusion() || aD * aD < 0.0625 * aTol2 )
          {
            aDist =  aD * aD;
            aPnt = &aSeg0.Point(0);
          }
        }
      }
      if (!IsNegative(aCoord[3] * (1. - aCoord[3]))) {
        const Standard_Real aD = (aSeg0.Point(1) - aSeg1.Point(0)) ^ aDir1;
        if (aD * aD < aDist) {
          if (!IsPositive(-(aLink ^ aDir1) * aD) ||
              aD * aD < 0.0625 * aTol2)
          {
            aDist =  aD * aD;
            aPnt = &aSeg0.Point(1);
          }
        }
      }
      if (aDist < aTol2) {
        aResult = Standard_True;
        outPnt = * aPnt;
        break;
      }
    }
    if (aResult)
      break;
  }
  return aResult;
}

//=======================================================================
//function : CheckMutulaIntersection
//purpose  : 
//=======================================================================

Standard_Boolean Poly_PolyCurve2d::CheckMutualIntersection
                                  (const Handle(Poly_PolyCurve2d)& theCurve0,
                                   const Handle(Poly_PolyCurve2d)& theCurve1,
                                   const Standard_Real theTol)
{
  Standard_Boolean aResult(Standard_False);
  if (theCurve0.IsNull() || theCurve1.IsNull())
    return Standard_False;
  //#ifdef _DEBUG
  //static gp_XY outPnt;
  //#endif

  const Standard_Real aTol2 = theTol*theTol;
  Bnd_B2d aBox1;
  Poly_Seg2dIterator anIterS1 = theCurve1->SegmentIterator();
  for (; anIterS1.More(); anIterS1.Next()) {
    aBox1.Add (anIterS1.Value().Point(0));
    // we do not add Point(1) because it should be taken from the next segment
  }
  aBox1.Enlarge(theTol);

  Poly_Seg2dIterator anIterS0 = theCurve0->SegmentIterator();
  for (; anIterS0.More(); anIterS0.Next()) {
    const Poly_Segment2d& aSeg0 = anIterS0.Value();
    Bnd_B2d aSegBox;
    aSegBox.Add(aSeg0.Point(0));
    aSegBox.Add(aSeg0.Point(1));
    if (aSegBox.IsOut(aBox1))
      continue;
    const gp_XY aDirSeg0 (aSeg0.Point(1) - aSeg0.Point(0));
    const gp_XY aDir0 = aSeg0.Dir();

    for (anIterS1.Reset(); anIterS1.More(); anIterS1.Next()) {
      const Poly_Segment2d& aSeg1 = anIterS1.Value();
      const gp_XY aDirSeg1 (aSeg1.Point(1) - aSeg1.Point(0));
      // Bounding box rejection
      if (fabs(aDirSeg0.X()) + fabs(aDirSeg1.X()) + 2.5 * theTol <
          fabs(aSeg0.Point(0).X() + aSeg0.Point(1).X() -
               aSeg1.Point(0).X() - aSeg1.Point(1).X()) ||
          fabs(aDirSeg0.Y()) + fabs(aDirSeg1.Y()) + 2.5 * theTol <
          fabs(aSeg0.Point(0).Y() + aSeg0.Point(1).Y() -
               aSeg1.Point(0).Y() - aSeg1.Point(1).Y()))
        continue;

      const gp_XY aDir1 = aSeg1.Dir();
      const gp_XY aLink = aSeg1.Point(0) - aSeg0.Point(0);
      const Standard_Real aProdDir = aDir0 ^ aDir1;
      // Check exact intersection
      if (aProdDir > Precision::Confusion() ||
          aProdDir < -Precision::Confusion())
      {
        const Standard_Real aCoeff [2] = {
          (aLink ^ aDir1) / (aProdDir * aSeg0.Length()),
          (aLink ^ aDir0) / (aProdDir * aSeg1.Length())
        };
        if ((aCoeff[0]*(1.-aCoeff[0]) > -Precision::Confusion()) &&
            (aCoeff[1]*(1.-aCoeff[1]) > -Precision::Confusion()))
        {
          aResult = Standard_True;
          //#ifdef _DEBUG
          //outPnt = ((1. - aCoeff[0]) * aSeg0.Point(0) +
          //          aCoeff[0] * aSeg0.Point(1));
          //#endif
          break;
        }
      }
      // Check the proximity that could be better than Tol
      Standard_Real aCoord[4] = {
        (aLink * aDir0) / aSeg0.Length(),
        ((aSeg1.Point(1) - aSeg0.Point(0)) * aDir0) / aSeg0.Length(),
        -(aLink * aDir1) / aSeg1.Length(),
        ((aSeg0.Point(1) - aSeg1.Point(0)) * aDir1) / aSeg1.Length()
      };
      Standard_Real aDist(1.01 * aTol2);
      const gp_XY * aPnt = 0L;
      if (!IsNegative(aCoord[0] * (1. - aCoord[0]))) {
        const Standard_Real aD = aLink ^ aDir0;
        if (aD * aD < aDist) {
          if ( (((aSeg1.Point(1) - aSeg0.Point(0)) ^ aDir0) * aD)
               < Precision::Confusion() || aD * aD < 0.0625 * aTol2 )
          {
            aDist = aD * aD;
            aPnt = &aSeg1.Point(0);
          }
        }
      }
      if (!IsNegative(aCoord[1] * (1. - aCoord[1]))) {
        const Standard_Real aD = (aSeg1.Point(1) - aSeg0.Point(0)) ^ aDir0;
        if (aD * aD < aDist) {
          if ( ((aLink ^ aDir0) * aD) < Precision::Confusion() ||
               aD * aD < 0.0625 * aTol2 )
          {
            aDist =  aD * aD;
            aPnt = &aSeg1.Point(1);
          }
        }
      }
      if (!IsNegative(aCoord[2] * (1. - aCoord[2]))) {
        const Standard_Real aD = - (aLink ^ aDir1);
        if (aD * aD < aDist) {
          if ( (((aSeg0.Point(1) - aSeg1.Point(0)) ^ aDir1) * aD)
               < Precision::Confusion() || aD * aD < 0.0625 * aTol2 )
          {
            aDist =  aD * aD;
            aPnt = &aSeg0.Point(0);
          }
        }
      }
      if (!IsNegative(aCoord[3] * (1. - aCoord[3]))) {
        const Standard_Real aD = (aSeg0.Point(1) - aSeg1.Point(0)) ^ aDir1;
        if (aD * aD < aDist) {
          if ( (-(aLink ^ aDir1) * aD) < Precision::Confusion() ||
               aD * aD < 0.0625 * aTol2 )
          {
            aDist =  aD * aD;
            aPnt = &aSeg0.Point(1);
          }
        }
      }
      if (aDist < aTol2) {
        aResult = Standard_True;
        //#ifdef _DEBUG
        //outPnt = * aPnt;
        //#endif
        break;
      }
    }
    if (aResult)
      break;
  }
  return aResult;
}

//=======================================================================
//function : Project
//purpose  : 
//=======================================================================

Standard_Real Poly_PolyCurve2d::Project(const gp_XY& thePnt,
                                        Location& theLoc) const
{
  Standard_Real aMinSqDist = RealLast();

  // iterate on segments
  Standard_Integer iSeg = 0;
  Poly_Seg2dIterator aSIter (*mySegments);
  for (; aSIter.More(); aSIter.Next(), iSeg++) {
    const Poly_Segment2d& aSeg = aSIter.Value();
    const gp_XY aDelta = aSeg.Point(1) - aSeg.Point(0);
    const Standard_Real aSqLen = aDelta.SquareModulus();
    //if (!IsPositive(aSqLen))
    if ( aSqLen < Precision::Confusion()*Precision::Confusion() )
      continue;
    gp_XY aVec0 = thePnt - aSeg.Point(0);
    Standard_Real anAbsc = aDelta * aVec0;
    Standard_Real aSqDist;
    //if (!IsPositive2(anAbsc))
    if ( anAbsc< Precision::Confusion() ) {
      // project on begin of segment
      aSqDist = aVec0.SquareModulus();
      anAbsc = 0.;
    }
    else if (!IsNegative2(anAbsc - aSqLen)) {
      // project on end of segment
      aSqDist = (thePnt - aSeg.Point(1)).SquareModulus();
      anAbsc = aSqLen;
    }
    else
      aSqDist = aVec0.CrossSquareMagnitude(aDelta) / aSqLen;
    Standard_Real aParam = anAbsc / aSqLen;
    if (aMinSqDist > aSqDist) {
      aMinSqDist = aSqDist;
      theLoc.pSeg = & aSeg;
      theLoc.Param = aParam;
      theLoc.SegNum = iSeg;
    }
  }
  return aMinSqDist;
}

//=======================================================================
//function : Location::IsEqual
//purpose  : 
//=======================================================================

Standard_Boolean Poly_PolyCurve2d::Location::IsEqual
                   (const Location& theOther,
                    const Standard_Real theSqConf) const
{
  gp_XY aP, aP1;
  pSeg->Interpolate (Param, aP);
  theOther.pSeg->Interpolate (theOther.Param, aP1);
  return (aP-aP1).SquareModulus() < theSqConf;
}

//=======================================================================
//function : checkOutTube
//purpose  : internal function
//=======================================================================

inline Standard_Boolean checkOutTube (const gp_XY& theVec,
                                      const gp_XY& theBaseDir,
                                      const Standard_Real theTol2)
{
  return theBaseDir.CrossSquareMagnitude(theVec) > theTol2;
}

//=======================================================================
//function : checkOutTube
//purpose  : static (internal) function
//=======================================================================

static const Standard_Real HALF_SQRT2 = 0.5 * sqrt(2.);

static Standard_Boolean checkOutTube (const Poly_Segment2d& theFirst,
                                      const Poly_Segment2d& theLast,
                                      const Standard_Real    theTol2)
{
  Standard_Boolean aResult(Standard_False);
  const gp_XY& aBase = theFirst.Point(0);
  gp_XY aBaseDir = theLast.Point(1) - aBase;
  const Standard_Real aBaseMag2 = aBaseDir.SquareModulus();

  // If the length of the chain is smaller than Tol, we return False
  // (i.e. the chain is in tube without the analysis of internal points)
  if (aBaseMag2 < theTol2)
    aResult = Standard_False;
  else {
    aBaseDir /= sqrt(aBaseMag2);

    Poly_Seg2dIterator anIter(theFirst.Next());
    for (; anIter.More(); anIter.Next()) {
      const Poly_Segment2d& aSeg = anIter.Value();
      if (checkOutTube(aSeg.Point(0) - aBase, aBaseDir, theTol2)) {
        aResult = Standard_True;
        break;
      }
      // end-of-loop statement
      if (&aSeg == &theLast) {
        gp_XY aSegDir = aSeg.Point(1) - aSeg.Point(0);
        Standard_Real aSegLen = aSegDir.SquareModulus();
        // Test for the angle between segments
        if (aSegLen > theTol2)
        {
          if (aSegDir.Dot(aBaseDir) < sqrt(aSegLen) * HALF_SQRT2)
            aResult = Standard_True;
        }
        break;
      }
      if (checkOutTube(aSeg.Point(1) - aBase, aBaseDir, theTol2)) {
        aResult = Standard_True;
        break;
      }
    }
  }
  return aResult;
}

//=======================================================================
//function : Filter
//purpose  : 
//=======================================================================

Standard_Boolean Poly_PolyCurve2d::Filter(const Standard_Real theTol,
                                          const Standard_Boolean theRemoveDegen)
{
  NCollection_List<Handle(Poly_PolyCurve2d)>::Iterator aChildIter (myChildren);
  while (aChildIter.More()) {
    Handle(Poly_PolyCurve2d) aCurve = aChildIter.Value();
    aCurve->Filter(theTol);
    if (theRemoveDegen && aCurve->NSegments() < 3)
      myChildren.Remove(aChildIter);
    else
      aChildIter.Next();
  }

  if (myNSegments > 1) {
    const Standard_Boolean isClosed = IsClosed();
    // iterate on all segments
    const Standard_Real aTol2 = theTol * theTol;
    Poly_Seg2dIterator anIter = SegmentIterator();
    Poly_Segment2d* pFirstSeg = &anIter.ChangeValue();
    Poly_Segment2d* pFirst = pFirstSeg; // First seg in the chain
    Poly_Segment2d* pLast  = pFirstSeg; // Last seg in the chain
    for (anIter.Next(); anIter.More(); anIter.Next()) {
      Poly_Segment2d& aCurSeg = anIter.ChangeValue();
      Standard_Boolean isStopChain(Standard_False);
      if ((aCurSeg.Point(0) - pLast->Point(1)).SquareModulus() > aTol2)
        // do not filter across breaks of continuity
        isStopChain = Standard_True;
      //else if (isFeatureVertex(aCurSeg))
      //  // do not filter across vertices
      //  isStopChain = Standard_True;
      else if (checkOutTube(*pFirst, aCurSeg, aTol2))
        // a sharp turn is detected
        isStopChain = Standard_True;

      if (isStopChain) {
        if (pFirst != pLast)
          // Merge the chain of segments first to last
          mergeSegments(pFirst, pLast);
        pFirst = &aCurSeg;
      }
      pLast = &aCurSeg;
    }
    // end-of-curve closeup
    if (pFirst != pLast) {
      // protection against obtaining a 1-segment closed curve (wrong result)
      if (!isClosed || pFirst != pFirstSeg)
        // Merge the chain of segments first to last
        mergeSegments(pFirst, pLast);
    }
  }
  return Standard_True;
}

//=======================================================================
//function : mergeSegments
//purpose  : 
//=======================================================================

void Poly_PolyCurve2d::mergeSegments(Poly_Segment2d* thepFirst,
                                     Poly_Segment2d* thepLast)
{
  // copy last point from the last segment to the first one
  thepFirst->SetPoint(1, thepLast->Point(1));

  // copy vertex as well
  //if (!thepFirst->Vertex(0) || thepFirst->Vertex(0) != thepLast->Vertex(1))
  //  thepFirst->SetVertex(1, thepLast->Vertex(1));
  //else
  //  thepFirst->SetVertex(1, NULL);

  // remove segments from the next to the first till the last one
  Poly_Seg2dIterator anIterS(thepFirst->Next());
  while (anIterS.More()) {
    Poly_Segment2d* pSeg = & anIterS.ChangeValue();
    const Standard_Boolean isFinish = (pSeg == thepLast);
    anIterS.Next();
    RemoveSegment(pSeg);
    if (isFinish)
      break;
  }

  // suppress last vertex if it is an artificial one
  //if (thepFirst->Vertex(1) && thepFirst->Vertex(1)->IsArtificial())
  //{
  //  thepFirst->SetVertex(1, NULL);
  //  const_cast<Poly_Segment&>(thepFirst->Next()).SetVertex(0, NULL);
  //}
}

//=======================================================================
//function : FilterCusps
//purpose  : 
//=======================================================================

void Poly_PolyCurve2d::FilterCusps(const Standard_Real theTol,
                                   const Standard_Boolean theRemoveDegen)
{
  NCollection_List<Handle(Poly_PolyCurve2d)>::Iterator aChildIter (myChildren);
  while (aChildIter.More()) {
    Handle(Poly_PolyCurve2d) aCurve = aChildIter.Value();
    aCurve->FilterCusps(theTol);
    if (theRemoveDegen && aCurve->NSegments() < 3)
      myChildren.Remove(aChildIter);
    else
      aChildIter.Next();
  }

  if (myNSegments < 3)
    return;

  const Standard_Real aSqTol = theTol * theTol;
  const Standard_Boolean isClosed = IsClosed();

  // iterate on all segments
  Poly_Segment2d* pPrevSeg =
    const_cast<Poly_Segment2d*>(&mySegments->Previous());
  if (!isClosed)
    pPrevSeg = mySegments;
  gp_XY aPrevDir = pPrevSeg->Dir();
  Standard_Boolean isBegin = Standard_True;
  while ((isBegin || &pPrevSeg->Next() != mySegments) && myNSegments > 2) {
    isBegin = Standard_False;
    Poly_Segment2d* pCurrSeg = const_cast<Poly_Segment2d*>(&pPrevSeg->Next());
    // compute dot product between the current and the previous segments
    gp_XY aCurrDir = pCurrSeg->Dir();
    Standard_Real aDot = aCurrDir * aPrevDir;
    const Standard_Real aThreshold = 0; //was -0.9848;
    if (aDot < aThreshold) {
      // check normal distance from an end point

      // project the point 1 of the curr seg to the prev seg
      const gp_XY& aP1 = pCurrSeg->Point(1);
      Standard_Real aPar = pPrevSeg->Project(aP1, Standard_False);
      if (!IsNegative(aPar) && !IsNegative(1.-aPar)) {
        gp_XY aProj;
        pPrevSeg->Interpolate(aPar, aProj);
        Standard_Real aSqDist = (aProj - aP1).SquareModulus();
        if (aSqDist < aSqTol) {
          // remove current, reduce previous
          pPrevSeg->SetPoint(1, aP1);
          RemoveSegment(pCurrSeg);
          // check new length of the previous
          Standard_Real aSqLen =
            (pPrevSeg->Point(1) - pPrevSeg->Point(0)).SquareModulus();
          if (aSqLen < aSqTol) {
            // remove the previous as well
            Poly_Segment2d* pNextSeg =
              const_cast<Poly_Segment2d*>(&pPrevSeg->Next());
            if (isClosed ||
                pNextSeg != mySegments && pPrevSeg != mySegments)
              pNextSeg->SetPoint(0, pPrevSeg->Point(0));
            RemoveSegment(pPrevSeg);
            pPrevSeg = const_cast<Poly_Segment2d*>(&pNextSeg->Previous());
          }
          aPrevDir = pPrevSeg->Dir();
          continue;
        }
      }

      // project the point 0 of the prev seg to the curr seg
      const gp_XY& aP0 = pPrevSeg->Point(0);
      aPar = pCurrSeg->Project(aP0, Standard_False);
      if (!IsNegative(aPar) && !IsNegative(1.-aPar)) {
        gp_XY aProj;
        pCurrSeg->Interpolate(aPar, aProj);
        Standard_Real aSqDist = (aProj - aP0).SquareModulus();
        if (aSqDist < aSqTol) {
          // remove previous, reduce current
          pCurrSeg->SetPoint(0, aP0);
          RemoveSegment(pPrevSeg);
          // check new length of the current
          Standard_Real aSqLen =
            (pCurrSeg->Point(1) - pCurrSeg->Point(0)).SquareModulus();
          if (aSqLen < aSqTol) {
            // remove the current as well
            Poly_Segment2d* pNextSeg =
              const_cast<Poly_Segment2d*>(&pCurrSeg->Next());
            if (isClosed ||
                pCurrSeg != mySegments && pNextSeg != mySegments)
              pNextSeg->SetPoint(0, pCurrSeg->Point(0));
            RemoveSegment(pCurrSeg);
            pCurrSeg = pNextSeg;
          }
          pPrevSeg = const_cast<Poly_Segment2d*>(&pCurrSeg->Previous());
          aPrevDir = pPrevSeg->Dir();
          continue;
        }
      }
    }

    pPrevSeg = pCurrSeg;
    aPrevDir = aCurrDir;
  }
}

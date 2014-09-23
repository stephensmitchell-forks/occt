// File:      Poly_IBuffer.cxx
// Created:   15.06.07 08:32
// Author:    Alexander GRIGORIEV
// Copyright: Open CASCADE 2007

#include <Poly_IBuffer.hxx>
#include <Precision.hxx>
#include <NCollection_IncAllocator.hxx>
#include <Standard_ProgramError.hxx>

#pragma warning(disable:4291)

IMPLEMENT_STANDARD_HANDLE (Poly_IBuffer, Standard_Transient)
IMPLEMENT_STANDARD_RTTIEXT(Poly_IBuffer, Standard_Transient)

static const Standard_Size INDEXLEN = 10;
#ifdef _DEBUG
int Poly_IBuffer::GlobalCount = 0;
#endif

//=======================================================================
//function : GetZ
//purpose  : local function
//=======================================================================

inline Standard_Real GetZ (const Poly_IBuffer::Interval& theInt,
                           const Standard_Real           theX)
{
  return (theInt.XZ(0).Y() + (theInt.XZ(1).Y() - theInt.XZ(0).Y()) *
          (theX - theInt.XZ(0).X()) / (theInt.XZ(1).X() - theInt.XZ(0).X()));
}

//=======================================================================
//function : Poly_IBuffer
//purpose  : Constructor
//=======================================================================

Poly_IBuffer::Poly_IBuffer (const Poly_IBuffer::Interval&           theInterval,
                            const Standard_Real                     theTol,
                            const Handle_NCollection_BaseAllocator& theAlloc)
{
  Init (theInterval, theTol, theAlloc);
#ifdef _DEBUG
  myCount = GlobalCount++;
#endif
}

//=======================================================================
//function : Init
//purpose  : Initialization, to be used after the default constructor.
//=======================================================================

void Poly_IBuffer::Init (const Interval&                         theInterval,
                         const Standard_Real                     theTol,
                         const Handle_NCollection_BaseAllocator& theAlloc)
{
  clearIntervals();
  myTol = theTol;
  if (theAlloc.IsNull())
    myAllocator = new NCollection_IncAllocator(1000);
  else
    myAllocator = theAlloc;

  myMinOrd = Precision::Infinite();
  myMaxOrd = -Precision::Infinite();
  myNIntervals = 1;

  // Create the first interval covering all possible X coordinates 
  Interval * pInt = &theInterval.Clone(myAllocator);
  // Create the index array
  myIndex = static_cast <PInterval *>
    (myAllocator->Allocate((INDEXLEN+1) * sizeof(PInterval)));
  const Standard_Real anXmin = theInterval.XZ(0).X();
  const Standard_Real aXstep = (theInterval.XZ(1).X() - anXmin) / INDEXLEN;
  for (Standard_Size i = 0; i <= INDEXLEN; i++) {
    myIndex[i].X   = Standard_Real (anXmin + i*aXstep);
    myIndex[i].Int = pInt;
  }
}

//=======================================================================
//function : ~Poly_IBuffer
//purpose  : Destructor
//=======================================================================

Poly_IBuffer::~Poly_IBuffer ()
{
  clearIntervals();
}

//=======================================================================
//function : Clone
//purpose  : Create a copy of the given Interval in the given allocator.
//=======================================================================

Poly_IBuffer::Interval& Poly_IBuffer::Interval::Clone
                        (const Handle_NCollection_BaseAllocator& theAlloc,
                         Poly_IBuffer::Interval *                theDest) const
{
  if (theDest == 0L)
    theDest = new (theAlloc) Interval(myXZ[0], myXZ[1], myModID);
  else {
    theDest->myXZ[0] = myXZ[0];
    theDest->myXZ[1] = myXZ[1];
    theDest->myNext  = 0L;
    theDest->myModID = 0UL;
    theDest->U.myModX  = RealLast();
  }
  return * theDest;
}

//=======================================================================
//function : NewRejectID
//purpose  : Generator of unique Reject ID values.
//=======================================================================

unsigned long Poly_IBuffer::Interval::NewRejectID ()
{
  static unsigned long aValue(0UL);
  aValue += 4;
  return aValue;
}

//=======================================================================
//function : IsIntersect
//purpose  : Check for intersection, returns the intersection point abscissa.
//   @return
//   True if a modification or removal is required. The decision is based
//   on 2 lower bits of myRejectID value:
//    - 0 : remove this interval;
//    - 1 : keep the lower abscisse part, remove the upper part;
//    - 2 : keep the upper abscisse part, remove the lower part;
//    - 3 : remove the middle.
//=======================================================================

Standard_Boolean Poly_IBuffer::Interval::IsIntersect
                                     (const Poly_IBuffer::Interval& theInt,
                                      const Standard_Real           theTol,
                                      const unsigned long           theRejectID)
{
  Standard_Integer aResult (-1);
  Standard_Real anAbscisse (Precision::Infinite());
  const Standard_Real anXtol[2] = {
    myXZ[0].X() + theTol,
    myXZ[1].X() - theTol
  };
  if (theInt.myXZ[0].X() < anXtol[1] && theInt.myXZ[1].X() > anXtol[0]) {
    const gp_XY aVec[3] = {
      gp_XY (Standard_Real(theInt.myXZ[0].X() - myXZ[0].X()),
             Standard_Real(theInt.myXZ[0].Y() - myXZ[0].Y())),
      gp_XY (Standard_Real(myXZ[1].X() - myXZ[0].X()), 
             Standard_Real(myXZ[1].Y() - myXZ[0].Y())),
      gp_XY (Standard_Real(theInt.myXZ[1].X() - theInt.myXZ[0].X()), 
             Standard_Real(theInt.myXZ[1].Y() - theInt.myXZ[0].Y()))
    };
    const Standard_Real aDirProd = aVec[1] ^ aVec[2];
    if (aDirProd  > Precision::Confusion()) { 
      // Intersection coefficients
      const Standard_Real fInter[2] = {
        (aVec[0] ^ aVec[2]) / aDirProd,
        (aVec[0] ^ aVec[1]) / aDirProd
      };
      anAbscisse = (myXZ[1].X() * fInter[0] + myXZ[0].X() * (1.-fInter[0]));
      if (anAbscisse > anXtol[0] && fInter[1] > 0.)
        if (anAbscisse < anXtol[1] && fInter[1] < 1.) {
          if (theInt.myXZ[0].X() < anXtol[0])
            aResult = 2;  // keep upper abscissa
          else {
            anAbscisse = theInt.myXZ[0].X();
            aResult = 3;
          }
        } else {
          if (theInt.myXZ[0].X() > anXtol[0]) {
            anAbscisse = theInt.myXZ[0].X();
            if (theInt.myXZ[1].X() > anXtol[1])
              aResult = 1; // keep lower abscissa
            else
              aResult = 3;
          } else
            if (theInt.myXZ[1].X() > anXtol[1])
              aResult = 0; // remove
            else {
              anAbscisse = theInt.myXZ[1].X();
              aResult = 2; // keep upper abscissa
            }
        }
    } else {
      Standard_Real fInter[2] = { 2., -1. };
      if (aDirProd  < -Precision::Confusion()) {
        // Intersection coefficients
        fInter[0] = (aVec[0] ^ aVec[2]) / aDirProd;
        fInter[1] = (aVec[0] ^ aVec[1]) / aDirProd;
      } else if (GetZ (theInt, myXZ[0].X()) < myXZ[0].Y())
        fInter[0] = -1.;

      anAbscisse = (myXZ[1].X() * fInter[0] + myXZ[0].X() * (1.-fInter[0]));
      if (anAbscisse < anXtol[1] &&
          fInter[1] < 1.-Precision::Confusion())
        if (anAbscisse > anXtol[0] &&
            fInter[1] > Precision::Confusion())
        {
          if (theInt.myXZ[1].X() > anXtol[1])
            aResult = 1;  // keep lower abscissa
          else
            aResult = 3;
        } else {
          if (theInt.myXZ[0].X() > anXtol[0]) {
            anAbscisse = theInt.myXZ[0].X();
            if (theInt.myXZ[1].X() > anXtol[1])
              aResult = 1; // keep lower abscissa
            else
              aResult = 3;
          } else
            if (theInt.myXZ[1].X() > anXtol[1])
              aResult = 0; // remove
            else {
              anAbscisse = theInt.myXZ[1].X();
              aResult = 2; //keep upper abscissa
            }
        }
    }
  }
  if (aResult >= 0) {
    myModID = theRejectID + aResult;
    U.myModX = anAbscisse;
    return Standard_True;
  }
  return Standard_False;
}

//=======================================================================
//function : Trim
//purpose  : Shorten the interval to the new value of Xmax.
//=======================================================================

void Poly_IBuffer::Interval::Trim (const Standard_Real    theX,
                                   const Standard_Integer theMode)
{
  const Standard_Real aZ = GetZ(* this, theX);
  switch (theMode) {
  case 3:
    myModID = 0UL;
  case 1:
    Standard_ProgramError_Raise_if
        (theX - myXZ[0].X() < Precision::Confusion() || theX > myXZ[1].X(),
         "Trim error");
    myXZ[1].SetCoord(theX, aZ);
    break;
  case 2:
    Standard_ProgramError_Raise_if
        (theX < myXZ[0].X() || myXZ[1].X() - theX < Precision::Confusion(),
         "Trim error");
    myXZ[0].SetCoord(theX, aZ);
    break;
  }
}

//=======================================================================
//function : AddInterval
//purpose  : Add one interval to buffer.
//=======================================================================

Poly_IBuffer::Interval * Poly_IBuffer::AddInterval
                             (const Poly_IBuffer::Interval& theInterval,
                              Poly_IBuffer::Interval *      theStart)
{
//  Variables:
// aLastModified: Newly created interval in the buffer. It is returned by
//                this method so that it can be passed as theStart
//                for the next call (next interval).
// aInt:          Currently checked/processed interval
// aBracket[0,1]: The first and the last in the chain of intervals where
//                the modification can be done, used for local optimisation.
// pLastInt:      The last non-modified interval. This pointer is used to set
//                pLastInt->Next() to the newly created interval.
// isWaiting:     True if there have been removed intervals, to be replaced by
//                a new interval.
// aFirst.Int:    Same as pLastInt. probably we can get rid of pLastInt
// aFirst.X:      Beginning of the new interval

  Interval * aLastModified = 0L, * aStart;
  if (theStart == 0L)
    aStart = const_cast <Interval *>(myIndex[0].Int);
  else
    aStart = theStart;
  Interval * anInt = aStart;
  Interval * aBracket[2] = { 0L, 0L };
  unsigned long aRejectID = Interval::NewRejectID();
  Interval * pLastInt = theStart;

  // Identify the intervals that should be modified
  for (; anInt; anInt = anInt->Next()) {
    if (anInt->XZ(0).X() > theInterval.XZ(1).X() - myTol)
      break;
    if (anInt->IsIntersect (theInterval, myTol, aRejectID)) {
      if (aBracket[0] == 0L)
        aBracket[0] = anInt;
      aBracket[1] = anInt->Next();
      const long aModMode = anInt->GetModificationID() - aRejectID;
      if (aModMode == 3L) {
        gp_XY anXZ = anInt->XZ(1);
        anInt->Trim (anInt->GetModificationAbscisse(), aModMode);
        Interval& aNewInt = anInt->Clone(myAllocator);
        aNewInt.myXZ[0] = anInt->XZ(1);
        aNewInt.myXZ[1] = anXZ;
        aNewInt.myNext = anInt->Next();
        anInt->myNext = &aNewInt;
        myNIntervals++;
      }
    } else if (aBracket[0] == 0L)
      pLastInt = anInt;
  }

  // Modify the intervals
  Standard_Boolean isWaiting (Standard_False);
  PInterval aFirst;
  aFirst.Int = theStart;
  aFirst.X = aStart->XZ(0).X();
  for (anInt = aBracket[0]; anInt; anInt = anInt->Next()) {
    const unsigned long aCurrentID = anInt->GetModificationID();
    if ((aCurrentID & ~0x3) != aRejectID) {
      if (isWaiting) {
        const Standard_Real anX[2] = {
          aFirst.X,
          anInt->XZ(0).X()
        };
        aLastModified = addInt (theInterval, anX, aFirst.Int);
        if (aStart == 0L)
          aStart = aLastModified;
        aLastModified->myNext = anInt;
        aFirst.Int = anInt;
        isWaiting = Standard_False;
      }
      if (anInt == aBracket[1])
        break;
      pLastInt = anInt;
    } else {
      const long aModMode = aCurrentID - aRejectID;
      switch (aModMode) {
      case 0:
        if (aStart == anInt)
          aStart = 0L;
        if (isWaiting == Standard_False) {
          aFirst.Int = pLastInt;
          aFirst.X = anInt->XZ(0).X();
          isWaiting = Standard_True;
        }
        myNHoles++;
        break;
      case 1:
        aFirst.X = anInt->GetModificationAbscisse();
        aFirst.Int = anInt;
        anInt->Trim (aFirst.X, 1);
        pLastInt = anInt;
        isWaiting = Standard_True;
        break;
      case 2:
        {
          if (isWaiting)
            isWaiting = Standard_False;
          else {
            aFirst.Int = pLastInt;
            aFirst.X = anInt->XZ(0).X();
          }
          anInt->Trim (anInt->GetModificationAbscisse(), 2);
          const Standard_Real anX[2] = {
            aFirst.X,
            anInt->GetModificationAbscisse()
          };
          aLastModified = addInt (theInterval, anX, aFirst.Int);
          if (aStart == 0L)
            aStart = aLastModified;
          aLastModified->myNext = anInt;
          aFirst.Int = anInt;
          pLastInt = anInt;
        }
        break;
      default:
        Standard_ProgramError::Raise ("Poly_IBuffer::AddInterval");
      }
    }
  }
  if (isWaiting) {
    const Standard_Real anX[2] = {
      aFirst.X,
      myIndex[INDEXLEN].X
    };
    aLastModified = addInt (theInterval, anX, aFirst.Int);
  }
  return aLastModified;
}

//=======================================================================
//function : CloseBuffer
//purpose  : Called after all additions of intervals
//=======================================================================

void Poly_IBuffer::CloseBuffer (const Standard_Boolean isFiltering)
{
  const Standard_Real aConf2 =
    0.01 * Precision::Confusion() * Precision::Confusion();
  const Standard_Real aTol2 = myTol * myTol;
  Interval * pPrev = 0L, * pInt = const_cast<Interval *>(myIndex[0].Int);
  if (pInt) {
    pInt->U.myPrev = 0L;
    pInt->myModID = 0UL;
    pPrev = pInt;
    pInt = pInt->Next();
  }
  while (pInt) {
    if (isFiltering &&
        (pPrev->myXZ[1] - pInt->myXZ[0]).SquareModulus() < aConf2)
    {
      const gp_XY aV0 = pInt->myXZ[1] - pPrev->myXZ[0];
      const Standard_Real aL0 = aV0.SquareModulus();
      if (aL0 > aConf2) {
        const Standard_Real aVProd = aV0 ^ (pPrev->myXZ[1] - pPrev->myXZ[0]);
        if (aVProd * aVProd / aL0 < aTol2) {
          // extend pPrev to take place of pInt
          pPrev->myXZ[1] = pInt->myXZ[1];
          pPrev->myNext = pInt->Next();
          pInt = pInt->Next();
          continue;
        }
      }
    }
    pInt->U.myPrev = pPrev;
    pInt->myModID = 0UL;
    pPrev = pInt;
    pInt = pInt->Next();
  }
}

//=======================================================================
//function : addInt
//purpose  : Internal method, called from AddInterval()
//=======================================================================

Poly_IBuffer::Interval * Poly_IBuffer::addInt
                        (const Poly_IBuffer::Interval&  theInterval,
                         const Standard_Real            theX[2],
                         const Poly_IBuffer::Interval * thePrevious)
{
  const gp_XY aXZ[2] = {
    gp_XY(theX[0], GetZ (theInterval, theX[0])),
    gp_XY(theX[1], GetZ (theInterval, theX[1]))
  };
  Interval& aNewInt = theInterval.Clone(myAllocator);
  aNewInt.myXZ[0] = aXZ[0];
  aNewInt.myXZ[1] = aXZ[1];
  myNIntervals++;
  if (thePrevious)
    const_cast<Interval *>(thePrevious)->myNext = &aNewInt;
  else {
    myIndex[0].Int = &aNewInt;
    if (myMinOrd > aXZ[0].Y())
      myMinOrd = aXZ[0].Y();
    if (myMaxOrd < aXZ[0].Y())
      myMaxOrd = aXZ[0].Y();
  }
  if (myMinOrd > aXZ[1].Y())
    myMinOrd = aXZ[1].Y();
  if (myMaxOrd < aXZ[1].Y())
    myMaxOrd = aXZ[1].Y();
  return &aNewInt;
}

//=======================================================================
//function : clearIntervals
//purpose  : 
//=======================================================================

void Poly_IBuffer::clearIntervals ()
{
  if (myAllocator.IsNull() == Standard_False)
  {
    Interval * anInt = const_cast <Interval *>(myIndex[0].Int);
    while (anInt) {
      Interval * anInt0 = anInt;
      anInt = anInt->Next();
      anInt0->Interval::~Interval();
      myAllocator->Free(anInt0);
    }
    myAllocator->Free (myIndex);
  }
  myIndex = 0L;
  myNIntervals = 0;
  myNHoles = 0;
}

//=======================================================================
//function : Dump
//purpose  : Debug facility.
//=======================================================================

void Poly_IBuffer::Dump (Standard_OStream& theStream)
{
  const Interval * anInt = First();
  int aCount(0);
  for (; anInt; anInt = anInt->Next()) {
    char buf[256];
    if (anInt->XZ(0).Y() > Precision::Infinite() * 0.1 &&
        anInt->XZ(1).Y() > Precision::Infinite() * 0.1)
      continue;;
    sprintf (buf, "%03d (%8.3f %8.3f) - (%8.3f %8.3f)", ++aCount,
             anInt->XZ(0).X(), anInt->XZ(0).Y(),
             anInt->XZ(1).X(), anInt->XZ(1).Y());
    theStream << buf << endl;
  }
}

//=======================================================================
//function : DebugPolyline
//purpose  : Debug facility.
//=======================================================================

void Poly_IBuffer::DebugPolyline (Standard_OStream&   theStream,
                                  const Standard_Real theY)
{
  static int aTestCount (0);
  const Interval * anInt = First();
  Standard_Real aLastVal[2] = { Precision::Infinite(), Precision::Infinite() };
  Standard_Boolean isStarted (Standard_True);
  std::streamsize oldPrec = theStream.precision(8);
  for (; anInt; anInt = anInt->Next()) {
    if (anInt->XZ(0).Y() > Precision::Infinite() * 0.1 &&
        anInt->XZ(1).Y() > Precision::Infinite() * 0.1)
    {
      if (isStarted == Standard_False) {
        theStream << endl;
        isStarted = Standard_True;
      }
      continue;
    }
    if (isStarted) {
      theStream << "polyline ibuf" << aTestCount++;
      isStarted = Standard_False;
    }
    if (fabs(anInt->XZ(0).X() - aLastVal[0]) > Precision::Confusion() ||
        fabs(anInt->XZ(0).Y() - aLastVal[1]) > Precision::Confusion())
      theStream << "  " << anInt->XZ(0).X() << ' '
                << theY << ' ' << anInt->XZ(0).Y() << ' ';
    theStream << "  " << anInt->XZ(1).X() << ' '
              << theY << ' ' << anInt->XZ(1).Y() << ' ';
    aLastVal[0] = anInt->XZ(1).X();
    aLastVal[1] = anInt->XZ(1).Y();
  }
  theStream.precision(oldPrec);
  theStream << endl;
}

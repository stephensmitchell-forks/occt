// File:      Poly_Outline.cxx
// Created:   25.07.10 10:18
// Author:    Alexander GRIGORIEV
// Copyright: Open Cascade 2010


#include <Poly_Outline.hxx>
#include <Poly_PlaneTrianglesSection.hxx>
#include <Poly_Triangulation.hxx>
#include <Poly_PolyCurve2d.hxx>
#include <Bnd_B2d.hxx>
#ifdef _DEBUG
#include <Standard_ProgramError.hxx>
using namespace std;
#endif

/**
 * Structure that is used in iterative extraction of outline contour points.
 */
struct Poly_CntExtractor {
  Standard_Integer      IBuf;        //!< Number of the current IBuffer
  Standard_Boolean      IsUpperEnd;  //!< Is it upper or lower interval end
  Standard_Boolean      IsFinished;  //!< Indicates end of extraction
  Poly_IBuffer::Interval * PInt;     //!< Current interval
  Standard_Real         Y;           //!< Y coordinate of the last interval
  Poly_Segment2d        LastSeg;     //!< Extracted segment
};

static Standard_Boolean sectionTriangle (Standard_Real                theRes[],
                                         const gp_XY                  theNode[],
                                         const Standard_Real          theY,
                                         const Standard_Real          theTol2);
static Standard_Boolean matchIntervals  (const Poly_IBuffer::Interval * theI0,
                                         const Poly_IBuffer::Interval * theI1);
static void             extractToNext   (Poly_CntExtractor&           theExtr,
                                         const Poly_IBuffer::Interval * theInt,
                                         const Standard_Integer       isUpper,
                                         const Standard_Real          theY);
static const Poly_IBuffer::Interval *
                        getPrevious     (const Poly_IBuffer::Interval * theInt);
static const Poly_IBuffer::Interval *
                        getNext         (const Poly_IBuffer::Interval * theInt);

//! Check that the interval is valid outline interval.
inline Standard_Boolean isInterval      (const Poly_IBuffer::Interval * pInt)
{
  return (pInt->XZ(0).Y() < 0.001) && (pInt->XZ(1).Y() < 0.001);
}

//=======================================================================
//function : Poly_Outline()
//purpose  : Constructor
//=======================================================================

Poly_Outline::Poly_Outline (const Standard_Real                     theTol,
                            const Handle_NCollection_BaseAllocator& theAlloc)
  : myAlloc             (theAlloc.IsNull() ?
                         new NCollection_IncAllocator() : theAlloc),
    myTol               (theTol < Precision::Confusion() ?
                         Precision::Confusion() : theTol),
    myListTriangulation (myAlloc),
    myBuffers           (0L),
    myNBuffers          (0u),
    myOutline           (myAlloc)
{}

//=======================================================================
//function : ~Poly_Outline
//purpose  : Destructor
//=======================================================================

Poly_Outline::~Poly_Outline()
{
  if (myBuffers) {
    delete [] myBuffers;
    myBuffers = 0L;
  }
  myNBuffers = 0;
}

//=======================================================================
//function : AddTriangulation
//purpose  : 
//=======================================================================

void Poly_Outline::AddTriangulation(const Handle_Poly_Triangulation& theTri,
                                    const TopLoc_Location&           theLoc)
{
  if (theTri.IsNull() == Standard_False) {
    const LocatedTriangulation aTri = { theTri, theLoc };
    myListTriangulation.Append(aTri);
    const gp_Trsf aTrsf = theLoc;
    const TColgp_Array1OfPnt& aNodes = theTri->Nodes();
    for (Standard_Integer i = aNodes.Lower(); i <= aNodes.Upper(); i++)
    {
      myBox.Add(aNodes(i).Transformed(aTrsf));
    }
    myBox.Enlarge(myTol);
  }
}

//=======================================================================
//function : Perform
//purpose  : 
//=======================================================================

Standard_Boolean Poly_Outline::Perform (const gp_Ax3& thePln)
{
  Standard_Integer i;

  // Find the 2d bounding box of the triangulations
  if (myBox.IsVoid())
    return Standard_False;
  Bnd_B2d aBox;
  Standard_Real anX[2], anY[2], aZ[2];  // 3D box dimensions
  myBox.CornerMin().Coord(anX[0], anY[0], aZ[0]);
  myBox.CornerMax().Coord(anX[1], anY[1], aZ[1]);
  gp_XYZ aPnt;
  for (i = 0 ; i < 8; i++)
    aBox.Add(PointTo2d(gp_XYZ(anX[(i>>2) & 0x1],anY[(i>>1) & 0x1],aZ[i & 0x1]),
                       thePln));

  // Create the intervals
  if (myBuffers)
    delete [] myBuffers;

  myY0 = aBox.CornerMin().Y();
  const Standard_Real aYSpan(aBox.CornerMax().Y() - myY0);
  myNYSteps = static_cast<Standard_Integer>(aYSpan / myTol);
  if (myNYSteps > 10000)
    myNYSteps = 10000;
  myYStep = aYSpan / myNYSteps;
  myNBuffers = static_cast<Standard_Size>(myNYSteps + 1);

  myBuffers = new Poly_IBuffer[myNBuffers];
  const Poly_IBuffer::Interval anInt0 (gp_XY(aBox.CornerMin().X(), 1.),
                                       gp_XY(aBox.CornerMax().X(), 1.));
  for (i = 0; i <= myNYSteps; i++) {
    myBuffers[i].Init(anInt0, 0.1 * myTol, myAlloc);
  }

  // Fill the intervals
  fillIntervals(thePln);

#ifdef _DEBUG_
  ofstream ff("outline.d", ios_base::out);
#endif
  for (i = 0; i <= myNYSteps; i++) {
    myBuffers[i].CloseBuffer(Standard_True);
#ifdef _DEBUG_
    const Standard_Real anY(myY0 + i * myYStep);
    myBuffers[i].DebugPolyline(ff, anY);
#endif
  }

  // == Extract contours from the Intervals.
  myOutline.Clear(myAlloc);
  
  struct Poly_CntExtractor anExtr;
  Standard_Integer anI0 = 0;
  for (;;) {
    // Start the extraction - find the 1st non-visited interval.
    anExtr.PInt = 0L;
    for (anExtr.IBuf = anI0; anExtr.IBuf < myNYSteps; anExtr.IBuf++) {
      const Poly_IBuffer::Interval * pInt = myBuffers[anExtr.IBuf].First();
      while (pInt) {
        if (isInterval(pInt) && (pInt->GetModificationID() & 0x1) == 0) {
          anExtr.IsUpperEnd = Standard_False;
          anExtr.IsFinished = Standard_False;
          anExtr.PInt = const_cast<Poly_IBuffer::Interval *>(pInt);
          anExtr.Y = myY0 + anExtr.IBuf * myYStep;
          const_cast<Poly_IBuffer::Interval *>(pInt)->SetModificationID(1);
          break;
        }
        pInt = pInt->Next();
      }
      if (anExtr.PInt) {
        anI0 = anExtr.IBuf;
        break;
      }
    }

    // If no appropriate interval is found then terminate
    if (anExtr.PInt == 0L)
      break;

    // Continue the extraction.
    const Handle(Poly_PolyCurve2d) aCurve = Poly_PolyCurve2d::Create(myAlloc);
    aCurve->SetLoc(thePln);
    while (anExtr.IsFinished == Standard_False) {
      goNext(anExtr);
      aCurve->CreateSegment(anExtr.LastSeg);
    }
    // Pre-filtering, the caller can further filter if necessary.
    aCurve->Filter(0.25 * myTol);
    myOutline.Append(aCurve);
  }

  return !myOutline.IsEmpty();
}

//=======================================================================
//function : Result
//purpose  : 
//=======================================================================

const NCollection_List<Handle_Poly_PolyCurve2d>& Poly_Outline::Result()
{  
  return myOutline;
}

//=======================================================================
//function : fillIntervals
//purpose  : Populate intervals in 'myBuffers' with the data obtained from
//           all given triangles
//=======================================================================

void Poly_Outline::fillIntervals (const gp_Ax3& thePln)
{
  const Standard_Real aTol2 = 0.01 * myTol * myTol;
  NCollection_List<LocatedTriangulation>::Iterator anIter(myListTriangulation);
  // Iterate triangulations
  for (; anIter.More(); anIter.Next()) {
    const LocatedTriangulation& aTri = anIter.Value();
    const TColgp_Array1OfPnt& arrNodes = aTri.Tri->Nodes();
    const Poly_Array1OfTriangle& arrTri = aTri.Tri->Triangles();
    const gp_Trsf aTrsf = aTri.Loc.Transformation();

    // Iterate triangles in each triangulation
    Standard_Integer iTri = arrTri.Lower();
    for (; iTri <= arrTri.Upper(); iTri++) {
      const Poly_Triangle& aTriangle = arrTri(iTri);
      Standard_Integer iNode[3];
      aTriangle.Get(iNode[0], iNode[1], iNode[2]);
      gp_XY aNode[3];

      // Copy the nodes of each triangle
      gp_XYZ tmpNode = arrNodes(iNode[0]).XYZ();
      aTrsf.Transforms(tmpNode);
      aNode[0] = PointTo2d(tmpNode, thePln);

      tmpNode = arrNodes(iNode[1]).XYZ();
      aTrsf.Transforms(tmpNode);
      aNode[1] = PointTo2d(tmpNode, thePln);

      tmpNode = arrNodes(iNode[2]).XYZ();
      aTrsf.Transforms(tmpNode);
      aNode[2] = PointTo2d(tmpNode, thePln);

      // Find min-max Y values for the triangle
      Standard_Real anY[2] = { aNode[0].Y(), aNode[0].Y() }; 
      if (aNode[1].Y() < anY[0])
        anY[0] = aNode[1].Y();
      else if (aNode[1].Y() > anY[1])
        anY[1] = aNode[1].Y();
      if (aNode[2].Y() < anY[0])
        anY[0] = aNode[2].Y();
      else if (aNode[2].Y() > anY[1])
        anY[1] = aNode[2].Y();

      // Find the sections indices (lower and upper boundaries);
      Standard_Integer iSec[2] = {
        static_cast<Standard_Integer>((anY[0] - myY0) / myYStep),
        static_cast<Standard_Integer>((anY[1] - myY0) / myYStep)
      };
      if (iSec[0] < 0)
        iSec[0] = 0;
      if (iSec[1] > myNYSteps)
        iSec[1] = myNYSteps;

      // Loop over the sections
      for (Standard_Integer iS = iSec[0]; iS <= iSec[1]; iS++) {
        Poly_IBuffer& anIBuf = myBuffers[iS];
        const Standard_Real anY(myY0 + iS * myYStep);
        Standard_Real aSec[2];
        if (sectionTriangle(aSec, aNode, anY, aTol2)) {
          aSec[0] -= 0.1 * myTol;
          aSec[1] += 0.1 * myTol;
          anIBuf.AddInterval(Poly_IBuffer::Interval(gp_XY(aSec[0], 0.),
                                                    gp_XY(aSec[1], 0.)));
        }
      }
    }
  }
}

//=======================================================================
//function : sectionTriangle
//purpose  : Internal function
//           section of the triangle 'theNode' by plane Y = 'theY'
//           output is theResult[0] = Xmin, theResult[1] = Xmax
//           Returns True if section exists or False if no intersection.
//=======================================================================

Standard_Boolean sectionTriangle (Standard_Real theResult[],
                                  const gp_XY   theNode[],
                                  const Standard_Real theY,
                                  const Standard_Real theTol2)
{
  static const Standard_Real aConf = Precision::Confusion();
  static const Standard_Real aConf2 = aConf * aConf;
  Standard_Boolean aResult(Standard_False);

  Standard_Real anAbs[3] = { RealLast(), RealLast(), RealLast() };
  Standard_Real aDeltaY (theNode[1].Y() - theNode[0].Y());
  if (aDeltaY * aDeltaY > aConf2)
    anAbs[0] = (theY - theNode[0].Y()) / aDeltaY;
  aDeltaY = theNode[2].Y() - theNode[1].Y();
  if (aDeltaY * aDeltaY > aConf2)
    anAbs[1] = (theY - theNode[1].Y()) / aDeltaY;
  aDeltaY = theNode[0].Y() - theNode[2].Y();
  if (aDeltaY * aDeltaY > aConf2)
    anAbs[2] = (theY - theNode[2].Y()) / aDeltaY;

  Standard_Integer aFlag(0);
  if (anAbs[0] > aConf && anAbs[0] < 0.5*RealLast() && (1. - anAbs[0]) > aConf)
    aFlag = 0x1;
  if (anAbs[1] > aConf && anAbs[1] < 0.5*RealLast() && (1. - anAbs[1]) > aConf)
    aFlag |= 0x2;
  if (anAbs[2] > aConf && anAbs[2] < 0.5*RealLast() && (1. - anAbs[2]) > aConf)
    aFlag |= 0x4;
  switch (aFlag) {
    case 0:
    case 1:
    case 2:
    case 4:
      break;
    case 3:
      theResult[0] = anAbs[0] * theNode[1].X() + (1.-anAbs[0]) * theNode[0].X();
      theResult[1] = anAbs[1] * theNode[2].X() + (1.-anAbs[1]) * theNode[1].X();
      aResult = Standard_True;
      break;
    case 5:
      theResult[0] = anAbs[0] * theNode[1].X() + (1.-anAbs[0]) * theNode[0].X();
      theResult[1] = anAbs[2] * theNode[0].X() + (1.-anAbs[2]) * theNode[2].X();
      aResult = Standard_True;
      break;
    case 6:
      theResult[0] = anAbs[1] * theNode[2].X() + (1.-anAbs[1]) * theNode[1].X();
      theResult[1] = anAbs[2] * theNode[0].X() + (1.-anAbs[2]) * theNode[2].X();
      aResult = Standard_True;
      break;
    default:;
#ifdef _DEBUG
      Standard_ProgramError::Raise("Poly_Outline: Algorithm failure");
#endif
  }
  if (aResult) {
    const Standard_Real aDiffX(theResult[1] - theResult[0]);
    if (aDiffX * aDiffX < theTol2)
      aResult = Standard_False;
    else if (aDiffX < 0.) {
      const Standard_Real aTmp = theResult[0];
      theResult[0] = theResult[1];
      theResult[1] = aTmp;
    }
  }
  return aResult;
}

//=======================================================================
//function : goNext
//purpose  : Find the next point of the currently extracted curve and create
//           the extracted segment in 'theExtr'.
//           Returns True if the search successful.
//=======================================================================

void Poly_Outline::goNext (Poly_CntExtractor& theExtr)
{
  gp_XY aNextPoint(0., 0.);
  const Poly_IBuffer::Interval * pInt = 0L;
  const Standard_Real anY(myY0 + theExtr.IBuf * myYStep);

  if (theExtr.IsUpperEnd == Standard_False)
  {
    if (static_cast<Standard_Size>(theExtr.IBuf) < myNBuffers) {
      // Find the first matching interval in Buffer(i+1)
      pInt = myBuffers[theExtr.IBuf+1].First();
      while (pInt) {
        if (isInterval(pInt))  // do not test trash intervals
        {
          if (matchIntervals(pInt, theExtr.PInt))
            break;
        }
        pInt = pInt->Next();
      }
    }
    if (pInt == 0L) {
      // The next matching interval is not found in the next IBuffer.
      // Continue with the upper end of the current interval.
      extractToNext (theExtr, theExtr.PInt, 1, anY);
    } else {
      // The next IBuffer contains a matching interval 'pInt'
      const Poly_IBuffer::Interval * pPrev = getPrevious(theExtr.PInt);
      if (pPrev != 0L && matchIntervals(pPrev, pInt)) {
        // If the current IBuffer has the previous interval also matching
        // 'pInt' of the next IBuffer then we go to the end of the previous.
        extractToNext (theExtr, pPrev, 1, anY);
      } else {
        // Continue at the beginning of 'pInt' of the next IBuffer
        extractToNext (theExtr, pInt, 0, anY + myYStep);
        theExtr.IBuf++;
      }
    }
  }
  else {        // theExtr.IsUpperEnd == Standard_True
    if (theExtr.IBuf > 0) {
      // Find the last matching interval in Buffer(i-1)
      const Poly_IBuffer::Interval * pItest = myBuffers[theExtr.IBuf-1].First();
      while (pItest) {
        if (isInterval(pItest))  // do not test trash intervals
        {
          if (matchIntervals(pItest, theExtr.PInt))
            pInt = pItest;
          else if (pInt != 0L)
            break;
        }
        pItest = pItest->Next();
      }
    }
    if (pInt == 0L) {
      // The previous matching interval is not found in the next IBuffer.
      // Continue with the lower end of the current interval.
      extractToNext (theExtr, theExtr.PInt, 0, anY);
    } else {
      // The previous IBuffer contains a matching interval 'pInt'
      const Poly_IBuffer::Interval * pNext = getNext(theExtr.PInt);
      if (pNext != 0L && matchIntervals(pNext, pInt)) {
        // If the current IBuffer has the next interval also matching 'pInt'
        // of the previous IBuffer then we go to the beginning of the next.
        extractToNext (theExtr, pNext, 0, anY);
      } else {
        // Continue at the end of 'pInt' of the previous IBuffer
        extractToNext (theExtr, pInt, 1, anY - myYStep);
        theExtr.IBuf--;
      }
    }    
  }
}

//=======================================================================
//function : matchIntervals
//purpose  : 
//=======================================================================

Standard_Boolean matchIntervals (const Poly_IBuffer::Interval * theInt0,
                                 const Poly_IBuffer::Interval * theInt1)
{
  return ((theInt0->XZ(1).X() > theInt1->XZ(0).X()) &&
          (theInt1->XZ(1).X() > theInt0->XZ(0).X()));
}

//=======================================================================
//function : extractToNext
//purpose  : 
//=======================================================================

void extractToNext (Poly_CntExtractor&           theExtr,
                    const Poly_IBuffer::Interval * theInt,
                    const Standard_Integer       isUpper,
                    const Standard_Real          theY)
{
  const gp_XY aPnt[2] = {
    gp_XY(theExtr.PInt->XZ(theExtr.IsUpperEnd).X(), theExtr.Y),
    gp_XY(theInt->XZ(isUpper).X(), theY)
  };
  theExtr.LastSeg = Poly_Segment2d(aPnt[0], aPnt[1]);
  theExtr.PInt = const_cast<Poly_IBuffer::Interval *>(theInt);
  theExtr.Y = theY;
  theExtr.IsUpperEnd = isUpper;

  const unsigned long aModID = theInt->GetModificationID();
  const unsigned long aNewID = aModID | (isUpper + 1);
  const_cast<Poly_IBuffer::Interval *>(theInt)->SetModificationID(aNewID);
  theExtr.IsFinished = (aModID == aNewID);
}

//=======================================================================
//function : getPrevious
//purpose  : 
//=======================================================================

const Poly_IBuffer::Interval * getPrevious
                                (const Poly_IBuffer::Interval * theInt)
{
  for (;;) {
    theInt = theInt->Previous();
    if (theInt == 0L)
      break;
    if (isInterval(theInt))
      break;
  }
  return theInt;
}

//=======================================================================
//function : getNext
//purpose  : 
//=======================================================================

const Poly_IBuffer::Interval * getNext (const Poly_IBuffer::Interval * theInt)
{
  for (;;) {
    theInt = theInt->Next();
    if (theInt == 0L)
      break;
    if (isInterval(theInt))
      break;
  }
  return theInt;
}


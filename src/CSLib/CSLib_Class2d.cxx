// Created on: 1995-03-08
// Created by: Laurent BUCHARD
// Copyright (c) 1995-1999 Matra Datavision
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

//#define No_Standard_OutOfRange

#include <CSLib_Class2d.hxx>
#include <gp_Pnt2d.hxx>
#include <Standard_ConstructionError.hxx>
#include <Precision.hxx>

static inline Standard_Boolean IsOn(const Standard_Real theTolU,
                                    const Standard_Real theTolV,
                                    const Standard_Real thePrevX,
                                    const Standard_Real thePrevY,
                                    const Standard_Real theDx,
                                    const Standard_Real theDy);


//=======================================================================
//function : Constructor
//purpose  : 
//=======================================================================
CSLib_Class2d::CSLib_Class2d(const TColgp_Array1OfPnt2d& theTP2d,
                             const Standard_Real theTolu,
                             const Standard_Real theTolv,
                             const Standard_Real theUmin,
                             const Standard_Real theVmin,
                             const Standard_Real theUmax,
                             const Standard_Real theVmax) : myPnts2d(0,
                                                                theTP2d.Length()),
                                                            myTolU(theTolu),
                                                            myTolV(theTolv),
                                                            myUmin(theUmin),
                                                            myUmax(theUmax),
                                                            myVmin(theVmin),
                                                            myVmax(theVmax)
{
  const size_t aN = theTP2d.Length();
  if((theUmax <= theUmin) || (theVmax <= theVmin) || (aN == 0))
  {
    // Nothing will be created.
    return;
  }

  myTolU = Max(myTolU, Precision::PConfusion());
  myTolV = Max(myTolV, Precision::PConfusion());

  // The classification algorithm works incorrectly
  // some segment(s) of the polygon has null length.
  // The code blow forbid creation of such polygons.

  Standard_Integer aCurrIdx = myPnts2d.Lower();
  gp_Pnt2d aPPrev = theTP2d(theTP2d.Lower());
  myPnts2d(aCurrIdx).SetCoord(aPPrev.X() - theUmin, aPPrev.Y() - theVmin);
  for (Standard_Integer i = theTP2d.Lower() + 1; i < theTP2d.Upper(); ++i)
  {
    const gp_Pnt2d& aPCur = theTP2d(i);

    if ((Abs(aPPrev.X() - aPCur.X()) <= myTolU) &&
        (Abs(aPPrev.Y() - aPCur.Y()) <= myTolV))
    {
      continue;
    }

    myPnts2d(++aCurrIdx).SetCoord(aPCur.X() - theUmin, aPCur.Y() - theVmin);
    aPPrev = aPCur;
  }

  {
    aPPrev = theTP2d(theTP2d.Lower());
    const gp_Pnt2d& aPCur = theTP2d(theTP2d.Upper());
    if ((Abs(aPPrev.X() - aPCur.X()) > myTolU) || 
        (Abs(aPPrev.Y() - aPCur.Y()) > myTolV))
    {
      myPnts2d(++aCurrIdx).SetCoord(aPCur.X() - theUmin, aPCur.Y() - theVmin);
    }

    myPnts2d(++aCurrIdx).SetCoord(aPPrev.X() - theUmin, aPPrev.Y() - theVmin);
  }

  if (aCurrIdx < myPnts2d.Upper())
  {
    myPnts2d.Resize(myPnts2d.Lower(), aCurrIdx, Standard_True);
  }
}

//=======================================================================
//function : Classify
//purpose  : 
//=======================================================================
CSLib_Class2d::PolyState CSLib_Class2d::Classify(const gp_Pnt2d& theP) const
{
  return Classify(theP, myTolU, myTolV);
}

//=======================================================================
//function : Classify
//purpose  : 
//=======================================================================
CSLib_Class2d::PolyState CSLib_Class2d::Classify(const gp_Pnt2d& theP,
                                                 const Standard_Real theTolU,
                                                 const Standard_Real theTolV) const
{ 
  if ((myPnts2d.Size() < 3) || (myUmin > myUmax) || (myVmin > myVmax))
  {
    // Return 0 for compatibility with
    // previous versions of OCCT (it does not
    // mean that the status is ON).

#ifdef OCCT_DEBUG
    std::cout << "CSLib_Class2d::Classify(...). No polygon "
                 "for classification!" << std::endl;
#endif

    return (PolyState)0;
  }
  //
  Standard_Real aX = theP.X(), aY = theP.Y();
  if ((theTolU < (myUmin - aX)) ||
      ((aX - myUmax) > theTolU) ||
      (theTolV < (myVmin - aY)) ||
      ((aY - myVmax) > theTolV))
  {
    // The point is outside of the UV-bounds
    return PolyOut;
  }

  aX -= myUmin;
  aY -= myVmin;

  return InternalClass(aX, aY, theTolU, theTolV);
}

//=======================================================================
//function : InternalClass
//purpose  : Checks if the point P(thePx, thePy) is inside the
//            given convex polygon.
//
// Algorithm description:
//
//  1. Catch the moment when some segment of the polygon
//      intersects the ray from point P in direction {1, 0}.
//  2. Compute the number of intersections between this ray and
//      the segments of this polygon.
//  3. The point P is inside if the number of intersections is even.
//      The point P is outside if the number of intersections is odd.
//
//  This method works wrong in case of tangent the ray with some
//  segment of polygon. This case must be investigated additionally.
//  Therefore, the method will return 0 (as ON-status).
//
//  ON-status is checked in parallel mode of this algorithm. It is
//  made by the following steps:
//
//  1. A vector V is created joining P with the nearest point of the
//    considered segment.
//  2. V is expanded to the two not collinear components: DU and DV.
//  3. If every coefficient of expanding is less than the corresponding
//    tolerance (theTolU and theTolV) then the ON-status is returned.
//
// ATTENTION!!!
//  theTolU or/and theTolV can be equal to 0.
//=======================================================================
CSLib_Class2d::PolyState CSLib_Class2d::InternalClass(const Standard_Real thePx,
                                                      const Standard_Real thePy,
                                                      const Standard_Real theTolU,
                                                      const Standard_Real theTolV) const
{
#ifdef CSLIB_CLASS2D_DEBUG
  bool flShow = false;

  if (flShow)
  {
    std::cout << "+++ Dump of CSLib_Class2d polygon ++" << std::endl;
    std::cout << "plane pl 0 0 0 0 0 1" << std::endl;
    std::cout << "polyclassify -s -c " << thePx+myUmin << " " << thePy + myVmin <<
                 " -t " << theTolU << " " << theTolV << " ";
    for (Standard_Integer i = myPnts2d.Lower(); i <= myPnts2d.Upper(); i++)
    {
      const gp_Pnt2d &aPCurr = myPnts2d(i);
      std::cout << "-p " << aPCurr.X() + myUmin << " " << aPCurr.Y() + myVmin << " ";
    }
    std::cout << std::endl << "mkface ff pl wirsrc" << std::endl;
    std::cout << "v2d; pcurve ff" << std::endl;

    std::cout << "--- Dump of CSLib_Class2d polygon --" << std::endl << std::endl;
  }
#endif

  // The source polygon is translated.
  // So, classified point has (0, 0) - coordinates.
  
  Standard_Real aPrevX = myPnts2d(0).X() - thePx,
                aPrevY = myPnts2d(0).Y() - thePy;

  if ((Abs(aPrevX) <= theTolU) && (Abs(aPrevY) <= theTolV))
  {
    //If the classified point is in the vertex of
    //the polygon
    return PolyOn;
  }

  const Standard_Real aNullTolV = Epsilon(myVmax - myVmin);

  CSLib_Class2d::PolyState aRetVal = PolyOut;

  const Standard_Integer aN = myPnts2d.Size();
  for (Standard_Integer i = 1; i < aN; i++)
  {
    const Standard_Real aNewX = myPnts2d(i).X() - thePx,
                        aNewY = myPnts2d(i).Y() - thePy;

    const Standard_Real aDx = aNewX - aPrevX;
    const Standard_Real aDy = aNewY - aPrevY;

    if (Abs(aNewY) <= theTolV)
    {
      // The ray goes through some vertex of the polygon

      if (Abs(aPrevY) < theTolV)
      {
        // Tangential case. Must be investigated by other methods.
        return (PolyState) 0;
      }

      if (Abs(aNewX) <= theTolU)
      {
        //If the classified point is in the vertex of
        //the polygon
        return PolyOn;
      }

      if ((Abs(aNewY) <= aNullTolV) && (aNewX > 0.0))
      {
        // Ray goes exactly through the vertex of the polygon.

        Standard_Real aY1 = aNewY;
        if (i < aN - 1)
        {
          aY1 = myPnts2d(i + 1).Y() - thePy;
        }
        else
        {
          aY1 = myPnts2d(myPnts2d.Lower() + 1).Y() - thePy;
        }

        if (aY1*aPrevY < 0.0)
        {
          // Case like this

          //        o
          //      o
          //x___o_____
          //    *
          //    *
          //    * 

          // x - the classified point;
          // ___ - ray Y=thePy
          // *** and ooo - adjacent segments of polygon

          //Change status IN <--> OUT;
          aRetVal = (PolyState) -aRetVal;
        }

        // Case like this

        //x________ 
        //     *o
        //    *  o
        //   *    o

        // must be ignored.

        if (IsOn(theTolU, theTolV, aPrevX, aPrevY, aDx, aDy))
        {
          return PolyOn;
        }

        aPrevX = aNewX;
        aPrevY = aNewY;
        continue;
      }
    }

    if (IsOn(theTolU, theTolV, aPrevX, aPrevY, aDx, aDy))
    {
      return PolyOn;
    }

    if (Abs(aPrevY) <= aNullTolV)
    {
      // Ray goes exactly through the vertex of the polygon.
      // This case was processed earlier.

      aPrevX = aNewX;
      aPrevY = aNewY;
      continue;
    }

    if (aNewY*aPrevY <= 0.0)
    {
      // The current segment intersects the line Y=thePy.
      // X-coord. of the intersection point is
      const Standard_Real aXi = aPrevX - aPrevY*aDx / aDy;

      //Note.
      // (aDy==0.0) if and only if (aNewY==aPrevY).
      // At that, (aNewY*aPrevY <= 0.0) <==> (aNewY==aPrevY==0.0).
      // Therefore, the condition above must be satisfied and
      // the method will return 0.
      // Consequently, here (aDy != 0.0) and aXi is found unambiguously.

      if (aXi > 0.0)
      {
        // The current segment intersects the ray Y=thePy

        //Change status IN <--> OUT;
        aRetVal = (PolyState) -aRetVal;
      }
    }

    aPrevX = aNewX;
    aPrevY = aNewY;
  }

  return aRetVal;
}

//=======================================================================
//function : IsOn
//purpose  : If the line (which the current segment lies in) has
//            the location L(thePrevX, thePrevY) and the direction
//            d{theDx, theDy} then the parameter of the classified
//            point (0, 0) on the line (i.e. the projection point) is
//              t = (d*V)/|| d || = -(aPrevX*aDx+aPrevY*aDy)/|| d ||,
//            where V{-thePrevX, -thePrevY} is the vector joining L with
//            the classified point (0, 0).
//            If the projection point is IN the current segment then
//            the inequation
//              0 <= t <= || d ||
//            must be satisfied.
//            Or
//              0 <= -(thePrevX*theDx+thePrevY*theDy) <= || d ||^2.
//=======================================================================
static inline Standard_Boolean IsOn(const Standard_Real theTolU,
                                    const Standard_Real theTolV,
                                    const Standard_Real thePrevX,
                                    const Standard_Real thePrevY,
                                    const Standard_Real theDx,
                                    const Standard_Real theDy)
{
  //Square length of the considered segment
  //aSqL != 0 (see algorithm of polygon building in constructor).
  const Standard_Real aSqL = theDx*theDx + theDy*theDy;
  const Standard_Real aDP = theDx*thePrevX + theDy*thePrevY;
  if ((-aSqL <= aDP) && (aDP <= 0.0))
  {
    // The projection point has coordinates
    //    L + t*d/|| d || = L - d*(aPrevX*aDx+aPrevY*aDy)/aSqL

    const Standard_Real aXNear = thePrevX - aDP*theDx / aSqL;
    const Standard_Real aYNear = thePrevY - aDP*theDy / aSqL;

    if ((Abs(aXNear) <= theTolU) && (Abs(aYNear) <= theTolV))
    {
      // The distance between the projection point and 
      // the classified point (0, 0) is in the given tolerances
      return Standard_True;
    }
  }

  return Standard_False;
}

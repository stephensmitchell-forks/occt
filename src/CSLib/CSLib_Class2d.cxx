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

  for (Standard_Integer i = myPnts2d.Lower(), j = theTP2d.Lower(); 
                                              i < myPnts2d.Upper(); ++i, j++)
  {
    const gp_Pnt2d& aP2D = theTP2d(j);
    myPnts2d(i).SetCoord(aP2D.X() - theUmin, aP2D.Y() - theVmin);
  }

  myPnts2d.SetValue(myPnts2d.Upper(), myPnts2d(myPnts2d.Lower()));

#ifdef CSLIB_CLASS2D_DEBUG
  bool flShow = false;

  if (flShow)
  {
    std::cout << "+++ Dump of CSLib_Class2d polygon ++" << std::endl;

    for (Standard_Integer i = 0; i < aN; i++)
    {
      const gp_Pnt2d &aPCurr = myPnts2d(i),
                     &aPNext = myPnts2d(i + 1);
      const Standard_Real aDX = aPNext.X() - aPCurr.X(),
                          aDY = aPNext.Y() - aPCurr.Y();
      
      if ((aDX*aDX + aDY*aDY) < gp::Resolution())
        continue;
      
      std::cout << "line l" << i << " " << aPCurr.X() + theUmin << " " <<
                                           aPCurr.Y() + theVmin << " " <<
                                           aDX << " " <<
                                           aDY << std::endl;

      std::cout << "trim l" << i << " l" << i << " " <<
                            0.0 << " " << aPCurr.Distance(aPNext) << std::endl;
    }
    std::cout << "--- Dump of CSLib_Class2d polygon --" << std::endl << std::endl;
  }
#endif
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
  if (!myPnts2d.Size())
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

  if (myUmin < myUmax && myVmin<myVmax)
  {
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
  }

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
//=======================================================================
CSLib_Class2d::PolyState CSLib_Class2d::InternalClass(const Standard_Real thePx,
                                                       const Standard_Real thePy,
                                                       const Standard_Real theTolU,
                                                       const Standard_Real theTolV) const
{
  // The source polygon is translated.
  // So, classified point has (0, 0) - coordinates.
  
  Standard_Real aPrevX = myPnts2d(0).X() - thePx,
                aPrevY = myPnts2d(0).Y() - thePy;

  if ((Abs(aPrevX) < theTolU) && (Abs(aPrevY) < theTolV))
  {
    //If the classified point is in the vertex of
    //the polygon
    return PolyOn;
  }

  CSLib_Class2d::PolyState aRetVal = PolyOut;

  const Standard_Integer aN = myPnts2d.Size();
  for (Standard_Integer i = 1; i < aN; i++)
  {
    const Standard_Real aNewX = myPnts2d(i).X() - thePx,
                        aNewY = myPnts2d(i).Y() - thePy;

    const Standard_Real aDx = aNewX - aPrevX;
    const Standard_Real aDy = aNewY - aPrevY;

    if (Abs(aNewY) < theTolV)
    {
      // The ray goes through some vertex of the polygon

      if (Abs(aDy) < gp::Resolution())
      {
        // Tangential case. Must be investigated by other methods.
        return (PolyState) 0;
      }

      if (Abs(aNewX) < theTolU)
      {
        //If the classified point is in the vertex of
        //the polygon
        return PolyOn;
      }

      if (aNewX > 0.0)
      {
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

        aPrevX = aNewX;
        aPrevY = aNewY;
        continue;
      }
    }

    if (Abs(aPrevY) < theTolV)
    {
      aPrevX = aNewX;
      aPrevY = aNewY;
      continue;
    }

    //Square length of the considered segment
    const Standard_Real aSqL = aDx*aDx + aDy*aDy;
    if (!IsEqual(aSqL, 0.0))
    {
      // If the line (which the current segment lies in) has
      // the location L(aPrevX, aPrevY) and the direction
      // d{aDx, aDy} then the parameter of the classified point (0, 0)
      // on the line (i.e. the projection point) is
      //      t = (d*V)/|| d || = -(aPrevX*aDx+aPrevY*aDy)/|| d ||,
      // where V{-aPrevX, -aPrevY} is the vector joining L with
      // the classified point (0, 0).
      // If the projection point is IN the current segment then
      // the inequation
      //      0 <= t <= || d ||
      // must be satisfied.
      //
      // Or
      //      0 <= -(aPrevX*aDx+aPrevY*aDy) <= aSqL

      const Standard_Real aDP = aDx*aPrevX + aDy*aPrevY;
      if ((-aSqL <= aDP) && (aDP <= 0.0))
      {
        // The projection point has coordinates
        //    L + t*d/|| d || = L - d*(aPrevX*aDx+aPrevY*aDy)/aSqL

        const Standard_Real aXNear = aPrevX - aDP*aDx / aSqL;
        const Standard_Real aYNear = aPrevY - aDP*aDy / aSqL;

        if ((Abs(aXNear) < theTolU) && (Abs(aYNear) < theTolV))
        {
          // The distance between the projection point and 
          // the classified point (0, 0) is in the given tolerances
          return PolyOn;
        }
      }
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

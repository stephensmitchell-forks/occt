// File:      NIS_Point.cpp
// Created:   12.02.09
// Author:    Natalia Kopnova
// Copyright: LPKF Laser and Electronics AG 2008

#include <NIS_Point.hxx>
#include <NIS_PointDrawer.hxx>
#include <NIS_InteractiveContext.hxx>
#include <NIS_Triangulated.hxx>
#include <Precision.hxx>
#include <gp_Ax1.hxx>

IMPLEMENT_STANDARD_HANDLE  (NIS_Point, NIS_InteractiveObject)
IMPLEMENT_STANDARD_RTTIEXT (NIS_Point, NIS_InteractiveObject)

//=======================================================================
//function : NIS_Point
//purpose  : Constructor
//=======================================================================

NIS_Point::NIS_Point
                (const gp_XY&                            thePoint,
                 const Standard_Real                     theZ)
  : myIs3D              (Standard_False)
{
    //   SetAttributeId(0); // agv: already initialized in the base class
    Init(thePoint, theZ);
}

//=======================================================================
//function : NIS_Point
//purpose  : Constructor
//=======================================================================

NIS_Point::NIS_Point
                (const gp_XYZ&                            thePoint)
  : myIs3D              (Standard_True),
    myZ                 (RealLast())
{
    //   SetAttributeId(0); // agv: already initialized in the base class
    Init(thePoint);
}

//=======================================================================
//function : Init
//purpose  : Re-initialize the internal structures, re-compute the presentation
//=======================================================================

void NIS_Point::Init(const gp_XY&        thePoint,
                           const Standard_Real theZ)
{
    myZ = theZ;

    myCoords[0] = static_cast<Standard_ShortReal> (thePoint.X());
    myCoords[1] = static_cast<Standard_ShortReal> (thePoint.Y());
    myCoords[2] = static_cast<Standard_ShortReal> (myZ);

    if (GetDrawer().IsNull() == Standard_False)
    {
        setDrawerUpdate();
    }
    setIsUpdateBox(Standard_True);
}

//=======================================================================
//function : Init
//purpose  : Re-initialize the internal structures, re-compute the presentation
//=======================================================================

void NIS_Point::Init(const gp_XYZ& thePoint)
{
    myCoords[0] = static_cast<Standard_ShortReal> (thePoint.X());
    myCoords[1] = static_cast<Standard_ShortReal> (thePoint.Y());
    myCoords[2] = static_cast<Standard_ShortReal> (thePoint.Z());

    if (GetDrawer().IsNull() == Standard_False)
    {
        setDrawerUpdate();
    }
    setIsUpdateBox(Standard_True);
}

//=======================================================================
//function : ~NIS_Point
//purpose  : Destructor
//=======================================================================

NIS_Point::~NIS_Point()
{
}

//=======================================================================
//function : DefaultDrawer
//purpose  :
//=======================================================================

NIS_Drawer* NIS_Point::DefaultDrawer (NIS_Drawer* theDrawer) const
{
    if (theDrawer == 0L)
    {
        return new NIS_PointDrawer(Quantity_NOC_GREEN);
    }
    return theDrawer;
}

//=======================================================================
//function : Clone
//purpose  : 
//=======================================================================

void NIS_Point::Clone
                        (const Handle_NCollection_BaseAllocator& theAlloc,
                         Handle_NIS_InteractiveObject&           theDest) const
{
    Handle(NIS_Point) aNewObj;
    if (theDest.IsNull())
    {
        aNewObj = new NIS_Point;
        theDest = aNewObj;
    }
    else
    {
        aNewObj = reinterpret_cast<NIS_Point*> (theDest.operator->());
    }
    NIS_InteractiveObject::Clone(theAlloc, theDest);
    aNewObj->myIs3D = myIs3D;
    aNewObj->myZ = myZ;
    aNewObj->myCoords[0] = myCoords[0];
    aNewObj->myCoords[1] = myCoords[1];
    aNewObj->myCoords[2] = myCoords[2];
}

//=======================================================================
//function : SetPointSize
//purpose  : Set the size of point presentations in pixels.
//=======================================================================

void NIS_Point::SetPointSize (const Standard_Real theSize)
{
    const Handle(NIS_PointDrawer) aDrawer =
        static_cast<NIS_PointDrawer*>(DefaultDrawer(0L));
    aDrawer->Assign(GetDrawer());
    aDrawer->myPointSize = static_cast<Standard_ShortReal>(theSize);
    SetDrawer(aDrawer);
}

//=======================================================================
//function : SetMarkerType
//purpose  : Set the type of point presentation.
//=======================================================================

void NIS_Point::SetMarkerType (const NIS_MarkerType theType)
{
    const Handle(NIS_PointDrawer) aDrawer =
        static_cast<NIS_PointDrawer*>(DefaultDrawer(0L));
    aDrawer->Assign(GetDrawer());
    aDrawer->myType = static_cast<Standard_Integer>(theType);
    SetDrawer(aDrawer);
}

//=======================================================================
//function : SetInvertColor
//purpose  : Indicates whether to draw point depending on underlying objects
//=======================================================================

void NIS_Point::SetInvertColor (const Standard_Boolean theInvert)
{
    const Handle(NIS_PointDrawer) aDrawer =
        static_cast<NIS_PointDrawer*>(DefaultDrawer(0L));
    aDrawer->Assign(GetDrawer());
    aDrawer->myInvertColor = static_cast<Standard_Boolean>(theInvert);
    SetDrawer(aDrawer);
    //   // Use "trasparency" layer for drawing (after all objects)
    //   SetTransparency();
}

//=======================================================================
//function : SetAngle
//purpose  : 
//=======================================================================

void NIS_Point::SetAngle (const Standard_Real theAngle)
{
    const Handle(NIS_PointDrawer) aDrawer =
        static_cast<NIS_PointDrawer*>(DefaultDrawer(0L));
    aDrawer->Assign(GetDrawer());
    Standard_Real anAngle = theAngle;
    while (anAngle > M_PI / 2.0)
    {
        anAngle -= M_PI;
    }
    while (anAngle < - M_PI / 2.0)
    {
        anAngle += M_PI;
    }
    if (std::abs(aDrawer->myAngle - anAngle) > Precision::Angular())
    {
        aDrawer->myAngle = anAngle;
        aDrawer->myIsUpdateAngle = Standard_True;
    }
    SetDrawer(aDrawer);
}

//=======================================================================
//function : Intersect
//purpose  :
//=======================================================================

Standard_Boolean NIS_Point::Intersect
                                        (const Bnd_B3f&         theBox,
                                         const gp_Trsf&         theTrf,
                                         const Standard_Boolean /*isFullIn*/) const
{
    gp_XYZ aPnt (static_cast<Standard_Real>(myCoords[0]),
                 static_cast<Standard_Real>(myCoords[1]),
                 static_cast<Standard_Real>(myCoords[2]));
    theTrf.Transforms(aPnt);
    return !theBox.IsOut(aPnt);
}

//=======================================================================
//function : Intersect
//purpose  :
//=======================================================================

Standard_Real NIS_Point::Intersect  (const gp_Ax1&       theAxis,
                                           const Standard_Real theOver) const
{
    Standard_Real aResult (RealLast());
    // We extend the intersect distance to facilitate selection
    const Standard_Real anOver2 = theOver * theOver * 5.;

    // check if the point is closed to line
    const gp_XYZ& aStart = theAxis.Location().XYZ();
    const gp_XYZ& aDir = theAxis.Direction().XYZ();
    const gp_XYZ aDelta(static_cast<Standard_Real>(myCoords[0]) - aStart.X(),
                        static_cast<Standard_Real>(myCoords[1]) - aStart.Y(),
                        static_cast<Standard_Real>(myCoords[2]) - aStart.Z());
    if ((aDelta ^ aDir).SquareModulus() < anOver2)
    {
        aResult = aDelta * aDir;
    }

    return aResult;
}

//=======================================================================
//function : Intersect
//purpose  :
//=======================================================================

Standard_Boolean NIS_Point::Intersect
                         (const NCollection_List<gp_XY> &thePolygon,
                          const gp_Trsf                 &theTrf,
                          const Standard_Boolean         /*isFullIn*/) const
{
    gp_XYZ aPnt (static_cast<Standard_Real>(myCoords[0]),
                 static_cast<Standard_Real>(myCoords[1]),
                 static_cast<Standard_Real>(myCoords[2]));
    theTrf.Transforms(aPnt);

    gp_XY aP2d(aPnt.X(), aPnt.Y());
    return NIS_Triangulated::IsIn(thePolygon, aP2d);
}

//=======================================================================
//function : computeBox
//purpose  :
//=======================================================================

void NIS_Point::computeBox ()
{
    myBox.Clear();
    myBox.Add (gp_XYZ(static_cast<Standard_Real>(myCoords[0]),
                      static_cast<Standard_Real>(myCoords[1]),
                      static_cast<Standard_Real>(myCoords[2])));
    myBox.Enlarge(2.0f); // enlarge a sensetive area
}

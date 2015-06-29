// File:      NIS_Line.cpp
// Created:   11.05.12
// Author:    Natalia Kopnova
// Copyright: LPKF Laser and Electronics AG 2012

#include <NIS_Line.hxx>
#include <NIS_LineDrawer.hxx>
#include <NIS_InteractiveContext.hxx>
#include <Precision.hxx>
#include <UnitsAPI.hxx>

IMPLEMENT_STANDARD_HANDLE  (NIS_Line, NIS_InteractiveObject)
IMPLEMENT_STANDARD_RTTIEXT (NIS_Line, NIS_InteractiveObject)

#define MAX_WIDTH 500000.

//=======================================================================
//function : NIS_Line
//purpose  : Constructor
//=======================================================================

NIS_Line::NIS_Line(const gp_Ax1& theAxis)
{
    //   SetAttributeId(0);
    Init(theAxis);
}

//=======================================================================
//function : Init
//purpose  : Re-initialize the internal structures, re-compute the presentation
//=======================================================================

void NIS_Line::Init(const gp_Ax1& theAxis)
{
    myLine = theAxis;

    const Handle(NIS_LineDrawer) aDrawer =
        Handle(NIS_LineDrawer)::DownCast(GetDrawer());

    // Standard_Real aLength = UnitsAPI::AnyToLS( 250000., "mm" );
    const gp_XYZ aLength = (0.5 * MAX_WIDTH) * myLine.Direction().XYZ();

    gp_XYZ aPoint1;

    if(aDrawer.IsNull())
    {
        aPoint1 = myLine.Location().XYZ();
    }
    else
    {
        if ( aDrawer->myLineType == NIS_LT_Line )
        {
            aPoint1 = myLine.Location().XYZ() - aLength;
        }
        else
        {
            aPoint1 = myLine.Location().XYZ();
        }
    }

    myPoint1[0] = static_cast<Standard_ShortReal> (aPoint1.X());
    myPoint1[1] = static_cast<Standard_ShortReal> (aPoint1.Y());
    myPoint1[2] = static_cast<Standard_ShortReal> (aPoint1.Z());

    const gp_XYZ aPoint2 = myLine.Location().XYZ() + aLength;
    myPoint2[0] = static_cast<Standard_ShortReal> (aPoint2.X());
    myPoint2[1] = static_cast<Standard_ShortReal> (aPoint2.Y());
    myPoint2[2] = static_cast<Standard_ShortReal> (aPoint2.Z());

    // setIsUpdateBox(Standard_True);
}

//=======================================================================
//function : ~NIS_Line
//purpose  : Destructor
//=======================================================================

NIS_Line::~NIS_Line()
{
}

//=======================================================================
//function : DefaultDrawer
//purpose  :
//=======================================================================

NIS_Drawer* NIS_Line::DefaultDrawer (NIS_Drawer* theDrawer) const
{
    if (theDrawer == 0L)
    {
        return new NIS_LineDrawer(Quantity_NOC_YELLOW);
    }
    NIS_LineDrawer* aDrawer = static_cast<NIS_LineDrawer*>(theDrawer);
    return aDrawer;
}

//=======================================================================
//function : Clone
//purpose  : 
//=======================================================================

void NIS_Line::Clone
                        (const Handle_NCollection_BaseAllocator& theAlloc,
                         Handle_NIS_InteractiveObject&           theDest) const
{
    Handle(NIS_Line) aNewObj;
    if (theDest.IsNull())
    {
        aNewObj = new NIS_Line(myLine);
        theDest = aNewObj;
    }
    else
    {
        aNewObj = reinterpret_cast<NIS_Line*> (theDest.operator->());
        aNewObj->myLine = myLine;
        aNewObj->myPoint1[0] = myPoint1[0];
        aNewObj->myPoint1[1] = myPoint1[1];
        aNewObj->myPoint1[2] = myPoint1[2];
        aNewObj->myPoint2[0] = myPoint2[0];
        aNewObj->myPoint2[1] = myPoint2[1];
        aNewObj->myPoint2[2] = myPoint2[2];
    }
    NIS_InteractiveObject::Clone(theAlloc, theDest);
}

//=======================================================================
//function : SetLineWidth
//purpose  : Set the line width in pixels.
//=======================================================================

void NIS_Line::SetLineWidth (const Standard_Real theSize)
{
    const Handle(NIS_LineDrawer) aDrawer =
        static_cast<NIS_LineDrawer*>(DefaultDrawer(0L));
    aDrawer->Assign(GetDrawer());
    aDrawer->myLineWidth = static_cast<Standard_ShortReal>(theSize);
    SetDrawer(aDrawer);
}

//=======================================================================
//function : SetLineStyle
//purpose  : Set the style of line presentation.
//=======================================================================

void NIS_Line::SetLineStyle  (const NIS_LineStyle theStyle)
{
    const Handle(NIS_LineDrawer) aDrawer =
        static_cast<NIS_LineDrawer*>(DefaultDrawer(0L));
    aDrawer->Assign(GetDrawer());
    aDrawer->myType = static_cast<Standard_Integer>(theStyle);
    SetDrawer(aDrawer);
}


//=======================================================================
//function : SetLineType
//purpose  : Set the type of line presentation.
//=======================================================================

void NIS_Line::SetLineType  (const NIS_LineType theType)
{
    const Handle(NIS_LineDrawer) aDrawer =
        static_cast<NIS_LineDrawer*>(DefaultDrawer(0L));
    aDrawer->Assign(GetDrawer());
    aDrawer->myLineType = static_cast<Standard_Integer>(theType);
    SetDrawer(aDrawer);
}

//=======================================================================
//function : SetInvertColor
//purpose  : Indicates whether to draw line depending on underlying objects
//=======================================================================

void NIS_Line::SetInvertColor (const Standard_Boolean theInvert)
{
    const Handle(NIS_LineDrawer) aDrawer =
        static_cast<NIS_LineDrawer*>(DefaultDrawer(0L));
    aDrawer->Assign(GetDrawer());
    aDrawer->myInvertColor = static_cast<Standard_Boolean>(theInvert);
    SetDrawer(aDrawer);
}

//=======================================================================
//function : Intersect
//purpose  :
//=======================================================================

Standard_Boolean NIS_Line::Intersect
                                        (const Bnd_B3f&         theBox,
                                         const gp_Trsf&         theTrf,
                                         const Standard_Boolean /*isFullIn*/) const
{
    return !theBox.IsOut(myLine.Transformed(theTrf));
}

//=======================================================================
//function : Intersect
//purpose  :
//=======================================================================

Standard_Real NIS_Line::Intersect(const gp_Ax1&       theAxis,
                                  const Standard_Real theOver) const
{
    Standard_Real aResult (RealLast());
    const Standard_Real anOver2 = theOver*theOver;

    const gp_XYZ anAxisDir = theAxis.Direction().XYZ();
    const gp_XYZ aDir = myLine.Direction().XYZ();
    const gp_XYZ aDirN = aDir ^ anAxisDir;  // normal direction to both lines
    // vector between 'start' points of two lines
    const gp_XYZ aDelta = myLine.Location().XYZ() - theAxis.Location().XYZ();

    Standard_Real aMod2 = aDirN.SquareModulus();
    if (aMod2 < Precision::Confusion() * 0.001)
    {
        // lines are parallel
        if ( (aDelta ^ anAxisDir).SquareModulus() < anOver2 )
        {
            aResult = aDelta * anAxisDir;
        }
    }
    else
    {
        // distance between two lines
        Standard_Real aDist = aDelta * aDirN;
        if (aDist * aDist < anOver2 * aMod2)
        {
            aResult = aDelta * anAxisDir;
        }
    }

    return aResult;
}

//=======================================================================
//function : Intersect
//purpose  :
//=======================================================================

Standard_Boolean NIS_Line::Intersect
                         (const NCollection_List<gp_XY> &thePolygon,
                          const gp_Trsf                 &theTrf,
                          const Standard_Boolean         /*isFullIn*/) const
{
    Standard_Boolean aResult = Standard_False;

    gp_Ax1 aLine = myLine.Transformed(theTrf);
    const gp_XYZ aLoc = aLine.Location().XYZ();
    const gp_XYZ aDir = aLine.Direction().XYZ();

    gp_XY aDir2d(aDir.X(), aDir.Y());
    Standard_Real aDist = aDir2d.SquareModulus();
    if (aDist > Precision::Confusion())
    {
        aDir2d.Divide(sqrt(aDist));

        // check whether all polygon points in in the same side of line
        // Line is presented in form Ax + By + C = 0
        Standard_Real A =  aDir.Y();
        Standard_Real B = -aDir.X();
        Standard_Real C = -(A*aLoc.X() + B*aLoc.Y());

        gp_XY aPoint = thePolygon.Last();
        Standard_Real aSigned[2];
        aSigned[0] = A*aPoint.X() + B*aPoint.Y() + C;

        NCollection_List<gp_XY>::Iterator anIter(thePolygon);
        for (; anIter.More(); anIter.Next())
        {
            aPoint = anIter.Value();
            aSigned[1] = A*aPoint.X() + B*aPoint.Y() + C;
            if (aSigned[0] * aSigned[1] <= Precision::Confusion())
            {
                aResult = Standard_True;
                break;
            }
            aSigned[0] = aSigned[1];
        }
    }
    return aResult;
}

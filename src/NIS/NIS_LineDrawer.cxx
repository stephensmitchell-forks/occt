// File:      NIS_LineDrawer.cpp
// Created:   11.05.12
// Author:    Natalia Kopnova
// Copyright: LPKF Laser and Electronics AG 2012

#include <NIS_LineDrawer.hxx>
#include <NIS_Line.hxx>
#ifdef _DEBUG
#include <Standard_ProgramError.hxx>
#endif
#ifdef WNT
#include <windows.h>
#include <GL/gl.h>
#endif

#define NUM_STYLES 5
static GLushort pattern[NUM_STYLES] = {
    0xFFFF,  // Solid
    0xFFC0,  // Dash
    0xCCCC,  // Dot
    0xFF18,  // DotDash
    0xFF24   // DoubleDotDash
};

IMPLEMENT_STANDARD_HANDLE  (NIS_LineDrawer, NIS_Drawer)
IMPLEMENT_STANDARD_RTTIEXT (NIS_LineDrawer, NIS_Drawer)

//=======================================================================
//function : NIS_LineDrawer
//purpose  : Constructor
//=======================================================================

NIS_LineDrawer::NIS_LineDrawer(const Quantity_Color& theColor)
  : myLineWidth(1.f), myType(0), myLineType(0), myInvertColor(Standard_False)
{
    SetColor(Draw_Normal, theColor);
    SetColor(Draw_Hilighted, Quantity_NOC_GRAY80);
    SetColor(Draw_DynHilighted, Quantity_NOC_CYAN1);
}

//=======================================================================
//function : Assign
//purpose  :
//=======================================================================

void NIS_LineDrawer::Assign (const Handle_NIS_Drawer& theOther)
{
    if (theOther.IsNull() == Standard_False)
    {
        NIS_Drawer::Assign(theOther);
        const Handle(NIS_LineDrawer)& anOther =
            static_cast <const Handle(NIS_LineDrawer)&> (theOther);
        myLineWidth = anOther->myLineWidth;
        myType = anOther->myType;
        myLineType = anOther->myLineType;
        myInvertColor = anOther->myInvertColor;
    }
}

//=======================================================================
//function : IsEqual
//purpose  : Comparison of two Drawers (for Map impementation)
//=======================================================================

Standard_Boolean NIS_LineDrawer::IsEqual
                                (const Handle_NIS_Drawer& theOther) const
{
    static const Standard_Real anEpsilon2 (1e-7);
    Standard_Boolean aResult (Standard_False);
    const Handle(NIS_LineDrawer) anOther =
        Handle(NIS_LineDrawer)::DownCast(theOther);
    if (NIS_Drawer::IsEqual(theOther))
    {
        aResult = ((anOther->myLineWidth - myLineWidth) *
                   (anOther->myLineWidth - myLineWidth) < 0.01 &&
                   anOther->myType == myType &&
                   anOther->myLineType == myLineType &&
                   anOther->myInvertColor == myInvertColor);
    }
    return aResult;
}

//=======================================================================
//function : BeforeDraw
//purpose  :
//=======================================================================

void NIS_LineDrawer::BeforeDraw (const DrawType         theType,
                                       const NIS_DrawList&    /*theDrawList*/)
{
    Quantity_Parameter   aValue[4];
    aValue[3] = 1.;                               // opaque as default
    Quantity_TypeOfColor bidTC (Quantity_TOC_RGB);
    GLfloat aLineWidth (myLineWidth);

    if (theType == Draw_DynHilighted)
    {
        aLineWidth = myLineWidth + 1.f;
    }

    if (myInvertColor)
    {
        glEnable(GL_COLOR_LOGIC_OP);
        glLogicOp(GL_XOR);
    }
    else if (theType == Draw_Transparent)
    {
        aValue[3] = 1. - myTransparency;
        glEnable(GL_BLEND);
        glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
    }

    GetColor(theType).Values(aValue[0], aValue[1], aValue[2], bidTC);
    glColor4d (aValue[0], aValue[1], aValue[2], aValue[3]);
    glDisable(GL_LIGHTING);

    if (myType > 0 && myType < NUM_STYLES)
    {
        glEnable(GL_LINE_STIPPLE);
        glLineStipple(1, pattern[myType]);
    }

    glLineWidth(aLineWidth);
}

//=======================================================================
//function : AfterDraw
//purpose  :
//=======================================================================

void NIS_LineDrawer::AfterDraw (const DrawType         theType,
                                      const NIS_DrawList&    /*theDrawList*/)
{
    if (myInvertColor)
    {
        glDisable(GL_COLOR_LOGIC_OP);
    }
    else if (theType == Draw_Transparent)
    {
        glDisable(GL_BLEND);
    }

    if (myType > 0 && myType < NUM_STYLES)
    {
        glDisable(GL_LINE_STIPPLE);
    }
}

//=======================================================================
//function : Draw
//purpose  :
//=======================================================================

void NIS_LineDrawer::Draw (const Handle_NIS_InteractiveObject& theObj,
                                 const DrawType         /*theType*/,
                                 const NIS_DrawList&    /*theDrawList*/)
{
    // Assertion for the type of the drawn object
#ifdef _DEBUG
    static const Handle(Standard_Type) ThisType =
        STANDARD_TYPE(NIS_Line);
    if (theObj->IsKind(ThisType) == Standard_False)
    {
        Standard_ProgramError::Raise("NIS_LineDrawer::Draw: "
                                     "irrelevant object type");
    }
#endif
    const NIS_Line* pObject =
        static_cast <const NIS_Line*> (theObj.operator->());

    // Draw line
    glBegin(GL_LINES);
    glVertex3fv(&pObject->myPoint1[0]);
    glVertex3fv(&pObject->myPoint2[0]);
    glEnd();
}

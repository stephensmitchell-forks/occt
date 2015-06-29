// Created on: 2008-03-20
// Created by: Alexander GRIGORIEV
// Copyright (c) 2008-2014 OPEN CASCADE SAS
//
// This file is part of Open CASCADE Technology software library.
//
// This library is free software; you can redistribute it and / or modify it
// under the terms of the GNU Lesser General Public version 2.1 as published
// by the Free Software Foundation, with special exception defined in the file
// OCCT_LGPL_EXCEPTION.txt. Consult the file LICENSE_LGPL_21.txt included in OCCT
// distribution for complete text of the license and disclaimer of any warranty.
//
// Alternatively, this file may be used under the terms of Open CASCADE
// commercial license or contractual agreement.

#include <NIS_SurfaceDrawer.hxx>
#include <NIS_Surface.hxx>
#include <NIS_InteractiveObject.hxx>
#include <Standard_ProgramError.hxx>

#include <OpenGl_GlCore11.hxx>

static void setColor(Quantity_Parameter *     theAmbientFront,
                     Quantity_Parameter *     theAmbientBack,
                     const Standard_ShortReal theSpecularity,
                     GLint                    theShininess);

IMPLEMENT_STANDARD_HANDLE  (NIS_SurfaceDrawer, NIS_Drawer)
IMPLEMENT_STANDARD_RTTIEXT (NIS_SurfaceDrawer, NIS_Drawer)

//=======================================================================
//function : NIS_SurfaceDrawer()
//purpose  : Constructor
//=======================================================================

NIS_SurfaceDrawer::NIS_SurfaceDrawer
                                (const Quantity_Color   &theNormal,
                                 const Quantity_Color   &theHilight,
                                 const Quantity_Color   &theDynHilight)
  : NIS_Drawer(theHilight, theDynHilight),
    myBackColor         (theNormal),
    myFreeEdgeColor     (theNormal),
    mySpecularity       (0.58f),
    myPolygonOffset     (0.f),
    myIsWireframe       (Standard_False),
    myHilightIsWireframe(Standard_False),
    myIsShowEdges       (Standard_True)
{
  SetColor(Draw_Normal, theNormal);
  myObjPerDrawer = 4;
}

//=======================================================================
//function : Assign
//purpose  :
//=======================================================================

void NIS_SurfaceDrawer::Assign (const Handle_NIS_Drawer& theOther)
{
  if (theOther.IsNull() == Standard_False) {
    NIS_Drawer::Assign (theOther);
    const Handle(NIS_SurfaceDrawer)& anOther =
      static_cast <const Handle(NIS_SurfaceDrawer)&> (theOther);
    myBackColor                = anOther->myBackColor;
    myFreeEdgeColor            = anOther->myFreeEdgeColor;
    mySpecularity              = anOther->mySpecularity;
    myPolygonOffset            = anOther->myPolygonOffset;
    myIsWireframe              = anOther->myIsWireframe;    
    myHilightIsWireframe       = anOther->myHilightIsWireframe;
    myIsShowEdges              = anOther->myIsShowEdges;    
    myTrsf                     = anOther->myTrsf;
  }
}

//=======================================================================
//function : IsEqual
//purpose  : Comparison of two Drawers (for Map impementation)
//=======================================================================

Standard_Boolean NIS_SurfaceDrawer::IsEqual
                                (const Handle_NIS_Drawer& theOther)const
{
  Standard_Boolean                 aResult (Standard_False);
  const Handle(NIS_SurfaceDrawer) anOther =
                        Handle(NIS_SurfaceDrawer)::DownCast (theOther);
  if (NIS_Drawer::IsEqual(theOther))
    aResult = (areEqual(anOther->myBackColor, myBackColor) &&
               areEqual(anOther->myFreeEdgeColor, myFreeEdgeColor) &&
               fabs(anOther->mySpecularity - mySpecularity) < 0.01 &&
               fabs(anOther->myPolygonOffset - myPolygonOffset) < 0.999 &&
               myIsWireframe == anOther->myIsWireframe && 
               myHilightIsWireframe == anOther->myHilightIsWireframe &&
               myIsShowEdges == anOther->myIsShowEdges && 
               fabs(anOther->myTrsf.TranslationPart().SquareModulus() -
                    myTrsf.TranslationPart().SquareModulus()) < 0.0001);
  if (aResult) {
    // Arbitrary point for test
    gp_XYZ aPnt[2] = {
      gp_XYZ(113., -31.3, 29.19),
      gp_XYZ(113., -31.3, 29.19)
    };
    anOther->myTrsf.Transforms(aPnt[0]);
    myTrsf.Transforms(aPnt[1]);
    if ((aPnt[0] - aPnt[1]).SquareModulus() > 1e-6)
      aResult = Standard_False;
  }
  return aResult;
}

//=======================================================================
//function : redraw
//purpose  :
//=======================================================================

void NIS_SurfaceDrawer::redraw  (const DrawType         theType,
                                 const Handle_NIS_View& theView)
{
  glMatrixMode( GL_MODELVIEW );
  glPushMatrix();

  GLdouble aMatrix[16] = {
    myTrsf.Value(1,1), myTrsf.Value(2,1), myTrsf.Value(3,1),  0.,
    myTrsf.Value(1,2), myTrsf.Value(2,2), myTrsf.Value(3,2),  0.,
    myTrsf.Value(1,3), myTrsf.Value(2,3), myTrsf.Value(3,3),  0.,
    myTrsf.Value(1,4), myTrsf.Value(2,4), myTrsf.Value(3,4),  1.
  };

  glMultMatrixd( aMatrix );

  NIS_Drawer::redraw(theType, theView);

  glPopMatrix();
}

//=======================================================================
//function : BeforeDraw
//purpose  :
//=======================================================================

void NIS_SurfaceDrawer::BeforeDraw (const DrawType      theType,
                                    const NIS_DrawList&)
{
  glEnable(GL_LIGHTING);
  glEnableClientState (GL_VERTEX_ARRAY);
  if (myIsWireframe == Standard_False) {
    glPolygonMode (GL_FRONT_AND_BACK, GL_FILL);
    glEnable(GL_POLYGON_OFFSET_FILL);
    glShadeModel(GL_SMOOTH);
  }

  Quantity_Parameter aValueCol[][4] = {
    {0., 0., 0., 1. - myTransparency},           // front color
    {0., 0., 0., 1. - myTransparency}            // back color
  };
  Quantity_Color aBackColor = myBackColor;
  Quantity_TypeOfColor bidTC (Quantity_TOC_RGB);
  GLfloat aLineWidth (1.f);
  GLfloat anOffset = myPolygonOffset;
  DrawType aDrawType = theType;
  myIsBlend = Standard_False;

  switch (theType) {
  case Draw_Hilighted:
    if (myHilightIsWireframe)
    {
      aDrawType = Draw_Normal;
      glEnableClientState (GL_NORMAL_ARRAY);
      if (myTransparency > 0.) {
        glEnable(GL_BLEND);
        glEnable(GL_CULL_FACE);
        glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
        // don't write triangles into depth test
        glDepthMask(GL_FALSE);
        myIsBlend = Standard_True;
      }      
  case Draw_DynHilighted:  
      aLineWidth = 3.f;
      GetColor(aDrawType).Values(aValueCol[0][0], aValueCol[0][1],
                                 aValueCol[0][2], bidTC);
      aBackColor.Values(aValueCol[1][0],aValueCol[1][1],aValueCol[1][2], bidTC);
      setColor(aValueCol[0], aValueCol[1], mySpecularity, 10);

      glLineWidth (aLineWidth);
      if (myIsWireframe == Standard_False)
        glPolygonOffset(1.f, -(anOffset + 11.f));
      return;
    }    
    anOffset += 10.f;
    aBackColor = GetColor(theType);
  case Draw_Normal:
  case Draw_Top:
  case Draw_Transparent:
    if (myIsWireframe == Standard_False) {
      glPolygonOffset(1.f, -anOffset);
      glEnableClientState (GL_NORMAL_ARRAY);
    }
    GetColor(theType).Values(aValueCol[0][0], aValueCol[0][1],
                             aValueCol[0][2], bidTC);
    aBackColor.Values(aValueCol[1][0], aValueCol[1][1], aValueCol[1][2], bidTC);
    if (theType == Draw_Transparent) {
      glEnable(GL_BLEND);
      glEnable(GL_CULL_FACE);
      glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
      myIsBlend = Standard_True;
    }
    break;
  default:
    return;
  }
  glLightModeli(GL_LIGHT_MODEL_LOCAL_VIEWER, 0);
  glLightModeli(GL_LIGHT_MODEL_TWO_SIDE, 1);
  setColor(aValueCol[0], aValueCol[1], mySpecularity, 10);

  glLineWidth (aLineWidth);
}

//=======================================================================
//function : AfterDraw
//purpose  :
//=======================================================================

void NIS_SurfaceDrawer::AfterDraw (const DrawType      theType,
                                   const NIS_DrawList&)
{
  glDisable(GL_LIGHTING);
  glDisableClientState(GL_VERTEX_ARRAY);
  if (theType != Draw_DynHilighted)
  {
    if (myIsBlend)
    {
      glDisable(GL_BLEND);
      glDisable(GL_CULL_FACE);
      if (theType == Draw_Hilighted)
      {
        glDepthMask (GL_TRUE);
      }
    }
    if (myIsWireframe == Standard_False)
    {
      glDisableClientState(GL_NORMAL_ARRAY);      
    }
  }
  
  if (myIsWireframe == Standard_False)
    glDisable(GL_POLYGON_OFFSET_FILL);
}

//=======================================================================
//function : Draw
//purpose  :
//=======================================================================

void NIS_SurfaceDrawer::Draw (const Handle_NIS_InteractiveObject& theObj,
                              const DrawType                      theType,
                              const NIS_DrawList&)
{
  // Assertion for the type of the drawn object
#ifdef _DEBUG
  static const Handle(Standard_Type) ThisType = STANDARD_TYPE(NIS_Surface);
  Standard_ProgramError_Raise_if (theObj->IsKind(ThisType) == Standard_False,
                                  "NIS_Surface::Draw: irrelevant object type");
#endif
  const NIS_Surface * pObject =
    static_cast <const NIS_Surface *> (theObj.operator->());
  glVertexPointer (3, GL_FLOAT, 0, pObject->Node(0));

  // In Highlited mode the shape must be shown as wireframe
  Standard_Boolean isWireframe(myIsWireframe);
  if (isWireframe == Standard_False && theType == Draw_DynHilighted)
    if (pObject->NEdges() > 0 || pObject->NFreeEdges() > 0)
      isWireframe = Standard_True;

  if (!isWireframe)
  {    
    if (pObject->NTriangles()) {
      if (theType != Draw_DynHilighted)
        glNormalPointer (GL_FLOAT, 0, pObject->Normal(0));
      if (myIsBlend) {
        // Drawing BACK faces before FRONT ones improves the transparency
        glCullFace(GL_FRONT);
        glDrawElements (GL_TRIANGLES, pObject->NTriangles()*3,
                        GL_UNSIGNED_INT, pObject->Triangle(0));
        glCullFace(GL_BACK);
      }
      glDrawElements (GL_TRIANGLES, pObject->NTriangles()*3,
                      GL_UNSIGNED_INT, pObject->Triangle(0));
    }
  }
  if (myIsShowEdges || isWireframe || pObject->IsShowFreeBounds() ||
      (theType == Draw_Hilighted && myHilightIsWireframe == Standard_True))
  {
    if (pObject->NEdges() > 0 || pObject->NFreeEdges() > 0) {
      glDisable (GL_LIGHTING);
      Quantity_Parameter R1, R2, R3;
      GetColor(theType).Values(R1, R2, R3, Quantity_TOC_RGB);
      glColor3d (R1, R2, R3);
      drawEdges(pObject, theType);
      glEnable (GL_LIGHTING);
    }
  }
}

//=======================================================================
//function : drawEdges 
//purpose  : 
//=======================================================================

void NIS_SurfaceDrawer::drawEdges (const NIS_Surface * pObject,
                                   const DrawType    theType)
{
  // Draw non-free edges
  for (Standard_Integer i = 0; i < pObject->NEdges(); i++) {        
    glDrawElements (GL_LINE_STRIP, pObject->NEdgeNodeInList(i),
                    GL_UNSIGNED_INT, pObject->Edge(i));
  }

  // Set color to show free edges, if they are requested
  if (pObject->IsShowFreeBounds() && theType != Draw_DynHilighted &&
      !(theType == Draw_Hilighted && myHilightIsWireframe == Standard_True))
  {
    Quantity_Parameter R1, R2, R3;
    myFreeEdgeColor.Values(R1, R2, R3, Quantity_TOC_RGB);
    glColor3d (R1, R2, R3);
    glLineWidth (2.5f);
  }

  // Draw free edges
  for (Standard_Integer i = 0; i < pObject->NFreeEdges(); i++)
    glDrawElements (GL_LINE_STRIP, pObject->NFreeEdgeNodeInList(i),
                    GL_UNSIGNED_INT, pObject->FreeEdge(i));
}

//=======================================================================
//function : setColor
//purpose  :
//=======================================================================

void setColor(Quantity_Parameter *     theAmbientFront,
              Quantity_Parameter *     theAmbientBack,
              const Standard_ShortReal theSpecularity,
              GLint                    theShininess)
{
  static const GLfloat gColorN[4]  = {0.f, 0.f, 0.f, 1.f};
  GLfloat aValueCol[4] = {
    GLfloat(theAmbientFront[0]),
    GLfloat(theAmbientFront[1]),
    GLfloat(theAmbientFront[2]),
    GLfloat(theAmbientFront[3])
  };
  glMaterialfv(GL_FRONT, GL_AMBIENT_AND_DIFFUSE, &aValueCol[0]);

  aValueCol[0] = GLfloat(theAmbientBack[0]);
  aValueCol[1] = GLfloat(theAmbientBack[1]);
  aValueCol[2] = GLfloat(theAmbientBack[2]);
  aValueCol[3] = GLfloat(theAmbientBack[3]);
  glMaterialfv(GL_BACK, GL_AMBIENT_AND_DIFFUSE, &aValueCol[0]);

  const GLfloat aSpec(theSpecularity < 0. ? 0.f :
                      theSpecularity > 1.f ? 1.f :
                      theSpecularity);
  aValueCol[0] = aSpec;
  aValueCol[1] = aSpec;
  aValueCol[2] = aSpec;
  aValueCol[3] = 1.f;
  glMaterialfv(GL_FRONT, GL_SPECULAR, &aValueCol[0]);
  glMateriali(GL_FRONT, GL_SHININESS, theShininess);
  glMaterialfv(GL_BACK, GL_SPECULAR, &aValueCol[0]);
  glMateriali(GL_BACK, GL_SHININESS, theShininess);
  glMaterialfv(GL_FRONT, GL_EMISSION, &gColorN[0]);
  glMaterialfv(GL_BACK, GL_EMISSION, &gColorN[0]);
}


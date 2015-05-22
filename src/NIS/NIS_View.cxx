// Created on: 2007-07-06
// Created by: Alexander GRIGORIEV
// Copyright (c) 2007-2014 OPEN CASCADE SAS
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

#include <OpenGl_GlCore11.hxx>
#include <OpenGl_Context.hxx>

#include <NIS_View.hxx>
#include <NIS_InteractiveContext.hxx>
#include <NIS_InteractiveObject.hxx>
#include <gp_Ax1.hxx>
#include <Visual3d_View.hxx>
#include <Bnd_B2f.hxx>
#include <TColStd_MapIteratorOfPackedMapOfInteger.hxx>

#include <OpenGl_GlCore11.hxx>

IMPLEMENT_STANDARD_HANDLE  (NIS_View, V3d_OrthographicView)
IMPLEMENT_STANDARD_RTTIEXT (NIS_View, V3d_OrthographicView)
IMPLEMENT_STANDARD_HANDLE  (NIS_ViewData, Standard_Transient)
IMPLEMENT_STANDARD_RTTIEXT (NIS_ViewData, Standard_Transient)

//=======================================================================
//function : NIS_View()
//purpose  : Constructor
//=======================================================================

NIS_View::NIS_View (const Handle(V3d_Viewer)&    theViewer,
                    const Handle(Aspect_Window)& theWindow)
  : V3d_OrthographicView (theViewer),
    myIsTopHilight       (Standard_False),
    myIsTopDynHilight    (Standard_False),
    myDoHilightSelected  (Standard_True),
    myGlCtx              (NULL),
    myHDC                (0L),
    myHGLRC              (0L)
{
  if (!theWindow.IsNull()) {
    myViewport[0] = -1;
    V3d_View::SetWindow (theWindow, NULL, &MyCallback, this);
  }
  myViewport[0] = 0;
  myViewport[1] = 0;
  myViewport[2] = 0;
  myViewport[3] = 0;
}

//=======================================================================
//function : SetWindow
//purpose  :
//=======================================================================

void NIS_View::SetWindow(const Handle(Aspect_Window) &theWindow)
{
  myViewport[0] = -1;
  V3d_View::SetWindow (theWindow, NULL, &MyCallback, this);
  myViewport[0] = 0;
  myHDC = 0L;
  myHGLRC = 0L;
}

// //=======================================================================
// //function : ~NIS_View
// //purpose  : Destructor
// //=======================================================================

// NIS_View::~NIS_View()
// {}


//=======================================================================
//function : AddContext
//purpose  :
//=======================================================================

void  NIS_View::AddContext (NIS_InteractiveContext * theCtx)
{
  // Check that the given context is not among already attached contexts
  NCollection_List<NIS_InteractiveContext *>::Iterator anIter (myContexts);
  for (; anIter.More(); anIter.Next())
    if (anIter.Value() == theCtx)
      break;
  if (anIter.More() == Standard_False)
    myContexts.Append (theCtx);
}

//=======================================================================
//function : RemoveContext
//purpose  :
//=======================================================================

void NIS_View::RemoveContext (NIS_InteractiveContext * theCtx)
{
  NCollection_List<NIS_InteractiveContext *>::Iterator anIter (myContexts);
  for (; anIter.More(); anIter.Next())
    if (anIter.Value() == theCtx) {
      myContexts.Remove (anIter);
      break;
    }

  // If draw lists are shared in this Context then they should be
  // destroyed when we detach from the last view (1 remaining view is detached).
  Standard_Boolean isForcedDestroy(Standard_False);
  if (theCtx->myIsShareDrawList && theCtx->myViews.Extent() == 1)
    isForcedDestroy = Standard_True;

  NCollection_Map<Handle_NIS_Drawer>::Iterator anIterD (theCtx->GetDrawers ());
  for (; anIterD.More(); anIterD.Next()) {
    const Handle(NIS_Drawer)& aDrawer = anIterD.Value();
    if (aDrawer.IsNull() == Standard_False) {
      aDrawer->UpdateExListId(isForcedDestroy ? 0L : this);
    }
  }
}

//=======================================================================
//function : FitAll3d
//purpose  :
//=======================================================================

Standard_Boolean NIS_View::FitAll3d (const Quantity_Coefficient theCoef)
{
  Standard_Boolean aResult(Standard_False);
  /*
  Standard_Integer aLimp[4] = { 1000000, -1000000, 1000000, -1000000 };
  GetBndBox( aLimp[0], aLimp[1], aLimp[2], aLimp[3] );
  if (aLimp[1] > -1000000 && aLimp[3] > -1000000 &&
      aLimp[0] < aLimp[1] && aLimp[2] < aLimp[3])
  {
    // Scale the view
    WindowFit (aLimp[0], aLimp[2], aLimp[1], aLimp[3]);
    aResult = Standard_True;
  }
  */

  Bnd_B3f aBox = GetBndBox();

  // Check that the box is not empty
  if (aBox.IsVoid() == Standard_False && MyView->IsDefined() == Standard_True) {
    // Convert the 3D box to 2D representation in view coordinates
    Standard_Real Umin = 0.,Umax = 0.,Vmin = 0.,Vmax = 0.,U,V,W;
    gp_XYZ aCoord;

    const gp_XYZ aCorner[2] = { aBox.CornerMin(), aBox.CornerMax() };

    Standard_Boolean doFit = Standard_True;
    while (doFit) {

    for (Standard_Integer i = 0; i < 8; i++) {
      if (i & 0x1) aCoord.SetX (aCorner[0].X());
      else         aCoord.SetX (aCorner[1].X());
      if (i & 0x2) aCoord.SetY (aCorner[0].Y());
      else         aCoord.SetY (aCorner[1].Y());
      if (i & 0x4) aCoord.SetZ (aCorner[0].Z());
      else         aCoord.SetZ (aCorner[1].Z());

      MyView->Projects(aCoord.X(), aCoord.Y(), aCoord.Z(), U, V, W);
      if (i) {
        Umin = Min(Umin, U); Umax = Max(Umax, U);
        Vmin = Min(Vmin, V); Vmax = Max(Vmax, V);
      }
      else {
        Umin = Umax = U;
        Vmin = Vmax = V;
      }
    }

    if ( (Umax > Umin) && (Vmax > Vmin) ) {
      Standard_Real OldUmin,OldUmax,OldVmin,OldVmax;
      MyViewMapping.WindowLimit(OldUmin, OldVmin, OldUmax, OldVmax);
      Standard_Real DxvOld = Abs(OldUmax - OldUmin);

      // make a margin
      Standard_Real Xrp, Yrp, DxvNew, DyvNew;

      DxvNew = Abs(Umax - Umin); DyvNew = Abs(Vmax - Vmin);
      DxvNew *= (1. + theCoef);
      DyvNew *= (1. + theCoef);

      Standard_Real aRatio = DxvNew / DxvOld;

      Xrp = (Umin + Umax)/2. ; Yrp = (Vmin + Vmax)/2. ;
      Umin = Xrp - DxvNew/2. ; Umax = Xrp + DxvNew/2. ;
      Vmin = Yrp - DyvNew/2. ; Vmax = Yrp + DyvNew/2. ;

      // fit view
      FitAll(Umin, Vmin, Umax, Vmax);

      // ratio 1e+6 often gives calculation error(s), reduce it
      // if (aRatio < 1e+6) doFit = Standard_False;
      if (aRatio < 100) doFit = Standard_False;
      aResult = Standard_True;
    }
    else doFit = Standard_False;

    }
  }

  return aResult;
}

//=======================================================================
//function : GetViewData
//purpose  : get or create a new ViewData instance.
//           For thread safety the instances are kept in a list instead of
//           map: operation List::Append is atomic thus safe.
//=======================================================================

Handle_NIS_ViewData& NIS_View::GetViewData (const char * theKey)
{
  NCollection_List<struct NamedViewData>::Iterator anIter(myViewData);
  for (; anIter.More(); anIter.Next()) {
    const NamedViewData& aData = anIter.Value();
    if (aData.Name.IsEqual(theKey))
      break;
  }
  if (anIter.More())
    return anIter.ChangeValue().Data;
  struct NamedViewData aNewData;
  aNewData.Name = theKey;
  return myViewData.Append(aNewData).Data; 
}

//=======================================================================
//function : Remove
//purpose  : Expressly close internal data structures
//=======================================================================

void NIS_View::Remove ()
{
  if (!myGlCtx.IsNull())
  {
    if (myGlCtx->MakeCurrent())
    {
      myGlCtx.Nullify();
      myViewData.Clear();
    }
  }
  V3d_View::Remove();
}

//=======================================================================
//function : GetBndBox
//purpose  :
//=======================================================================

Bnd_B3f NIS_View::GetBndBox() const
{
  // Calculate the 3D bounding box of visible objects
  // in all interactive contexts
  Bnd_B3f aBox;
  NCollection_List<NIS_InteractiveContext *>::Iterator anIterC (myContexts);
  for (; anIterC.More(); anIterC.Next()) {
    NCollection_Map<Handle_NIS_Drawer>::Iterator anIterD
      (anIterC.Value()->myDrawers);
    for (; anIterD.More(); anIterD.Next()) {
      const Handle(NIS_Drawer)& aDrawer = anIterD.Value();
      Bnd_B3f aBoxD = aDrawer->GetBox (this);
      aBox.Add (aBoxD);
    }
  }

  // Take the bounding box of AIS objects displayed in the view
  Standard_Real aVal[6];
  View()->MinMaxValues(aVal[0], aVal[1], aVal[2], aVal[3], aVal[4], aVal[5]);
  if (aVal[3] < 0.5 * RealLast()) {
    aBox.Add (gp_XYZ (aVal[0], aVal[1], aVal[2]));
    aBox.Add (gp_XYZ (aVal[3], aVal[4], aVal[5]));
  }

  return aBox;
}

//=======================================================================
//function : GetBndBox
//purpose  :
//=======================================================================

void NIS_View::GetBndBox( Standard_Integer& theXMin, Standard_Integer& theXMax,
                          Standard_Integer& theYMin, Standard_Integer& theYMax ) const
{
  theXMin = theYMin = 0;
  theXMax = theYMax = -1;

  Bnd_B3f aBox = GetBndBox();

  // Check that the box is not empty
  if (aBox.IsVoid() == Standard_False) {
    // Convert the 3D box to 2D representation in pixel coordinates
    gp_XYZ aCoord;
    Standard_Integer anXp, anYp;
    const gp_XYZ aCorner[2] = { aBox.CornerMin(), aBox.CornerMax() };
    Standard_Integer aLimp[4] = { 1000000, -1000000, 1000000, -1000000 };
    for (Standard_Integer i = 0; i < 8; i++) {
      if (i & 0x1) aCoord.SetX (aCorner[0].X());
      else         aCoord.SetX (aCorner[1].X());
      if (i & 0x2) aCoord.SetY (aCorner[0].Y());
      else         aCoord.SetY (aCorner[1].Y());
      if (i & 0x4) aCoord.SetZ (aCorner[0].Z());
      else         aCoord.SetZ (aCorner[1].Z());
      Convert( aCoord.X(), aCoord.Y(), aCoord.Z(), anXp, anYp );
      if (aLimp[0] > anXp) aLimp[0] = anXp;
      if (aLimp[1] < anXp) aLimp[1] = anXp;
      if (aLimp[2] > anYp) aLimp[2] = anYp;
      if (aLimp[3] < anYp) aLimp[3] = anYp;
    }
    if (aLimp[0] < aLimp[1] && aLimp[2] < aLimp[3])
    {
      // Scale the view
      // WindowFit (aLimp[0], aLimp[2], aLimp[1], aLimp[3]);
      theXMin = aLimp[0];
      theXMax = aLimp[1];
      theYMin = aLimp[2];
      theYMax = aLimp[3];
    }
  }
}

//=======================================================================
//function : MakeCurrent
//purpose  : 
//=======================================================================

Standard_Boolean NIS_View::MakeCurrent() const
{
  Standard_Boolean aResult(Standard_False);
  HDC anActiveHdc     = wglGetCurrentDC();
  HGLRC anActiveGlCtx = wglGetCurrentContext();
  if (myHDC == anActiveHdc && myHGLRC == anActiveGlCtx)
    aResult = Standard_True;
  else if (wglMakeCurrent(reinterpret_cast<HDC>(myHDC),
                          reinterpret_cast<HGLRC>(myHGLRC)))
    aResult = Standard_True;
  return aResult;
}

//=======================================================================
//function : MyCallback
//purpose  : 
//=======================================================================

int NIS_View::MyCallback (Aspect_Drawable /* Window ID */,
                          void*                         theUserData, 
                          Aspect_GraphicCallbackStruct* thecallData)
{
  // NKV 14.07.11: Avoid double rendering of the scene
  // (workaround of integration from BUG OCC22377, that calls view
  // call back twice: before drawing overlay layer and after drawing,
  // see comments to OCC_PRE_OVERLAY definition)
  NIS_View* thisView = static_cast<NIS_View*> (theUserData);
  if ((thecallData->reason & OCC_PRE_OVERLAY) == 0) {
    Handle(OpenGl_Context) aGlContext = 
      Handle(OpenGl_Context)::DownCast(thecallData->glContext);
    thisView->myGlCtx = aGlContext;
    if (!thisView->myHDC)
      thisView->myHDC = wglGetCurrentDC();
    if (!thisView->myHGLRC)
      thisView->myHGLRC = wglGetCurrentContext();
    if (thisView->myViewport[0] >= 0) {
      thisView->redrawCallback();
    }
  }

  return 0;
}

//=======================================================================
//function : redrawCallback
//purpose  :
//=======================================================================

void NIS_View::redrawCallback()
{
  // cache render mode state
  GLint aRendMode = GL_RENDER;
  glGetIntegerv (GL_RENDER_MODE, &aRendMode);

  if (myGlCtx.IsNull() == Standard_False) {
    myGlCtx->Init();
    myGlCtx->SetFeedback (aRendMode == GL_FEEDBACK);
  }

  const Handle(NIS_View) thisView = this;

  // ==== Get pixel size
  {
    glGetIntegerv(GL_VIEWPORT, myViewport);
    Standard_Integer anX(myViewport[0] + myViewport[2]/2);
    Standard_Integer anY(myViewport[1] + myViewport[3]/2);
    const gp_XYZ aPnt[3] = {
      UnConvert (anX, anY),
      UnConvert (anX + 5, anY),
      UnConvert (anX, anY + 5)
    };
    myPixelSize.SetCoord(0.2 * (aPnt[1] - aPnt[0]).Modulus(),
                         0.2 * (aPnt[2] - aPnt[0]).Modulus());
  }

  // ==== Get view transformation
  {
    Standard_Real anX, anY, aZ;
    // vector orthogonal to the view plane
    Proj (anX, anY, aZ);
    myProj.SetCoord (anX, anY, aZ);

    // 3D point for the 3D coordinates
    Convert(myViewport[0] + myViewport[2] / 2,
            myViewport[1] + myViewport[3] / 2, anX, anY, aZ);
    const gp_Pnt anEye (anX, anY, aZ);

    // 3D point for the 3D coordinates
    Convert(myViewport[0] + myViewport[2],
            myViewport[1] + myViewport[3] / 2, anX, anY, aZ);
    const gp_XYZ anXdir (gp_XYZ(anX, anY, aZ) - anEye.XYZ());
    myViewSize.SetX(anXdir.Modulus());
    Convert(myViewport[0] + myViewport[2] / 2, myViewport[1], anX, anY, aZ);
    myViewSize.SetY((gp_XYZ(anX, anY, aZ) - anEye.XYZ()).Modulus());
    if (anXdir.SquareModulus() > 1e-20) {
      const gp_Ax3 anAx3 (anEye, myProj, anXdir);
      myTrsf.SetTransformation (anAx3);
    }
  }

  // ==== Manage the ViewData
  NCollection_List<NamedViewData>::Iterator anIterVD(myViewData);
  for (; anIterVD.More(); anIterVD.Next()) {
    const Handle(NIS_ViewData)& aData = anIterVD.Value().Data;
    if (aData.IsNull() == Standard_False) {
      aData->Init();
      if (aData->myView != this)
        aData->myView = this;
    }
  }

#ifdef CLIP
  // Find the bounding box of all displayed objects by summing the boxes stored
  // in the relevant DrawList instances.
  Bnd_B3f aBndBox;
  NCollection_List<NIS_InteractiveContext *>::Iterator anIter = myContexts;
  for (; anIter.More(); anIter.Next())
  {
    anIter.Value()->GetBox (aBndBox, pView);
  }

  if (aBndBox.IsVoid() == Standard_False) {
    const gp_XYZ aBoxSize   = 0.5 * (aBndBox.CornerMax() - aBndBox.CornerMin());
    const gp_XYZ aBoxCenter = 0.5 * (aBndBox.CornerMax() + aBndBox.CornerMin());

    // Find the ray passing through the clicked point in the view window.
    Standard_Real aZ;
    Convert(0, 0, anX, anY, aZ);  // 3D point for the 3D coordinates
    const gp_Pnt anEye (anX, anY, aZ);
    Proj (anX, anY, aZ);  // vector orthogonal to the view plane
    const gp_Dir aProj (anX, anY, aZ);
    const gp_Ax1 anAxis (anEye, aProj);

    const Standard_Real aCenterDist = (anEye.XYZ() - aBoxCenter) * aProj.XYZ();
    const Standard_Real aBoxExtent /*(fabs(aBoxSize.X() * anX) +
                                    fabs(aBoxSize.Y() * anY) +
                                    fabs(aBoxSize.Z() * aZ))*/(100.);

#define FRONT_CLIPPING_PLANE (GL_CLIP_PLANE0 + 0)
#define BACK_CLIPPING_PLANE  (GL_CLIP_PLANE0 + 1)
    Standard_Real arr[4] = {
      0.0,  /* Nx */
      0.0,  /* Ny */
      1.0,  /* Nz */
      0.
    };
    arr[3] = aBoxExtent + 1.;
    glClipPlane (BACK_CLIPPING_PLANE, arr);
    glEnable (BACK_CLIPPING_PLANE);
    arr[2] = -1.0;
    arr[3] = aBoxExtent + 1.;
    glClipPlane (FRONT_CLIPPING_PLANE, arr);
    glEnable (FRONT_CLIPPING_PLANE);
  }
#endif //IS_DISABLED

  GLboolean isDepthWriteMask, isDepthTest;
  glGetBooleanv(GL_DEPTH_WRITEMASK,&isDepthWriteMask);
  glGetBooleanv(GL_DEPTH_TEST,&isDepthTest);
//   printf ("GlDepthMask=%d; GlDepthTest=%d\n", depthwritemask, depthtest);
  glDisableClientState(GL_COLOR_ARRAY);
  glDisableClientState(GL_EDGE_FLAG_ARRAY);
  glDisableClientState(GL_INDEX_ARRAY);
  glDisableClientState(GL_NORMAL_ARRAY);
  glDisableClientState(GL_TEXTURE_COORD_ARRAY);
  if (!isDepthTest) {
    glEnable(GL_DEPTH_TEST);
    glClearDepth(1.);
    glClear(GL_DEPTH_BUFFER_BIT);
  }
  glDepthFunc(GL_LEQUAL);
  glDepthMask(GL_TRUE);

  TColStd_MapIteratorOfPackedMapOfInteger anIterM (myExListId);
  for (; anIterM.More(); anIterM.Next())
    if (anIterM.Key() != 0) {
#ifdef ARRAY_LISTS
      glDeleteLists (anIterM.Key(), 5);
#else
      glDeleteLists (anIterM.Key(), 1);
    }
#endif
  myExListId.Clear();

  redraw (thisView, NIS_Drawer::Draw_Normal);

  // #818151 - selected object is hidden by covered unselected one
  // display hilighted objects always above the rest ones
  if (myIsTopHilight == Standard_True) {
    glDepthFunc(GL_ALWAYS);
  }

  redraw (thisView, NIS_Drawer::Draw_Hilighted);

  // Depth buffer should be read-only for transparent objects,
  // the same for further types
  glDepthMask(GL_FALSE);
  redraw (thisView, NIS_Drawer::Draw_Transparent);
  if (myIsTopDynHilight == Standard_False)
  {
    redraw (thisView, NIS_Drawer::Draw_DynHilighted);
  }

  // draw top objects always above
  if (myIsTopHilight == Standard_False)
  {
    glDepthFunc(GL_ALWAYS);
  }
  if (myIsTopDynHilight == Standard_True)
  {
    redraw (thisView, NIS_Drawer::Draw_DynHilighted);
  }

  redraw (thisView, NIS_Drawer::Draw_Top);
}

//=======================================================================
//function : redraw
//purpose  :
//=======================================================================

void NIS_View::redraw (const Handle(NIS_View)&    theView,
                       const NIS_Drawer::DrawType theType)
{
  for (Standard_Integer aPriority = 0; aPriority < NIS_Drawer::Priority_LevelsNb; ++aPriority)
  {
    for (NCollection_List<NIS_InteractiveContext *>::Iterator anIter = myContexts;
         anIter.More(); anIter.Next())
    {
      anIter.Value()->redraw (theView, theType, (NIS_Drawer::PriorityLevel )aPriority);
    }
  }
}

//=======================================================================
//function : DynamicHilight
//purpose  :
//=======================================================================

Standard_Boolean NIS_View::DynamicHilight  (const Standard_Integer theX,
                                            const Standard_Integer theY)
{
  Standard_Boolean aResult(Standard_False);
  myDetected.Clear();

  const Handle(NIS_InteractiveObject) aSelected =
    Pick (theX, theY, NIS_SelectFilter::DynHilight);

  // ASV: if at least one Context returns IsSelectable()==False,
  // hilight is canceled, this method returns
  if ((aSelected.IsNull() == Standard_False &&
       aSelected->IsSelectable() == Standard_False) ||
      aSelected == myDynHilighted)
  {
    ;//DrawDirect(theX, theY);
  } else {
    //DrawDirect(theX, theY, Standard_False);
    const Handle(NIS_View) aView (this);
    if (myDynHilighted.IsNull() == Standard_False)
      if (myDynHilighted->GetDrawer().IsNull() == Standard_False)
        myDynHilighted->GetDrawer()->SetDynamicHilighted(Standard_False,
                                                         myDynHilighted, aView);

    // 30.07.10 - NKV - synchronize behaviour with AIS interactive context (if need)
    if (aSelected.IsNull() ||
        (myDoHilightSelected == Standard_False &&
         aSelected->GetDrawer()->GetContext()->IsSelected(aSelected)))
    {
      myDynHilighted.Nullify();
    }
    else {
      aSelected->GetDrawer()->SetDynamicHilighted (Standard_True,
                                                   aSelected, aView);
      myDynHilighted = aSelected;
    }
    Redraw();
    aResult = Standard_True;
  }
  return aResult;
}

//=======================================================================
//function : DynamicUnhilight
//purpose  :
//=======================================================================

void NIS_View::DynamicUnhilight(const Handle_NIS_InteractiveObject& theObj)
{
  if (theObj.IsNull() == Standard_False && theObj == myDynHilighted) {
    const Handle(NIS_View) aView (this);
    if (myDynHilighted->GetDrawer().IsNull() == Standard_False)
      myDynHilighted->GetDrawer()->SetDynamicHilighted (Standard_False,
                                                        myDynHilighted, aView);
    myDynHilighted.Nullify();
    Redraw();
  }
}

//=======================================================================
//function : Select
//purpose  : selection by single click
//=======================================================================

void NIS_View::Select (const Standard_Integer theX,
                       const Standard_Integer theY,
                       const Standard_Boolean isModifierUsed,
                       const Standard_Boolean theRedraw)
{
  myDetected.Clear();
  Standard_Real anOver;
  const gp_Ax1 anAxis = PickAxis(theX, theY, anOver);
  const Handle(NIS_InteractiveObject) aSelected =
    Pick(anAxis, anOver, Standard_True, 0L, NIS_SelectFilter::Pick);

  NCollection_List<NIS_InteractiveContext *>::Iterator anIter (myContexts);
  Standard_Boolean isProcessed(Standard_False);
  for (; anIter.More(); anIter.Next()) {
    NIS_InteractiveContext* aCtx = anIter.Value();
    if (aCtx != 0L)
      if (aCtx->ProcessSelection (aSelected, isModifierUsed))
        isProcessed = Standard_True;
  }
  if (aSelected == myDynHilighted && aSelected.IsNull() == Standard_False)
  {
    myDynHilighted.Nullify();
    const Handle(NIS_Drawer)& aDrawer = aSelected->GetDrawer();
    aDrawer->SetDynamicHilighted (Standard_False, aSelected, this);
    isProcessed = Standard_True;
  }
  if (theRedraw && isProcessed)
    Redraw();
}

//=======================================================================
//function : Select
//purpose  : selection by rectange
//=======================================================================

void NIS_View::Select (const Standard_Integer  theXmin,
                       const Standard_Integer  theYmin,
                       const Standard_Integer  theXmax,
                       const Standard_Integer  theYmax,
                       const Standard_Boolean  isModifierUsed,
                       const Standard_Boolean  isFullyIncluded,
                       const Standard_Boolean  theRedraw)
{
  myDetected.Clear();
  Standard_Real anX, anY, aZ;
  if (theXmin == theXmax || theYmin == theYmax)
    return;

  //Transformed box corresponding to the selected rectangle
  Proj (anX, anY, aZ);                  // vector orthogonal to the view plane
  const gp_Dir aProj (anX, anY, aZ);

  Convert(theXmin, theYmin, anX, anY, aZ); // 3D point for the 3D coordinates
  const gp_Pnt anEye (anX, anY, aZ);

  Convert(theXmax, theYmin, anX, anY, aZ); // 3D point for the 3D coordinates
  const gp_XYZ anXdir (gp_XYZ(anX, anY, aZ) - anEye.XYZ());
  const gp_Ax3 anAx3 (anEye, aProj, anXdir);
  gp_Trsf aTrf;
  aTrf.SetTransformation (anAx3);
  const gp_Trsf aTrfInv = aTrf.Inverted();

  Convert(theXmax, theYmax, anX, anY, aZ); // 3D point for the 3D coordinates
  gp_XYZ anUpperCorner (anX, anY, aZ);
  aTrf.Transforms(anUpperCorner);

  // Selecting box
  Bnd_B3f aBoxSel;
  aBoxSel.Add (gp_XYZ(0., 0., -10000.));
  aBoxSel.Add (anUpperCorner);

  TColStd_PackedMapOfInteger mapSelected;
  NCollection_List<NIS_InteractiveContext *>::Iterator anIterC (myContexts);
  for (; anIterC.More(); anIterC.Next()) {
    NIS_InteractiveContext * pCtx = anIterC.Value();
    mapSelected.Clear();
    pCtx->selectObjects (mapSelected, aBoxSel, aTrfInv, aTrf, isFullyIncluded);
    pCtx->ProcessSelection (mapSelected, isModifierUsed);
  }
  if (theRedraw) Redraw();
}

//=======================================================================
//function : Select
//purpose  : Selection by polygon
//=======================================================================

void  NIS_View::Select (const NCollection_List<gp_XY> &thePolygon,
                        const Standard_Boolean         isModifierUsed,
                        const Standard_Boolean         isFullyIncluded,
                        const Standard_Boolean         theRedraw)
{
  myDetected.Clear();
  if (thePolygon.Extent() < 3)
    return;

  Standard_Real anX, anY, aZ;

  //Transformed box corresponding to the selected rectangle
  Proj (anX, anY, aZ);                  // vector orthogonal to the view plane
  const gp_Dir aProj (anX, anY, aZ);

  const gp_XY &aPf = thePolygon.First();
  // 3D point for the 3D coordinates
  Convert((Standard_Integer) aPf.X(), (Standard_Integer) aPf.Y(), anX, anY, aZ);
  const gp_Pnt anEye (anX, anY, aZ);

  // 3D point for the 3D coordinates
  const gp_XY &aPl = thePolygon.Last();

  Convert((Standard_Integer) aPl.X(), (Standard_Integer) aPl.Y(), anX, anY, aZ);

  // Compute transformation.
  const gp_XYZ anXdir (gp_XYZ(anX, anY, aZ) - anEye.XYZ());
  if (anXdir.Modulus() <= gp::Resolution())
  {
    return;
  }

  const gp_Ax3 anAx3 (anEye, aProj, anXdir);
  gp_Trsf aTrf;
  aTrf.SetTransformation (anAx3);

  // Prepare list of 2d points of selection polygon.
  NCollection_List<gp_XY>           aPoints;
  NCollection_List<gp_XY>::Iterator anIter(thePolygon);
  Bnd_B2f                           aPolyBox;

  for (; anIter.More(); anIter.Next()) {
    const gp_XY &aP = anIter.Value();

    Convert((Standard_Integer) aP.X(), (Standard_Integer) aP.Y(), anX, anY, aZ);
    gp_XYZ aP3d(anX, anY, aZ);

    aTrf.Transforms(aP3d);

    gp_XY aP2d(aP3d.X(), aP3d.Y());

    aPoints.Append(aP2d);
    aPolyBox.Add(aP2d);
  }

  TColStd_PackedMapOfInteger                           mapSelected;
  NCollection_List<NIS_InteractiveContext *>::Iterator anIterC(myContexts);

  for (; anIterC.More(); anIterC.Next()) {
    NIS_InteractiveContext * pCtx = anIterC.Value();
    mapSelected.Clear();
    pCtx->selectObjects (mapSelected, aPoints, aPolyBox, aTrf, isFullyIncluded);
    pCtx->ProcessSelection (mapSelected, isModifierUsed);
  }

  if (theRedraw) Redraw();
}

//=======================================================================
//function : PickAxis
//purpose  :
//=======================================================================

gp_Ax1 NIS_View::PickAxis       (const Standard_Integer theX,
                                 const Standard_Integer theY,
                                 Standard_Real&         theOver)
{
  // Find the ray passing through the clicked point in the view window.
  Standard_Real anX, anY, aZ;
  Convert(theX, theY, anX, anY, aZ);  // 3D point for the 3D coordinates
  const gp_Pnt anEye (anX, anY, aZ);
#ifdef OBSOLETE
  Proj (anX, anY, aZ);                // vector orthogonal to the view plane
  const gp_Dir aProj (-anX, -anY, -aZ);
  const gp_Ax1 anAxis (anEye, aProj);

  Convert (theX+1, theY+1, anX, anY, aZ);
  theOver= ((gp_XYZ(anX, anY, aZ) - anEye.XYZ()) ^ aProj.XYZ()).Modulus() * 1.5;
#endif
  theOver = 2 * myPixelSize.Modulus();
  return gp_Ax1(anEye, -gp_Dir(myProj));
}

//=======================================================================
//function : Pick
//purpose  : 
//=======================================================================

Handle_NIS_InteractiveObject NIS_View::Pick
                                (const Standard_Integer theX,
                                 const Standard_Integer theY,
                                 const NIS_SelectFilter::Event theEvent)
{
  Standard_Real anOver;
  const gp_Ax1 anAxis = PickAxis(theX, theY, anOver);
  return Pick(anAxis, anOver, Standard_True, 0L, theEvent);
}

//=======================================================================
//function : Pick
//purpose  :
//=======================================================================

Handle_NIS_InteractiveObject NIS_View::Pick
                                (const gp_Ax1&           theAxis,
                                 const Standard_Real     theOver,
                                 const Standard_Boolean  isOnlySelectable,
                                 Standard_Real*          theDistance,
                                 const NIS_SelectFilter::Event theEvent)
{
  typedef NCollection_List<NIS_InteractiveContext::DetectedEnt> LstDetected;
  LstDetected aDetected;
  for (NCollection_List<NIS_InteractiveContext*>::Iterator anIterC (myContexts);
       anIterC.More(); anIterC.Next())
  {
    anIterC.Value()->selectObject (aDetected, theAxis, theOver, isOnlySelectable, theEvent);
  }

  // simple iterating is enough to create list of detected objects
  // in the order of increasing distance
  myDetected.Clear();
  for (LstDetected::Iterator anIt (aDetected); anIt.More(); anIt.Next())
  {
    myDetected.Append (anIt.Value().PObj);
  }

  Handle(NIS_InteractiveObject) aSelected = aDetected.IsEmpty() ? (NIS_InteractiveObject* )NULL : aDetected.First().PObj;
  if (theDistance != NULL)
  {
    *theDistance = aDetected.IsEmpty() ? (0.1 * RealLast()) : aDetected.First().Dist;
  }

  if (aSelected.IsNull())
  {
    return aSelected;
  }

  // notify the SelectFilter if available
  NIS_InteractiveContext* aSelectedCtx = aSelected->GetDrawer()->GetContext();
  if (aSelectedCtx != NULL
   && !aSelectedCtx->GetFilter().IsNull()
   && aSelectedCtx->GetFilter()->ActsOn (aSelected->DynamicType()))
  {
    NIS_SelectFilter::Event aFinalEvent = NIS_SelectFilter::Indefinite;
    if (theEvent == NIS_SelectFilter::Pick)
    {
      aFinalEvent = NIS_SelectFilter::Picked;
    }
    else if (theEvent == NIS_SelectFilter::DynHilight)
    {
      aFinalEvent = NIS_SelectFilter::DynHilighted;
    }
    aSelectedCtx->GetFilter()->IsOk (aSelected.operator->(), theAxis,
                                     theOver, aFinalEvent);
  }
  return aSelected;
}

//=======================================================================
//function : UnConvert
//purpose  : 2d pixel coord => 3d point coord in Ortho viewer
//=======================================================================

gp_XYZ NIS_View::UnConvert (const Standard_Integer theX,
                            const Standard_Integer theY) const
{
  const Visual3d_ViewOrientation anOri = ViewOrientation();
  Standard_Real anX, anY, aZ, anXp, anYp;
  anOri.ViewReferencePoint().Coord(anX, anY, aZ);
  gp_XYZ anOrigin(anX, anY, aZ);
  anOri.ViewReferencePlane().Coord(anX, anY, aZ);
  gp_XYZ aNorm(anX, anY, aZ);
  anOri.ViewReferenceUp().Coord(anX, anY, aZ);
  gp_XYZ anYdir(anX, anY, aZ);
  gp_XYZ anXdir = anYdir ^ aNorm;
  Convert(theX, theY, anXp, anYp);
  return (anOrigin + anXdir * anXp + anYdir * anYp);
}

//=======================================================================
//function : ClipToViewport
//purpose  : Test and clip the given coordinates to fit to the Viewport
//=======================================================================

Standard_Boolean NIS_View::ClipToViewport  (Standard_Integer& theX,
                                            Standard_Integer& theY) const
{
  Standard_Integer iResult(0);
  if (theX < myViewport[0])
    theX = myViewport[0];
  else if (theX >= myViewport[0] + myViewport[2])
    theX = myViewport[0] + myViewport[2] - 1;
  else
    iResult |= 0x1; 
  if (theY < myViewport[1])
    theY = myViewport[1];
  else if (theY >= myViewport[1] + myViewport[3])
    theY = myViewport[1] + myViewport[3] - 1;
  else
    iResult |= 0x2;
  return iResult == 0x3;
}

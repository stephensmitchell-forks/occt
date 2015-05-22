// File:      NIS_PolygonSelector.cxx
// Created:   26.10.11 21:26
// Author:    Alexander GRIGORIEV
// Copyright: Open Cascade 2011

#include <NIS_PolygonSelector.hxx>
#include <NIS_InteractiveContext.hxx>
#include <NIS_View.hxx>
#ifdef WIN32
#include <windows.h>
#endif
#include <GL/gl.h>

/**
 * Drawer class for PolygonSelector.
 */
class NIS_PolygonSelectorDrawer : public NIS_Drawer
{
public:

    /**
     * Display the given interactive object in the given view.
     */
    virtual void Draw     (const Handle_NIS_InteractiveObject&,
                           const DrawType         theType,
                           const NIS_DrawList&    theDrawList);

public:
    // Declaration of CASCADE RTTI
    DEFINE_STANDARD_RTTI (NIS_PolygonSelectorDrawer)
};

// Definition of HANDLE object using Standard_DefineHandle.hxx
DEFINE_STANDARD_HANDLE (NIS_PolygonSelectorDrawer, NIS_Drawer)

IMPLEMENT_STANDARD_HANDLE  (NIS_PolygonSelector, NIS_InteractiveObject)
IMPLEMENT_STANDARD_RTTIEXT (NIS_PolygonSelector, NIS_InteractiveObject)
IMPLEMENT_STANDARD_HANDLE  (NIS_PolygonSelectorDrawer, NIS_Drawer)
IMPLEMENT_STANDARD_RTTIEXT (NIS_PolygonSelectorDrawer, NIS_Drawer)

//=======================================================================
//function : ~NIS_PolygonSelector
//purpose  : Destructor
//=======================================================================

NIS_PolygonSelector::~NIS_PolygonSelector ()
{
}

//=======================================================================
//function : DefaultDrawer
//purpose  : 
//=======================================================================

NIS_Drawer * NIS_PolygonSelector::DefaultDrawer(NIS_Drawer * theDrawer) const
{
  NIS_PolygonSelectorDrawer * aDrawer = 
    theDrawer ? static_cast<NIS_PolygonSelectorDrawer *>(theDrawer)
    : new NIS_PolygonSelectorDrawer();
  aDrawer->SetCompiled(Standard_False);

  return aDrawer;
}

//=======================================================================
//function : Clone
//purpose  : 
//=======================================================================

void NIS_PolygonSelector::Clone
                        (const Handle_NCollection_BaseAllocator& theAlloc,
                         Handle_NIS_InteractiveObject&           theDest) const
{
  Handle(NIS_PolygonSelector) aNewObj;
  if (theDest.IsNull())
  {
    aNewObj = new NIS_PolygonSelector(myIsRectangle);
    theDest = aNewObj;
  }
  else
  {
    aNewObj = reinterpret_cast<NIS_PolygonSelector*> (theDest.operator->());
  }
  NIS_InteractiveObject::Clone(theAlloc, theDest);
  aNewObj->myView = myView;
  aNewObj->myIsRectangle = myIsRectangle;
  aNewObj->myIsLoop = myIsLoop;
  if (myPoints.Extent() > 0)
  {
    aNewObj->myPoints = myPoints;
    aNewObj->myVertexes =
      new NCollection_Array1<Standard_ShortReal>(*myVertexes.operator->());
  }
}

//=======================================================================
//function : Intersect
//purpose  : Not used
//=======================================================================

Standard_Real NIS_PolygonSelector::Intersect (const gp_Ax1&,
                                             const Standard_Real) const
{
  return RealLast();
}

//=======================================================================
//function : computeBox
//purpose  : Not used
//=======================================================================

void NIS_PolygonSelector::computeBox ()
{
}

//=======================================================================
//function : AddPoint
//purpose  : Add a point to polygon
//=======================================================================

void NIS_PolygonSelector::AddPoint (const Standard_Integer theX,
                                    const Standard_Integer theY)
{
  // add the point to the list
  gp_XYZ aPnt = convertPoint( theX, theY );
  myPoints.Prepend( aPnt );

  updateVertexes();
}

//=======================================================================
//function : MovePoint
//purpose  : Change the location of the last added point.
//=======================================================================

void NIS_PolygonSelector::MovePoint (const Standard_Integer theX,
                                     const Standard_Integer theY)
{
  if (myPoints.IsEmpty())
    AddPoint( theX, theY );
  else if (myIsRectangle && myPoints.Extent() == 1)
    AddPoint( theX, theY );
  else {
    gp_XYZ aMovedPnt = convertPoint( theX, theY );

    gp_XYZ& aPnt = const_cast<gp_XYZ&>( myPoints.First() );
    aPnt.SetX( aMovedPnt.X() );
    aPnt.SetY( aMovedPnt.Y() );
    aPnt.SetZ( aMovedPnt.Z() );

    updateVertexes();
  }
}

//=======================================================================
//function : RemovePoint
//purpose  : Remove the last added point
//=======================================================================

void NIS_PolygonSelector::RemovePoint ()
{
  if (NPoints() > 0)
  {
    myPoints.RemoveFirst();
    updateVertexes();
  }
}

//=======================================================================
//function : Draw
//purpose  : 
//=======================================================================

void NIS_PolygonSelectorDrawer::Draw(const Handle_NIS_InteractiveObject& theObj,
                                     const DrawType         theType,
                                     const NIS_DrawList&    theDrawList)
{
  if (theObj.IsNull() == Standard_False &&
      theObj->IsKind(STANDARD_TYPE(NIS_PolygonSelector)))
  {
    const NIS_PolygonSelector * pObj =
      static_cast<const NIS_PolygonSelector *>(theObj.operator->());
    Standard_Integer nPnts = pObj->NPoints();
    if (pObj->myView.IsNull() == Standard_False &&
        theDrawList.GetView().IsNull() == Standard_False)
    {
      if (pObj->myView != theDrawList.GetView())
        nPnts = 0;
    }
    if (nPnts > 0)
    {
      GLenum aMode;
      glEnableClientState(GL_VERTEX_ARRAY);
      const Standard_ShortReal* aVertexes =
        &pObj->myVertexes->Value( pObj->myVertexes->Lower() );
      glVertexPointer(3, GL_FLOAT, 0, aVertexes );
      if (nPnts == 1)
      {
        glEnable(GL_POINT_SMOOTH);
        glPointSize(3.f);
        aMode = GL_POINTS;
      }
      else
      {
        glDisable(GL_LINE_SMOOTH);
        //         glEnable(GL_LINE_STIPPLE);
        //         glLineStipple(1, 0x3333);
        glLineWidth(0.5f);
        if (nPnts == 2)
        {
          aMode = GL_LINES;
        }
        else if (!pObj->myIsRectangle && !pObj->myIsLoop)
        {
          aMode = GL_LINE_STRIP;
        }
        else
        {
          aMode = GL_LINE_LOOP;
        }
      }
      glDisable (GL_LIGHTING);
      Quantity_Parameter R1, R2, R3;
      GetColor(theType).Values(R1, R2, R3, Quantity_TOC_RGB);
      glColor3d (R1, R2, R3);
      glEnable(GL_BLEND);
      glBlendFunc(GL_ONE_MINUS_DST_COLOR, GL_ZERO);
      glDrawArrays(aMode, 0, nPnts);
      glDisable(GL_BLEND);
      //       glDisable(GL_LINE_STIPPLE);
      glDisableClientState(GL_VERTEX_ARRAY);
      glEnable (GL_LIGHTING);
    }
  }
}

//=======================================================================
//function : convertPoint
//purpose  : 
//=======================================================================

gp_XYZ NIS_PolygonSelector::convertPoint ( Standard_Integer theX,
                                           Standard_Integer theY )
{
  Standard_Real aPickPnt[3];
  const Handle(NIS_View) aView = getView();
  if (aView.IsNull() == Standard_False)
  {
    aView->Convert (static_cast<Standard_Integer>(theX),
                    static_cast<Standard_Integer>(theY),
                    aPickPnt[0], aPickPnt[1], aPickPnt[2]);
  }
  return gp_XYZ( aPickPnt[0], aPickPnt[1], aPickPnt[2] );
}

//=======================================================================
//function : getView
//purpose  : 
//=======================================================================

const Handle_NIS_View& NIS_PolygonSelector::getView () const
{
  const Handle(NIS_Drawer)& aDrawer = GetDrawer();
  if (aDrawer.IsNull() == Standard_False) {
    const NIS_InteractiveContext * aCtx = aDrawer->GetContext();
    if (aCtx != NULL) {
      NCollection_List<Handle_NIS_View>::Iterator aViews = aCtx->GetViews();
      if (aViews.More())
        return aViews.Value();
    }
  }
  static const Handle(NIS_View) aNull;
  return aNull;
}

//=======================================================================
//function : Perform
//purpose  : 
//=======================================================================

void NIS_PolygonSelector::Perform (const Standard_Boolean isModifierUsed,
                                   const Standard_Boolean isFullyIncluded,
                                   const Standard_Boolean isRedraw) const
{
  // Find the View
  Handle(NIS_View) aView = myView;
  if (myView.IsNull() && GetDrawer().IsNull() == Standard_False)
  {
    const NIS_InteractiveContext * aCtx = GetDrawer()->GetContext();
    if (aCtx != 0L)
    {
      NCollection_List<Handle_NIS_View>::Iterator anIterV = aCtx->GetViews();
      if (anIterV.More())
        aView = anIterV.Value();
    }
  }
  if (aView.IsNull() == Standard_False)
  {
    NCollection_List<gp_XY> aSelectionPnts = GetPolygon();
    if (myIsRectangle == Standard_False)
    {
      // Polygon selection
      aView->Select( aSelectionPnts, isModifierUsed, isFullyIncluded, isRedraw );
    }
    else
    {
      // Rectangle selection
      Standard_Integer aV[4] = { 
        static_cast<Standard_Integer>( aSelectionPnts.First().X() + 0.1 ),
        static_cast<Standard_Integer>( aSelectionPnts.First().Y() + 0.1 ),
        static_cast<Standard_Integer>( aSelectionPnts.Last().X() + 0.1 ),
        static_cast<Standard_Integer>( aSelectionPnts.Last().Y() + 0.1 )
      };
      if (aV[0] > aV[2]) {
        const Standard_Integer aTmp = aV[0];
        aV[0] = aV[2];
        aV[2] = aTmp;
      }
      if (aV[1] > aV[3]) {
        const Standard_Integer aTmp = aV[1];
        aV[1] = aV[3];
        aV[3] = aTmp;
      }
      aView->Select(aV[0], aV[1], aV[2], aV[3], isModifierUsed, isFullyIncluded, isRedraw);
    }
  }
}

//=======================================================================
//function : GetPolygon
//purpose  : 
//=======================================================================

NCollection_List<gp_XY> NIS_PolygonSelector::GetPolygon() const
{
  NCollection_List<gp_XY> aScreenCoords;
  const Handle_NIS_View& aView = getView();
  if (aView.IsNull() == Standard_False)
  {
    NCollection_List<gp_XYZ>::Iterator anIt(myPoints);
    Standard_Integer aScreenX, aScreenY;
    for (; anIt.More(); anIt.Next())
    {
      gp_XYZ aViewPnt = anIt.Value();
      aView->Convert(aViewPnt.X(), aViewPnt.Y(), aViewPnt.Z(),
                     aScreenX, aScreenY);
      aScreenCoords.Append( gp_XY(aScreenX, aScreenY));
    }
  }
  return aScreenCoords;
}

//=======================================================================
//function : updateVertexes
//purpose  : 
//=======================================================================

void NIS_PolygonSelector::updateVertexes()
{
  myVertexes = new NCollection_Array1<Standard_ShortReal>(0, NPoints() * 3);

  if (myIsRectangle)
  {
    gp_XYZ aPnt1 = myPoints.Last();
    gp_XYZ aPnt2 = myPoints.First();

    Quantity_Parameter anUpX, anUpY, anUpZ;
    Handle(NIS_View) aView = getView();
    aView->Up( anUpX, anUpY, anUpZ );        
    gp_XYZ anUp( anUpX, anUpY, anUpZ );

    gp_XYZ aPnt3 = aPnt1 - anUp * (anUp * (aPnt1 - aPnt2));
    gp_XYZ aPnt4 = aPnt2 + anUp * (anUp * (aPnt1 - aPnt2));

    myVertexes->ChangeValue(0) = static_cast<Standard_ShortReal>(aPnt1.X());
    myVertexes->ChangeValue(1) = static_cast<Standard_ShortReal>(aPnt1.Y());
    myVertexes->ChangeValue(2) = static_cast<Standard_ShortReal>(aPnt1.Z());

    myVertexes->ChangeValue(3) = static_cast<Standard_ShortReal>(aPnt3.X());
    myVertexes->ChangeValue(4) = static_cast<Standard_ShortReal>(aPnt3.Y());
    myVertexes->ChangeValue(5) = static_cast<Standard_ShortReal>(aPnt3.Z()); 

    myVertexes->ChangeValue(6) = static_cast<Standard_ShortReal>(aPnt2.X());
    myVertexes->ChangeValue(7) = static_cast<Standard_ShortReal>(aPnt2.Y());
    myVertexes->ChangeValue(8) = static_cast<Standard_ShortReal>(aPnt2.Z()); 

    myVertexes->ChangeValue(9) = static_cast<Standard_ShortReal>(aPnt4.X());
    myVertexes->ChangeValue(10) = static_cast<Standard_ShortReal>(aPnt4.Y());
    myVertexes->ChangeValue(11) = static_cast<Standard_ShortReal>(aPnt4.Z());
  }
  else
  {
    Standard_Integer i = 0;
    NCollection_List<gp_XYZ>::Iterator anIter( myPoints );
    for (; anIter.More(); anIter.Next())
    {
      myVertexes->ChangeValue(i++) =
        static_cast<Standard_ShortReal>( anIter.Value().X() );
      myVertexes->ChangeValue(i++) =
        static_cast<Standard_ShortReal>( anIter.Value().Y() );
      myVertexes->ChangeValue(i++) =
        static_cast<Standard_ShortReal>( anIter.Value().Z() );
    }
  }

  setDrawerUpdate();
}

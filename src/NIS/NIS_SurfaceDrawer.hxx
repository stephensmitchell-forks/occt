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

#ifndef NIS_SurfaceDrawer_HeaderFile
#define NIS_SurfaceDrawer_HeaderFile

#include <NIS_Drawer.hxx>
#include <gp_Trsf.hxx>
//#include <Quantity_Color.hxx>

class NIS_Surface;

/**
 * Drawer for interactive object type NIS_Surface.
 * @ingroup nis_library
 */

class NIS_SurfaceDrawer : public NIS_Drawer
{
 public:
  // ---------- PUBLIC METHODS ----------

  /**
   * Constructor.
   */
  Standard_EXPORT NIS_SurfaceDrawer(const Quantity_Color   &theNormal,
                                    const Quantity_Color   &theHilight
                                                        = Quantity_NOC_GRAY65,
                                    const Quantity_Color   &theDynHilight
                                                        = Quantity_NOC_CYAN1);

  /**
   * Define the color used for the back side of rendered triangles.
   * By default this color is the same as the 'Normal' color. 
   */
  inline void                  SetBackColor (const Quantity_Color& theColor)
  {
    myBackColor = theColor;
  }

  /**
   * Define the specularity that is the property of displayed material for both
   * Front and Back faces. Can be in the range 0 to 1. Default value is 0.58
   */
  inline void                   SetSpecularity   (const Standard_Real theValue)
  {
    mySpecularity = static_cast<Standard_ShortReal>(theValue);
  } 

  /**
   * Sets the transformation to the drawer.
   */
  inline void                  SetTransformation (const gp_Trsf &theTrsf)
  {
    myTrsf = theTrsf;
  }

  /**
   * Returns the transformation to the drawer.
   */
  inline const gp_Trsf&        GetTransformation () const
  {
    return myTrsf;
  }

  /**
   * Sets the surface offset
   */
  inline void                  SetPolygonOffset  (const Standard_Real theOffset)
  {
    myPolygonOffset = static_cast<Standard_ShortReal>(theOffset);
  }

  /**
   * Get the surface offset.
   */
  inline Standard_Real         GetPolygonOffset  () const
  {
    return static_cast<Standard_Real>(myPolygonOffset);
  }

  /**
   * Copy the relevant information from another instance of Drawer.
   * raises exception if theOther has incompatible type (test IsKind).
   */
  Standard_EXPORT virtual void Assign   (const Handle_NIS_Drawer& theOther);

  /**
   * Called before execution of Draw(), once per group of interactive objects.
   */
  Standard_EXPORT virtual void BeforeDraw(const DrawType         theType,
                                          const NIS_DrawList&    theDrawList);

  /**
   * Called after execution of Draw(), once per group of interactive objects.
   */
  Standard_EXPORT virtual void AfterDraw(const DrawType         theType,
                                         const NIS_DrawList&    theDrawList);

  /**
   * Main function: display the given interactive object in the given view.
   */
  Standard_EXPORT virtual void Draw     (const Handle_NIS_InteractiveObject&,
                                         const DrawType         theType,
                                         const NIS_DrawList&    theDrawList);

  /**
   * Matching two instances, for Map interface.
   */
  Standard_EXPORT virtual Standard_Boolean
                               IsEqual  (const Handle_NIS_Drawer& theOth)const;


protected:
  Standard_EXPORT virtual void redraw   (const DrawType         theType,
                                         const Handle_NIS_View& theView);

private:
  void                  drawEdges       (const NIS_Surface *    pObject,
                                         const DrawType         theType);

private:
  Quantity_Color      myBackColor;
  Quantity_Color      myFreeEdgeColor;
  Standard_ShortReal  mySpecularity;
  gp_Trsf             myTrsf;
  Standard_ShortReal  myPolygonOffset;
  Standard_Boolean    myIsWireframe;  
  Standard_Boolean    myHilightIsWireframe;  
  Standard_Boolean    myIsShowEdges;  

  Standard_Boolean    myIsBlend;        // defined and used internally

  friend class NIS_Surface;

 public:
// Declaration of CASCADE RTTI
DEFINE_STANDARD_RTTI (NIS_SurfaceDrawer)
};

// Definition of HANDLE object using Standard_DefineHandle.hxx
DEFINE_STANDARD_HANDLE (NIS_SurfaceDrawer, NIS_Drawer)

#endif

// File:      NIS_Tools.hxx
// Created:   09.07.11 23:47
// Copyright: Open Cascade 2011


#ifndef NIS_Tools_HeaderFile
#define NIS_Tools_HeaderFile

#include <NIS_Surface.hxx>

#include <TopoDS_Shape.hxx>
#include <TopTools_DataMapOfIntegerShape.hxx>

template <class A> class NCollection_List;
class Handle_NIS_InteractiveContext;

/**
 * class NIS_Tools
 * Auxiliary algoritms and tools.
 */
class NIS_Tools 
{
 public:
  // ---------- PUBLIC METHODS ----------

  /**
   * Enumerated types of object for local selection
   */
  enum DecomposeType {
    Type_None   = 0x001,
    Type_Face   = 0x002,
    Type_Edge   = 0x004,
    Type_Vertex = 0x008
  };

  struct PresentableShape {
    TopoDS_Shape     Shape;
    Standard_Integer PresentationID;
  };

  /**
   * Function for creating the Local selection.
   * Update of viewer should be called by GUI layer. This is done for
   * performance reasons to have one update per all the operations.
   * @param theShape
   *   Original shape to be decomposed.
   * @param theToler
   *   Tolerance for triangulation/tessellation of subshapes.
   * @param theTypes
   *   Combination of bits, defined in enum DecomposeType.
   * @param theCtx
   *   The context where all created objects will be displayed.
   * @param theMap
   *   map of shapes
   * @param theColor
   *   Color for displaing of new IO in Normal mode
   * @param theHlgMode
   *   Set the hilight display mode for surface(Shading or Wirefram)
   *   The default mode is Shading
   * @returns
   *   the list of ID of new presentations (NIS_Surface for Faces and
   *   NIS_Triangulated for edges).
   *   If theCtx is NULL then returns an empty list and false.
   */
  Standard_EXPORT static Standard_Boolean Decompose
                (const TopoDS_Shape&                            theShape,
                 const Standard_Real                            theToler,
                 const Standard_Integer                         theTypes,
                 const Handle_NIS_InteractiveContext&           theCtx, 
                 TopTools_DataMapOfIntegerShape&                theMap,
                 const Quantity_Color&                          theColor,
                 const NIS_Surface::DisplayMode                 theHlgMode = NIS_Surface::Shading,
                 const Standard_Real                            theLineWidth = 1.f);

};

#endif

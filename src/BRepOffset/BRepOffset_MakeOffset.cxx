// Created on: 1995-10-27
// Created by: Yves FRICAUD
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

//  Modified by skv - Tue Mar 15 16:20:43 2005
// Add methods for supporting history.

//  Modified by skv - Mon Jan 12 11:50:02 2004 OCC4455

#include <BRepOffset_MakeOffset.ixx>
#include <BRepOffset_Analyse.hxx>
#include <BRepOffset_DataMapOfShapeOffset.hxx> 
#include <BRepOffset_DataMapOfShapeMapOfShape.hxx>
#include <BRepOffset_DataMapIteratorOfDataMapOfShapeOffset.hxx>
#include <BRepOffset_Interval.hxx>
#include <BRepOffset_ListOfInterval.hxx>
#include <BRepOffset_Offset.hxx>
#include <BRepOffset_Tool.hxx>
#include <BRepOffset_Inter2d.hxx>
#include <BRepOffset_Inter3d.hxx>
#include <BRepOffset_MakeLoops.hxx>


#include <BRepAdaptor_Surface.hxx>
#include <BRepCheck_Edge.hxx>
#include <BRepCheck_Vertex.hxx>
#include <BRepLib.hxx>
#include <BRepLib_MakeVertex.hxx>
#include <BRep_Builder.hxx>
#include <BRep_Tool.hxx>
#include <BRep_TVertex.hxx>
#include <BRepTools_Quilt.hxx>
#include <BRepClass3d_SolidClassifier.hxx>
#include <gp_Pnt.hxx>

#include <TopExp.hxx>
#include <TopExp_Explorer.hxx>
#include <TopoDS.hxx>
#include <TopoDS_Solid.hxx>
#include <TopoDS_Shell.hxx>
#include <TopoDS_Compound.hxx>
#include <TopoDS_Face.hxx>
#include <TopoDS_Edge.hxx>
#include <TopoDS_Vertex.hxx>

#include <TopTools_MapOfShape.hxx>
#include <TopTools_MapIteratorOfMapOfShape.hxx>
#include <TopTools_ListOfShape.hxx>
#include <TopTools_ListIteratorOfListOfShape.hxx>
#include <TopTools_DataMapOfShapeInteger.hxx>
#include <TopTools_DataMapOfShapeShape.hxx>
#include <TopTools_DataMapIteratorOfDataMapOfShapeShape.hxx>
#include <TopTools_DataMapIteratorOfDataMapOfShapeReal.hxx>
#include <TopTools_IndexedDataMapOfShapeListOfShape.hxx>
#include <TopTools_DataMapIteratorOfDataMapOfShapeListOfShape.hxx>
#include <TColStd_ListIteratorOfListOfInteger.hxx>

#include <Standard_NotImplemented.hxx>
#include <Standard_ConstructionError.hxx>
#include <Precision.hxx>

#include <TopTools_SequenceOfShape.hxx>
#include <Geom_OffsetSurface.hxx>
#include <Geom_ConicalSurface.hxx>
#include <TopTools_IndexedMapOfShape.hxx>
#include <BRep_TEdge.hxx>
#include <BRepTools.hxx>
#include <gp_Cone.hxx>
#include <ElSLib.hxx>
#include <ElCLib.hxx>
#include <gp_Lin2d.hxx>
#include <GCE2d_MakeLine.hxx>
#include <Geom2d_Line.hxx>
#include <TopoDS_Iterator.hxx>
#include <BRepLib_MakeFace.hxx>
#include <Geom_Circle.hxx>

#include <BRep_PointRepresentation.hxx>
#include <BRep_ListIteratorOfListOfPointRepresentation.hxx>
#include <GeomAPI_ProjectPointOnCurve.hxx>

#include <BRepAdaptor_Curve.hxx>
#include <BRepAdaptor_Curve2d.hxx>
#include <Geom_SphericalSurface.hxx>
#include <TopoDS_Wire.hxx>
#include <BRepTools_Substitution.hxx>
#include <Geom_TrimmedCurve.hxx>
#include <Geom2d_TrimmedCurve.hxx>

#include <BRepTools_WireExplorer.hxx>
#include <BRepLib_MakeEdge.hxx>
#include <gce_MakeDir.hxx>
#include <GC_MakeCylindricalSurface.hxx>
#include <gce_MakeCone.hxx>
#include <Geom_SurfaceOfLinearExtrusion.hxx>

#include <Geom2dAdaptor_HCurve.hxx>
#include <GeomAdaptor_HSurface.hxx>
#include <Adaptor3d_CurveOnSurface.hxx>
#include <GeomLib.hxx>
#include <GeomFill_Generator.hxx>
#include <Geom_Plane.hxx>
#include <IntTools_FClass2d.hxx>
#include <BRepLib_FindSurface.hxx>

#include <BOPDS_DS.hxx>

#include <BOPAlgo_PaveFiller.hxx>
#include <BOPAlgo_Builder.hxx>
#include <BOPAlgo_MakerVolume.hxx>

#include <BOPCol_ListOfShape.hxx>
#include <BOPCol_DataMapOfShapeShape.hxx>
#include <BOPCol_IndexedDataMapOfShapeListOfShape.hxx>

#include <BOPTools.hxx>
#include <BOPTools_AlgoTools3D.hxx>
#include <BOPTools_AlgoTools.hxx>

#include <IntTools_Context.hxx>
#include <IntTools_ShrunkRange.hxx>

#include <CPnts_AbscissaPoint.hxx>


// POP for NT
#include <stdio.h>

#ifdef DRAW

#include <DBRep.hxx>
#endif
#ifdef OCCT_DEBUG
#include <OSD_Chronometer.hxx>
//#define DEB_VERB
  Standard_Boolean AffichInt2d = Standard_False;       
  Standard_Boolean AffichOffC  = Standard_False;       
  Standard_Boolean ChronBuild  = Standard_False;
  Standard_Integer NbAE        = 0;
  Standard_Integer NbAF        = 0;  
  Standard_Integer NVP        = 0;  
  Standard_Integer NVM        = 0;  
  Standard_Integer NVN        = 0;  
  static OSD_Chronometer  Clock;
  char name[100];




//=======================================================================
//function :  DEBVerticesControl
//purpose  : 
//=======================================================================

static void DEBVerticesControl (const TopTools_IndexedMapOfShape& NewEdges,
                                      Handle(BRepAlgo_AsDes)      AsDes)
{
  TopTools_ListOfShape               LVP;
  TopTools_ListIteratorOfListOfShape it1LE ;    
  TopTools_ListIteratorOfListOfShape it2LE ;
  
  Standard_Integer i;
  for (i = 1; i <= NewEdges.Extent(); i++) {
    const TopoDS_Edge& NE = TopoDS::Edge(NewEdges(i));
    if (AsDes->HasDescendant(NE)) {
      for (it1LE.Initialize(AsDes->Descendant(NE)); it1LE.More(); it1LE.Next()) {
        if (AsDes->Ascendant(it1LE.Value()).Extent() < 3) {
          LVP.Append(it1LE.Value());
          cout <<"Vertex on at least 3 edges."<<endl;
#ifdef DRAW
          if (AffichInt2d) {
            sprintf (name,"VP_%d",NVP++);
            DBRep::Set(name,it1LE.Value());
          }
#endif
        }
        else if (AsDes->Ascendant(it1LE.Value()).Extent() > 3) {
          cout <<"Vertex on more than 3 edges."<<endl;
#ifdef DRAW
          if (AffichInt2d) {
            sprintf (name,"VM_%d",NVM++);
            DBRep::Set(name,it1LE.Value());
          }
#endif
          
        }
        else {
#ifdef DRAW
          if (AffichInt2d) {
            sprintf (name,"VN_%d",NVN++);
            DBRep::Set(name,it1LE.Value());
          }
#endif
        }
      }
    }
  }
  //------------------------------------------------
  // Try to mix spoiled vertices.
  //------------------------------------------------
  BRep_Builder B;
  TopTools_ListIteratorOfListOfShape it1(LVP);
  Standard_Real                      TolConf = 1.e-5;
  Standard_Real                      Tol     = Precision::Confusion();
  //Standard_Integer                   i = 1;
  
  i = 1;
  for ( ; it1.More(); it1.Next()) {
    TopoDS_Shape   V1 = it1.Value();
    gp_Pnt         P1 = BRep_Tool::Pnt(TopoDS::Vertex(V1));
    Standard_Real  distmin = Precision::Infinite();
    TopTools_ListIteratorOfListOfShape it2(LVP);
    Standard_Integer j = 1;

    for ( ; it2.More(); it2.Next()) {
      if (j > i) {
        TopoDS_Shape V2 = it2.Value();
        gp_Pnt       P2 = BRep_Tool::Pnt(TopoDS::Vertex(V2));
        if (!V1.IsSame(V2)) {
          Standard_Real       dist    = P1.Distance(P2);
          if (dist < distmin) distmin = dist;
          if (dist < TolConf) {
            Standard_Real UV2;
            TopoDS_Edge   EWE2;
            const TopTools_ListOfShape& EdgeWithV2 = AsDes->Ascendant(V2);
            TopTools_ListIteratorOfListOfShape itAsDes;
            for (itAsDes.Initialize(EdgeWithV2); itAsDes.More(); itAsDes.Next()) {
              EWE2  = TopoDS::Edge(itAsDes.Value());
              TopoDS_Shape aLocalShape = V2.Oriented(TopAbs_INTERNAL);
              UV2   = BRep_Tool::Parameter(TopoDS::Vertex(aLocalShape),EWE2);
              aLocalShape = V1.Oriented(TopAbs_INTERNAL) ;
              B.UpdateVertex(TopoDS::Vertex(aLocalShape),UV2,EWE2,Tol);
//              UV2   = 
//                BRep_Tool::Parameter(TopoDS::Vertex(),EWE2);
//              B.UpdateVertex(TopoDS::Vertex(V1.Oriented(TopAbs_INTERNAL)),
//                             UV2,EWE2,Tol);
            }
            AsDes->Replace(V2,V1);
          }
        }
      }
      j++;
    }
    i++;
    cout <<" distmin between VP : "<<distmin<<endl;
  }
}  
#endif


//=======================================================================
// static methods
//=======================================================================
static
  void BuildSplitsOfFaces(const TopTools_ListOfShape& theLF,
                          Handle(BRepAlgo_AsDes)& theAsDes,
                          TopTools_DataMapOfShapeListOfShape& theEdgesOrigins,
                          TopTools_DataMapOfShapeShape& theFacesOrigins,
                          BRepAlgo_Image& theImage,
                          TopTools_DataMapOfShapeShape& theETrimEInf);

static
  void BuildSplitsOfFaces(const TopTools_ListOfShape& theLF,
                          Handle(BRepAlgo_AsDes)& theAsDes,
                          BRepAlgo_Image& theImage);

static
  void FillHistory(const TopTools_IndexedDataMapOfShapeListOfShape& theFImages,
                   const TopTools_DataMapOfShapeListOfShape& theEImages,
                   BRepAlgo_Image& theImage);

static
  void BuildSplitsOfFaces(const TopTools_ListOfShape& theLF,
                          Handle(BRepAlgo_AsDes)& theAsDes,
                          const TopTools_MapOfShape& theModifiedEdges,
                          TopTools_DataMapOfShapeListOfShape& theEdgesOrigins,
                          TopTools_DataMapOfShapeShape& theFacesOrigins,
                          TopTools_DataMapOfShapeListOfShape& theOEImages,
                          TopTools_DataMapOfShapeListOfShape& theOEOrigins,
                          TopTools_MapOfShape& theLastInvEdges,
                          TopTools_IndexedMapOfShape& theEdgesToAvoid,
                          TopTools_IndexedMapOfShape& theInvEdges,
                          TopTools_IndexedMapOfShape& theValidEdges,
                          TopTools_DataMapOfShapeInteger& theAlreadyInvFaces,
                          TopTools_IndexedDataMapOfShapeListOfShape& theInvFaces,
                          TopTools_DataMapOfShapeShape& theArtInvFaces,
                          TopTools_IndexedDataMapOfShapeListOfShape& theFImages);

static
  void BuildSplitsOfFace(const TopoDS_Face& theFace,
                         const TopoDS_Shape& theEdges,
                         Standard_Boolean bLimited,
                         TopTools_DataMapOfShapeShape& theOrigins,
                         TopTools_ListOfShape& theLFImages);

static
  Standard_Boolean GetEdges(const TopoDS_Face& theFace,
                            const Handle(BRepAlgo_AsDes)& theAsDes,
                            const TopTools_DataMapOfShapeListOfShape& theEImages,
                            const TopTools_MapOfShape& theLastInvEdges,
                            const TopTools_IndexedMapOfShape& theInvEdges,
                            Handle(IntTools_Context)& theCtx,
                            const TopTools_MapOfShape& theModifiedEdges,
                            TopoDS_Shape& theEdges,
                            TopTools_MapOfShape& theInv);

static
  void IntersectTrimmedEdges(const TopTools_ListOfShape& theLF,
                             const Handle(BRepAlgo_AsDes)& theAsDes,
                             TopTools_DataMapOfShapeListOfShape& theOEImages,
                             TopTools_DataMapOfShapeListOfShape& theOEOrigins,
                             TopTools_DataMapOfShapeListOfShape& theEdgesOrigins,
                             Handle(IntTools_Context)& theCtx,
                             TopTools_MapOfShape& theNewEdges,
                             TopTools_DataMapOfShapeShape& theETrimEInf);

static
  void FindInvalidEdges(const TopoDS_Face& theF,
                        const TopTools_ListOfShape& theLFImages,
                        const TopTools_DataMapOfShapeListOfShape& theEdgesOrigins,
                        const TopTools_DataMapOfShapeShape& theFacesOrigins,
                        const TopTools_DataMapOfShapeListOfShape& theOEImages,
                        const TopTools_DataMapOfShapeListOfShape& theOEOrigins,
                        TopTools_IndexedMapOfShape& theInvEdges,
                        TopTools_IndexedMapOfShape& theValidEdges,
                        TopTools_DataMapOfShapeListOfShape& theDMFLVE,
                        TopTools_DataMapOfShapeListOfShape& theDMFLNE,
                        TopTools_DataMapOfShapeListOfShape& theDMFLIE,
                        TopTools_MapOfShape& theMEInverted);

static
  void FindInvalidFaces(TopTools_ListOfShape& theLFImages,
                        const TopTools_IndexedMapOfShape& theInvEdges,
                        const TopTools_IndexedMapOfShape& theValidEdges,
                        const TopTools_DataMapOfShapeListOfShape& theDMFLVE,
                        const TopTools_ListOfShape& theLEInvalid,
                        const TopTools_ListOfShape& theLENeutral,
                        const TopTools_MapOfShape& theMEInverted,
                        TopTools_ListOfShape& theInvFaces);

static
  void RebuildFaces(const TopTools_IndexedDataMapOfShapeListOfShape& theFToRebuild,
                    const TopTools_MapOfShape& theFSelfRebAvoid,
                    TopTools_IndexedDataMapOfShapeListOfShape& theDMFFIm,
                    TopTools_DataMapOfShapeListOfShape& theEdgesOrigins,
                    TopTools_DataMapOfShapeShape& theFacesOrigins,
                    TopTools_DataMapOfShapeListOfShape& theOEImages,
                    TopTools_DataMapOfShapeListOfShape& theOEOrigins,
                    TopTools_MapOfShape& theLastInvEdges,
                    TopTools_IndexedMapOfShape& theEdgesToAvoid,
                    TopTools_IndexedMapOfShape& theInvEdges,
                    TopTools_IndexedMapOfShape& theValidEdges,
                    TopTools_DataMapOfShapeInteger& theAlreadyInvFaces,
                    TopTools_IndexedDataMapOfShapeListOfShape& theInvFaces,
                    const TopTools_DataMapOfShapeShape& theArtInvFaces,
                    TopTools_MapOfShape& theVertsToAvoid,
                    TopTools_DataMapOfShapeShape& theETrimEInf,
                    Handle(BRepAlgo_AsDes)& theAsDes);

static
  void IntersectFaces(const TopTools_IndexedDataMapOfShapeListOfShape& theFToRebuild,
                      const TopTools_MapOfShape& theFSelfRebAvoid,
                      TopTools_IndexedDataMapOfShapeListOfShape& theDMFFIm,
                      TopTools_DataMapOfShapeListOfShape& theEdgesOrigins,
                      TopTools_DataMapOfShapeListOfShape& theOEImages,
                      TopTools_DataMapOfShapeListOfShape& theOEOrigins,
                      TopTools_IndexedMapOfShape& theInvEdges,
                      TopTools_IndexedMapOfShape& theValidEdges,
                      TopTools_IndexedDataMapOfShapeListOfShape& theInvFaces,
                      const TopTools_DataMapOfShapeShape& theArtInvFaces,
                      TopTools_MapOfShape& theVertsToAvoid,
                      TopTools_DataMapOfShapeShape& theETrimEInf,
                      TopTools_MapOfShape& theModifiedEdges,
                      Handle(BRepAlgo_AsDes)& theAsDes);

static
  void IntersectAndTrimEdges(const TopTools_IndexedDataMapOfShapeListOfShape& theFToRebuild,
                             const TopTools_IndexedMapOfShape& theMFInt,
                             const TopTools_IndexedMapOfShape& theMEInt,
                             const TopTools_DataMapOfShapeListOfShape& theDMEETrim,
                             const TopTools_IndexedMapOfShape& theMSInv,
                             const TopTools_IndexedMapOfShape& theMVE,
                             const TopTools_MapOfShape& theVertsToAvoid,
                             const TopTools_MapOfShape& theNewVertsToAvoid,
                             TopTools_MapOfShape& theMVBounds,
                             TopTools_DataMapOfShapeListOfShape& theEImages);

static
  void GetInvalidEdges(BOPAlgo_Builder& theGF,
                       //const TopTools_ListOfShape& theLENew,
                       const TopTools_MapOfShape& theVertsToAvoid,
                       const TopTools_MapOfShape& theMVBounds,
                       TopTools_MapOfShape& theMEInv);

static 
  void UpdateValidEdges(const TopTools_IndexedDataMapOfShapeListOfShape& theFToRebuild,
                        const TopTools_IndexedDataMapOfShapeListOfShape& theDMFFIm,
                        const TopTools_IndexedMapOfShape& theInvEdges,
                        TopTools_DataMapOfShapeListOfShape& theEdgesOrigins,
                        TopTools_DataMapOfShapeListOfShape& theOEImages,
                        TopTools_DataMapOfShapeListOfShape& theOEOrigins,
                        TopTools_IndexedDataMapOfShapeListOfShape& theFLE,
                        TopTools_MapOfShape& theVertsToAvoid,
                        TopTools_DataMapOfShapeShape& theETrimEInf,
                        TopTools_DataMapOfShapeListOfShape& theEImages,
                        TopTools_DataMapOfShapeListOfShape& theEETrim,
                        TopTools_MapOfShape& theMVBounds,
                        TopTools_MapOfShape& theModifiedEdges,
                        Handle(BRepAlgo_AsDes)& theAsDes);

static 
  void BuildSplitsOfInvFaces(const TopTools_IndexedDataMapOfShapeListOfShape& theFToRebuild, 
                             const TopTools_MapOfShape& theModifiedEdges,
                             TopTools_IndexedDataMapOfShapeListOfShape& theDMFFIm,
                             TopTools_DataMapOfShapeListOfShape& theEdgesOrigins,
                             TopTools_DataMapOfShapeShape& theFacesOrigins,
                             TopTools_DataMapOfShapeListOfShape& theOEImages,
                             TopTools_DataMapOfShapeListOfShape& theOEOrigins,
                             TopTools_MapOfShape& theLastInvEdges,
                             TopTools_IndexedMapOfShape& theEdgesToAvoid,
                             TopTools_MapOfShape& theVertsToAvoid,
                             TopTools_DataMapOfShapeInteger& theAlreadyInvFaces,
                             TopTools_IndexedMapOfShape& theValidEdges,
                             TopTools_DataMapOfShapeShape& theETrimEInf, 
                             Handle(BRepAlgo_AsDes)& theAsDes);

static
  Standard_Boolean CheckIfArtificial(const TopoDS_Shape& theF,
                                     const TopTools_ListOfShape& theLFImages,
                                     const TopoDS_Shape& theCE,
                                     const TopTools_MapOfShape& theMapEInv,
                                     TopTools_MapOfShape& theMENInv,
                                     Handle(BRepAlgo_AsDes)& theAsDes,
                                     const TopTools_DataMapOfShapeListOfShape& theOEImages);

static 
  void RemoveInvalidSplitsFromValid(TopTools_IndexedDataMapOfShapeListOfShape& theFImages,
                                    const TopTools_IndexedDataMapOfShapeListOfShape& theInvFaces,
                                    const TopTools_DataMapOfShapeShape& theArtInvFaces,
                                    const TopTools_MapOfShape& theMEInverted);

static
  void RemoveInsideFaces(TopTools_IndexedDataMapOfShapeListOfShape& theFImages,
                         TopTools_IndexedDataMapOfShapeListOfShape& theInvFaces,
                         const TopTools_DataMapOfShapeShape& theArtInvFaces,
                         const TopTools_IndexedMapOfShape& theInvEdges,
                         const TopTools_IndexedMapOfShape& theMFToCheckInt,
                         TopTools_IndexedMapOfShape& theMERemoved);

static
  void FilterEdgesImages(const TopoDS_Shape& theS,
                         TopTools_DataMapOfShapeListOfShape& theOEImages,
                         TopTools_DataMapOfShapeListOfShape& theOEOrigins);

static
  void FilterInvalidFaces(TopTools_IndexedDataMapOfShapeListOfShape& theInvFaces,
                          TopTools_DataMapOfShapeShape& theArtInvFaces,
                          const TopTools_IndexedDataMapOfShapeListOfShape& theFImages,
                          const TopTools_IndexedDataMapOfShapeListOfShape& theDMFE);

static
  void FilterInvalidEdges(TopTools_IndexedMapOfShape& theInvEdges,
                          const TopTools_IndexedDataMapOfShapeListOfShape& theInvFaces,
                          const TopTools_DataMapOfShapeShape& theArtInvFaces,
                          const TopTools_DataMapOfShapeListOfShape& theDMFLIE,
                          const TopTools_IndexedMapOfShape& theMERemoved);

static
  void RemoveValidSplits(TopTools_IndexedDataMapOfShapeListOfShape& theImages,
                        BOPAlgo_Builder& theGF,
                        const TopTools_MapOfShape& theSpRem,
                        TopTools_IndexedMapOfShape& theMERemoved);

static
  void RemoveInvalidSplits(TopTools_IndexedDataMapOfShapeListOfShape& theImages,
                           const TopTools_DataMapOfShapeShape& theArtInvFaces,
                           const TopTools_IndexedMapOfShape& theInvEdges,
                           BOPAlgo_Builder& theGF,
                           const TopTools_MapOfShape& theSpRem);

static
  void PrepareFacesForIntersection(const TopTools_IndexedDataMapOfShapeListOfShape& theFToRebuild,
                                   const TopTools_IndexedDataMapOfShapeListOfShape& theFImages,
                                   TopTools_IndexedDataMapOfShapeListOfShape& theInvFaces,
                                   const TopTools_DataMapOfShapeShape& theArtInvFaces,
                                   TopTools_IndexedDataMapOfShapeListOfShape& theFLE,
                                   TopTools_DataMapOfShapeListOfShape& theMDone,
                                   TopTools_DataMapOfShapeListOfShape& theDMSF,
                                   TopTools_DataMapOfShapeListOfShape& theMEInfETrim,
                                   TopTools_DataMapOfShapeListOfShape& theDMVEFull,
                                   TopTools_DataMapOfShapeShape& theETrimEInf,
                                   TopTools_IndexedDataMapOfShapeListOfShape& theDMEFInv,
                                   const Standard_Boolean bLookVertToAvoid);

static
  void FindVerticesToAvoid(const TopTools_IndexedDataMapOfShapeListOfShape& theDMEFInv,
                           TopTools_DataMapOfShapeListOfShape& theDMVEFull,
                           const TopTools_IndexedMapOfShape& theInvEdges,
                           const TopTools_IndexedMapOfShape& theValidEdges,
                           TopTools_MapOfShape& theMVRInv);

static
  void FindFacesForIntersection(const TopoDS_Shape& theFInv,
                                const TopTools_IndexedMapOfShape& theME,
                                const TopTools_IndexedDataMapOfShapeListOfShape& theFImages,
                                const TopTools_DataMapOfShapeShape& theArtInvFaces,
                                const TopTools_DataMapOfShapeListOfShape& theDMSF,
                                const TopTools_MapOfShape& theMVInvAll,
                                TopTools_IndexedMapOfShape& theMFAvoid,
                                TopTools_IndexedMapOfShape& theMFInt,
                                TopTools_ListOfShape& theLFImInt);

static
  void ProcessCommonEdges(const TopTools_ListOfShape& theLEC,
                          const TopTools_IndexedMapOfShape& theInvEdges,
                          const TopTools_IndexedMapOfShape& theValidEdges,
                          const TopTools_IndexedMapOfShape& theME,
                          const TopTools_DataMapOfShapeShape& theETrimEInf,
                          const TopTools_DataMapOfShapeListOfShape& theMEInfETrim,
                          TopTools_IndexedMapOfShape& theMECV,
                          TopTools_DataMapOfShapeListOfShape& theDMEETrim,
                          TopTools_ListOfShape& theLFEi,
                          TopTools_ListOfShape& theLFEj,
                          TopTools_IndexedMapOfShape& theMEToInt);

static
  void UpdateIntersectedFaces(const TopoDS_Shape& theFInv,
                              const TopoDS_Shape& theFi,
                              const TopoDS_Shape& theFj,
                              const TopTools_ListOfShape& theLFInv,
                              const TopTools_ListOfShape& theLFImi,
                              const TopTools_ListOfShape& theLFImj,
                              const TopTools_ListOfShape& theLFEi,
                              const TopTools_ListOfShape& theLFEj,
                              TopTools_DataMapOfShapeListOfShape& theEdgesOrigins,
                              TopTools_IndexedMapOfShape& theMEToInt);

static
  void IntersectFaces(const TopoDS_Shape& theFInv,
                      const TopoDS_Shape& theFi,
                      const TopoDS_Shape& theFj,
                      const TopTools_ListOfShape& theLFInv,
                      const TopTools_ListOfShape& theLFImi,
                      const TopTools_ListOfShape& theLFImj,
                      TopTools_ListOfShape& theLFEi,
                      TopTools_ListOfShape& theLFEj,
                      TopTools_DataMapOfShapeListOfShape& theEdgesOrigins,
                      TopTools_IndexedMapOfShape& theMECV,
                      TopTools_IndexedMapOfShape& theMEToInt);

static
  void TrimNewIntersectionEdges(const TopTools_ListOfShape& theLE,
                                TopTools_DataMapOfShapeListOfShape& theEImages,
                                const TopTools_DataMapOfShapeListOfShape& theEETrim,
                                TopTools_MapOfShape& theMEB,
                                TopTools_MapOfShape& theMVOld,
                                TopTools_MapOfShape& theMVBounds,
                                TopTools_ListOfShape& theLENew,
                                BOPCol_ListOfShape& theLA);

static
  void IntersectEdges(const BOPCol_ListOfShape& theLA,
                      const TopTools_ListOfShape& theLE,
                      const TopTools_ListOfShape& theLENew,
                      TopTools_DataMapOfShapeListOfShape& theEImages,
                      TopTools_MapOfShape& theVertsToAvoid,
                      TopTools_MapOfShape& theMVBounds,
                      TopTools_MapOfShape& theModifiedEdges,
                      TopTools_MapOfShape& theMENew,
                      TopoDS_Shape& theSplits);

static
  void GetBounds(const TopTools_IndexedDataMapOfShapeListOfShape& theFToRebuild,
                 const TopTools_IndexedDataMapOfShapeListOfShape& theDMFFIm,
                 const TopTools_MapOfShape& theMEB,
                 TopoDS_Shape& theBounds);

static
  void GetBoundsToUpdate(const TopTools_IndexedDataMapOfShapeListOfShape& theFToRebuild,
                         const TopTools_DataMapOfShapeListOfShape& theOEImages,
                         const TopTools_DataMapOfShapeListOfShape& theOEOrigins,
                         const TopTools_MapOfShape& theMEB,
                         TopTools_ListOfShape& theLABounds,
                         TopTools_ListOfShape& theLAValid,
                         TopoDS_Shape& theBounds,
                         Handle(BRepAlgo_AsDes)& theAsDes);

static
  void GetInvalidEdgesByBounds(const TopoDS_Shape& theSplits,
                               const TopoDS_Shape& theBounds,
                               const TopTools_IndexedDataMapOfShapeListOfShape& theFToRebuild,
                               const TopTools_IndexedDataMapOfShapeListOfShape& theDMFFIm,
                               const TopTools_IndexedMapOfShape& theInvEdges,
                               const TopTools_MapOfShape& theMVOld,
                               const TopTools_MapOfShape& theMENew,
                               TopTools_MapOfShape& theVertsToAvoid,
                               TopTools_MapOfShape& theMEInv);

static
  void UpdateNewIntersectionEdges(const TopTools_ListOfShape& theLE,
                                  const TopTools_DataMapOfShapeListOfShape& theMELF,
                                  const TopTools_DataMapOfShapeListOfShape& theEImages,
                                  TopTools_DataMapOfShapeListOfShape& theEdgesOrigins,
                                  TopTools_DataMapOfShapeListOfShape& theOEImages,
                                  TopTools_DataMapOfShapeListOfShape& theOEOrigins,
                                  TopTools_DataMapOfShapeShape& theETrimEInf,
                                  TopTools_DataMapOfShapeListOfShape& theEETrim,
                                  TopTools_MapOfShape& theModifiedEdges,
                                  Handle(BRepAlgo_AsDes)& theAsDes);

static 
  void FindFacesToRebuild(TopTools_IndexedDataMapOfShapeListOfShape&  theLFImages,
                          const TopTools_IndexedMapOfShape& theInvEdges,
                          const TopTools_IndexedDataMapOfShapeListOfShape& theInvFaces,
                          TopTools_IndexedDataMapOfShapeListOfShape& theFToRebuild,
                          TopTools_MapOfShape& theFSelfRebAvoid);

static 
  void FindCommonParts(const TopTools_ListOfShape& theLF1,
                       const TopTools_ListOfShape& theLF2,
                       TopTools_ListOfShape& theLEC,
                       const TopAbs_ShapeEnum theType = TopAbs_EDGE);

static
  void GetEnlargedFaces(const TopoDS_Shape& theShape,
                        const BRepOffset_DataMapOfShapeOffset& theMapSF,
                        const TopTools_DataMapOfShapeShape& theMES,
                        TopTools_DataMapOfShapeShape& theFacesOrigins,
                        BRepAlgo_Image& theImage,
                        TopTools_ListOfShape& theLSF);

static 
  Standard_Boolean FindShape(const TopoDS_Shape& theSWhat,
                             const TopoDS_Shape& theSWhere,
                             TopoDS_Shape& theRes);

static
  void UpdateImages(const TopTools_ListOfShape& theLA,
                    TopTools_DataMapOfShapeListOfShape& theImages,
                    BOPAlgo_Builder& theGF,
                    TopTools_MapOfShape& theModified);

static 
  void UpdateOrigins(const TopTools_ListOfShape& theLA,
                     TopTools_DataMapOfShapeListOfShape& theOrigins,
                     BOPAlgo_Builder& theGF);

static void UpdateIntersectedEdges(const TopTools_ListOfShape& theLA,
                                   TopTools_DataMapOfShapeShape& theETrimEInf,
                                   BOPAlgo_Builder& theB);

static 
  Standard_Boolean ProcessMicroEdge(const TopoDS_Edge& theEdge,
                                    const Handle(IntTools_Context)& theCtx);

static 
  Standard_Boolean CheckNormals(const TopoDS_Face& theFIm,
                                const TopoDS_Face& theFOr);

static 
  void UpdateInitOffset(BRepAlgo_Image&         myInitOffset,
                        BRepAlgo_Image&         myImageOffset,
                        const TopoDS_Shape&     myOffsetShape,
                        const TopAbs_ShapeEnum &theShapeType);

static 
  void RemoveShapes(TopoDS_Shape& theS,
                    const TopTools_ListOfShape& theLS);

static 
  Standard_Boolean IsSolid(const TopoDS_Shape& theS);

static 
  void UpdateHistory(const BOPCol_ListOfShape& theLF,
                     BOPAlgo_Builder& theGF,
                     BRepAlgo_Image& theImage);

static
  Standard_Boolean IsPlanar(const TopoDS_Shape& theS);

static
  Standard_Integer NbPoints(const TopoDS_Edge& theE);

static
  void AppendToList(TopTools_ListOfShape& theL,
                    const TopoDS_Shape& theS);

static 
  void FindOrigins(const TopTools_ListOfShape& theLFIm1,
                   const TopTools_ListOfShape& theLFIm2,
                   const TopTools_IndexedMapOfShape& theME,
                   const TopTools_DataMapOfShapeListOfShape& theOrigins,
                   TopTools_ListOfShape& theLEOr);

static 
  void TrimEdges(const TopoDS_Shape& theShape,
                 const Standard_Real theOffset,
                 BRepOffset_DataMapOfShapeOffset& theMapSF,
                 TopTools_DataMapOfShapeShape& theMES,
                 TopTools_DataMapOfShapeShape& theBuild,
                 Handle(BRepAlgo_AsDes)& theAsDes,
                 Handle(BRepAlgo_AsDes)& theAsDes2d,
                 TopTools_IndexedMapOfShape& theNewEdges,
                 TopTools_DataMapOfShapeShape& theETrimEInf,
                 TopTools_DataMapOfShapeListOfShape& theEdgesOrigins);


static BRepOffset_Error checkSinglePoint(const Standard_Real theUParam,
                                         const Standard_Real theVParam,
                                         const Handle(Geom_Surface)& theSurf,
                                         const NCollection_Vector<gp_Pnt>& theBadPoints);

//---------------------------------------------------------------------
static void UpdateTolerance (      TopoDS_Shape&               myShape,
                             const TopTools_IndexedMapOfShape& myFaces);

//---------------------------------------------------------------------
//
static Standard_Boolean FindParameter(const TopoDS_Vertex& V, 
                                      const TopoDS_Edge& E,
                                      Standard_Real& U)
{
  // Search the vertex in the edge

  Standard_Boolean rev = Standard_False;
  TopoDS_Shape VF;
  TopAbs_Orientation orient = TopAbs_INTERNAL;

  TopoDS_Iterator itv(E.Oriented(TopAbs_FORWARD));

  // if the edge has no vertices
  // and is degenerated use the vertex orientation
  // RLE, june 94

  if (!itv.More() && BRep_Tool::Degenerated(E)) {
    orient = V.Orientation();
  }

  while (itv.More()) {
    const TopoDS_Shape& Vcur = itv.Value();
    if (V.IsSame(Vcur)) {
      if (VF.IsNull()) {
        VF = Vcur;
      }
      else {
        rev = E.Orientation() == TopAbs_REVERSED;
        if (Vcur.Orientation() == V.Orientation()) {
          VF = Vcur;
        }
      }
    }
    itv.Next();
  }
  
  if (!VF.IsNull()) orient = VF.Orientation();
 
  Standard_Real f,l;

  if (orient ==  TopAbs_FORWARD) {
    BRep_Tool::Range(E,f,l);
    //return (rev) ? l : f;
    U = (rev) ? l : f;
    return Standard_True;
  }
 
  else if (orient ==  TopAbs_REVERSED) {
    BRep_Tool::Range(E,f,l);
    //return (rev) ? f : l;
    U = (rev) ? f : l;
    return Standard_True;
   }

  else {
    TopLoc_Location L;
    const Handle(Geom_Curve)& C = BRep_Tool::Curve(E,L,f,l);
    L = L.Predivided(V.Location());
    if (!C.IsNull() || BRep_Tool::Degenerated(E)) {
      BRep_ListIteratorOfListOfPointRepresentation itpr
        ((*((Handle(BRep_TVertex)*) &V.TShape()))->Points());

      while (itpr.More()) {
        const Handle(BRep_PointRepresentation)& pr = itpr.Value();
        if (pr->IsPointOnCurve(C,L)) {
          Standard_Real p = pr->Parameter();
          Standard_Real res = p;// SVV 4 nov 99 - to avoid warnings on Linux
          if (!C.IsNull()) {
            // Closed curves RLE 16 june 94
            if (Precision::IsNegativeInfinite(f))
              {
                //return pr->Parameter();//p;
                U = pr->Parameter();
                return Standard_True;
              }
            if (Precision::IsPositiveInfinite(l))
              {
                //return pr->Parameter();//p;
                U = pr->Parameter();
                return Standard_True;
              }
            gp_Pnt Pf = C->Value(f).Transformed(L.Transformation());
            gp_Pnt Pl = C->Value(l).Transformed(L.Transformation());
            Standard_Real tol = BRep_Tool::Tolerance(V);
            if (Pf.Distance(Pl) < tol) {
              if (Pf.Distance(BRep_Tool::Pnt(V)) < tol) {
                if (V.Orientation() == TopAbs_FORWARD) res = f;//p = f;
                else                                   res = l;//p = l;
              }
            }
          }
          //return res;//p;
          U = res;
          return Standard_True;
        }
        itpr.Next();
      }
    }
    else {
      // no 3d curve !!
      // let us try with the first pcurve
      Handle(Geom2d_Curve) PC;
      Handle(Geom_Surface) S;
      BRep_Tool::CurveOnSurface(E,PC,S,L,f,l);
      L = L.Predivided(V.Location()); 
      BRep_ListIteratorOfListOfPointRepresentation itpr
        ((*((Handle(BRep_TVertex)*) &V.TShape()))->Points());

      while (itpr.More()) {
        const Handle(BRep_PointRepresentation)& pr = itpr.Value();
        if (pr->IsPointOnCurveOnSurface(PC,S,L)) {
          Standard_Real p = pr->Parameter();
          // Closed curves RLE 16 june 94
          if (PC->IsClosed()) {
            if ((p == PC->FirstParameter()) || 
                (p == PC->LastParameter())) {
              if (V.Orientation() == TopAbs_FORWARD) p = PC->FirstParameter();
              else                                   p = PC->LastParameter();
            }
          }
          //return p;
          U = p;
          return Standard_True;
        }
        itpr.Next();
      }
    }
  }
  
  //Standard_NoSuchObject::Raise("BRep_Tool:: no parameter on edge");
  return Standard_False;
}

//=======================================================================
//function : GetEdgePoints
//purpose  : gets the first, last and middle points of the edge
//=======================================================================
static void GetEdgePoints(const TopoDS_Edge& anEdge,
                                      const TopoDS_Face& aFace,
                                      gp_Pnt& fPnt, gp_Pnt& mPnt,
                                      gp_Pnt& lPnt)
{
  Standard_Real f, l;
  Handle(Geom2d_Curve) theCurve = BRep_Tool::CurveOnSurface( anEdge, aFace, f, l );
  gp_Pnt2d fPnt2d = theCurve->Value(f);
  gp_Pnt2d lPnt2d = theCurve->Value(l);
  gp_Pnt2d mPnt2d = theCurve->Value(0.5*(f + l));
  Handle(Geom_Surface) aSurf = BRep_Tool::Surface(aFace);
  fPnt = aSurf->Value(fPnt2d.X(),fPnt2d.Y());
  lPnt = aSurf->Value(lPnt2d.X(),lPnt2d.Y());
  mPnt = aSurf->Value(mPnt2d.X(), mPnt2d.Y());
}

//=======================================================================
//function : FillContours
//purpose  : fills free boundary contours and faces connected (MapEF)
//=======================================================================
static void FillContours(const TopoDS_Shape& aShape,
                         const BRepOffset_Analyse& Analyser,
                         TopTools_DataMapOfShapeListOfShape& Contours,
                         TopTools_DataMapOfShapeShape& MapEF)
{
  TopTools_ListOfShape Edges;

  TopExp_Explorer Explo(aShape, TopAbs_FACE);
  BRepTools_WireExplorer Wexp;

  for (; Explo.More(); Explo.Next())
    {
      TopoDS_Face aFace = TopoDS::Face(Explo.Current());
      TopoDS_Iterator itf(aFace);
      for (; itf.More(); itf.Next())
        {
          TopoDS_Wire aWire = TopoDS::Wire(itf.Value());
          for (Wexp.Init(aWire, aFace); Wexp.More(); Wexp.Next())
            {
              TopoDS_Edge anEdge = Wexp.Current();
              if (BRep_Tool::Degenerated(anEdge))
                continue;
              const BRepOffset_ListOfInterval& Lint = Analyser.Type(anEdge);
              if (!Lint.IsEmpty() && Lint.First().Type() == BRepOffset_FreeBoundary)
                {
                  MapEF.Bind(anEdge, aFace);
                  Edges.Append(anEdge);
                }
            }
        }
    }

  TopTools_ListIteratorOfListOfShape itl;
  while (!Edges.IsEmpty())
    {
      TopoDS_Edge StartEdge = TopoDS::Edge(Edges.First());
      Edges.RemoveFirst();
      TopoDS_Vertex StartVertex, CurVertex;
      TopExp::Vertices(StartEdge, StartVertex, CurVertex, Standard_True);
      TopTools_ListOfShape aContour;
      aContour.Append(StartEdge);
      while (!CurVertex.IsSame(StartVertex))
        for (itl.Initialize(Edges); itl.More(); itl.Next())
          {
            TopoDS_Edge anEdge = TopoDS::Edge(itl.Value());
            TopoDS_Vertex V1, V2;
            TopExp::Vertices(anEdge, V1, V2);
            if (V1.IsSame(CurVertex) || V2.IsSame(CurVertex))
              {
                aContour.Append(anEdge);
                CurVertex = (V1.IsSame(CurVertex))? V2 : V1;
                Edges.Remove(itl);
                break;
              }
          }
      Contours.Bind(StartVertex, aContour);
    }
}


//
//-----------------------------------------------------------------------
//
//=======================================================================
//function : BRepOffset_MakeOffset
//purpose  : 
//=======================================================================
BRepOffset_MakeOffset::BRepOffset_MakeOffset()
{
  myAsDes = new BRepAlgo_AsDes();
}

//=======================================================================
//function : BRepOffset_MakeOffset
//purpose  : 
//=======================================================================
BRepOffset_MakeOffset::BRepOffset_MakeOffset(const TopoDS_Shape&    S, 
                                             const Standard_Real    Offset, 
                                             const Standard_Real    Tol, 
                                             const BRepOffset_Mode  Mode, 
                                             const Standard_Boolean Inter, 
                                             const Standard_Boolean SelfInter, 
                                             const GeomAbs_JoinType Join,
                                             const Standard_Boolean Thickening,
                                             const Standard_Boolean RemoveIntEdges,
                                             const Standard_Boolean RemInvFaces)
: 
myOffset     (Offset),
myTol        (Tol),
myShape      (S),
myMode       (Mode),
myInter      (Inter),
mySelfInter  (SelfInter),
myJoin       (Join),
myThickening    (Thickening),
myRemoveIntEdges(RemoveIntEdges),
myRemoveInvalidFaces(RemInvFaces),
myDone     (Standard_False)

{
  myAsDes = new BRepAlgo_AsDes();
  MakeOffsetShape();
}

//=======================================================================
//function : Initialize
//purpose  : 
//=======================================================================
void BRepOffset_MakeOffset::Initialize(const TopoDS_Shape&    S, 
                                       const Standard_Real    Offset, 
                                       const Standard_Real    Tol, 
                                       const BRepOffset_Mode  Mode,
                                       const Standard_Boolean Inter,
                                       const Standard_Boolean SelfInter,
                                       const GeomAbs_JoinType Join,
                                       const Standard_Boolean Thickening,
                                       const Standard_Boolean RemoveIntEdges,
                                       const Standard_Boolean RemInvFaces)
{
  myOffset     = Offset;
  myShape      = S;
  myTol        = Tol;
  myMode       = Mode;
  myInter      = Inter;
  mySelfInter  = SelfInter;
  myJoin       = Join;
  myThickening     = Thickening;
  myRemoveIntEdges = RemoveIntEdges;
  myRemoveInvalidFaces = RemInvFaces;
  myDone     = Standard_False;
  myIsPlanar = Standard_False;
  Clear();
}

//=======================================================================
//function : Clear
//purpose  : 
//=======================================================================
void BRepOffset_MakeOffset::Clear()
{
  myOffsetShape.Nullify();
  myInitOffsetFace .Clear();
  myInitOffsetEdge .Clear();
  myImageOffset    .Clear();
  myFaces          .Clear();  
  myFaceOffset     .Clear();
  myAsDes          ->Clear();
  myDone     = Standard_False;
}

//=======================================================================
//function : AddFace
//purpose  : 
//=======================================================================
void BRepOffset_MakeOffset::AddFace(const TopoDS_Face& F) {
  myFaces.Add(F);    
  //-------------
  // MAJ SD.
  //-------------
  myInitOffsetFace.SetRoot (F)  ;    
  myInitOffsetFace.Bind    (F,F);
  myImageOffset.SetRoot    (F)  ;  
}

//=======================================================================
//function : SetOffsetOnFace
//purpose  : 
//=======================================================================
void BRepOffset_MakeOffset::SetOffsetOnFace(const TopoDS_Face&  F, 
                                            const Standard_Real Off)
{
  if ( myFaceOffset.IsBound(F)) myFaceOffset.UnBind(F);
  myFaceOffset.Bind(F,Off);
}

//=======================================================================
//function : RemoveCorks
//purpose  : 
//=======================================================================

static void RemoveCorks (TopoDS_Shape&               S,
                         TopTools_IndexedMapOfShape& Faces)
{  
  TopoDS_Compound SS;
  BRep_Builder    B;
  B.MakeCompound (SS);
  //-----------------------------------------------------
  // Construction of a shape without caps.
  // and Orientation of caps as in shape S.
  //-----------------------------------------------------
  TopExp_Explorer exp(S,TopAbs_FACE);
  for (; exp.More(); exp.Next()) {
    const TopoDS_Shape& Cork = exp.Current(); 
    if (!Faces.Contains(Cork)) {
      B.Add(SS,Cork);
    }
    else {
      //Faces.Remove (Cork);
      //begin instead of Remove//
      TopoDS_Shape LastShape = Faces(Faces.Extent());
      Faces.RemoveLast();
      if (Faces.FindIndex(Cork) != 0)
        Faces.Substitute(Faces.FindIndex(Cork), LastShape);
      //end instead of Remove  //
      Faces.Add(Cork); // to reset it with proper orientation.
    }
  }
  S = SS;
#ifdef DRAW
  if ( AffichOffC) 
    DBRep::Set("myInit", SS);
#endif

}

//=======================================================================
//function : IsConnectedShell
//purpose  : 
//=======================================================================
static Standard_Boolean IsConnectedShell( const TopoDS_Shape& S )
{  
  BRepTools_Quilt Glue;
  Glue.Add( S );

  TopoDS_Shape SS = Glue.Shells();
  TopExp_Explorer Explo( SS, TopAbs_SHELL );
  Explo.Next();
  if (Explo.More())
    return Standard_False;
  
  return Standard_True;
}


//=======================================================================
//function : MakeList
//purpose  : 
//=======================================================================

static void MakeList (TopTools_ListOfShape&             OffsetFaces,
                      const BRepAlgo_Image&             myInitOffsetFace,
                      const TopTools_IndexedMapOfShape& myFaces)
{
  TopTools_ListIteratorOfListOfShape itLOF(myInitOffsetFace.Roots());
  for ( ; itLOF.More(); itLOF.Next()) {
    const TopoDS_Shape& Root = itLOF.Value();
    if (!myFaces.Contains(Root)) {
      if (myInitOffsetFace.HasImage(Root)) {
        OffsetFaces.Append(myInitOffsetFace.Image(Root).First());
      }
    }
  }
}

//=======================================================================
//function : EvalMax
//purpose  : 
//=======================================================================

static void EvalMax(const TopoDS_Shape& S, Standard_Real& Tol)
{
  TopExp_Explorer exp;
  for (exp.Init(S,TopAbs_VERTEX); exp.More(); exp.Next()) {
    const TopoDS_Vertex& V    = TopoDS::Vertex(exp.Current());
    Standard_Real        TolV = BRep_Tool::Tolerance(V); 
    if (TolV > Tol) Tol = TolV;
  }
}


//=======================================================================
//function : MakeOffsetShape
//purpose  : 
//=======================================================================
void BRepOffset_MakeOffset::MakeOffsetShape()
{  
  myDone = Standard_False;
  //
  // check if shape consists of only planar faces
  myIsPlanar = IsPlanar(myShape);
  //------------------------------------------
  // Construction of myShape without caps.
  //------------------------------------------
  if(!myFaces.IsEmpty())
  {
    RemoveCorks (myShape,myFaces);
  }

  if (! IsConnectedShell(myShape))
    Standard_ConstructionError::Raise("BRepOffset_MakeOffset : Incorrect set of faces to remove, the remaining shell is not connected");

  if (Abs(myOffset) <= myTol)
  {
    // Check for face with non-null offset value.
    Standard_Boolean isFound = Standard_False;
    TopTools_DataMapIteratorOfDataMapOfShapeReal anIter(myFaceOffset);
    for( ; anIter.More(); anIter.Next())
    {
      if (Abs(anIter.Value()) > myTol)
      {
        isFound = Standard_True;
        break;
      }
    }

    if (!isFound)
    {
      // No face with non-null offset found.
      return;
    }
  }

  TopAbs_State       Side = TopAbs_IN;
  if (myOffset < 0.) Side = TopAbs_OUT;

  // ------------
  // Preanalyse.
  // ------------
  EvalMax(myShape,myTol);
  // There are possible second variant: analytical continuation of arcsin.
  Standard_Real TolAngleCoeff = Min(myTol / (Abs(myOffset * 0.5) + Precision::Confusion()), 1.0);
  Standard_Real TolAngle = 4*ASin(TolAngleCoeff);
  myAnalyse.Perform(myShape,TolAngle);
  //---------------------------------------------------
  // Construction of Offset from preanalysis.
  //---------------------------------------------------  
  //----------------------------
  // MaJ of SD Face - Offset
  //----------------------------
  UpdateFaceOffset();

  if (myJoin == GeomAbs_Arc)          
    BuildOffsetByArc();
  else if (myJoin == GeomAbs_Intersection) 
    BuildOffsetByInter();
  //-----------------
  // Auto unwinding.
  //-----------------
  // if (mySelfInter)  SelfInter(Modif);
  //-----------------
  // Intersection 3d .
  //-----------------
  BRepOffset_Inter3d Inter(myAsDes,Side,myTol);
  Intersection3D (Inter);
  //-----------------
  // Intersection2D
  //-----------------
  TopTools_IndexedMapOfShape& Modif    = Inter.TouchedFaces(); 
  TopTools_IndexedMapOfShape& NewEdges = Inter.NewEdges();

  if (!Modif.IsEmpty()) Intersection2D (Modif,NewEdges);
  //-------------------------------------------------------
  // Unwinding 2D and reconstruction of modified faces
  //----------------------------------------------------
  MakeLoops (Modif);
  //-----------------------------------------------------
  // Reconstruction of non modified faces sharing 
  // reconstructed edges
  //------------------------------------------------------
  if (!Modif.IsEmpty()) MakeFaces (Modif);

  if (myThickening)
    MakeMissingWalls();

  //-------------------------
  // Construction of shells.
  //-------------------------
  MakeShells ();
  if (myOffsetShape.IsNull()) {
    // not done
    myDone = Standard_False;
    return;
  }
  //--------------
  // Unwinding 3D.
  //--------------
  SelectShells ();
  //----------------------------------
  // Remove INTERNAL edges if necessary
  //----------------------------------
  if (myRemoveIntEdges) {
    RemoveInternalEdges();
  }
  //----------------------------------
  // Coding of regularities.
  //----------------------------------
  EncodeRegularity();
  //----------------------
  // Creation of solids.
  //----------------------
  MakeSolid ();

  //-----------------------------
  // MAJ Tolerance edge and Vertex
  // ----------------------------
  if (!myOffsetShape.IsNull()) {
    UpdateTolerance (myOffsetShape,myFaces);
    BRepLib::UpdateTolerances( myOffsetShape );
  }

  CorrectConicalFaces();

  myDone = Standard_True;
}

//=======================================================================
//function : MakeThickSolid
//purpose  : 
//=======================================================================
void BRepOffset_MakeOffset::MakeThickSolid() 
{
  //--------------------------------------------------------------
  // Construction of shell parallel to shell (initial without cap).
  //--------------------------------------------------------------
  MakeOffsetShape ();

  //--------------------------------------------------------------------
  // Construction of a solid with the initial shell, parallel shell 
  // limited by caps.
  //--------------------------------------------------------------------
  if (!myFaces.IsEmpty()) {
    TopoDS_Solid    Res;
    TopExp_Explorer exp;
    BRep_Builder    B;
    Standard_Integer NbF = myFaces.Extent();

    B.MakeSolid(Res);

    BRepTools_Quilt Glue;
    for (exp.Init(myShape,TopAbs_FACE); exp.More(); exp.Next()) {
      NbF++;
      Glue.Add (exp.Current());
    } 
    Standard_Boolean YaResult = 0;
    if (!myOffsetShape.IsNull())
      {
      for (exp.Init(myOffsetShape,TopAbs_FACE);exp.More(); exp.Next())
        {
        YaResult = 1;
        Glue.Add (exp.Current().Reversed());
        }
#ifdef OCCT_DEBUG
      if(YaResult == 0)
        {
        cout << "OffsetShape does not contain a FACES." << endl;
        }
#endif
      }
#ifdef OCCT_DEBUG
    else
      {
      cout << "OffsetShape is null!" << endl;
      }
#endif

    if (YaResult == 0)
      {
      myDone = Standard_False;
      return;
      }

    myOffsetShape = Glue.Shells();
    for (exp.Init(myOffsetShape,TopAbs_SHELL); exp.More(); exp.Next()) {
      B.Add(Res,exp.Current());
    }
    Res.Closed(Standard_True);
    myOffsetShape = Res;

    // Test of Validity of the result of thick Solid 
    // more face than the initial solid.
        
    Standard_Integer NbOF = 0;
    for (exp.Init(myOffsetShape,TopAbs_FACE);exp.More(); exp.Next()) {
      NbOF++;
    }
    if (NbOF <= NbF) {
      myDone = Standard_False;
      return;
    }
  }

  if (myOffset > 0 ) myOffsetShape.Reverse();  

  myDone = Standard_True;
}

//=======================================================================
//function : IsDone
//purpose  : 
//=======================================================================
Standard_Boolean BRepOffset_MakeOffset::IsDone() const
{
  return myDone;
}

//=======================================================================
//function : Error
//purpose  : 
//=======================================================================
BRepOffset_Error BRepOffset_MakeOffset::Error() const
{
  return myError;
}

//=======================================================================
//function : Shape
//purpose  : 
//=======================================================================
const TopoDS_Shape&  BRepOffset_MakeOffset::Shape() const 
{
  return myOffsetShape;
}

//=======================================================================
//function : TrimEdge
//purpose  : Trim the edge of the largest of descendants in AsDes2d.
//           Order in AsDes two vertices that have trimmed the edge.
//=======================================================================
static 
  Standard_Boolean TrimEdge(TopoDS_Edge&                  NE,
                            const Handle(BRepAlgo_AsDes)& AsDes2d,
                            Handle(BRepAlgo_AsDes)& AsDes,
                            TopTools_DataMapOfShapeShape& theETrimEInf)
{
  TopoDS_Edge aSourceEdge;
  TopoDS_Vertex V1,V2;
  Standard_Real aT1, aT2;
  //
  TopExp::Vertices(NE, V1, V2);
  BRep_Tool::Range(NE, aT1, aT2);
  //
  BOPTools_AlgoTools::MakeSplitEdge(NE, V1, aT1, V2, aT2, aSourceEdge);
  //
  //
  Standard_Real aSameParTol = Precision::Confusion();
  
  Standard_Real U = 0.;
  Standard_Real UMin =  Precision::Infinite();
  Standard_Real UMax = -UMin;

  const TopTools_ListOfShape& LE = AsDes2d->Descendant(NE);
  //
  Standard_Boolean bTrim = Standard_False;
  //
  if (LE.Extent() > 1) {
    TopTools_ListIteratorOfListOfShape it (LE);
    for (; it.More(); it.Next()) {
      TopoDS_Vertex V = TopoDS::Vertex(it.Value());
      if (NE.Orientation() == TopAbs_REVERSED)
        V.Reverse();
      //V.Orientation(TopAbs_INTERNAL);
      if (!FindParameter(V, NE, U)) {
        Standard_Real f, l;
        Handle(Geom_Curve) theCurve = BRep_Tool::Curve(NE, f, l);
        gp_Pnt thePoint = BRep_Tool::Pnt(V);
        GeomAPI_ProjectPointOnCurve Projector(thePoint, theCurve);
        if (Projector.NbPoints() == 0)
          Standard_ConstructionError::Raise("BRepOffset_MakeOffset::TrimEdge no projection");
        U = Projector.LowerDistanceParameter();
      }
      if (U < UMin) {
        UMin = U; V1   = V;
      }
      if (U > UMax) {
        UMax = U; V2   = V;
      }
    }
    //
    if (V1.IsNull() || V2.IsNull()) {
      Standard_ConstructionError::Raise("BRepOffset_MakeOffset::TrimEdge");
    }
    if (!V1.IsSame(V2)) {
      NE.Free( Standard_True );
      BRep_Builder B;
      TopAbs_Orientation Or = NE.Orientation();
      NE.Orientation(TopAbs_FORWARD);
      TopoDS_Vertex VF,VL;
      TopExp::Vertices (NE,VF,VL);
      B.Remove(NE,VF);
      B.Remove(NE,VL);
      B.Add  (NE,V1.Oriented(TopAbs_FORWARD));
      B.Add  (NE,V2.Oriented(TopAbs_REVERSED));
      B.Range(NE,UMin,UMax);
      NE.Orientation(Or);
      AsDes->Add(NE,V1.Oriented(TopAbs_FORWARD));
      AsDes->Add(NE,V2.Oriented(TopAbs_REVERSED));
      BRepLib::SameParameter(NE, aSameParTol, Standard_True);
      //
      bTrim = Standard_True;
    }
  }
  //
  if (!bTrim) {
    BRepAdaptor_Curve aBAC(NE);
    if (aBAC.GetType() == GeomAbs_Line) {
      if (AsDes->HasAscendant(NE)) {
        AsDes->Remove(NE);
      }
    }
  }
  else
  {
    if (!theETrimEInf.IsBound(NE)) {
      theETrimEInf.Bind(NE, aSourceEdge);
    }
  }
  return bTrim;
}

//=======================================================================
//function : MakeOffsetFaces
//purpose  : 
//=======================================================================
void BRepOffset_MakeOffset::MakeOffsetFaces(BRepOffset_DataMapOfShapeOffset& theMapSF)
{
  Standard_Real aCurOffset;
  TopTools_ListOfShape aLF;
  TopTools_DataMapOfShapeShape ShapeTgt;
  TopTools_ListIteratorOfListOfShape aItLF;
  //
  Standard_Boolean OffsetOutside = (myOffset > 0.);
  //
  BRepLib::SortFaces(myShape, aLF);
  //
  aItLF.Initialize(aLF);
  for (; aItLF.More(); aItLF.Next()) {
    const TopoDS_Face& aF = TopoDS::Face(aItLF.Value());
    aCurOffset = myFaceOffset.IsBound(aF) ? myFaceOffset(aF) : myOffset;
    BRepOffset_Offset OF(aF, aCurOffset, ShapeTgt, OffsetOutside, myJoin);
    TopTools_ListOfShape Let;
    myAnalyse.Edges(aF,BRepOffset_Tangent,Let);
    TopTools_ListIteratorOfListOfShape itl(Let);    
    for (; itl.More(); itl.Next()) {
      const TopoDS_Edge& Cur = TopoDS::Edge(itl.Value());
      if ( !ShapeTgt.IsBound(Cur)) {
        TopoDS_Shape aLocalShape = OF.Generated(Cur);
        const TopoDS_Edge& OTE = TopoDS::Edge(aLocalShape);
        ShapeTgt.Bind(Cur,OF.Generated(Cur));
        TopoDS_Vertex V1,V2,OV1,OV2;
        TopExp::Vertices (Cur,V1,V2);
        TopExp::Vertices (OTE,OV1,OV2);      
        TopTools_ListOfShape LE;
        if (!ShapeTgt.IsBound(V1)) {
          myAnalyse.Edges(V1,BRepOffset_Tangent,LE);
          const TopTools_ListOfShape& LA =myAnalyse.Ancestors(V1);
          if (LE.Extent() == LA.Extent())
            ShapeTgt.Bind(V1,OV1);
        }
        if (!ShapeTgt.IsBound(V2)) {
          LE.Clear();
          myAnalyse.Edges(V2,BRepOffset_Tangent,LE);
          const TopTools_ListOfShape& LA =myAnalyse.Ancestors(V2);
          if (LE.Extent() == LA.Extent())
            ShapeTgt.Bind(V2,OV2);
        }
      }
    }
    theMapSF.Bind(aF,OF);
  }
}

//=======================================================================
//function : BuildOffsetByInter
//purpose  : 
//=======================================================================
void BRepOffset_MakeOffset::BuildOffsetByInter()
{
#ifdef OCCT_DEBUG
  if ( ChronBuild) {
    cout << " CONSTRUCTION OF OFFSETS :" << endl;
    Clock.Reset();
    Clock.Start();
  }
#endif

  TopExp_Explorer Exp, Exp2, ExpC;
  TopTools_ListIteratorOfListOfShape itLF;
  
  //--------------------------------------------------------
  // Construction of faces parallel to initial faces
  //--------------------------------------------------------
  BRepOffset_DataMapOfShapeOffset MapSF;
  MakeOffsetFaces(MapSF);
  //--------------------------------------------------------------------
  // MES   : Map of OffsetShape -> Extended Shapes.
  // Build : Map of Initial SS  -> OffsetShape build by Inter.
  //                               can be an edge or a compound of edges       
  //---------------------------------------------------------------------
  TopTools_DataMapOfShapeShape MES;  
  TopTools_DataMapOfShapeShape Build; 
  TopTools_ListOfShape         Failed;
  TopAbs_State                 Side = TopAbs_IN;  
  Handle(BRepAlgo_AsDes)       AsDes = new BRepAlgo_AsDes();

  //-------------------------------------------------------------------
  // Extension of faces and calculation of new edges of intersection.
  //-------------------------------------------------------------------
  Standard_Boolean  ExtentContext = 0;
  if (myOffset > 0) ExtentContext = 1;

  BRepOffset_Inter3d Inter3 (AsDes,Side,myTol);
  // Intersection between parallel faces
  Inter3.ConnexIntByInt(myShape,MapSF,myAnalyse,MES,Build,Failed,myIsPlanar);
  // Intersection with caps.
  Inter3.ContextIntByInt(myFaces,ExtentContext,MapSF,myAnalyse,MES,Build,Failed,myIsPlanar);


  //---------------------------------------------------------------------------------
  // Extension of neighbor edges of new edges and intersection between neighbors.
  //--------------------------------------------------------------------------------
  Handle(BRepAlgo_AsDes) AsDes2d = new BRepAlgo_AsDes();
  IntersectEdges(myShape, MapSF, MES, Build, AsDes, AsDes2d);
  //-----------------------------------------------------------
  // Great restriction of new edges and update of AsDes.
  //------------------------------------------ ----------------
  TopTools_DataMapOfShapeListOfShape anEdgesOrigins; // offset edge - initial edges
  TopTools_IndexedMapOfShape NewEdges;
  TopTools_DataMapOfShapeShape aETrimEInf; // trimmed - not trimmed edges
  //
  //Map of edges obtained after FACE-FACE (offsetted) intersection.
  //Key1 is edge trimmed by intersection points with other edges;
  //Item is not-trimmed edge. 
  TrimEdges(myShape, myOffset, MapSF, MES, Build, AsDes, AsDes2d, NewEdges, aETrimEInf, anEdgesOrigins);
  //
  //--------------------------------- 
  // Intersection 2D on //
  //---------------------------------  
  TopTools_IndexedDataMapOfShapeListOfShape aDMVV;
  TopTools_DataMapOfShapeShape aFacesOrigins; // offset face - initial face
  TopTools_ListOfShape LFE; 
  BRepAlgo_Image     IMOE;
  GetEnlargedFaces(myShape, MapSF, MES, aFacesOrigins, IMOE, LFE);
  //
  TopTools_ListIteratorOfListOfShape itLFE(LFE);
  for (; itLFE.More(); itLFE.Next())
  {
    const TopoDS_Face& NEF = TopoDS::Face(itLFE.Value());
    Standard_Real aCurrFaceTol = BRep_Tool::Tolerance(NEF);
    BRepOffset_Inter2d::Compute(AsDes, NEF, NewEdges, aCurrFaceTol, aDMVV);
  }
  //----------------------------------------------
  // Intersections 2d on caps.
  //----------------------------------------------
  Standard_Integer i;
  for (i = 1; i <= myFaces.Extent(); i++)
  {
    const TopoDS_Face& Cork = TopoDS::Face(myFaces(i));
    Standard_Real aCurrFaceTol = BRep_Tool::Tolerance(Cork);
    BRepOffset_Inter2d::Compute(AsDes, Cork, NewEdges, aCurrFaceTol, aDMVV);
  }
  //
  //
  BRepOffset_Inter2d::FuseVertices(aDMVV, AsDes);
  //-------------------------------
  // Unwinding of extended Faces.
  //-------------------------------
  //
  TopTools_MapOfShape aMFDone;
  //
  if ((myJoin == GeomAbs_Intersection) && myInter && myIsPlanar) {
    BuildSplitsOfFaces(LFE, AsDes, anEdgesOrigins, aFacesOrigins, IMOE, aETrimEInf);
    //
    TopTools_ListIteratorOfListOfShape aItLF(LFE);
    for (; aItLF.More(); aItLF.Next()) {
      const TopoDS_Shape& aS = aItLF.Value();
      aMFDone.Add(aS);
    }
  }
  else {
    myMakeLoops.Build(LFE, AsDes, IMOE);
  }
  //
#ifdef OCCT_DEBUG
  TopTools_IndexedMapOfShape COES;
#endif
  //---------------------------
  // MAJ SD. for faces //
  //---------------------------
  for (Exp.Init(myShape,TopAbs_FACE) ; Exp.More(); Exp.Next()) {
    const TopoDS_Shape& FI   = Exp.Current();
    myInitOffsetFace.SetRoot(FI);
    TopoDS_Face  OF  = MapSF(FI).Face();
    if (MES.IsBound(OF)) {
      OF = TopoDS::Face(MES(OF));
      if (IMOE.HasImage(OF)) {
        const TopTools_ListOfShape& LOFE = IMOE.Image(OF);
        myInitOffsetFace.Bind(FI,LOFE);
        for (itLF.Initialize(LOFE); itLF.More(); itLF.Next()) {
          const TopoDS_Shape& OFE =  itLF.Value();
          myImageOffset.SetRoot(OFE);
#ifdef DRAW
          if (AffichInt2d) {
            sprintf(name,"AF_%d",NbAF++);
            DBRep::Set(name,OFE);
          }
#endif
          TopTools_MapOfShape View;
          for (Exp2.Init(OFE.Oriented(TopAbs_FORWARD),TopAbs_EDGE); 
               Exp2.More(); Exp2.Next()) {
            const TopoDS_Edge& COE = TopoDS::Edge(Exp2.Current());
            
            myAsDes->Add (OFE,COE);
#ifdef DRAW
            if (AffichInt2d) {
              sprintf(name,"AE_%d",NbAE++);
              DBRep::Set(name,COE);
              COES.Add(COE);
            }
#endif
            if (View.Add(COE)){
              if (!myAsDes->HasDescendant(COE)) {
                TopoDS_Vertex CV1,CV2;
                TopExp::Vertices(COE,CV1,CV2);
                if (!CV1.IsNull()) myAsDes->Add(COE,CV1.Oriented(TopAbs_FORWARD));
                if (!CV2.IsNull()) myAsDes->Add(COE,CV2.Oriented(TopAbs_REVERSED));        
              }
            }
          }
        }
      }
      else {
        if (aMFDone.Contains(OF)) {
          continue;
        }
        //
        myInitOffsetFace.Bind(FI,OF);
        myImageOffset.SetRoot(OF);
#ifdef DRAW 
        if (AffichInt2d) {
          sprintf(name,"AF_%d",NbAF++);
          DBRep::Set(name,OF);
        }
#endif
        const TopTools_ListOfShape& LE = AsDes->Descendant(OF);
        for (itLF.Initialize(LE) ; itLF.More(); itLF.Next()) {
          const TopoDS_Edge& OE = TopoDS::Edge(itLF.Value());
          if (IMOE.HasImage(OE)) {
            const TopTools_ListOfShape& LOE = IMOE.Image(OE);
            TopTools_ListIteratorOfListOfShape itLOE(LOE);
            for (; itLOE.More(); itLOE.Next()) {
              TopoDS_Shape aLocalShape = itLOE.Value().Oriented(OE.Orientation());
              const TopoDS_Edge& COE = TopoDS::Edge(aLocalShape);
//              const TopoDS_Edge& COE = TopoDS::Edge(itLOE.Value().Oriented(OE.Orientation()));
              myAsDes->Add(OF,COE);
#ifdef DRAW
              if (AffichInt2d) {
                sprintf(name,"AE_%d",NbAE++);
                DBRep::Set(name,COE);
                COES.Add(COE);
              }
#endif
              
              if (!myAsDes->HasDescendant(COE)) {
                TopoDS_Vertex CV1,CV2;
                TopExp::Vertices(COE,CV1,CV2);
                 if (!CV1.IsNull()) myAsDes->Add(COE,CV1.Oriented(TopAbs_FORWARD));
                if (!CV2.IsNull()) myAsDes->Add(COE,CV2.Oriented(TopAbs_REVERSED));        
              }
            }
          }
          else {
            myAsDes->Add(OF,OE);
#ifdef DRAW
            if (AffichInt2d) {
              sprintf(name,"AE_%d",NbAE++);
              DBRep::Set(name,OE);
              COES.Add(OE);
            }
#endif

            const TopTools_ListOfShape& LV = AsDes->Descendant(OE);
            myAsDes->Add(OE,LV);
          }
        }
      }
    }
    else {
      myInitOffsetFace.Bind(FI,OF);
      myImageOffset.SetRoot(OF);
      TopTools_MapOfShape View;
      for (Exp2.Init(OF.Oriented(TopAbs_FORWARD),TopAbs_EDGE); 
           Exp2.More(); Exp2.Next()) {

        const TopoDS_Edge& COE = TopoDS::Edge(Exp2.Current());
        myAsDes->Add (OF,COE);
#ifdef DRAW
        if (AffichInt2d) {
          sprintf(name,"AE_%d",NbAE++);
          DBRep::Set(name,COE);
          COES.Add(COE);
        }
#endif
        
        if (View.Add(Exp2.Current())) {
          if (!myAsDes->HasDescendant(COE)) {
            TopoDS_Vertex CV1,CV2;
            TopExp::Vertices(COE,CV1,CV2);
            if (!CV1.IsNull()) myAsDes->Add(COE,CV1.Oriented(TopAbs_FORWARD));
            if (!CV2.IsNull()) myAsDes->Add(COE,CV2.Oriented(TopAbs_REVERSED));        
          }
        }
      } 
    }
  }
  //  Modified by skv - Tue Mar 15 16:20:43 2005
  // Add methods for supporting history.
  TopTools_MapOfShape aMapEdges;

  for (Exp.Init(myShape,TopAbs_FACE) ; Exp.More(); Exp.Next()) {
    const TopoDS_Shape& aFaceRef = Exp.Current();

    Exp2.Init(aFaceRef.Oriented(TopAbs_FORWARD), TopAbs_EDGE);

    for (; Exp2.More(); Exp2.Next()) {
      const TopoDS_Shape& anEdgeRef = Exp2.Current();

      if (aMapEdges.Add(anEdgeRef)) {
        myInitOffsetEdge.SetRoot(anEdgeRef);
        if (Build.IsBound(anEdgeRef)) {
          TopoDS_Shape aNewShape = Build(anEdgeRef);
          
          if (aNewShape.ShapeType() == TopAbs_EDGE) {
            if (IMOE.HasImage(aNewShape)) {
              const TopTools_ListOfShape& aListNewE = IMOE.Image(aNewShape);
              
              myInitOffsetEdge.Bind (anEdgeRef, aListNewE);
            } else
              myInitOffsetEdge.Bind (anEdgeRef, aNewShape);
          } else { // aNewShape != TopAbs_EDGE
            TopTools_ListOfShape aListNewEdge;
            
            for (ExpC.Init(aNewShape, TopAbs_EDGE); ExpC.More(); ExpC.Next()) {
              const TopoDS_Shape &aResEdge = ExpC.Current();
              
              if (IMOE.HasImage(aResEdge)) {
                const TopTools_ListOfShape& aListNewE = IMOE.Image(aResEdge);
                TopTools_ListIteratorOfListOfShape aNewEIter(aListNewE);
                
                for (; aNewEIter.More(); aNewEIter.Next())
                  aListNewEdge.Append(aNewEIter.Value());
              } else
                aListNewEdge.Append(aResEdge);
            }
            
            myInitOffsetEdge.Bind (anEdgeRef, aListNewEdge);
          }
        } 
        else { // Free boundary.
          TopoDS_Shape aNewEdge = MapSF(aFaceRef).Generated(anEdgeRef);

          if (MES.IsBound(aNewEdge))
            aNewEdge = MES(aNewEdge);

          if (IMOE.HasImage(aNewEdge)) {
            const TopTools_ListOfShape& aListNewE = IMOE.Image(aNewEdge);

            myInitOffsetEdge.Bind (anEdgeRef, aListNewE);
          } else
            myInitOffsetEdge.Bind (anEdgeRef, aNewEdge);
        }
      }
    }
  }
//  Modified by skv - Tue Mar 15 16:20:43 2005

  //---------------------------
  // MAJ SD. for caps 
  //---------------------------
  //TopTools_MapOfShape View; 
  for (i = 1; i <= myFaces.Extent(); i++) {
    const TopoDS_Shape& Cork = myFaces(i);
    const TopTools_ListOfShape& LE = AsDes->Descendant(Cork);
    for (itLF.Initialize(LE) ; itLF.More(); itLF.Next()) {
      const TopoDS_Edge& OE = TopoDS::Edge(itLF.Value());
      if (IMOE.HasImage(OE)) {
        const TopTools_ListOfShape& LOE = IMOE.Image(OE);
          TopTools_ListIteratorOfListOfShape itLOE(LOE);
        for (; itLOE.More(); itLOE.Next()) {
          const TopoDS_Edge& COE = TopoDS::Edge(itLOE.Value());
          myAsDes->Add(Cork,COE.Oriented(OE.Orientation())) ;
#ifdef DRAW
          if (AffichInt2d) {
            sprintf(name,"AE_%d",NbAE++);
            DBRep::Set(name,COE);
            COES.Add(COE);
          }
#endif
          
          if (!myAsDes->HasDescendant(COE)) {
            TopoDS_Vertex CV1,CV2;
            TopExp::Vertices(COE,CV1,CV2);
            if (!CV1.IsNull()) myAsDes->Add(COE,CV1.Oriented(TopAbs_FORWARD));
            if (!CV2.IsNull()) myAsDes->Add(COE,CV2.Oriented(TopAbs_REVERSED));          
          }
        }
      }
      else {
        myAsDes->Add(Cork,OE);
        if (AsDes->HasDescendant(OE)) {
          myAsDes->Add(OE,AsDes->Descendant(OE));
        }
#ifdef DRAW
        if (AffichInt2d) {
          sprintf(name,"AE_%d",NbAE++);
          DBRep::Set(name,OE);
          COES.Add(OE);
        }
#endif
      }
    }
  }
  
#ifdef OCCT_DEBUG
  DEBVerticesControl (COES,myAsDes);
  if ( ChronBuild) Clock.Show();
#endif
}

//=======================================================================
//function : BuildOffsetByArc
//purpose  : 
//=======================================================================
void BRepOffset_MakeOffset::BuildOffsetByArc()
{
#ifdef OCCT_DEBUG
  if ( ChronBuild) {
    cout << " CONSTRUCTION OF OFFSETS :" << endl;
    Clock.Reset();
    Clock.Start();
  }
#endif

  TopExp_Explorer Exp;
  TopTools_ListIteratorOfListOfShape itLF;
  TopTools_MapOfShape Done;
  
  //--------------------------------------------------------
  // Construction of faces parallel to initial faces
  //--------------------------------------------------------
  BRepOffset_DataMapOfShapeOffset MapSF;
  MakeOffsetFaces(MapSF);
  //--------------------------------------------------------
  // Construction of tubes on edge.
  //--------------------------------------------------------
  BRepOffset_Type    OT = BRepOffset_Convex;
  if (myOffset < 0.) OT = BRepOffset_Concave; 
   
  for (Exp.Init(myShape,TopAbs_EDGE); Exp.More(); Exp.Next()) {
    const TopoDS_Edge& E = TopoDS::Edge(Exp.Current());
    if (Done.Add(E)) {
      const TopTools_ListOfShape& Anc = myAnalyse.Ancestors(E);
      if (Anc.Extent() == 2) {
        const BRepOffset_ListOfInterval& L = myAnalyse.Type(E);
        if (!L.IsEmpty() && L.First().Type() == OT) {
          Standard_Real CurOffset = myOffset;
          if ( myFaceOffset.IsBound(Anc.First()))
            CurOffset = myFaceOffset(Anc.First());
          TopoDS_Shape aLocalShapeGen = MapSF(Anc.First()).Generated(E);
          TopoDS_Edge EOn1 = TopoDS::Edge(aLocalShapeGen);
          aLocalShapeGen = MapSF(Anc.Last()).Generated(E);
          TopoDS_Edge EOn2 = TopoDS::Edge(aLocalShapeGen);
//          TopoDS_Edge EOn1 = TopoDS::Edge(MapSF(Anc.First()).Generated(E));
//          TopoDS_Edge EOn2 = TopoDS::Edge(MapSF(Anc.Last()) .Generated(E));
          // find if exits tangent edges in the original shape
          TopoDS_Edge E1f, E1l;
          TopoDS_Vertex V1f, V1l;
          TopExp::Vertices(E,V1f,V1l);
          TopTools_ListOfShape TangE;
          myAnalyse.TangentEdges(E,V1f,TangE);
          // find if the pipe on the tangent edges are soon created.
          TopTools_ListIteratorOfListOfShape itl(TangE);
          Standard_Boolean Find = Standard_False;
          for ( ; itl.More() && !Find; itl.Next()) {
            if ( MapSF.IsBound(itl.Value())) {
              TopoDS_Shape aLocalShape = MapSF(itl.Value()).Generated(V1f);
              E1f  = TopoDS::Edge(aLocalShape);
//              E1f  = TopoDS::Edge(MapSF(itl.Value()).Generated(V1f));
              Find = Standard_True;
            }
          }
          TangE.Clear();
          myAnalyse.TangentEdges(E,V1l,TangE);
          // find if the pipe on the tangent edges are soon created.
          itl.Initialize(TangE);
          Find = Standard_False;
          for ( ; itl.More() && !Find; itl.Next()) {
            if ( MapSF.IsBound(itl.Value())) {
              TopoDS_Shape aLocalShape = MapSF(itl.Value()).Generated(V1l);
              E1l  = TopoDS::Edge(aLocalShape);
//              E1l  = TopoDS::Edge(MapSF(itl.Value()).Generated(V1l));
              Find = Standard_True;
            }
          }
          BRepOffset_Offset OF (E,EOn1,EOn2,CurOffset,E1f, E1l);
          MapSF.Bind(E,OF);
        }
      }
      else {
        // ----------------------
        // free border.
        // ----------------------
        TopoDS_Shape aLocalShape = MapSF(Anc.First()).Generated(E);
        TopoDS_Edge EOn1 = TopoDS::Edge(aLocalShape);
///        TopoDS_Edge EOn1 = TopoDS::Edge(MapSF(Anc.First()).Generated(E));
        myInitOffsetEdge.SetRoot(E); // skv: supporting history.
        myInitOffsetEdge.Bind (E,EOn1);      
      }
    }
  }

  //--------------------------------------------------------
  // Construction of spheres on vertex.
  //--------------------------------------------------------
  Done.Clear();
  TopTools_ListIteratorOfListOfShape it;

  for (Exp.Init(myShape,TopAbs_VERTEX); Exp.More(); Exp.Next()) {
    const TopoDS_Vertex& V = TopoDS::Vertex (Exp.Current());
    if (Done.Add(V)) {
      const TopTools_ListOfShape& LA = myAnalyse.Ancestors(V);
      TopTools_ListOfShape LE;
      myAnalyse.Edges(V,OT,LE);

      if (LE.Extent() >= 3 && LE.Extent() == LA.Extent()) {
        TopTools_ListOfShape LOE;
        //--------------------------------------------------------
        // Return connected edges on tubes.
        //--------------------------------------------------------
        for (it.Initialize(LE) ; it.More(); it.Next()) {
          LOE.Append(MapSF(it.Value()).Generated(V).Reversed());
        }
        //----------------------
        // construction sphere.
        //-----------------------
        const TopTools_ListOfShape& LLA = myAnalyse.Ancestors(LA.First());
        const TopoDS_Shape& FF = LLA.First();
        Standard_Real CurOffset = myOffset;
        if ( myFaceOffset.IsBound(FF))
          CurOffset = myFaceOffset(FF);
        
        BRepOffset_Offset OF(V,LOE,CurOffset);
        MapSF.Bind(V,OF);
      }
      //--------------------------------------------------------------
      // Particular processing if V is at least a free border.
      //-------------------------------------------------------------
      TopTools_ListOfShape LBF;
      myAnalyse.Edges(V,BRepOffset_FreeBoundary,LBF);
      if (!LBF.IsEmpty()) {        
        Standard_Boolean First = Standard_True;
        for (it.Initialize(LE) ; it.More(); it.Next()) {
          if (First) {
            myInitOffsetEdge.SetRoot(V); // skv: supporting history.
            myInitOffsetEdge.Bind(V,MapSF(it.Value()).Generated(V));
            First = Standard_False;
          }
          else {
            myInitOffsetEdge.Add(V,MapSF(it.Value()).Generated(V));
          }
        } 
      }
    }
  }

  //------------------------------------------------------------
  // Extension of parallel faces to the context.
  // Extended faces are ordered in DS and removed from MapSF.
  //------------------------------------------------------------
  if (!myFaces.IsEmpty()) ToContext (MapSF);

  //------------------------------------------------------
  // MAJ SD.
  //------------------------------------------------------
  BRepOffset_Type    RT = BRepOffset_Concave;
  if (myOffset < 0.) RT = BRepOffset_Convex;
  BRepOffset_DataMapIteratorOfDataMapOfShapeOffset It(MapSF);
  for ( ; It.More(); It.Next()) {
    const TopoDS_Shape& SI = It.Key(); 
    const BRepOffset_Offset& SF = It.Value();
    if (SF.Status() == BRepOffset_Reversed ||
        SF.Status() == BRepOffset_Degenerated ) {
      //------------------------------------------------
      // Degenerated or returned faces are not stored.
      //------------------------------------------------
      continue; 
    }        

    const TopoDS_Face&  OF = It.Value().Face();
    myInitOffsetFace.Bind    (SI,OF);      
    myInitOffsetFace.SetRoot (SI);      // Initial<-> Offset
    myImageOffset.SetRoot    (OF);      // FaceOffset root of images
    
    if (SI.ShapeType() == TopAbs_FACE) {
      for (Exp.Init(SI.Oriented(TopAbs_FORWARD),TopAbs_EDGE); 
           Exp.More(); Exp.Next()) {
        //--------------------------------------------------------------------
        // To each face are associatedthe edges that restrict that 
        // The edges that do not generate tubes or are not tangent
        // to two faces are removed.
        //--------------------------------------------------------------------
        const TopoDS_Edge& E = TopoDS::Edge(Exp.Current());
        const BRepOffset_ListOfInterval& L  = myAnalyse.Type(E);
        if (!L.IsEmpty() && L.First().Type() != RT) {
          TopAbs_Orientation OO  = E.Orientation();
          TopoDS_Shape aLocalShape = It.Value().Generated(E);
          TopoDS_Edge        OE  = TopoDS::Edge(aLocalShape);
//          TopoDS_Edge        OE  = TopoDS::Edge(It.Value().Generated(E));
          myAsDes->Add (OF,OE.Oriented(OO));
        }
      }
    }
    else {
      for (Exp.Init(OF.Oriented(TopAbs_FORWARD),TopAbs_EDGE); 
           Exp.More(); Exp.Next()) {
        myAsDes->Add (OF,Exp.Current());
      }
    }
  }

#ifdef OCCT_DEBUG
  if ( ChronBuild) Clock.Show();
#endif
}

//=======================================================================
//function : SelfInter
//purpose  : 
//=======================================================================
void BRepOffset_MakeOffset::SelfInter(TopTools_MapOfShape& /*Modif*/)
{
#ifdef OCCT_DEBUG
  if ( ChronBuild) {
    cout << " AUTODEBOUCLAGE:" << endl;
    Clock.Reset();
    Clock.Start();
  }    
#endif  

  Standard_NotImplemented::Raise();

#ifdef OCCT_DEBUG
  if ( ChronBuild) Clock.Show();
#endif
}

//=======================================================================
//function : ToContext
//purpose  : 
//=======================================================================
void BRepOffset_MakeOffset::ToContext (BRepOffset_DataMapOfShapeOffset& MapSF)
{
  TopTools_DataMapOfShapeShape        Created;   
  TopTools_DataMapOfShapeShape        MEF;
  TopTools_IndexedMapOfShape          FacesToBuild;
  TopTools_ListIteratorOfListOfShape  itl;
  TopExp_Explorer                     exp;

//  TopAbs_State       Side = TopAbs_IN;  
//  if (myOffset < 0.) Side = TopAbs_OUT;

  TopAbs_State       Side = TopAbs_OUT; 

  /*
  Standard_Integer i;
  for (i = 1; i <= myFaces.Extent(); i++) {
    const TopoDS_Face& CF = TopoDS::Face(myFaces(i));
    for (exp.Init(CF.Oriented(TopAbs_FORWARD),TopAbs_EDGE); 
         exp.More(); exp.Next()) {
      const TopoDS_Edge& E = TopoDS::Edge(exp.Current());
      if (!myAnalyse.HasAncestor(E)) {
        //----------------------------------------------------------------
        // The edges of context faces that are not in the initial shape
        // can appear in the result.
        //----------------------------------------------------------------
        //myAsDes->Add(CF,E);
      }  
    }
  }
  */
  
  //--------------------------------------------------------
  // Determine the edges and faces reconstructed by  
  // intersection.
  //---------------------------------------------------------
  Standard_Integer j;
  for (j = 1; j <= myFaces.Extent(); j++) {
    const TopoDS_Face& CF = TopoDS::Face(myFaces(j));
    for (exp.Init(CF.Oriented(TopAbs_FORWARD),TopAbs_EDGE); 
         exp.More(); exp.Next()) {
      const TopoDS_Edge& E = TopoDS::Edge(exp.Current()); 
      if (myAnalyse.HasAncestor(E)) {
        const TopTools_ListOfShape& LEA = myAnalyse.Ancestors(E);
        for (itl.Initialize(LEA); itl.More(); itl.Next()) {
          const BRepOffset_Offset& OF = MapSF(itl.Value());
          FacesToBuild.Add(itl.Value());
          MEF.Bind(OF.Generated(E),CF);
        }
         TopoDS_Vertex V[2];
        TopExp::Vertices(E,V[0],V[1]);
        for (Standard_Integer i = 0; i < 2; i++) {
          const TopTools_ListOfShape& LVA =  myAnalyse.Ancestors(V[i]);
          for ( itl.Initialize(LVA); itl.More(); itl.Next()) {
            const TopoDS_Edge& EV = TopoDS::Edge(itl.Value());
            if (MapSF.IsBound(EV)) {
              const BRepOffset_Offset& OF = MapSF(EV);
              FacesToBuild.Add(EV);
              MEF.Bind(OF.Generated(V[i]),CF);
            }
          }
        }
      }
    }
  }
  //---------------------------
  // Reconstruction of faces.
  //---------------------------
  TopoDS_Face        F,NF;
  BRepOffset_Type    RT = BRepOffset_Concave;
  if (myOffset < 0.) RT = BRepOffset_Convex;
  TopoDS_Shape       OE,NE;
  TopAbs_Orientation Or;

  for (j = 1; j <= FacesToBuild.Extent(); j++) {
    const TopoDS_Shape& S   = FacesToBuild(j);
    BRepOffset_Offset   BOF;
    BOF = MapSF(S);
    F = TopoDS::Face(BOF.Face());
    BRepOffset_Tool::ExtentFace(F,Created,MEF,Side,myTol,NF);
    MapSF.UnBind(S);
    //--------------
    // MAJ SD.
    //--------------
    myInitOffsetFace.Bind    (S,NF);      
    myInitOffsetFace.SetRoot (S);      // Initial<-> Offset
    myImageOffset.SetRoot    (NF);

    if (S.ShapeType() == TopAbs_FACE) {
      for (exp.Init(S.Oriented(TopAbs_FORWARD),TopAbs_EDGE); 
           exp.More(); exp.Next()) {
        
        const TopoDS_Edge& E = TopoDS::Edge(exp.Current());
        const BRepOffset_ListOfInterval& L  = myAnalyse.Type(E);
        OE = BOF.Generated(E);
        Or = E.Orientation();
        OE.Orientation(Or);
        if (!L.IsEmpty() && L.First().Type() != RT) {
          if (Created.IsBound(OE)) {
            NE = Created(OE); 
            if (NE.Orientation() == TopAbs_REVERSED) 
              NE.Orientation(TopAbs::Reverse(Or));
            else
              NE.Orientation(Or);
            myAsDes->Add(NF,NE);
          }
          else {
            myAsDes->Add(NF,OE);
          }
        }
      }
    }
    else {
      //------------------
      // Tube
      //---------------------
      for (exp.Init(NF.Oriented(TopAbs_FORWARD),TopAbs_EDGE); 
           exp.More(); exp.Next()) {
        myAsDes->Add (NF,exp.Current());
      }
    }    
    MapSF.UnBind(S);
  }

  //------------------
  // MAJ free borders
  //------------------
  TopTools_DataMapIteratorOfDataMapOfShapeShape itc;
  for (itc.Initialize(Created); itc.More(); itc.Next()) {
    OE = itc.Key();
    NE = itc.Value();
    if (myInitOffsetEdge.IsImage(OE)) {
      TopoDS_Shape E = myInitOffsetEdge.ImageFrom (OE);
      Or = myInitOffsetEdge.Image(E).First().Orientation();
      if (NE.Orientation() == TopAbs_REVERSED) 
        NE.Orientation(TopAbs::Reverse(Or));
      else
        NE.Orientation(Or);
      myInitOffsetEdge.Remove(OE);
      myInitOffsetEdge.Bind(E,NE);
    }
  }
}

//=======================================================================
//function : UpdateFaceOffset
//purpose  : 
//=======================================================================
void BRepOffset_MakeOffset::UpdateFaceOffset()
{
  TopTools_MapOfShape M;
  TopTools_DataMapOfShapeReal CopiedMap;
  CopiedMap.Assign(myFaceOffset);
  TopTools_DataMapIteratorOfDataMapOfShapeReal it(CopiedMap);

  BRepOffset_Type    RT = BRepOffset_Convex;
  if (myOffset < 0.) RT = BRepOffset_Concave;

  for ( ; it.More(); it.Next()) {
    const TopoDS_Face& F = TopoDS::Face(it.Key());
    Standard_Real CurOffset = CopiedMap(F);
    if ( !M.Add(F)) continue;
    TopoDS_Compound Co;
    BRep_Builder Build;
    Build.MakeCompound(Co);
    TopTools_MapOfShape Dummy;
    Build.Add(Co,F);
    if (myJoin == GeomAbs_Arc)
      myAnalyse.AddFaces(F,Co,Dummy,BRepOffset_Tangent,RT);
    else   
      myAnalyse.AddFaces(F,Co,Dummy,BRepOffset_Tangent);

    TopExp_Explorer exp(Co,TopAbs_FACE);
    for (; exp.More(); exp.Next()) {
      const TopoDS_Face& FF = TopoDS::Face(exp.Current());
      if ( !M.Add(FF)) continue;
      if ( myFaceOffset.IsBound(FF))
        myFaceOffset.UnBind(FF);
      myFaceOffset.Bind(FF,CurOffset);
    }
  }
}

//=======================================================================
//function : CorrectConicalFaces
//purpose  : 
//=======================================================================
void BRepOffset_MakeOffset::CorrectConicalFaces()
{
  TopTools_SequenceOfShape Cones;
  TopTools_SequenceOfShape Circs;
  TopTools_SequenceOfShape Seams;
  Standard_Real TolApex = 1.e-5;

  Standard_Integer i;

  TopTools_DataMapOfShapeListOfShape FacesOfCone;
  //TopTools_DataMapOfShapeShape DegEdges;
  TopExp_Explorer Explo( myOffsetShape, TopAbs_FACE );
  if (myJoin == GeomAbs_Arc)
  {
    for (; Explo.More(); Explo.Next())
    {
      TopoDS_Face aFace = TopoDS::Face( Explo.Current() );
      Handle(Geom_Surface) aSurf = BRep_Tool::Surface( aFace );
      //if (aSurf->DynamicType() == STANDARD_TYPE(Geom_OffsetSurface))
      //aSurf = (Handle(Geom_OffsetSurface)::DownCast(aSurf))->BasisSurface(); //???
      
      TopTools_IndexedMapOfShape Emap;
      TopExp::MapShapes( aFace, TopAbs_EDGE, Emap );
      for (i = 1; i <= Emap.Extent(); i++)
      {
        TopoDS_Edge anEdge = TopoDS::Edge( Emap(i) );
        //Standard_Real f, l;
        //Handle(Geom_Curve) theCurve = BRep_Tool::Curve( anEdge, f, l );
        //Handle(BRep_TEdge)& TE = *((Handle(BRep_TEdge)*) &anEdge.TShape());
        if (BRep_Tool::Degenerated(anEdge))
        {
          //Check if anEdge is a really degenerated edge or not
          BRepAdaptor_Curve BACurve(anEdge, aFace);
          gp_Pnt Pfirst, Plast, Pmid;
          Pfirst = BACurve.Value(BACurve.FirstParameter());
          Plast  = BACurve.Value(BACurve.LastParameter());
          Pmid   = BACurve.Value((BACurve.FirstParameter()+BACurve.LastParameter())/2.);
          if (Pfirst.Distance(Plast) <= TolApex &&
              Pfirst.Distance(Pmid)  <= TolApex)
            continue;
          //Cones.Append( aFace );
          //Circs.Append( anEdge );
          //TopoDS_Vertex Vdeg = TopExp::FirstVertex( anEdge );
          TopoDS_Edge OrEdge = 
            TopoDS::Edge( myInitOffsetEdge.Root( anEdge) );
          TopoDS_Vertex VF = TopExp::FirstVertex( OrEdge );
          if ( FacesOfCone.IsBound(VF) )
          {
            //add a face to the existing list
            TopTools_ListOfShape& aFaces = FacesOfCone.ChangeFind(VF);
            aFaces.Append (aFace);
            //DegEdges.Bind(aFace, anEdge);
          }
          else
          {
            //the vertex is not in the map => create a new key and items
            TopTools_ListOfShape aFaces;
            aFaces.Append (aFace);
            FacesOfCone.Bind(VF, aFaces);
            //DegEdges.Bind(aFace, anEdge);
          }
        }
      } //for (i = 1; i <= Emap.Extent(); i++)
    } //for (; fexp.More(); fexp.Next())
  } //if (myJoin == GeomAbs_Arc)

  TopTools_DataMapIteratorOfDataMapOfShapeListOfShape Cone(FacesOfCone);
  BRep_Builder BB;
  TopLoc_Location L;
  for (; Cone.More(); Cone.Next() ) {
    gp_Sphere theSphere;
    Handle(Geom_SphericalSurface) aSphSurf;
    TopoDS_Wire SphereWire;
    BB.MakeWire(SphereWire);
    TopoDS_Vertex anApex = TopoDS::Vertex(Cone.Key());
    const TopTools_ListOfShape& Faces = Cone.Value(); //FacesOfCone(anApex);
    TopTools_ListIteratorOfListOfShape itFaces(Faces);
    Standard_Boolean isFirstFace = Standard_True;
    gp_Pnt FirstPoint;
    TopoDS_Vertex theFirstVertex, CurFirstVertex;
    for (; itFaces.More(); itFaces.Next())
    {
      TopoDS_Face aFace = TopoDS::Face(itFaces.Value()); //TopoDS::Face(Faces.First());
      TopoDS_Edge DegEdge; // = TopoDS::Edge(DegEdges(aFace));
      for (Explo.Init(aFace, TopAbs_EDGE); Explo.More(); Explo.Next())
      {
        DegEdge = TopoDS::Edge(Explo.Current());
        if (BRep_Tool::Degenerated(DegEdge))
        {
          TopoDS_Edge OrEdge = TopoDS::Edge( myInitOffsetEdge.Root( DegEdge) );
          TopoDS_Vertex VF = TopExp::FirstVertex( OrEdge );
          if (VF.IsSame(anApex))
            break;
        }
      }
      TopoDS_Shape aLocalDegShape = DegEdge.Oriented(TopAbs_FORWARD);
      TopoDS_Edge CurEdge = TopoDS::Edge(aLocalDegShape);
      BB.Degenerated(CurEdge, Standard_False);
      BB.SameRange(CurEdge, Standard_False);
      BB.SameParameter(CurEdge, Standard_False);
      gp_Pnt fPnt, lPnt, mPnt;
      GetEdgePoints(CurEdge, aFace, fPnt, mPnt, lPnt);
      Standard_Real f, l;
      BRep_Tool::Range(CurEdge, f, l);
      if (isFirstFace)
      {
        gp_Vec aVec1(fPnt, mPnt);
        gp_Vec aVec2(fPnt, lPnt);
        gp_Vec aNorm = aVec1.Crossed(aVec2);
        gp_Pnt theApex = BRep_Tool::Pnt(anApex);
        gp_Vec ApexToFpnt(theApex, fPnt);
        gp_Vec Ydir = aNorm ^ ApexToFpnt;
        gp_Vec Xdir = Ydir ^ aNorm;
        //Xdir.Rotate(gp_Ax1(theApex, aNorm), -f);
        gp_Ax2 anAx2(theApex, gp_Dir(aNorm), gp_Dir(Xdir));
        theSphere.SetRadius(myOffset);
        theSphere.SetPosition(gp_Ax3(anAx2) /*gp_Ax3(theApex, gp_Dir(aNorm))*/);
        aSphSurf = new Geom_SphericalSurface(theSphere);
        FirstPoint = fPnt;
        theFirstVertex = BRepLib_MakeVertex(fPnt);
        CurFirstVertex = theFirstVertex;
      }
      
      TopoDS_Vertex v1, v2, FirstVert, EndVert;
      TopExp::Vertices(CurEdge, v1, v2);
      FirstVert = CurFirstVertex;
      if (lPnt.Distance(FirstPoint) <= Precision::Confusion())
        EndVert = theFirstVertex;
      else
        EndVert = BRepLib_MakeVertex(lPnt);
      CurEdge.Free( Standard_True );
      BB.Remove(CurEdge, v1);
      BB.Remove(CurEdge, v2);
      BB.Add(CurEdge, FirstVert.Oriented(TopAbs_FORWARD));
      BB.Add(CurEdge, EndVert.Oriented(TopAbs_REVERSED));
      //take the curve from sphere an put it to the edge
      Standard_Real Uf, Vf, Ul, Vl;
      ElSLib::Parameters( theSphere, fPnt, Uf, Vf );
      ElSLib::Parameters( theSphere, lPnt, Ul, Vl );
      if (Abs(Ul) <= Precision::Confusion())
        Ul = 2.*M_PI;
      Handle(Geom_Curve) aCurv = aSphSurf->VIso(Vf);
      /*
        if (!isFirstFace)
        {
        gp_Circ aCircle = (Handle(Geom_Circle)::DownCast(aCurv))->Circ();
        if (Abs(Uf - f) > Precision::Confusion())
        {
        aCircle.Rotate(aCircle.Axis(), f - Uf);
        aCurv = new Geom_Circle(aCircle);
        }
        }
      */
      Handle(Geom_TrimmedCurve) aTrimCurv = new Geom_TrimmedCurve(aCurv, Uf, Ul);
      BB.UpdateEdge(CurEdge, aTrimCurv, Precision::Confusion());
      BB.Range(CurEdge, Uf, Ul, Standard_True);
      Handle(Geom2d_Line) theLin2d = new Geom2d_Line( gp_Pnt2d( 0., Vf ), gp::DX2d() );
      Handle(Geom2d_TrimmedCurve) theTrimLin2d = new Geom2d_TrimmedCurve(theLin2d, Uf, Ul);
      BB.UpdateEdge(CurEdge, theTrimLin2d, aSphSurf, L, Precision::Confusion());
      BB.Range(CurEdge, aSphSurf, L, Uf, Ul);
      BRepLib::SameParameter(CurEdge);
      BB.Add(SphereWire, CurEdge);
      //Modifying correspondent edges in aFace: substitute vertices common with CurEdge
      BRepAdaptor_Curve2d BAc2d(CurEdge, aFace);
      gp_Pnt2d fPnt2d, lPnt2d;
      fPnt2d = BAc2d.Value(BAc2d.FirstParameter());
      lPnt2d = BAc2d.Value(BAc2d.LastParameter());
      TopTools_IndexedMapOfShape Emap;
      TopExp::MapShapes(aFace, TopAbs_EDGE, Emap);
      TopoDS_Edge EE [2];
      Standard_Integer j = 0, k;
      for (k = 1; k <= Emap.Extent(); k++)
      {
        const TopoDS_Edge& anEdge = TopoDS::Edge(Emap(k));
        if (!BRep_Tool::Degenerated(anEdge))
        {
          TopoDS_Vertex V1, V2;
          TopExp::Vertices(anEdge, V1, V2);
          if (V1.IsSame(v1) || V2.IsSame(v1))
            EE[j++] = anEdge;
        }
      }
      for (k = 0; k < j; k++)
      {
        TopoDS_Shape aLocalShape = EE[k].Oriented(TopAbs_FORWARD);
        TopoDS_Edge Eforward = TopoDS::Edge(aLocalShape);
        Eforward.Free(Standard_True);
        TopoDS_Vertex V1, V2;
        TopExp::Vertices( Eforward, V1, V2 );
        BRepAdaptor_Curve2d EEc( Eforward, aFace );
        gp_Pnt2d p2d1, p2d2;
        p2d1 = EEc.Value(EEc.FirstParameter());
        p2d2 = EEc.Value(EEc.LastParameter());
        if (V1.IsSame(v1))
        {
          TopoDS_Vertex NewV = (p2d1.Distance(fPnt2d) <= Precision::Confusion())?
            FirstVert : EndVert;
          BB.Remove( Eforward, V1 );
          BB.Add( Eforward, NewV.Oriented(TopAbs_FORWARD) );
        }
        else
        {
          TopoDS_Vertex NewV = (p2d2.Distance(fPnt2d) <= Precision::Confusion())?
            FirstVert : EndVert;
          BB.Remove( Eforward, V2 );
          BB.Add( Eforward, NewV.Oriented(TopAbs_REVERSED) );
        }
      }
      
      isFirstFace = Standard_False;
      CurFirstVertex = EndVert;
    }
    //Building new spherical face
    Standard_Real Ufirst = RealLast(), Ulast = RealFirst();
    gp_Pnt2d p2d1, p2d2;
    TopTools_ListOfShape EdgesOfWire;
    TopoDS_Iterator itw(SphereWire);
    for (; itw.More(); itw.Next())
    {
      const TopoDS_Edge& anEdge = TopoDS::Edge(itw.Value());
      EdgesOfWire.Append(anEdge);
      Standard_Real f, l;
      Handle(Geom2d_Curve) aC2d = BRep_Tool::CurveOnSurface(anEdge, aSphSurf, L, f, l);
      p2d1 = aC2d->Value(f);
      p2d2 = aC2d->Value(l);
      if (p2d1.X() < Ufirst)
        Ufirst = p2d1.X();
      if (p2d1.X() > Ulast)
        Ulast = p2d1.X();
      if (p2d2.X() < Ufirst)
        Ufirst = p2d2.X();
      if (p2d2.X() > Ulast)
        Ulast = p2d2.X();
    }
    TopTools_ListOfShape NewEdges;
    TopoDS_Edge FirstEdge;
    TopTools_ListIteratorOfListOfShape itl(EdgesOfWire);
    for (; itl.More(); itl.Next())
    {
      FirstEdge = TopoDS::Edge(itl.Value());
      Standard_Real f, l;
      Handle(Geom2d_Curve) aC2d = BRep_Tool::CurveOnSurface(FirstEdge, aSphSurf, L, f, l);
      p2d1 = aC2d->Value(f);
      p2d2 = aC2d->Value(l);
      if (Abs(p2d1.X() - Ufirst) <= Precision::Confusion())
      {
        EdgesOfWire.Remove(itl);
        break;
      }
    }
    NewEdges.Append(FirstEdge);
    TopoDS_Vertex Vf1, CurVertex;
    TopExp::Vertices(FirstEdge, Vf1, CurVertex);
    itl.Initialize(EdgesOfWire);
    while (itl.More())
    {
      const TopoDS_Edge& anEdge = TopoDS::Edge(itl.Value());
      TopoDS_Vertex V1, V2;
      TopExp::Vertices(anEdge, V1, V2);
      if (V1.IsSame(CurVertex) || V2.IsSame(CurVertex))
      {
        NewEdges.Append(anEdge);
        CurVertex = (V1.IsSame(CurVertex))? V2 : V1;
        EdgesOfWire.Remove(itl);
      }
      else
        itl.Next();
    }
    
    Standard_Real Vfirst, Vlast;
    if (p2d1.Y() > 0.)
    {
      Vfirst = p2d1.Y(); Vlast = M_PI/2.;
    }
    else
    {
      Vfirst = -M_PI/2.; Vlast = p2d1.Y();
    }
    TopoDS_Face NewSphericalFace = BRepLib_MakeFace(aSphSurf, Ufirst, Ulast, Vfirst, Vlast, Precision::Confusion());
    TopoDS_Edge OldEdge;
    for (Explo.Init(NewSphericalFace, TopAbs_EDGE); Explo.More(); Explo.Next())
    {
      OldEdge = TopoDS::Edge(Explo.Current());
      if (!BRep_Tool::Degenerated(OldEdge))
      {
        BRepAdaptor_Curve2d BAc2d(OldEdge, NewSphericalFace);
        p2d1 = BAc2d.Value(BAc2d.FirstParameter());
        p2d2 = BAc2d.Value(BAc2d.LastParameter());
        if (Abs(p2d1.X() - Ufirst) <= Precision::Confusion() &&
            Abs(p2d2.X() - Ulast)  <= Precision::Confusion())
          break;
      }
    }
    TopoDS_Vertex V1, V2;
    TopExp::Vertices(OldEdge, V1, V2);
    TopTools_ListOfShape LV1, LV2;
    LV1.Append(Vf1);
    LV2.Append(CurVertex);
    BRepTools_Substitution theSubstitutor;
    theSubstitutor.Substitute(V1, LV1);
    if (!V1.IsSame(V2))
      theSubstitutor.Substitute(V2, LV2);
    theSubstitutor.Substitute(OldEdge, NewEdges);
    theSubstitutor.Build(NewSphericalFace);
    if (theSubstitutor.IsCopied(NewSphericalFace))
    {
      const TopTools_ListOfShape& listSh = theSubstitutor.Copy(NewSphericalFace);
      NewSphericalFace = TopoDS::Face(listSh.First());
    }
    
    //Adding NewSphericalFace to the shell
    Explo.Init( myOffsetShape, TopAbs_SHELL );
    TopoDS_Shape theShell = Explo.Current();
    theShell.Free( Standard_True );
    BB.Add( theShell, NewSphericalFace );
  }

  if (myShape.ShapeType() == TopAbs_SOLID || myThickening)
  {
    Explo.Init( myOffsetShape, TopAbs_SHELL );
    
    if (Explo.More()) {
      TopoDS_Shape theShell = Explo.Current();
      theShell.Closed( Standard_True );
    }
    
    Standard_Integer            NbShell = 0;
    TopoDS_Compound             NC;
    TopoDS_Shape                S1;
    BB.MakeCompound (NC);
    
    for (Explo.Init(myOffsetShape,TopAbs_SHELL); Explo.More(); Explo.Next()) {
      const TopoDS_Shell& Sh = TopoDS::Shell(Explo.Current());
      NbShell++;
      if (Sh.Closed()) {
        TopoDS_Solid  Sol;
        BB.MakeSolid  (Sol);
        BB.Add        (Sol,Sh);
        Sol.Closed(Standard_True);
        BB.Add (NC,Sol);
        if (NbShell == 1) S1 = Sol;
      }
      else {
        BB.Add (NC,Sh);
        if (NbShell == 1) S1 = Sh;
      }
    }
    if (NbShell == 1) myOffsetShape = S1;
    else              myOffsetShape = NC;
  }
}

//=======================================================================
//function : Intersection3D
//purpose  : 
//=======================================================================
void BRepOffset_MakeOffset::Intersection3D(BRepOffset_Inter3d& Inter)
{
#ifdef OCCT_DEBUG
  if (ChronBuild) {
    cout << " INTERSECTION 3D:" << endl;
    Clock.Reset();
    Clock.Start();  
  }
#endif
  TopTools_ListOfShape OffsetFaces;  // list of faces // created.
  MakeList (OffsetFaces, myInitOffsetFace, myFaces);

  if (!myFaces.IsEmpty()) {     
    Standard_Boolean InSide = (myOffset < 0.); // Temporary
    // it is necessary to calculate Inside taking account of the concavity or convexity of edges
    // between the cap and the part.

    if (myJoin == GeomAbs_Arc) 
      Inter.ContextIntByArc (myFaces,InSide,myAnalyse,myInitOffsetFace,myInitOffsetEdge);
  }
  if (myInter) {
    //-------------
    //Complete.
    //-------------
    Inter.CompletInt (OffsetFaces,myInitOffsetFace);
    TopTools_IndexedMapOfShape& NewEdges = Inter.NewEdges();
    if (myJoin == GeomAbs_Intersection) {
      BRepOffset_Tool::CorrectOrientation (myShape,NewEdges,myAsDes,myInitOffsetFace,myOffset);
    }
  }
  else {
    //--------------------------------
    // Only between neighbor faces.
    //--------------------------------
    Inter.ConnexIntByArc(OffsetFaces,myShape,myAnalyse,myInitOffsetFace);
  }
#ifdef OCCT_DEBUG
  if ( ChronBuild) Clock.Show();
#endif
}

//=======================================================================
//function : Intersection2D
//purpose  : 
//=======================================================================

void BRepOffset_MakeOffset::Intersection2D(const TopTools_IndexedMapOfShape& Modif,
                                           const TopTools_IndexedMapOfShape& NewEdges)
{
#ifdef OCCT_DEBUG
  if (ChronBuild) {
    cout << " INTERSECTION 2D:" << endl;
    Clock.Reset();
    Clock.Start();  
  }
#endif
  //--------------------------------------------------------
  // calculate intersections2d on faces concerned by 
  // intersection3d
  //---------------------------------------------------------
  //TopTools_MapIteratorOfMapOfShape it(Modif);
  //-----------------------------------------------
  // Intersection of edges 2 by 2.
  //-----------------------------------------------
  TopTools_IndexedDataMapOfShapeListOfShape aDMVV;
  Standard_Integer i;
  for (i = 1; i <= Modif.Extent(); i++) {
    const TopoDS_Face& F  = TopoDS::Face(Modif(i));
    BRepOffset_Inter2d::Compute(myAsDes,F,NewEdges,myTol, aDMVV);
  }
  //
  BRepOffset_Inter2d::FuseVertices(aDMVV, myAsDes);
  //
#ifdef OCCT_DEBUG
  if (AffichInt2d) {
    DEBVerticesControl (NewEdges,myAsDes);
  }
  if ( ChronBuild) Clock.Show();
#endif
}

//=======================================================================
//function : MakeLoops
//purpose  : 
//=======================================================================
void BRepOffset_MakeOffset::MakeLoops(TopTools_IndexedMapOfShape& Modif)
{
#ifdef OCCT_DEBUG
  if (ChronBuild) {
     cout << " DEBOUCLAGE 2D:" << endl;
     Clock.Reset();
     Clock.Start(); 
  }
#endif
  //TopTools_MapIteratorOfMapOfShape    it(Modif);
  TopTools_ListOfShape                LF,LC;
  //-----------------------------------------
  // unwinding of faces // modified.
  //-----------------------------------------
  Standard_Integer i;
  for (i = 1; i <= Modif.Extent(); i++) { 
    if (!myFaces.Contains(Modif(i)))
      LF.Append(Modif(i));
  }
  //
  if ((myJoin == GeomAbs_Intersection) && myInter && myIsPlanar) {
    BuildSplitsOfFaces(LF, myAsDes, myImageOffset);
  }
  else {
    myMakeLoops.Build(LF,myAsDes,myImageOffset);
  }
  //-----------------------------------------
  // unwinding of caps.
  //-----------------------------------------
  for (i = 1; i <= myFaces.Extent(); i++)
    LC.Append(myFaces(i));

  Standard_Boolean   InSide = 1;
  if (myOffset > 0 ) InSide = 0;
  myMakeLoops.BuildOnContext(LC,myAnalyse,myAsDes,myImageOffset,InSide);

#ifdef OCCT_DEBUG
  if ( ChronBuild) Clock.Show();
#endif
}

//=======================================================================
//function : MakeFaces
//purpose  : Reconstruction of topologically unchanged faces that
//           share edges that were reconstructed.
//=======================================================================
void BRepOffset_MakeOffset::MakeFaces(TopTools_IndexedMapOfShape& /*Modif*/)
{
#ifdef OCCT_DEBUG
  if (ChronBuild) {  
    cout << " RECONSTRUCTION OF FACES:" << endl;
    Clock.Reset();
    Clock.Start();
  }
#endif
  TopTools_ListIteratorOfListOfShape itr;
  const TopTools_ListOfShape& Roots = myInitOffsetFace.Roots();
  TopTools_ListOfShape        LOF;
  //----------------------------------
  // Loop on all faces //.
  //----------------------------------
  for (itr.Initialize(Roots); itr.More(); itr.Next()) {
    TopoDS_Face F = TopoDS::Face(myInitOffsetFace.Image(itr.Value()).First());
    if (!myImageOffset.HasImage(F)) {
      LOF.Append(F);
    }
  }
  //
  if ((myJoin == GeomAbs_Intersection) && myInter && myIsPlanar) {
    BuildSplitsOfFaces(LOF, myAsDes, myImageOffset);
  }
  else {
    myMakeLoops.BuildFaces(LOF,myAsDes,myImageOffset);
  }
  
#ifdef OCCT_DEBUG
  if ( ChronBuild) Clock.Show();
#endif
}

//=======================================================================
//function : UpdateInitOffset
//purpose  : Update and cleaning of myInitOffset 
//=======================================================================
static void UpdateInitOffset (BRepAlgo_Image&         myInitOffset,
                              BRepAlgo_Image&         myImageOffset,
                              const TopoDS_Shape&     myOffsetShape,
                              const TopAbs_ShapeEnum &theShapeType) // skv
{
  BRepAlgo_Image NIOF;
  const TopTools_ListOfShape& Roots = myInitOffset.Roots();
  TopTools_ListIteratorOfListOfShape it(Roots);
  for (; it.More(); it.Next()) {
    NIOF.SetRoot (it.Value());    
  }
  for (it.Initialize(Roots); it.More(); it.Next()) {
    const TopoDS_Shape& SI = it.Value();
    TopTools_ListOfShape LI;
    TopTools_ListOfShape L1;
    myInitOffset.LastImage(SI,L1);
    TopTools_ListIteratorOfListOfShape itL1(L1);
    for (; itL1.More(); itL1.Next()) {
      const TopoDS_Shape& O1 = itL1.Value();
      TopTools_ListOfShape L2;
      myImageOffset.LastImage(O1,L2);
      LI.Append(L2);
    }
    NIOF.Bind(SI,LI);
  }
//  Modified by skv - Mon Apr  4 18:17:27 2005 Begin
//  Supporting history.
//   NIOF.Filter(myOffsetShape,TopAbs_FACE);
  NIOF.Filter(myOffsetShape, theShapeType);
//  Modified by skv - Mon Apr  4 18:17:27 2005 End
  myInitOffset = NIOF;
}

//=======================================================================
//function : MakeMissingWalls
//purpose  : 
//=======================================================================
void BRepOffset_MakeOffset::MakeMissingWalls ()
{
  TopTools_DataMapOfShapeListOfShape Contours; //Start vertex + list of connected edges (free boundary)
  TopTools_DataMapOfShapeShape MapEF; //Edges of contours: edge + face
  Standard_Real OffsetVal = Abs(myOffset);

  FillContours(myShape, myAnalyse, Contours, MapEF);

  TopTools_DataMapIteratorOfDataMapOfShapeListOfShape iter(Contours);
  for (; iter.More(); iter.Next())
    {
      TopoDS_Vertex StartVertex = TopoDS::Vertex(iter.Key());
      TopoDS_Edge StartEdge;
      const TopTools_ListOfShape& aContour = iter.Value();
      TopTools_ListIteratorOfListOfShape itl(aContour);
      Standard_Boolean FirstStep = Standard_True;
      TopoDS_Edge PrevEdge;
      TopoDS_Vertex PrevVertex = StartVertex;
      for (; itl.More(); itl.Next())
        {
          TopoDS_Edge anEdge = TopoDS::Edge(itl.Value());
          if (!myInitOffsetEdge.HasImage(anEdge))
            continue;
          //if (BRep_Tool::Degenerated(anEdge))
            //continue;
          TopoDS_Face aFace = TopoDS::Face(MapEF(anEdge));
          //TopoDS_Edge OE = TopoDS::Edge(myInitOffsetEdge.Image(anEdge).First());
          TopTools_ListOfShape LOE, LOE2;
          myInitOffsetEdge.LastImage( anEdge, LOE );
          myImageOffset.LastImage( LOE.Last(), LOE2 );
          TopoDS_Edge OE = TopoDS::Edge( LOE2.Last() );
          ////////////////////////////////////////////////////////////////////////
          TopoDS_Vertex V1, V2, V3, V4;
          TopExp::Vertices(anEdge, V1, V2/*, Standard_True*/);
          TopExp::Vertices(OE,     V4, V3/*, Standard_True*/);
          Standard_Boolean ToReverse = Standard_False;
          if (!V1.IsSame(PrevVertex))
            {
              TopoDS_Vertex aVtx = V1; V1 = V2; V2 = aVtx;
              aVtx = V3; V3 = V4; V4 = aVtx;
              ToReverse = Standard_True;
            }
          //Temporary
          //anEdge.Reverse();
          OE.Orientation(TopAbs::Reverse(anEdge.Orientation()));
          TopoDS_Edge E3, E4;
          if (FirstStep)
            {
              E4 = BRepLib_MakeEdge( V1, V4 );
              StartEdge = E4;
            }
          else
            E4 = PrevEdge;
          Standard_Boolean ArcOnV2 = ((myJoin == GeomAbs_Arc) && (myInitOffsetEdge.HasImage(V2)));
          if (V2.IsSame(StartVertex) && !ArcOnV2)
            E3 = StartEdge;
          else
            E3 = BRepLib_MakeEdge( V2, V3 );
          E4.Reverse();
          TopoDS_Shape localAnEdge = anEdge.Oriented(TopAbs_FORWARD);
          const TopoDS_Edge& anEdgeFWD = TopoDS::Edge(localAnEdge);
          Standard_Real ParV1 = BRep_Tool::Parameter(V1, anEdgeFWD);
          Standard_Real ParV2 = BRep_Tool::Parameter(V2, anEdgeFWD);
          BRep_Builder BB;
          TopoDS_Wire theWire;
          BB.MakeWire(theWire);
          if (ToReverse)
            {
              BB.Add(theWire, anEdge.Reversed());
              BB.Add(theWire, E3.Reversed());
              BB.Add(theWire, OE.Reversed());
              BB.Add(theWire, E4.Reversed());
            }
          else
            {
              BB.Add(theWire, anEdge);
              BB.Add(theWire, E3);
              BB.Add(theWire, OE);
              BB.Add(theWire, E4);
            }
          BRepLib::BuildCurves3d( theWire, myTol );
          theWire.Closed(Standard_True);
          TopoDS_Face NewFace;
          Handle(Geom_Surface) theSurf;
          BRepAdaptor_Curve BAcurve(anEdge);
          BRepAdaptor_Curve BAcurveOE(OE);
          Standard_Real fpar = BAcurve.FirstParameter();
          Standard_Real lpar = BAcurve.LastParameter();
          gp_Pnt PonE  = BAcurve.Value(fpar);
          gp_Pnt PonOE = BAcurveOE.Value(fpar);
          gp_Dir OffsetDir = gce_MakeDir( PonE, PonOE );
          Handle(Geom2d_Line) EdgeLine2d, OELine2d, aLine2d, aLine2d2;
          Standard_Boolean IsPlanar = Standard_False;
          if (BAcurve.GetType() == GeomAbs_Circle &&
              BAcurveOE.GetType() == GeomAbs_Circle)
          {
            gp_Circ aCirc = BAcurve.Circle();
            gp_Circ aCircOE = BAcurveOE.Circle();
            gp_Lin anAxisLine(aCirc.Axis());
            gp_Dir CircAxisDir = aCirc.Axis().Direction();
            if (aCirc.Axis().IsParallel(aCircOE.Axis(), Precision::Confusion()) &&
                anAxisLine.Contains(aCircOE.Location(), Precision::Confusion()))
            { //cylinder, plane or cone
              if (Abs(aCirc.Radius() - aCircOE.Radius()) <= Precision::Confusion()) //case of cylinder
                theSurf = GC_MakeCylindricalSurface(aCirc).Value();
              else if (aCirc.Location().Distance(aCircOE.Location()) <= Precision::Confusion()) {//case of plane
                IsPlanar = Standard_True;
                //
                gp_Pnt PonEL = BAcurve.Value(lpar);
                if (PonEL.Distance(PonE) <= Precision::PConfusion()) {
                  Standard_Boolean bIsHole;
                  TopoDS_Edge aE1, aE2;
                  TopoDS_Wire aW1, aW2;
                  Handle(Geom_Plane) aPL;
                  IntTools_FClass2d aClsf;
                  //
                  if (aCirc.Radius()>aCircOE.Radius()) {
                    aE1 = anEdge;
                    aE2 = OE;
                  } else {
                    aE1 = OE;
                    aE2 = anEdge;
                  }
                  //
                  BB.MakeWire(aW1);
                  BB.Add(aW1, aE1);
                  BB.MakeWire(aW2);
                  BB.Add(aW2, aE2);
                  //
                  aPL = new Geom_Plane(aCirc.Location(), CircAxisDir);
                  for (Standard_Integer i = 0; i < 2; ++i) {
                    TopoDS_Wire& aW = (i==0) ? aW1 : aW2;
                    TopoDS_Edge& aE = (i==0) ? aE1 : aE2;
                    //
                    TopoDS_Face aFace;
                    BB.MakeFace(aFace, aPL, Precision::Confusion());
                    BB.Add (aFace, aW);
                    aClsf.Init(aFace, Precision::Confusion());
                    bIsHole=aClsf.IsHole();
                    if ((bIsHole && !i) || (!bIsHole && i)) {
                      aW.Nullify();
                      BB.MakeWire(aW);
                      BB.Add(aW, aE.Reversed());
                    }
                  }
                  //
                  BB.MakeFace(NewFace, aPL, Precision::Confusion());
                  BB.Add(NewFace, aW1);
                  BB.Add(NewFace, aW2);
                }
              }
              else //case of cone
              {
                gp_Cone theCone = gce_MakeCone(aCirc.Location(), aCircOE.Location(),
                                               aCirc.Radius(), aCircOE.Radius());
                gp_Ax3 theAx3(aCirc.Position());
                if (CircAxisDir * theCone.Axis().Direction() < 0.)
                {
                  theAx3.ZReverse();
                  CircAxisDir.Reverse();
                }
                theCone.SetPosition(theAx3);
                theSurf = new Geom_ConicalSurface(theCone);
              }
              if (!IsPlanar) {
                TopLoc_Location Loc;
                EdgeLine2d = new Geom2d_Line(gp_Pnt2d(0., 0.), gp_Dir2d(1., 0.));
                BB.UpdateEdge(anEdge, EdgeLine2d, theSurf, Loc, Precision::Confusion());
                Standard_Real Coeff = (OffsetDir * CircAxisDir > 0.)? 1. : -1.;
                OELine2d = new Geom2d_Line(gp_Pnt2d(0., OffsetVal*Coeff), gp_Dir2d(1., 0.));
                BB.UpdateEdge(OE, OELine2d, theSurf, Loc, Precision::Confusion());
                aLine2d  = new Geom2d_Line(gp_Pnt2d(ParV2, 0.), gp_Dir2d(0., Coeff));
                aLine2d2 = new Geom2d_Line(gp_Pnt2d(ParV1, 0.), gp_Dir2d(0., Coeff));
                if (E3.IsSame(E4))
                {
                  if (Coeff > 0.)
                    BB.UpdateEdge(E3, aLine2d, aLine2d2, theSurf, Loc, Precision::Confusion());
                  else
                  {
                    BB.UpdateEdge(E3, aLine2d2, aLine2d, theSurf, Loc, Precision::Confusion());
                    theWire.Nullify();
                    BB.MakeWire(theWire);
                    BB.Add(theWire, anEdge.Oriented(TopAbs_REVERSED));
                    BB.Add(theWire, E4);
                    BB.Add(theWire, OE.Oriented(TopAbs_FORWARD));
                    BB.Add(theWire, E3);
                    theWire.Closed(Standard_True);
                  }
                }
                else
                {
                  BB.SameParameter(E3, Standard_False);
                  BB.SameRange(E3, Standard_False);
                  BB.SameParameter(E4, Standard_False);
                  BB.SameRange(E4, Standard_False);
                  BB.UpdateEdge(E3, aLine2d,  theSurf, Loc, Precision::Confusion());
                  BB.Range(E3, theSurf, Loc, 0., OffsetVal);
                  BB.UpdateEdge(E4, aLine2d2, theSurf, Loc, Precision::Confusion());
                  BB.Range(E4, theSurf, Loc, 0., OffsetVal);
                }
                NewFace = BRepLib_MakeFace(theSurf, theWire);
              }
            } //cylinder or cone
          } //if both edges are arcs of circles
          if (NewFace.IsNull())
            {
              BRepLib_MakeFace MF(theWire, Standard_True); //Only plane
              if (MF.Error() == BRepLib_FaceDone)
                {
                  NewFace = MF.Face();
                  IsPlanar = Standard_True;
                }
              else //Extrusion (by thrusections)
                {
                  Handle(Geom_Curve) EdgeCurve = BRep_Tool::Curve(anEdge, fpar, lpar);
                  Handle(Geom_TrimmedCurve) TrEdgeCurve =
                    new Geom_TrimmedCurve( EdgeCurve, fpar, lpar );
                  Standard_Real fparOE, lparOE;
                  Handle(Geom_Curve) OffsetCurve = BRep_Tool::Curve(OE, fparOE, lparOE);
                  Handle(Geom_TrimmedCurve) TrOffsetCurve =
                    new Geom_TrimmedCurve( OffsetCurve, fparOE, lparOE );
                  GeomFill_Generator ThrusecGenerator;
                  ThrusecGenerator.AddCurve( TrEdgeCurve );
                  ThrusecGenerator.AddCurve( TrOffsetCurve );
                  ThrusecGenerator.Perform( Precision::PConfusion() );
                  theSurf = ThrusecGenerator.Surface();
                  //theSurf = new Geom_SurfaceOfLinearExtrusion( TrOffsetCurve, OffsetDir );
                  Standard_Real Uf, Ul, Vf, Vl;
                  theSurf->Bounds(Uf, Ul, Vf, Vl);
                  TopLoc_Location Loc;
                  EdgeLine2d = new Geom2d_Line(gp_Pnt2d(0., Vf), gp_Dir2d(1., 0.));
                  BB.UpdateEdge(anEdge, EdgeLine2d, theSurf, Loc, Precision::Confusion());
                  OELine2d = new Geom2d_Line(gp_Pnt2d(0., Vl), gp_Dir2d(1., 0.));
                  BB.UpdateEdge(OE, OELine2d, theSurf, Loc, Precision::Confusion());
                  Standard_Real UonV1 = (ToReverse)? Ul : Uf;
                  Standard_Real UonV2 = (ToReverse)? Uf : Ul;
                  aLine2d  = new Geom2d_Line(gp_Pnt2d(UonV2, 0.), gp_Dir2d(0., 1.));
                  aLine2d2 = new Geom2d_Line(gp_Pnt2d(UonV1, 0.), gp_Dir2d(0., 1.));
                  if (E3.IsSame(E4))
                    {
                      BB.UpdateEdge(E3, aLine2d, aLine2d2, theSurf, Loc, Precision::Confusion());
                      Handle(Geom_Curve) BSplC34 = theSurf->UIso( Uf );
                      BB.UpdateEdge(E3, BSplC34, Precision::Confusion());
                      BB.Range(E3, Vf, Vl);
                    }
                  else
                    {
                      BB.SameParameter(E3, Standard_False);
                      BB.SameRange(E3, Standard_False);
                      BB.SameParameter(E4, Standard_False);
                      BB.SameRange(E4, Standard_False);
                      BB.UpdateEdge(E3, aLine2d,  theSurf, Loc, Precision::Confusion());
                      BB.Range(E3, theSurf, Loc, Vf, Vl);
                      BB.UpdateEdge(E4, aLine2d2, theSurf, Loc, Precision::Confusion());
                      BB.Range(E4, theSurf, Loc, Vf, Vl);
                      Handle(Geom_Curve) BSplC3 = theSurf->UIso( UonV2 );
                      BB.UpdateEdge(E3, BSplC3, Precision::Confusion());
                      BB.Range(E3, Vf, Vl, Standard_True); //only for 3d curve
                      Handle(Geom_Curve) BSplC4 = theSurf->UIso( UonV1 );
                      BB.UpdateEdge(E4, BSplC4, Precision::Confusion());
                      BB.Range(E4, Vf, Vl, Standard_True); //only for 3d curve
                    }
                  NewFace = BRepLib_MakeFace(theSurf, theWire);
                }
            }
          if (!IsPlanar)
            {
              Standard_Real fparOE = BAcurveOE.FirstParameter();
              Standard_Real lparOE = BAcurveOE.LastParameter();
              TopLoc_Location Loc;
              if (Abs(fpar - fparOE) > Precision::Confusion())
                {
                  const TopoDS_Edge& anE4 = (ToReverse)? E3 : E4;
                  gp_Pnt2d fp2d   = EdgeLine2d->Value(fpar);
                  gp_Pnt2d fp2dOE = OELine2d->Value(fparOE);
                  aLine2d2 = GCE2d_MakeLine( fp2d, fp2dOE ).Value();
                  Handle(Geom_Curve) aCurve;
                  Standard_Real FirstPar = 0., LastPar = fp2d.Distance(fp2dOE);
                  Geom2dAdaptor_Curve AC2d( aLine2d2, FirstPar, LastPar );
                  GeomAdaptor_Surface GAsurf( theSurf );
                  Handle(Geom2dAdaptor_HCurve) HC2d  = new Geom2dAdaptor_HCurve( AC2d );
                  Handle(GeomAdaptor_HSurface) HSurf = new GeomAdaptor_HSurface( GAsurf );
                  Adaptor3d_CurveOnSurface ConS( HC2d, HSurf );
                  Standard_Real max_deviation = 0., average_deviation;
                  GeomLib::BuildCurve3d(Precision::Confusion(),
                                        ConS, FirstPar, LastPar,
                                        aCurve, max_deviation, average_deviation);
                  BB.UpdateEdge( anE4, aCurve, max_deviation );
                  BB.UpdateEdge( anE4, aLine2d2, theSurf, Loc, max_deviation );
                  BB.Range( anE4, FirstPar, LastPar );
                }
              if (Abs(lpar - lparOE) > Precision::Confusion())
                {
                  const TopoDS_Edge& anE3 = (ToReverse)? E4 : E3;
                  gp_Pnt2d lp2d   = EdgeLine2d->Value(lpar);
                  gp_Pnt2d lp2dOE = OELine2d->Value(lparOE);
                  aLine2d = GCE2d_MakeLine( lp2d, lp2dOE ).Value();
                  Handle(Geom_Curve) aCurve;
                  Standard_Real FirstPar = 0., LastPar = lp2d.Distance(lp2dOE);
                  Geom2dAdaptor_Curve AC2d( aLine2d, FirstPar, LastPar );
                  GeomAdaptor_Surface GAsurf( theSurf );
                  Handle(Geom2dAdaptor_HCurve) HC2d  = new Geom2dAdaptor_HCurve( AC2d );
                  Handle(GeomAdaptor_HSurface) HSurf = new GeomAdaptor_HSurface( GAsurf );
                  Adaptor3d_CurveOnSurface ConS( HC2d, HSurf );
                  Standard_Real max_deviation = 0., average_deviation;
                  GeomLib::BuildCurve3d(Precision::Confusion(),
                                        ConS, FirstPar, LastPar,
                                        aCurve, max_deviation, average_deviation);
                  BB.UpdateEdge( anE3, aCurve, max_deviation );
                  BB.UpdateEdge( anE3, aLine2d, theSurf, Loc, max_deviation );
                  BB.Range( anE3, FirstPar, LastPar );
                }
            }
          BRepLib::SameParameter(NewFace);
          BRepTools::Update(NewFace);
          myWalls.Append(NewFace);
          if (ArcOnV2)
            {
              TopoDS_Edge anArc = TopoDS::Edge(myInitOffsetEdge.Image(V2).First());
              TopoDS_Vertex arcV1, arcV2;
              TopExp::Vertices(anArc, arcV1, arcV2);
              Standard_Boolean ArcReverse = Standard_False;
              if (!arcV1.IsSame(V3))
                {
                  TopoDS_Vertex aVtx = arcV1; arcV1 = arcV2; arcV2 = aVtx;
                  ArcReverse = Standard_True;
                }
              TopoDS_Edge EA1, EA2;
              //EA1 = (ToReverse)? E3 : TopoDS::Edge(E3.Reversed());
              EA1 = E3;
              EA1.Reverse();
              if (ToReverse)
                EA1.Reverse();
              //////////////////////////////////////////////////////
              if (V2.IsSame(StartVertex))
                EA2 = StartEdge;
              else
                EA2 = BRepLib_MakeEdge( V2, arcV2 );
              anArc.Orientation( ((ArcReverse)? TopAbs_REVERSED : TopAbs_FORWARD) );
              if (EA1.Orientation() == TopAbs_REVERSED)
                anArc.Reverse();
              EA2.Orientation(TopAbs::Reverse(EA1.Orientation()));
              TopoDS_Wire arcWire;
              BB.MakeWire(arcWire);
              BB.Add(arcWire, EA1);
              BB.Add(arcWire, anArc);
              BB.Add(arcWire, EA2);
              BRepLib::BuildCurves3d( arcWire, myTol );
              arcWire.Closed(Standard_True);
              TopoDS_Face arcFace = BRepLib_MakeFace(arcWire, Standard_True);
              BRepTools::Update(arcFace);
              myWalls.Append(arcFace);
              TopoDS_Shape localEA2 = EA2.Oriented(TopAbs_FORWARD);
              const TopoDS_Edge& CEA2 = TopoDS::Edge(localEA2);
              PrevEdge = CEA2;
              PrevVertex = V2;
            }
          else
            {
              PrevEdge = E3;
              PrevVertex = V2;
            }
          FirstStep = Standard_False;
        }
    }
}

//=======================================================================
//function : MakeShells
//purpose  : 
//=======================================================================
void BRepOffset_MakeOffset::MakeShells()
{
#ifdef OCCT_DEBUG
  if (ChronBuild) {  
    cout << " RECONSTRUCTION OF SHELLS:" << endl;
    Clock.Reset();
    Clock.Start();
  }
#endif
  //
  TopTools_DataMapOfShapeListOfShape anOrigins;
  //
  BOPCol_ListOfShape aLSF;
  const TopTools_ListOfShape& R = myImageOffset.Roots();
  TopTools_ListIteratorOfListOfShape it(R);
  //
  for (; it.More(); it.Next()) {
    TopTools_ListOfShape Image;
    myImageOffset.LastImage(it.Value(),Image);
    TopTools_ListIteratorOfListOfShape it2(Image);
    for (; it2.More(); it2.Next()) {
      const TopoDS_Shape& aF = it2.Value();
      aLSF.Append(aF);
      //
      if (anOrigins.IsBound(aF)) {
        anOrigins.ChangeFind(aF).Append(it.Value());
      } 
      else {
        TopTools_ListOfShape aLOr;
        aLOr.Append(it.Value());
        anOrigins.Bind(aF, aLOr);
      }
    }
  }
  //
  if (myThickening) {
    TopExp_Explorer Explo(myShape, TopAbs_FACE);
    for (; Explo.More(); Explo.Next()) {
      const TopoDS_Shape& aF = Explo.Current();
      aLSF.Append(aF);
    }
    //
    it.Initialize(myWalls);
    for (; it.More(); it.Next()) {
      const TopoDS_Shape& aF = it.Value();
      aLSF.Append(aF);
    }
  }
  //
  if (aLSF.IsEmpty()) {
    return;
  }
  //
  Standard_Boolean bDone = Standard_False;
  if ((myJoin == GeomAbs_Intersection) && myInter &&
      !myThickening && myFaces.IsEmpty() &&
      IsSolid(myShape) && myIsPlanar) {
    //
    // make shells using MakerVolume algorithm
    Standard_Integer i, aNb;
    TopTools_ListIteratorOfListOfShape aItLS, aItLS1;
    BRep_Builder aBB;
    //
    TopoDS_Compound aCSF;
    aBB.MakeCompound(aCSF);
    //
    BOPAlgo_Builder aGF;
    //
    aGF.SetArguments(aLSF);
    aGF.Perform();
    bDone = (aGF.ErrorStatus() == 0);
    if (bDone) {
      UpdateHistory(aLSF, aGF, myImageOffset);
      //
      TopTools_ListOfShape aLSFt;
      BOPCol_ListIteratorOfListOfShape aItBLS(aLSF);
      for (; aItBLS.More(); aItBLS.Next()) {
        aLSFt.Append(aItBLS.Value());
      }
      UpdateOrigins(aLSFt, anOrigins, aGF);
      //
      const TopoDS_Shape& aR = aGF.Shape();
      TopExp_Explorer aExp(aR, TopAbs_FACE);
      aLSF.Clear();
      for (; aExp.More(); aExp.Next()) {
        const TopoDS_Shape& aF = aExp.Current();
        aLSF.Append(aF);
        aBB.Add(aCSF, aF);
      }
      //
      BOPAlgo_MakerVolume aMV1;
      //
      aMV1.AddArgument(aCSF);
      aMV1.SetIntersect(Standard_False);
      aMV1.Perform();
      bDone = (aMV1.ErrorStatus() == 0);
      if (bDone) {
        TopoDS_Shape aResult = aMV1.Shape();
        //
        TopTools_IndexedMapOfShape aMFResult;
        TopExp::MapShapes(aResult, TopAbs_FACE, aMFResult);
        //
        // check the result
        Standard_Boolean bGood = Standard_True;
        myRemoveInvalidFaces = Standard_False;
        if (myRemoveInvalidFaces) {
          BOPCol_ListIteratorOfListOfShape aItLSF(aLSF);
          for (; aItLSF.More(); aItLSF.Next()) {
            const TopoDS_Shape& aFx = aItLSF.Value();
            if (!aMFResult.Contains(aFx)) {
              const TopTools_ListOfShape& aLFMx = aMV1.Modified(aFx);
              if (aLFMx.IsEmpty()) {
                bGood = Standard_False;
                break;
              }
            }
          }
        }
        //
        TopoDS_Compound aShells;
        aBB.MakeCompound(aShells);
        //
        if (!bGood) {
          myOffsetShape = aShells;
        }
        else {
          if (aResult.ShapeType() == TopAbs_COMPOUND) {
            // collect faces attached to only one solid
            BOPCol_IndexedDataMapOfShapeListOfShape aMFS;
            BOPCol_ListOfShape aLSF2;
            //
            BOPTools::MapShapesAndAncestors(aResult, TopAbs_FACE, TopAbs_SOLID, aMFS);
            aNb = aMFS.Extent();
            bDone = (aNb > 0);
            //
            if (bDone) {
              for (i = 1; i <= aNb; ++i) {
                const BOPCol_ListOfShape& aLSx = aMFS(i);
                if (aLSx.Extent() == 1) {
                  const TopoDS_Shape& aFx = aMFS.FindKey(i);
                  aLSF2.Append(aFx);
                }
              }
              //
              // make solids from the new list
              BOPAlgo_MakerVolume aMV2;
              //
              aMV2.SetArguments(aLSF2);
              aMV2.SetIntersect(Standard_False);
              //
              aMV2.Perform();
              bDone = (aMV2.ErrorStatus() == 0);
              if (bDone) {
                aResult = aMV2.Shape();
                if (aResult.ShapeType() == TopAbs_COMPOUND) {
                  BOPCol_ListOfShape aLSF3;
                  //
                  TopExp_Explorer aExp(aResult, TopAbs_FACE);
                  for (; aExp.More(); aExp.Next()) {
                    const TopoDS_Face& aF = *(TopoDS_Face*)&aExp.Current();
                    //
                    // check orientation
                    if (!anOrigins.IsBound(aF)) {
                      aLSF3.Append(aF);
                      continue;
                    }
                    //
                    const TopTools_ListOfShape& aLFOr = anOrigins.Find(aF);
                    aItLS.Initialize(aLFOr);
                    for (; aItLS.More(); aItLS.Next()) {
                      const TopoDS_Face& aFOr = *(TopoDS_Face*)&aItLS.Value();
                      //
                      if (CheckNormals(aF, aFOr)) {
                        aLSF3.Append(aF);
                        break;
                      }
                    }
                  }
                  //
                  // make solid containing most outer faces
                  BOPAlgo_MakerVolume aMV3;
                  //
                  aMV3.SetArguments(aLSF3);
                  aMV3.SetIntersect(Standard_False);
                  //
                  aMV3.Perform();
                  bDone = (aMV3.ErrorStatus() == 0);
                  if (bDone) {
                    aResult = aMV3.Shape();
                  }
                }
              }
            }
          }
          //
          TopExp_Explorer aExp(aResult, TopAbs_SHELL);
          bDone = aExp.More();
          for (; aExp.More(); aExp.Next()) {
            const TopoDS_Shell& aSh = *(TopoDS_Shell*)&aExp.Current();
            aBB.Add(aShells, aSh);
          }
          myOffsetShape = aShells;
        }
      }
    }
  }
  //
  if (!bDone) {
    BRepTools_Quilt Glue;
    BOPCol_ListIteratorOfListOfShape aItLS;
    //
    aItLS.Initialize(aLSF);
    for (; aItLS.More(); aItLS.Next()) {
      const TopoDS_Shape& aF = aItLS.Value();
      Glue.Add(aF);
    }
    myOffsetShape = Glue.Shells();
  }
}

//=======================================================================
//function : MakeSolid
//purpose  : 
//=======================================================================
void BRepOffset_MakeOffset::MakeSolid ()
{
 if (myOffsetShape.IsNull()) return;

//  Modified by skv - Mon Apr  4 18:17:27 2005 Begin
//  Supporting history.
  UpdateInitOffset (myInitOffsetFace,myImageOffset,myOffsetShape, TopAbs_FACE);
  UpdateInitOffset (myInitOffsetEdge,myImageOffset,myOffsetShape, TopAbs_EDGE);
//  Modified by skv - Mon Apr  4 18:17:27 2005 End
  TopExp_Explorer             exp;
  BRep_Builder                B;
  Standard_Integer            NbShell = 0;
  TopoDS_Compound             NC;
  TopoDS_Shape                S1;
  B.MakeCompound (NC);

  for (exp.Init(myOffsetShape,TopAbs_SHELL); exp.More(); exp.Next()) {
    TopoDS_Shell Sh = TopoDS::Shell(exp.Current());
    if (myThickening && myOffset > 0.)
      Sh.Reverse();
    NbShell++;
    if (Sh.Closed()) {
      TopoDS_Solid  Sol;
      B.MakeSolid  (Sol);
      B.Add        (Sol,Sh);
      Sol.Closed(Standard_True);
      B.Add (NC,Sol);
      if (NbShell == 1) S1 = Sol;
    }
    else {
      B.Add (NC,Sh);
      if (NbShell == 1) S1 = Sh;
    }
  }
  if (NbShell == 1) myOffsetShape = S1;
  else              myOffsetShape = NC;
}

//=======================================================================
//function : SelectShells
//purpose  : 
//=======================================================================
void BRepOffset_MakeOffset::SelectShells ()
{
  TopTools_MapOfShape FreeEdges;
  TopExp_Explorer exp(myShape,TopAbs_EDGE);
  //-------------------------------------------------------------
  // FreeEdges all edges that can have free border in the  
  // parallel shell
  // 1 - free borders of myShape .
  //-------------------------------------------------------------
  for ( ; exp.More(); exp.Next()) {
    const TopoDS_Edge& E = TopoDS::Edge(exp.Current());
    const TopTools_ListOfShape& LA = myAnalyse.Ancestors(E);
    if (LA.Extent() < 2) {
      if (myAnalyse.Type(E).First().Type() == BRepOffset_FreeBoundary) {
              FreeEdges.Add(E);                       
      }
    }  
  }
  // myShape has free borders and there are no caps
  // no unwinding 3d.
  if (!FreeEdges.IsEmpty() && myFaces.IsEmpty()) return;

  myOffsetShape = BRepOffset_Tool::Deboucle3D(myOffsetShape,FreeEdges);
}

//=======================================================================
//function : OffsetFacesFromShapes
//purpose  : 
//=======================================================================
const BRepAlgo_Image& BRepOffset_MakeOffset::OffsetFacesFromShapes() const
{
  return myInitOffsetFace;
}

//  Modified by skv - Tue Mar 15 16:20:43 2005 Begin

//=======================================================================
//function : GetJoinType
//purpose  : Query offset join type.
//=======================================================================
GeomAbs_JoinType BRepOffset_MakeOffset::GetJoinType() const
{
  return myJoin;
}

//=======================================================================
//function : OffsetEdgesFromShapes
//purpose  : 
//=======================================================================
const BRepAlgo_Image& BRepOffset_MakeOffset::OffsetEdgesFromShapes() const
{
  return myInitOffsetEdge;
}

//  Modified by skv - Tue Mar 15 16:20:43 2005 End

//=======================================================================
//function : ClosingFaces
//purpose  : 
//=======================================================================
const TopTools_IndexedMapOfShape& BRepOffset_MakeOffset::ClosingFaces () const
{
  return myFaces;
}

//=======================================================================
//function : EncodeRegularity
//purpose  : 
//=======================================================================
void BRepOffset_MakeOffset::EncodeRegularity ()
{
#ifdef OCCT_DEBUG
  if (ChronBuild) {  
    cout << " CODING OF REGULARITIES:" << endl;
    Clock.Reset();
    Clock.Start();
  }
#endif

  if (myOffsetShape.IsNull()) return;
  // find edges G1 in the result
  TopExp_Explorer exp(myOffsetShape,TopAbs_EDGE);

  BRep_Builder B;
  TopTools_MapOfShape MS;

  for ( ; exp.More(); exp.Next()) {
    TopoDS_Edge OE  = TopoDS::Edge(exp.Current());
    BRepLib::BuildCurve3d(OE,myTol);
    TopoDS_Edge ROE = OE;
    
    if ( !MS.Add(OE)) continue;
      
    if ( myImageOffset.IsImage(OE)) 
      ROE = TopoDS::Edge(myImageOffset.Root(OE));

    const TopTools_ListOfShape& LofOF    = myAsDes->Ascendant(ROE);
    
    if (LofOF.Extent() != 2) {
#ifdef OCCT_DEBUG_VERB
    cout << " Edge shared by " << LofOF.Extent() << " Faces" << endl;
#endif
      continue;
    }

    const TopoDS_Face& F1 = TopoDS::Face(LofOF.First());
    const TopoDS_Face& F2 = TopoDS::Face(LofOF.Last() );
    
    if ( F1.IsNull() || F2.IsNull()) 
      continue;
   
    const TopoDS_Shape& Root1 = myInitOffsetFace.Root(F1);
    const TopoDS_Shape& Root2 = myInitOffsetFace.Root(F2);

    TopAbs_ShapeEnum Type1 = Root1.ShapeType();
    TopAbs_ShapeEnum Type2 = Root2.ShapeType();
 
    if (F1.IsSame(F2)) {      
      if (BRep_Tool::IsClosed(OE,F1)) {
        // Temporary Debug for the Bench.
        // Check with YFR.
        // In mode intersection, the edges are not coded in myInitOffsetEdge
        // so, manage case by case
        // Note DUB; for Hidden parts, it is NECESSARY to code CN 
        // Analytic Surfaces.
        if (myJoin == GeomAbs_Intersection) {
          BRepAdaptor_Surface BS(F1,Standard_False);
          GeomAbs_SurfaceType SType = BS.GetType();
          if (SType == GeomAbs_Cylinder ||
              SType == GeomAbs_Cone     ||
              SType == GeomAbs_Sphere   ||
              SType == GeomAbs_Torus      ) {
            B.Continuity(OE,F1,F1,GeomAbs_CN);
          }
          else {
            // See YFR : MaJ of myInitOffsetFace
          }
        }
        else if (myInitOffsetEdge.IsImage(ROE)) {
          if ( Type1 == TopAbs_FACE && Type2 == TopAbs_FACE) {
            const TopoDS_Face& FRoot = TopoDS::Face(Root1);
            const TopoDS_Edge& EI = TopoDS::Edge(myInitOffsetEdge.ImageFrom(ROE));
            GeomAbs_Shape Conti = BRep_Tool::Continuity(EI,FRoot,FRoot);
            if (Conti == GeomAbs_CN) {
              B.Continuity(OE,F1,F1,GeomAbs_CN);
            }
            else if ( Conti > GeomAbs_C0) {
              B.Continuity(OE,F1,F1,GeomAbs_G1);
            }
          }
        }
      }
      continue;
    }


    //  code regularities G1 between :
    //    - sphere and tube : one root is a vertex, the other is an edge 
    //                        and the vertex is included in the edge
    //    - face and tube   : one root is a face, the other an edge 
    //                        and the edge is included in the face
    //    - face and face    : if two root faces are tangent in 
    //                        the initial shape, they will be tangent in the offset shape
    //    - tube and tube  : if 2 edges generating tubes are
    //                        tangents, the 2 will be tangent either.
    if ( Type1 == TopAbs_EDGE && Type2 == TopAbs_VERTEX) {
      TopoDS_Vertex V1,V2;
      TopExp::Vertices(TopoDS::Edge(Root1), V1, V2);
      if ( V1.IsSame(Root2) || V2.IsSame(Root2)) {
        B.Continuity(OE,F1,F2,GeomAbs_G1);
      }
    }
    else if ( Type1 == TopAbs_VERTEX && Type2 == TopAbs_EDGE) {
      TopoDS_Vertex V1,V2;
      TopExp::Vertices(TopoDS::Edge(Root2), V1, V2);
      if ( V1.IsSame(Root1) || V2.IsSame(Root1)) {
        B.Continuity(OE,F1,F2,GeomAbs_G1);
      }
    }
    else if ( Type1 == TopAbs_FACE && Type2 == TopAbs_EDGE) {
      TopExp_Explorer exp2(Root1,TopAbs_EDGE);
      for ( ; exp2.More(); exp2.Next()) {
        if ( exp2.Current().IsSame(Root2)) {
          B.Continuity(OE,F1,F2,GeomAbs_G1);
          break;
        }
      }
    }
    else if ( Type1 == TopAbs_EDGE && Type2 == TopAbs_FACE) {
      TopExp_Explorer exp2(Root2,TopAbs_EDGE);
      for ( ; exp2.More(); exp2.Next()) {
        if ( exp2.Current().IsSame(Root1)) {
          B.Continuity(OE,F1,F2,GeomAbs_G1);
          break;
        }
      }
    }
    else if ( Type1 == TopAbs_FACE && Type2 == TopAbs_FACE) {
    //  if two root faces are tangent in 
    //  the initial shape, they will be tangent in the offset shape
      TopTools_ListOfShape LE,LV;
      BRepOffset_Tool::HasCommonShapes(TopoDS::Face(Root1),
                                       TopoDS::Face(Root2),
                                       LE,LV);
      if ( LE.Extent() == 1) { 
        const TopoDS_Edge& Ed = TopoDS::Edge(LE.First());
        if ( myAnalyse.HasAncestor(Ed)) {
          const BRepOffset_ListOfInterval& LI = myAnalyse.Type(Ed);
          if (LI.Extent()       == 1   && 
              LI.First().Type() == BRepOffset_Tangent) {
            B.Continuity(OE,F1,F2,GeomAbs_G1);
          }
        }
      }
    }
    else if ( Type1 == TopAbs_EDGE && Type2 == TopAbs_EDGE) {
      TopTools_ListOfShape LV;
      TopExp_Explorer exp1;
      for (exp1.Init(Root1,TopAbs_VERTEX); exp1.More(); exp1.Next()) {
        TopExp_Explorer exp2(F2,TopAbs_EDGE);
        for (exp2.Init(Root2,TopAbs_VERTEX); exp2.More(); exp2.Next()) {
          if (exp1.Current().IsSame(exp2.Current())) {
            LV.Append(exp1.Current());
          }
        }
      }
      if ( LV.Extent() == 1) {
        TopTools_ListOfShape LEdTg;
        myAnalyse.TangentEdges(TopoDS::Edge(Root1),
                               TopoDS::Vertex(LV.First()),
                               LEdTg);
        TopTools_ListIteratorOfListOfShape it(LEdTg);
        for (; it.More(); it.Next()) {
          if ( it.Value().IsSame(Root2)) {
            B.Continuity(OE,F1,F2,GeomAbs_G1);
            break;
          }
        }
      }
    }
  }

#ifdef OCCT_DEBUG
  if ( ChronBuild) Clock.Show();
#endif
}

//=======================================================================
//function : UpDateTolerance
//purpose  : 
//=======================================================================

void UpdateTolerance (TopoDS_Shape& S,
                                        const TopTools_IndexedMapOfShape& Faces)
{
  BRep_Builder B;
  TopTools_MapOfShape View;
  TopoDS_Vertex V[2];

  // The edges of caps are not modified.
  Standard_Integer j;
  for (j = 1; j <= Faces.Extent(); j++) {
    const TopoDS_Shape& F = Faces(j);
    TopExp_Explorer Exp;
    for (Exp.Init(F,TopAbs_EDGE); Exp.More(); Exp.Next()) {
      View.Add(Exp.Current());
    }
  }
  
  TopExp_Explorer Exp;
  for (Exp.Init(S,TopAbs_EDGE); Exp.More(); Exp.Next()) {
    TopoDS_Edge E = TopoDS::Edge(Exp.Current());
    if (View.Add(E)) {
      Handle(BRepCheck_Edge) EdgeCorrector = new BRepCheck_Edge(E);
      Standard_Real    Tol = EdgeCorrector->Tolerance();
      B.UpdateEdge (E,Tol);
      
      // Update the vertices.
      TopExp::Vertices(E,V[0],V[1]);
     
      for (Standard_Integer i = 0 ; i <=1 ; i++) {
        if (View.Add(V[i])) {
          Handle(BRep_TVertex) TV = Handle(BRep_TVertex)::DownCast(V[i].TShape());
          TV->Tolerance(0.);
          Handle(BRepCheck_Vertex) VertexCorrector = new BRepCheck_Vertex(V[i]);
          B.UpdateVertex (V[i],VertexCorrector->Tolerance());
          // use the occasion to clean the vertices.
          (TV->ChangePoints()).Clear();
        }
        B.UpdateVertex(V[i],Tol);
      }
    }
  }
}

//=======================================================================
//function : RemoveInternalEdges
//purpose  : 
//=======================================================================
void BRepOffset_MakeOffset::RemoveInternalEdges()
{
  Standard_Boolean bRemoveWire, bRemoveEdge;
  TopExp_Explorer aExpF, aExpW, aExpE;
  TopTools_IndexedDataMapOfShapeListOfShape aDMELF;
  //
  TopExp::MapShapesAndAncestors(myOffsetShape, TopAbs_EDGE, TopAbs_FACE, aDMELF);
  //
  aExpF.Init(myOffsetShape, TopAbs_FACE);
  for (; aExpF.More(); aExpF.Next()) {
    TopoDS_Face& aF = *(TopoDS_Face*)&aExpF.Current();
    //
    TopTools_ListOfShape aLIW;
    //
    aExpW.Init(aF, TopAbs_WIRE);
    for (; aExpW.More(); aExpW.Next()) {
      TopoDS_Wire& aW = *(TopoDS_Wire*)&aExpW.Current();
      //
      bRemoveWire = Standard_True;
      TopTools_ListOfShape aLIE;
      //
      aExpE.Init(aW, TopAbs_EDGE);
      for (; aExpE.More(); aExpE.Next()) {
        const TopoDS_Edge& aE = *(TopoDS_Edge*)&aExpE.Current();
        if (aE.Orientation() != TopAbs_INTERNAL) {
          bRemoveWire = Standard_False;
          continue;
        }
        //
        const TopTools_ListOfShape& aLF = aDMELF.FindFromKey(aE);
        bRemoveEdge = (aLF.Extent() == 1);
        if (bRemoveEdge) {
          aLIE.Append(aE);
        }
        else {
          bRemoveWire = Standard_False;
        }
      }
      //
      if (bRemoveWire) {
        aLIW.Append(aW);
      }
      else if (aLIE.Extent()) {
        RemoveShapes(aW, aLIE);
      }
    }
    //
    if (aLIW.Extent()) {
      RemoveShapes(aF, aLIW);
    }
  }
}

//=======================================================================
// static methods implementation
//=======================================================================

//=======================================================================
//function : RemoveShapes
//purpose  : Removes the shapes <theLS> from the shape <theS>
//=======================================================================
void RemoveShapes(TopoDS_Shape& theS,
                  const TopTools_ListOfShape& theLS)
{
  BRep_Builder aBB;
  //
  Standard_Boolean bFree = theS.Free();
  theS.Free(Standard_True);
  //
  TopTools_ListIteratorOfListOfShape aIt(theLS);
  for (; aIt.More(); aIt.Next()) {
    const TopoDS_Shape& aSI = aIt.Value();
    aBB.Remove(theS, aSI);
  }
  //
  theS.Free(bFree);
}

//=======================================================================
//function : UpdateHistory
//purpose  : Updates the history information
//=======================================================================
void UpdateHistory(const BOPCol_ListOfShape& theLF,
                   BOPAlgo_Builder& theGF,
                   BRepAlgo_Image& theImage)
{
  BOPCol_ListIteratorOfListOfShape aIt(theLF);
  for (; aIt.More(); aIt.Next()) {
    const TopoDS_Shape& aF = aIt.Value();
    const TopTools_ListOfShape& aLFIm = theGF.Modified(aF);
    if (aLFIm.Extent()) {
      if (theImage.HasImage(aF)) {
        theImage.Add(aF, aLFIm);
      }
      else {
        theImage.Bind(aF, aLFIm);
      }
    }
  }
}

//=======================================================================
//function : IsPlanar
//purpose  : 
//=======================================================================
Standard_Boolean IsPlanar(const TopoDS_Shape& theS)
{
  TopExp_Explorer aExp(theS, TopAbs_FACE);
  for (; aExp.More(); aExp.Next()) {
    const TopoDS_Face& aF = *(TopoDS_Face*)&aExp.Current();
    BRepAdaptor_Surface aBAS(aF, Standard_False);
    if (aBAS.GetType() != GeomAbs_Plane) {
      break;
    }
  }
  return !aExp.More();
}


//=======================================================================
// Extension for the mode Complete and Join Type Intersection;
// Only Planar cases are supported yet.
//=======================================================================

//=======================================================================
//function : IntersectEdges
//purpose  : 
//=======================================================================
void BRepOffset_MakeOffset::IntersectEdges(const TopoDS_Shape& theShape,
                                           BRepOffset_DataMapOfShapeOffset& theMapSF,
                                           TopTools_DataMapOfShapeShape& theMES,
                                           TopTools_DataMapOfShapeShape& theBuild,
                                           Handle(BRepAlgo_AsDes)& theAsDes,
                                           Handle(BRepAlgo_AsDes)& theAsDes2d)
{
  Standard_Real aTolF;
  TopTools_IndexedDataMapOfShapeListOfShape aDMVV;
  TopExp_Explorer aExp(theShape, TopAbs_FACE);
  //
  for (; aExp.More(); aExp.Next()) {
    const TopoDS_Face& aF = TopoDS::Face(aExp.Current());
    aTolF = BRep_Tool::Tolerance(aF);
    //
    BRepOffset_Inter2d::ConnexIntByInt
      (aF, theMapSF(aF), theMES, theBuild, theAsDes, theAsDes2d, myOffset, aTolF, aDMVV);
  }
  //
  // fuse vertices on edges
  BRepOffset_Inter2d::FuseVertices(aDMVV, theAsDes2d);
}

//=======================================================================
//function : TrimEdges
//purpose  : 
//=======================================================================
void TrimEdges(const TopoDS_Shape& theShape,
               const Standard_Real theOffset,
               BRepOffset_DataMapOfShapeOffset& theMapSF,
               TopTools_DataMapOfShapeShape& theMES,
               TopTools_DataMapOfShapeShape& theBuild,
               Handle(BRepAlgo_AsDes)& theAsDes,
               Handle(BRepAlgo_AsDes)& theAsDes2d,
               TopTools_IndexedMapOfShape& theNewEdges,
               TopTools_DataMapOfShapeShape& theETrimEInf,
               TopTools_DataMapOfShapeListOfShape& theEdgesOrigins)
{
  TopExp_Explorer Exp,Exp2,ExpC;
  TopoDS_Shape    NE;
  TopoDS_Edge     TNE;
  TopoDS_Face     NF;
  //
  for (Exp.Init(theShape,TopAbs_FACE) ; Exp.More(); Exp.Next()) {
    const TopoDS_Face& FI = TopoDS::Face(Exp.Current());
    NF = theMapSF(FI).Face();
    if (theMES.IsBound(NF)) {
      NF = TopoDS::Face(theMES(NF));
    }
    //
    TopTools_MapOfShape View;
    TopTools_IndexedMapOfShape VEmap;
    Standard_Integer i, aNb;
    //
    TopExp::MapShapes(FI.Oriented(TopAbs_FORWARD), TopAbs_EDGE  , VEmap);
    TopExp::MapShapes(FI.Oriented(TopAbs_FORWARD), TopAbs_VERTEX, VEmap);
    //
    aNb = VEmap.Extent();
    for (i = 1; i <= aNb; ++i) {
      const TopoDS_Shape& aS = VEmap(i);
      if (!View.Add(aS)) {
        continue;
      }
      //
      if (theBuild.IsBound(aS)) {
        NE = theBuild(aS);
        if (NE.ShapeType() == TopAbs_EDGE) {
          if (theEdgesOrigins.IsBound(NE)) {
            TopTools_ListOfShape& aLEOr = theEdgesOrigins.ChangeFind(NE);
            AppendToList(aLEOr, aS);
          }
          else {
            TopTools_ListOfShape aLSx;
            aLSx.Append(aS);
            theEdgesOrigins.Bind(NE, aLSx);
          }
          //
          if (theNewEdges.Add(NE))
          {
            TrimEdge (TopoDS::Edge(NE),theAsDes2d,theAsDes, theETrimEInf);
          }
        }
        else {
          //------------------------------------------------------------
          // The Intersections are on several edges.
          // The pieces without intersections with neighbors  
          // are removed from AsDes.
          //------------------------------------------------------------
          for (ExpC.Init(NE,TopAbs_EDGE); ExpC.More(); ExpC.Next()) {
            TopoDS_Edge NEC = TopoDS::Edge(ExpC.Current());
            if (theNewEdges.Add(NEC)) {
              NEC.Free(Standard_True);
              if (theEdgesOrigins.IsBound(NEC)) {
                TopTools_ListOfShape& aLEOr = theEdgesOrigins.ChangeFind(NEC);
                AppendToList(aLEOr, aS);
              }
              else {
                TopTools_ListOfShape aLSx;
                aLSx.Append(aS);
                theEdgesOrigins.Bind(NEC, aLSx);
              }
              //
              if (!theAsDes2d->Descendant(NEC).IsEmpty()) {
                TrimEdge (NEC,theAsDes2d, theAsDes, theETrimEInf);
              }
              else {
                if (theAsDes->HasAscendant(NEC)) {
                  theAsDes->Remove(NEC);
                }
              }
            }
          }
        }
      }
      else {
        if (aS.ShapeType() != TopAbs_EDGE) {
          continue;
        }
        //
        NE = theMapSF(FI).Generated(aS);
        //// modified by jgv, 19.12.03 for OCC4455 ////
        NE.Orientation(aS.Orientation());
        if (theEdgesOrigins.IsBound(NE)) {
          TopTools_ListOfShape& aLEOr = theEdgesOrigins.ChangeFind(NE);
          AppendToList(aLEOr, aS);
        }
        else {
          TopTools_ListOfShape aLSx;
          aLSx.Append(aS);
          theEdgesOrigins.Bind(NE, aLSx);
        }
        //
        Standard_Boolean bTrimmed = Standard_True;
        if (theMES.IsBound(NE)) {
          NE = theMES(NE);
          NE.Orientation(aS.Orientation());
          if (theNewEdges.Add(NE))
          {
            bTrimmed = TrimEdge (TopoDS::Edge(NE), theAsDes2d, theAsDes, theETrimEInf);
          } 
        }
        else {
          TopoDS_Edge& anEdge = TopoDS::Edge(NE);
          BRepAdaptor_Curve aBAC(anEdge);
          if (aBAC.GetType() == GeomAbs_Line) {
            TopoDS_Edge aNewEdge;
            BRepOffset_Inter2d::ExtentEdge(anEdge, aNewEdge, theOffset);
            theETrimEInf.Bind(anEdge, aNewEdge);
          }
        }
        if (bTrimmed) {
          theAsDes->Add(NF, NE);
        }
      } 
    }
  }
}

//=======================================================================
//function : GetEnlargedFaces
//purpose  : 
//=======================================================================
void GetEnlargedFaces(const TopoDS_Shape& theShape,
                      const BRepOffset_DataMapOfShapeOffset& theMapSF,
                      const TopTools_DataMapOfShapeShape& theMES,
                      TopTools_DataMapOfShapeShape& theFacesOrigins,
                      BRepAlgo_Image& theImage,
                      TopTools_ListOfShape& theLSF)
{
  TopExp_Explorer aExp(theShape, TopAbs_FACE);
  for (; aExp.More(); aExp.Next()) {
    const TopoDS_Shape& FI  = aExp.Current();
    const TopoDS_Shape& OFI = theMapSF(FI).Face();
    if (theMES.IsBound(OFI)) {
      const TopoDS_Face& aLocalFace = TopoDS::Face(theMES(OFI));
      theLSF.Append(aLocalFace);
      theImage.SetRoot(aLocalFace);
      //
      theFacesOrigins.Bind(aLocalFace, FI);
    }
  }
}

//=======================================================================
//function : IntersectTrimmedEdges
//purpose  : 
//=======================================================================
void IntersectTrimmedEdges(const TopTools_ListOfShape& theLF,
                           const Handle(BRepAlgo_AsDes)& theAsDes,
                           TopTools_DataMapOfShapeListOfShape& theOEImages,
                           TopTools_DataMapOfShapeListOfShape& theOEOrigins,
                           TopTools_DataMapOfShapeListOfShape& theEdgesOrigins,
                           Handle(IntTools_Context)& theCtx,
                           TopTools_MapOfShape& theNewEdges,
                           TopTools_DataMapOfShapeShape& theETrimEInf)
{
  if (theLF.IsEmpty()) {
    return;
  }
  //
  BOPCol_ListOfShape aLS;
  BOPCol_ListIteratorOfListOfShape aIt;
  TopTools_ListIteratorOfListOfShape aItLF, aItLE;
  //
  aItLF.Initialize(theLF);
  for (; aItLF.More(); aItLF.Next()) {
    const TopoDS_Face& aF = *(TopoDS_Face*)&aItLF.Value();
    //
    const TopTools_ListOfShape& aLE = theAsDes->Descendant(aF);
    aItLE.Initialize(aLE);
    for (; aItLE.More(); aItLE.Next()) {
      const TopoDS_Edge& aE = *(TopoDS_Edge*)&aItLE.Value();
      //
      if (BRep_Tool::Degenerated(aE)) {
        continue;
      }
      //
      if (ProcessMicroEdge(aE, theCtx)) {
        continue;
      }
      //
      if (theNewEdges.Add(aE)) {
        aLS.Append(aE);
      }
    }
  }
  //
  if (aLS.Extent() > 1) {
    BOPAlgo_Builder aGFE;
    //
    aGFE.SetArguments(aLS);
    aGFE.Perform();
    if (aGFE.ErrorStatus())
      return;
    TopTools_ListOfShape aLA;
    // fill map with edges images
    aIt.Initialize(aLS);
    for (; aIt.More(); aIt.Next()) {
      const TopoDS_Shape& aE = aIt.Value();
      aLA.Append(aE);
      //
      const TopTools_ListOfShape& aLEIm = aGFE.Modified(aE);
      //
      if (aLEIm.Extent()) {
        theOEImages.Bind(aE, aLEIm);
        aItLE.Initialize(aLEIm);
        for (; aItLE.More(); aItLE.Next()) {
          const TopoDS_Shape& aEIm = aItLE.Value();
          if (theOEOrigins.IsBound(aEIm)) {
            TopTools_ListOfShape& aLEOr = theOEOrigins.ChangeFind(aEIm);
            AppendToList(aLEOr, aE);
          }
          else {
            TopTools_ListOfShape aLEOr;
            aLEOr.Append(aE);
            theOEOrigins.Bind(aEIm, aLEOr);
          }
        }
      }
    }
    //
    UpdateOrigins(aLA, theEdgesOrigins, aGFE);
    UpdateIntersectedEdges(aLA, theETrimEInf, aGFE);
  }
}

//=======================================================================
//function : BuildSplitsOfFaces
//purpose  : 
//=======================================================================
void BuildSplitsOfFaces(const TopTools_ListOfShape& theLF,
                        Handle(BRepAlgo_AsDes)& theAsDes,
                        BRepAlgo_Image& theImage)
{
  TopTools_DataMapOfShapeListOfShape anEImages, anEOrigins;
  TopTools_IndexedDataMapOfShapeListOfShape aDMFFIm;
  TopTools_IndexedMapOfShape anEmptyIM;
  TopTools_DataMapOfShapeListOfShape anEmptyDMSLS;
  TopTools_DataMapOfShapeShape anEmptyDMSS;
  TopTools_MapOfShape aNewEdges, anEmptyM;
  //
  // firstly it is necessary to fuse all the edges
  Handle(IntTools_Context) aCtx = new IntTools_Context();
  //
  IntersectTrimmedEdges(theLF, theAsDes, anEImages, anEOrigins, anEmptyDMSLS, aCtx, aNewEdges, anEmptyDMSS);
  //
  TopTools_ListIteratorOfListOfShape aItLF(theLF);
  for (; aItLF.More(); aItLF.Next()) {
    const TopoDS_Face& aF = *(TopoDS_Face*)&aItLF.Value();
    //
    TopoDS_Shape aCE;
    TopTools_ListOfShape aLFImages;
    //
    Standard_Boolean bFound = GetEdges(aF, theAsDes, anEImages, anEmptyM, 
                                       anEmptyIM, aCtx, aNewEdges, aCE, anEmptyM);
    // split the face by the edges
    if (!bFound) {
      if (!theImage.HasImage(aF)) {
        aLFImages.Append(aF);
        aDMFFIm.Add(aF, aLFImages);
      }
      continue;
    }
    //
    BuildSplitsOfFace(aF, aCE, Standard_True, anEmptyDMSS, aLFImages);
    aDMFFIm.Add(aF, aLFImages);
  }
  //
  FillHistory(aDMFFIm, anEImages, theImage);
}

//=======================================================================
//function : BuildSplitsOfFaces
//purpose  : 
//=======================================================================
void BuildSplitsOfFaces(const TopTools_ListOfShape& theLF,
                        Handle(BRepAlgo_AsDes)& theAsDes,
                        TopTools_DataMapOfShapeListOfShape& theEdgesOrigins,
                        TopTools_DataMapOfShapeShape& theFacesOrigins,
                        BRepAlgo_Image& theImage,
                        TopTools_DataMapOfShapeShape& theETrimEInf)
{
  TopTools_DataMapOfShapeInteger anAlreadyInvFaces;
  TopTools_IndexedMapOfShape anInvEdges, aValidEdges, anEdgesToAvoid;
  TopTools_DataMapOfShapeListOfShape anOEImages, anOEOrigins; // images and origins for offset edges
  TopTools_IndexedDataMapOfShapeListOfShape aDMFFIm, anInvFaces;
  TopTools_MapOfShape aNewEdges, aLastInvEdges, aVAEmpty;
  //
  // artificially invalid faces
  TopTools_DataMapOfShapeShape anArtInvFaces;
  //
  // firstly it is necessary to fuse all the edges
  Handle(IntTools_Context) aCtx = new IntTools_Context();
  IntersectTrimmedEdges(theLF, theAsDes, anOEImages, anOEOrigins, 
                        theEdgesOrigins, aCtx, aNewEdges, theETrimEInf);
  //
  // now we can split the faces
  BuildSplitsOfFaces(theLF, theAsDes, aNewEdges, theEdgesOrigins, theFacesOrigins, 
                     anOEImages, anOEOrigins, aLastInvEdges, anEdgesToAvoid, anInvEdges, aValidEdges, 
                     anAlreadyInvFaces, anInvFaces, anArtInvFaces, aDMFFIm);
  //
  // Find faces to rebuild
  if (anInvFaces.Extent()) {
    TopTools_IndexedDataMapOfShapeListOfShape aFToRebuild;
    TopTools_MapOfShape aFSelfRebAvoid;
    FindFacesToRebuild(aDMFFIm, anInvEdges, anInvFaces, aFToRebuild, aFSelfRebAvoid);
    //
    if (aFToRebuild.Extent()) {
      RebuildFaces(aFToRebuild, aFSelfRebAvoid, aDMFFIm, theEdgesOrigins, theFacesOrigins,
                   anOEImages, anOEOrigins, aLastInvEdges, anEdgesToAvoid, anInvEdges, aValidEdges,
                   anAlreadyInvFaces, anInvFaces, anArtInvFaces, aVAEmpty, theETrimEInf, theAsDes);
    }
  }
  //Fill history for faces
  FillHistory(aDMFFIm, anOEImages, theImage);
}

//=======================================================================
//function : BuildSplitsOfFaces
//purpose  : 
//=======================================================================
void BuildSplitsOfFaces(const TopTools_ListOfShape& theLF,
                        Handle(BRepAlgo_AsDes)& theAsDes,
                        const TopTools_MapOfShape& theModifiedEdges,
                        TopTools_DataMapOfShapeListOfShape& theEdgesOrigins,
                        TopTools_DataMapOfShapeShape& theFacesOrigins,
                        TopTools_DataMapOfShapeListOfShape& theOEImages,
                        TopTools_DataMapOfShapeListOfShape& theOEOrigins,
                        TopTools_MapOfShape& theLastInvEdges,
                        TopTools_IndexedMapOfShape& theEdgesToAvoid,
                        TopTools_IndexedMapOfShape& theInvEdges,
                        TopTools_IndexedMapOfShape& theValidEdges,
                        TopTools_DataMapOfShapeInteger& theAlreadyInvFaces,
                        TopTools_IndexedDataMapOfShapeListOfShape& theInvFaces,
                        TopTools_DataMapOfShapeShape& theArtInvFaces,
                        TopTools_IndexedDataMapOfShapeListOfShape& theFImages)
{
  if (theLF.IsEmpty()) {
    return;
  }
  //
  Standard_Integer i, aNb;
  Standard_Boolean bFound;
  // processed faces
  TopTools_ListOfShape aLFDone;
  // extended face - list of neutral edges, i.e. in one splits - valid and in others - invalid
  TopTools_DataMapOfShapeListOfShape aDMFLNE;
  // list of valid edges for each face
  TopTools_DataMapOfShapeListOfShape aDMFLVE;
  // list of invalid edges for each face
  TopTools_DataMapOfShapeListOfShape aDMFLIE;
  // map of inverted edges
  TopTools_MapOfShape aMEInverted;
  // map of splits to check for internals
  TopTools_IndexedMapOfShape aMFToCheckInt;
  //
  Handle(IntTools_Context) aCtx = new IntTools_Context;
  // build splits of faces
  TopTools_ListIteratorOfListOfShape aItLF(theLF);
  for (; aItLF.More(); aItLF.Next()) {
    const TopoDS_Face& aF = *(TopoDS_Face*)&aItLF.Value();
    //
    // get edges by which the face should be split
    TopoDS_Shape aCE;
    TopTools_MapOfShape aMapEInv;
    bFound = GetEdges(aF, theAsDes, theOEImages, theLastInvEdges,
      theEdgesToAvoid, aCtx, theModifiedEdges, aCE, aMapEInv);
    if (!bFound) {
      continue;
    }
    //
    // build splits
    TopTools_ListOfShape aLFImages;
    BuildSplitsOfFace(aF, aCE, Standard_False, theFacesOrigins, aLFImages);
    //
    if (aMapEInv.Extent()) {
      // check if all possible faces are built
      TopTools_MapOfShape aMENInv;
      Standard_Boolean bArtificialCase = aLFImages.IsEmpty() ||
        CheckIfArtificial(aF, aLFImages, aCE, aMapEInv, aMENInv, theAsDes, theOEImages);
      //
      // try to build splits using invalid edges
      TopTools_IndexedMapOfShape anEmptyInvEdges;
      TopoDS_Shape aCE1;
      GetEdges(aF, theAsDes, theOEImages, theLastInvEdges, anEmptyInvEdges,
        aCtx, theModifiedEdges, aCE1, aMapEInv);
      //
      TopTools_ListOfShape aLFImages1;
      BuildSplitsOfFace(aF, aCE1, Standard_False, theFacesOrigins, aLFImages1);
      //
      // additionally check if newly created faces are not the same
      Standard_Boolean bAllInv = Standard_True;
      TopTools_ListIteratorOfListOfShape aItLFIm(aLFImages1);
      for (; aItLFIm.More();) {
        const TopoDS_Shape& aFIm = aItLFIm.Value();
        TopExp_Explorer aExpE(aFIm, TopAbs_EDGE);
        for (; aExpE.More(); aExpE.Next()) {
          const TopoDS_Shape& aE = aExpE.Current();
          if (!aMapEInv.Contains(aE) && !aMENInv.Contains(aE)) {
            break;
          }
          else if (bAllInv) {
            bAllInv = aMapEInv.Contains(aE);
          }
        }
        //
        if (!aExpE.More()) {
          if (bAllInv) {
            aMFToCheckInt.Add(aFIm);
          }
          aLFImages1.Remove(aItLFIm);
        }
        else {
          aItLFIm.Next();
        }
      }
      //
      if (bArtificialCase) {
        if (aLFImages.Extent() == aLFImages1.Extent()) {
          bArtificialCase = Standard_False;
        }
        else {
          aLFImages = aLFImages1;
        }
      }
      //
      if (bArtificialCase) {
        if (theFImages.Contains(aF)) {
          TopTools_ListOfShape& aLFIm1 = theFImages.ChangeFromKey(aF);
          if (aLFIm1.Extent()) {
            TopTools_ListOfShape aLEInv;
            // make the face invalid
            theArtInvFaces.Bind(aF, aCE);
            //
            aLFIm1 = aLFImages;
            TopTools_ListIteratorOfListOfShape aItLFIm(aLFImages);
            for (; aItLFIm.More(); aItLFIm.Next()) {
              const TopoDS_Shape& aFIm = aItLFIm.Value();
              TopExp_Explorer aExpE(aFIm, TopAbs_EDGE);
              for (; aExpE.More(); aExpE.Next()) {
                const TopoDS_Shape& aE = aExpE.Current();
                if (aMapEInv.Contains(aE)) {
                  theInvEdges.Add(aE);
                  AppendToList(aLEInv, aE);
                }
                else {
                  theValidEdges.Add(aE);
                }
              }
            }
            //
            aDMFLIE.Bind(aF, aLEInv);
            aLFDone.Append(aF);
          }
        }
        continue;
      }
    }
    //
    // find invalid edges
    FindInvalidEdges(aF, aLFImages, theEdgesOrigins, theFacesOrigins,
      theOEImages, theOEOrigins, theInvEdges, theValidEdges,
      aDMFLVE, aDMFLNE, aDMFLIE, aMEInverted);
    //
    // save the new splits
    if (theFImages.Contains(aF)) {
      theFImages.ChangeFromKey(aF) = aLFImages;
    }
    else {
      theFImages.Add(aF, aLFImages);
    }
    //
    aLFDone.Append(aF);
  }
  //
  if (theInvEdges.IsEmpty() && theArtInvFaces.IsEmpty()) {
    return;
  }
  //
  // find invalid faces
  // considering faces containing only invalid edges as invalid
  aItLF.Initialize(aLFDone);
  for (; aItLF.More(); aItLF.Next()) {
    const TopoDS_Shape& aF = aItLF.Value();
    TopTools_ListOfShape& aLFImages = theFImages.ChangeFromKey(aF);
    //
    TopTools_ListOfShape aLFInv;
    Standard_Boolean bArtificialCase = theArtInvFaces.IsBound(aF);
    if (bArtificialCase) {
      aLFInv = aLFImages;
    }
    else {
      // neutral edges
      const TopTools_ListOfShape& aLNE = aDMFLNE.Find(aF);
      // invalid edges
      const TopTools_ListOfShape& aLIE = aDMFLIE.Find(aF);
      //
      FindInvalidFaces(aLFImages, theInvEdges, theValidEdges, aDMFLVE, aLIE, aLNE, aMEInverted, aLFInv);
    }
    //
    if (aLFInv.Extent()) {
      if (theAlreadyInvFaces.IsBound(aF)) {
        if (aLFInv.Extent() == aLFImages.Extent() && !bArtificialCase) {
          aLFImages.Clear();
        }
        //
        if (theAlreadyInvFaces.Find(aF) > 3) {
          aLFInv.Clear();
        }
      }
      theInvFaces.Add(aF, aLFInv);
    }
  }
  //
  aNb = theInvFaces.Extent();
  if (!aNb) {
    theInvEdges.Clear();
    return;
  }
  //
  // remove invalid splits from valid splits
  RemoveInvalidSplitsFromValid(theFImages, theInvFaces, theArtInvFaces, aMEInverted);
  //
  // remove inside faces
  TopTools_IndexedMapOfShape aMERemoved;
  RemoveInsideFaces(theFImages, theInvFaces, theArtInvFaces, 
                    theInvEdges, aMFToCheckInt, aMERemoved);
  //
  // make compound of valid splits
  TopoDS_Compound aCFIm;
  BRep_Builder aBB;
  aBB.MakeCompound(aCFIm);
  //
  aNb = theFImages.Extent();
  for (i = 1; i <= aNb; ++i) {
    const TopTools_ListOfShape& aLFIm = theFImages(i);
    aItLF.Initialize(aLFIm);
    for (; aItLF.More(); aItLF.Next()) {
      const TopoDS_Shape& aFIm = aItLF.Value();
      aBB.Add(aCFIm, aFIm);
    }
  }
  //
  TopTools_IndexedDataMapOfShapeListOfShape aDMFE;
  TopExp::MapShapesAndAncestors(aCFIm, TopAbs_EDGE, TopAbs_FACE, aDMFE);
  //
  // filter maps of images and origins
  FilterEdgesImages(aCFIm, theOEImages, theOEOrigins);
  //
  // filter invalid faces
  FilterInvalidFaces(theInvFaces, theArtInvFaces, theFImages, aDMFE);
  aNb = theInvFaces.Extent();
  if (!aNb) {
    theInvEdges.Clear();
    return;
  }
  //
  // filter invalid edges
  FilterInvalidEdges(theInvEdges, theInvFaces, theArtInvFaces, aDMFLIE, aMERemoved);
  //
  theLastInvEdges.Clear();
  aNb = theInvEdges.Extent();
  for (i = 1; i <= aNb; ++i) {
    const TopoDS_Shape& aE = theInvEdges(i);
    theEdgesToAvoid.Add(aE);
    theLastInvEdges.Add(aE);
  }
  //
  aNb = theInvFaces.Extent();
  for (i = 1; i <= aNb; ++i) {
    const TopoDS_Shape& aF = theInvFaces.FindKey(i);
    if (theAlreadyInvFaces.IsBound(aF)) {
      theAlreadyInvFaces.ChangeFind(aF)++;
    }
    else {
      theAlreadyInvFaces.Bind(aF, 1);
    }
  }
}

//=======================================================================
//function : FindFacesToRebuild
//purpose  : 
//=======================================================================
void FindFacesToRebuild(TopTools_IndexedDataMapOfShapeListOfShape&  theLFImages,
                        const TopTools_IndexedMapOfShape&  theInvEdges,
                        const TopTools_IndexedDataMapOfShapeListOfShape& theInvFaces,
                        TopTools_IndexedDataMapOfShapeListOfShape& theFToRebuild,
                        TopTools_MapOfShape& theFSelfRebAvoid)
{
  Standard_Integer i, aNb = theLFImages.Extent();
  if (!aNb) {
    return;
  }
  //
  Standard_Boolean bRebuild;
  TopTools_ListIteratorOfListOfShape aItLF;
  TopTools_ListOfShape aLEValid;
  TopTools_IndexedMapOfShape aMEReb;
  TopTools_MapOfShape aMFence;
  TopExp_Explorer aExp;
  //
  TopTools_DataMapOfShapeListOfShape aDMFLV;
  // get edges from invalid faces
  aNb = theInvFaces.Extent();
  for (i = 1; i <= aNb; i++) {
    const TopoDS_Shape& aF = theInvFaces.FindKey(i);
    aMFence.Clear();
    TopTools_ListOfShape aLVAvoid;
    const TopTools_ListOfShape& aLFIm = theInvFaces(i);
    aItLF.Initialize(aLFIm);
    for (; aItLF.More(); aItLF.Next()) {
      const TopoDS_Shape& aFIm = aItLF.Value();
      aExp.Init(aFIm, TopAbs_EDGE);
      for (; aExp.More(); aExp.Next()) {
        const TopoDS_Shape& aE = aExp.Current();
        aMEReb.Add(aE);
        if (theInvEdges.Contains(aE)) {
          TopExp_Explorer aExpV(aE, TopAbs_VERTEX);
          for (; aExpV.More(); aExpV.Next()) {
            const TopoDS_Shape& aV = aExpV.Current();
            if (aMFence.Add(aV)) {
              aLVAvoid.Append(aV);
              aMEReb.Add(aV);
            }
          }
        }
      }
    }
    if (aLVAvoid.Extent()) {
      aDMFLV.Bind(aF, aLVAvoid);
    }
  }
  //
  // get face to rebuild
  aNb = theLFImages.Extent();
  for (i = 1; i <= aNb; i++) {
    const TopoDS_Shape& aF = theLFImages.FindKey(i);
    TopTools_ListOfShape& aLFIm = theLFImages(i);
    TopTools_MapOfShape aMVAvoid;
    if (aDMFLV.IsBound(aF)) {
      const TopTools_ListOfShape& aLVAvoid = aDMFLV.Find(aF);
      TopTools_ListIteratorOfListOfShape aItLV(aLVAvoid);
      for (; aItLV.More(); aItLV.Next()) {
        const TopoDS_Shape& aV = aItLV.Value();
        aMVAvoid.Add(aV);
      }
    }
    //
    bRebuild = Standard_False;
    aLEValid.Clear();
    aMFence.Clear();
    //
    aItLF.Initialize(aLFIm);
    for (; aItLF.More(); aItLF.Next()) {
      const TopoDS_Shape& aFIm = aItLF.Value();
      aExp.Init(aFIm, TopAbs_EDGE);
      for (; aExp.More(); aExp.Next()) {
        const TopoDS_Edge& anEIm = TopoDS::Edge(aExp.Current());
        if (!theInvEdges.Contains(anEIm)) {
          if (aMFence.Add(anEIm)) {
            aLEValid.Append(anEIm);
          }
        }
        //
        if (!bRebuild) {
          bRebuild = aMEReb.Contains(anEIm);
        }
        //
        if (!bRebuild) {
          // check vertices
          TopExp_Explorer aExpV(anEIm, TopAbs_VERTEX);
          for (; aExpV.More() && !bRebuild; aExpV.Next()) {
            const TopoDS_Shape& aV = aExpV.Current();
            if (!aMVAvoid.Contains(aV)) {
              bRebuild = aMEReb.Contains(aV);
            }
          }
        }
      }
    }
    //
    if (!bRebuild) {
      bRebuild = aLFIm.Extent() && theInvFaces.Contains(aF);
      if (bRebuild) {
        theFSelfRebAvoid.Add(aF);
      }
    }
    //
    if (bRebuild) {
      theFToRebuild.Add(aF, aLEValid);
    }
  }
}

//=======================================================================
//function : RebuildFaces
//purpose  : 
//=======================================================================
void RebuildFaces(const TopTools_IndexedDataMapOfShapeListOfShape& theFToRebuild,
                  const TopTools_MapOfShape& theFSelfRebAvoid,
                  TopTools_IndexedDataMapOfShapeListOfShape& theDMFFIm,
                  TopTools_DataMapOfShapeListOfShape& theEdgesOrigins,
                  TopTools_DataMapOfShapeShape& theFacesOrigins,
                  TopTools_DataMapOfShapeListOfShape& theOEImages,
                  TopTools_DataMapOfShapeListOfShape& theOEOrigins,
                  TopTools_MapOfShape& theLastInvEdges,
                  TopTools_IndexedMapOfShape& theEdgesToAvoid,
                  TopTools_IndexedMapOfShape& theInvEdges,
                  TopTools_IndexedMapOfShape& theValidEdges,
                  TopTools_DataMapOfShapeInteger& theAlreadyInvFaces,
                  TopTools_IndexedDataMapOfShapeListOfShape& theInvFaces,
                  const TopTools_DataMapOfShapeShape& theArtInvFaces,
                  TopTools_MapOfShape& theVertsToAvoid,
                  TopTools_DataMapOfShapeShape& theETrimEInf,
                  Handle(BRepAlgo_AsDes)& theAsDes)
{
  TopTools_MapOfShape aModifiedEdges;
  //
  // 1. Intersect faces
  IntersectFaces(theFToRebuild, theFSelfRebAvoid, theDMFFIm, theEdgesOrigins, theOEImages,
                 theOEOrigins, theInvEdges, theValidEdges, theInvFaces, theArtInvFaces,
                 theVertsToAvoid, theETrimEInf, aModifiedEdges, theAsDes);
  //
  // 2. Repeat steps to build the correct faces
  BuildSplitsOfInvFaces(theFToRebuild, aModifiedEdges, theDMFFIm, theEdgesOrigins,
                        theFacesOrigins, theOEImages, theOEOrigins, theLastInvEdges,
                        theEdgesToAvoid, theVertsToAvoid, theAlreadyInvFaces, theValidEdges, 
                        theETrimEInf, theAsDes);
}

//=======================================================================
//function : IntersectFaces
//purpose  : 
//=======================================================================
void IntersectFaces(const TopTools_IndexedDataMapOfShapeListOfShape& theFToRebuild,
                    const TopTools_MapOfShape& theFSelfRebAvoid,
                    TopTools_IndexedDataMapOfShapeListOfShape& theDMFFIm,
                    TopTools_DataMapOfShapeListOfShape& theEdgesOrigins,
                    TopTools_DataMapOfShapeListOfShape& theOEImages,
                    TopTools_DataMapOfShapeListOfShape& theOEOrigins,
                    TopTools_IndexedMapOfShape& theInvEdges,
                    TopTools_IndexedMapOfShape& theValidEdges,
                    TopTools_IndexedDataMapOfShapeListOfShape& theInvFaces,
                    const TopTools_DataMapOfShapeShape& theArtInvFaces,
                    TopTools_MapOfShape& theVertsToAvoid,
                    TopTools_DataMapOfShapeShape& theETrimEInf,
                    TopTools_MapOfShape& theModifiedEdges,
                    Handle(BRepAlgo_AsDes)& theAsDes)
{
  Standard_Integer aNbFR = theFToRebuild.Extent();
  if (!aNbFR) {
    return;
  }
  //
  Standard_Integer i, j, k, aNbInv;
  TopTools_ListIteratorOfListOfShape aItLF, aItLE;
  TopExp_Explorer aExp;
  //
  // get vertices from invalid edges
  TopTools_MapOfShape aMVInv, aMVInvAll;
  aNbInv = theInvEdges.Extent();
  for (i = 1; i <= aNbInv; ++i) {
    const TopoDS_Shape& aEInv = theInvEdges(i);
    Standard_Boolean bValid = theValidEdges.Contains(aEInv);
    aExp.Init(aEInv, TopAbs_VERTEX);
    for (; aExp.More(); aExp.Next()) {
      const TopoDS_Shape& aV = aExp.Current();
      aMVInvAll.Add(aV);
      if (!bValid) {
        aMVInv.Add(aV);
      }
    }
  }
  //
  Standard_Boolean bLookVertToAvoid = (aMVInv.Extent() > 0);
  //
  TopTools_DataMapOfShapeListOfShape aDMSF, aMDone, aMEInfETrim, aDMVEFull;
  TopTools_IndexedDataMapOfShapeListOfShape aFLE, aDMEFInv;
  //
  // Add all faces to rebuild to outgoing map <aFLE>,
  // plus link edges and vertices to the faces to
  // define intersection faces
  PrepareFacesForIntersection(theFToRebuild, theDMFFIm, theInvFaces, 
                              theArtInvFaces, aFLE, aMDone, aDMSF, aMEInfETrim, 
                              aDMVEFull, theETrimEInf, aDMEFInv, bLookVertToAvoid);

  // Find vertices to avoid while trimming the edges.
  // These vertices are taken from the invalid edges common between
  // splits of different invalid, but not artificially, faces.
  // Additional condition for these vertices is that all 
  // edges adjacent to this vertex must be either invalid
  // or contained in invalid faces
  TopTools_MapOfShape aMVRInv = theVertsToAvoid;
  FindVerticesToAvoid(aDMEFInv, aDMVEFull, theInvEdges, theValidEdges, aMVRInv);
  //
  // The faces should be intersected selectively - 
  // intersect only faces neighboring to the same invalid face
  // and connected to its invalid edges and its valid edges with free bounds
  TopTools_MapOfShape aMEAlone, aMFence;
  TopTools_IndexedDataMapOfShapeListOfShape aDMVEVal;
  //
  aNbInv = theInvFaces.Extent();
  for (i = 1; i <= aNbInv; ++i) {
    const TopTools_ListOfShape& aLFInv = theInvFaces(i);
    aItLF.Initialize(aLFInv);
    for (; aItLF.More(); aItLF.Next()) {
      const TopoDS_Shape& aFInv = aItLF.Value();
      aExp.Init(aFInv, TopAbs_EDGE);
      for (; aExp.More(); aExp.Next()) {
        const TopoDS_Shape& aE = aExp.Current();
        if (!theInvEdges.Contains(aE) && aMFence.Add(aE)) {
          TopExp::MapShapesAndAncestors(aE, TopAbs_VERTEX, TopAbs_EDGE, aDMVEVal);
        }
      }
    }
  }
  //
  // collect edges with free vertices
  Standard_Integer aNbV = aDMVEVal.Extent();
  for (i = 1; i <= aNbV; ++i) {
    const TopTools_ListOfShape& aLEV = aDMVEVal(i);
    if (aLEV.Extent() == 1) {
      const TopoDS_Shape& aE = aLEV.First();
      aMEAlone.Add(aE);
    }
  }
  //
  // Bounding vertices of not trimmed edges
  TopTools_MapOfShape aMVBounds;
  // Save connections between not trimmed edge and its trimmed parts
  TopTools_DataMapOfShapeListOfShape aDMEETrim;
  // Splits of the new edges
  TopTools_DataMapOfShapeListOfShape aEImages;
  BRep_Builder aBB;
  //
  aNbInv = theInvFaces.Extent();
  for (k = 1; k <= aNbInv; ++k) {
    const TopoDS_Shape& aFInv = theInvFaces.FindKey(k);
    Standard_Boolean bSelfRebAvoid = theFSelfRebAvoid.Contains(aFInv);
    const TopTools_ListOfShape& aLFInv = theInvFaces(k);
    //
    BOPCol_ListOfShape aLCB;
    if (aLFInv.Extent() > 1) {
      // make compound of invalid faces
      TopoDS_Compound aCFInv;
      aBB.MakeCompound(aCFInv);
      //
      TopTools_ListIteratorOfListOfShape aIt(aLFInv);
      for (; aIt.More(); aIt.Next()) {
        const TopoDS_Shape& aFIm = aIt.Value();
        aBB.Add(aCFInv, aFIm);
      }
      //
      // make connexity blocks
      BOPTools_AlgoTools::MakeConnexityBlocks(aCFInv, TopAbs_EDGE, TopAbs_FACE, aLCB);
    }
    else {
      aLCB.Append(aLFInv.First());
    }
    //
    BOPCol_ListIteratorOfListOfShape aItLCB(aLCB);
    for (; aItLCB.More(); aItLCB.Next()) {
      const TopoDS_Shape& aCBInv = aItLCB.Value();
      //
      TopTools_MapOfShape aMEFence;
      //
      TopoDS_Compound aCBE;
      aBB.MakeCompound(aCBE);
      //
      TopExp_Explorer aExp(aCBInv, TopAbs_EDGE);
      for (; aExp.More(); aExp.Next()) {
        const TopoDS_Shape& aE = aExp.Current();
        if (theInvEdges.Contains(aE) || aMEAlone.Contains(aE)) {
          if (aMEFence.Add(aE)) {
            aBB.Add(aCBE, aE);
          }
        }
      }
      //
      // make connexity blocks of edges
      BOPCol_ListOfShape aLCBE;
      BOPTools_AlgoTools::MakeConnexityBlocks(aCBE, TopAbs_VERTEX, TopAbs_EDGE, aLCBE);
      //
      BOPCol_ListIteratorOfListOfShape aItLCBE(aLCBE);
      for (; aItLCBE.More(); aItLCBE.Next()) {
        const TopoDS_Shape& aCBELoc = aItLCBE.Value();
        //
        // map of edges and vertices of processing invalidity
        TopTools_IndexedMapOfShape aME;
        // map of vertices to trim the new edges
        TopTools_IndexedMapOfShape  aMECV;
        TopExp::MapShapes(aCBELoc, TopAbs_EDGE, aME);
        aMECV = aME;
        TopExp::MapShapes(aCBELoc, TopAbs_VERTEX, aME);
        //
        // Using the map <aME> find chain of faces to be intersected;
        //
        // faces for intersection
        TopTools_IndexedMapOfShape aMFInt;
        // splits of faces for intersection
        TopTools_ListOfShape aLFInt;
        // faces to avoid intersection
        TopTools_IndexedMapOfShape aMFAvoid;
        //
        FindFacesForIntersection(aFInv, aME, theDMFFIm, theArtInvFaces, 
                                 aDMSF, aMVInvAll, aMFAvoid, aMFInt, aLFInt);
        //
        // intersect the faces, but do not intersect the invalid ones
        // among each other (except for the artificially invalid faces)
        TopTools_IndexedMapOfShape aMEToInt;
        Standard_Integer aNb = aMFInt.Extent();
        for (i = 1; i <= aNb; ++i) {
          const TopoDS_Face& aFi = TopoDS::Face(aMFInt(i));
          if (bSelfRebAvoid && aFi.IsSame(aFInv)) {
            continue;
          }
          //
          const TopTools_ListOfShape& aLFImi = theDMFFIm.FindFromKey(aFi);
          //
          TopTools_ListOfShape& aLFEi = aFLE.ChangeFromKey(aFi);
          //
          TopTools_ListOfShape& aLFDone = aMDone.ChangeFind(aFi);
          //
          for (j = i + 1; j <= aNb; ++j) {
            const TopoDS_Face& aFj = TopoDS::Face(aMFInt(j));
            if (bSelfRebAvoid && aFj.IsSame(aFInv)) {
              continue;
            }
            //
            const TopTools_ListOfShape& aLFImj = theDMFFIm.FindFromKey(aFj);
            //
            TopTools_ListOfShape& aLFEj = aFLE.ChangeFromKey(aFj);
            //
            // if there are some common edges between faces
            // we should use these edges and do not intersect again.
            TopTools_ListOfShape aLEC;
            FindCommonParts(aLFImi, aLFImj, aLEC);
            //
            if (aLEC.Extent()) {
              ProcessCommonEdges(aLEC, theInvEdges, theValidEdges, aME, theETrimEInf,
                                 aMEInfETrim, aMECV, aDMEETrim, aLFEi, aLFEj, aMEToInt);
              //
              // no need to intersect if we have common edges between faces
              continue;
            }
            //
            // check if both these faces are invalid and sharing edges
            if (theInvFaces.Contains(aFi) && theInvFaces.Contains(aFj) &&
              !theArtInvFaces.IsBound(aFi) && !theArtInvFaces.IsBound(aFj)) {
              continue;
            }
            //
            // check if these two faces have already been treated
            aItLE.Initialize(aLFDone);
            for (; aItLE.More(); aItLE.Next()) {
              const TopoDS_Shape& aF = aItLE.Value();
              if (aF.IsSame(aFj)) {
                break;
              }
            }
            //
            if (aItLE.More()) {
              // use intersection line obtained on the previous steps
              // plus, find new origins for these lines
              UpdateIntersectedFaces(aFInv, aFi, aFj, aLFInv, aLFImi, aLFImj,
                                     aLFEi, aLFEj, theEdgesOrigins, aMEToInt);
              continue;
            }
            //
            if (aMFAvoid.Contains(aFi) || aMFAvoid.Contains(aFj)) {
              continue;
            }
            //
            aLFDone.Append(aFj);
            aMDone.ChangeFind(aFj).Append(aFi);
            //
            IntersectFaces(aFInv, aFi, aFj, aLFInv, aLFImi, aLFImj, 
                           aLFEi, aLFEj, theEdgesOrigins, aMECV, aMEToInt);
          }
        }
        //
        // intersect and trim edges for this chain
        IntersectAndTrimEdges(theFToRebuild, aMFInt, aMEToInt, aDMEETrim,
                              aME, aMECV, aMVInv, aMVRInv, aMVBounds, aEImages);
      }
    }
  }
  //
  // filter the obtained edges
  UpdateValidEdges(theFToRebuild, theDMFFIm, theInvEdges, theEdgesOrigins, theOEImages, 
                   theOEOrigins, aFLE, theVertsToAvoid, theETrimEInf, aEImages,
                   aDMEETrim, aMVBounds, theModifiedEdges, theAsDes);
}

//=======================================================================
//function : IntersectAndTrimEdges
//purpose  : 
//=======================================================================
void IntersectAndTrimEdges(const TopTools_IndexedDataMapOfShapeListOfShape& theFToRebuild,
                           const TopTools_IndexedMapOfShape& theMFInt,
                           const TopTools_IndexedMapOfShape& theMEInt,
                           const TopTools_DataMapOfShapeListOfShape& theDMEETrim,
                           const TopTools_IndexedMapOfShape& theMSInv,
                           const TopTools_IndexedMapOfShape& theMVE,
                           const TopTools_MapOfShape& theVertsToAvoid,
                           const TopTools_MapOfShape& theNewVertsToAvoid,
                           TopTools_MapOfShape& theMVBounds,
                           TopTools_DataMapOfShapeListOfShape& theEImages)
{
  Standard_Integer i, aNb = theMEInt.Extent();
  if (!aNb) {
    return;
  }
  //
  BOPCol_ListOfShape aLArgs;
  TopTools_MapOfShape aMFence;
  TopTools_ListIteratorOfListOfShape aIt, aIt1;
  TopExp_Explorer aExp;
  //
  // get vertices from the splits of intersected faces
  // vertices are taken from the edges close to invalidity
  //
  TopTools_IndexedDataMapOfShapeListOfShape aDMVE;
  aNb = theMFInt.Extent();
  for (i = 1; i <= aNb; ++i) {
    const TopoDS_Shape& aF = theMFInt(i);
    const TopTools_ListOfShape& aLE = theFToRebuild.FindFromKey(aF);
    //
    aIt.Initialize(aLE);
    for (; aIt.More(); aIt.Next()) {
      const TopoDS_Shape& aE = aIt.Value();
      TopExp::MapShapesAndAncestors(aE, TopAbs_VERTEX, TopAbs_EDGE, aDMVE);
      //
      aExp.Init(aE, TopAbs_VERTEX);
      for (; aExp.More(); aExp.Next()) {
        const TopoDS_Shape& aV1 = aExp.Current();
        if (!theVertsToAvoid.Contains(aV1) && theMVE.Contains(aV1) && aMFence.Add(aV1)) {
          aLArgs.Append(aV1);
        }
      }
    }
  }
  //
  aNb = theMSInv.Extent();
  for (i = 1; i <= aNb; ++i) {
    const TopoDS_Shape& aV = theMSInv(i);
    if (aV.ShapeType() != TopAbs_VERTEX) {
      continue;
    }
    //
    if (!aDMVE.Contains(aV)) {
      continue;
    }
    //
    const TopTools_ListOfShape& aLVE = aDMVE.FindFromKey(aV);
    aIt.Initialize(aLVE);
    for (; aIt.More(); aIt.Next()) {
      const TopoDS_Shape& aE = aIt.Value();
      //
      aExp.Init(aE, TopAbs_VERTEX);
      for (; aExp.More(); aExp.Next()) {
        const TopoDS_Shape& aV1 = aExp.Current();
        if (!theVertsToAvoid.Contains(aV1) && aMFence.Add(aV1)) {
          aLArgs.Append(aV1);
        }
      }
    }
  }
  //
  // bounding vertices of untrimmed edges
  TopTools_ListOfShape aLVBounds;
  // new intersection edges
  TopTools_ListOfShape aLENew;
  // get edges to intersect
  TopTools_ListOfShape aLEInt;
  aNb = theMEInt.Extent();
  for (i = 1; i <= aNb; ++i) {
    const TopoDS_Shape& aE = theMEInt(i);
    if (!aMFence.Add(aE)) {
      continue;
    }
    //
    if (!theDMEETrim.IsBound(aE)) {
      aLENew.Append(aE);
    }
    //
    aLEInt.Append(aE);
    aLArgs.Append(aE);
    //
    aExp.Init(aE, TopAbs_VERTEX);
    for (; aExp.More(); aExp.Next()) {
      const TopoDS_Shape& aV = aExp.Current();
      aLVBounds.Append(aV);
    }
  }
  //
  // Intersect Edges
  BOPAlgo_Builder aGF;
  //
  aGF.SetArguments(aLArgs);
  aGF.Perform();
  if (aGF.ErrorStatus())
    return;
  //
  // update vertices to avoid with sd vertices
  aIt.Initialize(aLVBounds);
  for (; aIt.More(); aIt.Next()) {
    const TopoDS_Shape& aV = aIt.Value();
    const TopTools_ListOfShape& aLVIm = aGF.Modified(aV);
    if (aLVIm.IsEmpty()) {
      theMVBounds.Add(aV);
    }
    else {
      const TopoDS_Shape& aVIm = aLVIm.First();
      theMVBounds.Add(aVIm);
    }
  }
  //
  // find invalid splits of edges
  TopTools_MapOfShape aMEInv;
  GetInvalidEdges(aGF, /*aLENew, */theNewVertsToAvoid, theMVBounds, aMEInv);
  //
  // remove the splits containing veritces from invalid edges
  aIt.Initialize(aLEInt);
  for (; aIt.More(); aIt.Next()) {
    const TopoDS_Shape& aE = aIt.Value();
    //
    TopTools_ListOfShape aLEIm = aGF.Modified(aE);
    if (aLEIm.IsEmpty()) {
      continue;
    }
    //
    aIt1.Initialize(aLEIm);
    for (; aIt1.More(); ) {
      const TopoDS_Shape& aEIm = aIt1.Value();
      //
      if (aMEInv.Contains(aEIm)) {
        aLEIm.Remove(aIt1);
      }
      else {
        aIt1.Next();
      }
    }
    //
    if (aLEIm.Extent()) {
      if (theEImages.IsBound(aE)) {
        theEImages.ChangeFind(aE).Append(aLEIm);
      }
      else {
        theEImages.Bind(aE, aLEIm);
      }
    }
  }
}

//=======================================================================
//function : UpdateValidEdges
//purpose  : 
//=======================================================================
void UpdateValidEdges(const TopTools_IndexedDataMapOfShapeListOfShape& theFToRebuild,
                      const TopTools_IndexedDataMapOfShapeListOfShape& theDMFFIm,
                      const TopTools_IndexedMapOfShape& theInvEdges,
                      TopTools_DataMapOfShapeListOfShape& theEdgesOrigins,
                      TopTools_DataMapOfShapeListOfShape& theOEImages,
                      TopTools_DataMapOfShapeListOfShape& theOEOrigins,
                      TopTools_IndexedDataMapOfShapeListOfShape& theFLE,
                      TopTools_MapOfShape& theVertsToAvoid,
                      TopTools_DataMapOfShapeShape& theETrimEInf,
                      TopTools_DataMapOfShapeListOfShape& theEImages,
                      TopTools_DataMapOfShapeListOfShape& theEETrim,
                      TopTools_MapOfShape& theMVBounds,
                      TopTools_MapOfShape& theModifiedEdges,
                      Handle(BRepAlgo_AsDes)& theAsDes)
{
  //
  // update images and origins of edges, plus update AsDes
  Standard_Integer i, aNb;
  TopTools_ListOfShape aLE;
  TopTools_DataMapOfShapeListOfShape aMELF;
  TopTools_ListIteratorOfListOfShape aIt, aIt1;
  //
  aNb = theFLE.Extent();
  for (i = 1; i <= aNb; ++i) {
    const TopoDS_Face& aF = TopoDS::Face(theFLE.FindKey(i));
    const TopTools_ListOfShape& aLEInt = theFLE(i);
    //
    aIt.Initialize(aLEInt);
    for (; aIt.More(); aIt.Next()) {
      const TopoDS_Shape& aE = aIt.Value();
      if (aMELF.IsBound(aE)) {
        aMELF.ChangeFind(aE).Append(aF);
      }
      else {
        aLE.Append(aE);
        TopTools_ListOfShape aLF;
        aLF.Append(aF);
        aMELF.Bind(aE, aLF);
      }
    }
  }
  //
  if (aLE.IsEmpty()) {
    return;
  }
  //
  // bounding edges, that are going to be replaced
  TopTools_MapOfShape aMEB;
  //
  // new intersection edges
  TopTools_ListOfShape aLENew;
  TopTools_MapOfShape aMENew;
  // map of old vertices
  TopTools_MapOfShape aMVOld;
  //
  // trim the new intersection edges
  BOPCol_ListOfShape aLA;
  TrimNewIntersectionEdges(aLE, theEImages, theEETrim, aMEB,
                           aMVOld, theMVBounds, aLENew, aLA);
  //
  // intersect the splits among themselves
  BOPAlgo_Builder aGF;
  TopTools_ListOfShape aLAValid, aLABounds;
  //
  if (aLA.Extent()) {
    TopoDS_Shape aSplits1;
    if (aLA.Extent() > 1) {
      // fuse all images to avoid self-intersections
      IntersectEdges(aLA, aLE, aLENew, theEImages, theVertsToAvoid, 
                     theMVBounds, theModifiedEdges, aMENew, aSplits1);
    }
    else {
      aSplits1 = aLA.First();
    }
    //
    TopoDS_Shape aFilterBounds;
    GetBounds(theFToRebuild, theDMFFIm, aMEB, aFilterBounds);
    //
    // intersect splits and bounds and remove those splits which have pure E/E intersection
    TopTools_MapOfShape aMEInv;
    GetInvalidEdgesByBounds(aSplits1, aFilterBounds, theFToRebuild, theDMFFIm, 
                            theInvEdges, aMVOld, aMENew, theVertsToAvoid, aMEInv);
    //
    // get valid edges only
    TopoDS_Shape aSplits;
    // clear images from newly found invalid edges
    if (aMEInv.Extent()) {
      TopoDS_Compound aSp;
      BRep_Builder aBB;
      TopTools_MapOfShape aMFence;
      aBB.MakeCompound(aSp);
      //
      aIt.Initialize(aLE);
      for (; aIt.More(); aIt.Next()) {
        const TopoDS_Shape& aE = aIt.Value();
        //
        if (!theEImages.IsBound(aE)) {
          continue;
        }
        //
        TopTools_ListOfShape& aLEIm = theEImages.ChangeFind(aE);
        TopTools_ListIteratorOfListOfShape aItLEIm(aLEIm);
        for (; aItLEIm.More(); ) {
          const TopoDS_Shape& aEIm = aItLEIm.Value();
          if (aMEInv.Contains(aEIm)) {
            aLEIm.Remove(aItLEIm);
          }
          else {
            if (aMFence.Add(aEIm)) {
              aBB.Add(aSp, aEIm);
            }
            aItLEIm.Next();
          }
        }
        //
        if (aLEIm.IsEmpty()) {
          theEImages.UnBind(aE);
        }
      }
      aSplits = aSp;
    }
    else {
      aSplits = aSplits1;
    }
    //
    TopoDS_Shape aBounds;
    GetBoundsToUpdate(theFToRebuild, theOEImages, theOEOrigins,
                      aMEB, aLABounds, aLAValid, aBounds, theAsDes);
    //
    aGF.AddArgument(aSplits);
    aGF.AddArgument(aBounds);
    aGF.Perform();
    // update splits
    UpdateImages(aLE, theEImages, aGF, theModifiedEdges);
  }
  //
  // update intersection edges
  UpdateNewIntersectionEdges(aLE, aMELF, theEImages, theEdgesOrigins, theOEImages, 
    theOEOrigins, theETrimEInf, theEETrim, theModifiedEdges, theAsDes);
  //
  // update bounds
  UpdateImages(aLAValid, theOEImages, aGF, theModifiedEdges);
  UpdateOrigins(aLABounds, theOEOrigins, aGF);
  UpdateOrigins(aLABounds, theEdgesOrigins, aGF);
  UpdateIntersectedEdges(aLABounds, theETrimEInf, aGF);
}

//=======================================================================
//function : GetInvalidEdges
//purpose  : 
//=======================================================================
void GetInvalidEdges(BOPAlgo_Builder& theGF,
                     //const TopTools_ListOfShape& theLENew,
                     const TopTools_MapOfShape& theVertsToAvoid,
                     const TopTools_MapOfShape& theMVBounds,
                     TopTools_MapOfShape& theMEInv)
{
  TopTools_ListIteratorOfListOfShape aIt, aIt1;
  // get vertices created with intersection edges
  const TopoDS_Shape& aRes = theGF.Shape();
  TopTools_IndexedDataMapOfShapeListOfShape aDMVE;
  TopExp::MapShapesAndAncestors(aRes, TopAbs_VERTEX, TopAbs_EDGE, aDMVE);
  //
  const BOPDS_PDS& pDS = theGF.PDS();
  //
  // find invalid splits of edges
  // check if the vertex is invalid:
  // a. it may be the vertex SD with the vertices to avoid
  // b. or it may be the vertex which is created by the intersection 
  //    of only existing edges, i.e. no new intersection edges goes
  //    through this vertex
  //
  TopTools_MapOfShape aMVInv;
  Standard_Integer i, aNb = aDMVE.Extent();
  for (i = 1; i <= aNb; ++i) {
    const TopoDS_Vertex& aV = TopoDS::Vertex(aDMVE.FindKey(i));
    if (theMVBounds.Contains(aV)) {
      continue;
    }
    //
    Standard_Integer nV = pDS->Index(aV);
    if ((nV >= 0) && !pDS->IsNewShape(nV)) {
      continue;
    }
    //
    TopTools_MapIteratorOfMapOfShape aItM(theVertsToAvoid);
    for (; aItM.More(); aItM.Next()) {
      const TopoDS_Vertex& aVInv = *(TopoDS_Vertex*)&aItM.Key();
      Standard_Integer iFlag = BOPTools_AlgoTools::ComputeVV(aV, aVInv);
      if (!iFlag) {
        aMVInv.Add(aV);
        break;
      }
    }
    //
    if (aItM.More()) {
      const TopTools_ListOfShape& aLVE = aDMVE.FindFromKey(aV);
      aIt.Initialize(aLVE);
      for (; aIt.More(); aIt.Next()) {
        const TopoDS_Shape& aE = aIt.Value();
        theMEInv.Add(aE);
      }
    }
  }
  //
  /*// check the second condition now
  TopTools_IndexedMapOfShape aMVKeep;
  aIt.Initialize(theLENew);
  for (; aIt.More(); aIt.Next()) {
    const TopoDS_Shape& aE = aIt.Value();
    const TopTools_ListOfShape& aLEIm = theGF.Modified(aE);
    if (aLEIm.IsEmpty()) {
      if (!theMEInv.Contains(aE)) {
        TopExp::MapShapes(aE, TopAbs_VERTEX, aMVKeep);
      }
    }
    else {
      aIt1.Initialize(aLEIm);
      for (; aIt1.More(); aIt1.Next()) {
        const TopoDS_Shape& aEIm = aIt1.Value();
        if (!theMEInv.Contains(aEIm)) {
          TopExp::MapShapes(aEIm, TopAbs_VERTEX, aMVKeep);
        }
      }
    }
  }
  //
  for (i = 1; i <= aNb; ++i) {
    const TopoDS_Shape& aV = aDMVE.FindKey(i);
    if (theMVBounds.Contains(aV) || aMVInv.Contains(aV)) {
      continue;
    }
    //
    Standard_Integer nV = pDS->Index(aV);
    if ((nV >= 0) && !pDS->IsNewShape(nV)) {
      continue;
    }
    //
    if (!aMVKeep.Contains(aV)) {
      const TopTools_ListOfShape& aLVE = aDMVE.FindFromKey(aV);
      aIt.Initialize(aLVE);
      for (; aIt.More(); aIt.Next()) {
        const TopoDS_Shape& aE = aIt.Value();
        theMEInv.Add(aE);
      }
    }
  }*/
}

//=======================================================================
//function : BuildSplitsOfInvFaces
//purpose  : 
//=======================================================================
void BuildSplitsOfInvFaces(const TopTools_IndexedDataMapOfShapeListOfShape& theFToRebuild, 
                           const TopTools_MapOfShape& theModifiedEdges,
                           TopTools_IndexedDataMapOfShapeListOfShape& theDMFFIm,
                           TopTools_DataMapOfShapeListOfShape& theEdgesOrigins,
                           TopTools_DataMapOfShapeShape& theFacesOrigins,
                           TopTools_DataMapOfShapeListOfShape& theOEImages,
                           TopTools_DataMapOfShapeListOfShape& theOEOrigins,
                           TopTools_MapOfShape& theLastInvEdges,
                           TopTools_IndexedMapOfShape& theEdgesToAvoid,
                           TopTools_MapOfShape& theVertsToAvoid,
                           TopTools_DataMapOfShapeInteger& theAlreadyInvFaces,
                           TopTools_IndexedMapOfShape& theValidEdges,
                           TopTools_DataMapOfShapeShape& theETrimEInf, 
                           Handle(BRepAlgo_AsDes)& theAsDes)
{
  Standard_Integer aNb = theFToRebuild.Extent();
  if (!aNb) {
    return;
  }
  //
  TopTools_IndexedMapOfShape anInvEdges;
  TopTools_ListOfShape aLF;
  aNb = theDMFFIm.Extent();
  for (Standard_Integer i = 1; i <= aNb; ++i) {
    const TopoDS_Shape& aF = theDMFFIm.FindKey(i);
    aLF.Append(aF);
  }
  //
  // invalid faces
  TopTools_IndexedDataMapOfShapeListOfShape anInvFaces;
  //
  // artificially invalid faces
  TopTools_DataMapOfShapeShape anArtInvFaces;
  //
  BuildSplitsOfFaces(aLF, theAsDes, theModifiedEdges, theEdgesOrigins, theFacesOrigins, 
                     theOEImages, theOEOrigins, theLastInvEdges, theEdgesToAvoid, anInvEdges, theValidEdges, 
                     theAlreadyInvFaces, anInvFaces, anArtInvFaces, theDMFFIm);
  //
  if (anInvFaces.Extent()) {
    TopTools_IndexedDataMapOfShapeListOfShape aFToRebuild;
    TopTools_MapOfShape aFSelfRebAvoid;
    FindFacesToRebuild(theDMFFIm, anInvEdges, anInvFaces, aFToRebuild, aFSelfRebAvoid);
    //
    if (aFToRebuild.Extent()) {
      RebuildFaces(aFToRebuild, aFSelfRebAvoid, theDMFFIm, theEdgesOrigins, theFacesOrigins,
                   theOEImages, theOEOrigins, theLastInvEdges, theEdgesToAvoid, anInvEdges, theValidEdges,
                   theAlreadyInvFaces, anInvFaces, anArtInvFaces, theVertsToAvoid, theETrimEInf, theAsDes);
    }
  }
}

//=======================================================================
//function : GetEdges
//purpose  : 
//=======================================================================
Standard_Boolean GetEdges(const TopoDS_Face& theFace,
                          const Handle(BRepAlgo_AsDes)& theAsDes,
                          const TopTools_DataMapOfShapeListOfShape& theEImages,
                          const TopTools_MapOfShape& theLastInvEdges,
                          const TopTools_IndexedMapOfShape& theInvEdges,
                          Handle(IntTools_Context)& theCtx,
                          const TopTools_MapOfShape& theModifiedEdges,
                          TopoDS_Shape& theEdges,
                          TopTools_MapOfShape& theInv)
{
  Standard_Boolean bFound, bUpdate;
  TopTools_ListIteratorOfListOfShape aItLE, aItLEIm;
  TopTools_MapOfShape aMFBounds;
  TopoDS_Compound anEdges;
  BRep_Builder aBB;
  //
  bFound = Standard_False;
  bUpdate = Standard_False;
  aBB.MakeCompound(anEdges);
  //
  // get boundary edges
  TopExp_Explorer aExp(theFace, TopAbs_EDGE);
  for (; aExp.More(); aExp.Next()) {
    const TopoDS_Shape& aE = aExp.Current();
    if (theEImages.IsBound(aE)) {
      const TopTools_ListOfShape& aLEIm = theEImages.Find(aE);
      aItLE.Initialize(aLEIm);
      for (; aItLE.More(); aItLE.Next()) {
        const TopoDS_Shape& aEIm = aItLE.Value();
        aMFBounds.Add(aEIm);
      }
    }
    else {
      aMFBounds.Add(aE);
    }
  }
  //
  // the edges by which the offset face should be split
  const TopTools_ListOfShape& aLE = theAsDes->Descendant(theFace);
  aItLE.Initialize(aLE);
  for (; aItLE.More(); aItLE.Next()) {
    const TopoDS_Edge& aE = TopoDS::Edge(aItLE.Value());
    if (BRep_Tool::Degenerated(aE)) {
      continue;
    }
    //
    if (!bUpdate) {
      bUpdate = theModifiedEdges.Contains(aE);
    }
    //
    if (theEImages.IsBound(aE)) {
      const TopTools_ListOfShape& aLEIm = theEImages.Find(aE);
      aItLEIm.Initialize(aLEIm);
      for (; aItLEIm.More(); aItLEIm.Next()) {
        const TopoDS_Edge& aEIm = TopoDS::Edge(aItLEIm.Value());
        //
        if (theInvEdges.Contains(aEIm)) {
          theInv.Add(aEIm);
          if (!bUpdate) {
            bUpdate = theLastInvEdges.Contains(aEIm);
          }
          continue;
        }
        // check for micro edge
        if (ProcessMicroEdge(aEIm, theCtx)) {
          continue;
        }
        //
        aBB.Add(anEdges, aEIm);
        if (!bFound) {
          bFound = !aMFBounds.Contains(aEIm);
        }
        //
        if (!bUpdate) {
          bUpdate = theModifiedEdges.Contains(aEIm);
        }
      }
    }
    else {
      if (theInvEdges.Contains(aE)) {
        theInv.Add(aE);
        if (!bUpdate) {
          bUpdate = theLastInvEdges.Contains(aE);
        }
        continue;
      }
      //
      if (ProcessMicroEdge(aE, theCtx)) {
        continue;
      }
      aBB.Add(anEdges, aE);
      if (!bFound) {
        bFound = !aMFBounds.Contains(aE);
      }
    }
  }
  //
  theEdges = anEdges;
  return bFound && bUpdate;
}

//=======================================================================
//function : BuildSplitsOfFace
//purpose  : 
//=======================================================================
void BuildSplitsOfFace(const TopoDS_Face& theFace,
                       const TopoDS_Shape& theEdges,
                       Standard_Boolean bLimited,
                       TopTools_DataMapOfShapeShape& theFacesOrigins,
                       TopTools_ListOfShape& theLFImages)
{
  BOPAlgo_Builder aGF;
  //
  aGF.AddArgument(theFace);
  aGF.AddArgument(theEdges);
  aGF.Perform();
  if (aGF.ErrorStatus()) {
    return;
  }
  //
  // splits of the offset shape
  theLFImages = aGF.Modified(theFace);
  if (theLFImages.IsEmpty()) {
    return;
  }
  //
  if (!bLimited) {
    //
    // to overcome the often errors in trimming edges it is 
    // better to remove first the faces containing the boundaries
    // of the extended surfaces;
    Standard_Boolean bKeep;
    TopExp_Explorer aExp;
    TopTools_IndexedMapOfShape aME;
    //
    TopExp::MapShapes(theEdges, TopAbs_EDGE, aME);
    //
    TopTools_ListIteratorOfListOfShape aItLE(theLFImages);
    for (; aItLE.More();) {
      const TopoDS_Face& aFIm = *(TopoDS_Face*)&aItLE.Value();
      //
      aExp.Init(aFIm, TopAbs_EDGE);
      for (bKeep = Standard_True; aExp.More() && bKeep; aExp.Next()) {
        const TopoDS_Edge& aE = *(TopoDS_Edge*)&aExp.Current();
        //
        if (BRep_Tool::Degenerated(aE)) {
          continue;
        }
        //
        bKeep = aME.Contains(aE);
      }
      //
      if (bKeep) {
        aItLE.Next();
      }
      else {
        theLFImages.Remove(aItLE);
      }
    }
  }
  // update origins
  TopTools_ListIteratorOfListOfShape aIt(theLFImages);
  for (; aIt.More(); aIt.Next()) {
    const TopoDS_Shape& aFIm = aIt.Value();
    theFacesOrigins.Bind(aFIm, theFace);
  }
}

//=======================================================================
//function : NbPoints
//purpose  : 
//=======================================================================
Standard_Integer NbPoints(const TopoDS_Edge& theEdge)
{
  Standard_Integer aNbP;
  BRepAdaptor_Curve aBAC(theEdge);
  switch (aBAC.GetType()) {
  case GeomAbs_Line:
    aNbP = 1;
    break;
  default:
    aNbP = 11;
  }
  //
  return aNbP;
}

//=======================================================================
//function : UpdateIntersectedEdges
//purpose  : 
//=======================================================================
void UpdateIntersectedEdges(const TopTools_ListOfShape& theLA,
                            TopTools_DataMapOfShapeShape& theETrimEInf,
                            BOPAlgo_Builder& theB)
{
  TopTools_ListIteratorOfListOfShape aItA, aIt;
  //
  aItA.Initialize(theLA);
  for (; aItA.More(); aItA.Next()) {
    const TopoDS_Shape& aS = aItA.Value();
    //
    if (!theETrimEInf.IsBound(aS)) {
      continue;
    }
    //
    const TopTools_ListOfShape& aLSIm = theB.Modified(aS);
    if (aLSIm.IsEmpty()) {
      continue;
    }
    //
    const TopoDS_Shape& anEInf = theETrimEInf.Find(aS);
    //
    aIt.Initialize(aLSIm);
    for (; aIt.More(); aIt.Next()) {
      const TopoDS_Shape& anE = aIt.Value();
      if (!theETrimEInf.IsBound(anE)) {
        theETrimEInf.Bind(anE, anEInf);
      }
    }
  }
}

//=======================================================================
//function : FindCommonParts
//purpose  : 
//=======================================================================
void FindCommonParts(const TopTools_ListOfShape& theLF1,
                     const TopTools_ListOfShape& theLF2,
                     TopTools_ListOfShape& theLEC,
                     const TopAbs_ShapeEnum theType)
{
  TopExp_Explorer anExp;
  TopTools_IndexedMapOfShape aME1;
  TopTools_MapOfShape aMFence;
  TopTools_ListIteratorOfListOfShape aIt;
  //
  aIt.Initialize(theLF1);
  for (; aIt.More(); aIt.Next()) {
    const TopoDS_Shape& aF1 = aIt.Value();
    TopExp::MapShapes(aF1, theType, aME1);
  }
  //
  aIt.Initialize(theLF2);
  for (; aIt.More(); aIt.Next()) {
    const TopoDS_Shape& aF2 = aIt.Value();
    anExp.Init(aF2, theType);
    for(; anExp.More(); anExp.Next()) {
      const TopoDS_Shape& anE2 = anExp.Current();
      if (aME1.Contains(anE2)) {
        if (aMFence.Add(anE2)) {
          theLEC.Append(anE2);
        }
      }
    }
  }
}

//=======================================================================
//function : FindShape
//purpose  : 
//=======================================================================
Standard_Boolean FindShape(const TopoDS_Shape& theSWhat,
                           const TopoDS_Shape& theSWhere,
                           TopoDS_Shape& theRes)
{
  Standard_Boolean bFound = Standard_False;
  TopAbs_ShapeEnum aType = theSWhat.ShapeType();
  TopExp_Explorer aExp(theSWhere, aType);
  for (; aExp.More(); aExp.Next()) {
    const TopoDS_Shape& aS = aExp.Current();
    if (aS.IsSame(theSWhat)) {
      theRes = aS;
      bFound = Standard_True;
      break;
    }
  }
  return bFound;
}

//=======================================================================
//function : UpdateImages
//purpose  : 
//=======================================================================
void UpdateImages(const TopTools_ListOfShape& theLA,
                  TopTools_DataMapOfShapeListOfShape& theImages,
                  BOPAlgo_Builder& theGF,
                  TopTools_MapOfShape& theModified)
{
  TopTools_ListIteratorOfListOfShape aIt(theLA);
  for (; aIt.More(); aIt.Next()) {
    const TopoDS_Shape& aS = aIt.Value();
    //
    if (!theImages.IsBound(aS)) {
      const TopTools_ListOfShape& aLSIm = theGF.Modified(aS);
      if (aLSIm.Extent()) {
        theImages.Bind(aS, aLSIm);
        theModified.Add(aS);
      }
      continue;
    }
    //
    TopTools_MapOfShape aMFence;
    TopTools_ListOfShape aLSImNew;
    //
    Standard_Boolean bModified = Standard_False;
    //
    // check modifications of the images
    TopTools_ListOfShape& aLSIm = theImages.ChangeFind(aS);
    TopTools_ListIteratorOfListOfShape aIt1(aLSIm);
    for (; aIt1.More(); aIt1.Next()) {
      const TopoDS_Shape& aSIm = aIt1.Value();
      const TopTools_ListOfShape& aLSIm1 = theGF.Modified(aSIm);
      if (aLSIm1.IsEmpty()) {
        if (aMFence.Add(aSIm)) {
          aLSImNew.Append(aSIm);
        }
      }
      else {
        TopTools_ListIteratorOfListOfShape aIt2(aLSIm1);
        for (; aIt2.More(); aIt2.Next()) {
          const TopoDS_Shape& aSImIm = aIt2.Value();
          if (aMFence.Add(aSImIm)) {
            aLSImNew.Append(aSImIm);
          }
        }
        bModified = Standard_True;
      }
    }
    //
    if (bModified) {
      aLSIm = aLSImNew;
      theModified.Add(aS);
    }
  }
}

//=======================================================================
//function : UpdateOrigins
//purpose  : 
//=======================================================================
void UpdateOrigins(const TopTools_ListOfShape& theLA,
                   TopTools_DataMapOfShapeListOfShape& theOrigins,
                   BOPAlgo_Builder& theGF)
{
  TopTools_ListIteratorOfListOfShape aItA, aIt, aIt1;
  //
  aItA.Initialize(theLA);
  for (; aItA.More(); aItA.Next()) {
    const TopoDS_Shape& aS = aItA.Value();
    //
    const TopTools_ListOfShape& aLSIm = theGF.Modified(aS);
    if (aLSIm.IsEmpty()) {
      continue;
    }
    //
    TopTools_ListOfShape aLSO;
    Standard_Boolean bOr = theOrigins.IsBound(aS);
    TopTools_ListOfShape& aLS = bOr ? theOrigins.ChangeFind(aS) : aLSO;
    if (!bOr) {
      aLS.Append(aS);
    }
    //
    aIt.Initialize(aLSIm);
    for (; aIt.More(); aIt.Next()) {
      const TopoDS_Shape& aSIm = aIt.Value();
      //
      if (!theOrigins.IsBound(aSIm)) {
        theOrigins.Bind(aSIm, aLS);
        continue;
      }
      //
      TopTools_ListOfShape& aLS1 = theOrigins.ChangeFind(aSIm);
      aIt1.Initialize(aLS);
      for (; aIt1.More(); aIt1.Next()) {
        const TopoDS_Shape& aS1 = aIt1.Value();
        AppendToList(aLS1, aS1);
      }
    }
  }
}

//=======================================================================
//function : ProcessMicroEdge
//purpose  : 
//=======================================================================
Standard_Boolean ProcessMicroEdge(const TopoDS_Edge& theEdge,
                                  const Handle(IntTools_Context)& theCtx)
{
  TopoDS_Vertex aV1, aV2;
  TopExp::Vertices(theEdge, aV1, aV2);
  Standard_Boolean bNull = aV1.IsNull() || aV2.IsNull();
  if (bNull) {
    return Standard_False;
  }
  //
  Standard_Boolean bMicro;
  bMicro = BOPTools_AlgoTools::IsMicroEdge(theEdge, theCtx);
  if (bMicro) {
    BRepAdaptor_Curve aBAC(theEdge);
    if (aBAC.GetType() == GeomAbs_Line) {
      BRep_Builder aBB;
      Standard_Real aLen = CPnts_AbscissaPoint::Length(aBAC);
      //
      aBB.UpdateVertex(aV1, aLen/2.);
      aBB.UpdateVertex(aV2, aLen/2.);
    }
  }
  //
  return bMicro;
}

//=======================================================================
//function : FindInvalidEdges
//purpose  : Looking for invalid edges
//=======================================================================
void FindInvalidEdges(const TopoDS_Face& theF,
                      const TopTools_ListOfShape& theLFImages,
                      const TopTools_DataMapOfShapeListOfShape& theEdgesOrigins,
                      const TopTools_DataMapOfShapeShape& theFacesOrigins,
                      const TopTools_DataMapOfShapeListOfShape& theOEImages,
                      const TopTools_DataMapOfShapeListOfShape& theOEOrigins,
                      TopTools_IndexedMapOfShape& theInvEdges,
                      TopTools_IndexedMapOfShape& theValidEdges,
                      TopTools_DataMapOfShapeListOfShape& theDMFLVE,
                      TopTools_DataMapOfShapeListOfShape& theDMFLNE,
                      TopTools_DataMapOfShapeListOfShape& theDMFLIE,
                      TopTools_MapOfShape& theMEInverted)
{
  // Edge is considered as invalid in the following cases:
  // 1. Its orientation on the face has changed comparing to the originals edge and face;
  // 2. The vertices of the edge have changed places comparing to the originals edge and face.
  Standard_Integer i, aNb;
  Standard_Real aT, aDt, aCos, aT1, aT2;
  gp_Pnt aP;
  gp_Vec aV;
  //
  const TopoDS_Face& aFOr = *(TopoDS_Face*)&theFacesOrigins.Find(theF);
  //
  TopTools_IndexedMapOfShape aME;
  TopTools_MapOfShape aMEVal;
  TopTools_ListOfShape aLEN, aLEInv;
  //
  // prepare map for checking the second condition
  TopoDS_Compound aCF;
  BRep_Builder aBB;
  //
  aBB.MakeCompound(aCF);
  //
  TopTools_ListIteratorOfListOfShape aItLF(theLFImages);
  for (; aItLF.More(); aItLF.Next()) {
    const TopoDS_Face& aFIm = *(TopoDS_Face*)&aItLF.Value();
    aBB.Add(aCF, aFIm);
  }
  //
  TopExp::MapShapes(aCF, TopAbs_EDGE, aME);
  //
  TopTools_IndexedDataMapOfShapeListOfShape aDMVE;
  TopExp::MapShapesAndAncestors(aCF, TopAbs_VERTEX, TopAbs_EDGE, aDMVE);
  //
  TopTools_IndexedDataMapOfShapeListOfShape aDMEF;
  TopExp::MapShapesAndAncestors(aCF, TopAbs_EDGE, TopAbs_FACE, aDMEF);
  //
  aItLF.Initialize(theLFImages);
  for (; aItLF.More(); aItLF.Next()) {
    const TopoDS_Face& aFIm = *(TopoDS_Face*)&aItLF.Value();
    //
    TopTools_ListOfShape aLVE;
    //const TopoDS_Wire& aWIm = BRepTools::OuterWire(aFIm);
    TopExp_Explorer aExp(aFIm, TopAbs_EDGE);
    for (; aExp.More(); aExp.Next()) {
      const TopoDS_Edge& aEIm = *(TopoDS_Edge*)&aExp.Current();
      //
      if (aEIm.Orientation() == TopAbs_INTERNAL) {
        continue;
      }
      //
      if (BRep_Tool::Degenerated(aEIm)) {
        continue;
      }
      //
      if (!theEdgesOrigins.IsBound(aEIm)) {
        continue;
      }
      //
      const TopTools_ListOfShape& aLEOr = theEdgesOrigins.Find(aEIm);
      if (aLEOr.IsEmpty()) {
        continue;
      }
      //
      const TopoDS_Shape& aSOr = aLEOr.First();
      if (aSOr.ShapeType() != TopAbs_EDGE) {
        continue;
      }
      //
      TopTools_MapOfShape aME, aMV;
      Standard_Integer aNbE, aNbV;
      Standard_Boolean bInvalid = Standard_False;
      Standard_Integer aNbPoint = NbPoints(aEIm);
      TopoDS_Edge aEOrF;
      TopTools_ListIteratorOfListOfShape aItLEO(aLEOr);
      for (; aItLEO.More(); aItLEO.Next()) {
        const TopoDS_Shape& aSOr = aItLEO.Value();
        if (aSOr.ShapeType() != TopAbs_EDGE) {
          continue;
        }
        //
        const TopoDS_Edge& aEOr = *(TopoDS_Edge*)&aSOr;
        if (!FindShape(aEOr, aFOr, aEOrF)) {
          continue;
        }
        //
        // Check orientations of the image edge and original edge.
        // In case the 3d curves are having the same direction the orientations 
        // must be the same. Otherwise the orientations should also be different.
        //
        TopAbs_Orientation anOrEIm = aEIm.Orientation();
        TopAbs_Orientation anOrEOr = aEOrF.Orientation();
        //
        // get sum of tangents of each curve
        gp_Vec aVSum1(0., 0., 0.), aVSum2(0., 0., 0.);
        for (i = 0; i < 2; ++i) {
          const TopoDS_Edge& aE = !i ? aEIm : aEOrF;
          gp_Vec& aVSum = !i ? aVSum1 : aVSum2;
          const Handle(Geom_Curve)& aC = BRep_Tool::Curve(aE, aT1, aT2);
          //
          aDt = (aT2 - aT1) / aNbPoint;
          aT = aT1;
          while (aT <= aT2) {
            aC->D1(aT, aP, aV);
            aVSum += aV.Normalized();
            aT += aDt;
          }
        }
        //
        aVSum1.Normalize();
        aVSum2.Normalize();
        //
        aCos = aVSum1.Dot(aVSum2);
        if (Abs(aCos) < 0.9999) {
          continue;
        }
        //
        aME.Add(aEOrF);
        TopExp_Explorer aExpE(aEOrF, TopAbs_VERTEX);
        for (; aExpE.More(); aExpE.Next()) {
          const TopoDS_Shape& aV = aExpE.Current();
          aMV.Add(aV);
        }
        //
        if ((aCos <= 0.) && (anOrEIm == anOrEOr) ||
            (aCos >= 0.) && (anOrEIm != anOrEOr)) {
          bInvalid = Standard_True;
        }
      }
      //
      aNbE = aME.Extent();
      if (!aNbE) {
        // not checked
        continue;
      }
      //
      aNbV = aMV.Extent();
      if ((aNbE > 1) && (aNbV == 2*aNbE)) {
        continue;
      }
      //
      if (bInvalid) {
        theInvEdges.Add(aEIm);
        aLEInv.Append(aEIm);
        continue;
      }
      //
      theValidEdges.Add(aEIm);
      aLVE.Append(aEIm);
      aMEVal.Add(aEIm);
      //
      // check if the edge has been inverted
      if (aDMEF.FindFromKey(aEIm).Extent() > 1) {
        continue;
      }
      //
      if (theMEInverted.Contains(aEIm)) {
        continue;
      }
      //
      TopoDS_Vertex aVI1, aVI2; // vertices on the offset edge
      TopoDS_Vertex aVO1, aVO2; // vertices on the original edge
      //
      // find vertices of the offset shape
      TopExp::Vertices(aEIm, aVI1, aVI2);
      //
      TopTools_ListOfShape aLEImages;
      if (theOEOrigins.IsBound(aEIm)) {
        TopoDS_Wire anImages;
        aBB.MakeWire(anImages);
        //
        TopTools_MapOfShape aMImFence;
        const TopTools_ListOfShape& aLOffsetOr = theOEOrigins.Find(aEIm);
        TopTools_ListIteratorOfListOfShape aItOffset(aLOffsetOr);
        for (; aItOffset.More(); aItOffset.Next()) {
          const TopoDS_Shape& aEOffsetOr = aItOffset.Value();
          const TopTools_ListOfShape& aLImages = theOEImages.Find(aEOffsetOr);
          //
          TopTools_ListIteratorOfListOfShape aItImages(aLImages);
          for (; aItImages.More(); aItImages.Next()) {
            const TopoDS_Edge& anIm = *(TopoDS_Edge*)&aItImages.Value();
            if (aME.Contains(anIm) && aMImFence.Add(anIm)) {
              aBB.Add(anImages, anIm);
              aLEImages.Append(anIm);
            }
          }
        }
        //
        // find alone vertices
        TopoDS_Vertex aVW1, aVW2;
        TopTools_IndexedDataMapOfShapeListOfShape aDMImVE;
        TopExp::MapShapesAndAncestors(anImages, TopAbs_VERTEX, TopAbs_EDGE, aDMImVE);
        //
        TopTools_ListOfShape aLVAlone;
        aNb = aDMImVE.Extent();
        for (i = 1; i <= aNb; ++i) {
          const TopTools_ListOfShape& aLImE = aDMImVE(i);
          if (aLImE.Extent() == 1) {
            aLVAlone.Append(aDMImVE.FindKey(i));
          }
        }
        //
        if (aLVAlone.Extent() > 1) {
          aVW1 = *(TopoDS_Vertex*)&aLVAlone.First();
          aVW2 = *(TopoDS_Vertex*)&aLVAlone.Last();
          //
          // check distances
          const gp_Pnt& aPI1 = BRep_Tool::Pnt(aVI1);
          const gp_Pnt& aPW1 = BRep_Tool::Pnt(aVW1);
          const gp_Pnt& aPW2 = BRep_Tool::Pnt(aVW2);
          //
          Standard_Real aDist1 = aPI1.SquareDistance(aPW1);
          Standard_Real aDist2 = aPI1.SquareDistance(aPW2);
          //
          if (aDist1 < aDist2) {
            aVI1 = aVW1;
            aVI2 = aVW2;
          }
          else {
            aVI1 = aVW2;
            aVI2 = aVW1;
          }
        }
      }
      else {
        aLEImages.Append(aEIm);
      }
      //
      // Find edges connected to these vertices
      const TopTools_ListOfShape& aLIE1 = aDMVE.FindFromKey(aVI1);
      const TopTools_ListOfShape& aLIE2 = aDMVE.FindFromKey(aVI2);
      //
      // Find vertices on the original face corresponding to vertices on the offset edge
      //
      // find original edges for both lists
      TopTools_ListOfShape aLOE1, aLOE2;
      for (i = 0; i < 2; ++i) {
        const TopTools_ListOfShape& aLIE = !i ? aLIE1 : aLIE2;
        TopTools_ListOfShape& aLOE = !i ? aLOE1 : aLOE2;
        //
        TopTools_MapOfShape aMFence;
        //
        TopTools_ListIteratorOfListOfShape aItLIE(aLIE);
        for (; aItLIE.More(); aItLIE.Next()) {
          const TopoDS_Shape& aEI = aItLIE.Value();
          if (theEdgesOrigins.IsBound(aEI)) {
            const TopTools_ListOfShape& aLEOrigins = theEdgesOrigins.Find(aEI);
            //
            TopTools_ListIteratorOfListOfShape aItLOE(aLEOrigins);
            for (; aItLOE.More(); aItLOE.Next()) {
              const TopoDS_Shape& aEO = aItLOE.Value();
              if (aEO.ShapeType() == TopAbs_EDGE && aMFence.Add(aEO)) {
                TopoDS_Shape aEOin;
                if (FindShape(aEO, aFOr, aEOin)) {
                  aLOE.Append(aEO);
                }
              }
            }
          }
        }
      }
      //
      if (aLOE1.Extent() < 2 || aLOE2.Extent() < 2) {
        continue;
      }
      //
      // find vertices common for all edges in the lists
      for (i = 0; i < 2; ++i) {
        const TopTools_ListOfShape& aLOE = !i ? aLOE1 : aLOE2;
        TopoDS_Vertex& aVO = !i ? aVO1 : aVO2;
        //
        const TopoDS_Shape& aEO = aLOE.First();
        TopExp_Explorer aExpV(aEO, TopAbs_VERTEX);
        for (; aExpV.More(); aExpV.Next()) {
          const TopoDS_Vertex& aV = *(TopoDS_Vertex*)&aExpV.Current();
          //
          Standard_Boolean bVertValid = Standard_True;
          TopTools_ListIteratorOfListOfShape aItLOE(aLOE);
          for (aItLOE.Next(); aItLOE.More(); aItLOE.Next()) {
            const TopoDS_Shape& aEOx = aItLOE.Value();
            TopExp_Explorer aExpVx(aEOx, TopAbs_VERTEX);
            for (; aExpVx.More(); aExpVx.Next()) {
              const TopoDS_Shape& aVx = aExpVx.Current();
              if (aVx.IsSame(aV)) {
                break;
              }
            }
            //
            if (!aExpVx.More()) {
              bVertValid = Standard_False;
              break;
            }
          }
          //
          if (bVertValid) {
            aVO = aV;
            break;
          }
        }
      }
      //
      if (aVO1.IsNull() || aVO2.IsNull() || aVO1.IsSame(aVO2)) {
        continue;
      }
      //
      // check positions of the offset and original vertices
      const gp_Pnt& aPI1 = BRep_Tool::Pnt(aVI1);
      const gp_Pnt& aPI2 = BRep_Tool::Pnt(aVI2);
      const gp_Pnt& aPO1 = BRep_Tool::Pnt(aVO1);
      const gp_Pnt& aPO2 = BRep_Tool::Pnt(aVO2);
      //
      gp_Vec aVI(aPI1, aPI2);
      gp_Vec aVO(aPO1, aPO2);
      //
      Standard_Real anAngle = aVI.Angle(aVO);
      if (Abs(anAngle - M_PI) < 1.e-4) {
        TopTools_ListIteratorOfListOfShape aItLEIm(aLEImages);
        for (; aItLEIm.More(); aItLEIm.Next()) {
          const TopoDS_Shape& aEInvr = aItLEIm.Value();
          theMEInverted.Add(aEInvr);
        }
      }
    }
    //
    if (aLVE.Extent()) {
      theDMFLVE.Bind(aFIm, aLVE);
    }
  }
  //
  // find intersections between two maps
  aItLF.Initialize(aLEInv);
  for (; aItLF.More(); aItLF.Next()) {
    const TopoDS_Shape& aE = aItLF.Value();
    if (aMEVal.Contains(aE)) {
      aLEN.Append(aE);
    }
  }
  theDMFLNE.Bind(theF, aLEN);
  theDMFLIE.Bind(theF, aLEInv);
}

//=======================================================================
//function : FindInvalidFaces
//purpose  : Find invalid faces
//=======================================================================
void FindInvalidFaces(TopTools_ListOfShape& theLFImages,
                      const TopTools_IndexedMapOfShape& theInvEdges,
                      const TopTools_IndexedMapOfShape& theValidEdges,
                      const TopTools_DataMapOfShapeListOfShape& theDMFLVE,
                      const TopTools_ListOfShape& theLEInvalid,
                      const TopTools_ListOfShape& theLENeutral,
                      const TopTools_MapOfShape& theMEInverted,
                      TopTools_ListOfShape& theInvFaces)
{
  // The face should be considered as invalid in the following cases:
  // 1. It has been reverted, i.e. at least two not connected edges 
  //    have changed orientation (i.e. invalid). In this case all edges,
  //    should be invalid for that face, because edges have also been reverted;
  // 2. All checked edges of the face are invalid for this face;
  // The face should be removed from the splits in the following cases:
  // 1. All checked edges of the face are invalid for this one, but valid for
  //    some other face in this list of splits.
  // The face will be kept in the following cases:
  // 1. Some of the edges are valid for this face.
  Standard_Boolean bHasValid, bAllValid, bAllInvalid, bHasReallyInvalid, bAllInvNeutral;
  Standard_Boolean bValid, bValidLoc, bInvalid, bNeutral, bRemove;
  Standard_Integer aNbChecked;
  //
  TopTools_MapOfShape aMEN;
  TopTools_ListIteratorOfListOfShape aIt(theLENeutral);
  for (; aIt.More(); aIt.Next())  {
    aMEN.Add(aIt.Value());
  }
  //
  Standard_Boolean bCheckInverted = (theLFImages.Extent() == 1);
  //
  TopTools_MapOfShape aMEChecked;
  // valid edges
  aIt.Initialize(theLFImages);
  for (; aIt.More(); aIt.Next() ) {
    const TopoDS_Shape& aFIm = aIt.Value();
    if (theDMFLVE.IsBound(aFIm)) {
      const TopTools_ListOfShape& aLVE = theDMFLVE.Find(aFIm);
      TopTools_ListIteratorOfListOfShape aItLE(aLVE);
      for (; aItLE.More(); aItLE.Next()) {
        const TopoDS_Shape& aE = aItLE.Value();
        aMEChecked.Add(aE);
      }
    }
  }
  // invalid edges
  aIt.Initialize(theLEInvalid);
  for (; aIt.More(); aIt.Next() ) {
    const TopoDS_Shape& anInvE = aIt.Value();
    aMEChecked.Add(anInvE);
  }
  //
  aIt.Initialize(theLFImages);
  for (; aIt.More(); ) {
    const TopoDS_Face& aFIm = *(TopoDS_Face*)&aIt.Value();
    //
    TopTools_MapOfShape aMVE;
    if (theDMFLVE.IsBound(aFIm)) {
      const TopTools_ListOfShape& aLVE = theDMFLVE.Find(aFIm);
      TopTools_ListIteratorOfListOfShape aItLE(aLVE);
      for (; aItLE.More(); aItLE.Next()) {
        const TopoDS_Shape& aE = aItLE.Value();
        aMVE.Add(aE);
      }
    }
    //
    bHasValid = Standard_False;
    bAllValid = Standard_True;
    bAllInvalid = Standard_True;
    bHasReallyInvalid = Standard_False;
    bAllInvNeutral = Standard_True;
    aNbChecked = 0;
    //
    const TopoDS_Wire& aWIm = BRepTools::OuterWire(aFIm);
    TopExp_Explorer aExp(aWIm, TopAbs_EDGE);
    for (; aExp.More(); aExp.Next()) {
      const TopoDS_Shape& aEIm = aExp.Current();
      //
      bValid = theValidEdges.Contains(aEIm);
      bInvalid = theInvEdges.Contains(aEIm);
      //
      if (!bValid && !bInvalid) {
        // edge has not been checked for some reason
        continue;
      }
      //
      ++aNbChecked;
      //
      bNeutral = aMEN.Contains(aEIm);
      bValidLoc = aMVE.Contains(aEIm);
      //
      if (!bInvalid && bCheckInverted) {
        bInvalid = theMEInverted.Contains(aEIm);
      }
      //
      if (bValidLoc && bNeutral) {
        bHasValid = Standard_True;
      }
      //
      bAllValid = bAllValid && bValidLoc;
      bAllInvalid = bAllInvalid && bInvalid;
      bHasReallyInvalid = bInvalid && !bValid && aMEChecked.Contains(aEIm);
      bAllInvNeutral = bAllInvNeutral && bAllInvalid && bNeutral;
      //
      if (bHasReallyInvalid) {
        break;
      }
    }
    //
    if (!aNbChecked) {
      aIt.Next();
      continue;
    }
    //
    if (!bHasReallyInvalid && (bAllInvNeutral && !bHasValid) && (aNbChecked > 1)) {
      theLFImages.Remove(aIt);
      continue;
    }
    //
    bRemove = bHasReallyInvalid || (bAllInvalid && !(bHasValid || bAllValid));
    if (bRemove) {
      theInvFaces.Append(aFIm);
    }
    aIt.Next();
  }
}

//=======================================================================
//function : CheckNormals
//purpose  : 
//=======================================================================
Standard_Boolean CheckNormals(const TopoDS_Face& theFIm,
                              const TopoDS_Face& theFOr)
{
  
  Standard_Real aUMin, aUMax, aVMin, aVMax, aU, aV, anAngle;
  gp_Pnt aP;
  gp_Vec aVecU, aVecV, aVNIm, aVNOr;
  Standard_Boolean bIsCollinear;
  //
  BRepAdaptor_Surface aSFIm(theFIm), aSFOr(theFOr);
  //
  aUMin = aSFIm.FirstUParameter();
  aUMax = aSFIm.LastUParameter();
  aVMin = aSFIm.FirstVParameter();
  aVMax = aSFIm.LastVParameter();
  //
  aU = (aUMin + aUMax) * 0.5;
  if (Precision::IsInfinite(aUMin) && 
      Precision::IsInfinite(aUMax)) {
    aU = 0.;
  }
  else if (Precision::IsInfinite(aUMin) && 
           !Precision::IsInfinite(aUMax)) {
    aU = aUMax;
  }
  else if (!Precision::IsInfinite(aUMin) && 
           Precision::IsInfinite(aUMax)) {
    aU = aUMin;
  }
  //
  aV = (aVMin + aVMax) * 0.5;
  if (Precision::IsInfinite(aVMin) && 
      Precision::IsInfinite(aVMax)) {
    aV = 0.;
  }
  else if (Precision::IsInfinite(aVMin) && 
           !Precision::IsInfinite(aVMax)) {
    aV = aVMax;
  }
  else if (!Precision::IsInfinite(aVMin) && 
           Precision::IsInfinite(aVMax)) {
    aV = aVMin;
  }
  //
  aSFIm.D1(aU, aV, aP, aVecU, aVecV);
  aVNIm = aVecU.Crossed(aVecV);
  if (theFIm.Orientation() == TopAbs_REVERSED) {
    aVNIm.Reverse();
  }
  //
  aSFOr.D1(aU, aV, aP, aVecU, aVecV);
  aVNOr = aVecU.Crossed(aVecV);
  if (theFOr.Orientation() == TopAbs_REVERSED) {
    aVNOr.Reverse();
  }
  //
  anAngle = aVNIm.Angle(aVNOr);
  bIsCollinear = (anAngle < Precision::Confusion());
  return bIsCollinear;
}

//=======================================================================
//function : IsSolid
//purpose  : Check if the shape is solid
//=======================================================================
Standard_Boolean IsSolid(const TopoDS_Shape& theS)
{
  TopExp_Explorer aExp(theS, TopAbs_SOLID);
  return aExp.More();
}

//=======================================================================
//function : FillHistory
//purpose  : 
//=======================================================================
void FillHistory(const TopTools_IndexedDataMapOfShapeListOfShape& theFImages,
                 const TopTools_DataMapOfShapeListOfShape& theEImages,
                 BRepAlgo_Image& theImage)
{
  Standard_Integer aNb = theFImages.Extent();
  if (!aNb) {
    return;
  }
  //
  Standard_Integer i;
  TopTools_IndexedMapOfShape aMFE;
  TopTools_ListIteratorOfListOfShape aIt;
  TopoDS_Compound aFaces;
  BRep_Builder aBB;
  //
  aBB.MakeCompound(aFaces);
  //
  // Fill history for faces
  for (i = 1; i <= aNb; ++i) {
    const TopoDS_Shape& aF = theFImages.FindKey(i);
    const TopTools_ListOfShape& aLFImages = theFImages(i);
    //
    if (aLFImages.Extent()) {
      if (theImage.HasImage(aF)) {
        theImage.Add(aF, aLFImages);
      }
      else {
        theImage.Bind(aF, aLFImages);
      }
    }
    //
    aIt.Initialize(aLFImages);
    for (; aIt.More(); aIt.Next()) {
      const TopoDS_Shape& aFIm = aIt.Value();
      aBB.Add(aFaces, aFIm);
    }
  }
  //
  // fill history for edges
  TopExp::MapShapes(aFaces, TopAbs_EDGE, aMFE);
  //
  TopTools_DataMapIteratorOfDataMapOfShapeListOfShape aItEIm(theEImages);
  for (; aItEIm.More(); aItEIm.Next()) {
    const TopoDS_Shape& aE = aItEIm.Key();
    const TopTools_ListOfShape& aLEIm = aItEIm.Value();
    //
    Standard_Boolean bHasImage = theImage.HasImage(aE);
    aIt.Initialize(aLEIm);
    for (; aIt.More(); aIt.Next()) {
      const TopoDS_Shape& aEIm = aIt.Value();
      if (aMFE.Contains(aEIm)) {
        if (bHasImage) {
          theImage.Add(aE, aEIm);
        }
        else {
          theImage.Bind(aE, aEIm);
          bHasImage = Standard_True;
        }
      }
    }
  }
}

//=======================================================================
//function : FindOrigins
//purpose  : 
//=======================================================================
void FindOrigins(const TopTools_ListOfShape& theLFIm1,
                 const TopTools_ListOfShape& theLFIm2,
                 const TopTools_IndexedMapOfShape& theME,
                 const TopTools_DataMapOfShapeListOfShape& theOrigins,
                 TopTools_ListOfShape& theLEOr)
{
  Standard_Integer i;
  TopTools_MapOfShape aMFence;
  TopExp_Explorer aExp;
  TopTools_ListIteratorOfListOfShape aIt, aItLE;
  //
  for (i = 0; i < 2; ++i) {
    const TopTools_ListOfShape& aLF = !i ? theLFIm1 : theLFIm2;
    aIt.Initialize(aLF);
    for (; aIt.More(); aIt.Next()) {
      const TopoDS_Shape& aF = aIt.Value();
      //
      aExp.Init(aF, TopAbs_EDGE);
      for (; aExp.More(); aExp.Next()) {
        const TopoDS_Shape& aE = aExp.Current();
        //
        if (theME.Contains(aE) && theOrigins.IsBound(aE)) {
          const TopTools_ListOfShape& aLEOr = theOrigins.Find(aE);
          aItLE.Initialize(aLEOr);
          for (; aItLE.More(); aItLE.Next()) {
            const TopoDS_Shape& aEOr = aItLE.Value();
            //
            if (aMFence.Add(aEOr) && (aEOr.ShapeType() == TopAbs_EDGE)) {
              theLEOr.Append(aEOr);
            }
          } // for (; aItLE.More(); aItLE.Next()) {
        } // if (theME.Contains(aE) && theOrigins.IsBound(aE)) {
      } // aExp.Init(aF, TopAbs_EDGE);
    } // for (; aIt.More(); aIt.Next()) {
  } // for (i = 0; i < 2; ++i) {
}

//=======================================================================
//function : CheckIfArtificial
//purpose  : Checks if the face is artificially invalid
//=======================================================================
Standard_Boolean CheckIfArtificial(const TopoDS_Shape& theF,
                                   const TopTools_ListOfShape& theLFImages,
                                   const TopoDS_Shape& theCE,
                                   const TopTools_MapOfShape& theMapEInv,
                                   TopTools_MapOfShape& theMENInv,
                                   Handle(BRepAlgo_AsDes)& theAsDes,
                                   const TopTools_DataMapOfShapeListOfShape& theOEImages)
{
  // all boundary edges should be used
  TopTools_IndexedMapOfShape aMEUsed;
  TopTools_ListIteratorOfListOfShape aItLFIm(theLFImages);
  for (; aItLFIm.More(); aItLFIm.Next()) {
    const TopoDS_Shape& aFIm = aItLFIm.Value();
    TopExp::MapShapes(aFIm, TopAbs_EDGE, aMEUsed);
    TopExp::MapShapes(aFIm, TopAbs_VERTEX, aMEUsed);
  }
  //
  TopTools_IndexedDataMapOfShapeListOfShape aMVE;
  TopExp::MapShapesAndAncestors(theCE, TopAbs_VERTEX, TopAbs_EDGE, aMVE);
  //
  TopTools_MapIteratorOfMapOfShape aItM(theMapEInv);
  for (; aItM.More(); aItM.Next()) {
    const TopoDS_Shape& aEInv = aItM.Key();
    TopExp_Explorer aExpV(aEInv, TopAbs_VERTEX);
    for (; aExpV.More(); aExpV.Next()) {
      const TopoDS_Shape& aVEInv = aExpV.Current();
      if (!aMEUsed.Contains(aVEInv) && aMVE.Contains(aVEInv)) {
        const TopTools_ListOfShape& aLENInv = aMVE.FindFromKey(aVEInv);
        TopTools_ListIteratorOfListOfShape aItLEInv(aLENInv);
        for (; aItLEInv.More(); aItLEInv.Next()) {
          const TopoDS_Shape& aENInv = aItLEInv.Value();
          theMENInv.Add(aENInv);
        }
      }
    }
  }
  //
  if (theMENInv.IsEmpty()) {
    return Standard_False;
  }
  //
  TopTools_IndexedMapOfShape aMEFound;
  TopExp::MapShapes(theCE, TopAbs_EDGE, aMEFound);
  //
  const TopTools_ListOfShape& aLE = theAsDes->Descendant(theF);
  TopTools_ListIteratorOfListOfShape aItLE(aLE);
  for (; aItLE.More(); aItLE.Next()) {
    const TopoDS_Edge& aE = TopoDS::Edge(aItLE.Value());
    //
    if (theOEImages.IsBound(aE)) {
      Standard_Boolean bChecked = Standard_False;
      const TopTools_ListOfShape& aLEIm = theOEImages.Find(aE);
      TopTools_ListIteratorOfListOfShape aItLEIm(aLEIm);
      for (; aItLEIm.More(); aItLEIm.Next()) {
        const TopoDS_Edge& aEIm = TopoDS::Edge(aItLEIm.Value());
        if (!aMEFound.Contains(aEIm) || theMENInv.Contains(aEIm)) {
          continue;
        }
        //
        bChecked = Standard_True;
        if (aMEUsed.Contains(aEIm)) {
          break;
        }
      }
      //
      if (bChecked && !aItLEIm.More()) {
        break;
      }
    }
    else {
      if (aMEFound.Contains(aE) && !theMENInv.Contains(aE) && !aMEUsed.Contains(aE)) {
        break;
      }
    }
  }
  //
  return aItLE.More();
}

//=======================================================================
//function : RemoveInvalidSplitsFromValid
//purpose  : 
//=======================================================================
void RemoveInvalidSplitsFromValid(TopTools_IndexedDataMapOfShapeListOfShape& theFImages,
                                  const TopTools_IndexedDataMapOfShapeListOfShape& theInvFaces,
                                  const TopTools_DataMapOfShapeShape& theArtInvFaces,
                                  const TopTools_MapOfShape& theMEInverted)
{
  // Decide whether to remove the found invalid faces or not.
  // The procedure is the following:
  // 1. Make connexity blocks from invalid faces;
  // 2. Find free edges in this blocks;
  // 3. If all free edges are valid for the faces - remove block.
  //
  TopTools_MapOfShape aMFence, aMFToRem;
  TopoDS_Compound aCFInv;
  BRep_Builder aBB;
  aBB.MakeCompound(aCFInv);
  TopTools_ListIteratorOfListOfShape aItLF;
  //
  // make compound of invalid faces
  TopTools_DataMapOfShapeShape aDMIFOF;
  Standard_Integer i, aNb = theInvFaces.Extent();
  for (i = 1; i <= aNb; ++i) {
    const TopoDS_Shape& aF = theInvFaces.FindKey(i);
    // artificially invalid faces should not be removed
    if (theArtInvFaces.IsBound(aF)) {
      continue;
    }
    const TopTools_ListOfShape& aLFInv = theInvFaces(i);
    aItLF.Initialize(aLFInv);
    for (; aItLF.More(); aItLF.Next()) {
      const TopoDS_Shape& aFIm = aItLF.Value();
      if (aMFence.Add(aFIm)) {
        aBB.Add(aCFInv, aFIm);
        aDMIFOF.Bind(aFIm, aF);
      }
    }
  }
  //
  // make connexity blocks
  BOPCol_ListOfShape aLCBInv;
  BOPTools_AlgoTools::MakeConnexityBlocks(aCFInv, TopAbs_EDGE, TopAbs_FACE, aLCBInv);
  //
  // analyze each block
  BOPCol_ListIteratorOfListOfShape aItLCB(aLCBInv);
  for (; aItLCB.More(); aItLCB.Next()) {
    const TopoDS_Shape& aCB = aItLCB.Value();
    //
    // if connexity block contains only one face - it should be removed;
    TopExp_Explorer aExp(aCB, TopAbs_FACE);
    aExp.Next();
    if (aExp.More()) {
      // check if there are valid images left
      aExp.Init(aCB, TopAbs_FACE);
      for (; aExp.More(); aExp.Next()) {
        const TopoDS_Shape& aFIm = aExp.Current();
        const TopoDS_Shape& aF = aDMIFOF.Find(aFIm);
        //
        const TopTools_ListOfShape& aLFIm = theFImages.FindFromKey(aF);
        const TopTools_ListOfShape& aLFInv = theInvFaces.FindFromKey(aF);
        //
        if (aLFIm.Extent() == aLFInv.Extent()) {
          break;
        }
      }
    }
    //
    if (!aExp.More()) {
      aExp.Init(aCB, TopAbs_FACE);
      for (; aExp.More(); aExp.Next()) {
        const TopoDS_Shape& aF = aExp.Current();
        aMFToRem.Add(aF);
      }
      continue;
    }
    //
    // remove faces connected by inverted edges
    TopTools_IndexedDataMapOfShapeListOfShape aDMEF;
    TopExp::MapShapesAndAncestors(aCB, TopAbs_EDGE, TopAbs_FACE, aDMEF);
    //
    aExp.Init(aCB, TopAbs_FACE);
    for (; aExp.More(); aExp.Next()) {
      const TopoDS_Shape& aFCB = aExp.Current();
      //
      TopExp_Explorer aExpE(aFCB, TopAbs_EDGE);
      for (; aExpE.More(); aExpE.Next()) {
        const TopoDS_Shape& aECB = aExpE.Current();
        if (aDMEF.FindFromKey(aECB).Extent() > 1) {
          if (!theMEInverted.Contains(aECB)) {
            break;
          }
        }
      }
      //
      if (!aExpE.More()) {
        aMFToRem.Add(aFCB);
      }
    }
  }
  //
  if (aMFToRem.Extent()) {
    // remove invalid faces from images
    aNb = theInvFaces.Extent();
    for (i = 1; i <= aNb; ++i) {
      const TopoDS_Shape& aF = theInvFaces.FindKey(i);
      TopTools_ListOfShape& aLFImages = theFImages.ChangeFromKey(aF);
      aItLF.Initialize(aLFImages);
      for (; aItLF.More();) {
        const TopoDS_Shape& aFIm = aItLF.Value();
        if (aMFToRem.Contains(aFIm)) {
          aLFImages.Remove(aItLF);
        }
        else {
          aItLF.Next();
        }
      }
    }
  }
}

//=======================================================================
//function : RemoveInsideFaces
//purpose  : 
//=======================================================================
void RemoveInsideFaces(TopTools_IndexedDataMapOfShapeListOfShape& theFImages,
                       TopTools_IndexedDataMapOfShapeListOfShape& theInvFaces,
                       const TopTools_DataMapOfShapeShape& theArtInvFaces,
                       const TopTools_IndexedMapOfShape& theInvEdges,
                       const TopTools_IndexedMapOfShape& theMFToCheckInt,
                       TopTools_IndexedMapOfShape& theMERemoved)
{
  BOPCol_ListOfShape aLS;
  TopTools_MapOfShape aMFence, aMFToRem;
  TopTools_IndexedMapOfShape aMFInv;
  TopTools_ListIteratorOfListOfShape aItLF;
  //
  Standard_Integer i, aNb = theInvFaces.Extent();
  for (i = 1; i <= aNb; ++i) {
    const TopTools_ListOfShape& aLFInv = theInvFaces(i);
    aItLF.Initialize(aLFInv);
    for (; aItLF.More(); aItLF.Next()) {
      const TopoDS_Shape& aFIm = aItLF.Value();
      if (aMFence.Add(aFIm)) {
        aLS.Append(aFIm);
        aMFInv.Add(aFIm);
      }
    }
  }
  //
  // make the solid from the images and remove those faces inside the shape
  aNb = theFImages.Extent();
  for (i = 1; i <= aNb; ++i) {
    const TopTools_ListOfShape& aLFImages = theFImages(i);
    aItLF.Initialize(aLFImages);
    for (; aItLF.More(); aItLF.Next()) {
      const TopoDS_Shape& aFIm = aItLF.Value();
      if (aMFence.Add(aFIm)) {
        aLS.Append(aFIm);
      }
    }
  }
  //
  aNb = theMFToCheckInt.Extent();
  for (i = 1; i <= aNb; ++i) {
    const TopoDS_Shape& aFSp = theMFToCheckInt(i);
    if (aMFence.Add(aFSp)) {
      aLS.Append(aFSp);
    }
  }
  //
  BOPAlgo_MakerVolume aMV;
  aMV.SetArguments(aLS);
  aMV.SetIntersect(Standard_True);
  aMV.Perform();
  //
  const TopoDS_Shape& aSols = aMV.Shape();
  //
  TopTools_IndexedDataMapOfShapeListOfShape aDMFS;
  TopExp::MapShapesAndAncestors(aSols, TopAbs_FACE, TopAbs_SOLID, aDMFS);
  //
  aNb = aDMFS.Extent();
  if (!aNb) {
    return;
  }
  //
  for (i = 1; i <= aNb; ++i) {
    const TopTools_ListOfShape& aLSol = aDMFS(i);
    if (aLSol.Extent() > 1) {
      const TopoDS_Shape& aFIm = aDMFS.FindKey(i);
      aMFToRem.Add(aFIm);
    }
  }
  //
  // update invalid faces with images
  aNb = aMFInv.Extent();
  for (i = 1; i <= aNb; ++i) {
    const TopoDS_Shape& aFInv = aMFInv(i);
    const TopTools_ListOfShape& aLFInvIm = aMV.Modified(aFInv);
    TopTools_ListIteratorOfListOfShape aItLFInvIm(aLFInvIm);
    for (; aItLFInvIm.More(); aItLFInvIm.Next()) {
      const TopoDS_Shape& aFInvIm = aItLFInvIm.Value();
      aMFInv.Add(aFInvIm);
    }
  }
  //
  TopExp_Explorer aExpS(aSols, TopAbs_SOLID);
  for (; aExpS.More(); aExpS.Next()) {
    const TopoDS_Shape& aSol = aExpS.Current();
    //
    Standard_Boolean bAllInv(Standard_True), bAllRemoved(Standard_True);

    TopExp_Explorer aExpF(aSol, TopAbs_FACE);
    for (; aExpF.More(); aExpF.Next()) {
      const TopoDS_Shape& aFS = aExpF.Current();
      //
      if (aFS.Orientation() == TopAbs_INTERNAL) {
        aMFToRem.Add(aFS);
      }
      //
      bAllRemoved = bAllRemoved && aMFToRem.Contains(aFS);
      bAllInv = bAllInv && (aMFToRem.Contains(aFS) || aMFInv.Contains(aFS));
    }
    //
    if (bAllInv && !bAllRemoved) {
      // remove invalid faces but keep those that have already been marked for removal
      TopExp_Explorer aExpF(aSol, TopAbs_FACE);
      for (; aExpF.More(); aExpF.Next()) {
        const TopoDS_Shape& aFS = aExpF.Current();
        //
        if (aMFToRem.Contains(aFS)) {
          aMFToRem.Remove(aFS);
        }
        else {
          aMFToRem.Add(aFS);
        }
      }
    }
  }
  //
  // remove newly found faces
  RemoveValidSplits(theFImages, aMV, aMFToRem, theMERemoved);
  RemoveInvalidSplits(theInvFaces, theArtInvFaces, theInvEdges, aMV, aMFToRem);
}

//=======================================================================
//function : FilterInvalidFaces
//purpose  : 
//=======================================================================
void FilterInvalidFaces(TopTools_IndexedDataMapOfShapeListOfShape& theInvFaces,
                        TopTools_DataMapOfShapeShape& theArtInvFaces,
                        const TopTools_IndexedDataMapOfShapeListOfShape& theFImages,
                        const TopTools_IndexedDataMapOfShapeListOfShape& theDMFE)
{
  //
  // filter invalid faces, considering faces having only valid 
  // images left with non-free edges as valid
  // do not remove invalid faces if it creates free edges
  //
  TopTools_IndexedDataMapOfShapeListOfShape aReallyInvFaces;
  TopTools_ListIteratorOfListOfShape aItLF;
  //
  Standard_Integer i, aNb = theInvFaces.Extent();
  for (i = 1; i <= aNb; ++i) {
    const TopoDS_Shape& aF = theInvFaces.FindKey(i);
    const TopTools_ListOfShape& aLFInv = theInvFaces(i);
    //
    if (theArtInvFaces.IsBound(aF)) {
      if (aLFInv.IsEmpty()) {
        theArtInvFaces.UnBind(aF);
      }
      else {
        aReallyInvFaces.Add(aF, aLFInv);
      }
      continue;
    }
    //
    if (aLFInv.IsEmpty()) {
      continue;
    }
    //
    const TopTools_ListOfShape& aLFIm = theFImages.FindFromKey(aF);
    Standard_Boolean bInvalid = aLFIm.IsEmpty();
    //
    if (!bInvalid) {
      // check two lists on common splits
      aItLF.Initialize(aLFInv);
      for (; aItLF.More(); aItLF.Next()) {
        const TopoDS_Shape& aFInv = aItLF.Value();
        //
        TopTools_ListIteratorOfListOfShape aItLFIm(aLFIm);
        for (; aItLFIm.More(); aItLFIm.Next()) {
          const TopoDS_Shape& aFIm = aItLFIm.Value();
          //
          if (aFInv.IsSame(aFIm)) {
            break;
          }
        }
        //
        if (aItLFIm.More()) {
          break;
        }
      }
      //
      bInvalid = aItLF.More();
    }
    //
    if (!bInvalid) {
      // check for free edges
      for (Standard_Integer j = 0; !bInvalid && j < 2; ++j) {
        const TopTools_ListOfShape& aLI = !j ? aLFIm : aLFInv;
        aItLF.Initialize(aLI);
        for (; aItLF.More(); aItLF.Next()) {
          const TopoDS_Shape& aFIm = aItLF.Value();
          //
          TopExp_Explorer aExp(aFIm, TopAbs_EDGE);
          for (; aExp.More(); aExp.Next()) {
            const TopoDS_Shape& aE = aExp.Current();
            if (theDMFE.Contains(aE)) {
              const TopTools_ListOfShape& aLEF = theDMFE.FindFromKey(aE);
              if (aLEF.Extent() == 1) {
                break;
              }
            }
          }
          //
          if (aExp.More()) {
            break;
          }
        }
        bInvalid = aItLF.More();
      }
    }
    //
    if (bInvalid) {
      aReallyInvFaces.Add(aF, aLFInv);
    }
  }
  //
  theInvFaces = aReallyInvFaces;
}

//=======================================================================
//function : FilterInvalidEdges
//purpose  : 
//=======================================================================
void FilterInvalidEdges(TopTools_IndexedMapOfShape& theInvEdges,
                        const TopTools_IndexedDataMapOfShapeListOfShape& theInvFaces,
                        const TopTools_DataMapOfShapeShape& theArtInvFaces,
                        const TopTools_DataMapOfShapeListOfShape& theDMFLIE,
                        const TopTools_IndexedMapOfShape& theMERemoved)
{
  TopoDS_Compound aCEInv;
  TopTools_IndexedMapOfShape aMEInv;
  BRep_Builder aBB;
  aBB.MakeCompound(aCEInv);
  TopTools_ListIteratorOfListOfShape aItLF;
  //
  Standard_Integer i, aNb = theInvFaces.Extent();
  for (i = 1; i <= aNb; ++i) {
    const TopTools_ListOfShape& aLFInv = theInvFaces(i);
    aItLF.Initialize(aLFInv);
    for (; aItLF.More(); aItLF.Next()) {
      const TopoDS_Shape& aFIm = aItLF.Value();
      TopExp::MapShapes(aFIm, TopAbs_EDGE, aMEInv);
      //
      TopExp_Explorer aExpE(aFIm, TopAbs_EDGE);
      for (; aExpE.More(); aExpE.Next()) {
        const TopoDS_Shape& aE = aExpE.Current();
        if (theInvEdges.Contains(aE)) {
          aBB.Add(aCEInv, aE);
        }
      }
    }
  }
  //
  // remove edges which have been marked for removal
  TopTools_IndexedMapOfShape aMEInvToAvoid;
  BOPCol_ListOfShape aLCBE;
  BOPTools_AlgoTools::MakeConnexityBlocks(aCEInv, TopAbs_VERTEX, TopAbs_EDGE, aLCBE);
  //
  BOPCol_ListIteratorOfListOfShape aItLCBE(aLCBE);
  for (; aItLCBE.More(); aItLCBE.Next()) {
    const TopoDS_Shape& aCBE = aItLCBE.Value();
    TopExp_Explorer aExpCB(aCBE, TopAbs_EDGE);
    for (; aExpCB.More(); aExpCB.Next()) {
      const TopoDS_Shape& aE = aExpCB.Current();
      if (!theMERemoved.Contains(aE)) {
        break;
      }
    }
    //
    if (!aExpCB.More()) {
      TopExp::MapShapes(aCBE, TopAbs_EDGE, aMEInvToAvoid);
    }
  }
  //
  TopTools_IndexedMapOfShape aReallyInvEdges;
  //
  aNb = theInvFaces.Extent();
  for (i = 1; i <= aNb; ++i) {
    const TopoDS_Shape& aF = theInvFaces.FindKey(i);
    if (theArtInvFaces.IsBound(aF)) {
      const TopTools_ListOfShape& aLEInv = theDMFLIE.Find(aF);
      aItLF.Initialize(aLEInv);
      for (; aItLF.More(); aItLF.Next()) {
        const TopoDS_Shape& aE = aItLF.Value();
        if (aMEInv.Contains(aE) && !aMEInvToAvoid.Contains(aE)) {
          aReallyInvEdges.Add(aE);
        }
      }
    }
    else {
      const TopTools_ListOfShape& aLFInv = theInvFaces(i);
      aItLF.Initialize(aLFInv);
      for (; aItLF.More(); aItLF.Next()) {
        const TopoDS_Shape& aFIm = aItLF.Value();
        TopExp_Explorer aExpE(aFIm, TopAbs_EDGE);
        for (; aExpE.More(); aExpE.Next()) {
          const TopoDS_Shape& aE = aExpE.Current();
          if (theInvEdges.Contains(aE) && !aMEInvToAvoid.Contains(aE)) {
            aReallyInvEdges.Add(aE);
          }
        }
      }
    }
  }
  //
  theInvEdges = aReallyInvEdges;
}

//=======================================================================
//function : FilterEdgesImages
//purpose  : 
//=======================================================================
void FilterEdgesImages(const TopoDS_Shape& theS,
                       TopTools_DataMapOfShapeListOfShape& theOEImages,
                       TopTools_DataMapOfShapeListOfShape& theOEOrigins)
{
  // map edges
  TopTools_IndexedMapOfShape aME;
  TopExp::MapShapes(theS, TopAbs_EDGE, aME);
  //
  TopTools_DataMapIteratorOfDataMapOfShapeListOfShape aItDM(theOEImages);
  for (; aItDM.More(); aItDM.Next()) {
    const TopoDS_Shape& aE = aItDM.Key();
    TopTools_ListOfShape& aLE = theOEImages.ChangeFind(aE);
    //
    TopTools_ListIteratorOfListOfShape aItIm(aLE);
    for (; aItIm.More();) {
      const TopoDS_Shape& aEIm = aItIm.Value();
      if (aME.Contains(aEIm)) {
        aItIm.Next();
        continue;
      }
      //
      if (!theOEOrigins.IsBound(aEIm)) {
        aLE.Remove(aItIm);
        continue;
      }
      // remove aE from the origins
      TopTools_ListOfShape& aLEOr = theOEOrigins.ChangeFind(aEIm);
      TopTools_ListIteratorOfListOfShape aItOr(aLEOr);
      for (; aItOr.More(); aItOr.Next()) {
        const TopoDS_Shape& aEOr = aItOr.Value();
        if (aEOr.IsSame(aE)) {
          aLEOr.Remove(aItOr);
          break;
        }
      }
      if (aLEOr.IsEmpty()) {
        theOEOrigins.UnBind(aEIm);
      }
      //
      aLE.Remove(aItIm);
    }
  }
}

//=======================================================================
//function : RemoveValidSplits
//purpose  : 
//=======================================================================
void RemoveValidSplits(TopTools_IndexedDataMapOfShapeListOfShape& theImages,
                       BOPAlgo_Builder& theGF,
                       const TopTools_MapOfShape& theSpRem,
                       TopTools_IndexedMapOfShape& theMERemoved)
{
  Standard_Integer i, aNb = theImages.Extent();
  if (!aNb) {
    return;
  }
  //
  for (i = 1; i <= aNb; ++i) {
    TopTools_ListOfShape& aLSIm = theImages(i);
    TopTools_ListIteratorOfListOfShape aIt(aLSIm);
    for (; aIt.More(); ) {
      const TopoDS_Shape& aSIm = aIt.Value();
      if (theSpRem.Contains(aSIm)) {
        TopExp::MapShapes(aSIm, TopAbs_EDGE, theMERemoved);
        aLSIm.Remove(aIt);
        continue;
      }
      //
      // check if all its images are have to be removed
      const TopTools_ListOfShape& aLSImIm = theGF.Modified(aSIm);
      if (aLSImIm.Extent()) {
        Standard_Boolean bAllRem = Standard_True;
        TopTools_ListIteratorOfListOfShape aIt1(aLSImIm);
        for (; aIt1.More(); aIt1.Next()) {
          const TopoDS_Shape& aSImIm = aIt1.Value();
          if (theSpRem.Contains(aSImIm)) {
            TopExp::MapShapes(aSImIm, TopAbs_EDGE, theMERemoved);
          }
          else {
            bAllRem = Standard_False;
          }
        }
        //
        if (bAllRem) {
          aLSIm.Remove(aIt);
          continue;
        }
      }
      aIt.Next();
    }
  }
}

//=======================================================================
//function : RemoveInvalidSplits
//purpose  : 
//=======================================================================
void RemoveInvalidSplits(TopTools_IndexedDataMapOfShapeListOfShape& theImages,
                         const TopTools_DataMapOfShapeShape& theArtInvFaces,
                         const TopTools_IndexedMapOfShape& theInvEdges,
                         BOPAlgo_Builder& theGF,
                         const TopTools_MapOfShape& theSpRem)
{
  Standard_Integer i, aNb = theImages.Extent();
  if (!aNb) {
    return;
  }
  //
  for (i = 1; i <= aNb; ++i) {
    const TopoDS_Shape& aS = theImages.FindKey(i);
    Standard_Boolean bArt = theArtInvFaces.IsBound(aS);
    //
    TopTools_ListOfShape& aLSIm = theImages(i);
    TopTools_ListIteratorOfListOfShape aIt(aLSIm);
    for (; aIt.More();) {
      const TopoDS_Shape& aSIm = aIt.Value();
      if (theSpRem.Contains(aSIm)) {
        aLSIm.Remove(aIt);
        continue;
      }
      //
      // check if all its images are have to be removed
      const TopTools_ListOfShape& aLSImIm = theGF.Modified(aSIm);
      if (aLSImIm.IsEmpty()) {
        aIt.Next();
        continue;
      }
      //
      Standard_Boolean bAllRem = Standard_True;
      TopTools_IndexedMapOfShape aMERemoved;
      TopTools_ListIteratorOfListOfShape aIt1(aLSImIm);
      for (; aIt1.More(); aIt1.Next()) {
        const TopoDS_Shape& aSImIm = aIt1.Value();
        if (theSpRem.Contains(aSImIm)) {
          TopExp::MapShapes(aSImIm, TopAbs_EDGE, aMERemoved);
        }
        else {
          bAllRem = Standard_False;
        }
      }
      //
      if (bAllRem) {
        aLSIm.Remove(aIt);
        continue;
      }
      //
      if (bArt) {
        aIt.Next();
        continue;
      }
      //
      // remove the face from invalid if all invalid edges of this face
      // have been marked for removal
      TopExp_Explorer aExpE(aSIm, TopAbs_EDGE);
      for (; aExpE.More(); aExpE.Next()) {
        const TopoDS_Shape& aEInv = aExpE.Current();
        if (theInvEdges.Contains(aEInv) && !aMERemoved.Contains(aEInv)) {
          break;
        }
      }
      if (!aExpE.More()) {
        aLSIm.Remove(aIt);
      }
      else {
        aIt.Next();
      }
    }
  }
}

//=======================================================================
//function : PrepareFacesForIntersection
//purpose  : 
//=======================================================================
void PrepareFacesForIntersection(const TopTools_IndexedDataMapOfShapeListOfShape& theFToRebuild,
                                 const TopTools_IndexedDataMapOfShapeListOfShape& theFImages,
                                 TopTools_IndexedDataMapOfShapeListOfShape& theInvFaces,
                                 const TopTools_DataMapOfShapeShape& theArtInvFaces,
                                 TopTools_IndexedDataMapOfShapeListOfShape& theFLE,
                                 TopTools_DataMapOfShapeListOfShape& theMDone,
                                 TopTools_DataMapOfShapeListOfShape& theDMSF,
                                 TopTools_DataMapOfShapeListOfShape& theMEInfETrim,
                                 TopTools_DataMapOfShapeListOfShape& theDMVEFull,
                                 TopTools_DataMapOfShapeShape& theETrimEInf,
                                 TopTools_IndexedDataMapOfShapeListOfShape& theDMEFInv,
                                 const Standard_Boolean bLookVertToAvoid)
{
  Standard_Integer i, aNb = theFToRebuild.Extent();
  for (i = 1; i <= aNb; ++i) {
    const TopoDS_Shape& aF = theFToRebuild.FindKey(i);
    //
    TopTools_ListOfShape aLE;
    theFLE.Add(aF, aLE);
    theMDone.Bind(aF, aLE);
    //
    const TopTools_ListOfShape& aLFIm = theFImages.FindFromKey(aF);
    TopTools_ListIteratorOfListOfShape aItLF(aLFIm);
    for (; aItLF.More(); aItLF.Next()) {
      const TopoDS_Shape& aFIm = aItLF.Value();
      TopExp_Explorer aExp(aFIm, TopAbs_EDGE);
      for (; aExp.More(); aExp.Next()) {
        const TopoDS_Shape& aE = aExp.Current();
        // save connection to untrimmed face
        if (theDMSF.IsBound(aE)) {
          TopTools_ListOfShape& aLF = theDMSF.ChangeFind(aE);
          AppendToList(aLF, aF);
        }
        else {
          TopTools_ListOfShape aLF;
          aLF.Append(aF);
          theDMSF.Bind(aE, aLF);
        }
        //
        // save connection to untrimmed edge
        const TopoDS_Shape& aEInf = theETrimEInf.Find(aE);
        if (theMEInfETrim.IsBound(aEInf)) {
          TopTools_ListOfShape& aLETrim = theMEInfETrim.ChangeFind(aEInf);
          AppendToList(aLETrim, aE);
        }
        else {
          TopTools_ListOfShape aLETrim;
          aLETrim.Append(aE);
          theMEInfETrim.Bind(aEInf, aLETrim);
        }
        //
        TopExp_Explorer aExpV(aE, TopAbs_VERTEX);
        for (; aExpV.More(); aExpV.Next()) {
          const TopoDS_Shape& aV = aExpV.Current();
          // save connection to face
          if (theDMSF.IsBound(aV)) {
            TopTools_ListOfShape& aLFV = theDMSF.ChangeFind(aV);
            AppendToList(aLFV, aF);
          }
          else {
            TopTools_ListOfShape aLFV;
            aLFV.Append(aF);
            theDMSF.Bind(aV, aLFV);
          }
          //
          if (bLookVertToAvoid) {
            // save connection to edges
            if (theDMVEFull.IsBound(aV)) {
              TopTools_ListOfShape& aLEV = theDMVEFull.ChangeFind(aV);
              AppendToList(aLEV, aE);
            }
            else {
              TopTools_ListOfShape aLEV;
              aLEV.Append(aE);
              theDMVEFull.Bind(aV, aLEV);
            }
          }
        }
      }
    }
    //
    if (bLookVertToAvoid) {
      // get edges of invalid faces (from invalid splits only)
      if (!theInvFaces.Contains(aF) || theArtInvFaces.IsBound(aF)) {
        continue;
      }
      //
      const TopTools_ListOfShape& aLFInv = theInvFaces.FindFromKey(aF);
      aItLF.Initialize(aLFInv);
      for (; aItLF.More(); aItLF.Next()) {
        const TopoDS_Shape& aFInv = aItLF.Value();
        TopExp_Explorer aExp(aFInv, TopAbs_EDGE);
        for (; aExp.More(); aExp.Next()) {
          const TopoDS_Shape& aE = aExp.Current();
          if (theDMEFInv.Contains(aE)) {
            TopTools_ListOfShape& aLF = theDMEFInv.ChangeFromKey(aE);
            AppendToList(aLF, aF);
          }
          else {
            TopTools_ListOfShape aLF;
            aLF.Append(aF);
            theDMEFInv.Add(aE, aLF);
          }
        }
      }
    }
  }
}

//=======================================================================
//function : FindVerticesToAvoid
//purpose  : 
//=======================================================================
void FindVerticesToAvoid(const TopTools_IndexedDataMapOfShapeListOfShape& theDMEFInv,
                         TopTools_DataMapOfShapeListOfShape& theDMVEFull,
                         const TopTools_IndexedMapOfShape& theInvEdges,
                         const TopTools_IndexedMapOfShape& theValidEdges,
                         TopTools_MapOfShape& theMVRInv)
{
  TopTools_MapOfShape aMFence;
  Standard_Integer i, aNb = theDMEFInv.Extent();
  for (i = 1; i <= aNb; ++i) {
    const TopTools_ListOfShape& aLFInv = theDMEFInv(i);
    if (aLFInv.Extent() == 1) {
      continue;
    }
    //
    const TopoDS_Shape& aE = theDMEFInv.FindKey(i);
    if (!theInvEdges.Contains(aE) || theValidEdges.Contains(aE)) {
      continue;
    }
    //
    TopExp_Explorer aExp(aE, TopAbs_VERTEX);
    for (; aExp.More(); aExp.Next()) {
      const TopoDS_Shape& aV = aExp.Current();
      if (!theDMVEFull.IsBound(aV)) {
        theMVRInv.Add(aV);
        continue;
      }
      //
      const TopTools_ListOfShape& aLE = theDMVEFull.Find(aV);
      TopTools_ListIteratorOfListOfShape aItLE(aLE);
      for (; aItLE.More(); aItLE.Next()) {
        const TopoDS_Shape& aEV = aItLE.Value();
        if (!theInvEdges.Contains(aEV) && !theDMEFInv.Contains(aEV)) {
          break;
        }
      }
      if (!aItLE.More()) {
        theMVRInv.Add(aV);
      }
    }
  }
}

//=======================================================================
//function : FindFacesForIntersection
//purpose  : 
//=======================================================================
void FindFacesForIntersection(const TopoDS_Shape& theFInv,
                              const TopTools_IndexedMapOfShape& theME,
                              const TopTools_IndexedDataMapOfShapeListOfShape& theFImages,
                              const TopTools_DataMapOfShapeShape& theArtInvFaces,
                              const TopTools_DataMapOfShapeListOfShape& theDMSF,
                              const TopTools_MapOfShape& theMVInvAll,
                              TopTools_IndexedMapOfShape& theMFAvoid,
                              TopTools_IndexedMapOfShape& theMFInt,
                              TopTools_ListOfShape& theLFImInt)
{
  Standard_Integer i, aNbE = theME.Extent();

  for (i = 1; i <= aNbE; ++i) {
    const TopoDS_Shape& aS = theME(i);
    if (!theDMSF.IsBound(aS)) {
      continue;
    }
    //
    const TopTools_ListOfShape& aLF = theDMSF.Find(aS);
    TopTools_ListIteratorOfListOfShape aItLE(aLF);
    for (; aItLE.More(); aItLE.Next()) {
      const TopoDS_Shape& aF = aItLE.Value();
      if (theMFInt.Contains(aF)) {
        continue;
      }
      //
      const TopTools_ListOfShape& aLFIm = theFImages.FindFromKey(aF);
      // check if this face is close to invalidity
      if (theArtInvFaces.IsBound(theFInv) && theArtInvFaces.IsBound(aF)) {
        if (aS.ShapeType() != TopAbs_EDGE && !theMVInvAll.Contains(aS)) {
          theMFAvoid.Add(aF);
        }
      }
      //
      theMFInt.Add(aF);
      // Debug
      TopTools_ListIteratorOfListOfShape aItLFIm(aLFIm);
      for (; aItLFIm.More(); aItLFIm.Next()) {
        const TopoDS_Shape& aFIm = aItLFIm.Value();
        theLFImInt.Append(aFIm);
      }
    }
  }
}

//=======================================================================
//function : ProcessCommonEdges
//purpose  : 
//=======================================================================
void ProcessCommonEdges(const TopTools_ListOfShape& theLEC,
                        const TopTools_IndexedMapOfShape& theInvEdges,
                        const TopTools_IndexedMapOfShape& theValidEdges,
                        const TopTools_IndexedMapOfShape& theME,
                        const TopTools_DataMapOfShapeShape& theETrimEInf,
                        const TopTools_DataMapOfShapeListOfShape& theMEInfETrim,
                        TopTools_IndexedMapOfShape& theMECV,
                        TopTools_DataMapOfShapeListOfShape& theDMEETrim,
                        TopTools_ListOfShape& theLFEi,
                        TopTools_ListOfShape& theLFEj,
                        TopTools_IndexedMapOfShape& theMEToInt)
{
  // process common edges
  TopTools_ListIteratorOfListOfShape aItLE(theLEC);
  for (; aItLE.More(); aItLE.Next()) {
    const TopoDS_Shape& aEC = aItLE.Value();
    //
    // check first if common edges are valid
    if (theInvEdges.Contains(aEC) && !theValidEdges.Contains(aEC)) {
      continue;
    }
    //
    // common edge should have connection to current invalidity
    if (!theME.Contains(aEC)) {
      TopExp_Explorer aExpV(aEC, TopAbs_VERTEX);
      for (; aExpV.More(); aExpV.Next()) {
        const TopoDS_Shape& aVE = aExpV.Current();
        if (theME.Contains(aVE)) {
          break;
        }
      }
      //
      if (!aExpV.More()) {
        continue;
      }
    }
    //
    const TopoDS_Shape& aEInt = theETrimEInf.Find(aEC);
    // find the edges of the same original edge
    // and take their vertices as well
    const TopTools_ListOfShape& aLVE = theMEInfETrim.Find(aEInt);
    TopTools_ListIteratorOfListOfShape aItLVE(aLVE);
    for (; aItLVE.More(); aItLVE.Next()) {
      const TopoDS_Shape& aECx = aItLVE.Value();
      TopExp::MapShapes(aECx, TopAbs_VERTEX, theMECV);
    }
    //
    // bind unlimited edge to its trimmed part in face to update maps of 
    // images and origins in the future
    if (theDMEETrim.IsBound(aEInt)) {
      TopTools_ListOfShape& aLTAdded = theDMEETrim.ChangeFind(aEInt);
      AppendToList(aLTAdded, aEC);
    }
    else {
      TopTools_ListOfShape aLT;
      aLT.Append(aEC);
      theDMEETrim.Bind(aEInt, aLT);
    }
    //
    AppendToList(theLFEi, aEInt);
    AppendToList(theLFEj, aEInt);
    theMEToInt.Add(aEInt);
  }
}

//=======================================================================
//function : UpdateIntersectedFaces
//purpose  : 
//=======================================================================
void UpdateIntersectedFaces(const TopoDS_Shape& theFInv,
                            const TopoDS_Shape& theFi,
                            const TopoDS_Shape& theFj,
                            const TopTools_ListOfShape& theLFInv,
                            const TopTools_ListOfShape& theLFImi,
                            const TopTools_ListOfShape& theLFImj,
                            const TopTools_ListOfShape& theLFEi,
                            const TopTools_ListOfShape& theLFEj,
                            TopTools_DataMapOfShapeListOfShape& theEdgesOrigins,
                            TopTools_IndexedMapOfShape& theMEToInt)
{
  // Find common edges in these two lists
  TopTools_MapOfShape aMEi;
  TopTools_ListIteratorOfListOfShape aItLE(theLFEi);
  for (; aItLE.More(); aItLE.Next()) {
    const TopoDS_Shape& aE = aItLE.Value();
    aMEi.Add(aE);
  }
  //
  // find origins
  TopTools_IndexedMapOfShape aMEToFindOrigins;
  TopTools_ListOfShape aLEToFindOrigins;
  if (!theFi.IsSame(theFInv)) {
    FindCommonParts(theLFImi, theLFInv, aLEToFindOrigins);
  }
  if (!theFj.IsSame(theFInv)) {
    FindCommonParts(theLFImj, theLFInv, aLEToFindOrigins);
  }
  //
  TopTools_ListOfShape aLEOrInit;
  aItLE.Initialize(aLEToFindOrigins);
  for (; aItLE.More(); aItLE.Next()) {
    const TopoDS_Shape& aEC = aItLE.Value();
    aMEToFindOrigins.Add(aEC);
  }
  //
  FindOrigins(theLFImi, theLFImj, aMEToFindOrigins, theEdgesOrigins, aLEOrInit);
  //
  aItLE.Initialize(theLFEj);
  for (; aItLE.More(); aItLE.Next()) {
    const TopoDS_Shape& aE = aItLE.Value();
    if (aMEi.Contains(aE)) {
      theMEToInt.Add(aE);
      if (aLEOrInit.Extent()) {
        if (theEdgesOrigins.IsBound(aE)) {
          TopTools_ListOfShape& aLEOr = theEdgesOrigins.ChangeFind(aE);
          TopTools_ListIteratorOfListOfShape aItLEOr(aLEOrInit);
          for (; aItLEOr.More(); aItLEOr.Next()) {
            const TopoDS_Shape& aEOr = aItLEOr.Value();
            AppendToList(aLEOr, aEOr);
          }
        }
        else {
          theEdgesOrigins.Bind(aE, aLEOrInit);
        }
      }
    }
  }
}

//=======================================================================
//function : IntersectFaces
//purpose  : 
//=======================================================================
void IntersectFaces(const TopoDS_Shape& theFInv,
                    const TopoDS_Shape& theFi,
                    const TopoDS_Shape& theFj,
                    const TopTools_ListOfShape& theLFInv,
                    const TopTools_ListOfShape& theLFImi,
                    const TopTools_ListOfShape& theLFImj,
                    TopTools_ListOfShape& theLFEi,
                    TopTools_ListOfShape& theLFEj,
                    TopTools_DataMapOfShapeListOfShape& theEdgesOrigins,
                    TopTools_IndexedMapOfShape& theMECV,
                    TopTools_IndexedMapOfShape& theMEToInt)
{
  // intersect faces
  TopAbs_State aSide = TopAbs_OUT;
  TopTools_ListOfShape aLInt1, aLInt2;
  TopoDS_Edge aNullEdge;
  BRepOffset_Tool::Inter3D(TopoDS::Face(theFi), TopoDS::Face(theFj), aLInt1, aLInt2, aSide, aNullEdge);
  //
  if (aLInt1.IsEmpty()) {
    return;
  }
  //
  // find common vertices for trimming edges
  TopTools_ListOfShape aLCV;
  TopTools_ListIteratorOfListOfShape aItLE;
  FindCommonParts(theLFImi, theLFImj, aLCV, TopAbs_VERTEX);
  if (aLCV.Extent() > 1) {
    aItLE.Initialize(aLCV);
    for (; aItLE.More(); aItLE.Next()) {
      const TopoDS_Shape& aCV = aItLE.Value();
      theMECV.Add(aCV);
    }
  }
  //
  // find origins
  TopTools_IndexedMapOfShape aMEToFindOrigins;
  TopTools_ListOfShape aLEToFindOrigins;
  if (!theFi.IsSame(theFInv)) {
    FindCommonParts(theLFImi, theLFInv, aLEToFindOrigins);
  }
  if (!theFj.IsSame(theFInv)) {
    FindCommonParts(theLFImj, theLFInv, aLEToFindOrigins);
  }
  TopTools_ListOfShape aLEOrInit;
  aItLE.Initialize(aLEToFindOrigins);
  for (; aItLE.More(); aItLE.Next()) {
    const TopoDS_Shape& aEC = aItLE.Value();
    aMEToFindOrigins.Add(aEC);
  }
  //
  FindOrigins(theLFImi, theLFImj, aMEToFindOrigins, theEdgesOrigins, aLEOrInit);
  //
  aItLE.Initialize(aLInt1);
  for (; aItLE.More(); aItLE.Next()) {
    const TopoDS_Shape& aEInt = aItLE.Value();
    theLFEi.Append(aEInt);
    theLFEj.Append(aEInt);
    //
    if (aLEOrInit.Extent()) {
      theEdgesOrigins.Bind(aEInt, aLEOrInit);
    }
    //
    theMEToInt.Add(aEInt);
  }
}

//=======================================================================
//function : TrimNewIntersectionEdges
//purpose  : 
//=======================================================================
void TrimNewIntersectionEdges(const TopTools_ListOfShape& theLE,
                              TopTools_DataMapOfShapeListOfShape& theEImages,
                              const TopTools_DataMapOfShapeListOfShape& theEETrim,
                              TopTools_MapOfShape& theMEB,
                              TopTools_MapOfShape& theMVOld,
                              TopTools_MapOfShape& theMVBounds,
                              TopTools_ListOfShape& theLENew,
                              BOPCol_ListOfShape& theLA)
{
  TopTools_ListIteratorOfListOfShape aIt, aIt1;
  aIt.Initialize(theLE);
  for (; aIt.More(); aIt.Next()) {
    const TopoDS_Shape& aE = aIt.Value();
    //
    if (theEETrim.IsBound(aE)) {
      const TopTools_ListOfShape& aLET = theEETrim.Find(aE);
      aIt1.Initialize(aLET);
      for (; aIt1.More(); aIt1.Next()) {
        const TopoDS_Shape& aET = aIt1.Value();
        theMEB.Add(aET);
        TopExp_Explorer aExpV(aET, TopAbs_VERTEX);
        for (; aExpV.More(); aExpV.Next()) {
          const TopoDS_Shape& aV = aExpV.Current();
          theMVOld.Add(aV);
        }
      }
    }
    //
    if (!theEImages.IsBound(aE)) {
      continue;
    }
    //
    TopTools_ListOfShape& aLEIm = theEImages.ChangeFind(aE);
    if (aLEIm.IsEmpty()) {
      theEImages.UnBind(aE);
      continue;
    }
    //
    TopoDS_Shape aCEIm;
    TopTools_MapOfShape aMEVBounds;
    //
    if (aLEIm.Extent() > 2) {
      // fuse these parts
      BOPAlgo_Builder aGFE;
      TopTools_ListIteratorOfListOfShape aItLEIm(aLEIm);
      for (; aItLEIm.More(); aItLEIm.Next()) {
        const TopoDS_Shape& aEIm = aItLEIm.Value();
        aGFE.AddArgument(aEIm);
      }
      //
      // add two bounding vertices of this edge to the operation
      TopoDS_Vertex aV1, aV2;
      TopExp::Vertices(TopoDS::Edge(aE), aV1, aV2);
      //
      if (!aV1.IsNull()) {
        aGFE.AddArgument(aV1);
      }
      //
      if (!aV2.IsNull()) {
        aGFE.AddArgument(aV2);
      }
      //
      aGFE.Perform();
      if (aGFE.ErrorStatus() == 0)
      {
        // get images of bounding vertices to remove splits containing them
        const TopTools_ListOfShape& aLV1Im = aGFE.Modified(aV1);
        if (aLV1Im.Extent()) {
          aMEVBounds.Add(aLV1Im.First());
        }
        else {
          aMEVBounds.Add(aV1);
        }
        //
        const TopTools_ListOfShape& aLV2Im = aGFE.Modified(aV2);
        if (aLV2Im.Extent()) {
          aMEVBounds.Add(aLV2Im.First());
        }
        else {
          aMEVBounds.Add(aV2);
        }
        //
        {
          TopTools_MapIteratorOfMapOfShape aItM(theMVBounds);
          for (; aItM.More(); aItM.Next()) {
            const TopoDS_Shape& aV = aItM.Key();
            const TopTools_ListOfShape& aLVIm = aGFE.Modified(aV);
            if (aLVIm.Extent()) {
              aMEVBounds.Add(aLVIm.First());
            }
            else {
              aMEVBounds.Add(aV);
            }
          }
        }
        //
        aCEIm = aGFE.Shape();
      }
    }
    else {
      aCEIm = aLEIm.First();
    }
    //
    aLEIm.Clear();
    //
    TopExp_Explorer aExp(aCEIm, TopAbs_EDGE);
    for (; aExp.More(); aExp.Next()) {
      const TopoDS_Edge& aEIm = TopoDS::Edge(aExp.Current());
      //
      // check the split to be trimmed
      Standard_Real aFirst, aLast;
      BRep_Tool::Range(aEIm, aFirst, aLast);
      if (Precision::IsInfinite(aFirst) || Precision::IsInfinite(aLast)) {
        continue;
      }
      //
      // check the split not to contain bounding vertices
      TopExp_Explorer aExpV(aEIm, TopAbs_VERTEX);
      for (; aExpV.More(); aExpV.Next()) {
        const TopoDS_Shape& aV = aExpV.Current();
        //
        if (theMVBounds.Contains(aV) || aMEVBounds.Contains(aV)) {
          break;
        }
      }
      //
      if (!aExpV.More()) {
        theLA.Append(aEIm);
        aLEIm.Append(aEIm);
      }
    }
    //
    if (aLEIm.IsEmpty()) {
      theEImages.UnBind(aE);
    }
    else {
      if (!theEETrim.IsBound(aE)) {
        TopTools_ListIteratorOfListOfShape aItLEIm(aLEIm);
        for (; aItLEIm.More(); aItLEIm.Next()) {
          const TopoDS_Shape& aEIm = aItLEIm.Value();
          theLENew.Append(aEIm);
        }
      }
    }
  }
}

//=======================================================================
//function : IntersectEdges
//purpose  : 
//=======================================================================
void IntersectEdges(const BOPCol_ListOfShape& theLA,
                    const TopTools_ListOfShape& theLE,
                    const TopTools_ListOfShape& theLENew,
                    TopTools_DataMapOfShapeListOfShape& theEImages,
                    TopTools_MapOfShape& theVertsToAvoid,
                    TopTools_MapOfShape& theMVBounds,
                    TopTools_MapOfShape& theModifiedEdges,
                    TopTools_MapOfShape& theMENew,
                    TopoDS_Shape& theSplits)
{
  BOPAlgo_Builder aGFA;
  aGFA.SetArguments(theLA);
  aGFA.Perform();
  if (aGFA.ErrorStatus())
  {
    // just copy input to the result
    TopoDS_Compound aSp;
    BRep_Builder aBB;
    aBB.MakeCompound(aSp);
    BOPCol_ListOfShape::Iterator anIt(theLA);
    for (; anIt.More(); anIt.Next()) {
      const TopoDS_Shape& aE = anIt.Value();
      aBB.Add(aSp, aE);
    }
    theSplits = aSp;
    return;
  }
  //
  UpdateImages(theLE, theEImages, aGFA, theModifiedEdges);
  //
  // compound of valid splits
  theSplits = aGFA.Shape();
  //
  // update new edges
  TopTools_ListIteratorOfListOfShape aIt, aIt1;
  aIt.Initialize(theLENew);
  for (; aIt.More(); aIt.Next()) {
    const TopoDS_Shape& aE = aIt.Value();
    const TopTools_ListOfShape& aLEIm = aGFA.Modified(aE);
    if (aLEIm.Extent()) {
      aIt1.Initialize(aLEIm);
      for (; aIt1.More(); aIt1.Next()) {
        const TopoDS_Shape& aEIm = aIt1.Value();
        theMENew.Add(aEIm);
      }
    }
    else {
      theMENew.Add(aE);
    }
  }
  //
  TopTools_MapOfShape aMEInv;
  GetInvalidEdges(aGFA, /*aLENew, */theVertsToAvoid, theMVBounds, aMEInv);
  if (aMEInv.Extent()) {
    // update shape
    TopoDS_Compound aSp;
    BRep_Builder aBB;
    aBB.MakeCompound(aSp);
    TopExp_Explorer aExp(theSplits, TopAbs_EDGE);
    for (; aExp.More(); aExp.Next()) {
      const TopoDS_Shape& aE = aExp.Current();
      if (!aMEInv.Contains(aE)) {
        aBB.Add(aSp, aE);
      }
    }
    theSplits = aSp;
  }
}

//=======================================================================
//function : GetBounds
//purpose  : 
//=======================================================================
void GetBounds(const TopTools_IndexedDataMapOfShapeListOfShape& theFToRebuild,
               const TopTools_IndexedDataMapOfShapeListOfShape& theDMFFIm,
               const TopTools_MapOfShape& theMEB,
               TopoDS_Shape& theBounds)
{
  // make compound of edges contained in the faces splits
  TopoDS_Compound aBounds;
  BRep_Builder aBB;
  aBB.MakeCompound(aBounds);
  //
  TopTools_MapOfShape aMFence;
  //
  Standard_Integer i, aNb = theFToRebuild.Extent();
  for (i = 1; i <= aNb; ++i) {
    const TopoDS_Shape& aF = theFToRebuild.FindKey(i);
    const TopTools_ListOfShape& aLFIm = theDMFFIm.FindFromKey(aF);
    //
    TopTools_ListIteratorOfListOfShape aIt(aLFIm);
    for (; aIt.More(); aIt.Next()) {
      const TopoDS_Shape& aFIm = aIt.Value();
      TopExp_Explorer aExpE(aFIm, TopAbs_EDGE);
      for (; aExpE.More(); aExpE.Next()) {
        const TopoDS_Shape& aEIm = aExpE.Current();
        if (!theMEB.Contains(aEIm) && aMFence.Add(aEIm)) {
          aBB.Add(aBounds, aEIm);
        }
      }
    }
  }
  theBounds = aBounds;
}

//=======================================================================
//function : GetBoundsToUpdate
//purpose  : 
//=======================================================================
void GetBoundsToUpdate(const TopTools_IndexedDataMapOfShapeListOfShape& theFToRebuild,
                       const TopTools_DataMapOfShapeListOfShape& theOEImages,
                       const TopTools_DataMapOfShapeListOfShape& theOEOrigins,
                       const TopTools_MapOfShape& theMEB,
                       TopTools_ListOfShape& theLABounds,
                       TopTools_ListOfShape& theLAValid,
                       TopoDS_Shape& theBounds,
                       Handle(BRepAlgo_AsDes)& theAsDes)
{
  // get all edges
  TopoDS_Compound aBounds;
  BRep_Builder aBB;
  aBB.MakeCompound(aBounds);
  //
  TopTools_MapOfShape aMAValid, aMFence;
  //
  Standard_Integer i, aNb = theFToRebuild.Extent();
  for (i = 1; i <= aNb; ++i) {
    const TopoDS_Shape& aF = theFToRebuild.FindKey(i);
    //
    TopTools_IndexedMapOfShape aMDE;
    const TopTools_ListOfShape& aLFDes = theAsDes->Descendant(aF);
    TopTools_ListIteratorOfListOfShape aItLFDes(aLFDes);
    for (; aItLFDes.More(); aItLFDes.Next()) {
      const TopoDS_Shape& aED = aItLFDes.Value();
      if (!theOEImages.IsBound(aED)) {
        aMDE.Add(aED);
        continue;
      }
      //
      const TopTools_ListOfShape& aLEDIm = theOEImages.Find(aED);
      TopTools_ListIteratorOfListOfShape aItLEDIm(aLEDIm);
      for (; aItLEDIm.More(); aItLEDIm.Next()) {
        const TopoDS_Shape& aEDIm = aItLEDIm.Value();
        aMDE.Add(aEDIm);
      }
    }
    //
    Standard_Integer j, aNbE = aMDE.Extent();
    for (j = 1; j <= aNbE; ++j) {
      const TopoDS_Edge& aEIm = TopoDS::Edge(aMDE(j));
      //
      if (!theMEB.Contains(aEIm) && aMFence.Add(aEIm)) {
        aBB.Add(aBounds, aEIm);
        theLABounds.Append(aEIm);
      }
      //
      if (theOEOrigins.IsBound(aEIm)) {
        const TopTools_ListOfShape& aLO = theOEOrigins.Find(aEIm);
        TopTools_ListIteratorOfListOfShape aItLO(aLO);
        for (; aItLO.More(); aItLO.Next()) {
          const TopoDS_Shape& aEO = aItLO.Value();
          //
          if (aMAValid.Add(aEO)) {
            theLAValid.Append(aEO);
          }
        }
      }
      else {
        if (aMAValid.Add(aEIm)) {
          theLAValid.Append(aEIm);
        }
      }
    }
  }
  theBounds = aBounds;
}

//=======================================================================
//function : GetInvalidEdgesByBounds
//purpose  : 
//=======================================================================
void GetInvalidEdgesByBounds(const TopoDS_Shape& theSplits,
                             const TopoDS_Shape& theBounds,
                             const TopTools_IndexedDataMapOfShapeListOfShape& theFToRebuild,
                             const TopTools_IndexedDataMapOfShapeListOfShape& theDMFFIm,
                             const TopTools_IndexedMapOfShape& theInvEdges,
                             const TopTools_MapOfShape& theMVOld,
                             const TopTools_MapOfShape& theMENew,
                             TopTools_MapOfShape& theVertsToAvoid,
                             TopTools_MapOfShape& theMEInv)
{
  BOPAlgo_Builder aGF;
  aGF.AddArgument(theSplits);
  aGF.AddArgument(theBounds);
  //
  aGF.Perform();
  //
  // invalid vertices
  TopTools_IndexedMapOfShape aMVInv;
  // collect parts for removal
  const BOPDS_PDS& pDS = aGF.PDS();
  //
  // check edge/edge intersections
  const BOPDS_VectorOfInterfEE& aEEs = pDS->InterfEE();
  Standard_Integer i, aNb = aEEs.Extent();
  for (i = 0; i < aNb; ++i) {
    const BOPDS_InterfEE& aEE = aEEs(i);
    const TopoDS_Shape& aE1 = pDS->Shape(aEE.Index1());
    const TopoDS_Shape& aE2 = pDS->Shape(aEE.Index2());
    if (!aEE.HasIndexNew()) {
      continue;
    }
    //
    if (!theInvEdges.Contains(aE2)) {
      theMEInv.Add(aE1);
      //
      TopExp_Explorer aExpV(aE1, TopAbs_VERTEX);
      for (; aExpV.More(); aExpV.Next()) {
        const TopoDS_Shape& aV = aExpV.Current();
        if (!theMVOld.Contains(aV)) {
          aMVInv.Add(aV);
        }
      }
    }
  }
  //
  // add for check also the vertices connected only to new edges
  TopTools_IndexedDataMapOfShapeListOfShape aDMVE;
  TopExp::MapShapesAndAncestors(theSplits, TopAbs_VERTEX, TopAbs_EDGE, aDMVE);
  //
  aNb = aDMVE.Extent();
  for (i = 1; i <= aNb; ++i) {
    const TopoDS_Shape& aV = aDMVE.FindKey(i);
    if (aMVInv.Contains(aV)) {
      continue;
    }
    //
    Standard_Boolean bNew = Standard_False, bOld = Standard_False;
    const TopTools_ListOfShape& aLEx = aDMVE(i);
    TopTools_ListIteratorOfListOfShape aIt(aLEx);
    for (; aIt.More(); aIt.Next()) {
      const TopoDS_Shape& aE = aIt.Value();
      if (theMENew.Contains(aE)) {
        bNew = Standard_True;
      }
      else {
        bOld = Standard_True;
      }
      //
      if (bNew && bOld) {
        break;
      }
    }
    //
    if (!bNew || !bOld) {
      aMVInv.Add(aV);
    }
  }
  //
  Handle(IntTools_Context) aCtx = new IntTools_Context;
  // filter vertices
  Standard_Integer iv, aNbIV = aMVInv.Extent(), aNbF = theFToRebuild.Extent();
  for (iv = 1; iv <= aNbIV; ++iv) {
    const TopoDS_Vertex& aV = TopoDS::Vertex(aMVInv(iv));
    const gp_Pnt& aP = BRep_Tool::Pnt(aV);
    Standard_Real aTolV = BRep_Tool::Tolerance(aV);
    //
    for (i = 1; i <= aNbF; ++i) {
      const TopoDS_Shape& aF = theFToRebuild.FindKey(i);
      const TopTools_ListOfShape& aLFIm = theDMFFIm.FindFromKey(aF);
      //
      TopTools_ListIteratorOfListOfShape aItLF(aLFIm);
      for (; aItLF.More(); aItLF.Next()) {
        const TopoDS_Face& aFIm = TopoDS::Face(aItLF.Value());
        if (aCtx->IsValidPointForFace(aP, aFIm, aTolV)) {
          break;
        }
      }
      //
      if (aItLF.More()) {
        break;
      }
    }
    //
    if (i > aNbF) {
      theVertsToAvoid.Add(aV);
      if (aDMVE.Contains(aV)) {
        const TopTools_ListOfShape& aLEInv = aDMVE.FindFromKey(aV);
        TopTools_ListIteratorOfListOfShape aItLEInv(aLEInv);
        for (; aItLEInv.More(); aItLEInv.Next()) {
          const TopoDS_Shape& aEInv = aItLEInv.Value();
          theMEInv.Add(aEInv);
        }
      }
    }
  }
}

//=======================================================================
//function : UpdateNewIntersectionEdges
//purpose  : 
//=======================================================================
void UpdateNewIntersectionEdges(const TopTools_ListOfShape& theLE,
                                const TopTools_DataMapOfShapeListOfShape& theMELF,
                                const TopTools_DataMapOfShapeListOfShape& theEImages,
                                TopTools_DataMapOfShapeListOfShape& theEdgesOrigins,
                                TopTools_DataMapOfShapeListOfShape& theOEImages,
                                TopTools_DataMapOfShapeListOfShape& theOEOrigins,
                                TopTools_DataMapOfShapeShape& theETrimEInf,
                                TopTools_DataMapOfShapeListOfShape& theEETrim,
                                TopTools_MapOfShape& theModifiedEdges,
                                Handle(BRepAlgo_AsDes)& theAsDes)
{
  TopTools_ListOfShape aLEImEmpty;
  TopTools_ListIteratorOfListOfShape aIt, aIt1;
  // update global maps of images and origins with new splits
  aIt.Initialize(theLE);
  for (; aIt.More(); aIt.Next()) {
    const TopoDS_Shape& aE = aIt.Value();
    //
    /*if (!theEImages.IsBound(aE)) {
      continue;
    }*/
    // new images
    const TopTools_ListOfShape& aLENew = 
      theEImages.IsBound(aE) ? theEImages.Find(aE) : aLEImEmpty;
    //
    // save connection to untrimmed edge for the next steps
    aIt1.Initialize(aLENew);
    for (; aIt1.More(); aIt1.Next()) {
      const TopoDS_Shape& aET = aIt1.Value();
      theETrimEInf.Bind(aET, aE);
      theModifiedEdges.Add(aET);
    }
    //
    // check if it is existing edge
    if (!theEETrim.IsBound(aE)) {
      const TopTools_ListOfShape& aLF = theMELF.Find(aE);
      // the edge is new
      // add this edge to AsDes
      aIt1.Initialize(aLF);
      for (; aIt1.More(); aIt1.Next()) {
        const TopoDS_Shape& aF = aIt1.Value();
        theAsDes->Add(aF, aE);
      }
      //
      // add aE to the images
      theOEImages.Bind(aE, aLENew);
      theModifiedEdges.Add(aE);
      //
      // add to origins
      TopTools_ListIteratorOfListOfShape aItNew(aLENew);
      for (; aItNew.More(); aItNew.Next()) {
        const TopoDS_Shape& aENew = aItNew.Value();
        if (theOEOrigins.IsBound(aENew)) {
          TopTools_ListOfShape& aEOrigins = theOEOrigins.ChangeFind(aENew);
          AppendToList(aEOrigins, aE);
        }
        else {
          TopTools_ListOfShape aEOrigins;
          aEOrigins.Append(aE);
          theOEOrigins.Bind(aENew, aEOrigins);
        }
      }
      //
      // update connection to initial origins
      if (theEdgesOrigins.IsBound(aE)) {
        const TopTools_ListOfShape& aLEOrInit = theEdgesOrigins.Find(aE);
        aIt1.Initialize(aLENew);
        for (; aIt1.More(); aIt1.Next()) {
          const TopoDS_Shape& aENew = aIt1.Value();
          if (theEdgesOrigins.IsBound(aENew)) {
            TopTools_ListOfShape& aLENewOr = theEdgesOrigins.ChangeFind(aENew);
            TopTools_ListIteratorOfListOfShape aItOrInit(aLEOrInit);
            for (; aItOrInit.More(); aItOrInit.Next()) {
              const TopoDS_Shape& aEOr = aItOrInit.Value();
              AppendToList(aLENewOr, aEOr);
            }
          }
          else {
            theEdgesOrigins.Bind(aENew, aLEOrInit);
          }
        }
      }
      //
      continue;
    }
    //
    // old images
    const TopTools_ListOfShape& aLEOld = theEETrim.Find(aE);
    //
    // list of initial origins
    TopTools_ListOfShape anInitOrigins;
    //
    // it is necessary to replace the old edges with new ones
    aIt1.Initialize(aLEOld);
    for (; aIt1.More(); aIt1.Next()) {
      const TopoDS_Shape& aEOld = aIt1.Value();
      //
      if (theOEOrigins.IsBound(aEOld)) {
        // get its origins
        const TopTools_ListOfShape& aEOrigins = theOEOrigins.Find(aEOld);
        //
        TopTools_ListIteratorOfListOfShape aItOr(aEOrigins);
        for (; aItOr.More(); aItOr.Next()) {
          const TopoDS_Shape& aEOr = aItOr.Value();
          //
          theModifiedEdges.Add(aEOr);
          //
          TopTools_ListOfShape& aEImages = theOEImages.ChangeFind(aEOr);
          //
          // remove old edge from images
          TopTools_ListIteratorOfListOfShape aItIm(aEImages);
          for (; aItIm.More(); ) {
            const TopoDS_Shape& aEIm = aItIm.Value();
            if (aEIm.IsSame(aEOld)) {
              aEImages.Remove(aItIm);
            }
            else {
              aItIm.Next();
            }
          }
          //
          // add new images
          TopTools_ListIteratorOfListOfShape aItNew(aLENew);
          for (; aItNew.More(); aItNew.Next()) {
            const TopoDS_Shape& aENew = aItNew.Value();
            AppendToList(aEImages, aENew);
            if (theOEOrigins.IsBound(aENew)) {
              TopTools_ListOfShape& aENewOrigins = theOEOrigins.ChangeFind(aENew);
              AppendToList(aENewOrigins, aEOr);
            }
            else {
              TopTools_ListOfShape aENewOrigins;
              aENewOrigins.Append(aEOr);
              theOEOrigins.Bind(aENew, aENewOrigins);
            }
          }
        }
      }
      else {
        // add to images
        theOEImages.Bind(aEOld, aLENew);
        //
        theModifiedEdges.Add(aEOld);
        //
        // add to origins
        TopTools_ListIteratorOfListOfShape aItNew(aLENew);
        for (; aItNew.More(); aItNew.Next()) {
          const TopoDS_Shape& aENew = aItNew.Value();
          if (theOEOrigins.IsBound(aENew)) {
            TopTools_ListOfShape& aEOrigins = theOEOrigins.ChangeFind(aENew);
            AppendToList(aEOrigins, aEOld);
          }
          else {
            TopTools_ListOfShape aEOrigins;
            aEOrigins.Append(aEOld);
            theOEOrigins.Bind(aENew, aEOrigins);
          }
        }
      }
      //
      // update connection to initial shape
      if (theEdgesOrigins.IsBound(aEOld)) {
        const TopTools_ListOfShape& aLEOrInit = theEdgesOrigins.Find(aEOld);
        TopTools_ListIteratorOfListOfShape aItEOrInit(aLEOrInit);
        for (; aItEOrInit.More(); aItEOrInit.Next()) {
          const TopoDS_Shape& aEOrInit = aItEOrInit.Value();
          AppendToList(anInitOrigins, aEOrInit);
        }
      }
    }
    //
    if (anInitOrigins.Extent()) {
      TopTools_ListIteratorOfListOfShape aItNew(aLENew);
      for (; aItNew.More(); aItNew.Next()) {
        const TopoDS_Shape& aENew = aItNew.Value();
        if (theEdgesOrigins.IsBound(aENew)) {
          TopTools_ListOfShape& aLENewOr = theEdgesOrigins.ChangeFind(aENew);
          TopTools_ListIteratorOfListOfShape aItOrInit(anInitOrigins);
          for (; aItOrInit.More(); aItOrInit.Next()) {
            const TopoDS_Shape& aEOr = aItOrInit.Value();
            AppendToList(aLENewOr, aEOr);
          }
        }
        else {
          theEdgesOrigins.Bind(aENew, anInitOrigins);
        }
      }
    }
  }
}

//=======================================================================
//function : AppendToList
//purpose  : to be used for very short lists (1-3 elements)
//=======================================================================
void AppendToList(TopTools_ListOfShape& theL,
                  const TopoDS_Shape& theS)
{
  TopTools_ListIteratorOfListOfShape aIt(theL);
  for (; aIt.More(); aIt.Next()) {
    const TopoDS_Shape& aS = aIt.Value();
    if (aS.IsSame(theS)) {
      break;
    }
  }
  if (!aIt.More()) {
    theL.Append(theS);
  }
}

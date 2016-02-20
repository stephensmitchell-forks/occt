// Copyright (c) 2015 OPEN CASCADE SAS
//
// This file is part of commercial software by OPEN CASCADE SAS.
//
// This software is furnished in accordance with the terms and conditions
// of the contract and with the inclusion of this copyright notice.
// This software or any other copy thereof may not be provided or otherwise
// be made available to any third party.
// No ownership title to the software is transferred hereby.
//
// OPEN CASCADE SAS makes no representation or warranties with respect to the
// performance of this software, and specifically disclaims any responsibility
// for any damages, special or consequential, connected with its use.

#ifndef MeshDataSource_H
#define MeshDataSource_H

#include <MeshScalarProperty.h>

#include <MeshVS_DataSource.hxx>
#include <MeshVS_EntityType.hxx>
#include <MeshVS_ElementalColorPrsBuilder.hxx>
#include <MeshVS_NodalColorPrsBuilder.hxx>
#include <MeshVS_PrsBuilder.hxx>
#include <Message_ProgressIndicator.hxx>
#include <NCollection_DataMap.hxx>
#include <TColStd_HArray2OfInteger.hxx>
#include <TColStd_HArray2OfReal.hxx>

#if defined(_WIN32)
  #define ftell64(a)     _ftelli64(a)
  #define fseek64(a,b,c) _fseeki64(a,b,c)
#else
  #define ftell64(a)     ftello(a)
  #define fseek64(a,b,c) fseeko(a,b,c)
#endif

//! Material definition.
struct MeshMaterial
{
  Graphic3d_MaterialAspect Aspect;
  TCollection_AsciiString  Texture;
};

//! Mesh sub-group definition
struct MeshGroup
{
  TCollection_AsciiString Name;      //!< group name
  Standard_Integer        NodeLower; //!< index of lower Vertex
  Standard_Integer        NodeUpper; //!< index of upper Vertex
  Standard_Integer        ElemLower; //!< index of lower Element
  Standard_Integer        ElemUpper; //!< index of upper Element
  MeshMaterial            Material;  //!< associated material

  MeshGroup() : NodeLower (-1), NodeUpper (-1), ElemLower (-1), ElemUpper (-1) {}

};

//! The DataSource for working with mesh data.
class MeshDataSource : public MeshVS_DataSource
{

public:

  //! Empty constructor.
  Standard_EXPORT MeshDataSource();

  //! Clear mesh structure.
  Standard_EXPORT void Clear();

  //! Read the mesh from specified file.
  virtual Standard_Boolean Read (const TCollection_AsciiString&           theFile,
                                 const Handle(Message_ProgressIndicator)& theProgress = NULL,
                                 const Standard_Integer                   theIndexLimit  = -1,
                                 const Standard_Integer                   theMemoryLimitMiB = -1) = 0;

  //! Returns geometry information about node or element by co-ordinates.
  //! For element this method must return all its nodes co-ordinates in the strict order: X, Y, Z and
  //! with nodes order is the same as in wire bounding the face or link.
  //! @param theIsElement flag to retrieve information about Element or Node
  //! @param theNbNodes   number of nodes of element, it is recommended to return 1 for node
  //! @param theType      element type
  Standard_EXPORT virtual Standard_Boolean GetGeom (const Standard_Integer theID,
                                                    const Standard_Boolean theIsElement,
                                                    TColStd_Array1OfReal&  theCoords,
                                                    Standard_Integer&      theNbNodes,
                                                    MeshVS_EntityType&     theType) const Standard_OVERRIDE;

  //! This method is similar to GetGeom, but returns only element or node type.
  virtual Standard_Boolean GetGeomType (const Standard_Integer /*theID*/,
                                        const Standard_Boolean theIsElement,
                                        MeshVS_EntityType&     theType) const Standard_OVERRIDE
  {
    theType = theIsElement ? MeshVS_ET_Face : MeshVS_ET_Node;
    return Standard_True;
  }

  //! Return pointer which represents element or node data structure.
  virtual Standard_Address GetAddr (const Standard_Integer /*theID*/,
                                    const Standard_Boolean /*theIsElement*/) const Standard_OVERRIDE { return NULL; }

  //! This method returns information about what node this element consist of.
  Standard_EXPORT virtual Standard_Boolean GetNodesByElement (const Standard_Integer   theID,
                                                              TColStd_Array1OfInteger& theNodeIDs,
                                                              Standard_Integer&        theNbNodes) const Standard_OVERRIDE;

  //! This method returns map of all nodes the object consist of.
  virtual const TColStd_PackedMapOfInteger& GetAllNodes()    const Standard_OVERRIDE { return myFreeNodes; }

  //! This method returns map of all elements the object consist of.
  virtual const TColStd_PackedMapOfInteger& GetAllElements() const Standard_OVERRIDE { return myUsedElements; }

  //! This method return normal of node ranknode of face Id,
  //! which is using for smooth shading presentation.
  //! Returns false if normal isn't defined.
  Standard_EXPORT virtual Standard_Boolean GetNodeNormal (const Standard_Integer theRankNode,
                                                          const Standard_Integer theElementId,
                                                          Standard_Real&         theNX,
                                                          Standard_Real&         theNY,
                                                          Standard_Real&         theNZ) const Standard_OVERRIDE;

public:

  //! Return file comments.
  const NCollection_Sequence<TCollection_AsciiString>& FileComments() const
  {
    return myComments;
  }

  //! Return list of sub-groups with materials.
  const NCollection_Vector<MeshGroup>& Groups() { return myGroups; }

  //! Return overall nodes number.
  Standard_Integer NbNodes()    const { return myNodes.Length(); }

  //! Return vector of nodes.
  const NCollection_Vector<Graphic3d_Vec3>& Nodes() const { return myNodes; }

  //! Return overall elements number.
  Standard_Integer NbElements() const { return myElements.Length(); }

  //! Determine elements number.
  Standard_EXPORT void NbElements (Standard_Integer& theNbTris,
                                   Standard_Integer& theNbQuads) const;

  //! Return vector of elements.
  const NCollection_Vector<Graphic3d_Vec4i>& Elements() const { return myElements; }

  //! Return per-node scalar properties.
  const MeshScalarProperties& NodalQuantities()     const { return myNodalQuantities; }

  //! Return per-element scalar properties.
  const MeshScalarProperties& ElementalQuantities() const { return myElementalQuantities; }

  //! Return true if per-node colors have been assigned
  Standard_Boolean HasNodalColors() const
  {
    return myNodalColors.Length() == myNodes.Length()
       && !myNodalColors.IsEmpty();
  }

  //! Return vector of nodal colors (optional).
  const NCollection_Vector<Graphic3d_Vec4ub>& NodalColors() const { return myNodalColors; }

  //! Return true if per-node normals have been assigned
  Standard_Boolean HasNormals() const
  {
    return myNormals.Length() == myNodes.Length()
       && !myNormals.IsEmpty();
  }

  //! Return vector of nodal normals (optional).
  const NCollection_Vector<Graphic3d_Vec3>& Normals() const { return myNormals; }

  //! Return true if per-node UV parameters have been assigned
  Standard_Boolean HasUV() const
  {
    return myNodesUV.Length() == myNodes.Length()
       && !myNodesUV.IsEmpty();
  }

  //! Return vector of nodal UV parameters (optional).
  const NCollection_Vector<Graphic3d_Vec2>& NodesUV() const { return myNodesUV; }

  //! Return true if texture has been assigned.
  Standard_Boolean HasTexture() const
  {
    Standard_Integer aTextureCount = 0;
    for (auto anIter = myGroups.cbegin(); anIter != myGroups.cend(); ++anIter)
    {
      aTextureCount += (anIter->Material.Texture.IsEmpty()) ? 0 : 1;
    }
    return !myGroups.IsEmpty()
        //&& !myGroups.First().Material.Texture.IsEmpty()
        && aTextureCount
        &&  HasUV();
  }

  //! Return assigned texture.
  TCollection_AsciiString Texture() const
  {
    return HasTexture() ? myGroups.First().Material.Texture : "";
  }

public:

  //! Fill-in nodal (MeshVS_NodalColorPrsBuilder) or elemental (MeshVS_ElementalColorPrsBuilder) builder.
  Standard_EXPORT void FillPropertyBuilder (const Handle(MeshVS_PrsBuilder)&  theBuilder,
                                            const Handle(MeshScalarProperty)& theProperty) const;

  //! Fill-in nodal (MeshVS_NodalColorPrsBuilder) builder by colors.
  Standard_EXPORT void FillNodalColorsBuilder (const Handle(MeshVS_NodalColorPrsBuilder)& theBuilder) const;

public:

  //! Append new vertex.
  void AddNodePosition (const Graphic3d_Vec3& thePosition)
  {
    myNodes.Append (thePosition);
    myUsedNodes.Add (myNodes.Upper());
  }

  //! Append new vertex.
  void AddNodePosition (const gp_XYZ& thePosition)
  {
    myNodes.Append (Graphic3d_Vec3 ((float )thePosition.X(),
                                    (float )thePosition.Y(),
                                    (float )thePosition.Z()));
    myUsedNodes.Add  (myNodes.Upper());
  }

  //! Append new normal.
  void AddNodeNormal (const Graphic3d_Vec3& theNormal)
  {
    myNormals.Append (theNormal);
  }

  //! Append new normal.
  void AddNodeNormal (const gp_XYZ& theNormal)
  {
    myNormals.Append (Graphic3d_Vec3 ((float )theNormal.X(),
                                      (float )theNormal.Y(),
                                      (float )theNormal.Z()));
  }

  //! Append new UV parameters pair.
  void AddNodeUV (const Graphic3d_Vec2& theUV)
  {
    myNodesUV.Append (theUV);
  }

  //! Append new color.
  void AddNodeColor (const Graphic3d_Vec4ub& theColor)
  {
    myNodalColors.Append (theColor);
  }

  //! Append node property.
  void AddNodeProperty (const Standard_Integer thePropIndex,
                        const Standard_Real    theValue)
  {
    myNodalQuantities.ChangeValue (thePropIndex)->AppendValue (theValue);
  }

  //! Append new element.
  void AddElement (const Graphic3d_Vec4i& theNodeIndices)
  {
    myElements.Append (theNodeIndices);
    myUsedElements.Add (myElements.Upper());
  }

  //! Append element property.
  void AddElementProperty (const Standard_Integer thePropIndex,
                           const Standard_Real    theValue)
  {
    myElementalQuantities.ChangeValue (thePropIndex)->AppendValue (theValue);
  }

protected:

  TColStd_PackedMapOfInteger           myUsedNodes;           //!< used node indices
  TColStd_PackedMapOfInteger           myFreeNodes;           //!< indices of free nodes
  TColStd_PackedMapOfInteger           myUsedElements;        //!< used element indices
  NCollection_Sequence<TCollection_AsciiString>
                                       myComments;            //!< file comments
  NCollection_Vector<Graphic3d_Vec3>   myNodes;               //!< vector of nodes
  NCollection_Vector<Graphic3d_Vec3>   myNormals;             //!< vector of nodal normals (optional)
  NCollection_Vector<Graphic3d_Vec2>   myNodesUV;             //!< vector of UV nodes (optional)
  NCollection_Vector<Graphic3d_Vec4ub> myNodalColors;         //!< vector of nodal colors  (optional)
  NCollection_Vector<Graphic3d_Vec4i>  myElements;            //!< vector of elements
  MeshScalarProperties                 myNodalQuantities;     //!< per-node    scalar quantities
  MeshScalarProperties                 myElementalQuantities; //!< per-element scalar quantities

  NCollection_Vector<MeshGroup>        myGroups;              //!< sub-groups with materials

public:

  DEFINE_STANDARD_RTTIEXT(MeshDataSource, MeshVS_DataSource)

};

DEFINE_STANDARD_HANDLE(MeshDataSource, MeshVS_DataSource)

#endif // MeshDataSource_H

// Created on: 2013-08-27
// Created by: Denis BOGOLEPOV
// Copyright (c) 2013-2014 OPEN CASCADE SAS
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

#ifndef _OpenGl_SceneGeometry_Header
#define _OpenGl_SceneGeometry_Header

#include <BVH_Geometry.hxx>
#include <BVH_Triangulation.hxx>
#include <BVH_BinnedBuilder.hxx>
#include <NCollection_StdAllocator.hxx>
#include <OpenGl_TextureBufferArb.hxx>
#include <OpenGl_Texture.hxx>
#include <OpenGl_Sampler.hxx>

class  OpenGl_Element;
struct OpenGl_ElementNode;
class  OpenGl_Group;
class  OpenGl_Structure;
class  OpenGl_PrimitiveArray;

namespace OpenGl_Raytrace
{
  //! Checks to see if the group contains ray-trace geometry.
  Standard_Boolean IsRaytracedGroup (const OpenGl_Group* theGroup);

  //! Checks to see if the element contains ray-trace geometry.
  Standard_Boolean IsRaytracedElement (const OpenGl_ElementNode* theNode);

  //! Checks to see if the element contains ray-trace geometry.
  Standard_Boolean IsRaytracedElement (const OpenGl_Element* theElement);

  //! Checks to see if the structure contains ray-trace geometry.
  Standard_Boolean IsRaytracedStructure (const OpenGl_Structure* theStructure);
}

//! Stores properties of surface material.
class OpenGl_RaytraceMaterial
{
public:

  //! Ambient reflection coefficient.
  BVH_Vec4f Ambient;

  //! Diffuse reflection coefficient.
  BVH_Vec4f Diffuse;

  //! Glossy reflection coefficient.
  BVH_Vec4f Specular;

  //! Material emission.
  BVH_Vec4f Emission;

  //! Specular reflection coefficient.
  BVH_Vec4f Reflection;

  //! Specular refraction coefficient.
  BVH_Vec4f Refraction;

  //! Material transparency.
  BVH_Vec4f Transparency;

  //! Texture transformation matrix.
  BVH_Mat4f TextureTransform;

  //! Physically-based material properties (used in path tracing engine).
  struct Physical
  {
    //! Weight of the diffuse BRDF.
    BVH_Vec4f Kd;

    //! Weight of the reflection BRDF.
    BVH_Vec4f Kr;

    //! Weight of the transmission BTDF.
    BVH_Vec4f Kt;

    //! Weight of the Blinn's glossy BRDF.
    BVH_Vec4f Ks;

    //! Self-emitted radiance.
    BVH_Vec4f Le;

    //! Fresnel coefficients.
    BVH_Vec4f Fresnel;

    //! Absorption color for the transmission BSDF.
    BVH_Vec4f Absorption;

  } BSDF;

public:

  //! Creates new default material.
  OpenGl_RaytraceMaterial();

  //! Creates new material with specified properties.
  OpenGl_RaytraceMaterial (const BVH_Vec4f& theAmbient,
                           const BVH_Vec4f& theDiffuse,
                           const BVH_Vec4f& theSpecular);

  //! Creates new material with specified properties.
  OpenGl_RaytraceMaterial (const BVH_Vec4f& theAmbient,
                           const BVH_Vec4f& theDiffuse,
                           const BVH_Vec4f& theSpecular,
                           const BVH_Vec4f& theEmission,
                           const BVH_Vec4f& theTranspar);

  //! Creates new material with specified properties.
  OpenGl_RaytraceMaterial (const BVH_Vec4f& theAmbient,
                           const BVH_Vec4f& theDiffuse,
                           const BVH_Vec4f& theSpecular,
                           const BVH_Vec4f& theEmission,
                           const BVH_Vec4f& theTranspar,
                           const BVH_Vec4f& theReflection,
                           const BVH_Vec4f& theRefraction);

  //! Returns packed (serialized) representation of material.
  const Standard_ShortReal* Packed()
  {
    return reinterpret_cast<Standard_ShortReal*> (this);
  }
};

//! Stores properties of OpenGL light source.
class OpenGl_RaytraceLight
{
public:

  //! Diffuse intensity (in terms of OpenGL).
  BVH_Vec4f Diffuse;

  //! Position of light source (in terms of OpenGL).
  BVH_Vec4f Position;

public:

  //! Creates new light source.
  OpenGl_RaytraceLight (const BVH_Vec4f& theDiffuse,
                        const BVH_Vec4f& thePosition);

  //! Returns packed (serialized) representation of light source.
  const Standard_ShortReal* Packed()
  {
    return reinterpret_cast<Standard_ShortReal*> (this);
  }
};

//! Triangulation of single OpenGL primitive array.
class OpenGl_TriangleSet : public BVH_Triangulation<Standard_ShortReal, 3>
{
public:

  //! Value of invalid material index to return in case of errors.
  static const Standard_Integer INVALID_MATERIAL = -1;

public:

  //! Creates new OpenGL element triangulation.
  OpenGl_TriangleSet (const Standard_Size theArrayID);

  //! Releases resources of OpenGL element triangulation.
  ~OpenGl_TriangleSet()
  {
    //
  }

  //! Returns Id of associated primitive array.
  Standard_Size AssociatedPArrayID() const
  {
    return myArrayID;
  }

  //! Returns material index of triangle set.
  Standard_Integer MaterialIndex() const
  {
    if (Elements.size() == 0)
    {
      return INVALID_MATERIAL;
    }

    return Elements.front().w();
  }

  //! Sets material index for entire triangle set.
  void SetMaterialIndex (Standard_Integer theMatID)
  {
    for (Standard_Size anIdx = 0; anIdx < Elements.size(); ++anIdx)
    {
      Elements[anIdx].w() = theMatID;
    }
  }

  //! Returns AABB of the given object.
  using BVH_Triangulation<Standard_ShortReal, 3>::Box;

  //! Returns AABB of primitive set.
  BVH_BoxNt Box() const;

  //! Returns centroid position along the given axis.
  Standard_ShortReal Center (const Standard_Integer theIndex, const Standard_Integer theAxis) const;

public:

  BVH_Array3f Normals; //!< Array of vertex normals.

  BVH_Array2f TexCrds; //!< Array of vertex UV coords.

private:

  Standard_Size myArrayID; //!< ID of associated primitive array.

};

//! Set of clipping planes specific for OpenGL primitive array.
class OpenGl_RaytraceClipPlanes
{
public:

  //! Maximum number of clipping planes used in ray-tracing for each
  //! OpenGL primitive array. This is not implementation restriction,
  //! but it is reasonable to limit max number of planes in order to
  //! simplify GLSL data representation.
  static const Standard_Integer MAX_PLANE_NUMBER = 8;

  //! State of clipping plane.
  enum ClipPlaneState {

    CLIP_PLANE_OFF   = -1,  //!< plane is deactivated
    CLIP_PLANE_VIEW  =  0,  //!< plane is in view space
    CLIP_PLANE_WORLD =  1   //!< plane is in world space
  };

  //! Wrapper for clipping plane configuration.
  class ClipPlane
  {
  public:

    //! Creates new clipping plane wrapper.
    ClipPlane (BVH_Vec4f& theSettings,
               BVH_Vec4f& theEquation) : mySettings (theSettings),
                                         myEquation (theEquation) {}

    //! Sets 4D equation vector for clipping plane.
    void SetEquation (const BVH_Vec4d& theEquation, const ClipPlaneState theState = CLIP_PLANE_WORLD)
    {
      for (Standard_Integer anIndex = 0; anIndex < 4; ++anIndex)
      {
        myEquation[anIndex] = static_cast<Standard_ShortReal> (theEquation[anIndex]);
      }

      SetState (theState);
    }

    //! Returns state of clipping plane.
    ClipPlaneState State()
    {
      return static_cast<ClipPlaneState> (mySettings.x());
    }

    //! Sets state of clipping plane.
    void SetState (const ClipPlaneState theState)
    {
      mySettings.x() = static_cast<Standard_ShortReal> (theState);
    }

  private:

    //! Settings of clipping plane.
    BVH_Vec4f& mySettings;

    //! 4D equation vector of clipping plane.
    BVH_Vec4f& myEquation;
  };

public:

  //! Creates new set of clipping planes.
  OpenGl_RaytraceClipPlanes();

  //! Returns clipping plane for the given index.
  ClipPlane operator[] (const Standard_Integer theIndex)
  {
    return ClipPlane (myClipPlanes[theIndex * 2 + 0],
                      myClipPlanes[theIndex * 2 + 1]);
  }

  //! Returns packed (serialized) representation of clipping planes set.
  const Standard_ShortReal* Packed()
  {
    return reinterpret_cast<Standard_ShortReal*> (this);
  }

private:

  //! Serialized clipping planes storage.
  BVH_Vec4f myClipPlanes[MAX_PLANE_NUMBER * 2];

};

//! Stores transform properties of ray-tracing object.
class OpenGl_RaytraceTransform : public BVH_Transform<Standard_ShortReal, 4>
{
public:

  //! Value of invalid clipping plane set.
  static const Standard_Integer NO_CLIPPING = -1;

public:

  //! Creates new identity transformation.
  OpenGl_RaytraceTransform() : BVH_Transform<Standard_ShortReal, 4>()
  {
    myClipSetID = NO_CLIPPING; // no clipping by default
  }

  //! Creates new transformation with specified matrix.
  OpenGl_RaytraceTransform (const BVH_Mat4f& theTransform) : BVH_Transform<Standard_ShortReal, 4> (theTransform)
  {
    myClipSetID = NO_CLIPPING; // no clipping by default
  }

  //! Returns ID of associated set of clipping planes.
  Standard_Integer ClipSetID() const
  {
    return myClipSetID;
  }

  //! Sets ID of associated set of clipping planes.
  void SetClipSetID (const Standard_Integer theClipSetID)
  {
    myClipSetID = theClipSetID;
  }

protected:

  //! ID of associated set of clipping planes.
  Standard_Integer myClipSetID;

};

//! Stores geometry of ray-tracing scene.
class OpenGl_RaytraceGeometry : public BVH_Geometry<Standard_ShortReal, 3>
{
public:

  //! Value of invalid offset to return in case of errors.
  static const Standard_Integer INVALID_OFFSET = -1;

  //! Maximum number of textures used in ray-tracing shaders.
  //! This is not restriction of the solution implemented, but
  //! rather the reasonable limit of the number of textures in
  //! various applications (can be increased if needed).
  static const Standard_Integer MAX_TEX_NUMBER = 32;

public:

  //! Array of properties of light sources.
  std::vector<OpenGl_RaytraceLight,
    NCollection_StdAllocator<OpenGl_RaytraceLight> > Sources;

  //! Array of 'front' material properties.
  std::vector<OpenGl_RaytraceMaterial,
    NCollection_StdAllocator<OpenGl_RaytraceMaterial> > Materials;

  //! Array of sets of clipping plane parameters.
  std::vector<OpenGl_RaytraceClipPlanes,
    NCollection_StdAllocator<OpenGl_RaytraceClipPlanes> > ClipPlanes;

  //! Global ambient from all light sources.
  BVH_Vec4f Ambient;

public:

  //! Creates uninitialized ray-tracing geometry.
  OpenGl_RaytraceGeometry()
  : BVH_Geometry<Standard_ShortReal, 3>(),
    myHighLevelTreeDepth (0),
    myBottomLevelTreeDepth (0)
  {
    //
  }

  //! Releases resources of ray-tracing geometry.
  ~OpenGl_RaytraceGeometry()
  {
    //
  }

  //! Clears only ray-tracing materials.
  void ClearMaterials()
  {
    std::vector<OpenGl_RaytraceMaterial,
      NCollection_StdAllocator<OpenGl_RaytraceMaterial> > anEmptyMaterials;

    Materials.swap (anEmptyMaterials);

    myTextures.Clear();
  }

  //! Clears ray-tracing geometry.
  void Clear();

public: //! @name methods related to acceleration structure

  //! Performs post-processing of high-level scene BVH.
  Standard_Boolean ProcessAcceleration();

  //! Returns offset of bottom-level BVH for given leaf node.
  //! If the node index is not valid the function returns -1.
  //! @note Can be used after processing acceleration structure.
  Standard_Integer AccelerationOffset (Standard_Integer theNodeIdx);

  //! Returns offset of triangulation vertices for given leaf node.
  //! If the node index is not valid the function returns -1.
  //! @note Can be used after processing acceleration structure.
  Standard_Integer VerticesOffset (Standard_Integer theNodeIdx);

  //! Returns offset of triangulation elements for given leaf node.
  //! If the node index is not valid the function returns -1.
  //! @note Can be used after processing acceleration structure.
  Standard_Integer ElementsOffset (Standard_Integer theNodeIdx);

  //! Returns triangulation data for given leaf node.
  //! If the node index is not valid the function returns NULL.
  //! @note Can be used after processing acceleration structure.
  OpenGl_TriangleSet* TriangleSet (Standard_Integer theNodeIdx);

public: //! @name methods related to texture management

  //! Adds new OpenGL texture to the scene and returns its index.
  Standard_Integer AddTexture (const Handle(OpenGl_Texture)& theTexture);

  //! Updates unique 64-bit texture handles to use in shaders.
  Standard_Boolean UpdateTextureHandles (const Handle(OpenGl_Context)& theContext);

  //! Makes the OpenGL texture handles resident (must be called before using).
  Standard_Boolean AcquireTextures (const Handle(OpenGl_Context)& theContext) const;

  //! Makes the OpenGL texture handles non-resident (must be called after using).
  Standard_Boolean ReleaseTextures (const Handle(OpenGl_Context)& theContext) const;

  //! Returns array of texture handles.
  const std::vector<GLuint64>& TextureHandles() const
  {
    return myTextureHandles;
  }

  //! Checks if scene contains textured objects.
  Standard_Boolean HasTextures() const
  {
    return !myTextures.IsEmpty();
  }

  //! Releases OpenGL resources.
  void ReleaseResources (const Handle(OpenGl_Context)& theContext)
  {
    if (!myTextureSampler.IsNull())
    {
      myTextureSampler->Release (theContext.operator->());
      myTextureSampler.Nullify();
    }
  }

public: //! @name auxiliary methods

  //! Returns depth of high-level scene BVH from last build.
  Standard_Integer HighLevelTreeDepth() const
  {
    return myHighLevelTreeDepth;
  }

  //! Returns maximum depth of bottom-level scene BVHs from last build.
  Standard_Integer BottomLevelTreeDepth() const
  {
    return myBottomLevelTreeDepth;
  }

protected:

  NCollection_Vector<Handle(OpenGl_Texture)> myTextures;             //!< Array of texture maps shared between rendered objects
  Handle(OpenGl_Sampler)                     myTextureSampler;       //!< Sampler object providing fixed sampling params for texures
  std::vector<GLuint64>                      myTextureHandles;       //!< Array of unique 64-bit texture handles obtained from OpenGL
  Standard_Integer                           myHighLevelTreeDepth;   //!< Depth of high-level scene BVH from last build
  Standard_Integer                           myBottomLevelTreeDepth; //!< Maximum depth of bottom-level scene BVHs from last build

};

#endif

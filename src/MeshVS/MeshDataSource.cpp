// Copyright (c) 2014-2015 OPEN CASCADE SAS
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

#include <MeshDataSource.h>

#include <Standard_CLocaleSentry.hxx>
#include <MeshVS_Drawer.hxx>
#include <MeshVS_DrawerAttribute.hxx>
#include <Message.hxx>
#include <Message_Messenger.hxx> 
#include <Message_ProgressSentry.hxx> 
#include <NCollection_Handle.hxx>
#include <TColgp_SequenceOfXYZ.hxx>
#include <TColStd_DataMapOfIntegerReal.hxx>
#include <TColStd_DataMapOfIntegerInteger.hxx>
#include <TColStd_Array1OfInteger.hxx>
#include <TColStd_Array1OfReal.hxx>
#include <TColStd_MapIteratorOfPackedMapOfInteger.hxx>
#include <TCollection_ExtendedString.hxx>
#include <Quantity_Color.hxx>

IMPLEMENT_STANDARD_RTTIEXT(MeshScalarProperty, Standard_Transient)

IMPLEMENT_STANDARD_RTTIEXT(MeshDataSource, MeshVS_DataSource)

//================================================================
// Function : Constructor
// Purpose  :
//================================================================
MeshDataSource::MeshDataSource()
{
  //
}

//================================================================
// Function : Clear
// Purpose  :
//================================================================
void MeshDataSource::Clear()
{
  myUsedNodes.Clear();
  myFreeNodes.Clear();
  myUsedElements.Clear();
  myComments.Clear();
  myNodes.Clear();
  myNormals.Clear();
  myNodesUV.Clear();
  myNodalColors.Clear();
  myElements.Clear();
  myNodalQuantities.Clear();
  myElementalQuantities.Clear();
  myGroups.Clear();
}

//================================================================
// Function : GetGeom
// Purpose  :
//================================================================
Standard_Boolean MeshDataSource::GetGeom (const Standard_Integer theID,
                                          const Standard_Boolean theIsElement,
                                          TColStd_Array1OfReal&  theCoords,
                                          Standard_Integer&      theNbNodes,
                                          MeshVS_EntityType&     theType) const
{
  const Standard_Integer aLow = theCoords.Lower();
  if (theIsElement)
  {
    if (theID < 0 || theID >= myUsedElements.Extent())
    {
      return Standard_False;
    }

    const Graphic3d_Vec4i& anElem = myElements.Value (theID);
    theType    = MeshVS_ET_Face;
    theNbNodes = anElem[3] == -1 ? 3 : 4;
    for (Standard_Integer aNodeIter = 0, aCoordIter = aLow - 1; aNodeIter < theNbNodes; ++aNodeIter)
    {
      const Graphic3d_Vec3& aNode = myNodes.Value (anElem[aNodeIter]);
      theCoords (++aCoordIter) = aNode.x();
      theCoords (++aCoordIter) = aNode.y();
      theCoords (++aCoordIter) = aNode.z();
    }
    return Standard_True;
  }
  else if (theID < 0 || theID >= myUsedNodes.Extent())
  {
    return Standard_False;
  }

  theType = MeshVS_ET_Node;
  theNbNodes = 1;
  const Graphic3d_Vec3& aNode = myNodes.Value (theID);
  theCoords (aLow)     = aNode.x();
  theCoords (aLow + 1) = aNode.y();
  theCoords (aLow + 2) = aNode.z();
  return Standard_True;
}

//================================================================
// Function : GetNodesByElement
// Purpose  :
//================================================================
Standard_Boolean MeshDataSource::GetNodesByElement (const Standard_Integer   theID,
                                                    TColStd_Array1OfInteger& theNodeIDs,
                                                    Standard_Integer&        theNbNodes) const
{
  if (theID < 0 || theID >= myUsedElements.Extent())
  {
    return Standard_False;
  }

  const Graphic3d_Vec4i& anElem = myElements.Value (theID);
  theNbNodes = anElem[3] == -1 ? 3 : 4;
  if (theNodeIDs.Length() < theNbNodes)
  {
    return Standard_False;
  }

  const Standard_Integer aLow = theNodeIDs.Lower();
  theNodeIDs (aLow)     = anElem[0];
  theNodeIDs (aLow + 1) = anElem[1];
  theNodeIDs (aLow + 2) = anElem[2];
  if (theNbNodes == 4)
  {
    theNodeIDs (aLow + 3) = anElem[3];
  }
  return Standard_True;
}

//================================================================
// Function : GetNodeNormal
// Purpose  :
//================================================================
Standard_Boolean MeshDataSource::GetNodeNormal (const Standard_Integer theRankNode,
                                                const Standard_Integer theElementId,
                                                Standard_Real&         theNX,
                                                Standard_Real&         theNY,
                                                Standard_Real&         theNZ) const
{
  if (theElementId < 0 || theElementId >= myUsedElements.Extent()
  || !HasNormals())
  {
    return Standard_False;
  }

  const Graphic3d_Vec4i& anElem = myElements.Value (theElementId);
  const Graphic3d_Vec3&  aNorm  = myNormals.Value (anElem[theRankNode - 1]);
  theNX = Standard_Real(aNorm.x());
  theNY = Standard_Real(aNorm.y());
  theNZ = Standard_Real(aNorm.z());
  return Standard_True;
}

inline void truncateToRange (Standard_Real&      theValue,
                             const Standard_Real theLower,
                             const Standard_Real theUpper)
{
  if (theValue <= theLower)
  {
    theValue = theLower;
  }
  else if (theValue >= theUpper)
  {
    theValue = theUpper;
  }
}

//================================================================
// Function : FillPropertyBuilder
// Purpose  :
//================================================================
void MeshDataSource::FillPropertyBuilder (const Handle(MeshVS_PrsBuilder)&  theBuilder,
                                          const Handle(MeshScalarProperty)& theProperty) const
{
  Handle(MeshVS_NodalColorPrsBuilder)     aNodalBuilder = Handle(MeshVS_NodalColorPrsBuilder)    ::DownCast (theBuilder);
  Handle(MeshVS_ElementalColorPrsBuilder) anElemBuilder = Handle(MeshVS_ElementalColorPrsBuilder)::DownCast (theBuilder);
  if ((aNodalBuilder.IsNull() && anElemBuilder.IsNull())
   || theProperty.IsNull())
  {
    return;
  }

  //theBuilder->UseTexture (Standard_True);
  const Standard_Boolean isGrayscale = (theProperty->Name() == "intensity");

  const Standard_Real aLowerValue = isGrayscale ? 0.0 : theProperty->LowerValue();
  const Standard_Real aRange      = isGrayscale ? 1.0 : theProperty->UpperValue() - theProperty->LowerValue();

  const Graphic3d_Vec3d aScale00 = isGrayscale ? Graphic3d_Vec3d (0.0, 0.0, 0.0) : Graphic3d_Vec3d (0.0, 0.0, 1.0);
  const Graphic3d_Vec3d aScale05 = isGrayscale ? Graphic3d_Vec3d (0.5, 0.5, 0.5) : Graphic3d_Vec3d (0.0, 1.0, 0.0);
  const Graphic3d_Vec3d aScale10 = isGrayscale ? Graphic3d_Vec3d (1.0, 1.0, 1.0) : Graphic3d_Vec3d (1.0, 0.0, 0.0);
  Graphic3d_Vec3d aColor;
  for (Standard_Integer anElemIter = theProperty->LowerIndex(); anElemIter <= theProperty->UpperIndex(); ++anElemIter)
  {
    if (aRange <= Precision::Confusion())
    {
      aColor = aScale00;
    }
    else
    {
      Standard_Real aValue = (theProperty->Value (anElemIter) - aLowerValue) / aRange;
      if (aValue < 0.5)
      {
        aValue *= 2.0;
        aColor = aScale00 * (1.0 - aValue) + aScale05 * aValue;
      }
      else
      {
        aValue = (aValue - 0.5) * 2.0;
        aColor = aScale05 * (1.0 - aValue) + aScale10 * aValue;
      }
      truncateToRange (aColor.r(), 0.0, 1.0);
      truncateToRange (aColor.g(), 0.0, 1.0);
      truncateToRange (aColor.b(), 0.0, 1.0);
    }

    const Quantity_Color aQColor (aColor.r(), aColor.g(), aColor.b(), Quantity_TOC_RGB);
    if (!aNodalBuilder.IsNull())
    {
      aNodalBuilder->SetColor (anElemIter, aQColor);
    }
    else
    {
      anElemBuilder->SetColor1 (anElemIter, aQColor);
    }
  }
}

//================================================================
// Function : FillNodalColorsBuilder
// Purpose  :
//================================================================
void MeshDataSource::FillNodalColorsBuilder (const Handle(MeshVS_NodalColorPrsBuilder)& theBuilder) const
{
  if (theBuilder.IsNull()
  || !HasNodalColors())
  {
    return;
  }

  for (Standard_Integer aNodeIter = myNodalColors.Lower(); aNodeIter <= myNodalColors.Upper(); ++aNodeIter)
  {
    const Graphic3d_Vec4ub& aColor = myNodalColors.Value (aNodeIter);
    const Quantity_Color    aQColor (aColor.r() / 255.0, aColor.g() / 255.0, aColor.b() / 255.0, Quantity_TOC_RGB);
    theBuilder->SetColor (aNodeIter, aQColor);
  }
}

//================================================================
// Function : NbElements
// Purpose  :
//================================================================
void MeshDataSource::NbElements (Standard_Integer& theNbTris,
                                 Standard_Integer& theNbQuads) const
{
  for (NCollection_Vector<Graphic3d_Vec4i>::Iterator anElemIter (myElements); anElemIter.More(); anElemIter.Next())
  {
    const Graphic3d_Vec4i& anElem = anElemIter.Value();
    if (anElem[3] == -1)
    {
      ++theNbTris;
    }
    else
    {
      ++theNbQuads;
    }
  }
}

// Created on: 2014-12-24
// Created by: Ilya Sevrikov
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

#ifndef _Graphic3d_RgbaTransferFunction_HeaderFile
#define _Graphic3d_RgbaTransferFunction_HeaderFile

#include <NCollection_Handle.hxx>
#include <NCollection_Sequence.hxx>
#include <Graphic3d_TransferFunction.hxx>
#include <Graphic3d_TransferFunctionNode.hxx>
#include <Graphic3d_TransferFunctionLinearFilter.hxx>
#include <Graphic3d_TransferFunctionNearestFilter.hxx>

//! Defines RGBA transfer function for specific type of scalar field.
template<class PointType, class ValueType = Standard_ShortReal>
class Graphic3d_RgbaTransferFunction : public Graphic3d_TransferFunction
{
public:

  //! Defines RGB color type.
  typedef NCollection_Vec3<ValueType> ColorType;

  //! Defines type of color node.
  typedef Graphic3d_TransferFunctionNode<PointType, ColorType> ColorNode;

  //! Defines type of opacity node.
  typedef Graphic3d_TransferFunctionNode<PointType, ValueType> AlphaNode;

  //! Defines type of color filter tool.
  typedef Graphic3d_TransferFunctionFilter<PointType, ColorType> ColorFilterTool;

  //! Defines type of opacity filter tool.
  typedef Graphic3d_TransferFunctionFilter<PointType, ValueType> AlphaFilterTool;

public:

  //! Creates empty transfer function.
  Graphic3d_RgbaTransferFunction();

  //! Releases resources of transfer function.
  ~Graphic3d_RgbaTransferFunction();

  //! Saves TF to file.
  Standard_Boolean SaveToFile (const Standard_Character* theFileName)
  {
    std::ofstream aStream;

    OSD_OpenStream (aStream, theFileName, std::ios_base::binary);

    if (!aStream.is_open())
    {
      return Standard_False;
    }

    const Standard_Integer aNbColorNodes = myColorNodes.Size();
    const Standard_Integer aNbAlphaNodes = myAlphaNodes.Size();

    aStream.write (reinterpret_cast<const Standard_Character*> (&aNbColorNodes), sizeof (Standard_Integer));
    aStream.write (reinterpret_cast<const Standard_Character*> (&aNbAlphaNodes), sizeof (Standard_Integer));

    for (Standard_Integer anIdx = 0; anIdx < aNbColorNodes; ++anIdx)
    {
      PointType aPoint = myColorNodes.Value (anIdx + 1).Point;

      ValueType aR = myColorNodes.Value (anIdx + 1).Value.x();
      ValueType aG = myColorNodes.Value (anIdx + 1).Value.y();
      ValueType aB = myColorNodes.Value (anIdx + 1).Value.z();

      aStream.write (reinterpret_cast<const Standard_Character*> (&aPoint), sizeof (PointType));

      aStream.write (reinterpret_cast<const Standard_Character*> (&aR), sizeof (ValueType));
      aStream.write (reinterpret_cast<const Standard_Character*> (&aG), sizeof (ValueType));
      aStream.write (reinterpret_cast<const Standard_Character*> (&aB), sizeof (ValueType));
    }

    for (Standard_Integer anIdx = 0; anIdx < aNbAlphaNodes; ++anIdx)
    {
      PointType aPoint = myAlphaNodes.Value (anIdx + 1).Point;
      ValueType aValue = myAlphaNodes.Value (anIdx + 1).Value;

      aStream.write (reinterpret_cast<const Standard_Character*> (&aPoint), sizeof (PointType));
      aStream.write (reinterpret_cast<const Standard_Character*> (&aValue), sizeof (ValueType));
    }

    aStream.close();
  }

  //! Loads TF from file.
  Standard_Boolean LoadFromFile (const Standard_Character* theFileName)
  {
    std::ifstream aStream;

    OSD_OpenStream (aStream, theFileName, std::ios_base::binary);

    if (!aStream.is_open())
    {
      return Standard_False;
    }

    Standard_Integer aNbColorNodes = 0;
    Standard_Integer aNbAlphaNodes = 0;

    aStream.read (reinterpret_cast<Standard_Character*> (&aNbColorNodes), sizeof (Standard_Integer));
    aStream.read (reinterpret_cast<Standard_Character*> (&aNbAlphaNodes), sizeof (Standard_Integer));

    if (aNbColorNodes <= 0 || aNbAlphaNodes <= 0)
    {
      return Standard_False;
    }

    myColorNodes.Clear();
    myAlphaNodes.Clear();

    for (Standard_Integer anIdx = 0; anIdx < aNbColorNodes; ++anIdx)
    {
      PointType aPoint;

      ValueType aR;
      ValueType aG;
      ValueType aB;

      aStream.read (reinterpret_cast<Standard_Character*> (&aPoint), sizeof (PointType));

      aStream.read (reinterpret_cast<Standard_Character*> (&aR), sizeof (ValueType));
      aStream.read (reinterpret_cast<Standard_Character*> (&aG), sizeof (ValueType));
      aStream.read (reinterpret_cast<Standard_Character*> (&aB), sizeof (ValueType));

      AddNode (aPoint, ColorType (aR, aG, aB));
    }

    for (Standard_Integer anIdx = 0; anIdx < aNbAlphaNodes; ++anIdx)
    {
      PointType aPoint;
      ValueType aValue;

      aStream.read (reinterpret_cast<Standard_Character*> (&aPoint), sizeof (PointType));
      aStream.read (reinterpret_cast<Standard_Character*> (&aValue), sizeof (ValueType));

      AddNode (aPoint, aValue);
    }

    aStream.close();

    Update();
  }

public:

  //! Appends color control point.
  void AddNode (const PointType  thePoint,
                const ColorType& theColor)
  {
    myColorNodes.Append (ColorNode (thePoint, theColor));
  }

  //! Appends opacity control point.
  void AddNode (const PointType thePoint,
                const ValueType theAlpha)
  {
    myAlphaNodes.Append (AlphaNode (thePoint, theAlpha));
  }

  //! Appends color-opacity control point.
  void AddNode (const PointType  thePoint,
                const ColorType& theColor,
                const ValueType  theAlpha)
  {
    AddNode (thePoint, theColor);
    AddNode (thePoint, theAlpha);
  }

  //! Returns reference to the given color control point.
  ColorNode& ChangeColorNodeData (const Standard_Integer theIndex)
  {
    return myColorNodes.ChangeValue (theIndex - 1);
  }

  //! Returns reference to the given opacity control point.
  AlphaNode& ChangeAlphaNodeData (const Standard_Integer theIndex)
  {
    return myAlphaNodes.ChangeValue (theIndex - 1);
  }

  //! Returns const reference to the given color control point.
  const ColorNode& ColorNodeData (const Standard_Integer theIndex) const
  {
    return myColorNodes.Value (theIndex - 1);
  }

  //! Returns const reference to the given opacity control point.
  const AlphaNode& AlphaNodeData (const Standard_Integer theIndex) const
  {
    return myAlphaNodes.Value (theIndex - 1);
  }

  //! Removes the given color control point.
  void RemoveColorNode (const Standard_Integer theIndex)
  {
    myColorNodes.Remove (theIndex - 1);
  }

  //! Removes the given opacity control point.
  void RemoveAlphaNode (const Standard_Integer theIndex)
  {
    myAlphaNodes.Remove (theIndex - 1);
  }

  //! Clears all color control points.
  void ClearColorNodes()
  {
    myColorNodes.Clear();
  }

  //! Clears all opacity control points.
  void ClearAlphaNodes()
  {
    myAlphaNodes.Clear();
  }

  //! Returns number of color nodes.
  Standard_Integer NbOfColorNodes() const
  {
    return myColorNodes.Size();
  }

  //! Returns number of opacity nodes.
  Standard_Integer NbOfAlphaNodes() const
  {
    return myAlphaNodes.Size();
  }

  //! Returns color filter.
  const NCollection_Handle<ColorFilterTool>& ColorFilter() const
  {
    return myColorFilter;
  }

  //! Sets color filter.
  void SetColorFilter (const NCollection_Handle<ColorFilterTool>& theColorFilter)
  {
    myColorFilter = theColorFilter;
  }

  //! Returns opacity filter.
  const NCollection_Handle<AlphaFilterTool>& AlphaFilter() const
  {
    return myAlphaFilter;
  }

  //! Sets opacity filter.
  void SetAlphaFilter (const NCollection_Handle<AlphaFilterTool>& theAlphaFilter)
  {
    myAlphaFilter = theAlphaFilter;
  }

public:

  //! Initializes transfer function with default values.
  void InitDefault (const Standard_ShortReal theScale, const Standard_Integer theNbOfNodes = 32);

  //! Computes discretization table as 1D image.
  virtual Handle(Image_PixMap) ImageTable (Standard_Real&         theNormMinValue,
                                           Standard_Real&         theNormMaxValue,
                                           const Standard_Integer theNumberOfRows = SIZE_OF_TABLE) const Standard_OVERRIDE;

protected:

  //! List of color nodes.
  NCollection_Sequence<ColorNode> myColorNodes;

  //! List of opacity nodes.
  NCollection_Sequence<AlphaNode> myAlphaNodes;

  //! Color filter tool.
  mutable NCollection_Handle<ColorFilterTool> myColorFilter;

  //! Opacity filter tool.
  mutable NCollection_Handle<AlphaFilterTool> myAlphaFilter;

};

#include <Graphic3d_RgbaTransferFunction.lxx>

#endif // _Graphic3d_RgbaTransferFunction_HeaderFile

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

#ifndef MeshPrsBuilder_H
#define MeshPrsBuilder_H

#include "MeshDataSource.h"

#include <MeshVS_PrsBuilder.hxx>
#include <Graphic3d_Texture2D.hxx>
#include <Graphic3d_TextureParams.hxx>

//! Auxiliary class to hold texture.
class MeshPrsTexture : public Graphic3d_Texture2D
{
public:

  MeshPrsTexture (const Handle(Image_PixMap)& theImg)
  : Graphic3d_Texture2D (theImg, Graphic3d_TOT_2D)
  {
    myParams->SetModulate (Standard_True);
    myParams->SetFilter   (Graphic3d_TOTF_BILINEAR);
  }

public:

  DEFINE_STANDARD_RTTIEXT(MeshPrsTexture, Graphic3d_Texture2D)

};

DEFINE_STANDARD_HANDLE(MeshPrsTexture, Graphic3d_Texture2D)

//! Auxiliary builder for Nodal or Elemental mesh properties using textured colorscale.
class MeshPrsBuilder : public MeshVS_PrsBuilder
{

public:

  //! Default constructor.
  Standard_EXPORT MeshPrsBuilder (const Handle(MeshVS_Mesh)&     theMesh,
                                  const MeshVS_DisplayModeFlags& theFlags);

  //! Setup textures when available.
  Standard_Boolean ToMapTextures() const { return myToMapTexture; }

  //! Setup textures when available.
  void SetMapTextures (const Standard_Boolean theToMap) { myToMapTexture = theToMap; }

public:

  //! Setup property to display.
  Standard_EXPORT void SetProperty (const Handle(MeshScalarProperty)& theProperty,
                                    const Standard_Boolean            theIsElemental);

  //! Grayscale flag.
  Standard_Boolean IsGrayscale() const
  {
    return myIsGrayscale;
  }

  //! Specify using grayscale or B->G->R colorscale
  void SetGrayscale (const Standard_Boolean theIsGrayscale)
  {
    myIsGrayscale = theIsGrayscale;
  }

  //! Adjust colorscale range.
  Standard_EXPORT void SetRange (const Standard_Real theFrom,
                                 const Standard_Real theTo,
                                 const bool          theIsInversed);

public:

  //! Main builder.
  Standard_EXPORT virtual void Build (const Handle(Prs3d_Presentation)& thePrs,
                                      const TColStd_PackedMapOfInteger& theIDs,
                                      TColStd_PackedMapOfInteger&       theIDsToExclude,
                                      const Standard_Boolean            theIsElement,
                                      const Standard_Integer            theDisplayMode) const Standard_OVERRIDE;

protected:

  //! Textured presentation builder.
  void buildTextured (const Handle(Prs3d_Presentation)& thePrs,
                      const TColStd_PackedMapOfInteger& theIDs,
                      TColStd_PackedMapOfInteger&       theIDsToExclude) const;

  //! Auxiliary method to create texture.
  static Handle(Graphic3d_Texture2D) createTexture (const Standard_Boolean theIsGrayscale,
                                                    const Standard_Real    theFrom,
                                                    const Standard_Real    theTo,
                                                    const bool             theIsInversed);

  //! Return lower property value.
  Standard_Real propertyLowerValue() const
  {
    if (myProperty.IsNull())
    {
      return 0.0;
    }
    return myIsGrayscale ? Min (0.0, myProperty->LowerValue()) : myProperty->LowerValue();
  }

  //! Return upper property value.
  Standard_Real propertyUpperValue() const
  {
    if (myProperty.IsNull())
    {
      return 1.0;
    }
    return myIsGrayscale ? Max (1.0, myProperty->UpperValue()) : myProperty->UpperValue();
  }

protected:

  Handle(MeshScalarProperty)                 myProperty;    //!< property to display
  Standard_Boolean                           myIsElemental; //!< flag indicating elemental property
  Standard_Boolean                           myIsGrayscale; //!< flag to create grayscale colorscale
  Standard_Real                              myRangeFrom;   //!< colorscale range, lower value
  Standard_Real                              myRangeTo;     //!< colorscale range, upper value

  Standard_Boolean                           myToMapTexture;//!< setup textures when available
  Standard_Boolean                           myToFlipV;     //!< flip texture coordinates vertically

  mutable Handle(Graphic3d_Group)            myPrsGroup;    //!< presentation group with triangles
  mutable Handle(Graphic3d_AspectFillArea3d) myFillAsp;

public:

  DEFINE_STANDARD_RTTIEXT(MeshPrsBuilder, MeshVS_PrsBuilder)

};

DEFINE_STANDARD_HANDLE(MeshPrsBuilder, MeshVS_PrsBuilder)

#endif // MeshPrsBuilder_H

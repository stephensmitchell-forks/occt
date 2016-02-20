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

#ifndef MeshPresentation_H
#define MeshPresentation_H

#include <MeshVS_Mesh.hxx>

//! Interactive presentation for the mesh.
class MeshPresentation : public MeshVS_Mesh
{
public:

  //! Empty constructor.
  Standard_EXPORT MeshPresentation();

  //! Return presentation type.
  virtual AIS_KindOfInteractive Type() const Standard_OVERRIDE { return AIS_KOI_Object; }

  //! Return presentation signature.
  virtual Standard_Integer Signature() const Standard_OVERRIDE { return 0; }

  //! Setup new material.
  virtual void SetMaterial (const Graphic3d_NameOfMaterial theName) Standard_OVERRIDE
  {
    SetMaterial (Graphic3d_MaterialAspect (theName));
  }

  //! Setup new material.
  virtual void SetMaterial (const Graphic3d_MaterialAspect& theMat) Standard_OVERRIDE;

  //! Override presentation compute.
  virtual void Compute (const Handle(PrsMgr_PresentationManager3d)& thePrsMgr,
                        const Handle(Prs3d_Presentation)&           thePrs,
                        const Standard_Integer                      theMode) Standard_OVERRIDE;

  //! Setup closed flag.
  void SetClosed (const bool theIsClosed);

public:

  DEFINE_STANDARD_RTTIEXT(MeshPresentation, MeshVS_Mesh)

};

#endif // MeshPresentation_H

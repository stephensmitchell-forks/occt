// Created on: 2017-08-22
// Created by: Sergey SLYADNEV
// Copyright (c) 2017 OPEN CASCADE SAS
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

#ifndef _XDEDRAW_DrawableAssemblyGraph_HeaderFile
#define _XDEDRAW_DrawableAssemblyGraph_HeaderFile

// XDE includes
#include <XCAFDoc_AssemblyGraph.hxx>

// OCCT includes
#include <Draw_Drawable3D.hxx>

//! Drawable container for assembly graph.
class XDEDRAW_DrawableAssemblyGraph : public Draw_Drawable3D
{
public:

  // OCCT RTTI
  DEFINE_STANDARD_RTTI_INLINE(XDEDRAW_DrawableAssemblyGraph, Draw_Drawable3D)

public:

  //! Ctor accepting an assembly graph.
  //! \param[in] asmGraph assembly graph to have as drawable.
  Standard_EXPORT
    XDEDRAW_DrawableAssemblyGraph(const Handle(XCAFDoc_AssemblyGraph)& asmGraph);

public:

  //! \return stored assembly graph.
  Standard_EXPORT const Handle(XCAFDoc_AssemblyGraph)&
    GetGraph() const;

public:

  Standard_EXPORT void
    DrawOn(Draw_Display& dis) const Standard_OVERRIDE;

  Standard_EXPORT virtual void
    Dump(Standard_OStream& out) const Standard_OVERRIDE;

  Standard_EXPORT virtual void
    Whatis(Draw_Interpretor& di) const Standard_OVERRIDE;

private:

  Handle(XCAFDoc_AssemblyGraph) m_graph; //!< Assembly graph to "draw".

};

#endif

// Created on: 1997-05-14
// Created by: Christian CAILLET
// Copyright (c) 1997-1999 Matra Datavision
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

#ifndef _XSControl_Reader_HeaderFile
#define _XSControl_Reader_HeaderFile

#include <Standard.hxx>
#include <Standard_DefineAlloc.hxx>
#include <Standard_Handle.hxx>

#include <TColStd_SequenceOfTransient.hxx>
#include <TColStd_HSequenceOfTransient.hxx>
#include <TopTools_SequenceOfShape.hxx>
#include <IFSelect_ReturnStatus.hxx>
class XSControl_WorkSession;
class Interface_InterfaceModel;

//! A groundwork to convert a shape to data which complies
//! with a particular norm. This data can be that of a whole
//! model or that of a specific list of entities in the model.
//! You specify the list using a single selection or a
//! combination of selections. A selection is an operator which
//! computes a list of entities from a list given in input. To
//! specify the input, you can use:
//! - A predefined selection such as "xst-transferrable-roots"
//! - A filter based on a  signature.
//! A signature is an operator which returns a string from an
//! entity according to its type.
//! For example:
//! - "xst-type" (CDL)
//! - "iges-level"
//! - "step-type".
//! A filter can be based on a signature by giving a value to
//! be matched by the string returned. For example,
//! "xst-type(Curve)".
//! If no list is specified, the selection computes its list of
//! entities from the whole model. To use this class, you have to
//! initialize the transfer norm first, as shown in the example below.
//! Example:
//! Control_Reader reader;
//! IFSelect_ReturnStatus status = reader.ReadFile (filename.);
//! When using IGESControl_Reader or STEPControl_Reader - as the
//! above example shows - the reader initializes the norm directly.
//! Note that loading the file only stores the data. It does
//! not translate this data. Shapes are accumulated by
//! successive transfers. The last shape is cleared by:
//! - ClearShapes which allows you to handle a new batch
//! - TransferRoots which restarts the list of shapes from scratch.
class XSControl_Reader 
{
public:

  DEFINE_STANDARD_ALLOC
  
  //! Creates a Reader from scratch (creates an empty WorkSession)
  //! A WorkSession or a Controller must be provided before running
  Standard_EXPORT XSControl_Reader();
  
  //! Creates a Reader from scratch, with a norm name which
  //! identifies a Controller
  XSControl_Reader(const Standard_CString norm) { SetNorm (norm); }
  
  //! Creates a Reader from an already existing Session, with a
  //! Controller already set
  //! Virtual destructor
  XSControl_Reader(const Handle(XSControl_WorkSession)& WS, const Standard_Boolean scratch = Standard_True) { SetWS (WS,scratch); }

  Standard_EXPORT virtual ~XSControl_Reader() {}
  
  //! Sets a specific norm to <me>
  //! Returns True if done, False if <norm> is not available
  Standard_EXPORT Standard_Boolean SetNorm (const Standard_CString norm);
  
  //! Sets a specific session to <me>
  Standard_EXPORT void SetWS (const Handle(XSControl_WorkSession)& WS, const Standard_Boolean scratch = Standard_True);
  
  //! Returns the session used in <me>
  const Handle(XSControl_WorkSession) & WS() const { return thesession; }
  
  //! Loads a file and returns the read status
  //! Zero for a Model which compies with the Controller
  Standard_EXPORT IFSelect_ReturnStatus ReadFile (const Standard_CString filename);
  
  //! Returns the model. It can then be consulted (header, product)
  Standard_EXPORT Handle(Interface_InterfaceModel) Model() const;
  
  //! Returns a list of entities from the IGES or STEP file
  //! according to the following rules:
  //! - if first and second are empty strings, the whole file is selected.
  //! - if first is an entity number or label, the entity referred to is selected.
  //! - if first is a list of entity numbers/labels separated by commas, the entities referred to are selected,
  //! - if first is the name of a selection in the worksession and second is not defined,
  //! the list contains the standard output for that selection.
  //! - if first is the name of a selection and second is defined, the criterion defined
  //! by second is applied to the result of the first selection.
  //! A selection is an operator which computes a list of entities from a list given in
  //! input according to its type. If no list is specified, the selection computes its
  //! list of entities from the whole model.
  //! A selection can be:
  //! - A predefined selection (xst-transferrable-mode)
  //! - A filter based on a signature
  //! A Signature is an operator which returns a string from an entity according to its type. For example:
  //! - "xst-type" (CDL)
  //! - "iges-level"
  //! - "step-type".
  //! For example, if you wanted to select only the advanced_faces in a STEP file you
  //! would use the following code:
  //! Example
  //! Reader.GiveList("xst-transferrable-roots","step-type(ADVANCED_FACE)");
  //! Warning
  //! If the value given to second is incorrect, it will simply be ignored.
  Standard_EXPORT Handle(TColStd_HSequenceOfTransient) GiveList (const Standard_CString first = "", const Standard_CString second = "");
  
  //! Computes a List of entities from the model as follows
  //! <first> beeing a Selection, <ent> beeing an entity or a list
  //! of entities (as a HSequenceOfTransient) :
  //! the standard result of this selection applied to this list
  //! if <first> is erroneous, a null handle is returned
  Standard_EXPORT Handle(TColStd_HSequenceOfTransient) GiveList (const Standard_CString first, const Handle(Standard_Transient)& ent);
  
  //! Determines the list of root entities which are candidate for
  //! a transfer to a Shape, and returns the number
  //! of entities in the list
  Standard_EXPORT virtual Standard_Integer NbRootsForTransfer();
  
  //! Returns an IGES or STEP root
  //! entity for translation. The entity is identified by its
  //! rank in a list.
  Standard_EXPORT Handle(Standard_Transient) RootForTransfer (const Standard_Integer num = 1);
  
  //! Translates a root identified by the rank num in the model.
  //! false is returned if no shape is produced.
  Standard_EXPORT Standard_Boolean TransferOneRoot (const Standard_Integer num = 1);
  
  //! Translates an IGES or STEP
  //! entity identified by the rank num in the model.
  //! false is returned if no shape is produced.
  Standard_EXPORT Standard_Boolean TransferOne (const Standard_Integer num);
  
  //! Translates an IGES or STEP
  //! entity in the model. true is returned if a shape is
  //! produced; otherwise, false is returned.
  Standard_EXPORT Standard_Boolean TransferEntity (const Handle(Standard_Transient)& start);
  
  //! Translates a list of entities.
  //! Returns the number of IGES or STEP entities that were
  //! successfully translated. The list can be produced with GiveList.
  //! Warning - This function does not clear the existing output shapes.
  Standard_EXPORT Standard_Integer TransferList (const Handle(TColStd_HSequenceOfTransient)& list);
  
  //! Translates all translatable
  //! roots and returns the number of successful translations.
  //! Warning - This function clears existing output shapes first.
  Standard_EXPORT Standard_Integer TransferRoots();
  
  //! Clears the list of shapes that
  //! may have accumulated in calls to TransferOne or TransferRoot.C
  void ClearShapes() { theshapes.Clear(); }
  
  //! Returns the number of shapes produced by translation.
  Standard_Integer NbShapes() const { return theshapes.Length(); }
  
  //! Returns the shape resulting
  //! from a translation and identified by the rank num.
  //! num equals 1 by default. In other words, the first shape
  //! resulting from the translation is returned.
  const TopoDS_Shape & Shape (const Standard_Integer num = 1) const { return theshapes.Value(num); }
  
  //! Returns all of the results in
  //! a single shape which is:
  //! - a null shape if there are no results,
  //! - a shape if there is one result,
  //! - a compound containing the resulting shapes if there are more than one.
  Standard_EXPORT TopoDS_Shape OneShape() const;

 protected:
  
  //! Returns a sequence of produced shapes
  //szv:TopTools_SequenceOfShape& Shapes() { return theshapes; }
  TopTools_SequenceOfShape theshapes;

  Standard_Boolean therootsta;
  TColStd_SequenceOfTransient theroots;

 private:

  Handle(XSControl_WorkSession) thesession;
};

#endif // _XSControl_Reader_HeaderFile

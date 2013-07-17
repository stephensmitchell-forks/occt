// Copyright (c) 2013 OPEN CASCADE SAS
//
// The content of this file is subject to the Open CASCADE Technology Public
// License Version 6.5 (the "License"). You may not use the content of this file
// except in compliance with the License. Please obtain a copy of the License
// at http://www.opencascade.org and read it completely before using this file.
//
// The Initial Developer of the Original Code is Open CASCADE S.A.S., having its
// main offices at: 1, place des Freres Montgolfier, 78280 Guyancourt, France.
//
// The Original Code and all software distributed under the License is
// distributed on an "AS IS" basis, without warranty of any kind, and the
// Initial Developer hereby disclaims all such warranties, including without
// limitation, any warranties of merchantability, fitness for a particular
// purpose or non-infringement. Please see the License for the specific terms
// and conditions governing the rights and limitations under the License.

#ifndef _XSControl_Reader_HeaderFile
#define _XSControl_Reader_HeaderFile

#include <Standard.hxx>
#include <Standard_DefineAlloc.hxx>
#include <Standard_Macro.hxx>
#include <XSControl_WorkSession.hxx>
#include <Standard_Boolean.hxx>
#include <TColStd_SequenceOfTransient.hxx>
#include <TopTools_SequenceOfShape.hxx>
#include <Standard_CString.hxx>
#include <IFSelect_ReturnStatus.hxx>
#include <Handle_Interface_InterfaceModel.hxx>
#include <Handle_TColStd_HSequenceOfTransient.hxx>
#include <Handle_Standard_Transient.hxx>
#include <Standard_Integer.hxx>
#include <IFSelect_PrintCount.hxx>

class XSControl_WorkSession;
class Interface_InterfaceModel;
class TColStd_HSequenceOfTransient;
class Standard_Transient;
class TopTools_SequenceOfShape;
class TopoDS_Shape;



//! A groundwork to convert a shape to data which complies <br>
//! with a particular norm. This data can be that of a whole <br>
//! model or that of a specific list of entities in the model. <br>
//! You specify the list using a single selection or a <br>
//! combination of selections. A selection is an operator which <br>
//! computes a list of entities from a list given in input. To <br>
//! specify the input, you can use: <br>
//! - A predefined selection such as "xst-transferrable-roots" <br>
//! - A filter based on a  signature. <br>
//! A signature is an operator which returns a string from an <br>
//! entity according to its type. <br>
//! For example: <br>
//! - "xst-type" (CDL) <br>
//! - "iges-level" <br>
//! - "step-type". <br>
//! A filter can be based on a signature by giving a value to <br>
//! be matched by the string returned. For example, <br>
//! "xst-type(Curve)". <br>
//! If no list is specified, the selection computes its list of <br>
//! entities from the whole model. To use this class, you have to <br>
//! initialize the transfer norm first, as shown in the example below. <br>
//! Example: <br>
//! Control_Reader reader; <br>
//! IFSelect_ReturnStatus status = reader.ReadFile (filename.); <br>
//! When using IGESControl_Reader or STEPControl_Reader - as the <br>
//! above example shows - the reader initializes the norm directly. <br>
//! Note that loading the file only stores the data. It does <br>
//! not translate this data. Shapes are accumulated by <br>
//! successive transfers. The last shape is cleared by: <br>
//! - ClearShapes which allows you to handle a new batch <br>
//! - TransferRoots which restarts the list of shapes from scratch. <br>
class XSControl_Reader  {
public:

  DEFINE_STANDARD_ALLOC

  //! Creates a Reader from scratch (creates an empty WorkSession) <br>
//!           A WorkSession or a Controller must be provided before running <br>
  Standard_EXPORT   XSControl_Reader();
  //! Creates a Reader from scratch, with a norm name which <br>
//!           identifies a Controller <br>
  Standard_EXPORT   XSControl_Reader(const Standard_CString norm);
  //! Creates a Reader from an already existing Session, with a <br>
//!           Controller already set <br>//! Virtual destructor <br>
  Standard_EXPORT   XSControl_Reader(const Handle(XSControl_WorkSession)& WS,const Standard_Boolean scratch = Standard_True);
Standard_EXPORT virtual ~XSControl_Reader() {}
  //! Sets a specific norm to <me> <br>
//!           Returns True if done, False if <norm> is not available <br>
  Standard_EXPORT     Standard_Boolean SetNorm(const Standard_CString norm) ;
  //! Sets a specific session to <me> <br>
  Standard_EXPORT     void SetWS(const Handle(XSControl_WorkSession)& WS,const Standard_Boolean scratch = Standard_True) ;
  //! Returns the session used in <me> <br>
  Standard_EXPORT     Handle_XSControl_WorkSession WS() const;
  //! Loads a file and returns the read status <br>
//!           Zero for a Model which compies with the Controller <br>
  Standard_EXPORT     IFSelect_ReturnStatus ReadFile(const Standard_CString filename) ;
  //! Returns the model. It can then be consulted (header, product) <br>
  Standard_EXPORT     Handle_Interface_InterfaceModel Model() const;
  //! Returns a list of entities from the IGES or STEP file <br>
//! according to the following rules: <br>
//! - if first and second are empty strings, the whole file is selected. <br>
//! - if first is an entity number or label, the entity referred to is selected. <br>
//! - if first is a list of entity numbers/labels separated by commas, the entities referred to are selected, <br>
//! - if first is the name of a selection in the worksession and second is not defined, <br>
//! the list contains the standard output for that selection. <br>
//! - if first is the name of a selection and second is defined, the criterion defined <br>
//! by second is applied to the result of the first selection. <br>
//!  A selection is an operator which computes a list of entities from a list given in <br>
//! input according to its type. If no list is specified, the selection computes its <br>
//! list of entities from the whole model. <br>
//!   A selection can be: <br>
//! - A predefined selection (xst-transferrable-mode) <br>
//! - A filter based on a signature <br>
//! A Signature is an operator which returns a string from an entity according to its type. For example: <br>
//! - "xst-type" (CDL) <br>
//! - "iges-level" <br>
//! - "step-type". <br>
//! For example, if you wanted to select only the advanced_faces in a STEP file you <br>
//! would use the following code: <br>
//! Example <br>
//! Reader.GiveList("xst-transferrable-roots","step-type(ADVANCED_FACE)"); <br>
//! Warning <br>
//! If the value given to second is incorrect, it will simply be ignored. <br>
  Standard_EXPORT     Handle_TColStd_HSequenceOfTransient GiveList(const Standard_CString first = "",const Standard_CString second = "") ;
  //! Computes a List of entities from the model as follows <br>
//!           <first> beeing a Selection, <ent> beeing an entity or a list <br>
//!           of entities (as a HSequenceOfTransient) : <br>
//!             the standard result of this selection applied to this list <br>
//!           if <first> is erroneous, a null handle is returned <br>
  Standard_EXPORT     Handle_TColStd_HSequenceOfTransient GiveList(const Standard_CString first,const Handle(Standard_Transient)& ent) ;
  //! Determines the list of root entities which are candidate for <br>
//!           a transfer to a Shape, and returns the number <br>
//!           of entities in the list <br>
  Standard_EXPORT   virtual  Standard_Integer NbRootsForTransfer() ;
  //! Returns an IGES or STEP root <br>
//!           entity for translation. The entity is identified by its <br>
//!           rank in a list. <br>
  Standard_EXPORT     Handle_Standard_Transient RootForTransfer(const Standard_Integer num = 1) ;
  //! Translates a root identified by the rank num in the model. <br>
//!           false is returned if no shape is produced. <br>
  Standard_EXPORT     Standard_Boolean TransferOneRoot(const Standard_Integer num = 1) ;
  //! Translates an IGES or STEP <br>
//!           entity identified by the rank num in the model. <br>
//!           false is returned if no shape is produced. <br>
  Standard_EXPORT     Standard_Boolean TransferOne(const Standard_Integer num) ;
  //! Translates an IGES or STEP <br>
//!           entity in the model. true is returned if a shape is <br>
//!           produced; otherwise, false is returned. <br>
  Standard_EXPORT     Standard_Boolean TransferEntity(const Handle(Standard_Transient)& start) ;
  //! Translates a list of entities. <br>
//!           Returns the number of IGES or STEP entities that were <br>
//!           successfully translated. The list can be produced with GiveList. <br>
//!           Warning - This function does not clear the existing output shapes. <br>
  Standard_EXPORT     Standard_Integer TransferList(const Handle(TColStd_HSequenceOfTransient)& list) ;
  //! Translates all translatable <br>
//!           roots and returns the number of successful translations. <br>
//!           Warning - This function clears existing output shapes first. <br>
  Standard_EXPORT     Standard_Integer TransferRoots() ;
  //! Clears the list of shapes that <br>
//!           may have accumulated in calls to TransferOne or TransferRoot.C <br>
  Standard_EXPORT     void ClearShapes() ;
  //! Returns the number of shapes produced by translation. <br>
  Standard_EXPORT     Standard_Integer NbShapes() const;
  //! Returns the shape resulting <br>
//! from a translation and identified by the rank num. <br>
//! num equals 1 by default. In other words, the first shape <br>
//! resulting from the translation is returned. <br>
  Standard_EXPORT     TopoDS_Shape Shape(const Standard_Integer num = 1) const;
  //! Returns all of the results in <br>
//! a single shape which is: <br>
//! - a null shape if there are no results, <br>
//! - a shape if there is one result, <br>
//! - a compound containing the resulting shapes if there are more than one. <br>
  Standard_EXPORT     TopoDS_Shape OneShape() const;
  //! Prints the check list attached to loaded data, on the Standard <br>
//!            Trace File (starts at cout) <br>
//!           All messages or fails only, according to <failsonly> <br>
//!           mode = 0 : per entity, prints messages <br>
//!           mode = 1 : per message, just gives count of entities per check <br>
//!           mode = 2 : also gives entity numbers <br>
  Standard_EXPORT     void PrintCheckLoad(const Standard_Boolean failsonly,const IFSelect_PrintCount mode) const;
  //! Displays check results for the <br>
//! last translation of IGES or STEP entities to Open CASCADE <br>
//! entities. Only fail messages are displayed if failsonly is <br>
//! true. All messages are displayed if failsonly is <br>
//! false. mode determines the contents and the order of the <br>
//! messages according to the terms of the IFSelect_PrintCount enumeration. <br>
  Standard_EXPORT     void PrintCheckTransfer(const Standard_Boolean failsonly,const IFSelect_PrintCount mode) const;
  //!  Displays the statistics for <br>
//! the last translation. what defines the kind of <br>
//! statistics that are displayed as follows: <br>
//! - 0 gives general statistics (number of translated roots, <br>
//! number of warnings, number of fail messages), <br>
//! - 1 gives root results, <br>
//!    - 2 gives statistics for all checked entities, <br>
//! - 3 gives the list of translated entities, <br>
//! - 4 gives warning and fail messages, <br>
//! - 5 gives fail messages only. <br>
//! The use of mode depends on the value of what. If what is 0, <br>
//! mode is ignored. If what is 1, 2 or 3, mode defines the following: <br>
//! - 0 lists the numbers of IGES or STEP entities in the respective model <br>
//! - 1 gives the number, identifier, type and result <br>
//! type for each IGES or STEP entity and/or its status <br>
//! (fail, warning, etc.) <br>
//! - 2 gives maximum information for each IGES or STEP entity (i.e. checks) <br>
//! - 3 gives the number of entities per type of IGES or STEP entity <br>
//! - 4 gives the number of IGES or STEP entities per result type and/or status <br>
//! - 5 gives the number of pairs (IGES or STEP or result type and status) <br>
//! - 6 gives the number of pairs (IGES or STEP or result type <br>
//! and status) AND the list of entity numbers in the IGES or STEP model. <br>
//!   If what is 4 or 5, mode defines the warning and fail <br>
//! messages as follows: <br>
//! - if mode is 0 all warnings and checks per entity are returned <br>
//! - if mode is 2 the list of entities per warning is returned. <br>
//!   If mode is not set, only the list of all entities per warning is given. <br>
  Standard_EXPORT     void PrintStatsTransfer(const Standard_Integer what,const Standard_Integer mode = 0) const;
  //! Gives statistics about Transfer <br>
  Standard_EXPORT     void GetStatsTransfer(const Handle(TColStd_HSequenceOfTransient)& list,Standard_Integer& nbMapped,Standard_Integer& nbWithResult,Standard_Integer& nbWithFail) const;

protected:
  //! Returns a sequence of produced shapes <br>
  Standard_EXPORT     TopTools_SequenceOfShape& Shapes();
  
  Standard_Boolean therootsta;
  TColStd_SequenceOfTransient theroots;

private:

  Handle_XSControl_WorkSession thesession;
  TopTools_SequenceOfShape theshapes;


};
#endif

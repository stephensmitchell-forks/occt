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

#ifndef _IGESCAFControl_Writer_HeaderFile
#define _IGESCAFControl_Writer_HeaderFile

#include <Standard.hxx>
#include <Standard_DefineAlloc.hxx>
#include <Standard_Macro.hxx>
#include <Standard_Boolean.hxx>
#include <IGESControl_Writer.hxx>
#include <XSControl_WorkSession.hxx>
#include <Handle_TDocStd_Document.hxx>
#include <Standard_CString.hxx>

class XSControl_WorkSession;
class TDocStd_Document;
class TCollection_AsciiString;
class TDF_LabelSequence;
class TopoDS_Shape;
class XCAFPrs_DataMapOfShapeStyle;
class XCAFPrs_DataMapOfStyleTransient;
class TopTools_MapOfShape;
class XCAFPrs_Style;


//! Provides a tool to write DECAF document to the <br>
//! IGES file. Besides transfer of shapes (including <br>
//! assemblies) provided by IGESControl, supports also <br>
//! colors and part names <br>
//! IGESCAFControl_Writer writer(); <br>
//!   Methods for writing IGES file: <br>
//! writer.Transfer (Document); <br>
//! writer.Write("filename") or writer.Write(OStream)  or <br>
//! writer.Perform(Document,"filename"); <br>
//!   Methods for managing the writing of attributes. <br>
//!   Colors <br>
//! writer.SetColorMode(colormode); <br>
//! Standard_Boolean colormode = writer.GetColorMode(); <br>
//!   Layers <br>
//! writer.SetLayerMode(layermode); <br>
//! Standard_Boolean layermode = writer.GetLayerMode(); <br>
//! Names <br>
//! writer.SetNameMode(namemode); <br>
//! Standard_Boolean namemode = writer.GetNameMode(); <br>
class IGESCAFControl_Writer  : public IGESControl_Writer {
public:

  DEFINE_STANDARD_ALLOC

  //! Creates a writer with an empty <br>
  //! IGES model and sets ColorMode, LayerMode and NameMode to Standard_True. <br>
  Standard_EXPORT   IGESCAFControl_Writer();
  //! Creates a reader tool and attaches it to an already existing Session <br>
  //!          Clears the session if it was not yet set for IGES <br>
  Standard_EXPORT   IGESCAFControl_Writer(const Handle(XSControl_WorkSession)& WS,const Standard_Boolean scratch = Standard_True);
  //! Transfers a document to a IGES model <br>
  //!          Returns True if translation is OK <br>
  Standard_EXPORT     Standard_Boolean Transfer(const Handle(TDocStd_Document)& doc) ;
  
  Standard_EXPORT     Standard_Boolean Perform(const Handle(TDocStd_Document)& doc,const TCollection_AsciiString& filename) ;
  //! Transfers a document and writes it to a IGES file <br>
  //!           Returns True if translation is OK <br>
  Standard_EXPORT     Standard_Boolean Perform(const Handle(TDocStd_Document)& doc,const Standard_CString filename) ;
  //! Set ColorMode for indicate write Colors or not. <br>
  Standard_EXPORT     void SetColorMode(const Standard_Boolean colormode) ;
  
  Standard_EXPORT     Standard_Boolean GetColorMode() const;
  //! Set NameMode for indicate write Name or not. <br>
  Standard_EXPORT     void SetNameMode(const Standard_Boolean namemode) ;
  
  Standard_EXPORT     Standard_Boolean GetNameMode() const;
  //! Set LayerMode for indicate write Layers or not. <br>
  Standard_EXPORT     void SetLayerMode(const Standard_Boolean layermode) ;
  
  Standard_EXPORT     Standard_Boolean GetLayerMode() const;

protected:

  //! Transfers labels to a IGES model <br>
  //!          Returns True if translation is OK <br>
  Standard_EXPORT     Standard_Boolean Transfer(const TDF_LabelSequence& labels) ;
  //! Reads colors from DECAF document and assigns them <br>
  //!          to corresponding IGES entities <br>
  Standard_EXPORT     Standard_Boolean WriteAttributes(const TDF_LabelSequence& labels) ;
  //! Reads layers from DECAF document and assigns them <br>
  //!          to corresponding IGES entities <br>
  Standard_EXPORT     Standard_Boolean WriteLayers(const TDF_LabelSequence& labels) ;
  //! Recursivile iterates on subshapes and assign names <br>
  //!          to IGES entity <br>
  Standard_EXPORT     Standard_Boolean WriteNames(const TDF_LabelSequence& labels) ;

private:

  //! Recursively iterates on subshapes and assigns colors <br>
  //!          to faces and edges (if set) <br>
  Standard_EXPORT     void MakeColors(const TopoDS_Shape& S,const XCAFPrs_DataMapOfShapeStyle& settings,XCAFPrs_DataMapOfStyleTransient& colors,TopTools_MapOfShape& Map,const XCAFPrs_Style& inherit) ;

  Standard_Boolean myColorMode;
  Standard_Boolean myNameMode;
  Standard_Boolean myLayerMode;

};
#endif

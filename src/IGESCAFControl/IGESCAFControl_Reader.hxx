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

#ifndef _IGESCAFControl_Reader_HeaderFile
#define _IGESCAFControl_Reader_HeaderFile

#include <Standard.hxx>
#include <Standard_DefineAlloc.hxx>
#include <Standard_Macro.hxx>
#include <Standard_Boolean.hxx>
#include <IGESControl_Reader.hxx>
#include <XSControl_WorkSession.hxx>
#include <Handle_TDocStd_Document.hxx>
#include <Standard_CString.hxx>

class XSControl_WorkSession;
class TDocStd_Document;
class TCollection_AsciiString;

//! Provides a tool to read IGES file and put it into <br>
//!          DECAF document. Besides transfer of shapes (including <br>
//!          assemblies) provided by IGESControl, supports also <br>
//!          colors and part names <br>
//!          IGESCAFControl_Reader reader; Methods for translation of an IGES file: <br>
//!          reader.ReadFile("filename"); <br>
//!          reader.Transfer(Document); or <br>
//!          reader.Perform("filename",doc); <br>
//!          Methods for managing reading attributes. <br>
//!          Colors <br>
//!          reader.SetColorMode(colormode); <br>
//!          Standard_Boolean colormode = reader.GetColorMode(); <br>
//!          Layers <br>
//!          reader.SetLayerMode(layermode); <br>
//!          Standard_Boolean layermode = reader.GetLayerMode(); <br>
//!          Names <br>
//!          reader.SetNameMode(namemode); <br>
//!          Standard_Boolean namemode = reader.GetNameMode(); <br>
class IGESCAFControl_Reader  : public IGESControl_Reader
{
public:

  DEFINE_STANDARD_ALLOC

  //! Creates a reader with an empty <br>
  //!          IGES model and sets ColorMode, LayerMode and NameMode to Standard_True. <br>
  Standard_EXPORT   IGESCAFControl_Reader();
  //! Creates a reader tool and attaches it to an already existing Session <br>
  //! 	    Clears the session if it was not yet set for IGES <br>
  Standard_EXPORT   IGESCAFControl_Reader(const Handle(XSControl_WorkSession)& WS,const Standard_Boolean scratch = Standard_True);
  //! Translates currently loaded IGES file into the document <br>
  //!          Returns True if succeeded, and False in case of fail <br>
  Standard_EXPORT     Standard_Boolean Transfer(Handle(TDocStd_Document)& doc) ;
  
  Standard_EXPORT     Standard_Boolean Perform(const TCollection_AsciiString& filename,Handle(TDocStd_Document)& doc) ;
  //! Translate IGES file given by filename into the document <br>
  //!          Return True if succeeded, and False in case of fail <br>
  Standard_EXPORT     Standard_Boolean Perform(const Standard_CString filename,Handle(TDocStd_Document)& doc) ;
  //! Set ColorMode for indicate read Colors or not. <br>
  Standard_EXPORT     void SetColorMode(const Standard_Boolean colormode) ;
  
  Standard_EXPORT     Standard_Boolean GetColorMode() const;
  //! Set NameMode for indicate read Name or not. <br>
  Standard_EXPORT     void SetNameMode(const Standard_Boolean namemode) ;
  
  Standard_EXPORT     Standard_Boolean GetNameMode() const;
  //! Set LayerMode for indicate read Layers or not. <br>
  Standard_EXPORT     void SetLayerMode(const Standard_Boolean layermode) ;
  
  Standard_EXPORT     Standard_Boolean GetLayerMode() const;

protected:
  //! Reads colors of IGES entities and sets <br>
  //! corresponding color assignments in the DECAF document <br>
  Standard_EXPORT     Standard_Boolean ReadColors(Handle(TDocStd_Document)& doc) const;
  //! Reads Names of IGES entities and sets <br>
  //! corresponding name to label with shape in the DECAF document <br>
  Standard_EXPORT     Standard_Boolean ReadNames(Handle(TDocStd_Document)& doc) const;
  //! Reads layers of parts defined in the IGES model and <br>
  //! set reference between shape and layers in the DECAF document <br>
  Standard_EXPORT     Standard_Boolean ReadLayers(Handle(TDocStd_Document)& doc) const;

private:
  Standard_Boolean myColorMode;
  Standard_Boolean myNameMode;
  Standard_Boolean myLayerMode;

};
#endif

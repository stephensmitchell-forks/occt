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

#ifndef _STEPCAFControl_Writer_HeaderFile
#define _STEPCAFControl_Writer_HeaderFile

#include <Standard.hxx>
#include <Standard_DefineAlloc.hxx>
#include <Standard_Macro.hxx>
#include <STEPControl_Writer.hxx>
#include <Handle_STEPCAFControl_DictionaryOfExternFile.hxx>
#include <STEPCAFControl_DataMapOfLabelShape.hxx>
#include <STEPCAFControl_DataMapOfLabelExternFile.hxx>
#include <Standard_Boolean.hxx>
#include <MoniTool_DataMapOfShapeTransient.hxx>
#include <XSControl_WorkSession.hxx>
#include <IFSelect_ReturnStatus.hxx>
#include <Standard_CString.hxx>
#include <Handle_TDocStd_Document.hxx>
#include <STEPControl_StepModelType.hxx>
#include <Handle_STEPCAFControl_ExternFile.hxx>

class STEPCAFControl_DictionaryOfExternFile;
class XSControl_WorkSession;
class TDocStd_Document;
class TDF_Label;
class TCollection_AsciiString;
class STEPCAFControl_ExternFile;
class STEPControl_Writer;
class TDF_LabelSequence;
class TopoDS_Shape;


//! Provides a tool to write DECAF document to the <br>
//!          STEP file. Besides transfer of shapes (including <br>
//!          assemblies) provided by STEPControl, supports also <br>
//!          colors and part names <br>
//! <br>
//!          Also supports multifile writing <br>
class STEPCAFControl_Writer  {
public:

  DEFINE_STANDARD_ALLOC

  //! Creates a writer with an empty <br>
//! STEP model and sets ColorMode, LayerMode, NameMode and <br>
//! PropsMode to Standard_True. <br>
  Standard_EXPORT   STEPCAFControl_Writer();
  //! Creates a reader tool and attaches it to an already existing Session <br>
//!          Clears the session if it was not yet set for STEP <br>
//!          Clears the internal data structures <br>
  Standard_EXPORT   STEPCAFControl_Writer(const Handle(XSControl_WorkSession)& WS,const Standard_Boolean scratch = Standard_True);
  //! Clears the internal data structures and attaches to a new session <br>
//!          Clears the session if it was not yet set for STEP <br>
  Standard_EXPORT     void Init(const Handle(XSControl_WorkSession)& WS,const Standard_Boolean scratch = Standard_True) ;
  //! Writes all the produced models into file <br>
//!          In case of multimodel with extern references, <br>
//!          filename will be a name of root file, all other files <br>
//!          have names of corresponding parts <br>
//!          Provided for use like single-file writer <br>
  Standard_EXPORT     IFSelect_ReturnStatus Write(const Standard_CString filename) ;
  
  Standard_EXPORT     Standard_Boolean Transfer(const Handle(TDocStd_Document)& doc,const STEPControl_StepModelType mode = STEPControl_AsIs,const Standard_CString multi = 0) ;
  //! Transfers a document (or single label) to a STEP model <br>
//!          The mode of translation of shape is AsIs <br>
//!          If multi is not null pointer, it switches to multifile <br>
//!          mode (with external refs), and string pointed by <multi> <br>
//!          gives prefix for names of extern files (can be empty string) <br>
//!          Returns True if translation is OK <br>
  Standard_EXPORT     Standard_Boolean Transfer(const TDF_Label& L,const STEPControl_StepModelType mode = STEPControl_AsIs,const Standard_CString multi = 0) ;
  
  Standard_EXPORT     Standard_Boolean Perform(const Handle(TDocStd_Document)& doc,const TCollection_AsciiString& filename) ;
  //! Transfers a document and writes it to a STEP file <br>
//!           Returns True if translation is OK <br>
  Standard_EXPORT     Standard_Boolean Perform(const Handle(TDocStd_Document)& doc,const Standard_CString filename) ;
  //! Returns data on external files <br>
//!          Returns Null handle if no external files are read <br>
  Standard_EXPORT    const Handle_STEPCAFControl_DictionaryOfExternFile& ExternFiles() const;
  //! Returns data on external file by its original label <br>
//!          Returns False if no external file with given name is read <br>
  Standard_EXPORT     Standard_Boolean ExternFile(const TDF_Label& L,Handle(STEPCAFControl_ExternFile)& ef) const;
  //! Returns data on external file by its name <br>
//!          Returns False if no external file with given name is read <br>
  Standard_EXPORT     Standard_Boolean ExternFile(const Standard_CString name,Handle(STEPCAFControl_ExternFile)& ef) const;
  //! Returns basic reader for root file <br>
  Standard_EXPORT     STEPControl_Writer& ChangeWriter() ;
  //! Returns basic reader as const <br>
  Standard_EXPORT    const STEPControl_Writer& Writer() const;
  //! Set ColorMode for indicate write Colors or not. <br>
  Standard_EXPORT     void SetColorMode(const Standard_Boolean colormode) ;
  
  Standard_EXPORT     Standard_Boolean GetColorMode() const;
  //! Set NameMode for indicate write Name or not. <br>
  Standard_EXPORT     void SetNameMode(const Standard_Boolean namemode) ;
  
  Standard_EXPORT     Standard_Boolean GetNameMode() const;
  //! Set LayerMode for indicate write Layers or not. <br>
  Standard_EXPORT     void SetLayerMode(const Standard_Boolean layermode) ;
  
  Standard_EXPORT     Standard_Boolean GetLayerMode() const;
  //! PropsMode for indicate write Validation properties or not. <br>
  Standard_EXPORT     void SetPropsMode(const Standard_Boolean propsmode) ;
  
  Standard_EXPORT     Standard_Boolean GetPropsMode() const;
  //! Set SHUO mode for indicate write SHUO or not. <br>
  Standard_EXPORT     void SetSHUOMode(const Standard_Boolean shuomode) ;
  
  Standard_EXPORT     Standard_Boolean GetSHUOMode() const;
  //! Set dimtolmode for indicate write D&GTs or not. <br>
  Standard_EXPORT     void SetDimTolMode(const Standard_Boolean dimtolmode) ;
  
  Standard_EXPORT     Standard_Boolean GetDimTolMode() const;
  //! Set dimtolmode for indicate write D&GTs or not. <br>
  Standard_EXPORT     void SetMaterialMode(const Standard_Boolean matmode) ;
  
  Standard_EXPORT     Standard_Boolean GetMaterialMode() const;





protected:

  //! Transfers labels to a STEP model <br>
//!          Returns True if translation is OK <br>
//!          isExternFile setting from TransferExternFiles method <br>
  Standard_EXPORT     Standard_Boolean Transfer(STEPControl_Writer& wr,const TDF_LabelSequence& labels,const STEPControl_StepModelType mode = STEPControl_AsIs,const Standard_CString multi = 0,const Standard_Boolean isExternFile = Standard_False) ;
  //! Parses assembly structure of label L, writes all the simple <br>
//!          shapes each to its own file named by name of its label plus <br>
//!          prefix <br>
//!          Returns shape representing that assembly structure <br>
//!          in the form of nested empty compounds (and a sequence of <br>
//!          labels which are newly written nodes of this assembly) <br>
  Standard_EXPORT     TopoDS_Shape TransferExternFiles(const TDF_Label& L,const STEPControl_StepModelType mode,TDF_LabelSequence& Lseq,const Standard_CString prefix = "") ;
  //! Write external references to STEP <br>
  Standard_EXPORT     Standard_Boolean WriteExternRefs(const Handle(XSControl_WorkSession)& WS,const TDF_LabelSequence& labels) const;
  //! Write colors assigned to specified labels, to STEP model <br>
  Standard_EXPORT     Standard_Boolean WriteColors(const Handle(XSControl_WorkSession)& WS,const TDF_LabelSequence& labels) ;
  //! Write names assigned to specified labels, to STEP model <br>
  Standard_EXPORT     Standard_Boolean WriteNames(const Handle(XSControl_WorkSession)& WS,const TDF_LabelSequence& labels) const;
  //! Write D&GTs assigned to specified labels, to STEP model <br>
  Standard_EXPORT     Standard_Boolean WriteDGTs(const Handle(XSControl_WorkSession)& WS,const TDF_LabelSequence& labels) const;
  //! Write materials assigned to specified labels, to STEP model <br>
  Standard_EXPORT     Standard_Boolean WriteMaterials(const Handle(XSControl_WorkSession)& WS,const TDF_LabelSequence& labels) const;
  //! Write validation properties assigned to specified labels, <br>
//!          to STEP model <br>
  Standard_EXPORT     Standard_Boolean WriteValProps(const Handle(XSControl_WorkSession)& WS,const TDF_LabelSequence& labels,const Standard_CString multi) const;
  //! Write layers assigned to specified labels, to STEP model <br>
  Standard_EXPORT     Standard_Boolean WriteLayers(const Handle(XSControl_WorkSession)& WS,const TDF_LabelSequence& labels) const;
  //! Write SHUO assigned to specified component, to STEP model <br>
  Standard_EXPORT     Standard_Boolean WriteSHUOs(const Handle(XSControl_WorkSession)& WS,const TDF_LabelSequence& labels) ;




private:



STEPControl_Writer myWriter;
Handle_STEPCAFControl_DictionaryOfExternFile myFiles;
STEPCAFControl_DataMapOfLabelShape myLabels;
STEPCAFControl_DataMapOfLabelExternFile myLabEF;
Standard_Boolean myColorMode;
Standard_Boolean myNameMode;
Standard_Boolean myLayerMode;
Standard_Boolean myPropsMode;
Standard_Boolean mySHUOMode;
MoniTool_DataMapOfShapeTransient myMapCompMDGPR;
Standard_Boolean myDGTMode;
Standard_Boolean myMatMode;


};





// other Inline functions and methods (like "C++: function call" methods)


#endif

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

#ifndef _IGESToBRep_Reader_HeaderFile
#define _IGESToBRep_Reader_HeaderFile

#include <Standard.hxx>
#include <Standard_DefineAlloc.hxx>
#include <Standard_Macro.hxx>
#include <Handle_IGESData_IGESModel.hxx>
#include <Standard_Boolean.hxx>
#include <TopTools_SequenceOfShape.hxx>
#include <IGESToBRep_Actor.hxx>
#include <Transfer_TransientProcess.hxx>
#include <Standard_Integer.hxx>
#include <Standard_CString.hxx>
#include <Standard_Real.hxx>

class IGESData_IGESModel;
class TopoDS_Shape;


//! A simple way to read geometric IGES data. <br>
//!           Encapsulates reading file and calling transfer tools <br>
class IGESToBRep_Reader
{
public:

  DEFINE_STANDARD_ALLOC

  //! Creates a Reader <br>
  Standard_EXPORT   IGESToBRep_Reader();

  //! Loads a Model from a file.Returns 0 if success. <br>
  //!           returns 1 if the file could not be opened, <br>
  //!           returns -1 if an error occurred while the file was being loaded. <br>
  Standard_EXPORT     Standard_Integer LoadFile(const Standard_CString filename);

  //! Specifies a Model to work on <br>
  //!           Also clears the result and Done status, sets TransientProcess <br>
  Standard_EXPORT     void SetModel(const Handle(IGESData_IGESModel)& model);

  //! Returns the Model to be worked on. <br>
  Standard_EXPORT     Handle(IGESData_IGESModel) Model() const;

  //! Allows to set an already defined TransientProcess <br>
//!           (to be called after LoadFile or SetModel) <br>
  Standard_EXPORT     void SetTransientProcess(const Handle(Transfer_TransientProcess)& TP);

  //! Returns the TransientProcess <br>
  Standard_EXPORT     Transfer_TransientProcess_Handle TransientProcess() const;

  //! Returns "theActor" <br>
  Standard_EXPORT     Handle(IGESToBRep_Actor) Actor() const;

  //! Clears the results between two translation operations. <br>
  Standard_EXPORT     void Clear();

  //! Checks the IGES file that was <br>
  //! loaded into memory. Displays error messages in the default <br>
  //! message file if withprint is true. Returns True if no fail <br>
  //! message was found and False if there was at least one fail message. <br>
  Standard_EXPORT     Standard_Boolean Check(const Standard_Boolean withprint) const;

  //! Translates root entities in an <br>
  //! IGES file. Standard_True is the default value and means that only <br>
  //! visible root entities are translated. Standard_False <br>
  //! translates all of the roots (visible and invisible). <br>
  Standard_EXPORT     void TransferRoots(const Standard_Boolean onlyvisible = Standard_True);

  //! Transfers an Entity given its rank in the Model (Root or not) <br>
  //!           Returns True if it is recognized as Geom-Topol. <br>
  //!           (But it can have failed : see IsDone) <br>
  Standard_EXPORT     Standard_Boolean Transfer(const Standard_Integer num);

  //! Returns True if the LAST Transfer/TransferRoots was a success <br>
  Standard_EXPORT     Standard_Boolean IsDone() const;

  //! Returns the Tolerance which has been actually used, converted <br>
  //!           in millimeters <br>
  //!          (either that from File or that from Session, according the mode) <br>
  Standard_EXPORT     Standard_Real UsedTolerance() const;

  //! Returns the number of shapes produced by the translation. <br>
  Standard_EXPORT     Standard_Integer NbShapes() const;

  //!  Returns the num the resulting shape in a translation operation. <br>
  Standard_EXPORT     TopoDS_Shape Shape(const Standard_Integer num = 1) const;

  //! Returns all of the results in a <br>
  //! single shape which is: <br>
  //! - a null shape if there are no results, <br>
  //! - a shape if there is one result, <br>
  //! - a compound containing the resulting shapes if there are several. <br>
  Standard_EXPORT     TopoDS_Shape OneShape() const;

private:

  Handle(IGESData_IGESModel) theModel;
  Standard_Boolean theDone;
  TopTools_SequenceOfShape theShapes;
  Handle(IGESToBRep_Actor) theActor;
  Handle(Transfer_TransientProcess) theProc;

};
#endif

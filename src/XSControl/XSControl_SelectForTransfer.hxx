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

#ifndef _XSControl_SelectForTransfer_HeaderFile
#define _XSControl_SelectForTransfer_HeaderFile

#include <Standard.hxx>
#include <Standard_DefineHandle.hxx>
#include <XSControl_TransferReader.hxx>
#include <Transfer_ActorOfTransientProcess.hxx>
#include <IFSelect_SelectExtract.hxx>
#include <Standard_Boolean.hxx>
#include <Standard_Integer.hxx>
#include <Handle_Standard_Transient.hxx>
#include <Handle_Interface_InterfaceModel.hxx>
#include <Handle_MMgt_TShared.hxx>

class XSControl_TransferReader;
class Transfer_ActorOfTransientProcess;
class Standard_Transient;
class Interface_InterfaceModel;
class TCollection_AsciiString;
class Standard_Transient;
class Handle(Standard_Type);
class Handle(MMgt_TShared);
class Handle(IFSelect_SelectExtract);

DEFINE_STANDARD_HANDLE(XSControl_SelectForTransfer,IFSelect_SelectExtract)

//! This selection selects the entities which are recognised for <br>
//!           transfer by an Actor for Read : current one or another one. <br>
//! <br>
//!           An Actor is an operator which runs transfers from interface <br>
//!           entities to objects for Imagine. It has a method to recognize <br>
//!           the entities it can process (by default, it recognises all, <br>
//!           this method can be redefined). <br>
//! <br>
//!           A TransferReader brings an Actor, according to the currently <br>
//!           selected norm and transfer conditions. <br>
//! <br>
//!           This selection considers, either the current Actor (brought by <br>
//!           the TransferReader, updated as required), or a precise one. <br>
class XSControl_SelectForTransfer : public IFSelect_SelectExtract {

public:

  //! Creates a SelectForTransfer, non initialised <br>
//!           it sorts nothing, unless an Actor has been defined <br>
  Standard_EXPORT   XSControl_SelectForTransfer();
  //! Creates a SelectForTransfer, which will work with the <br>
//!           currently defined Actor brought by the TransferReader <br>
  Standard_EXPORT   XSControl_SelectForTransfer(const Handle(XSControl_TransferReader)& TR);
  //! Sets a TransferReader to sort entities : it brings the Actor, <br>
//!           which may change, while the TransferReader does not <br>
  Standard_EXPORT     void SetReader(const Handle(XSControl_TransferReader)& TR) ;
  //! Sets a precise actor to sort entities <br>
//!           This definition oversedes the creation with a TransferReader <br>
  Standard_EXPORT     void SetActor(const Handle(Transfer_ActorOfTransientProcess)& act) ;
  //! Returns the Actor used as precised one. <br>
//!           Returns a Null Handle for a creation from a TransferReader <br>
//!           without any further setting <br>
  Standard_EXPORT     Handle_Transfer_ActorOfTransientProcess Actor() const;
  //! Returns the Reader (if created with a Reader) <br>
//!           Returns a Null Handle if not created with a Reader <br>
  Standard_EXPORT     Handle_XSControl_TransferReader Reader() const;
  //! Returns True for an Entity which is recognized by the Actor, <br>
//!           either the precised one, or the one defined by TransferReader <br>
  Standard_EXPORT     Standard_Boolean Sort(const Standard_Integer rank,const Handle(Standard_Transient)& ent,const Handle(Interface_InterfaceModel)& model) const;
  //! Returns a text defining the criterium : "Recognized for Transfer [(current actor)]" <br>
  Standard_EXPORT     TCollection_AsciiString ExtractLabel() const;




  DEFINE_STANDARD_RTTI(XSControl_SelectForTransfer)

protected:




private: 


Handle_XSControl_TransferReader theTR;
Handle_Transfer_ActorOfTransientProcess theAC;


};





// other Inline functions and methods (like "C++: function call" methods)


#endif

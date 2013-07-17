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

#ifndef _XSDRAW_HeaderFile
#define _XSDRAW_HeaderFile

#include <Standard.hxx>
#include <Standard_DefineAlloc.hxx>
#include <Standard_Macro.hxx>
#include <Standard_CString.hxx>
#include <Standard_Boolean.hxx>
#include <Standard_Integer.hxx>
#include <Handle_IFSelect_SessionPilot.hxx>
#include <XSControl_WorkSession.hxx>
#include <XSControl_Controller.hxx>
#include <Handle_Interface_Protocol.hxx>
#include <Handle_Interface_InterfaceModel.hxx>
#include <Handle_Standard_Transient.hxx>
#include <Transfer_TransientProcess.hxx>
#include <Transfer_FinderProcess.hxx>
#include <XSControl_TransferReader.hxx>
#include <Handle_TColStd_HSequenceOfTransient.hxx>
#include <Handle_TopTools_HSequenceOfShape.hxx>
class Draw_Interpretor;
class IFSelect_SessionPilot;
class XSControl_WorkSession;
class XSControl_Controller;
class Interface_Protocol;
class Interface_InterfaceModel;
class Standard_Transient;
class Transfer_TransientProcess;
class Transfer_FinderProcess;
class XSControl_TransferReader;
class TColStd_HSequenceOfTransient;
class TCollection_AsciiString;
class TopTools_HSequenceOfShape;
class XSDRAW_Functions;
class XSDRAW_Shape;
class XSDRAW_Vars;


//! Basic package to work functions of X-STEP (IFSelect & Co) <br>
//!           under control of DRAW <br>
//! <br>
//!           Works with some "static" data : a SessionPilot (used to run) <br>
//!           with its WorkSession and Model and TransferReader, a <br>
//!           FinderProcess <br>
class XSDRAW  {
public:

  DEFINE_STANDARD_ALLOC

  //! Takes variables to/from the DRAW session <br>//! Implements ProgressIndicator for DRAW <br>//! Changes the name under which a command of xstep is known by <br>
  //!           Draw. This allows to avoid collisions <br>
  //!           To be called before LoadDraw or any other xstep initialisation <br>
  Standard_EXPORT   static  void ChangeCommand(const Standard_CString oldname,const Standard_CString newname) ;
  //! Removes a command from the interpretation list of Draw <br>
  //!           To be called before LoadDraw or any other xstep initialisation <br>
  Standard_EXPORT   static  void RemoveCommand(const Standard_CString oldname) ;
  //! Defines the basic context to work with a X-STEP Session : <br>
  //!           it performs the basic inits, also records the Controller <br>
  //!           If the Controller is not yet set, it must be set after <br>
  //!           (call to SetController) <br>
  //!           LoadSession is called by LoadDraw <br>
  //!           Returns True the first time, False if already called <br>
  Standard_EXPORT   static  Standard_Boolean LoadSession() ;
  //! Defines the context for using a X-STEP Session under DRAW <br>
  //!           Once the various INITs have been done, a call to LoadDraw <br>
  //!           records the defined commands for the X-STEP SessionPilot, <br>
  //!           into the DRAW interpretation list. <br>
  //!           "Set" commands are accessed under command xset <br>
  //!             SDS>xset name command ... <br>
  //!           Other commands can be accessed directly or under command xstep <br>
  //!             SDS>command ...  and  SDS>xstep command ...  are equivalent <br>
  //! <br>
  //!           Only the command xinit is accessed directly only : <br>
  //!             SDS>xinit  (from the already defined Controller) <br>
  //!             SDS>xinit iges  (first defines the Controller as for "iges") <br>
  //! <br>
  //!           It also records the function to be called by DRAW (not <br>
  //!           declared because specific). <br>
  //!           And it defines the context variables, i.e. a WorkSession, then <br>
  //!             it calls SetController with the currently defined Controller <br>
  //!           Remark : at least, the standard commands are recorded <br>
  //! <br>
  //!           See also Controller : it is part of the Context, but it must <br>
  //!           be precised separately <br>
  Standard_EXPORT   static  void LoadDraw(Draw_Interpretor& theCommands) ;
  //! Allows to execute a xstep-draw command from C++ program <br>
  //!           Fixed form :  Execute("command args..."); <br>
  //!           Form with a variable text part : add %s for the variable : <br>
  //!             Execute ("command args %s args..",var)  [var is a CString] <br>
  //!           Returns the same value as returned by call from DRAW <br>
  Standard_EXPORT   static  Standard_Integer Execute(const Standard_CString command,const Standard_CString var = "") ;
  //! Returns the SessionPilot (can be used for direct call) <br>
  Standard_EXPORT   static  Handle_IFSelect_SessionPilot Pilot() ;
  //! Returns the WorkSession defined in AddDraw (through Pilot) <br>
  //!           It is from XSControl, it brings functionnalities for Transfers <br>
  Standard_EXPORT   static  Handle_XSControl_WorkSession Session() ;
  //! Defines a Controller for the command "xinit" and applies it <br>
  //!           (i.e. calls its method Customise) <br>
  Standard_EXPORT   static  void SetController(const Handle(XSControl_Controller)& control) ;
  //! Returns the Controller, a Null Handle if not yet defined <br>
  Standard_EXPORT   static  Handle_XSControl_Controller Controller() ;
  //! Sets a norm by its name (controller recorded as <normname> ) <br>
  //!           Returns True if done, False if this norm is unknown <br>
  //!           If <profile> is given, also sets the norm to <profile> <br>
  //!             (if <profile> is undefined for <normname>, it is ignored) <br>
  Standard_EXPORT   static  Standard_Boolean SetNorm(const Standard_CString normname,const Standard_CString profile = "") ;
  //! Returns the actually defined Protocol <br>
  Standard_EXPORT   static  Handle_Interface_Protocol Protocol() ;
  //! Returns the Model of the Session (it is Session()->Model() ) <br>
  Standard_EXPORT   static  Handle_Interface_InterfaceModel Model() ;
  //! Sets a Model in session (it is Session()->SetModel(model) ) <br>
  //!           If <file> is defined, SetLoadedFile is also done <br>
  Standard_EXPORT   static  void SetModel(const Handle(Interface_InterfaceModel)& model,const Standard_CString file = "") ;
  //! Produces a new model (from the Controller), can be Null <br>
  //!           Does not set it in the session <br>
  Standard_EXPORT   static  Handle_Interface_InterfaceModel NewModel() ;
  //! Returns the entity n0 <num> of the Model of the Session <br>
  //!           (it is StartingEntity) <br>
  //!           Null Handle if <num> is not suitable <br>
  Standard_EXPORT   static  Handle_Standard_Transient Entity(const Standard_Integer num) ;
  //! Returns the number of an entity in the Model (StartingNumber) <br>
  //!           0 if <ent> unknown in the model, or null <br>
  Standard_EXPORT   static  Standard_Integer Number(const Handle(Standard_Transient)& ent) ;
  //! Sets a TransferProcess in order to analyse it (see Activator) <br>
  //!           It can be either a FinderProcess or a TransientProcess, in <br>
  //!           that case a new TransferReader is created on it <br>
  Standard_EXPORT   static  void SetTransferProcess(const Handle(Standard_Transient)& TP) ;
  //! Returns the TransferProcess : TransientProcess detained by <br>
  //!           the TransferReader <br>
  Standard_EXPORT   static  Handle_Transfer_TransientProcess TransientProcess() ;
  //! Returns the FinderProcess, detained by the TransferWriter <br>
  Standard_EXPORT   static  Handle_Transfer_FinderProcess FinderProcess() ;
  //! Initialises a TransferReader, according to mode : <br>
  //!           0 nullifies it,  1 clears it (not nullify) <br>
  //!           2 sets it with TransientProcess & Model <br>
  //!           3 idem plus roots of TransientProcess <br>
  //!           Remark : called with 0 at least at each SetModel/NewModel <br>
  Standard_EXPORT   static  void InitTransferReader(const Standard_Integer mode) ;
  //! Returns the current TransferReader, can be null <br>
  //!           It detains the TransientProcess <br>
  Standard_EXPORT   static  Handle_XSControl_TransferReader TransferReader() ;
  //! Takes the name of an entity, either as argument, or (if <name> <br>
  //!           is empty) on keybord, and returns the entity <br>
  //!           name can be a label or a number (in alphanumeric), it is <br>
  //!           searched by NumberFromLabel from WorkSession. <br>
  //!           If <name> doesn't match en entity, a Null Handle is returned <br>
  Standard_EXPORT   static  Handle_Standard_Transient GetEntity(const Standard_CString name = "") ;
  //! Same as GetEntity, but returns the number in the model of the <br>
  //!           entity. Returns 0 for null handle <br>
  Standard_EXPORT   static  Standard_Integer GetEntityNumber(const Standard_CString name = "") ;
  //! Evaluates and returns a list of entity, from : <br>
  //!           keyboard if <first> and <second> are empty, see below <br>
  //!           first if second is empty : can be a number/label of an entity <br>
  //!             or the name of a selection to be evaluated (standard) <br>
  //!           first : name of a selection, evaluated from a list defined by <br>
  //!           second <br>
  //!           In case of failure, returns a Null Handle <br>
  Standard_EXPORT   static  Handle_TColStd_HSequenceOfTransient GetList(const Standard_CString first = "",const Standard_CString second = "") ;
  //! Analyses given file name and variable name, with a default <br>
  //!           name for variables. Returns resulting file name and variable <br>
  //!           name plus status "file to read"(True) or "already read"(False) <br>
  //!           In the latter case, empty resfile means no file available <br>
  //! <br>
  //!           If <file> is null or empty or equates ".", considers Session <br>
  //!             and returned status is False <br>
  //!           Else, returns resfile = file and status is True <br>
  //!           If <var> is neither null nor empty, resvar = var <br>
  //!           Else, the root part of <resfile> is considered, if defined <br>
  //!           Else, <def> is taken <br>
  Standard_EXPORT   static  Standard_Boolean FileAndVar(const Standard_CString file,const Standard_CString var,const Standard_CString def,TCollection_AsciiString& resfile,TCollection_AsciiString& resvar) ;
  //! Analyses a name as designating Shapes from DRAW variables or <br>
  //!           XSTEP transfer (last Transfer on Reading). <name> can be : <br>
  //!           "*" : all the root shapes produced by last Transfer (Read) <br>
  //!                 i.e. considers roots of the TransientProcess <br>
  //!           a name : a name of a variable DRAW <br>
  //! <br>
  //!           Returns the count of designated Shapes. Their list is put in <br>
  //!           <list>. If <list> is null, it is firstly created. Then it is <br>
  //!           completed (Append without Clear) by the Shapes found <br>
  //!           Returns 0 if no Shape could be found <br>
  Standard_EXPORT   static  Standard_Integer MoreShapes(Handle(TopTools_HSequenceOfShape)& list,const Standard_CString name) ;

private:

friend class XSDRAW_Functions;
friend class XSDRAW_Shape;
friend class XSDRAW_Vars;

};





// other Inline functions and methods (like "C++: function call" methods)


#endif

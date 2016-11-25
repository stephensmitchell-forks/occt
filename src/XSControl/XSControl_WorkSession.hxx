// Created on: 1995-06-01
// Created by: Christian CAILLET
// Copyright (c) 1995-1999 Matra Datavision
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

#ifndef _XSControl_WorkSession_HeaderFile
#define _XSControl_WorkSession_HeaderFile

#include <MMgt_TShared.hxx>

#include <TCollection_AsciiString.hxx>
#include <Interface_ReturnStatus.hxx>
#include <Transfer_FinderProcess.hxx>
#include <Transfer_TransientProcess.hxx>
class Interface_InterfaceModel;
class Interface_Graph;
class Interface_HGraph;
class Interface_GTool;
class Interface_CheckIterator;
class XSControl_Controller;
class Message_Messenger;
class TopoDS_Shape;


class XSControl_WorkSession;
DEFINE_STANDARD_HANDLE(XSControl_WorkSession, MMgt_TShared)

//! This WorkSession completes the basic one, by adding :
//! - use of Controller, with norm selection...
//! - management of transfers (both ways) with auxiliary classes
//! ReaderProcess and WriterProcess
//! -> these transfers may work with a Context List : its items
//! are given by the user, according to the transfer to be
//! i.e. it is interpreted by the Actors
//! Each item is accessed by a Name
class XSControl_WorkSession : public MMgt_TShared
{
 public:

  Standard_EXPORT XSControl_WorkSession ();
  
  //! Sets a Model as input : this will be the Model from which the
  //! ShareOut will work
  //! All SelectPointed items are cleared
  //! Remark : SetModel clears the Graph, recomputes it if a
  //! Protocol is set and if the Model is not empty, of course
  Standard_EXPORT void SetModel (const Handle(Interface_InterfaceModel)& model);
  
  //! Returns the Model of the Work Session (Null Handle if none)
  //! should be C++ : return const &
  const Handle(Interface_InterfaceModel) & Model () const { return myModel; }
  
  //! Stores the filename used for read for setting the model
  //! It is cleared by SetModel and ClearData(1)
  void SetLoadedFile (const Standard_CString theFileName) { theloaded = theFileName; }
  
  //! Returns the filename used to load current model, empty if unknown
  Standard_CString LoadedFile() const { return theloaded.ToCString(); }
  
  //! Set value of mode responsible for precence of selections after loading
  //! If mode set to true that different selections will be accessible after loading
  //! else selections will be not accessible after loading( for economy memory in applicatios)
  void SetModeStat (const Standard_Boolean theMode) { themodelstat = theMode; }
  
  //! Return value of mode defining of filling selection during loading
  Standard_Boolean GetModeStat() const { return themodelstat; }
  
  //! Computes the Graph used for Selections, Displays ...
  //! If a HGraph is already set, with same model as given by method
  //! Model, does nothing. Else, computes a new Graph.
  //! If <enforce> is given True, computes a new Graph anyway.
  //! Remark that a call to ClearGraph will cause ComputeGraph to
  //! really compute a new Graph
  //! Returns True if Graph is OK, False else (i.e. if no Protocol
  //! is set, or if Model is absent or empty).
  Standard_EXPORT Standard_Boolean ComputeGraph (const Standard_Boolean enforce = Standard_False);
  
  //! Returns the Computed Graph as HGraph (Null Handle if not set)
  Standard_EXPORT Handle(Interface_HGraph) HGraph();
  
  //! Returns the Computed Graph, for Read only
  Standard_EXPORT const Interface_Graph& Graph();
  
  //! Returns True if a Model is defined and really loaded (not
  //! empty), a Protocol is set and a Graph has been computed.
  //! In this case, the WorkSession can start to work
  Standard_EXPORT Standard_Boolean IsLoaded() const;
  
  //! Computes the CheckList for the Model currently loaded
  //! It can then be used for displays, querries ...
  //! Returns True if OK, False else (i.e. no Protocol set, or Model absent).
  //! Works only if not already done or if a new Model has been loaded from last call.
  //! Remark : computation is enforced by every call to SetModel
  Standard_EXPORT Standard_Boolean ComputeCheck ();

  //! Clears recorded data (not the items) according mode :
  //! 1 : all Data : Model, Graph, CheckList, + ClearData 4
  //! 2 : Graph and CheckList (they will then be recomputed later)
  //! 3 : CheckList (it will be recomputed by ComputeCheck)
  //! 4 : just content of SelectPointed and Counters
  //! Plus 0 : does nothing but called by SetModel
  //! ClearData is virtual, hence it can be redefined to clear
  //! other data of a specialised Work Session
  //! Clears Transfer and Management
  //! for interactive use, for mode = 0,1,2 and over 4
  //! Plus : mode = 5 to clear Transfers (both ways) only
  //! mode = 6 to clear enforced results
  Standard_EXPORT virtual void ClearData (const Standard_Integer theMode);
  
  //! Selects a Norm defined by its name.
  //! A Norm is described and handled by a Controller
  //! Returns True if done, False if <normname> is unknown
  //!
  //! The current Profile for this Norm is taken.
  Standard_EXPORT Standard_Boolean SelectNorm (const Standard_CString theNormName);
  
  //! Selects a Norm defined by its Controller itself
  Standard_EXPORT virtual void SetController (const Handle(XSControl_Controller)& theCtl); //szv_c1: made virtual
  
  //! Returns the norm controller itself
  const Handle(XSControl_Controller) & NormAdaptor () const { return myController; }
  
  //! Reads a file with the Controller (sets Model and LoadedFile)
  //! Returns a integer status which can be :
  //! RetDone if OK,  RetVoid if no Protocol not defined,
  //! RetError for file not found, RetFail if fail during read
  Standard_EXPORT Interface_ReturnStatus ReadFile (const Standard_CString theFileName);

  //! Writes the current Interface Model globally to a File, and
  //! The Model and File Modifiers recorded to be applied on sending files.
  //! Returns a status of execution :
  //! Done if OK,
  //! Void if no data available,
  //! Error if errors occured (controller is not defined, errors during translation)
  //! Fail if exception during translation is raised
  //! Stop if no disk space or disk, file is write protected
  //! Fills LastRunCheckList
  Standard_EXPORT Interface_ReturnStatus WriteFile (const Standard_CString theFileName);
  
  //! Sets a Transfer Reader, by internal ways, according mode :
  //! 0 recreates it clear
  //! 3 aligns final Results from Roots of TransientProcess
  //! 4 begins a new transfer (by BeginTransfer)
  Standard_EXPORT void InitTransferReader (const Standard_Integer theMode);

  //! Changes the ReaderProcess, i.e. considers that the new one
  //! defines the relevant read results (forgets the former ones)
  //! Returns True when done, False in case of bad definition, i.e.
  //! if Model from TP differs from that of Session
  Standard_EXPORT Standard_Boolean SetReaderProcess (const Handle(Transfer_TransientProcess)& theTP);
  
  //! Returns the Reader Process, Null if not set
  const Handle(Transfer_TransientProcess) & ReaderProcess () const { return myReaderProcess; }
  
  //! Returns the Writer Process, Null if not set
  const Handle(Transfer_FinderProcess) & WriterProcess () const { return myWriterProcess; }

  //! Changes the Map Writer, i.e. considers that the new one
  //! defines the relevant write results (forgets the former ones)
  void SetWriterProcess (const Handle(Transfer_FinderProcess)& theFP) { if (!theFP.IsNull()) myWriterProcess = theFP; }
  
  //! produces and returns a new Model well conditionned
  //! It is produced by the Norm Controller
  //! It can be Null (if this function is not implemented)
  Standard_EXPORT Handle(Interface_InterfaceModel) NewModel ();
  
  DEFINE_STANDARD_RTTIEXT(XSControl_WorkSession,MMgt_TShared)

 protected:

  Handle(XSControl_Controller) myController; // thelibrary;

  Handle(Interface_InterfaceModel) myModel;

  Handle(Interface_HGraph) thegraph;

  Handle(Interface_GTool) thegtool;
  Standard_Boolean thecheckdone;
  Standard_Boolean themodelstat;
  TCollection_AsciiString theloaded;

  Handle(Transfer_TransientProcess) myReaderProcess;
  Handle(Transfer_FinderProcess) myWriterProcess;
};

#endif // _XSControl_WorkSession_HeaderFile

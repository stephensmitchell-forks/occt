// Created on: 1993-07-27
// Created by: Christian CAILLET
// Copyright (c) 1993-1999 Matra Datavision
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

#ifndef _XSDRAW_SessionPilot_HeaderFile
#define _XSDRAW_SessionPilot_HeaderFile

#include <Standard.hxx>
#include <Standard_Type.hxx>

#include <TCollection_AsciiString.hxx>
#include <TColStd_Array1OfAsciiString.hxx>
#include <TColStd_Array1OfInteger.hxx>
#include <TColStd_SequenceOfAsciiString.hxx>
#include <Interface_ReturnStatus.hxx>
#include <IFSelect_PrintCount.hxx>
#include <XSDRAW_Activator.hxx>
class IFSelect_SignCounter;
class IFSelect_WorkSession;


class XSDRAW_SessionPilot;
DEFINE_STANDARD_HANDLE(XSDRAW_SessionPilot, XSDRAW_Activator)

//! A SessionPilot is intended to make easier the use of a
//! WorkSession. It receives commands, under alphanumeric form,
//! then calls a library of Activators to interprete and run them.
//!
//! Then, WorkSession just records data required to work :
//! Rules for Selection, Dispatch ... ; File Data (InterfaceModel
//! and results of Evaluations and Transfer as required).
//! SessionPilot records and works with alphanumeric commands and
//! their results (under a very simple form). It calls a list of
//! Activators to perform the actions.
//!
//! A Command can have several forms :
//! - classic execution, to list, evaluate, or enrich the session
//! - command which creates a new item (a Selection for instance)
//! such a command should not add it to the session, but make it
//! recorded by the Pilot (method RecordItem). The Pilot will
//! add the item in the session, with no name
//! -> such a command may be called :
//! - directly, it will add an item with no name
//! - by command xset, in the following form :
//! xset name command ...  calls the command and adds the item
//! to the session under the specified name (if not yet known)
//!
//! Thus, to a specific Norm or way of working, only Activators
//! change. A specific Initialisation can be done by starting
//! with a specific set of commands.
//!
//! In addition, SessionPilot is a sub-type of Activator, to
//! recognize some built-in commands : exit/x, help/?, control of
//! command line, and commands xstep xset ... See method Do
//!
//! At least, empty lines and comment lines (beginning by '#')
//! are skipped (comment lines are display if read from file)
class XSDRAW_SessionPilot : public XSDRAW_Activator
{
 public:
  
  //! Returns a unique static pilot
  Standard_EXPORT static const Handle(XSDRAW_SessionPilot) & Pilot();
  
  //! Returns the WorkSession which is worked on
  const Handle(IFSelect_WorkSession) & Session() const { return thesession; }
  
  //! Returns the Record Mode for Commands. Default is False.
  Standard_Boolean RecordMode() const { return therecord; }
  
  //! Sets a WorkSession to be worked on
  void SetSession (const Handle(IFSelect_WorkSession)& WS) { thesession = WS; }
  
  //! Changes the RecordMode.
  void SetRecordMode (const Standard_Boolean mode) { therecord = mode; }
  
  //! Sets the value of the Command Line to be interpreted
  //! Also prepares the interpretation (splitting by blanks)
  Standard_EXPORT void SetCommandLine (const TCollection_AsciiString& command);
  
  //! Returns the Command Line to be interpreted
  const TCollection_AsciiString& CommandLine() const { return thecommand; }
  
  //! Returns the part of the command line which begins at argument
  //! <numarg> between 0 and NbWords-1 (by default, all the line)
  //! Empty string if out of range
  Standard_EXPORT Standard_CString CommandPart (const Standard_Integer numarg = 0) const;
  
  //! Returns the count of words of the Command Line, separated by
  //! blanks : 0 if empty, one if a command without args, else it
  //! gives the count of args minus one.
  //! Warning : limited to 10 (command title + 9 args)
  Standard_Integer NbWords() const { return thenbwords; }
  
  //! Returns a word given its rank in the Command Line. Begins at 0
  //! which is the Command Title, 1 is the 1st arg., etc...
  Standard_EXPORT const TCollection_AsciiString& Word (const Standard_Integer num) const;
  
  //! Returns a word given its rank, as a CString.
  //! As for Word, begins at 0 (the command name), etc...
  Standard_EXPORT Standard_CString Arg (const Standard_Integer num) const;
  
  //! Removes a word given its rank. Returns True if Done, False if
  //! <num> is out of range
  Standard_EXPORT Standard_Boolean RemoveWord (const Standard_Integer num);
  
  //! Returns the count of recorded Commands
  Standard_Integer NbCommands() const { return thecomlist.Length(); }

  //! Returns a recorded Command, given its rank (from 1)
  const TCollection_AsciiString& Command (const Standard_Integer num) const { return thecomlist(num); }
  
  //! Allows to associate a Transient Value with the last execution
  //! as a partial result
  //! Returns RetDone if item is not Null, RetFail if item is Null
  //! Remark : it is nullified for each Perform
  Standard_EXPORT Interface_ReturnStatus RecordItem (const Handle(Standard_Transient)& item);
  
  //! Returns the Transient Object which was recorded with the
  //! current Line Command. If none was, returns a Null Handle
  const Handle(Standard_Transient) & RecordedItem() const { return theobjrec; }

  //! Clears the recorded informations (commands, objects)
  void Clear() { thecomlist.Clear(); }
  
  //! Reads commands from a Script File, named <file>. By default
  //! (file = ""), reads from standard input with a prompt
  //! Else (reading from a file), the read commands are displayed
  //! onto standard output. Allows nested reads. Reading is stopped
  //! either by command x or exit, or by reaching end of file
  //! Return Value follows the rules of Do : RetEnd for normal end,
  //! RetFail if script could not be opened
  Standard_EXPORT Interface_ReturnStatus ReadScript (const Standard_CString file = "");
  
  //! Executes the Command, itself (for built-in commands, which
  //! have priority) or by using the list of Activators.
  //! The value returned is : RetVoid if nothing done (void command)
  //! RetDone if execution OK, RetEnd if END OF SESSION, RetError if
  //! command unknown or incorrect, RetFail if error on execution
  //! If execution is OK and RecordMode is set, this Command Line is
  //! recorded to the list (see below).
  Standard_EXPORT Interface_ReturnStatus Perform();
  
  //! Sets the Command then tries to execute it. Return value :
  //! same as for Perform
  Standard_EXPORT Interface_ReturnStatus Execute (const TCollection_AsciiString& command);
  
  //! Executes a Counter in a general way
  //! If <numword> is greater than count of command words, it counts
  //! all the model. Else it considers the word <numword> as the
  //! identifier of a Selection
  //! <mode> gives the mode of printing results, default is
  //! CountByItem
  Standard_EXPORT Interface_ReturnStatus ExecuteCounter (const Handle(IFSelect_SignCounter)& counter, const Standard_Integer numword, const IFSelect_PrintCount mode = IFSelect_CountByItem);
  
  //! Interprets a string value as an entity number :
  //! if it gives an integer, returns its value
  //! else, considers it as ENtityLabel (preferably case sensitive)
  //! in case of failure, returns 0
  Standard_EXPORT Standard_Integer Number (const Standard_CString val) const;
  
  //! Processes specific commands, which are :
  //! x or exit for end of session
  //! ? or help for help messages
  //! xcommand to control command lines (Record Mode, List, Clear,
  //! File Output ...)
  //! xsource to execute a command file (no nesting allowed),
  //! in case of error, source is stopped and keyword recovers
  //! xstep is a simple prefix (useful in a wider environment, to
  //! avoid conflicts on command names)
  //! xset control commands which create items with names
  Standard_EXPORT Interface_ReturnStatus Do (const Standard_Integer number, const Handle(XSDRAW_SessionPilot)& session) Standard_OVERRIDE;
  
  //! Help for specific commands (apart from general command help)
  Standard_EXPORT Standard_CString Help (const Standard_Integer number) const Standard_OVERRIDE;

  DEFINE_STANDARD_RTTIEXT(XSDRAW_SessionPilot,XSDRAW_Activator)

 private:
  
  //! Creates an empty SessionPilot, with a prompt which will be displayed on querying commands.
  Standard_EXPORT XSDRAW_SessionPilot();

  Handle(IFSelect_WorkSession) thesession;
  TCollection_AsciiString theprompt;
  TCollection_AsciiString thecommand;
  Standard_Integer thenbwords;
  TColStd_Array1OfAsciiString thewords;
  TColStd_Array1OfInteger thewordeb;
  Standard_Boolean therecord;
  Standard_Integer thenumrec;
  Handle(Standard_Transient) theobjrec;
  TColStd_SequenceOfAsciiString thecomlist;
};

#endif // _XSDRAW_SessionPilot_HeaderFile

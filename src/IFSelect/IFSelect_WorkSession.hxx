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

#ifndef _IFSelect_WorkSession_HeaderFile
#define _IFSelect_WorkSession_HeaderFile

#include <XSControl_WorkSession.hxx>

#include <TColStd_IndexedDataMapOfTransientTransient.hxx>
#include <IFSelect_PrintCount.hxx>
class Dico_DictionaryOfTransient;
class Message_Messenger;
class IFSelect_IntParam;
class IFSelect_Selection;
class IFSelect_Signature;
class IFSelect_SignCounter;
class IFSelect_Vars;

//! This WorkSession is used in Draw
class IFSelect_WorkSession : public XSControl_WorkSession
{
 public:

  //! Default constructor
  Standard_EXPORT IFSelect_WorkSession ();

  //! Selects a Norm defined by its Controller itself
  Standard_EXPORT virtual void SetController (const Handle(XSControl_Controller)& theCtl) Standard_OVERRIDE; //szv_c1: made virtual
  
  //! Clears recorded data (not the items) according mode :
  //! 1 : all Data : Model, Graph, CheckList, + ClearData 4
  //! 2 : Graph and CheckList (they will then be recomputed later)
  //! 3 : CheckList (it will be recomputed by ComputeCheck)
  //! 4 : just content of SelectPointed and Counters
  //! Plus 0 : does nothing but called by SetModel
  //! ClearData is virtual, hence it can be redefined to clear
  //! other data of a specialised Work Session
  Standard_EXPORT virtual void ClearData (const Standard_Integer mode) Standard_OVERRIDE;
  
  //! Sets a specific Signature to be the SignType, i.e. the
  //! Signature which will determine TypeName from the Model
  //! (basic function). It is recorded in the GTool
  //! This Signature is also set as "xst-sign-type" (reserved name)
  Standard_EXPORT void SetSignType (const Handle(IFSelect_Signature)& signtype);
  
  //! Returns the current SignType
  Standard_EXPORT Handle(IFSelect_Signature) SignType() const;
  
  //! Returns the count of Entities stored in the Model, or 0
  Standard_EXPORT Standard_Integer NbStartingEntities() const;
  
  //! Returns an  Entity stored in the Model of the WorkSession
  //! (Null Handle is no Model or num out of range)
  Standard_EXPORT Handle(Standard_Transient) StartingEntity (const Standard_Integer num) const;
  
  //! Returns the Number of an Entity in the Model
  //! (0 if no Model set or <ent> not in the Model)
  Standard_EXPORT Standard_Integer StartingNumber (const Handle(Standard_Transient)& ent) const;
  
  //! From a given label in Model, returns the corresponding number
  //! Starts from first entity by Default, may start after a given
  //! number : this number may be given negative, its absolute value
  //! is then considered. Hence a loop on NumberFromLabel may be
  //! programmed (stop test is : returned value positive or null)
  //!
  //! Returns 0 if not found, < 0 if more than one found (first
  //! found in negative).
  //! If <val> just gives an integer value, returns it
  Standard_EXPORT Standard_Integer NumberFromLabel (const Standard_CString val, const Standard_Integer afternum = 0) const;
  
  //! Returns the Name of an Entity
  //! This Name is computed by the general service Name
  //! Returns a Null Handle if fails
  Standard_EXPORT Handle(TCollection_HAsciiString) EntityName (const Handle(Standard_Transient)& ent) const;
  
  //! Returns the Category Number determined for an entity
  //! it is computed by the class Category
  //! An unknown entity (number 0) gives a value -1
  Standard_EXPORT Standard_Integer CategoryNumber (const Handle(Standard_Transient)& ent) const;
  
  //! Returns the list of entities shared by <ent> (can be empty)
  //! Returns a null Handle if <ent> is unknown
  Standard_EXPORT Handle(TColStd_HSequenceOfTransient) Shareds (const Handle(Standard_Transient)& ent);
  
  //! Returns the list of entities sharing <ent> (can be empty)
  //! Returns a null Handle if <ent> is unknown
  Standard_EXPORT Handle(TColStd_HSequenceOfTransient) Sharings (const Handle(Standard_Transient)& ent);
  
  //! Returns a Check for a single entity, under the form of a
  //! CheckIterator (this gives only one form for the user)
  //! if <ent> is Null or equates the current Model, it gives the
  //! Global Check, else the Check for the given entity
  //! <complete> as for ModelCheckList
  Standard_EXPORT Interface_CheckIterator CheckOne (const Handle(Standard_Transient)& ent, const Standard_Boolean complete = Standard_True);
  
  //! Returns True if an Item of the WorkSession has an attached Name
  Standard_EXPORT Standard_Boolean HasName (const Handle(Standard_Transient)& item) const;
  
  //! Returns the Name attached to an Item as a Variable of this
  //! WorkSession. If <item> is Null or not recorded, returns an
  //! empty string.
  Standard_EXPORT Handle(TCollection_HAsciiString) Name (const Handle(Standard_Transient)& item) const;
  
  //! Adds an Item and returns its attached Ident. Does nothing
  //! if <item> is already recorded (and returns its attached Ident)
  //! Remark : the determined Ident is used if <item> is a Dispatch,
  //! to fill the ShareOut
  Standard_EXPORT Standard_Integer AddItem (const Handle(Standard_Transient)& item);
  
  //! Adds an Item with an attached Name. If the Name is already
  //! known in the WorkSession, the older item losts it
  //! Returns Ident if Done, 0 else, i.e. if <item> is null
  //! If <name> is empty, works as AddItem (i.e. with no name)
  //! If <item> is already known but with no attached Name, this
  //! method tries to attached a Name to it
  Standard_EXPORT Standard_Integer AddNamedItem (const Standard_CString name, const Handle(Standard_Transient)& item);
  
  //! Removes an Item given its Ident. Returns False if <id> is
  //! attached to no Item in the WorkSession. For a Named Item,
  //! also removes its Name.
  Standard_EXPORT Standard_Boolean RemoveItem (const Handle(Standard_Transient)& item);

  //! For query by Label with possible iterations
  //! Searches the Ident of which Item has a Label which matches a
  //! given one, the search starts from an initial Ident.
  //! Returns the first found Ident which follows <id>, or ZERO
  //!
  //! The search must start with <id> = 0, it returns the next Ident
  //! which matches. To iterate, call again this method which this
  //! returned value as <id>. Once an Ident has been returned, the
  //! Item can be obtained by the method Item
  //!
  //! <mode> precises the required matching mode :
  //! - 0 (Default) : <label> must match exactly with the Item Label
  //! - 1 : <label> must match the exact beginning (the end is free)
  //! - 2 : <label> must be at least once wherever in the Item Label
  //! - other values are ignored
  Standard_EXPORT Standard_Integer NextIdentForLabel (const Standard_CString label, const Standard_Integer id, const Standard_Integer mode = 0) const;
  
  //! Returns the Maximum Value for an Item Identifier. It can be
  //! greater to the count of known Items, because some can have
  //! been removed
  Standard_EXPORT Standard_Integer MaxIdent() const;
  
  //! Returns an Item, given its Ident. Returns a Null Handle if
  //! no Item corresponds to this Ident.
  Standard_EXPORT Handle(Standard_Transient) Item (const Standard_Integer id) const;
  
  //! Returns the Ident attached to an Item in the WorkSession, or
  //! Zero if it is unknown
  Standard_EXPORT Standard_Integer ItemIdent (const Handle(Standard_Transient)& item) const;
  
  //! Returns the Item which corresponds to a Variable, given its
  //! Name (whatever the type of this Item).
  //! Returns a Null Handle if this Name is not recorded
  Standard_EXPORT Handle(Standard_Transient) NamedItem (const Standard_CString name) const;
  
  //! Same as above, but <name> is given through a Handle
  //! Especially Usefull with methods SelectionNames, etc...
  Standard_EXPORT Handle(Standard_Transient) NamedItem (const Handle(TCollection_HAsciiString)& name) const;
  
  //! Returns the Ident attached to a Name, 0 if name not recorded
  Standard_EXPORT Standard_Integer NameIdent (const Standard_CString name) const;
  
  //! Clears all the recorded Items : Selections, Dispatches,
  //! Modifiers, and Strings & IntParams, with their Idents & Names.
  //! Remark that if a Model has been loaded, it is not cleared.
  Standard_EXPORT void ClearItems ();
  
  //! Returns a Label which illustrates the content of an Item,
  //! given its Ident. This Label is :
  //! - for a Text Parameter, "Text:<text value>"
  //! - for an Integer Parameter, "Integer:<integer value>"
  //! - for a Selection, a Dispatch or a Modifier, its Label
  //! (see these classes)
  //! - for any other kind of Variable, its cdl type
  Standard_EXPORT Handle(TCollection_HAsciiString) ItemLabel (const Standard_Integer id) const;
  
  //! Fills a Sequence with the List of Idents attached to the Items
  //! of which Type complies with (IsKind) <type> (alphabetic order)
  //! Remark : <type> = TYPE(Standard_Transient) gives all the
  //! Idents which are suitable in the WorkSession
  Standard_EXPORT Handle(TColStd_HSequenceOfInteger) ItemIdents (const Handle(Standard_Type)& type) const;
  
  //! Returns an IntParam, given its Ident in the Session
  //! Null result if <id> is not suitable for an IntParam
  //! (undefined, or defined for another kind of variable)
  Standard_EXPORT Handle(IFSelect_IntParam) IntParam (const Standard_Integer id) const;
  
  //! Returns Integer Value of an IntParam
  Standard_EXPORT Standard_Integer IntValue (const Handle(IFSelect_IntParam)& it) const;
  
  //! Changes the Integer Value of an IntParam
  //! Returns True if Done, False if <it> is not in the WorkSession
  Standard_EXPORT Standard_Boolean SetIntValue (const Handle(IFSelect_IntParam)& it, const Standard_Integer val);
  
  //! Returns a TextParam, given its Ident in the Session
  //! Null result if <id> is not suitable for a TextParam
  //! (undefined, or defined for another kind of variable)
  Standard_EXPORT Handle(TCollection_HAsciiString) TextParam (const Standard_Integer id) const;
  
  //! Returns Text Value of a TextParam (a String)
  //! or an empty string if <it> is not in the WorkSession
  Standard_EXPORT TCollection_AsciiString TextValue (const Handle(TCollection_HAsciiString)& par) const;
  
  //! Changes the Text Value of a TextParam (an HAsciiString)
  //! Returns True if Done, False if <it> is not in the WorkSession
  Standard_EXPORT Standard_Boolean SetTextValue (const Handle(TCollection_HAsciiString)& par, const Standard_CString val);
  
  //! Returns a Signature, given its Ident in the Session
  //! Null result if <id> is not suitable for a Signature
  //! (undefined, or defined for another kind of variable)
  Standard_EXPORT Handle(IFSelect_Signature) Signature (const Standard_Integer id) const;
  
  //! Returns the Value computed by a Signature for an Entity
  //! Returns an empty string if the entity does not belong to the
  //! loaded model
  Standard_EXPORT Standard_CString SignValue (const Handle(IFSelect_Signature)& sign, const Handle(Standard_Transient)& ent) const;
  
  //! Returns a Selection, given its Ident in the Session
  //! Null result if <id> is not suitable for a Selection
  //! (undefined, or defined for another kind of variable)
  Standard_EXPORT Handle(IFSelect_Selection) Selection (const Standard_Integer id) const;
  
  //! Evaluates the effect of a Selection applied on the input Model
  //! Returned Result remains empty if no input Model has been set
  Standard_EXPORT Interface_EntityIterator EvalSelection (const Handle(IFSelect_Selection)& sel) const;
  
  //! Returns the result of a Selection, computed by EvalSelection
  //! (see above) under the form of a HSequence (hence, it can be
  //! used by a frontal-engine logic). It can be empty
  //! Returns a Null Handle if <sel> is not in the WorkSession
  Standard_EXPORT Handle(TColStd_HSequenceOfTransient) SelectionResult (const Handle(IFSelect_Selection)& sel) const;
  
  //! Returns the result of a Selection, by forcing its input with
  //! a given list <list> (unless <list> is Null).
  //! RULES :
  //! <list> applies only for a SelectDeduct kind Selection :
  //! its Input is considered : if it is a SelectDeduct kind
  //! Selection, its Input is considered, etc... until an Input
  //! is not a Deduct/Extract : its result is replaced by <list>
  //! and all the chain of deductions is applied
  Standard_EXPORT Handle(TColStd_HSequenceOfTransient) SelectionResultFromList (const Handle(IFSelect_Selection)& sel, const Handle(TColStd_HSequenceOfTransient)& list) const;
  
  //! Returns a SignCounter from its ident in the Session
  //! Null result if <id> is not suitable for a SignCounter
  //! (undefined, or defined for another kind of variable)
  Standard_EXPORT Handle(IFSelect_SignCounter) SignCounter (const Standard_Integer id) const;
  
  //! Returns the list of Entities sent in files, accourding the
  //! count of files each one has been sent (these counts are reset
  //! by SetModel ) stored in Graph Status
  //! <count> = -1 (default) is for ENtities sent at least once
  //! <count> = 0 is for the Remaining List (entities not yet sent)
  //! <count> = 1 is for entities sent in one and only one file
  //! (the ideal case)
  //! Remaining Data are computed on each Sending/Copying output
  //! files (see methods EvaluateFile and SendSplit)
  //! Graph Status is 0 for Remaining Entity, <count> for Sent into
  //! <count> files
  //! This status is set to 0 (not yet sent) for all by SetModel
  Standard_EXPORT Interface_EntityIterator SentList (const Standard_Integer count = -1) const;
  
  //! Returns True if <sel> a Reversed SelectExtract, False else
  Standard_EXPORT Standard_Boolean IsReversedSelectExtract (const Handle(IFSelect_Selection)& sel) const;
  
  //! Toggles the Sense (Direct <-> Reversed) of a SelectExtract
  //! Returns True if Done, False if <sel> is not a SelectExtract or
  //! is not in the WorkSession
  Standard_EXPORT Standard_Boolean ToggleSelectExtract (const Handle(IFSelect_Selection)& sel);
  
  //! Sets an Input Selection (as <input>) to a SelectExtract or
  //! a SelectDeduct (as <sel>).
  //! Returns True if Done, False if <sel> is neither a
  //! SelectExtract nor a SelectDeduct, or not in the WorkSession
  Standard_EXPORT Standard_Boolean SetInputSelection (const Handle(IFSelect_Selection)& sel, const Handle(IFSelect_Selection)& input);
  
  //! Sets an Input Selection, Main if <formain> is True, Second else
  //! (as <sc>) to a SelectControl (as <sel>). Returns True if Done,
  //! False if <sel> is not a SelectControl, or <sc> or <sel> is not
  //! in the WorkSession
  Standard_EXPORT Standard_Boolean SetControl (const Handle(IFSelect_Selection)& sel, const Handle(IFSelect_Selection)& sc, const Standard_Boolean formain = Standard_True);
  
  //! Adds an input selection to a SelectCombine (Union or Inters.).
  //! Returns new count of inputs for this SelectCombine if Done or
  //! 0 if <sel> is not kind of SelectCombine, or if <seladd> or
  //! <sel> is not in the WorkSession
  //! By default, adding is done at the end of the list
  //! Else, it is an insertion to rank <atnum> (usefull for Un-ReDo)
  Standard_EXPORT Standard_Integer CombineAdd (const Handle(IFSelect_Selection)& selcomb, const Handle(IFSelect_Selection)& seladd, const Standard_Integer atnum = 0);
  
  //! Removes an input selection from a SelectCombine (Union or
  //! Intersection). Returns True if done, False if <selcomb> is not
  //! kind of SelectCombine or <selrem> is not source of <selcomb>
  Standard_EXPORT Standard_Boolean CombineRemove (const Handle(IFSelect_Selection)& selcomb, const Handle(IFSelect_Selection)& selrem);
  
  //! Returns a Selection from a Name :
  //! - the name of a Selection : this Selection
  //! - the name of a Signature + criteria between (..) : a new
  //! Selection from this Signature
  //! - an entity or a list of entities : a new SelectPointed
  //! Else, returns a Null Handle
  Standard_EXPORT Handle(IFSelect_Selection) GiveSelection (const Standard_CString selname) const;
  
  //! Determines a list of entities from an object :
  //! <obj> already HSequenceOfTransient : returned itself
  //! <obj> Selection : its Result of Evaluation is returned
  //! <obj> an entity of the Model : a HSequence which contains it
  //! else, an empty HSequence
  //! <obj> the Model it self : ALL its content (not only the roots)
  Standard_EXPORT Handle(TColStd_HSequenceOfTransient) GiveList (const Handle(Standard_Transient)& obj) const;
  
  //! Computes a List of entities from two alphanums,
  //! first and second, as follows :
  //! if <first> is a Number or Label of an entity : this entity
  //! if <first> is a list of Numbers/Labels : the list of entities
  //! if <first> is the name of a Selection in <WS>, and <second>
  //! not defined, the standard result of this Selection
  //! else, let's consider "first second" : this whole phrase is
  //! splitted by blanks, as follows (RECURSIVE CALL) :
  //! - the leftest term is the final selection
  //! - the other terms define the result of the selection
  //! - and so on (the "leftest minus one" is a selection, of which
  //! the input is given by the remaining ...)
  Standard_EXPORT Handle(TColStd_HSequenceOfTransient) GiveList (const Standard_CString first, const Standard_CString second = "") const;
  
  //! Computes a List of entities from the model as follows
  //! <first> beeing a Selection or a combination of Selections,
  //! <ent> beeing an entity or a list
  //! of entities (as a HSequenceOfTransient) :
  //! the standard result of this selection applied to this list
  //! if <ent> is Null, the standard definition of the selection is
  //! used (which contains a default input selection)
  //! if <selname> is erroneous, a null handle is returned
  //!
  //! REMARK : selname is processed as <first second> of preceeding
  //! GiveList
  Standard_EXPORT Handle(TColStd_HSequenceOfTransient) GiveListFromList (const Standard_CString selname, const Handle(Standard_Transient)& ent) const;
  
  //! Determines if <entdad> is parent of <entson> (in the graph),
  //! returns : -1 if no; 0 if <entdad> = <entson>
  //! 1 if immediate parent, > 1 if parent, gives count of steps
  Standard_EXPORT Standard_Integer QueryParent (const Handle(Standard_Transient)& entdad, const Handle(Standard_Transient)& entson) const;
  
  //! Traces the Statics attached to a given use number
  //! If <use> is given positive (normal), the trace is embedded
  //! with a header and a trailer
  //! If <use> is negative, just values are printed
  //! (this allows to make compositions)
  //! Remark : use number  5 commands use -2 to be traced
  //! Remark : use numbers 4 and 6 command use -3 to be traced
  Standard_EXPORT void TraceStatics (const Standard_Integer use, const Standard_Integer mode = 0) const;
  
  //! Lists the Labels of all Items of the WorkSession
  //! If <label> is defined, lists labels which contain it
  Standard_EXPORT void ListItems (const Standard_CString label = "") const;
  
  //! Lists a Selection and its Sources (see SelectionIterator),
  //! given its rank in the list
  Standard_EXPORT void DumpSelection (const Handle(IFSelect_Selection)& sel) const;
  
  //! Lists the content of the Input Model (if there is one)
  //! According level : 0 -> gives only count of Entities and Roots
  //! 1 -> Lists also Roots;  2 -> Lists all Entities (by TraceType)
  //! 3 -> Performs a call to CheckList (Fails) and lists the result
  //! 4 -> as 3 but all CheckList (Fails + Warnings)
  //! 5,6,7  : as 3 but resp. Count,List,Labels by Fail
  //! 8,9,10 : as 4 but resp. Count,List,Labels by message
  Standard_EXPORT void DumpModel (const Standard_Integer level, const Handle(Message_Messenger)& S);
  
  //! Dumps the current Model (as inherited DumpModel), on currently
  //! defined Default Trace File (default is standard output)
  Standard_EXPORT void TraceDumpModel (const Standard_Integer mode);
  
  //! Prints main informations about an entity : its number, type,
  //! validity (and checks if any), category, shareds and sharings..
  //! mutable because it can recompute checks as necessary
  Standard_EXPORT void PrintEntityStatus (const Handle(Standard_Transient)& ent, const Handle(Message_Messenger)& S);
  
  //! Prints a CheckIterator to the current Trace File, controlled
  //! with the current Model
  //! complete or fails only, according to <failsonly>
  //! <mode> defines the mode of printing
  //! 0 : sequential, according entities; else with a CheckCounter
  //! 1 : according messages, count of entities
  //! 2 : id but with list of entities, designated by their numbers
  //! 3 : as 2 but with labels of entities
  Standard_EXPORT void PrintCheckList (const Interface_CheckIterator& checklist, const Standard_Boolean failsonly, const IFSelect_PrintCount mode) const;
  
  //! Internal method which displays an EntityIterator
  //! <mode> 0 gives short display (only entity numbers)
  //! 1 gives a more complete trace (1 line per Entity)
  //! (can be used each time a trace has to be output from a list)
  //! 2 gives a form suitable for givelist : (n1,n2,n3...)
  Standard_EXPORT void ListEntities (const Interface_EntityIterator& iter, const Standard_Integer mode) const;
  
  //! Commands the transfer on reading for a list of entities to
  //! data for Imagine, using the selected Actor for Read
  //! Returns count of transferred entities, ok or with fails (0/1)
  //! The results are recorded by RecordResult
  Standard_EXPORT Standard_Integer TransferList (const Handle(TColStd_HSequenceOfTransient)& theList);
  
  const Handle(IFSelect_Vars) & Vars () const { return myVars; }
  
  void SetVars (const Handle(IFSelect_Vars)& theVars) { myVars = theVars; }
  
  DEFINE_STANDARD_RTTIEXT(IFSelect_WorkSession,XSControl_WorkSession)

 private:
  
  //! Returns the Category Name determined for an entity
  //! it is computed by the class Category
  //! Remark : an unknown entity gives an empty string
  Standard_EXPORT Standard_CString CategoryName (const Handle(Standard_Transient)& ent) const;
  
  //! Returns the Validity Name determined for an entity
  //! it is computed by the class SignValidity
  //! Remark : an unknown entity gives an empty string
  Standard_EXPORT Standard_CString ValidityName (const Handle(Standard_Transient)& ent) const;
  
  //! Returns the Check List for the Model currently loaded,
  //! (syntactic & semantic messages), computed if not yet done
  Standard_EXPORT Interface_CheckIterator ModelCheckList ();

  TColStd_IndexedDataMapOfTransientTransient myItems;
  Handle(Dico_DictionaryOfTransient) myNames;
  Handle(IFSelect_Vars) myVars;
};

#endif // _IFSelect_WorkSession_HeaderFile

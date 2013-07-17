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

#ifndef _LibCtl_Library_HeaderFile
#define _LibCtl_Library_HeaderFile

#include <Standard_NoSuchObject.hxx>
#include <NCollection_Handle.hxx>

template<class ModuleType, class ProtocolType> class LibCtl_GlobalNode;


// Node for LibCtl_Library
//! Manages a list of Modules for a Library (as an instance) :
//!  Designates a GlobalNode (couple Module-Protocol)
template<class ModuleType, class ProtocolType>
class LibCtl_Node
{
private:
  typedef LibCtl_GlobalNode<ModuleType, ProtocolType> LibCtl_GlobalNode;

  NCollection_Handle < LibCtl_GlobalNode > thenode;
  NCollection_Handle < LibCtl_Node > thenext;

public:

  Standard_EXPORT   LibCtl_Node ();
  Standard_EXPORT   void AddNode (const NCollection_Handle< LibCtl_GlobalNode >& anode);
  Standard_EXPORT   const ModuleType&  Module () const;
  Standard_EXPORT   const ProtocolType& Protocol () const;
  Standard_EXPORT   const NCollection_Handle< LibCtl_Node >& Next () const;
};

//! Creates an empty Node, with no Next
template<class ModuleType, class ProtocolType>
LibCtl_Node<ModuleType,ProtocolType>
::LibCtl_Node () {  }

//! Adds a couple (Module,Protocol), that is, stores it into
//! itself if not yet done, else creates a Next Node to do it.
template<class ModuleType, class ProtocolType>
void LibCtl_Node<ModuleType,ProtocolType>::AddNode (const NCollection_Handle< LibCtl_GlobalNode >& anode)
{
  if (thenode == anode) return;
  if (thenext.IsNull()) {
    if (thenode.IsNull()) thenode = anode;
    else {
      thenext = new LibCtl_Node;
      thenext->AddNode (anode);
    }
  }
  else thenext->AddNode (anode);
}

//! Returns the Module designated by a precise Node.
template<class ModuleType, class ProtocolType>
const ModuleType& LibCtl_Node<ModuleType,ProtocolType>::Module () const
{
  return thenode->Module();
}

//! Returns the Protocol designated by a precise Node.
template<class ModuleType, class ProtocolType>
const ProtocolType& LibCtl_Node<ModuleType,ProtocolType>::Protocol () const
{
  return thenode->Protocol();
}

//! Returns the next Node.
//! If none was defined, returned value is a null handle.
template<class ModuleType, class ProtocolType>
const NCollection_Handle< LibCtl_Node<ModuleType,ProtocolType> >& LibCtl_Node<ModuleType,ProtocolType>::Next () const
{
  return thenext;
}

// GlobalNode for LibCtl_Library
//! Manages a (possibly static) Global List of Modules bound to protocols.
//! Remark that it requires independance from Memory Management
//! (because a Global List of Modules can be build through static
//! declarations, i.e. before any sequential execution)
//! Remark there will not be many many GlobalNodes created
template<class ModuleType, class ProtocolType>
class LibCtl_GlobalNode
{
private:
  ModuleType themod;
  ProtocolType theprot;
  NCollection_Handle <LibCtl_GlobalNode> thenext;
public:
  Standard_EXPORT   LibCtl_GlobalNode ();
  Standard_EXPORT   void Add (const ModuleType& amodule, const ProtocolType& aprotocol);
  Standard_EXPORT   const ModuleType& Module () const;
  Standard_EXPORT   const ProtocolType& Protocol () const;
  Standard_EXPORT   const NCollection_Handle <LibCtl_GlobalNode >& Next () const;
};

//! Creates an empty GlobalNode, with no Next
template<class ModuleType, class ProtocolType>
LibCtl_GlobalNode<ModuleType,ProtocolType>
::LibCtl_GlobalNode ()    {  }

//! Adds a Module bound with a Protocol to the list : does
//! nothing if already in the list, THAT IS, Same Type (exact
//! match) and Same State (that is, IsEqual is not required).
//! Once added, stores its attached Protocol in correspondance.
template<class ModuleType, class ProtocolType>
void LibCtl_GlobalNode <ModuleType,ProtocolType>::Add
(const ModuleType& amodule, const ProtocolType& aprotocol)
{
  if (themod == amodule) return;
  if (theprot == aprotocol) themod = amodule;
  else if (thenext.IsNull()) {
    if (themod.IsNull()) {  themod = amodule;   theprot = aprotocol;  }
    else {
      thenext = new LibCtl_GlobalNode;
      thenext->Add (amodule,aprotocol);
    }
  }
  else thenext->Add (amodule,aprotocol);
}

//! Returns the Module stored in a given GlobalNode.
template<class ModuleType, class ProtocolType>
const ModuleType& LibCtl_GlobalNode <ModuleType,ProtocolType>::Module () const
{
  return themod;
}

//! Returns the attached Protocol stored in a given GlobalNode.
template<class ModuleType, class ProtocolType>
const ProtocolType& LibCtl_GlobalNode <ModuleType,ProtocolType>::Protocol () const
{
  return theprot;
}

//! Returns the next GlobalNode.
//! If none is defined, returned value is a null handle.
template<class ModuleType, class ProtocolType>
const NCollection_Handle <LibCtl_GlobalNode<ModuleType,ProtocolType> >&
LibCtl_GlobalNode <ModuleType,ProtocolType>::Next () const
{
  return thenext;
}

// Library
//!
template<class ObjectType,class ModuleType, class ProtocolType>
class LibCtl_Library
{
public:
  typedef LibCtl_GlobalNode<ModuleType, ProtocolType > LibCtl_GlobalNode;
  typedef LibCtl_Node<ModuleType, ProtocolType > LibCtl_Node;
  static NCollection_Handle< LibCtl_GlobalNode > theglobal;
  static ProtocolType theprotocol;
  static NCollection_Handle< LibCtl_Node > thelast;

  // Public methods
  Standard_EXPORT   static void SetGlobal(const ModuleType& amodule, const ProtocolType& aprotocol);
  Standard_EXPORT   LibCtl_Library (const ProtocolType& aprotocol);
  Standard_EXPORT   LibCtl_Library ();
  Standard_EXPORT   void AddProtocol(const Handle(Standard_Transient)& aprotocol);
  Standard_EXPORT   void Clear ();
  Standard_EXPORT   void SetComplete ();
  Standard_EXPORT   Standard_Boolean Select (const ObjectType& obj,
                                             ModuleType& module,
                                             Standard_Integer& CN) const;
  Standard_EXPORT   void Start ();
  Standard_EXPORT   Standard_Boolean More () const;
  Standard_EXPORT   void Next ();
  Standard_EXPORT   const ModuleType& Module () const;
  Standard_EXPORT   const ProtocolType& Protocol () const;

protected:
    NCollection_Handle < LibCtl_Node > thelist;
    NCollection_Handle < LibCtl_Node > thecurr;
};

//! Adds a couple (Module-Protocol) into the global definition set
//! for this class of Library.
template<class ObjectType,class ModuleType, class ProtocolType>
void LibCtl_Library <ObjectType, ModuleType, ProtocolType> 
::SetGlobal (const ModuleType& amodule, const ProtocolType& aprotocol)
{
  if (theglobal.IsNull()) theglobal = new LibCtl_GlobalNode;
  theglobal->Add(amodule,aprotocol);
}

//! Creates a Library which complies with a Protocol, that is :
//! - Same class (criterium IsInstance)
//! - This creation gets the Modules from the global set, those
//!   which are bound to the given Protocol and its Resources.
template<class ObjectType,class ModuleType, class ProtocolType>
LibCtl_Library<ObjectType, ModuleType, ProtocolType>::LibCtl_Library (const ProtocolType& aprotocol)
{
  Standard_Boolean last = Standard_False;
  if (aprotocol.IsNull()) return;    // PAS de protocole = Lib VIDE
  if (!theprotocol.IsNull()) last =
    (theprotocol == aprotocol);

  if (last) thelist = thelast;
  //  Si Pas d optimisation disponible : construire la liste
  else {
    AddProtocol(aprotocol);
    //  Ceci definit l optimisation (pour la fois suivante)
    thelast     = thelist;
    theprotocol = aprotocol;
  }
}

//! Creates an empty Library : it will later by filled by method AddProtocol.
template<class ObjectType,class ModuleType, class ProtocolType>
LibCtl_Library<ObjectType, ModuleType, ProtocolType>::LibCtl_Library ()    {  }

//! Adds a couple (Module-Protocol) to the Library, given the
//! class of a Protocol. Takes Resources into account.
//! (if <aprotocol> is not of type TheProtocol, it is not added).
template<class ObjectType,class ModuleType, class ProtocolType>
void LibCtl_Library<ObjectType, ModuleType, ProtocolType>::AddProtocol
    (const Handle(Standard_Transient)& aprotocol)
{
  ProtocolType aproto = ProtocolType::DownCast(aprotocol);
  if (aproto.IsNull()) return;

  //  D first, add it to the list: search the Node
  NCollection_Handle<LibCtl_GlobalNode > curr;
  for (curr = theglobal; !curr.IsNull(); )
  {
    const ProtocolType& protocol = curr->Protocol();
    if (!protocol.IsNull()) {
      //  Match Protocol ?
      if (protocol->DynamicType() == aprotocol->DynamicType()) {
        if (thelist.IsNull()) thelist = new LibCtl_Node;
        thelist->AddNode(curr);
        break;
      }
    }
    curr = curr->Next();
  }
  // Treat resources
  Standard_Integer nb = aproto->NbResources();
  for (Standard_Integer i = 1; i <= nb; i ++) {
    AddProtocol (aproto->Resource(i));
  }

  theprotocol.Nullify();
  thelast.Nullify();
}

//! Clears the list of Modules of a library (can be used to
//! redefine the order of Modules before action : Clear then
//! refill the Library by calls to AddProtocol).
template<class ObjectType,class ModuleType, class ProtocolType>
void LibCtl_Library<ObjectType, ModuleType, ProtocolType>::Clear ()
{
  thelist = new LibCtl_Node;
}

//! Sets a library to be defined with the complete Global list
//! (all the couples Protocol/Modules recorded in it).
template<class ObjectType,class ModuleType, class ProtocolType>
void LibCtl_Library<ObjectType, ModuleType, ProtocolType>::SetComplete ()
{
  thelist = new LibCtl_Node;
  // Take each of the Protocols of the Global list and add it
  NCollection_Handle<LibCtl_GlobalNode > curr;
  for (curr = theglobal; !curr.IsNull(); )
  {
    const ProtocolType& protocol = curr->Protocol();
    // As we take all, it is not preoccupied resources.
    if (!protocol.IsNull()) thelist->AddNode(curr);
    curr = curr->Next();
  }
}

//! Selects a Module from the Library, given an Object.
//! Returns True if Select has succeeded, False else.
//! Also Returns (as arguments) the selected Module and the Case
//! Number determined by the associated Protocol.
//! If Select has failed, <module> is Null Handle and CN is zero.
//! (Select can work on any criterium, such as Object DynamicType).
template<class ObjectType,class ModuleType, class ProtocolType>
Standard_Boolean LibCtl_Library<ObjectType, ModuleType, ProtocolType>::Select
                                             (const ObjectType& obj,
                                              ModuleType& module,
                                              Standard_Integer& CN) const
{
  module.Nullify();  CN = 0;    // Answer "not found"
  if (thelist.IsNull()) return Standard_False;
  NCollection_Handle<LibCtl_Node > curr = thelist;
  for (curr = thelist; !curr.IsNull(); )
  {
    const ProtocolType& protocol = curr->Protocol();
    if (!protocol.IsNull()) {
      CN = protocol->CaseNumber(obj);
      if (CN > 0) {
        module = curr->Module();
        return Standard_True;
      }
    }
    curr = curr->Next();
  }
  return Standard_False;
}

//! Starts Iteration on the Modules (sets it on the first one).
template<class ObjectType,class ModuleType, class ProtocolType>
void LibCtl_Library<ObjectType, ModuleType, ProtocolType>::Start ()
{
  thecurr = thelist;
}

//! Returns True if there are more Modules to iterate on.
template<class ObjectType,class ModuleType, class ProtocolType>
Standard_Boolean LibCtl_Library<ObjectType, ModuleType, ProtocolType>::More () const
{
  return (!thecurr.IsNull());
}

//! Iterates by getting the next Module in the list.
//! If there is none, the exception will be raised by Value.
template<class ObjectType,class ModuleType, class ProtocolType>
void LibCtl_Library<ObjectType, ModuleType, ProtocolType>::Next ()
{
  if (!thecurr.IsNull()) thecurr = thecurr->Next();
}

//! Returns the current Module in the Iteration.
//! Error if there is no current Module to iterate on.
template<class ObjectType,class ModuleType, class ProtocolType>
const ModuleType& LibCtl_Library<ObjectType, ModuleType, ProtocolType>::Module () const
{
  if (thecurr.IsNull()) Standard_NoSuchObject::Raise("Library from LibCtl");
  return thecurr->Module();
}

//! Returns the current Protocol in the Iteration.
//! Error if there is no current Protocol to iterate on.
template<class ObjectType,class ModuleType, class ProtocolType>
const ProtocolType& LibCtl_Library<ObjectType, ModuleType, ProtocolType>::Protocol () const
{
  if (thecurr.IsNull()) Standard_NoSuchObject::Raise("Library from LibCtl");
  return thecurr->Protocol();
}
#endif

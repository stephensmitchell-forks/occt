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

#include <Standard.hxx>
#include <Standard_DefineAlloc.hxx>
#include <Standard_NoSuchObject.hxx>
#include <NCollection_Handle.hxx>

template <class ModuleType, class ProtocolType> class LibCtl_GlobalNode;

//! Manages a list of Modules for a Library (as an instance) :
//! Designates a GlobalNode (couple Module-Protocol)
template <class ModuleType, class ProtocolType>
class LibCtl_Node
{
private:
  typedef LibCtl_GlobalNode<ModuleType, ProtocolType> LibCtl_GlobalNode;

  NCollection_Handle < LibCtl_GlobalNode > myNode;
  NCollection_Handle < LibCtl_Node > myNext;

public:

  Standard_EXPORT   LibCtl_Node ();
  Standard_EXPORT   void AddNode (const NCollection_Handle< LibCtl_GlobalNode >& theNode);
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
void LibCtl_Node<ModuleType,ProtocolType>::AddNode (const NCollection_Handle< LibCtl_GlobalNode >& theNode)
{
  if (myNode == theNode) return;
  if (myNext.IsNull()) {
    if (myNode.IsNull()) myNode = theNode;
    else {
      myNext = new LibCtl_Node;
      myNext->AddNode (theNode);
    }
  }
  else myNext->AddNode (theNode);
}

//! Returns the Module designated by a precise Node.
template<class ModuleType, class ProtocolType>
const ModuleType& LibCtl_Node<ModuleType,ProtocolType>::Module () const
{
  return myNode->Module();
}

//! Returns the Protocol designated by a precise Node.
template<class ModuleType, class ProtocolType>
const ProtocolType& LibCtl_Node<ModuleType,ProtocolType>::Protocol () const
{
  return myNode->Protocol();
}

//! Returns the next Node.
//! If none was defined, returned value is a null handle.
template<class ModuleType, class ProtocolType>
const NCollection_Handle< LibCtl_Node<ModuleType,ProtocolType> >& LibCtl_Node<ModuleType,ProtocolType>::Next () const
{
  return myNext;
}

//! Manages a (possibly static) Global List of Modules bound to protocols.
//! Remark that it requires independance from Memory Management
//! (because a Global List of Modules can be build through static
//! declarations, i.e. before any sequential execution)
//! Remark there will not be many many GlobalNodes created
template<class ModuleType, class ProtocolType>
class LibCtl_GlobalNode
{
private:
  ModuleType myMod;
  ProtocolType myProt;
  NCollection_Handle <LibCtl_GlobalNode> myNext;
public:
  Standard_EXPORT   LibCtl_GlobalNode ();
  Standard_EXPORT   void Add (const ModuleType& theModule, const ProtocolType& theProtocol);
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
(const ModuleType& theModule, const ProtocolType& theProtocol)
{
  if (myMod == theModule) return;
  if (myProt == theProtocol) myMod = theModule;
  else if (myNext.IsNull())
  {
    if (myMod.IsNull())
    {
      myMod = theModule;
      myProt = theProtocol;
    }
    else
    {
      myNext = new LibCtl_GlobalNode;
      myNext->Add (theModule,theProtocol);
    }
  }
  else myNext->Add (theModule,theProtocol);
}

//! Returns the Module stored in a given GlobalNode.
template<class ModuleType, class ProtocolType>
const ModuleType& LibCtl_GlobalNode <ModuleType,ProtocolType>::Module () const
{
  return myMod;
}

//! Returns the attached Protocol stored in a given GlobalNode.
template<class ModuleType, class ProtocolType>
const ProtocolType& LibCtl_GlobalNode <ModuleType,ProtocolType>::Protocol () const
{
  return myProt;
}

//! Returns the next GlobalNode.
//! If none is defined, returned value is a null handle.
template<class ModuleType, class ProtocolType>
const NCollection_Handle <LibCtl_GlobalNode<ModuleType,ProtocolType> >&
LibCtl_GlobalNode <ModuleType,ProtocolType>::Next () const
{
  return myNext;
}

// Library
//! Manages a list of Execution Modules attached to Protocols
//! to perform a specific set of functionnalities.
//!           
//! Each instantiated class of Library has a global set a Modules.
//! These Modules are put in this set before working, for instance
//! by static construction (using method SetGlobal). One Module
//! is bound which each Protocol (considered as a class).
//!
//! To work, a Library is created by taking the Modules which
//! comply with a Protocol (bound with its class and the classes
//! of its Resources), given as parameter of its creation.
//!
//! Thus, any tool can use it to get the suitable Modules
//! Warning : The order of the Modules in the Library has assumed to be
//! useless, and is not managed.
template <class ObjectType, class ModuleType, class ProtocolType>
class LibCtl_Library
{
public:
  // Type definitions for template classes
  typedef LibCtl_GlobalNode<ModuleType, ProtocolType> LibCtl_GlobalNode;
  typedef LibCtl_Node<ModuleType, ProtocolType>       LibCtl_Node;
public:
  // Basic data for optimization (Protocol last request)
  static ProtocolType myProtocol;
  static NCollection_Handle< LibCtl_Node > myLast;

  DEFINE_STANDARD_ALLOC

private:
    NCollection_Handle< LibCtl_Node > myList;
    NCollection_Handle< LibCtl_Node > myCurr;
public:

  //! Gets global list of modules
  static NCollection_Handle< LibCtl_GlobalNode >& GetGlobal();
 
   //! Supply the global list
  Standard_EXPORT static void SetGlobal (const ModuleType& theModule, const ProtocolType& theProtocol)
  {
    if (GetGlobal().IsNull()) GetGlobal() = new LibCtl_GlobalNode;
    GetGlobal()->Add(theModule,theProtocol);
  }
  
  Standard_EXPORT LibCtl_Library (const ProtocolType& theProtocol)
  {
    Standard_Boolean last = Standard_False;
    // No protocol = empty library
    if (theProtocol.IsNull()) return;
    if (!myProtocol.IsNull()) last =
      (myProtocol == theProtocol);

    if (last)
     myList = myLast;
    // If no optimization available: list building
    else
    {
      AddProtocol(theProtocol);
      // This defines the optimization (for the next time)
      myLast     = myList;
      myProtocol = theProtocol;
    }
  }

  Standard_EXPORT LibCtl_Library ()
  { }

  //! Add a Protocol: attention: it is desoptimise (if not confusion!)
  Standard_EXPORT void AddProtocol (const Handle(Standard_Transient)& theProtocol)
  {
    // Downcast as Protocol-> Resources, and even redefined to use in other
    // Library must always return the type highest
    ProtocolType aProtocol = ProtocolType::DownCast(theProtocol);
    if (aProtocol.IsNull()) return;

    NCollection_Handle< LibCtl_GlobalNode > curr;
    for (curr = GetGlobal(); !curr.IsNull(); )
    {
      const ProtocolType& aProtocol = curr->Protocol();
      if (!aProtocol.IsNull())
      {
        //  Match Protocol ?
        if (aProtocol->DynamicType() == theProtocol->DynamicType())
        {
          if (myList.IsNull())
            myList = new LibCtl_Node;

          myList->AddNode(curr);
          break;  // UN SEUL MODULE PAR PROTOCOLE
        }
      }
      curr = curr->Next();
    }
    //  Treat resources
    Standard_Integer nb = aProtocol->NbResources();
    for (Standard_Integer i = 1; i <= nb; i++)
    {
      AddProtocol (aProtocol->Resource(i));
    }
    
    myProtocol.Nullify();
    myLast.Nullify();
  }

  Standard_EXPORT void Clear ()
  {
    myList = new LibCtl_Node();
  }

  Standard_EXPORT void SetComplete ()
  {
    myList = new LibCtl_Node;
    // Take each of the protocols of the Global list
    NCollection_Handle< LibCtl_GlobalNode > curr;
    for (curr = GetGlobal(); !curr.IsNull(); )
    {
      const ProtocolType& aProtocol = curr->Protocol();
      // As we take all , it is not preoccupied resources
      if (!aProtocol.IsNull()) myList->AddNode(curr);
      curr = curr->Next();
    }
  }

  //!  Selection: the corresponding theModule is returned to a Type
  //! (as well as CaseNumber returns the corresponding protocol)
  Standard_EXPORT Standard_Boolean Select (const ObjectType& theObj,
                                           ModuleType& theModule,
                                           Standard_Integer& theCN) const
  {
    // Reponse: "not found"
    theModule.Nullify(); theCN = 0;
    if (myList.IsNull())
      return Standard_False;
    NCollection_Handle< LibCtl_Node > curr = myList;
    for (curr = myList; !curr.IsNull(); )
    {
      const ProtocolType& aProtocol = curr->Protocol();
      if (!aProtocol.IsNull())
      {
        theCN = aProtocol->CaseNumber(theObj);
        if (theCN > 0)
        {
          theModule = curr->Module();
          return Standard_True;
        }
      }
      curr = curr->Next();
    }
    return Standard_False;
  }

  //! Initialization for iteration through the library
  Standard_EXPORT void Start ()
  {
    myCurr = myList;
  }

  Standard_EXPORT Standard_Boolean More () const
  {
    return (!myCurr.IsNull());  
  }

  Standard_EXPORT void LibCtl_Library::Next ()
  {
    if (!myCurr.IsNull())
      myCurr = myCurr->Next();
  }

  Standard_EXPORT const ModuleType& Module () const
  {
    if (myCurr.IsNull())
      Standard_NoSuchObject::Raise("Library from LibCtl");
    return myCurr->Module();
  }

  Standard_EXPORT const ProtocolType& Protocol () const
  {
    if (myCurr.IsNull())
      Standard_NoSuchObject::Raise("Library from LibCtl");
    return myCurr->Protocol();
  }
};

//! Basic data for optimization (Protocol last request)
template <class ObjectType, class ModuleType, class ProtocolType>
ProtocolType LibCtl_Library<ObjectType, ModuleType, ProtocolType>::myProtocol;

template <class ObjectType, class ModuleType, class ProtocolType>
NCollection_Handle <LibCtl_Node <ModuleType, ProtocolType> >
LibCtl_Library<ObjectType, ModuleType, ProtocolType>::myLast;

#endif

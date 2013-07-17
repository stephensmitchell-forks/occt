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

#include <Interface_ReaderLib.hxx>


  static NCollection_Handle < LibCtl_GlobalNode<Handle(Interface_ReaderModule),
                                                Handle(Interface_Protocol)> >
     myGlobal;

  static NCollection_Handle < LibCtl_Node<Handle(Interface_ReaderModule),
                                          Handle(Interface_Protocol)> >
     myLast;

  static Handle(Interface_Protocol)
     myProtocol;


  void Interface_ReaderLib::SetGlobal (const Handle(Interface_ReaderModule)& theModule,
                                         const Handle(Interface_Protocol)& theProtocol)
  {
    if (myGlobal.IsNull())
      myGlobal = new LibCtl_ReaderLib::LibCtl_GlobalNode;
    myGlobal->Add(theModule,theProtocol);
  }

  Interface_ReaderLib::Interface_ReaderLib(const Handle(Interface_Protocol)& theProtocol)
  {
    Standard_Boolean last = Standard_False;
    if (theProtocol.IsNull())
      return;
    if (!myProtocol.IsNull())
      last = (myProtocol == theProtocol);

    if (last)
     thelist = myLast;
    // If no optimization available: list building
    else
    {
      AddProtocol(theProtocol);
      // This defines the optimization (for the next time)
      myLast     = thelist;
      myProtocol = theProtocol;
    }
  }

  void Interface_ReaderLib::SetComplete ()
  {
    thelist = new LibCtl_ReaderLib::LibCtl_Node;
    // Take each of the protocols of the Global list
    NCollection_Handle <LibCtl_ReaderLib::LibCtl_GlobalNode> curr;
    for (curr = myGlobal; !curr.IsNull(); )
    {
      const Handle(Interface_Protocol)& protocol = curr->Protocol();
      // As we take all , it is not preoccupied resources
      if (!protocol.IsNull()) thelist->AddNode(curr);
      curr = curr->Next();
    }
  }

  void Interface_ReaderLib::AddProtocol (const Handle(Standard_Transient)& theProtocol)
  {
    // Downcast as Protocol-> Resources, and even redefined to use in other
    // Library must always return the type highest
    Handle(Interface_Protocol) aProtocol = Handle(Interface_Protocol)::DownCast(theProtocol);
    if (aProtocol.IsNull()) return;

    NCollection_Handle <LibCtl_ReaderLib::LibCtl_GlobalNode> curr;
    for (curr = myGlobal; !curr.IsNull(); )
    {
      const Handle(Interface_Protocol)& protocol = curr->Protocol();
      if (!protocol.IsNull())
      {
        if (protocol->DynamicType() == theProtocol->DynamicType())
        {
          if (thelist.IsNull())
            thelist = new LibCtl_ReaderLib::LibCtl_Node;
          thelist->AddNode(curr);
          break;
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

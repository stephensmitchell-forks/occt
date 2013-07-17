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

#ifndef _Interface_Recognizer_HeaderFile
#define _Interface_Recognizer_HeaderFile

#include <Standard_Transient.hxx>
#include <Standard_NoSuchObject.hxx>


template <class KeyType, class ResultType, class HandleType>
class Interface_Recognizer : public Standard_Transient
{
private:
  // Storing result of last evaluation 
  ResultType myRes;
  
protected: 
  Standard_Boolean myHasNext;
 
  // Managing compound definition
  HandleType myNext;

public:
  
  Standard_EXPORT Standard_Boolean Evaluate (const KeyType& theKey,
                                             ResultType& theRes)
  {
    myRes.Nullify();
    Eval(theKey);
    if (!myRes.IsNull())
    {
      theRes = myRes;
      return Standard_True;
    }
    else if (myHasNext) return myNext->Evaluate(theKey, theRes);
  return Standard_False;
  }

  Standard_EXPORT  ResultType Result () const
  {
    if (!myRes.IsNull()) return myRes;
    if (myHasNext) return myNext->Result();
    Standard_NoSuchObject::Raise ("Recognizer evaluation has failed");
    return myRes;
  }

    Standard_EXPORT void Add (const HandleType& theRecognizer)
    {
      myNext = theRecognizer;
      myHasNext = Standard_True;
    }
protected:

  Standard_EXPORT Interface_Recognizer ()
  {
    myHasNext = Standard_False;
  }

  Standard_EXPORT void SetOK (const ResultType& theResult)
  {
    myRes = theResult;
  }
  Standard_EXPORT void SetKO ()
  {
    myRes.Nullify();
  }
  Standard_EXPORT   virtual  void Eval(const KeyType& akey)  = 0;
};

#endif
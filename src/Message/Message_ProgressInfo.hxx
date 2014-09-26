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

#ifndef _Message_ProgressInfo_HeaderFile
#define _Message_ProgressInfo_HeaderFile

#include <Standard.hxx>
#include <Standard_Real.hxx>
#include <Standard_Transient.hxx>
#include <Standard_Mutex.hxx>
#include <Standard_Failure.hxx>
#include <TCollection_HAsciiString.hxx>

DEFINE_STANDARD_HANDLE(Message_ProgressInfo, Standard_Transient)

//! Base class for hierarchical progress indicators and progress scopes.
//! Provides thread safe incrementation of progress position.
//! Number of sub-scopes depends on the given step.
class Message_ProgressInfo : public Standard_Transient
{
public:

  //! Default constructor.
  //! Creates progress info with single scope.
  Standard_EXPORT Message_ProgressInfo();

  //! Constructor.
  //! Creates progress with the given parameters.
  Standard_EXPORT Message_ProgressInfo(
    const Standard_CString theName,
    const Standard_Real    theMin,
    const Standard_Real    theMax,
    const Standard_Integer theSubScopesNb);

  //! Constructor.
  //! Creates progress with the given parameters.
  Standard_EXPORT Message_ProgressInfo(
    const Handle(TCollection_HAsciiString)& theName,
    const Standard_Real                     theMin,
    const Standard_Real                     theMax,
    const Standard_Integer                  theSubScopesNb);

  //! Sets scale name.
  Standard_EXPORT void SetName(const Standard_CString theName);

  //! Sets scale name.
  Standard_EXPORT virtual void SetName(const Handle(TCollection_HAsciiString)& theName);

  //! Gets scale name. It may be Null handle if not set.
  inline const Handle(TCollection_HAsciiString)& GetName() const
  {
    return myName;
  }

  //! Set range for current scale.
  //! Throws Standard_Failure exception in case if theMin > theMax.
  inline void SetRange(const Standard_Real theMin,
                       const Standard_Real theMax)
  {
    if (theMin > theMax)
      Standard_Failure::Raise("Message_ProgressInfo: Incorrect range");

    myMin = theMin;
    myMax = theMax;
    updateStep();
  }

  //! Get range for current scale.
  inline void GetRange(Standard_Real& theMin,
                       Standard_Real& theMax)
  {
    theMin = myMin;
    theMax = myMax;
  }

  //! Gets minimum value of scale.
  inline Standard_Real GetMin() const
  {
    return myMin;
  }

  //! Gets maximum value of scale.
  inline Standard_Real GetMax() const
  {
    return myMax;
  }

  //! Gets scale step.
  //! Depends on number of scopes and scope range.
  inline Standard_Real GetStep() const
  {
    return myStep;
  }

  //! Sets number of scopes for current scale
  inline void SetScopesNb(const Standard_Integer theScopesNb)
  {
    myScopesNb = Max(1, theScopesNb);
    updateStep();
  }

  //! Gets number of scopes for current scale
  inline Standard_Integer GetScopesNb() const
  {
    return myScopesNb;
  }

  //! Gets current position of this scale.
  inline Standard_Real GetPosition() const
  {
    return myPosition;
  }

  //! Increments progress position by default step.
  inline void Increment()
  {
    Increment(myStep);
  }

  //! Increments progress position by the given step.
  //! Internally calls increment() method wrapped by mutex.
  //! @param theStep value the progress position should be incremented by,
  //!                should be consistent with range of current scale. 
  Standard_EXPORT virtual void Increment(const Standard_Real theStep);

  //! Resets progress data to start new counting.
  Standard_EXPORT virtual void Reset();

  DEFINE_STANDARD_RTTI(Message_ProgressInfo)

protected:

  //! Updates step value according to scope parameters.
  inline void updateStep()
  {
    Standard_Real aRange = myMax - myMin;
    myStep = aRange / myScopesNb;
  }

  //! Performs updating of progress indicator data within thread safe scope.
  //! By default increments position by the given step.
  Standard_EXPORT virtual Standard_Boolean increment(const Standard_Real theStep);

private:
  Handle(TCollection_HAsciiString) myName;
  Standard_Real                    myMin;
  Standard_Real                    myMax;
  Standard_Integer                 myScopesNb;
  Standard_Real                    myStep;
  Standard_Real                    myPosition;
  Standard_Mutex                   myPositionMutex;
};

#endif

// File:      OSD_MPData.cxx
// Created:   29.06.09 09:19
// Author:    Alexander GRIGORIEV
// Copyright: Open Cascade 2009

#include <OSD_MProcess.hxx>

#ifdef WNT
// 'this' : used in base member initializer list
#pragma warning(disable:4355)
#endif

static const OSD_MProcess * DummyProcessor = 0L;

//=======================================================================
//function : OSD_MPData
//purpose  : Empty constructor
//=======================================================================

OSD_MPData::OSD_MPData ()
  : myID              (0u),
    myPrevious        (this),
    myNext            (this),
    myThreadID        (UINT_MAX),
    myProcessor       (* DummyProcessor),
    myStatus          (StatusEmpty)    
{
  myThreadID = UINT_MAX;
}

//=======================================================================
//function : OSD_MPData()
//purpose  : Constructor
//=======================================================================

OSD_MPData::OSD_MPData (const OSD_MProcess& theProcessor)
  : myPrevious  (this),
    myNext      (this),
    myThreadID  (UINT_MAX),
    myProcessor (theProcessor),
    myStatus    (StatusEmpty)
{
  static volatile Standard_Size MyCounter = 0u;
  myID = MyCounter++;
}

//=======================================================================
//function : ~OSD_MPData
//purpose  : Destructor
//=======================================================================

OSD_MPData::~OSD_MPData ()
{
}

//=======================================================================
//function : AddBefore
//purpose  : Insert a new data object in the queue
//=======================================================================

void OSD_MPData::AddBefore (OSD_MPData * theAdded)
{
  theAdded->myPrevious = myPrevious;
  theAdded->myNext = this;
  myPrevious->myNext = theAdded;
  myPrevious = theAdded;
}

//=======================================================================
//function : Detach
//purpose  : Remove this data object from the queue
//=======================================================================

void OSD_MPData::Detach ()
{
  myPrevious->myNext = myNext;
  myNext->myPrevious = myPrevious;
//   myPrevious = this;
//   myNext = this;
  myThreadID = UINT_MAX;
}

//=======================================================================
//function : ThreadAlloc
//purpose  : Get the thread-owned allocator instance
//=======================================================================

const Handle_NCollection_IncAllocator& OSD_MPData::ThreadAlloc() const
{
  if (myThreadID == UINT_MAX) {
    static Handle_NCollection_IncAllocator aNullAlloc;
    aNullAlloc.Nullify();
    return aNullAlloc;
  }
  return myProcessor.myArrThread[myThreadID].Allocator();
}

//=======================================================================
//function : Process
//purpose  : 
//=======================================================================

void OSD_MPData::Process ()
{
}

//=======================================================================
//function : PostProcess
//purpose  : 
//=======================================================================

void OSD_MPData::PostProcess ()
{
}

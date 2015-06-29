// File:      OSD_MProcess.cxx
// Created:   29.06.09 09:40
// Author:    Alexander GRIGORIEV
// Copyright: Open Cascade 2009


#include <OSD_MProcess.hxx>

#ifdef WNT
// 'this' : used in base member initializer list
#pragma warning(disable:4355)
#endif

static Standard_Integer Delay (const unsigned int theValue);

//=======================================================================
//function : MPThread::MPThread
//purpose  : Constructor
//=======================================================================

OSD_MProcess::MPThread::MPThread ()
  : myProcessor (0L),
    mypData     (0L),
    myID        (UINT_MAX),
    myNProcessed(0u),
    myAlloc     (new NCollection_IncAllocator),
    myIsBusy    (Standard_False),
    myIsActive  (Standard_False),
    myIsSleeping(Standard_False)
{
}

//=======================================================================
//function : Process
//purpose  : Thread function
//=======================================================================

Standard_Address OSD_MProcess::MPThread::Process(Standard_Address pThread)
{
  MPThread& aThread = * reinterpret_cast<MPThread *> (pThread);
  const OSD_MProcess& aMP = * aThread.myProcessor;
  OSD_MPData * aBaseData = aThread.mypData;
  OSD_MPData * aData = aBaseData;
  aThread.myIsActive = Standard_True;
  Standard_Integer aSleepCounter(0);
  while (aThread.myIsActive) {
    // Seek a waiting data block and process it
    if (processData(aThread, aData, aBaseData, aSleepCounter))
    {
      if (aSleepCounter < 100 || aMP.AllowSleep() == Standard_False)
        Delay(100);
      else {
        aThread.myIsBusy = Standard_False;
        aThread.myIsSleeping = Standard_True;
        Sleep(1);
        aThread.myIsSleeping = Standard_False;
      }
      if (++aSleepCounter > 100)
        aThread.myIsBusy = Standard_False;
    }
  }
  return 0L;
}

//=======================================================================
//function : processData
//purpose  : 
//=======================================================================

Standard_Boolean OSD_MProcess::processData (MPThread&         aThread,
                                            OSD_MPData*&      theData,
                                            OSD_MPData*       theBaseData,
                                            Standard_Integer& aSleepCounter)
{
  const Standard_Size aThreadID = aThread.myID;
  Standard_Boolean isWait(Standard_True);
  if (theData) {
    isWait = Standard_False;
    OSD_MPData * aData = theData->myNext;
    while (aData != theBaseData)
    {
      if (aData->myStatus == OSD_MPData::StatusWait)
      {
        InterlockedCompareExchange((volatile LONG *)&aData->myThreadID,
                                   aThreadID, UINT_MAX);
        if (aData->myThreadID == aThreadID)
        {
          aData->myStatus = OSD_MPData::StatusProcess;
          aThread.myIsBusy = Standard_True;
          aSleepCounter = 0;
          aData->Process();
          aThread.myNProcessed++;
          aData->myStatus = OSD_MPData::StatusDone;           
          aThread.mypData = aData;
          break;
        }
      }
      aData = aData->myNext;
    }
    if (aData == theBaseData) {
      isWait = Standard_True;
    }
    theData = aData;
  }
  return isWait;
}

//=======================================================================
//function : OSD_MProcess()
//purpose  : Constructor
//=======================================================================

OSD_MProcess::OSD_MProcess ()
  : myNThreads  (0),
    myArrThread (0L),
    myNDataAll  (0u),
    myIsSleepy  (Standard_False)
{
  myData.myStatus = OSD_MPData::DummyData;
}

//=======================================================================
//function : Init
//purpose  : 
//=======================================================================

void OSD_MProcess::Init (const Standard_Size nThreads,
                         const Standard_Integer thePriority)
{
  if (myNThreads)
    Clear();
  myAlloc = new NCollection_IncAllocator;
  myNThreads = nThreads;
  myIsSleepy = Standard_False;
  myArrThread = new MPThread[nThreads + 1];
  for (Standard_Size iThread = 0; iThread <= nThreads; iThread++) {
    myArrThread[iThread].myProcessor = this;
    myArrThread[iThread].myID = iThread;
    myArrThread[iThread].mypData = &myData;
    if (iThread) {
      myArrThread[iThread].SetFunction(&MPThread::Process);
      myArrThread[iThread].SetPriority(thePriority);
      myArrThread[iThread].Run(&myArrThread[iThread]);
    }
  }
}

//=======================================================================
//function : OSD_MProcess()
//purpose  : Constructor
//=======================================================================

OSD_MProcess::OSD_MProcess (const Standard_Size nThreads,
                            const Standard_Integer thePriority)
  : myAlloc        (new NCollection_IncAllocator),
    myNThreads     (nThreads),
    myArrThread    (new MPThread[nThreads + 1]),
    myNDataAll     (0u),
    myIsSleepy     (Standard_False)
{
  myData.myStatus = OSD_MPData::DummyData;
  for (Standard_Size iThread = 0; iThread <= nThreads; iThread++) {
    myArrThread[iThread].myProcessor = this;
    myArrThread[iThread].myID = iThread;
    myArrThread[iThread].mypData = &myData;
    if (iThread) {
      myArrThread[iThread].SetFunction(&MPThread::Process);
      myArrThread[iThread].SetPriority(thePriority);
      myArrThread[iThread].Run(&myArrThread[iThread]);
    }
  }
}

//=======================================================================
//function : ~OSD_MProcess
//purpose  : Destructor
//=======================================================================

OSD_MProcess::~OSD_MProcess ()
{
  Clear();
}

//=======================================================================
//function : NProcessed
//purpose  : 
//=======================================================================

Standard_Size OSD_MProcess::NProcessed () const
{
  Standard_Size aSum(0u);
  for (Standard_Size i = 0; i <= myNThreads; i++)
    aSum += myArrThread[i].myNProcessed;
  return aSum;
}

//=======================================================================
//function : AddData
//purpose  : Put a data block into the queue.
//=======================================================================

void OSD_MProcess::AddData (OSD_MPData * theData)
{
  theData->myStatus = OSD_MPData::StatusEmpty;
  theData->myThreadID = UINT_MAX;
  myData.AddBefore(theData);
  myNDataAll++;

  theData->myStatus = OSD_MPData::StatusWait;
}

//=======================================================================
//function : TakeData
//purpose  : Extract already processed data block, removing it from the queue.
//=======================================================================

OSD_MPData * OSD_MProcess::TakeData ()
{
  OSD_MPData * aData = 0L;
  const OSD_MPData * pBaseData = &myData;
  OSD_MPData * pData = pBaseData->myNext;
  for (; pData != pBaseData; pData = pData->myNext) {
    if (pData->myStatus == OSD_MPData::StatusDone) {
      pData->PostProcess();
      myNDataAll--;
      pData->Detach();
      aData = pData;
      break;
    }
  }
  return aData;
}

//=======================================================================
//function : Wait
//purpose  : Wait for the time theMillisec
//=======================================================================

Standard_Boolean OSD_MProcess::Wait (const Standard_Integer theMillisec,
                                     const Standard_Boolean doTasks)
{
  // Wait till all threads conclude
  Standard_Boolean aResult(Standard_True);
  Standard_Size i;
  if (theMillisec > 0)
    Sleep(theMillisec);
  else if (doTasks)
    myArrThread[0].mypData = &myData;

  for (;;)
  {
    for (i = 1; i <= myNThreads; i++) {
      if (myArrThread[i].IsBusy()) {
        aResult = Standard_False;
        break;
      }
    }
    if (aResult) {
      OSD_MPData * pData = myData.myNext;
      for (; pData != &myData; pData = pData->myNext) {
        if (pData->myStatus != OSD_MPData::StatusDone) {
          aResult = Standard_False;
          break;
        }
      }
      if (aResult)
        break;
    }
    if (theMillisec > 0)
      break;
    if (doTasks) {
      OSD_MPData * aData = myArrThread[0].mypData;
      Standard_Integer aSleepCounter(0);
      // Seek a waiting data block and process it
      processData(myArrThread[0], aData, &myData, aSleepCounter);
    } else if (myIsSleepy)
      Sleep(1);
    aResult = Standard_True;
  }
  return aResult;
}

//=======================================================================
//function : Open
//purpose  : 
//=======================================================================

void OSD_MProcess::Open() const
{
  Standard_Integer aCounter(5);
  while (aCounter) {
    for (Standard_Size i = 1; i <= myNThreads; i++) {
      if (myArrThread[i].IsBusy())
        return;
    }
    Standard_Integer aFlag(0);
    OSD_MPData * pData = myData.myNext;
    if (pData == &myData)
      break;    // protect from the case when no Data are loaded
    for (; pData != &myData; pData = pData->myNext) {
      if (pData->myStatus == OSD_MPData::StatusDone)
        aFlag |= 0x1;
      else
        aFlag |= 0x2;
      if (aFlag == 3) {
        aCounter--;
        break;
      }
    }
    if (aFlag == 0x1) // all having StatusDone
      break;
  }
}

//=======================================================================
//function : Close
//purpose  : 
//=======================================================================

void OSD_MProcess::Close()
{
  Wait(0, Standard_True);
  OSD_MPData * pData = myData.myNext;
  for (; pData != &myData; pData = pData->myNext) {
    if (pData->myStatus == OSD_MPData::StatusDone) {
      pData->PostProcess();
      myNDataAll--;
    }
  }
  myData.myPrevious = &myData;
  myData.myNext = &myData;
  myNDataAll = 0;
  for (Standard_Size i = 0; i <= myNThreads; i++) {
    const Handle(NCollection_IncAllocator)& anAlloc =
      myArrThread[i].Allocator();
    anAlloc->Reset(Standard_False);
  }
}

//=======================================================================
//function : Clear
//purpose  : 
//=======================================================================

void OSD_MProcess::Clear(const Standard_Boolean isWait)
{
  // Stop all threads
  for (Standard_Size iThread = 1; iThread <= myNThreads; iThread++) {
    myArrThread[iThread].myIsActive = Standard_False;
    if (isWait)
      myArrThread[iThread].Wait();
  }

  // Collect the data
  if (isWait) {
    OSD_MPData * pData = myData.myNext;
    for (; pData != &myData; pData = pData->myNext) {
      if (pData->myStatus == OSD_MPData::StatusDone) {
        pData->PostProcess();
        myNDataAll--;
      }
    }
  }

  // Free the resources
  myData.myPrevious = &myData;
  myData.myNext = &myData;
  if (myNThreads) {
    myNThreads = 0;
    delete [] myArrThread;
    myAlloc.Nullify();
    myNDataAll = 0;
  }
}

//=======================================================================
//function : Delay
//purpose  : local function; delays proportionally to the given value
//=======================================================================

Standard_Integer Delay (const unsigned int theValue)
{
  volatile Standard_Integer aSum = 1; 
  for (unsigned int i = 0; i < theValue; i++)
    aSum += aSum / (i+1);
  return aSum;
}

//=======================================================================
//function : AllowSleep
//purpose  : 
//=======================================================================

Standard_Boolean OSD_MProcess::AllowSleep () const
{
  if (myIsSleepy)
    return Standard_True;
  Standard_Integer nAwake(myNThreads);
  for (Standard_Size i = 1; i <= myNThreads; i++) {
    if (myArrThread[i].myIsSleeping)
      nAwake--;
  }
  return (nAwake >= GetMaxThreads());
}

//=======================================================================
//function : getMaxThreads
//purpose  : 
//=======================================================================

Standard_Integer OSD_MProcess::GetMaxThreads()
{
#ifdef WNT
#pragma warning(disable:4996)
#endif
  static Standard_Integer nThreads(0);
  if (nThreads == 0) {
    char * strVar = getenv("OMP_NUM_THREADS");
    if (strVar) {
      nThreads = atoi(strVar);
      if (nThreads == 0)
        nThreads = -1;
    }
    if (nThreads < 0)
      nThreads = 0;
    else if (nThreads == 0) {
      strVar = getenv("NUMBER_OF_PROCESSORS");
      if (strVar)
        nThreads = atoi(strVar);
    }
  }
  return nThreads;
}


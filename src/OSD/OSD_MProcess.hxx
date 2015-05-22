// File:      OSD_MProcess.hxx
// Created:   29.06.09 09:34
// Author:    Alexander GRIGORIEV
// Copyright: Open Cascade 2009


#ifndef OSD_MProcess_HeaderFile
#define OSD_MProcess_HeaderFile

#include <OSD_MPData.hxx>
#include <OSD_Thread.hxx>

/**
 * Manager of calculations in multiple threads (OSD_MPThread)
 */

class OSD_MProcess
{
 public:
  class MPThread : public OSD_Thread
  {
  public:
    //! Constructor.
    Standard_EXPORT MPThread ();

    //! Thread function
    static Standard_Address Process(Standard_Address);

    //! Query if the thread is busy or idle.
    inline Standard_Boolean IsBusy () const     { return myIsBusy; }

    //! Get the thread-owned memory Allocator
    inline const Handle_NCollection_IncAllocator&
                            Allocator() const   { return myAlloc; }

  private:
    MPThread (const MPThread& theOther);
    MPThread& operator= (const MPThread& theOther);

  private:
    // ---------- PRIVATE FIELDS ----------
    const OSD_MProcess                  * myProcessor;
    OSD_MPData                          * mypData;
    unsigned int                        myID;
    Standard_Boolean                    myIsBusy;
    Standard_Size                       myNProcessed;
    Handle_NCollection_IncAllocator     myAlloc;
    volatile Standard_Boolean           myIsActive;
    volatile Standard_Boolean           myIsSleeping;
    friend class OSD_MProcess;
  };

  // ---------- PUBLIC METHODS ----------


  /**
   * Default constructor.
   */
  Standard_EXPORT OSD_MProcess ();

  /**
   * Constructor. Starts the threads immediately.
   */
  Standard_EXPORT OSD_MProcess    (const Standard_Size nThreads,
                                   const Standard_Integer thePriority = 0);

  /**
   * Initialize threads and start them. Used after empty constructor or Close
   */
  Standard_EXPORT void Init       (const Standard_Size nThreads,
                                   const Standard_Integer thePriority = 0);

  /**
   * Destructor.
   */
  Standard_EXPORT ~OSD_MProcess   ();

  //! Query the maximal advisable number of threads for this system.
  static Standard_EXPORT Standard_Integer
                       GetMaxThreads ();

  //! Query the number of opened threads
  inline Standard_Size NThreads   () const      { return myNThreads; }

  //! Query the length of the data queue
  inline Standard_Size NData      () const      { return myNDataAll; }

  //! Query the number of already processed data blocks
  Standard_EXPORT Standard_Size
                       NProcessed () const;

  //! Get the Allocator, to allocate new OSD_MPData instances
  inline const Handle_NCollection_IncAllocator&
                       Allocator  () const      { return myAlloc; }

  /**
   * Put a data block into the queue.
   */
  Standard_EXPORT void AddData    (OSD_MPData * theData);

  /**
   * Extract already processed data block, removing it from the queue.
   * Returns NULL if not available.
   */
  Standard_EXPORT OSD_MPData *
                       TakeData   ();

  /**
   * Wait for the time theMsec then return True if the execution is finished
   * or False if it still continues. If theMillisec <= 0, then the method
   * waits until all threads are exhausted.
   */
  Standard_EXPORT Standard_Boolean
                       Wait       (const Standard_Integer theMsec = 0,
                                   const Standard_Boolean doTasks = 0u);

  /**
   * Avoid future sleeping of the managed threads, this reduces the delays
   * during Open() and Close() calls. Call this method if it is expected to
   * call Open/Close repeatedly for untensive processing.
   */
  inline void          GoAwake    ()
  { myIsSleepy = Standard_False; }

  /**
   * Returns True if a Thread is allowed to sleep. Called from thread functions.
   */
  Standard_EXPORT Standard_Boolean
                       AllowSleep () const;

  /**
   * Allow future sleeping of managed threads. This method shoul be called if
   * there is not expected any intensive operation with repeated Open/Close.
   */
  inline void          GoSleepy   ()
  { myIsSleepy = Standard_True; }

  /**
   * Query the Sleepy state
   */
  inline Standard_Boolean IsSleepy () const
  { return myIsSleepy; }

  /**
   * Wait till the 1st task is started.
   */
  Standard_EXPORT void Open () const;

  /**
   * Closes the processing without stopping the threads.
   */
  Standard_EXPORT void Close ();

  /**
   * Stops the threads.
   */
  Standard_EXPORT void Clear (const Standard_Boolean isWait = Standard_True);

 private:
  OSD_MProcess (const OSD_MProcess& theOther);
  OSD_MProcess& operator- (const OSD_MProcess& theOther);

  static Standard_Boolean processData (MPThread&         aThread,
                                       OSD_MPData*&      theData,
                                       OSD_MPData*       theBaseData,
                                       Standard_Integer& aSleepCounter);

 private:
  // ---------- PRIVATE FIELDS ----------
  Handle_NCollection_IncAllocator       myAlloc;
  Standard_Size                         myNThreads;
  OSD_MPData                            myData;
  MPThread                              * myArrThread;
  Standard_Size                         myNDataAll;
  Standard_Boolean                      myIsSleepy;

  friend class OSD_MPData;
};


#endif

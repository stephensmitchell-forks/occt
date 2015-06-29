// File:      OSD_MPData.hxx
// Created:   29.06.09 09:01
// Author:    Alexander GRIGORIEV
// Copyright: Open Cascade 2009


#ifndef OSD_MPData_HeaderFile
#define OSD_MPData_HeaderFile

#include <NCollection_IncAllocator.hxx>
class OSD_MProcess;

#ifdef WNT
// Disable the warning "operator new unmatched by delete"
#pragma warning (push)
#pragma warning (disable:4291)
#endif

/**
 * Data block processed by MP execution thread (OSD_MPThread)
 */

class OSD_MPData
{
 public:
  // ---------- PUBLIC METHODS ----------
  enum Status {
    StatusEmpty = 0,            ///! empty data block, being initialized
    StatusWait,                 ///! initialized with data, waiting for thread
    StatusRequested,            ///! Data requested by thread but not assigned
    StatusProcess,              ///! captured by thread, being processed
    StatusDone,                 ///! released by thread after success
    DummyData
  };

  //!Empty constructor.
  Standard_EXPORT OSD_MPData ();

  //! Consructor
  Standard_EXPORT OSD_MPData (const OSD_MProcess& theProcessor);

  //! Virtual destructor
  Standard_EXPORT virtual ~OSD_MPData ();

  //!! Allocator-based operator new for dynamic allocations
  inline void* operator new (Standard_Size theSz,
                             const Handle_NCollection_BaseAllocator& theAlloc)
  { return theAlloc->Allocate(theSz); }

  //! Query the unique ID of the data block
  inline Standard_Size          ID       () const { return myID; }

  //! Query the ID of the holding thread
  inline unsigned int           ThreadID () const { return myThreadID; }

  //! Get the thread-owned allocator instance
  Standard_EXPORT const Handle_NCollection_IncAllocator&
                                ThreadAlloc() const;

  //! Processing of the data block, implements the algorithm executed by thread
  Standard_EXPORT virtual void  Process  ();

  //! Retrieving the calculated data (optional). This method is executed
  //! in main thread, from OSD_MProcess::TakeData
  Standard_EXPORT virtual void  PostProcess ();

 private:
  //! Insert a new data object in the queue
  Standard_EXPORT void  AddBefore (OSD_MPData * theAdded);

  //! Remove this data object from the queue.
  Standard_EXPORT void  Detach    ();

  OSD_MPData (const OSD_MPData& theOther);
  OSD_MPData& operator= (const OSD_MPData& theOther);

 private:
  // ---------- PRIVATE FIELDS ----------
  Standard_Size         myID;
  OSD_MPData            * myPrevious;
  OSD_MPData            * myNext;

 protected:
  // ---------- PROTECTED FIELDS ----------
  const OSD_MProcess&   myProcessor;
  volatile unsigned int myThreadID;
  volatile Status       myStatus;

  friend class OSD_MProcess;
};

#ifdef WNT
#pragma warning (pop)
#endif

#endif

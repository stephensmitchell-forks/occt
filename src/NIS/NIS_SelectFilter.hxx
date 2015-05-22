// Created on: 2007-08-20
// Created by: Alexander GRIGORIEV
// Copyright (c) 2007-2014 OPEN CASCADE SAS
//
// This file is part of Open CASCADE Technology software library.
//
// This library is free software; you can redistribute it and / or modify it
// under the terms of the GNU Lesser General Public version 2.1 as published
// by the Free Software Foundation, with special exception defined in the file
// OCCT_LGPL_EXCEPTION.txt. Consult the file LICENSE_LGPL_21.txt included in OCCT
// distribution for complete text of the license and disclaimer of any warranty.
//
// Alternatively, this file may be used under the terms of Open CASCADE
// commercial license or contractual agreement.

#ifndef NIS_SelectFilter_HeaderFile
#define NIS_SelectFilter_HeaderFile

#include <Standard_DefineHandle.hxx>
#include <Standard_Transient.hxx>

class NIS_InteractiveObject;
class Handle_Standard_Type;
class gp_Ax1;

/**
 * Interface for selection filters. It can be used in NIS_InteractiveContext
 * by methods SetFilter and GetFilter.<br>
 * Unlike in AIS, only one filter can be installed in InteractiveContext. If
 * you need more than one filter instance to work together, create a composite
 * NIS_SelectFilter specialization that would hold a list of simpler Filter
 * instances.
 * @ingroup nis_library
 */

class NIS_SelectFilter : public Standard_Transient
{
 public:
  // ---------- PUBLIC METHODS ----------
  //! binary flags (1,2,4,8,...) describing the events for which it responds
  enum Event {
    Indefinite   = 0,
    Pick         = 1,
    DynHilight   = 2,
    Picked       = 4,
    DynHilighted = 8
  };

  /**
   * Empty constructor.
   */
  inline NIS_SelectFilter () : myEventFlags (Pick | DynHilight) {}

  /**
   * Set the combination of events as bit flags defined in enum Event.
   */ 
  inline void SetEventFlags (const unsigned int theFlags)
  { myEventFlags = theFlags; }

  /**
   * Indicate that the given NIS_InteractiveObject passes the Filter.
   * @return
   *   True - theObject is kept in the Selection, False - excluded from it.
   */
  virtual Standard_Boolean
        IsOk    (const NIS_InteractiveObject * theObject,
                 const Event                   theEvent = Indefinite) const = 0;

  /**
   * Indicate that the given NIS_InteractiveObject passes the Filter.
   * This method is called during a selection by mouse click. If it returns
   * True then the normal hilighting is performed. Otherwise the object
   * is skipped from hilighted selection but this method may perform some
   * special actions, if redefined. the default implementation just calls
   * the previous method IsOk.
   * @param theObject
   *   Tested object.
   * @param theAxis
   *   Selection ray
   * @param theOver
   *   Thickness of the selecting ray
   * @param theEvent
   *   Event type that invokes this method.
   * @return
   *   True - theObject is kept in the Selection, False - excluded from it.
   */
  Standard_EXPORT virtual Standard_Boolean
        IsOk    (const NIS_InteractiveObject * theObject,
                 const gp_Ax1&                 theAxis,
                 const Standard_Real           theOver,
                 const Event                   theEvent = Indefinite) const;

  /**
   * Check if the type of InteractiveObject is allowed for selection.
   * Default implementation returns always True. 
   * @return
   *   True if objects of the given Type should be checked, False if such
   *   objects are excluded from Selection before any checking.
   */ 
  Standard_EXPORT virtual Standard_Boolean
        ActsOn  (const Handle_Standard_Type& theType)           const;


 protected:
  // ---------- PROTECTED METHODS ----------

  unsigned int myEventFlags;

 public:
// Declaration of CASCADE RTTI
DEFINE_STANDARD_RTTI (NIS_SelectFilter)
};

// Definition of HANDLE object using Standard_DefineHandle.hxx
DEFINE_STANDARD_HANDLE (NIS_SelectFilter, Standard_Transient)

#endif

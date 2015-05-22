// File:      NIS_ViewData.hxx
// Created:   11.05.12 17:26
// Author:    Alexander GRIGORIEV
// Copyright: Open Cascade 2012


#ifndef NIS_ViewData_HeaderFile
#define NIS_ViewData_HeaderFile

#include <Standard_Transient.hxx>
#include <Standard_DefineHandle.hxx>
class NIS_View;

/**
 * Data belonging to particular instances of NIS_View, to extend the basic features of NIS_View.
 * There may be several data objects per View, distinguished by their names
 * (method NIS_View::GetViewData)
 * @see NIS_View
 * @ingroup nis_library
 */

class NIS_ViewData : public Standard_Transient
{
 public:
  // ---------- PUBLIC METHODS ----------


  /**
   * Empty constructor.
   */
  NIS_ViewData () : myView (0L) {}

  /**
   * Method that is called on every refresh cycle of NIS_View.
   */
  Standard_EXPORT virtual void Init () = 0;

  /**
   * Query the host view.
   */
  inline NIS_View* GetView () const
  { return myView; }

 private:
  NIS_View*  myView;

 public:
// Declaration of CASCADE RTTI
DEFINE_STANDARD_RTTI (NIS_ViewData)
  friend class NIS_View;
};

// Definition of HANDLE object using Standard_DefineHandle.hxx
DEFINE_STANDARD_HANDLE (NIS_ViewData, Standard_Transient)


#endif

// File generated by CPPExt (Transient)
//
//                     Copyright (C) 1991,1995 by
//  
//                      MATRA DATAVISION, FRANCE
//  
// This software is furnished in accordance with the terms and conditions
// of the contract and with the inclusion of the above copyright notice.
// This software or any other copy thereof may not be provided or otherwise
// be made available to any other person. No title to an ownership of the
// software is hereby transferred.
//  
// At the termination of the contract, the software and all copies of this
// software must be deleted.

#ifndef _Handle_TOcaf_Application_HeaderFile
#define _Handle_TOcaf_Application_HeaderFile

#ifndef _Standard_Macro_HeaderFile
#include <Standard_Macro.hxx>
#endif

#ifndef _Handle_TDocStd_Application_HeaderFile
#include <Handle_TDocStd_Application.hxx>
#endif

class Standard_Transient;
class Handle_Standard_Type;
class Handle(TDocStd_Application);
class TOcaf_Application;

Standard_EXPORT Handle_Standard_Type& STANDARD_TYPE(TOcaf_Application);


class Handle(TOcaf_Application) : public Handle(TDocStd_Application) {
  public:
    Handle(TOcaf_Application)():Handle(TDocStd_Application)() {} 
    Handle(TOcaf_Application)(const Handle(TOcaf_Application)& aHandle) : Handle(TDocStd_Application)(aHandle) 
     {
     }

    Handle(TOcaf_Application)(const TOcaf_Application* anItem) : Handle(TDocStd_Application)((TDocStd_Application *)anItem) 
     {
     }

    Handle(TOcaf_Application)& operator=(const Handle(TOcaf_Application)& aHandle)
     {
      Assign(aHandle.Access());
      return *this;
     }

    Handle(TOcaf_Application)& operator=(const TOcaf_Application* anItem)
     {
      Assign((Standard_Transient *)anItem);
      return *this;
     }

    TOcaf_Application* operator->() 
     {
      return (TOcaf_Application *)ControlAccess();
     }

    TOcaf_Application* operator->() const 
     {
      return (TOcaf_Application *)ControlAccess();
     }

   Standard_EXPORT ~Handle(TOcaf_Application)();
 
   Standard_EXPORT static const Handle(TOcaf_Application) DownCast(const Handle(Standard_Transient)& AnObject);
};
#endif
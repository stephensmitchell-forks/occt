// Created on: 1995-12-01
// Created by: EXPRESS->CDL V0.2 Translator
// Copyright (c) 1995-1999 Matra Datavision
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

#ifndef _StepBasic_ApprovalPersonOrganization_HeaderFile
#define _StepBasic_ApprovalPersonOrganization_HeaderFile

#include <Standard.hxx>
#include <Standard_Type.hxx>

#include <StepBasic_PersonOrganizationSelect.hxx>
#include <MMgt_TShared.hxx>
class StepBasic_Approval;
class StepBasic_ApprovalRole;
class StepBasic_PersonOrganizationSelect;


class StepBasic_ApprovalPersonOrganization;
DEFINE_STANDARD_HANDLE(StepBasic_ApprovalPersonOrganization, MMgt_TShared)


class StepBasic_ApprovalPersonOrganization : public MMgt_TShared
{

public:

  
  //! Returns a ApprovalPersonOrganization
  Standard_EXPORT StepBasic_ApprovalPersonOrganization();
  
  Standard_EXPORT virtual void Init (const StepBasic_PersonOrganizationSelect& aPersonOrganization, const Handle(StepBasic_Approval)& aAuthorizedApproval, const Handle(StepBasic_ApprovalRole)& aRole);
  
  Standard_EXPORT void SetPersonOrganization (const StepBasic_PersonOrganizationSelect& aPersonOrganization);
  
  Standard_EXPORT StepBasic_PersonOrganizationSelect PersonOrganization() const;
  
  Standard_EXPORT void SetAuthorizedApproval (const Handle(StepBasic_Approval)& aAuthorizedApproval);
  
  Standard_EXPORT Handle(StepBasic_Approval) AuthorizedApproval() const;
  
  Standard_EXPORT void SetRole (const Handle(StepBasic_ApprovalRole)& aRole);
  
  Standard_EXPORT Handle(StepBasic_ApprovalRole) Role() const;




  DEFINE_STANDARD_RTTIEXT(StepBasic_ApprovalPersonOrganization,MMgt_TShared)

protected:




private:


  StepBasic_PersonOrganizationSelect personOrganization;
  Handle(StepBasic_Approval) authorizedApproval;
  Handle(StepBasic_ApprovalRole) role;


};







#endif // _StepBasic_ApprovalPersonOrganization_HeaderFile

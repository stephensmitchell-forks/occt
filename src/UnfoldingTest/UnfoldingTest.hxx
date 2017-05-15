// File:	UnfoldingTest.cdl
// Created:	Tue Jul 22 18:15:41 2008
// Author:	Sergey KHROMOV
//		<skv@dimox>
//-Copyright:	 Matra Datavision 2008

#ifndef _UnfoldingTest_HeaderFile
#define _UnfoldingTest_HeaderFile

#include <Standard.hxx>
#include <Standard_DefineAlloc.hxx>
#include <Standard_Handle.hxx>

#include <Draw_Interpretor.hxx>


//! This package defines a set of Draw commands for testing of
//! functionality of the package Unfolding.
class UnfoldingTest 
{
public:

  DEFINE_STANDARD_ALLOC

  
  //! Adds Draw commands to the draw interpretor.
  Standard_EXPORT static void Commands (Draw_Interpretor& theDI);
  
  //! Plugin entry point function.
  Standard_EXPORT static void Factory (Draw_Interpretor& theDI);




protected:





private:





};







#endif // _UnfoldingTest_HeaderFile

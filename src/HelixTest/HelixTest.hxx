// File:	HelixTest.cdl

#ifndef _HelixTest_HeaderFile
#define _HelixTest_HeaderFile

#include <Standard.hxx>
#include <Standard_DefineAlloc.hxx>
#include <Standard_Handle.hxx>

#include <Draw_Interpretor.hxx>



class HelixTest 
{
public:

  DEFINE_STANDARD_ALLOC

  
  Standard_EXPORT static void AllCommands (Draw_Interpretor& aDI);
  
  Standard_EXPORT static void HelixCommands (Draw_Interpretor& aDI);
  
  Standard_EXPORT static void Factory (Draw_Interpretor& aDI);




protected:





private:





};







#endif // _HelixTest_HeaderFile

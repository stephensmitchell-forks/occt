// File:	HelixTest.cxx


#include <HelixTest.hxx>

#include <stdio.h>
//=======================================================================
//function : AllCommands
//purpose  : 
//=======================================================================
  void HelixTest::AllCommands(Draw_Interpretor& theCommands)
{
  static Standard_Boolean done = Standard_False;
  if (done) return;
  done = Standard_True;
  //
  HelixTest::HelixCommands (theCommands);
}
//=======================================================================
//function : Factory
//purpose  : 
//=======================================================================
  void HelixTest::Factory(Draw_Interpretor& theCommands)
{
  static Standard_Boolean FactoryDone = Standard_False;
  if (FactoryDone) return;

  FactoryDone = Standard_True;

  HelixTest::AllCommands(theCommands);
  Printf(" Helix Plugin is loaded\n");
}
#include <Draw_PluginMacro.hxx>
DPLUGIN(HelixTest)



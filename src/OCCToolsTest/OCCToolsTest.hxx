#ifndef _PFC_HeaderFile
#define _PFC_HeaderFile

class Draw_Interpretor;

#include <Standard_TypeDef.hxx>

//! The OCCToolsTest package provides the plug-in for DRAW Test Harness,
//! which contains a set of DRAW commands wrapping OCCTools functions.
class OCCToolsTest  {
public:

  //! Defines all OCCToolsTest commands.
  Standard_EXPORT static  void Commands(Draw_Interpretor& DI);

  //! Defines the weld-bead plugin. <br>
  Standard_EXPORT static  void Factory(Draw_Interpretor& DI);
};
#endif

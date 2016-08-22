// File:	HelixGeom_BuilderHelix.cdl

#ifndef _HelixGeom_BuilderHelix_HeaderFile
#define _HelixGeom_BuilderHelix_HeaderFile

#include <Standard.hxx>
#include <Standard_DefineAlloc.hxx>
#include <Standard_Handle.hxx>

#include <gp_Ax2.hxx>
#include <HelixGeom_BuilderHelixGen.hxx>
class gp_Ax2;


//! Upper level class for geometrical algorithm of building
//! helix curves using arbitrary axis
class HelixGeom_BuilderHelix  : public HelixGeom_BuilderHelixGen
{
public:

  DEFINE_STANDARD_ALLOC

  
  //! Empty  constructor
  Standard_EXPORT HelixGeom_BuilderHelix();
Standard_EXPORT virtual ~HelixGeom_BuilderHelix();
  
  //! Sets coordinate axes for helix
  Standard_EXPORT void SetPosition (const gp_Ax2& aAx2);
  
  //! Gets coordinate axes for helix
  Standard_EXPORT const gp_Ax2& Position() const;
  
  //! Performs  calculations
  Standard_EXPORT virtual void Perform() Standard_OVERRIDE;




protected:



  gp_Ax2 myPosition;


private:





};







#endif // _HelixGeom_BuilderHelix_HeaderFile

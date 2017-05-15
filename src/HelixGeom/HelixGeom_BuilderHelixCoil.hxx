// File:	HelixGeom_BuilderHelixCoil.cdl

#ifndef _HelixGeom_BuilderHelixCoil_HeaderFile
#define _HelixGeom_BuilderHelixCoil_HeaderFile

#include <Standard.hxx>
#include <Standard_DefineAlloc.hxx>
#include <Standard_Handle.hxx>

#include <HelixGeom_BuilderHelixGen.hxx>


//! Implementation  of  algorithm  for  building  helix  coil  with
//! axis  OZ
class HelixGeom_BuilderHelixCoil  : public HelixGeom_BuilderHelixGen
{
public:

  DEFINE_STANDARD_ALLOC

  
  //! Empty  constructor
  Standard_EXPORT HelixGeom_BuilderHelixCoil();
Standard_EXPORT virtual ~HelixGeom_BuilderHelixCoil();
  
  //! Performs  calculations
  Standard_EXPORT virtual void Perform() Standard_OVERRIDE;




protected:





private:





};







#endif // _HelixGeom_BuilderHelixCoil_HeaderFile

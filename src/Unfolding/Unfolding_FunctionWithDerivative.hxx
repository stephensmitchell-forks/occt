// File:	Unfolding_FunctionWithDerivative.cdl
// Created:	Fri Sep  5 17:44:29 2008
// Author:	Mikhail KLOKOV
//		<mkk@kurox>
//-Copyright:	Open CASCADE 2008

#ifndef _Unfolding_FunctionWithDerivative_HeaderFile
#define _Unfolding_FunctionWithDerivative_HeaderFile

#include <Standard.hxx>
#include <Standard_DefineAlloc.hxx>
#include <Standard_Handle.hxx>

#include <TColgp_Array1OfXY.hxx>
#include <gp_Trsf2d.hxx>
#include <gp_Dir2d.hxx>
#include <Standard_Real.hxx>
#include <math_FunctionWithDerivative.hxx>
#include <Standard_Boolean.hxx>
#include <Standard_Integer.hxx>
#include <TColgp_Array1OfXY.hxx>

class gp_Dir2d;
class gp_Trsf2d;



class Unfolding_FunctionWithDerivative  : public math_FunctionWithDerivative
{
public:

  DEFINE_STANDARD_ALLOC

  
  Standard_EXPORT Unfolding_FunctionWithDerivative(const TColgp_Array1OfXY& theMaster, const TColgp_Array1OfXY& theSlave, const gp_Dir2d& theDir, const gp_Trsf2d& theTrsf);
  
  //! Computes the value <F>of the function for the variable <X>.
  //! Returns True if the calculation were successfully done,
  //! False otherwise.
  Standard_EXPORT virtual Standard_Boolean Value (const Standard_Real X, Standard_Real& F) Standard_OVERRIDE;
  
  //! Computes the derivative <D> of the function
  //! for the variable <X>.
  //! Returns True if the calculation were successfully done,
  //! False otherwise.
  Standard_EXPORT virtual Standard_Boolean Derivative (const Standard_Real X, Standard_Real& D) Standard_OVERRIDE;
  
  //! Computes the value <F> and the derivative <D> of the
  //! function for the variable <X>.
  //! Returns True if the calculation were successfully done,
  //! False otherwise.
  Standard_EXPORT virtual Standard_Boolean Values (const Standard_Real X, Standard_Real& F, Standard_Real& D) Standard_OVERRIDE;
  
  Standard_EXPORT virtual Standard_Integer GetStateNumber() Standard_OVERRIDE;
  
  Standard_EXPORT Standard_Real Area() const;




protected:





private:



  TColgp_Array1OfXY myMasterPolyLine;
  TColgp_Array1OfXY mySlavePolyLine;
  gp_Trsf2d myTrsf;
  gp_Dir2d myShiftDir;
  Standard_Real myShift;
  Standard_Real myArea;


};







#endif // _Unfolding_FunctionWithDerivative_HeaderFile

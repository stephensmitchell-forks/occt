// File:	Unfolding_FaceDataMapHasher.cdl
// Created:	Fri Sep 19 16:52:05 2008
// Author:	Sergey KHROMOV
//		<skv@kurox>
//-Copyright:	 Matra Datavision 2008

#ifndef _Unfolding_FaceDataMapHasher_HeaderFile
#define _Unfolding_FaceDataMapHasher_HeaderFile

#include <Standard.hxx>
#include <Standard_DefineAlloc.hxx>
#include <Standard_Handle.hxx>

#include <Standard_Integer.hxx>
#include <Standard_Boolean.hxx>
class Unfolding_FaceDataContainer;


//! Hash tool, used for generating maps of face data containers.
class Unfolding_FaceDataMapHasher 
{
public:

  DEFINE_STANDARD_ALLOC

  
  //! Returns a HasCode value  for  the  Key <K>  in the
  //! range 0..theUpper.
    static Standard_Integer HashCode (const Handle(Unfolding_FaceDataContainer)& theKey, const Standard_Integer theUpper);
  
  //! Returns True  when the two  keys are the same. Two
  //! same  keys  must   have  the  same  hashcode,  the
  //! contrary is not necessary.
    static Standard_Boolean IsEqual (const Handle(Unfolding_FaceDataContainer)& theKey1, const Handle(Unfolding_FaceDataContainer)& theKey2);




protected:





private:





};


#include <Unfolding_FaceDataMapHasher.lxx>





#endif // _Unfolding_FaceDataMapHasher_HeaderFile

// File:	Unfolding.cdl
// Created:	Tue Jul 22 12:48:05 2008
// Author:	Sergey KHROMOV
//		<skv@dimox>
//-Copyright:	Open CASCADE 2008

#ifndef Unfolding_IndexedMapOfFaceDataContainer_HeaderFile
#define Unfolding_IndexedMapOfFaceDataContainer_HeaderFile

#include <Unfolding_FaceDataContainer.hxx>
#include <Unfolding_FaceDataMapHasher.hxx>
#include <NCollection_IndexedMap.hxx>

typedef NCollection_IndexedMap<Handle(Unfolding_FaceDataContainer),Unfolding_FaceDataMapHasher> Unfolding_IndexedMapOfFaceDataContainer;


#endif

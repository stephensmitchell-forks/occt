// File:      NIS_IndexLists.cxx
// Created:   19.07.2011 13:42
// Author:    
// Copyright: Open Cascade 2011

#include <NIS_IndexLists.hxx>
#include <NCollection_BaseAllocator.hxx>
#include <Standard_OutOfRange.hxx>

/**
 * Compute the size in bytes of an index array.
 */ 
static inline Standard_Size    NBytesInd  (const Standard_Size nInd,
                                           const unsigned int  theIndType)
{
  static const Standard_Size nBytes[] = { 1, 2, 4 };
  return nInd * nBytes[theIndType];
}

//=======================================================================
//function : Constructor
//purpose  : 
//=======================================================================

NIS_IndexLists::NIS_IndexLists()
  : mypIndexes          (0L),
    myNbArrays          (0u),
    myIndexType         (2u)
{
}

//=======================================================================
//function : Clear()
//purpose  : 
//=======================================================================

void NIS_IndexLists::Clear (NCollection_BaseAllocator * theAlloc)
{
  if (theAlloc && theAlloc != UndefinedHandleAddress)
  {
    if (myNbArrays) {
      for (unsigned int i = 0; i < myNbArrays; i++)
        theAlloc->Free(mypIndexes[i]);
      theAlloc->Free(mypIndexes);
      myNbArrays = 0u;
      mypIndexes = 0L;
    }
  }
}

//=======================================================================
//function : Clone()
//purpose  : 
//=======================================================================

void NIS_IndexLists::Clone (const Handle_NCollection_BaseAllocator& theAlloc,
                            NIS_IndexLists&                      theDest) const
{
  theDest.myNbArrays = myNbArrays;
  theDest.myIndexType = myIndexType;
  if (myNbArrays) {
    const Standard_Size nBytes = sizeof(Standard_Integer *) * myNbArrays;
    theDest.mypIndexes = static_cast<Standard_Integer **>
      (theAlloc->Allocate(nBytes));
    for (unsigned int i = 0; i < myNbArrays; i++) {
      const Standard_Size nIdx = NArrayIndexes(i);
      const Standard_Size nBytes = NBytesInd(nIdx, myIndexType)+4;
      theDest.mypIndexes[i] = static_cast <Standard_Integer *>
        (theAlloc->Allocate (nBytes));
      memcpy(theDest.mypIndexes[i], mypIndexes[i], nBytes);
    }
  }
}

//=======================================================================
//function : InitArray 
//purpose  : 
//=======================================================================

void NIS_IndexLists::InitArray (const unsigned int          theIndexType,
                                const Standard_Integer      theNLists,
                                NCollection_BaseAllocator * theAlloc)
{
  if (theNLists > 0)
  {
    Clear(theAlloc);
    myIndexType = theIndexType;
    myNbArrays = static_cast<unsigned int>(theNLists);
    mypIndexes = static_cast<Standard_Integer **>
      (theAlloc->Allocate(sizeof(Standard_Integer *) * theNLists));
  }
}

//=======================================================================
//function : SetRealNumbersOfArrays 
//purpose  : 
//=======================================================================

void NIS_IndexLists::SetRealNumbersOfArrays (const Standard_Integer theNLists)
{
  if (theNLists < 0)
    return;
  if (theNLists > static_cast<Standard_Integer>(myNbArrays))
    Standard_OutOfRange::Raise ("NIS_IndexLists::SetRealNumbersOfArrays");

  myNbArrays = static_cast<unsigned int>(theNLists);
}

//=======================================================================
//function : SetArrayIndex 
//purpose  : 
//=======================================================================

void NIS_IndexLists::SetArrayIndex (const Standard_Size    theIndArray,
                                    const Standard_Size    theIndInArray,
                                    const Standard_Integer theIndex)
{
  Standard_OutOfRange_Raise_if (theIndArray >= myNbArrays,
                                "NIS_IndexLists::SetArrayIndex");

  Standard_Integer * aArray  = mypIndexes[theIndArray];
  switch (myIndexType) {
  case 0: // 8bit
  {
    Standard_OutOfRange_Raise_if (theIndInArray >= aNIdx,
                                  "NIS_IndexLists::SetArrayIndex");
    unsigned char * pInd =
      reinterpret_cast<unsigned char *>(aArray) + (theIndInArray + 4);
    pInd[0] = static_cast<unsigned char>(theIndex);   
  }
  break;
  case 1: // 16bit
  {
    Standard_OutOfRange_Raise_if (theIndInArray >= aNIdx,
                                  "NIS_IndexLists::SetArrayIndex");
    unsigned short * pInd =
      reinterpret_cast<unsigned short *>(aArray) + (theIndInArray + 2);
    pInd[0] = static_cast<unsigned short>(theIndex);
  }
  break;
  default: // 32bit
    Standard_OutOfRange_Raise_if (static_cast<int>(theIndInArray) >= aArray[0],
                                  "NIS_IndexLists::SetArrayIndex");
    aArray[theIndInArray + 1] = theIndex;
  }
}

//=======================================================================
//function : SetArray 
//purpose  : 
//=======================================================================

void NIS_IndexLists::SetArray (const Standard_Size         theIndArray,
                               const Standard_Integer      theNbIndexes,
                               NCollection_BaseAllocator * theAlloc)
{
  Standard_OutOfRange_Raise_if (theIndArray >= myNbArrays,
                                "NIS_IndexLists::SetArray");
  switch (myIndexType) {
    case 0: // 8bit
    {
      unsigned char * anArray  = static_cast <unsigned char *>
        (theAlloc->Allocate (sizeof(unsigned char) * (theNbIndexes+4)));
      mypIndexes[theIndArray] = reinterpret_cast<Standard_Integer *> (anArray);
      *(reinterpret_cast<unsigned int*>(anArray)) = theNbIndexes;
    }
    break;
    case 1: // 16bit
    {
      unsigned short * anArray  = static_cast <unsigned short *>
        (theAlloc->Allocate (sizeof(unsigned short) * (theNbIndexes+2)));
      mypIndexes[theIndArray] = reinterpret_cast<Standard_Integer *> (anArray);
      *(reinterpret_cast<unsigned int*>(anArray)) = theNbIndexes;
    }
    break;
    default: // 32bit
    {
      Standard_Integer * anArray  = static_cast <Standard_Integer *>
        (theAlloc->Allocate (sizeof(Standard_Integer) * (theNbIndexes+1)));
      mypIndexes[theIndArray] = anArray;
      anArray[0] = theNbIndexes;
    }
  } 
}

//=======================================================================
//function : SetArray
//purpose  : 
//=======================================================================

void NIS_IndexLists::SetArray (const Standard_Size         theIndArray,
                               const Standard_Integer      theNbIndexes, 
                               const int*                  theIdxList,
                               NCollection_BaseAllocator * theAlloc,
                               const Standard_Integer      theStartIdx)
{
  if (theIdxList)
  {
    SetArray (theIndArray, theNbIndexes, theAlloc);
    if (mypIndexes)
    {
      for (Standard_Integer i = 0; i < theNbIndexes; i++)
      {
        Standard_Integer aN = theIdxList[i] + theStartIdx;   
        SetArrayIndex (theIndArray, i, aN);
      }
    }
  }
}

//=======================================================================
//function : ArrayIndex
//purpose  : 
//=======================================================================

Standard_Integer NIS_IndexLists::ArrayIndex
                                (const Standard_Size    theIndArray,
                                 const Standard_Integer theIndInArray) const
{
  Standard_Integer aResult(-1);
  Standard_OutOfRange_Raise_if (theIndArray >= myNbArrays,
                                "NIS_IndexLists::ArrayIndex");
  const Standard_Integer * aArray  = mypIndexes[theIndArray];
  switch (myIndexType) {
    case 0: // 8bit
    {
      const unsigned char * pInd =
        reinterpret_cast<const unsigned char *>(aArray);
      Standard_OutOfRange_Raise_if (theIndInArray >= pInd[0],
                                    "NIS_IndexLists::ArrayIndex");
      aResult = static_cast<Standard_Integer>(pInd[theIndInArray + 4]);
    }
    break;
    case 1: // 16bit
    {
      const unsigned short * pInd =
        reinterpret_cast<const unsigned short *>(aArray);
      Standard_OutOfRange_Raise_if (theIndInArray >= pInd[0],
                                    "NIS_IndexLists::ArrayIndex");
      aResult = static_cast<Standard_Integer>(pInd[theIndInArray + 2]);
    }
    break;
    default: // 32bit
    {
      Standard_OutOfRange_Raise_if (theIndInArray >= aArray[0],
                                    "NIS_IndexLists::ArrayIndex");
      aResult = aArray[theIndInArray + 1];
    }
  }
  return aResult;
}

//=======================================================================
//function : NArraysIndexes
//purpose  : 
//=======================================================================

Standard_Size NIS_IndexLists::NArrayIndexes
                                (const Standard_Integer theIndArray) const
{
  Standard_Size aResult(0);
  Standard_OutOfRange_Raise_if ((theIndArray >=
                                 static_cast<Standard_Integer>(myNbArrays)),
                                "NIS_IndexLists::NArraysIndexes");
  const Standard_Integer * aArray = mypIndexes[theIndArray];
  aResult = * (reinterpret_cast<const unsigned int *>(aArray));
  return aResult;
}

//=======================================================================
//function : ArraysIndexes
//purpose  : 
//=======================================================================

void * NIS_IndexLists::ArrayIndexes(const Standard_Size theIndArray)const
{
  Standard_OutOfRange_Raise_if (theIndArray >= myNbArrays,
                                "NIS_IndexLists::ArraysIndexes");
  void * anArray;
  if (myIndexType == 0)
    anArray = reinterpret_cast<unsigned char *>(mypIndexes[theIndArray])+4;
  else if (myIndexType == 1)
    anArray = reinterpret_cast<unsigned short *>(mypIndexes[theIndArray])+2;
  else
    anArray = mypIndexes[theIndArray]+1;          

  return anArray;
}


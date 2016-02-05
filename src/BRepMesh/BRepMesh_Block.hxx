// Created on: 2015-11-27
// Created by: Danila ULYANOV
// Copyright (c) 2015 OPEN CASCADE SAS
//
// This file is part of Open CASCADE Technology software library.
//
// This library is free software; you can redistribute it and/or modify it under
// the terms of the GNU Lesser General Public License version 2.1 as published
// by the Free Software Foundation, with special exception defined in the file
// OCCT_LGPL_EXCEPTION.txt. Consult the file LICENSE_LGPL_21.txt included in OCCT
// distribution for complete text of the license and disclaimer of any warranty.
//
// Alternatively, this file may be used under the terms of Open CASCADE
// commercial license or contractual agreement.

#ifndef __BLOCK_H__
#define __BLOCK_H__

#include <stdlib.h>

template <class Type> class Block
{
public:

  Block (int theSize, void (*theErrFunction) (char*) = NULL)
  {
    myFirst = myLast = NULL;
    myBlockSize = theSize;
    myErrorFun = theErrFunction;
  }

  ~Block()
  {
    while (myFirst)
    {
      BlockStruct* aNext = myFirst->Next;
      delete myFirst;
      myFirst = aNext;
    }
  }

  Type* New (int theNum = 1)
  {
    Type* aLast;

    if (!myLast || myLast->Current + theNum > myLast->Last)
    {
      if (myLast && myLast->Next) { myLast = myLast->Next; }
      else
      {
        BlockStruct* aNext = (BlockStruct*) new char [sizeof (BlockStruct) + (myBlockSize-1) *sizeof (Type)];

        if (!aNext) { if (myErrorFun) { (*myErrorFun) ("Not enough memory!"); } exit (1); }

        if (myLast) { myLast->Next = aNext; }
        else { myFirst = aNext; }

        myLast = aNext;
        myLast->Current = & (myLast->Data[0]);
        myLast->Last = myLast->Current + myBlockSize;
        myLast->Next = NULL;
      }
    }

    aLast = myLast->Current;
    myLast->Current += theNum;
    return aLast;
  }

  Type* ScanFirst()
  {
    myScanCurrentBlock = myFirst;

    if (!myScanCurrentBlock) { return NULL; }

    myScanCurrentData = & (myScanCurrentBlock->Data[0]);
    return myScanCurrentData++;
  }

  Type* ScanNext()
  {
    if (myScanCurrentData >= myScanCurrentBlock->Current)
    {
      myScanCurrentBlock = myScanCurrentBlock->Next;

      if (!myScanCurrentBlock) { return NULL; }

      myScanCurrentData = & (myScanCurrentBlock->Data[0]);
    }

    return myScanCurrentData++;
  }

  void Reset()
  {
    BlockStruct* aBlock;

    if (!myFirst) { return; }

    for (aBlock = myFirst; ; aBlock = aBlock->Next)
    {
      aBlock->Current = & (aBlock->Data[0]);

      if (aBlock == myLast) { break; }
    }

    myLast = myFirst;
  }

private:

  typedef struct BlockSt
  {
    Type* Current;
    Type* Last;
    struct BlockSt* Next;
    Type Data[1];
  } BlockStruct;

  int   myBlockSize;
  BlockStruct* myFirst;
  BlockStruct* myLast;

  BlockStruct* myScanCurrentBlock;
  Type*  myScanCurrentData;

  void (*myErrorFun) (char*);
};

template <class Type> class DBlock
{
public:

  DBlock (int theSize, void (*theErrFunction) (char*) = NULL) { myFirst = NULL; myFirstFree = NULL; myBLockSize = theSize; myErrFun = theErrFunction; }

  ~DBlock() { while (myFirst) { BlockStruct* aNext = myFirst->Next; delete myFirst; myFirst = aNext; } }

  Type* New()
  {
    BlockItem* anItem;

    if (!myFirstFree)
    {
      BlockStruct* next = myFirst;
      myFirst = (BlockStruct*) new char [sizeof (BlockStruct) + (myBLockSize-1) *sizeof (BlockItem)];

      if (!myFirst) { if (myErrFun) { (*myErrFun) ("Not enough memory!"); } exit (1); }

      myFirstFree = & (myFirst->Data[0]);

      for (anItem=myFirstFree; anItem<myFirstFree+myBLockSize-1; anItem++)
      { anItem->NextFree = anItem + 1; }

      anItem->NextFree = NULL;
      myFirst->Next = next;
    }

    anItem = myFirstFree;
    myFirstFree = anItem->NextFree;
    return (Type*) anItem;
  }

  void Delete (Type* t)
  {
    ( (BlockItem*) t)->NextFree = myFirstFree;
    myFirstFree = (BlockItem*) t;
  }

private:

  typedef union BlockItemSt
  {
    Type      Item;
    BlockItemSt* NextFree;
  } BlockItem;

  typedef struct BlockSt
  {
    struct BlockSt*      Next;
    BlockItem       Data[1];
  } BlockStruct;

  int     myBLockSize;
  BlockStruct*   myFirst;
  BlockItem* myFirstFree;

  void (*myErrFun) (char*);
};

#endif


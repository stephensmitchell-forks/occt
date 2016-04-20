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

#ifndef _BRepMesh_Block_HeaderFile
#define _BRepMesh_Block_HeaderFile

#include <stdlib.h>
#include <stdexcept>

template <class Type> class Storage
{
public:
  Storage (int theSize)
  {
    m_first = m_last = NULL;
    m_BlockSize = theSize;
  }

  ~Storage()
  {
    while (m_first)
    {
      Block* next = m_first->Next;
      delete[] ( (char*) m_first);
      m_first = next;
    }
  }

  Type* New (int aNum = 1)
  {
    Type* aResult;

    if (!m_last || m_last->Current + aNum > m_last->Last)
    {
      if (m_last && m_last->Next)
      {
        m_last = m_last->Next;
      }

      else
      {
        Block* aNext = (Block*) new char [sizeof (Block) + (m_BlockSize - 1) *sizeof (Type)];

        if (!aNext)
        {
          throw std::runtime_error ("Not enough memory!");
        }

        if (m_last)
        {
          m_last->Next = aNext;
        }

        else
        {
          m_first = aNext;
        }

        m_last = aNext;
        m_last->Current = & (m_last->Data[0]);
        m_last->Last = m_last->Current + m_BlockSize;
        m_last->Next = NULL;
      }
    }

    aResult = m_last->Current;
    m_last->Current += aNum;
    return aResult;
  }

  Type* ScanFirst()
  {
    for (m_scanCurrentBlock = m_first; m_scanCurrentBlock; m_scanCurrentBlock = m_scanCurrentBlock->Next)
    {
      m_scanCurrentData = & (m_scanCurrentBlock->Data[0]);

      if (m_scanCurrentData < m_scanCurrentBlock->Current)
      {
        return m_scanCurrentData ++;
      }
    }

    return NULL;
  }

  Type* ScanNext()
  {
    while (m_scanCurrentData >= m_scanCurrentBlock->Current)
    {
      m_scanCurrentBlock = m_scanCurrentBlock->Next;

      if (!m_scanCurrentBlock)
      {
        return NULL;
      }

      m_scanCurrentData = & (m_scanCurrentBlock->Data[0]);
    }

    return m_scanCurrentData++;
  }

  struct BlockIterator;
  Type* ScanFirst (BlockIterator& anIter)
  {
    for (anIter.ScanCurrentBlock = m_first; anIter.ScanCurrentBlock; anIter.ScanCurrentBlock = anIter.ScanCurrentBlock->Next)
    {
      anIter.ScanCurrentData = & (anIter.ScanCurrentBlock->Data[0]);

      if (anIter.ScanCurrentData < anIter.ScanCurrentBlock->Current)
      {
        return anIter.ScanCurrentData ++;
      }
    }

    return NULL;
  }
  Type* ScanNext (BlockIterator& anIter)
  {
    while (anIter.ScanCurrentData >= anIter.ScanCurrentBlock->Current)
    {
      anIter.ScanCurrentBlock = anIter.ScanCurrentBlock->Next;

      if (!anIter.ScanCurrentBlock)
      {
        return NULL;
      }

      anIter.ScanCurrentData = & (anIter.ScanCurrentBlock->Data[0]);
    }

    return anIter.ScanCurrentData ++;
  }

  void Reset()
  {
    Block* aBlock;

    if (!m_first)
    {
      return;
    }

    for (aBlock = m_first; ; aBlock = aBlock->Next)
    {
      aBlock->Current = & (aBlock->Data[0]);

      if (aBlock == m_last)
      {
        break;
      }
    }

    m_last = m_first;
  }

private:

  typedef struct BlockStruct
  {
    Type* Current, *Last;
    struct BlockStruct* Next;
    Type Data[1];
  } Block;

  int m_BlockSize;
  Block* m_first;
  Block* m_last;
public:

  struct BlockIterator
  {
    Block* ScanCurrentBlock;
    Type* ScanCurrentData;
  };

private:
  Block* m_scanCurrentBlock;
  Type* m_scanCurrentData;
};

template <class Type> class DataBlock
{
public:
  DataBlock (int size)
  {
    m_first = NULL;
    m_firstFree = NULL;
    m_blockSize = size;
  }

  ~DataBlock()
  {
    while (m_first)
    {
      Block* next = m_first->Next;
      delete[] ( (char*) m_first);
      m_first = next;
    }
  }

  Type* New()
  {
    BlockItem* anItem;

    if (!m_firstFree)
    {
      Block* next = m_first;
      m_first = (Block*) new char [sizeof (Block) + (m_blockSize - 1) *sizeof (BlockItem)];

      if (!m_first)
      {
        throw std::runtime_error ("Not enough memory!");
      }

      m_firstFree = & (m_first->Data[0]);

      for (anItem = m_firstFree; anItem < m_firstFree + m_blockSize - 1; anItem++)
      {
        anItem->NextFree = anItem + 1;
      }

      anItem->NextFree = NULL;
      m_first->Next = next;
    }

    anItem = m_firstFree;
    m_firstFree = anItem->NextFree;
    return (Type*) anItem;
  }

  void Delete (Type* t)
  {
    ( (BlockItem*) t)->NextFree = m_firstFree;
    m_firstFree = (BlockItem*) t;
  }

private:

  typedef union BlockItemStruct
  {
    Type Val;
    BlockItemStruct* NextFree;
  } BlockItem;

  typedef struct BlockStruct
  {
    struct BlockStruct* Next;
    BlockItem Data[1];
  } Block;

  int m_blockSize;
  Block* m_first;
  BlockItem* m_firstFree;
};

#endif // _BRepMesh_Block_HeaderFile


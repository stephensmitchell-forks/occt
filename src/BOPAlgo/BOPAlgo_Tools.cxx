// Created by: Peter KURNEV
// Copyright (c) 1999-2012 OPEN CASCADE SAS
//
// The content of this file is subject to the Open CASCADE Technology Public
// License Version 6.5 (the "License"). You may not use the content of this file
// except in compliance with the License. Please obtain a copy of the License
// at http://www.opencascade.org and read it completely before using this file.
//
// The Initial Developer of the Original Code is Open CASCADE S.A.S., having its
// main offices at: 1, place des Freres Montgolfier, 78280 Guyancourt, France.
//
// The Original Code and all software distributed under the License is
// distributed on an "AS IS" basis, without warranty of any kind, and the
// Initial Developer hereby disclaims all such warranties, including without
// limitation, any warranties of merchantability, fitness for a particular
// purpose or non-infringement. Please see the License for the specific terms
// and conditions governing the rights and limitations under the License.

#include <BOPAlgo_Tools.ixx>
#include <BOPDS_MapOfPaveBlock.hxx>
#include <BOPDS_CommonBlock.hxx>
#include <BOPDS_DataMapOfPaveBlockListOfPaveBlock.hxx>

//=======================================================================
//function : MakeBlocksCnx
//purpose  : 
//=======================================================================
  void BOPAlgo_Tools::MakeBlocksCnx(const BOPCol_DataMapOfIntegerListOfInteger& aMILI,
                                    BOPCol_DataMapOfIntegerListOfInteger& aMBlocks,
                                    Handle(NCollection_BaseAllocator)& aAllocator)
{
  Standard_Integer aNbV, nV, aNbVS, nVP, nVx, aNbVP, k;
  BOPCol_DataMapIteratorOfDataMapOfIntegerListOfInteger aItMILI;
  BOPCol_MapIteratorOfMapOfInteger aItMVP;
  BOPCol_ListIteratorOfListOfInteger aItLI;
  //
  BOPCol_MapOfInteger aMVS(100, aAllocator);
  BOPCol_MapOfInteger aMEC(100, aAllocator);
  BOPCol_MapOfInteger aMVP(100, aAllocator);
  BOPCol_MapOfInteger aMVAdd(100, aAllocator);
  //
  aNbV=aMILI.Extent();
  //
  aItMILI.Initialize(aMILI);
  for (k=0; aItMILI.More(); aItMILI.Next()) {
    aNbVS=aMVS.Extent();
    if (aNbVS==aNbV) {
      break;
    }
    //
    nV=aItMILI.Key();
    if (aMVS.Contains(nV)){
      continue;
    }
    aMVS.Add(nV);
    //
    aMEC.Clear();
    aMVP.Clear();
    aMVAdd.Clear();
    //
    aMVP.Add(nV);
    while(1) {
      aNbVP=aMVP.Extent();
      aItMVP.Initialize(aMVP);
      for (; aItMVP.More(); aItMVP.Next()) {
        nVP=aItMVP.Value();
        const BOPCol_ListOfInteger& aLV=aMILI.Find(nVP);
        aItLI.Initialize(aLV);
        for (; aItLI.More(); aItLI.Next()) {
          nVx=aItLI.Value();
          if (aMEC.Contains(nVx)) {
            continue;
          }
          //
          aMVS.Add(nVx);
          aMEC.Add(nVx);
          aMVAdd.Add(nVx);
        }
      }
      //
      aNbVP=aMVAdd.Extent();
      if (!aNbVP) {
        break; // from while(1)
      }
      //
      aMVP.Clear();
      aItMVP.Initialize(aMVAdd);
      for (; aItMVP.More(); aItMVP.Next()) {
        aMVP.Add(aItMVP.Value());
      }
      aMVAdd.Clear();
    }//while(1) {
    //
    BOPCol_ListOfInteger aLIx(aAllocator);
    //
    //aLIx.Append(nV);
    aItMVP.Initialize(aMEC);
    for (; aItMVP.More(); aItMVP.Next()) {
      nVx=aItMVP.Value();
      aLIx.Append(nVx);
    }
    //
    aMBlocks.Bind(k, aLIx);
    ++k;
  }//for (; aItMILI.More(); aItMILI.Next()) {
  aMVAdd.Clear();
  aMVP.Clear();
  aMEC.Clear();
  aMVS.Clear();
}
//=======================================================================
//function : FillMap
//purpose  : 
//=======================================================================
  void BOPAlgo_Tools::FillMap(const Standard_Integer n1,
                              const Standard_Integer n2,
                              BOPCol_DataMapOfIntegerListOfInteger& aMILI,
                              Handle(NCollection_BaseAllocator)& aAllocator)
{
  if (aMILI.IsBound(n1)) {
    BOPCol_ListOfInteger& aLI=aMILI.ChangeFind(n1);
    aLI.Append(n2);
  }
  else {
    BOPCol_ListOfInteger aLI(aAllocator);
    aLI.Append(n2);
    aMILI.Bind(n1, aLI);
  }
  if (aMILI.IsBound(n2)) {
    BOPCol_ListOfInteger& aLI=aMILI.ChangeFind(n2);
    aLI.Append(n1);
  }
  else {
    BOPCol_ListOfInteger aLI(aAllocator);
    aLI.Append(n1);
    aMILI.Bind(n2, aLI);
  }
}
//=======================================================================
//function : FillMap
//purpose  : 
//=======================================================================
  void BOPAlgo_Tools::FillMap(const Handle(BOPDS_PaveBlock)& aPB1,
                              const Handle(BOPDS_PaveBlock)& aPB2,
                              BOPDS_DataMapOfPaveBlockListOfPaveBlock& aMPBLPB,
                              Handle(NCollection_BaseAllocator)& aAllocator)
{
  if (aMPBLPB.IsBound(aPB1)) {
    BOPDS_ListOfPaveBlock& aLPB=aMPBLPB.ChangeFind(aPB1);
    aLPB.Append(aPB2);
  }
  else {
    BOPDS_ListOfPaveBlock aLPB(aAllocator);
    aLPB.Append(aPB2);
    aMPBLPB.Bind(aPB1, aLPB);
  }
  if (aMPBLPB.IsBound(aPB2)) {
    BOPDS_ListOfPaveBlock& aLPB=aMPBLPB.ChangeFind(aPB2);
    aLPB.Append(aPB1);
  }
  else {
    BOPDS_ListOfPaveBlock aLPB(aAllocator);
    aLPB.Append(aPB1);
    aMPBLPB.Bind(aPB2, aLPB);
  }
}
//=======================================================================
//function : FillMap
//purpose  : 
//=======================================================================
  void BOPAlgo_Tools::FillMap(const Handle(BOPDS_PaveBlock)& aPB,
                              const Standard_Integer nF,
                              BOPDS_DataMapOfPaveBlockListOfInteger& aMPBLI,
                              Handle(NCollection_BaseAllocator)& aAllocator)
{
  if (aMPBLI.IsBound(aPB)) {
    BOPCol_ListOfInteger& aLI=aMPBLI.ChangeFind(aPB);
    aLI.Append(nF);
  }
  else {
    BOPCol_ListOfInteger aLI(aAllocator);
    aLI.Append(nF);
    aMPBLI.Bind(aPB, aLI);
  }
}
//=======================================================================
//function : MakeBlocks
//purpose  : 
//=======================================================================
  void BOPAlgo_Tools::MakeBlocks(const BOPDS_DataMapOfPaveBlockListOfPaveBlock& aMILI,
                                 BOPDS_DataMapOfPaveBlockListOfPaveBlock& aMBlocks,
                                 Handle(NCollection_BaseAllocator)& aAllocator)
{
  Standard_Integer aNbV,  aNbVS, aNbVP, k;//nV,nVx,nVP
  BOPDS_DataMapIteratorOfDataMapOfPaveBlockListOfPaveBlock aItMILI;
  BOPDS_MapIteratorOfMapOfPaveBlock aItMVP;
  BOPDS_ListIteratorOfListOfPaveBlock aItLI;
  //
  BOPDS_MapOfPaveBlock aMVS(100, aAllocator);
  BOPDS_MapOfPaveBlock aMEC(100, aAllocator);
  BOPDS_MapOfPaveBlock aMVP(100, aAllocator);
  BOPDS_MapOfPaveBlock aMVAdd(100, aAllocator);
  //
  aNbV=aMILI.Extent();
  //
  aItMILI.Initialize(aMILI);
  for (k=0; aItMILI.More(); aItMILI.Next()) {
    aNbVS=aMVS.Extent();
    if (aNbVS==aNbV) {
      break;
    }
    //
    const Handle(BOPDS_PaveBlock)& nV=aItMILI.Key();
    if (aMVS.Contains(nV)){
      continue;
    }
    aMVS.Add(nV);
    //
    aMEC.Clear();
    aMVP.Clear();
    aMVAdd.Clear();
    //
    aMVP.Add(nV);
    while(1) {
      aNbVP=aMVP.Extent();
      aItMVP.Initialize(aMVP);
      for (; aItMVP.More(); aItMVP.Next()) {
        const Handle(BOPDS_PaveBlock)& nVP=aItMVP.Value();
        const BOPDS_ListOfPaveBlock& aLV=aMILI.Find(nVP);
        aItLI.Initialize(aLV);
        for (; aItLI.More(); aItLI.Next()) {
          const Handle(BOPDS_PaveBlock)& nVx=aItLI.Value();
          if (aMEC.Contains(nVx)) {
            continue;
          }
          //
          aMVS.Add(nVx);
          aMEC.Add(nVx);
          aMVAdd.Add(nVx);
        }
      }
      //
      aNbVP=aMVAdd.Extent();
      if (!aNbVP) {
        break; // from while(1)
      }
      //
      aMVP.Clear();
      aItMVP.Initialize(aMVAdd);
      for (; aItMVP.More(); aItMVP.Next()) {
        aMVP.Add(aItMVP.Value());
      }
      aMVAdd.Clear();
    }//while(1) {
    //
    BOPDS_ListOfPaveBlock aLIx(aAllocator);
    //
    //aLIx.Append(nV);
    aItMVP.Initialize(aMEC);
    for (; aItMVP.More(); aItMVP.Next()) {
      const Handle(BOPDS_PaveBlock)& nVx=aItMVP.Value();
      aLIx.Append(nVx);
    }
    //
    aMBlocks.Bind(aLIx.First(), aLIx);
    ++k;
  }//for (; aItMILI.More(); aItMILI.Next()) {
  aMVAdd.Clear();
  aMVP.Clear();
  aMEC.Clear();
  aMVS.Clear();
}
//=======================================================================
//function : PerformCommonBlocks
//purpose  : 
//=======================================================================
  void BOPAlgo_Tools::PerformCommonBlocks(BOPDS_DataMapOfPaveBlockListOfPaveBlock& aMPBLPB,
                                          Handle(NCollection_BaseAllocator)& aAllocator)
{
  Standard_Integer aNbCB;
  //
  aNbCB=aMPBLPB.Extent();
  if (!aNbCB) {
    return;
  }
  //
  Standard_Integer aNbPB;
  BOPDS_DataMapIteratorOfDataMapOfPaveBlockListOfPaveBlock aItMPBLPB;
  BOPDS_ListIteratorOfListOfPaveBlock aItLPB;
  Handle(BOPDS_CommonBlock) aCB;
  BOPDS_DataMapOfPaveBlockListOfPaveBlock aMBlocks(100, aAllocator);
  //
  BOPAlgo_Tools::MakeBlocks(aMPBLPB, aMBlocks, aAllocator);
  //
  aItMPBLPB.Initialize(aMBlocks);
  for (; aItMPBLPB.More(); aItMPBLPB.Next()) {
    const Handle(BOPDS_PaveBlock)& aPB=aItMPBLPB.Key();
    const BOPDS_ListOfPaveBlock& aLPB=aItMPBLPB.Value();
    aNbPB=aLPB.Extent();
    if (aNbPB>1) {
      aCB=new BOPDS_CommonBlock;
      //
      aItLPB.Initialize(aLPB);
      for (; aItLPB.More(); aItLPB.Next()) {
        const Handle(BOPDS_PaveBlock)& aPBx=aItLPB.Value();
        aCB->AddPaveBlock(aPBx);
      }
      //
      aItLPB.Initialize(aLPB);
      for (; aItLPB.More(); aItLPB.Next()) {
        const Handle(BOPDS_PaveBlock)& aPBx=aItLPB.Value();
        aPBx->SetCommonBlock(aCB);
      }
    }//if (aNbPB>1) {
  }
}
//=======================================================================
//function : PerformCommonBlocks
//purpose  : 
//=======================================================================
  void BOPAlgo_Tools::PerformCommonBlocks(const BOPDS_DataMapOfPaveBlockListOfInteger& aMPBLI,
                                          Handle(NCollection_BaseAllocator)& )//aAllocator)
{
  Standard_Integer nF;
  BOPDS_DataMapIteratorOfDataMapOfPaveBlockListOfInteger aItMPBLI;
  BOPCol_ListIteratorOfListOfInteger aItLI;
  Handle(BOPDS_PaveBlock) aPB;
  Handle(BOPDS_CommonBlock) aCB;
  //
  aItMPBLI.Initialize(aMPBLI);
  for (; aItMPBLI.More(); aItMPBLI.Next()) {
    aPB=aItMPBLI.Key();
    if (aPB->IsCommonBlock()) {
      aCB=aPB->CommonBlock();
    }
    else {
      aCB=new BOPDS_CommonBlock;
      aCB->AddPaveBlock(aPB);
    }
    //
    const BOPCol_ListOfInteger& aLI=aItMPBLI.Value();
    aItLI.Initialize(aLI);
    for (; aItLI.More(); aItLI.Next()) {
      nF=aItLI.Value();
      aCB->AddFace(nF);
    }
    aPB->SetCommonBlock(aCB);
  }
}

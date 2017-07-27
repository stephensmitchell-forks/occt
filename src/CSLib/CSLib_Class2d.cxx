// Created on: 1995-03-08
// Created by: Laurent BUCHARD
// Copyright (c) 1995-1999 Matra Datavision
// Copyright (c) 1999-2014 OPEN CASCADE SAS
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

//#define No_Standard_OutOfRange

#include <CSLib_Class2d.hxx>
#include <gp_Pnt2d.hxx>
#include <Standard_ConstructionError.hxx>

#ifdef _WIN32
#if _MSC_VER < 1800
inline double log2 (const double arg)
{
  return log(arg) / log(2.);
}
#endif
#endif

static inline 
  Standard_Real Transform2d(const Standard_Real u,
			    const Standard_Real umin,
			    const Standard_Real umaxmumin);

//=======================================================================
//function : CSLib_Class2d
//purpose  : Constructor
//CR28569  : When the number of points is big enough (now 64 or more) then
//           the internal array of 1D Y-boxes myBoxes1d is initialized. It
//           cointains approx. sqrt(N) boxes with equal number of points in
//           them (mySizeBox is a degree of 2). In each box two double
//           numbers define min Y and max Y of the referred points.
//=======================================================================
CSLib_Class2d::CSLib_Class2d(const TColgp_Array1OfPnt2d& TP2d,
			     const Standard_Real theTolu,
			     const Standard_Real theTolv,
			     const Standard_Real umin,
			     const Standard_Real vmin,
			     const Standard_Real umax,
			     const Standard_Real vmax) 
  : MyPnts2d    (NULL),
    MyBoxes1d   (NULL),
    mySizeBox   (0),
    myNBoxes    (0),
    N           (0)
{
  //
  if(umax > umin && vmax > vmin)
  {
    Standard_Integer i, iLower;
    Standard_Real du,dv,*Pnts2dX,*Pnts2dY, aPrc;
    Umin=umin;
    Vmin=vmin;
    Umax=umax;
    Vmax=vmax;
    //
    aPrc=1.e-10;
    N = TP2d.Length();
    Tolu = theTolu;
    Tolv = theTolv;
    MyPnts2d = new Standard_Real [2*N+2];
    MyBoxes1d = NULL;
    Pnts2dX = MyPnts2d;
    Pnts2dY = Pnts2dX + 1;

    du=umax-umin;
    dv=vmax-vmin;

    iLower=TP2d.Lower();

    if (N > 63) {
      // large polygon, here 1D Y-boxes are created and filled with data.
      const Standard_Integer aDegBox = static_cast<Standard_Integer>
        (0.5 * log2(static_cast<Standard_Real>(N-1)));
      mySizeBox = 1 << aDegBox;
      myNBoxes = (N + mySizeBox - 1) / mySizeBox;
      MyBoxes1d = new Standard_Real[2 * myNBoxes];

      Standard_Integer iBox(0), maskBox(mySizeBox-1);
      const gp_Pnt2d& aP0 = TP2d(iLower);
      Pnts2dX[0] = Transform2d(aP0.X(), umin, du);
      Pnts2dY[0] = Transform2d(aP0.Y(), vmin, dv);
      MyBoxes1d[0] = Pnts2dY[0];
      MyBoxes1d[1] = Pnts2dY[0];

      for (i = 1; i<N; ++i) { 
        const gp_Pnt2d& aP2D=TP2d(i+iLower);
        Pnts2dX[2*i] = Transform2d(aP2D.X(), umin, du);
        const Standard_Real anY = Transform2d(aP2D.Y(), vmin, dv);
        Pnts2dY[2*i] = anY;
        if ((i & maskBox) == 0) {
          if (anY < MyBoxes1d[iBox])
            MyBoxes1d[iBox] = anY;
          if (anY > MyBoxes1d[iBox+1])
            MyBoxes1d[iBox+1] = anY;
          iBox+=2;
          MyBoxes1d[iBox] = anY;
          MyBoxes1d[iBox+1] = anY;
        }
        else if (anY < MyBoxes1d[iBox])
          MyBoxes1d[iBox] = anY;
        else if (anY > MyBoxes1d[iBox+1])
          MyBoxes1d[iBox+1] = anY;
      }
      Pnts2dX[2*N]=Pnts2dX[0];
      Pnts2dY[2*N]=Pnts2dY[0];
      // now iBox must be equal to 2*myNBoxes-2
      if (Pnts2dY[0] < MyBoxes1d[iBox])
        MyBoxes1d[iBox] = Pnts2dY[0];
      if (Pnts2dY[0] > MyBoxes1d[iBox+1])
        MyBoxes1d[iBox+1] = Pnts2dY[0];
    }
    else {
      // small polygon, simply fill the array of points.
      for(i = 0; i<N; ++i) {
        const gp_Pnt2d& aP2D=TP2d(i+iLower);
        Pnts2dX[2*i] = Transform2d(aP2D.X(), umin, du);
        Pnts2dY[2*i] = Transform2d(aP2D.Y(), vmin, dv);
      }
      Pnts2dX[2*N]=Pnts2dX[0];
      Pnts2dY[2*N]=Pnts2dY[0];
    }
    //
    if(du>aPrc) {
      Tolu/=du;
    }
    if(dv>aPrc) {
      Tolv/=dv;
    }
  }
}
//=======================================================================
//function : Destroy
//purpose  : 
//=======================================================================
void CSLib_Class2d::Destroy() { 
  if(MyPnts2d) { 
    delete [] MyPnts2d;
    MyPnts2d=NULL;
  }
  if(MyBoxes1d) { 
    delete [] MyBoxes1d;
    MyBoxes1d=NULL;
  }
}

//-- Attention   Table of 0 ------> N + 1 
//--                      P1 ..... Pn P1
//--
//--     1  2  3
//--     4  0  5
//--     6  7  8
//-- 
//=======================================================================
//function : SiDans
//purpose  : 
//=======================================================================
Standard_Integer CSLib_Class2d::SiDans(const gp_Pnt2d& P) const
{ 
  if(!N) {
    return 0;
  }
  //
  Standard_Real x,y, aTolu, aTolv;
  //
  x = P.X(); y = P.Y();
  aTolu=Tolu*(Umax-Umin);
  aTolv=Tolv*(Vmax-Vmin);
  //
  if(Umin<Umax && Vmin<Vmax)    {
    if( ( x<(Umin-aTolu) ) || 
       ( x>(Umax+aTolu) ) || 
       ( y<(Vmin-aTolv) ) || 
       ( y>(Vmax+aTolv) ) ) {
      return -1;
    }
    x=Transform2d(x,Umin,Umax-Umin);
    y=Transform2d(y,Vmin,Vmax-Vmin);
  }


  Standard_Integer res = InternalSiDansOuOn(x,y);
  if(res==-1) {    
    return 0;
  }
  if(Tolu || Tolv) {
    if(res != InternalSiDans(x-Tolu,y-Tolv)) return 0;
    if(res != InternalSiDans(x+Tolu,y-Tolv)) return 0;
    if(res != InternalSiDans(x-Tolu,y+Tolv)) return 0;
    if(res != InternalSiDans(x+Tolu,y+Tolv)) return 0; 
  }
  //
  return((res)? 1: -1);
}
//=======================================================================
//function : SiDans_OnMode
//purpose  : 
//=======================================================================
Standard_Integer CSLib_Class2d::SiDans_OnMode(const gp_Pnt2d& P,
					      const Standard_Real Tol) const
{ 
  if(!N){
    return 0;
  }
  //
  Standard_Real x,y, aTolu, aTolv;
  //
  x = P.X(); y = P.Y();
  aTolu=Tol; 
  aTolv=Tol; 

  //-- ****** TO DO LATER, ESTIMATE AT EACH POINT Tol2d depending on Tol3d *****
  if(Umin<Umax && Vmin<Vmax) { 
    if(x<(Umin-aTolu) || (x>Umax+aTolu) || 
       (y<Vmin-aTolv) || (y>Vmax+aTolv)) {
      return -1;
    }
    x=Transform2d(x,Umin,Umax-Umin);
    y=Transform2d(y,Vmin,Vmax-Vmin);
  }
  //
  Standard_Integer res = InternalSiDansOuOn(x,y);
  if(aTolu || aTolv) {
    if(res != InternalSiDans(x-aTolu,y-aTolv)) return 0;
    if(res != InternalSiDans(x+aTolu,y-aTolv)) return 0;
    if(res != InternalSiDans(x-aTolu,y+aTolv)) return 0;
    if(res != InternalSiDans(x+aTolu,y+aTolv)) return 0; 
  }
  return((res)? 1: -1);
}
//=======================================================================
//function : InternalSiDans
//purpose  : 
//=======================================================================
Standard_Integer CSLib_Class2d::InternalSiDans(const Standard_Real Px,
					       const Standard_Real Py) const
{ 
  Standard_Integer nbc, i, SH, NH;
  Standard_Real *Pnts2dX, *Pnts2dY;
  Standard_Real  x, y, nx, ny;
  //
  nbc = 0;
  i   = 0;
  Pnts2dX = MyPnts2d;
  Pnts2dY = Pnts2dX + 1;
  if (MyBoxes1d == NULL)
  {
    x   = (Pnts2dX[0]-Px);
    y   = (Pnts2dY[0]-Py);
    SH  = (y<0.)? -1 : 1;
    //
    for(i=0; i<N ; i++) { 
      nx = Pnts2dX[2*(i+1)] - Px;
      ny = Pnts2dY[2*(i+1)] - Py;
    
      NH = (ny<0.)? -1 : 1;
      if(NH!=SH) {
        if(x>0. && nx>0.) {
          nbc++;
        }
        else {
          if(x>0.0 || nx>0.) {
            if((x-y*(nx-x)/(ny-y))>0.) {
              nbc++;
            }
          }
        }
        SH = NH;
      }
      x = nx; y = ny;
    }
  }
  else
  {
    // Find Y-boxes that include Py. Box with index iBox refers the range of
    // points starting from iBox*sizeBox.  
    for (Standard_Integer iBox = 0; iBox < myNBoxes; iBox++) {
      if ((MyBoxes1d[2*iBox] - Tolv - Py) * (MyBoxes1d[2*iBox+1] + Tolv - Py) < 0.)
      {
        Standard_Integer lastBox = (iBox + 1) * mySizeBox;
        if (lastBox > N)
          lastBox = N;
        x = (Pnts2dX[2*iBox*mySizeBox]-Px);
        y = (Pnts2dY[2*iBox*mySizeBox]-Py);
        for (i = iBox*mySizeBox; i < lastBox; i++)
        {
          nx = Pnts2dX[2*(i+1)] - Px;
          ny = Pnts2dY[2*(i+1)] - Py;
          if (ny * y < 0.) {
            if (x > 0. && nx > 0.)
              nbc++;
            else if (x > 0.0 || nx > 0.) {
              if ((x - y*(nx-x)/(ny-y)) > 0.)
                nbc++;
            }
          }
          x = nx; y = ny;
        }
      }
    }
  }
  return(nbc&1);
}

//modified by NIZNHY-PKV Fri Jan 15 09:03:48 2010f
//=======================================================================
//function : InternalSiDansOuOn
//purpose  : same code as above + test on ON (return(-1) in this case
//=======================================================================
Standard_Integer CSLib_Class2d::InternalSiDansOuOn(const Standard_Real Px,
						   const Standard_Real Py) const 
{ 
  Standard_Integer nbc, i, ip1, SH, NH, iRet = 0;
  Standard_Real *Pnts2dX, *Pnts2dY;
  Standard_Real x, y, nx, ny, aX;
  //
  nbc = 0;
  i   = 0;
  ip1 = 1;
  Pnts2dX = MyPnts2d;
  Pnts2dY = Pnts2dX + 1;
  if (MyBoxes1d == NULL)
  {
    x   = (Pnts2dX[0]-Px);
    y   = (Pnts2dY[0]-Py);
    SH  = (y<0.)? -1 : 1;
    for(i=0; i<N ; i++, ip1++)
    { 
      nx = Pnts2dX[2*ip1] - Px;
      ny = Pnts2dY[2*ip1] - Py;
      //-- le 14 oct 97 
      if(nx<Tolu && nx>-Tolu && ny<Tolv && ny>-Tolv) { 
        iRet=-1;
        return iRet;
      }
      //find Y coordinate of polyline for current X gka
      //in order to detect possible status ON
      Standard_Real aDx = (Pnts2dX[2*ip1] - Pnts2dX[2*(ip1-1)]);
      if( (Pnts2dX[2*(ip1-1)] - Px) * nx < 0.)
      {
     
        Standard_Real aCurPY =
          Pnts2dY[2*ip1] - (Pnts2dY[2*ip1] - Pnts2dY[2*(ip1-1)])/aDx *nx;
        Standard_Real aDeltaY = aCurPY - Py;
        if(aDeltaY >= -Tolv && aDeltaY <= Tolv)
        {
          iRet=-1;
          return iRet;
        }
      }
      //
      
      NH = (ny<0.)? -1 : 1;
      if(NH!=SH) { 
        if(x>0. && nx>0.) {
          nbc++;
        }
        else { 
          if(x>0. || nx>0.) { 
            aX=x-y*(nx-x)/(ny-y);
            if(aX>0.){
              nbc++;
            }
          }
        }
        SH = NH;
      }
      x = nx; y = ny; 
    }// for(i=0; i<N ; i++,ip1++) { 
  }
  else
  {
    const Standard_Real aTolu2(Tolu * Tolu);
    const Standard_Real aTolv2(Tolv * Tolv);

    // Find Y-boxes that include Py. Box with index iBox refers the range of
    // points starting from iBox*sizeBox.  
    for (Standard_Integer iBox = 0; iBox < myNBoxes; iBox++) {
      if ((MyBoxes1d[2*iBox] - Tolv - Py) * (MyBoxes1d[2*iBox+1] + Tolv - Py) < 0.)
      {
        Standard_Integer lastBox = (iBox + 1) * mySizeBox;
        if (lastBox > N)
          lastBox = N;
        x = (Pnts2dX[2*iBox*mySizeBox]-Px);
        y = (Pnts2dY[2*iBox*mySizeBox]-Py);
        for (i = iBox*mySizeBox; i < lastBox; i++)
        {
          nx = Pnts2dX[2*(i+1)] - Px;
          ny = Pnts2dY[2*(i+1)] - Py;
          if (nx * nx < aTolu2 && ny * ny < aTolv2) {
            iRet = -1;
            break;
          }
          //find Y coordinate of polyline for current X gka
          //in order to detect possible status ON
          if (x /*(Pnts2dX[2*(ip1-1)] - Px)*/ * nx < 0.)
          {
            const Standard_Real aDeltaY = ny - (ny - y)/(nx - x) * nx;
            if (aDeltaY*aDeltaY <= aTolv2) {
              iRet = -1;
              break;
            }
          }
          if (ny * y < 0.) {
            if (x > 0. && nx > 0.)
              nbc++;
            else if (x > 0. || nx > 0.) {
              if ((x - y*(nx-x)/(ny-y)) > 0.)
                nbc++;
            }
          }
          x = nx; y = ny;
        } // for(.. i ..)
        if (iRet < 0)
          break;
      }
    } // for(.. iBox ..)
  }
  if (iRet == 0)
    iRet = nbc&1;
  return iRet;
}

//modified by NIZNHY-PKV Fri Jan 15 09:03:55 2010t
//=======================================================================
//function : Copy
//purpose  : 
//=======================================================================
const CSLib_Class2d& CSLib_Class2d::Copy(const CSLib_Class2d& ) const 
{ 
#ifdef OCCT_DEBUG
  cerr<<"Copy not allowed in CSLib_Class2d"<<endl;
#endif
  throw Standard_ConstructionError();
}
//=======================================================================
//function : Transform2d
//purpose  : 
//=======================================================================
Standard_Real Transform2d(const Standard_Real u,
			  const Standard_Real umin,
			  const Standard_Real umaxmumin) 
{ 
  if(umaxmumin>1e-10) { 
    Standard_Real U = (u-umin)/umaxmumin;
    return U;
  }
  else { 
    return u;
  }
}

// Created on: 1995-12-11
// Created by: Jacques GOUSSARD
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

#include <BRepCheck_Edge.ixx>
#include <TColStd_Array1OfTransient.hxx>

#include <BRepCheck_ListOfStatus.hxx>
#include <BRepCheck_ListIteratorOfListOfStatus.hxx>

#include <BRep_TEdge.hxx>
#include <BRep_TFace.hxx>

#include <BRep_CurveRepresentation.hxx>
#include <BRep_ListOfCurveRepresentation.hxx>
#include <BRep_ListIteratorOfListOfCurveRepresentation.hxx>
#include <BRep_GCurve.hxx>
#include <BRep_CurveOnSurface.hxx>

#include <BRep_Tool.hxx>

#include <TopExp_Explorer.hxx>

#include <Geom_Surface.hxx>
#include <Geom_RectangularTrimmedSurface.hxx>
#include <Geom_Plane.hxx>
#include <Geom_TrimmedCurve.hxx>
#include <Geom2d_Curve.hxx>

#include <Geom2dAdaptor_HCurve.hxx>
#include <GeomAdaptor_Curve.hxx>
#include <GeomAdaptor_HCurve.hxx>
#include <GeomAdaptor_HSurface.hxx>
#include <Adaptor3d_CurveOnSurface.hxx>
#include <Adaptor3d_HCurveOnSurface.hxx>

#include <ProjLib_ProjectedCurve.hxx>
#include <GeomProjLib.hxx>
#include <Extrema_LocateExtPC.hxx>

#include <BRepCheck.hxx>
#include <Geom2dAdaptor.hxx>
#include <TopoDS.hxx>
#include <TopAbs_ShapeEnum.hxx>
#include <TopoDS_Face.hxx>
#include <Precision.hxx>


static 
  BRepCheck_Status Validate(const Adaptor3d_Curve&, 
                            const Adaptor3d_CurveOnSurface&,
                            const Standard_Real,
                            const Standard_Boolean,
                            const Standard_Boolean theSurfIsUPeriodic,
                            const Standard_Boolean theSurfIsVPeriodic);

static
  void PrintProblematicPoint(const gp_Pnt&,
			     const Standard_Real,
			     const Standard_Real);

static
  Standard_Real Prec(const Adaptor3d_Curve& aAC3D,
		     const Adaptor3d_CurveOnSurface& aACS);
static
  Standard_Real PrecCurve(const Adaptor3d_Curve& aAC3D);
static
  Standard_Real PrecSurface(const Adaptor3d_CurveOnSurface& aACS);

//modified by NIZNHY-PKV Thu May 05 09:02:01 2011t

static const Standard_Integer aNbControl = 23;

//=======================================================================
//function : BRepCheck_Edge
//purpose  : 
//=======================================================================

BRepCheck_Edge::BRepCheck_Edge(const TopoDS_Edge& E)
{
  Init(E);
  myGctrl = Standard_True;
}

//=======================================================================
//function : Minimum
//purpose  : 
//=======================================================================

void BRepCheck_Edge::Minimum()
{

  if (!myMin) {
    BRepCheck_ListOfStatus thelist;
    myMap.Bind(myShape, thelist);
    BRepCheck_ListOfStatus& lst = myMap(myShape);
    myCref.Nullify();

    // Existence et unicite d`une representation 3D
    Handle(BRep_TEdge)& TE = *((Handle(BRep_TEdge)*)&myShape.TShape());
    BRep_ListIteratorOfListOfCurveRepresentation itcr(TE->Curves());
    Standard_Boolean exist = Standard_False;
    Standard_Boolean unique = Standard_True;
    // Search for a 3D reference. If no existent one, creates it with the 
    // first encountered CurveOnSurf; if multiple, chooses the first one...
    
    Standard_Boolean Degenerated = TE->Degenerated();
    Standard_Boolean SameParameter = TE->SameParameter();
    Standard_Boolean SameRange = TE->SameRange();
    if (!SameRange && SameParameter) {
      BRepCheck::Add(lst,BRepCheck_InvalidSameParameterFlag);
    }
//    Handle(Geom_Curve) C3d;

    while (itcr.More()) {
      const Handle(BRep_CurveRepresentation)& cr = itcr.Value();
      if (cr->IsCurve3D()) {
	if (!exist) {
	  exist = Standard_True;
	}
	else {
	  unique = Standard_False;
	}
	if (myCref.IsNull() && !cr->Curve3D().IsNull()) {
	  myCref = cr;
	}
      }
      itcr.Next();
    }

    if (!exist) {
      BRepCheck::Add(lst,BRepCheck_No3DCurve);
      // myCref est nulle
    }
    else if (!unique) {
      BRepCheck::Add(lst,BRepCheck_Multiple3DCurve);
    }

    if (myCref.IsNull() && !Degenerated) {
      itcr.Initialize(TE->Curves());
      while (itcr.More()) {
	const Handle(BRep_CurveRepresentation)& cr = itcr.Value();
	if (cr->IsCurveOnSurface()) {
	  myCref = cr;
	  break;
	}
	itcr.Next();
      }
    }
    else if (!myCref.IsNull() && Degenerated){
      BRepCheck::Add(lst,BRepCheck_InvalidDegeneratedFlag);
    }

    if (!myCref.IsNull()) {
      const Handle(BRep_GCurve)& GCref = *((Handle(BRep_GCurve)*)&myCref);
      Standard_Real First,Last;
      GCref->Range(First,Last);
      if (Last<=First) {
	myCref.Nullify();
	BRepCheck::Add(lst,BRepCheck_InvalidRange);
      }
      else {
	if (myCref->IsCurve3D()) {
          // eap 6 Jun 2002 occ332
          // better transform C3d instead of transforming Surf upto C3d initial location,
          // on transformed BSpline surface 'same parameter' may seem wrong
          TopLoc_Location L = myShape.Location() * myCref->Location();
	  Handle(Geom_Curve) C3d = Handle(Geom_Curve)::DownCast
	    (myCref->Curve3D()->Transformed
	     (/*myCref->Location()*/L.Transformation()));
	  GeomAdaptor_Curve GAC3d(C3d,First,Last);
	  myHCurve = new GeomAdaptor_HCurve(GAC3d);
	}
	else { // curve on surface
	  Handle(Geom_Surface) Sref = myCref->Surface();
	  Sref = Handle(Geom_Surface)::DownCast
	    (Sref->Transformed(myCref->Location().Transformation()));
	  const Handle(Geom2d_Curve)& PCref = myCref->PCurve();
	  Handle(GeomAdaptor_HSurface) GAHSref = new GeomAdaptor_HSurface(Sref);
	  Handle(Geom2dAdaptor_HCurve) GHPCref = 
	    new Geom2dAdaptor_HCurve(PCref,First,Last);
	  Adaptor3d_CurveOnSurface ACSref(GHPCref,GAHSref);
	  myHCurve = new Adaptor3d_HCurveOnSurface(ACSref);
	}
      }
    }
    if (lst.IsEmpty()) {
      lst.Append(BRepCheck_NoError);
    }
    myMin = Standard_True;
  }
}


//=======================================================================
//function : InContext
//purpose  : 
//=======================================================================

void BRepCheck_Edge::InContext(const TopoDS_Shape& S)
{
  if (myMap.IsBound(S))
    return;

  BRepCheck_ListOfStatus thelist;
  myMap.Bind(S, thelist);
  BRepCheck_ListOfStatus& lst = myMap(S);

  Handle(BRep_TEdge)& TE = *((Handle(BRep_TEdge)*)&myShape.TShape());
  Standard_Real Tol = BRep_Tool::Tolerance(TopoDS::Edge(myShape));

  TopAbs_ShapeEnum styp = S.ShapeType();
  TopExp_Explorer exp(S,TopAbs_EDGE) ;
  for ( ; exp.More(); exp.Next())
  {
    if (exp.Current().IsSame(myShape))
      break;
  }

  if (!exp.More())
  {
    BRepCheck::Add(lst,BRepCheck_SubshapeNotInShape);
    return;
  }

  switch (styp)
  {
    case TopAbs_FACE:
      if (myCref.IsNull())
        break;

      {
        Standard_Boolean SameParameter = TE->SameParameter();
        Standard_Boolean SameRange = TE->SameRange();

        if (!SameParameter || !SameRange)
        {
          if (!SameParameter)
            BRepCheck::Add(lst,BRepCheck_InvalidSameParameterFlag);

          if (!SameRange)
            BRepCheck::Add(lst,BRepCheck_InvalidSameRangeFlag);

          return;
        }//if (!SameParameter || !SameRange)

        const Standard_Real First = myHCurve->FirstParameter();
        const Standard_Real Last  = myHCurve->LastParameter();

        Handle(BRep_TFace)& TF = *((Handle(BRep_TFace)*) &S.TShape());
        const TopLoc_Location& Floc = S.Location();
        const TopLoc_Location& TFloc = TF->Location();
        const Handle(Geom_Surface)& Su = TF->Surface();
        TopLoc_Location L = (Floc * TFloc).Predivided(myShape.Location());
        Standard_Boolean pcurvefound = Standard_False;

        BRep_ListIteratorOfListOfCurveRepresentation itcr(TE->Curves());
        
        while (itcr.More())
        {
          const Handle(BRep_CurveRepresentation)& cr = itcr.Value();
          if (cr != myCref && cr->IsCurveOnSurface(Su,L))
          {
            pcurvefound = Standard_True;
            const Handle(BRep_GCurve)& GC = *((Handle(BRep_GCurve)*)&cr);
            Standard_Real f,l;
            GC->Range(f,l);

            if (fabs(f-First) > Precision::PConfusion() ||
                              fabs(l-Last)  > Precision::PConfusion())
            {
              BRepCheck::Add(lst,BRepCheck_InvalidSameRangeFlag);
              BRepCheck::Add(lst,BRepCheck_InvalidSameParameterFlag);
            }

            if (myGctrl)
            {
              Handle(Geom_Surface) Sb = cr->Surface();
              {
                Standard_Real U1Su, U2Su, V1Su, V2Su;
                Standard_Real U1Sb, U2Sb, V1Sb, V2Sb;
                
                Standard_Boolean isTrimU = Standard_False, isTrimV = Standard_False;
                if(Su->DynamicType() == STANDARD_TYPE(Geom_RectangularTrimmedSurface))
                {
                  Handle(Geom_RectangularTrimmedSurface) TS = 
                        Handle(Geom_RectangularTrimmedSurface)::DownCast(Su);

                  TS->GetTrimmedFlags(isTrimU, isTrimV);
                }
                
                Su->Bounds(U1Su, U2Su, V1Su, V2Su);
                Sb = Handle(Geom_Surface)::DownCast
                      (Su->Transformed((Floc * TFloc).Transformation()));

                Sb->Bounds(U1Sb, U2Sb, V1Sb, V2Sb);
                Standard_Boolean  isUtr = ((Abs(U1Su - U1Sb) + Abs(U2Su - U2Sb)) > Precision::PConfusion()), 
                                  isVtr = ((Abs(V1Su - V1Sb) + Abs(V2Su - V2Sb)) > Precision::PConfusion());

                if(isUtr || isVtr)
                {
                  Handle(Geom_Surface) St = Handle(Geom_RectangularTrimmedSurface)::DownCast(Sb)->BasisSurface();
                  Sb = new Geom_RectangularTrimmedSurface(St, isTrimU || isUtr, isVtr || isTrimV, U1Su, U2Su, V1Su, V2Su);
                }
              }

              Handle(Geom2d_Curve) PC = cr->PCurve();
              Handle(GeomAdaptor_HSurface) GAHS = new GeomAdaptor_HSurface(Sb);
              Handle(Geom2dAdaptor_HCurve) GHPC = new Geom2dAdaptor_HCurve(PC,f,l);
              Adaptor3d_CurveOnSurface ACS(GHPC,GAHS);
              BRepCheck_Status aStatus = Validate(myHCurve->Curve(),ACS,Tol,SameParameter,
                                  Sb->IsUPeriodic(),Sb->IsVPeriodic());

              if(aStatus == BRepCheck_PCurveIsOutOfDomainFace)
                  BRepCheck::Add(lst,BRepCheck_PCurveIsOutOfDomainFace);
              else if(aStatus == BRepCheck_InvalidCurveOnSurface)
              {
                if (cr->IsCurveOnClosedSurface())
                  BRepCheck::Add(lst,BRepCheck_InvalidCurveOnClosedSurface);
                else
                  BRepCheck::Add(lst,BRepCheck_InvalidCurveOnSurface);

                BRepCheck::Add(lst,BRepCheck_InvalidSameParameterFlag);
              }
              else if(aStatus != BRepCheck_NoError)
                BRepCheck::Add(lst,aStatus);

              if (cr->IsCurveOnClosedSurface())
              {
                GHPC->ChangeCurve2d().Load(cr->PCurve2(),f,l); // same bounds
                ACS.Load(GAHS); // sans doute inutile
                ACS.Load(GHPC); // meme remarque...
                aStatus = Validate(myHCurve->Curve(),ACS,Tol,SameParameter,
                              Sb->IsUPeriodic(),Sb->IsVPeriodic());

                if(aStatus == BRepCheck_PCurveIsOutOfDomainFace)
                  BRepCheck::Add(lst,BRepCheck_PCurveIsOutOfDomainFace);
                else if(aStatus == BRepCheck_InvalidCurveOnSurface)
                {
                  BRepCheck::Add(lst,BRepCheck_InvalidCurveOnClosedSurface);
                  if (SameParameter)
                    BRepCheck::Add(lst,BRepCheck_InvalidSameParameterFlag);
                }
                else if(aStatus != BRepCheck_NoError)
                  BRepCheck::Add(lst,aStatus);
              }//if (cr->IsCurveOnClosedSurface())
            }//if (myGctrl)
          }//if (cr != myCref && cr->IsCurveOnSurface(Su,L))
          itcr.Next();
        }//while (itcr.More())
        
        if (!pcurvefound)
        {
          Handle(Geom_Plane) P;
          Handle(Standard_Type) dtyp = Su->DynamicType();
          if (dtyp == STANDARD_TYPE(Geom_RectangularTrimmedSurface))
          {
            P = Handle(Geom_Plane)::DownCast
                    (Handle(Geom_RectangularTrimmedSurface)::
                    DownCast(Su)->BasisSurface());
          }
          else
            P = Handle(Geom_Plane)::DownCast(Su);
          
          if (P.IsNull()) // not a plane
            BRepCheck::Add(lst,BRepCheck_NoCurveOnSurface);
          else
          {
  // on fait la projection a la volee, comme BRep_Tool plan en position
            if (myGctrl)
            {
              P = Handle(Geom_Plane)::DownCast(
                    P->Transformed((Floc * TFloc).Transformation()));// eap occ332

              //on projette Cref sur ce plan
              Handle(GeomAdaptor_HSurface) GAHS = new GeomAdaptor_HSurface(P);

              // Dub - Normalement myHCurve est une GeomAdaptor_HCurve
              GeomAdaptor_Curve& Gac = 
                Handle(GeomAdaptor_HCurve)::DownCast(myHCurve)->ChangeCurve();

              Handle(Geom_Curve) C3d = Gac.Curve();
              Handle(Geom_Curve) ProjOnPlane = 
                GeomProjLib::ProjectOnPlane(new Geom_TrimmedCurve(C3d,First,Last),
                P, P->Position().Direction(),Standard_True);

              Handle(GeomAdaptor_HCurve) aHCurve = 
                                          new GeomAdaptor_HCurve(ProjOnPlane);

              ProjLib_ProjectedCurve proj(GAHS,aHCurve);
              Handle(Geom2d_Curve) PC = Geom2dAdaptor::MakeCurve(proj);
              Handle(Geom2dAdaptor_HCurve) GHPC = new Geom2dAdaptor_HCurve(PC,
                                myHCurve->FirstParameter(),
                                myHCurve->LastParameter());

              Adaptor3d_CurveOnSurface ACS(GHPC,GAHS);

              BRepCheck_Status aStatus = Validate(myHCurve->Curve(),ACS,Tol,
                    Standard_True, P->IsUPeriodic(),P->IsVPeriodic()); // voir dub...

              if (aStatus != BRepCheck_NoError)
                BRepCheck::Add(lst,aStatus);
            }//if (myGctrl)
          }//else of "if (P.IsNull())" condition
        }//if (!pcurvefound)

        break;
      }
    case TopAbs_SOLID:
      {
        // on verifie que l`edge est bien connectee 2 fois (pas de bord libre)
        Standard_Integer nbconnection = 0;

        //TopExp_Explorer exp;
        for (exp.Init(S,TopAbs_FACE); exp.More(); exp.Next())
        {
          const TopoDS_Face& fac = TopoDS::Face(exp.Current());
          TopExp_Explorer exp2;
          
          for (exp2.Init(fac,TopAbs_EDGE); exp2.More(); exp2.Next())
          {
            if (exp2.Current().IsSame(myShape))
              nbconnection++;
          }//for (exp2.Init(fac,TopAbs_EDGE); exp2.More(); exp2.Next())
        }//for (exp.Init(S,TopAbs_FACE); exp.More(); exp.Next())

        if (nbconnection < 2 && !TE->Degenerated())
          BRepCheck::Add(myMap(S),BRepCheck_FreeEdge);
        else if (nbconnection > 2)
        {
          BRepCheck::Add(myMap(S),BRepCheck_InvalidMultiConnexity);
        }
        else
          BRepCheck::Add(myMap(S),BRepCheck_NoError);
        
      }
      break;

    default:
      break;
  }//switch (styp)

  if (myMap(S).IsEmpty())
    myMap(S).Append(BRepCheck_NoError);
}


//=======================================================================
//function : Blind
//purpose  : 
//=======================================================================

void BRepCheck_Edge::Blind()
{
//  Modified by skv - Tue Apr 27 11:36:01 2004 Begin
// The body of this function is removed because of its useless.
  if (!myBlind) {
    myBlind = Standard_True;
  }
//  Modified by skv - Tue Apr 27 11:36:02 2004 End
}


//=======================================================================
//function : GeometricControls
//purpose  : 
//=======================================================================

void BRepCheck_Edge::GeometricControls(const Standard_Boolean B)
{
  myGctrl = B;
}


//=======================================================================
//function : GeometricControls
//purpose  : 
//=======================================================================

Standard_Boolean BRepCheck_Edge::GeometricControls() const
{
  return myGctrl;
}




//=======================================================================
//function : Tolerance
//purpose  : 
//=======================================================================

Standard_Real BRepCheck_Edge::Tolerance()
{
  Handle(BRep_TEdge)& TE = *((Handle(BRep_TEdge)*)&myShape.TShape());
  Standard_Integer it, iRep=1, nbRep=(TE->Curves()).Extent();
  if (nbRep<=1) {
    return Precision::Confusion();
  }
  TColStd_Array1OfTransient theRep(1, nbRep*2);
  Standard_Real First, Last;
  if (!myHCurve.IsNull()) {
    First = myHCurve->FirstParameter();
    Last=  myHCurve->LastParameter();
  }
  else {
    BRep_Tool::Range(TopoDS::Edge(myShape), First, Last);
  }

  BRep_ListIteratorOfListOfCurveRepresentation itcr(TE->Curves());
  for (; itcr.More(); itcr.Next()) {
    const Handle(BRep_CurveRepresentation)& cr = itcr.Value();
    if (cr->IsCurve3D() && !TE->Degenerated()) {
      //// modified by jgv, 20.03.03 ////
      TopLoc_Location Loc = myShape.Location() * cr->Location();
      Handle(Geom_Curve) C3d = Handle(Geom_Curve)::DownCast
	(cr->Curve3D()->Transformed( Loc.Transformation() ));
      ///////////////////////////////////
      GeomAdaptor_Curve GAC3d(C3d,First,Last);
      it=iRep;
      if (iRep>1) {
	theRep(iRep)=theRep(1);
	it=1;
      }
      theRep(it) = new GeomAdaptor_HCurve(GAC3d);
      iRep++;
    }
    else if (cr->IsCurveOnSurface()) {
      {
        Handle(Geom_Surface) Sref = cr->Surface();
	//// modified by jgv, 20.03.03 ////
	TopLoc_Location Loc = myShape.Location() * cr->Location();
        Sref = Handle(Geom_Surface)::DownCast
          (Sref->Transformed( Loc.Transformation() ));
	///////////////////////////////////
        const Handle(Geom2d_Curve)& PCref = cr->PCurve();
        Handle(GeomAdaptor_HSurface) GAHSref = new GeomAdaptor_HSurface(Sref);
        Handle(Geom2dAdaptor_HCurve) GHPCref = 
          new Geom2dAdaptor_HCurve(PCref,First,Last);
        Adaptor3d_CurveOnSurface ACSref(GHPCref,GAHSref);
        theRep(iRep) = new Adaptor3d_HCurveOnSurface(ACSref);
        iRep++;
      }
      if (cr->IsCurveOnClosedSurface()) {
	Handle(Geom_Surface) Sref = cr->Surface();
	Sref = Handle(Geom_Surface)::DownCast
	  (Sref->Transformed(cr->Location().Transformation()));
	const Handle(Geom2d_Curve)& PCref = cr->PCurve2();
	Handle(GeomAdaptor_HSurface) GAHSref = new GeomAdaptor_HSurface(Sref);
	Handle(Geom2dAdaptor_HCurve) GHPCref = 
	  new Geom2dAdaptor_HCurve(PCref,First,Last);
	Adaptor3d_CurveOnSurface ACSref(GHPCref,GAHSref);
	theRep(iRep) = new Adaptor3d_HCurveOnSurface(ACSref);
	iRep++;
	nbRep++;
      }
    }
    else {
      nbRep--;
    }
  }

  Standard_Real dist2, tol2, tolCal=0., prm;
  gp_Pnt center, othP;
  Standard_Integer i;
  for (i= 0; i< aNbControl; i++) {
    prm = ((aNbControl-1-i)*First + i*Last)/(aNbControl-1);
    tol2=dist2=0.;
    center=(*(Handle(Adaptor3d_HCurve)*)&theRep(1))->Value(prm);
    for (iRep=2; iRep<=nbRep; iRep++) {
      othP=(*(Handle(Adaptor3d_HCurve)*)&theRep(iRep))->Value(prm);
      dist2=center.SquareDistance(othP);
      if (dist2>tolCal) tolCal=dist2;
    }
    if (tol2>tolCal) {
      tolCal=tol2;
    }
  }
  // On prend 5% de marge car au dessus on crontrole severement
  return sqrt(tolCal)*1.05;
}

//=======================================================================
//function : Validate
//purpose  :
//Remark : If the original surface is not periodic in U or V direction
//         (for example, rectangular trimmed on periodic surface),
//         the surface that <Adaptor3d_CurveOnSurface> contains
//         can be periodic.
//         To use true values of flags, the parameters
//         <theSurfIsUPeriodic> and <theSurfIsVPeriodic> have been added.
//=======================================================================
BRepCheck_Status Validate (const Adaptor3d_Curve& CRef,
                           const Adaptor3d_CurveOnSurface& Other,
                           const Standard_Real Tol,
                           const Standard_Boolean SameParameter,
                           const Standard_Boolean theSurfIsUPeriodic,
                           const Standard_Boolean theSurfIsVPeriodic)
{
  BRepCheck_Status  Status = BRepCheck_NoError;
  const Standard_Real aPC = Precision::PConfusion(),
                      First = CRef.FirstParameter(),
                      Last = CRef.LastParameter();

  const Standard_Boolean proj = (!SameParameter ||
                                  fabs(Other.FirstParameter()-First) > aPC ||
                                  fabs(Other.LastParameter()-Last) > aPC);

  if (!proj)
  {
    //modified by NIZNHY-PKV Thu May 05 09:06:41 2011f
    //OCC22428
    const Standard_Real dD = Prec(CRef, Other);//3.e-15;
    Standard_Real Tol2=Tol+dD;
    Tol2=Tol2*Tol2;

    //Tol2=Tol*Tol;
    //modified by NIZNHY-PKV Thu May 05 09:06:47 2011t

    const Standard_Real uf = Other.GetSurface()->FirstUParameter ();
    const Standard_Real ul = Other.GetSurface()->LastUParameter ();
    const Standard_Real vf = Other.GetSurface()->FirstVParameter();
    const Standard_Real vl = Other.GetSurface()->LastVParameter();
    
    //These values are used for estimation of toleranves of
    //"plane-like" surfaces
    const Standard_Real aDeltaU = (ul - uf);
    const Standard_Real aDeltaV = (vl - vf);

    //(for Rectangular trimmed surface for example)
    const Standard_Boolean isBaseSurfUPeriodic = Other.GetSurface()->IsUPeriodic() && !theSurfIsUPeriodic;
    const Standard_Boolean isBaseSurfVPeriodic = Other.GetSurface()->IsVPeriodic() && !theSurfIsVPeriodic;

    const Standard_Boolean isUPeriodic = isBaseSurfUPeriodic || theSurfIsUPeriodic;
    const Standard_Boolean isVPeriodic = isBaseSurfVPeriodic || theSurfIsVPeriodic;

    const Standard_Boolean isUClosedOrPeriodic = Other.GetSurface()->IsUClosed() || theSurfIsUPeriodic;
    const Standard_Boolean isVClosedOrPeriodic = Other.GetSurface()->IsVClosed() || theSurfIsVPeriodic;

    const Standard_Real UResSt =Other.GetSurface()->UResolution(Tol);
    const Standard_Real VResSt =Other.GetSurface()->VResolution(Tol);

//    if(isUClosedOrPeriodic)
//    {
//      const Standard_Real aF = Other.GetCurve()->Value(First).X();
//      const Standard_Real aL = Other.GetCurve()->Value(Last).X();
//      const Standard_Real aDim = theSurfIsUPeriodic ? 
//                                    Other.GetSurface()->UPeriod() :
//                                    aDeltaU;
//
//      if(Abs(aL - aF) - aDim > 2 * UResSt)
//      {
//#ifdef DEB
//        cout << endl << "----\nFunction Validate(...); file: "
//                                        "BRepCheck_Edge.cxx" << endl;
//        if(theSurfIsUPeriodic)
//          cout << "The surface is U-periodic." << endl;
//        else
//          cout << "The surface is U-closed." << endl;
//
//        cout << "P1.X() = " << aF << "; P2.X() = " << aL << endl;
//        cout << "Surface dimension D = " << aDim << 
//                    ". (P2.X() - P1.X()) > D." << "\n-----" << endl;
//#endif
//        Status = BRepCheck_PCurveIsOutOfDomainFace;
//        return Status;
//      }//if(aL - aF - aT > 2* URes)
//    }//if(theSurfIsUPeriodic)
//
//    if(isVClosedOrPeriodic)
//    {
//      const Standard_Real aF = Other.GetCurve()->Value(First).Y();
//      const Standard_Real aL = Other.GetCurve()->Value(Last).Y();
//      const Standard_Real aDim = theSurfIsVPeriodic ? 
//                                    Other.GetSurface()->VPeriod() : 
//                                    aDeltaV;
//
//      if(Abs(aL - aF) - aDim > 2 * VResSt)
//      {
//#ifdef DEB
//        cout << endl << "----\nFunction Validate(...); file: "
//                                          "RepCheck_Edge.cxx" << endl;
//        if(theSurfIsVPeriodic)
//          cout << "The surface is V-periodic." << endl;
//        else
//          cout << "The surface is V-closed." << endl;
//
//        cout << "P1.Y() = " << aF << "; P2.Y() = " << aL << endl;
//        cout << "Surface dimension D = " << aDim << 
//                    ". (P2.Y() - P1.Y()) > D." << "\n-----" << endl;
//#endif
//        Status = BRepCheck_PCurveIsOutOfDomainFace;
//        return Status;
//      }//if(aL - aF - aT > 2* VRes)
//    }//if(theSurfIsVPeriodic)

    Standard_Real aCriticalParameterU = First,
                  aCriticalParameterV = First;

    Standard_Real aDeltaUmax = 0.0,
                  aDeltaVmax = 0.0;

    Standard_Real dUmaxU = 0.0, dUmaxV = 0.0;
    Standard_Real dVmaxU = 0.0, dVmaxV = 0.0;

    Standard_Boolean isIntoBoundaries = Standard_True;

    for (Standard_Integer i = 0; i < aNbControl; ++i)
    {
      const Standard_Real prm = ((aNbControl-1-i)*First + i*Last)/(aNbControl-1);
      const gp_Pnt pref = CRef.Value(prm);
      const gp_Pnt pother = Other.Value(prm);

      if (pref.SquareDistance(pother) > Tol2)
      {
        const gp_Pnt problematic_point(pref) ;
        Status = BRepCheck_InvalidCurveOnSurface;
        const Standard_Real Error  = pref.Distance(pother);
        PrintProblematicPoint(problematic_point, Error, Tol);
        return Status;
      }

      const gp_Pnt2d CP = Other.GetCurve()->Value(prm);

      if(isUClosedOrPeriodic && isVClosedOrPeriodic)
        continue;

      const Standard_Real u = Max(uf, Min(CP.X(), ul));
      const Standard_Real v = Max(vf, Min(CP.Y(), vl));

      const Standard_Boolean  isUbound = ((uf <= CP.X()) && (CP.X() <= ul)) || isUClosedOrPeriodic,
                              isVbound = ((vf <= CP.Y()) && (CP.Y() <= vl)) || isVClosedOrPeriodic;

      //Point CP is in surface boundary.
      if(isUbound && isVbound)
        continue;

      isIntoBoundaries = Standard_False;

      //Values of overrun of surface boundaries 
      const Standard_Real dUpar = (isUPeriodic || isUbound) ? 0.0 : CP.X() - u, 
                          dVpar = (isVPeriodic || isVbound) ? 0.0 : CP.Y() - v;

      if(Abs(dUpar) > aDeltaUmax)
      {
        aCriticalParameterU = prm;
        aDeltaUmax = Abs(dUpar);
        dUmaxU = dUpar;
        dUmaxV = dVpar;
      }

      if(Abs(dVpar) > aDeltaVmax)
      {
        aCriticalParameterV = prm;
        aDeltaVmax = Abs(dVpar);
        dVmaxU = dUpar;
        dVmaxV = dVpar;
      }
    }//for (i = 0; i< aNbControl; ++i)

    if(!isIntoBoundaries)
    {
      // Max U(V) resolution is calculated from condition:
      // |S'|*dU > aCoeff*(0.5*|S"|dU*dU), it means that
      // first order differential of surface >> second order one
      const Standard_Real aCoeff = 10.;
      //Value to check modulus of derivatives against zero
      const Standard_Real eps = 1.e-16;
      //Value for estimation Max resolution if |S"| < eps
      const Standard_Real aFactor = 10.;

      for(Standard_Integer anIndex = 0; anIndex < 2; anIndex++)
      {
        const Standard_Real aParam = !anIndex ? aCriticalParameterU : aCriticalParameterV;
        const Standard_Real du = !anIndex ? dUmaxU : dUmaxV;
        const Standard_Real dv = !anIndex ? dVmaxU : dVmaxV;

        const gp_Pnt2d CP = Other.GetCurve()->Value(aParam);

        const Standard_Real u = Max(uf, Min(CP.X(), ul));
        const Standard_Real v = Max(vf, Min(CP.Y(), vl));

        gp_Pnt aPref;
        gp_Vec aDSdu, aDSdv, aD2Sdu2, aD2Sdv2, aD2Sdudv;
        Other.GetSurface()->D2(u, v, aPref, aDSdu, aDSdv, aD2Sdu2, aD2Sdv2, aD2Sdudv);

        Standard_Real UResMax = 0.0, VResMax = 0.0;
        const Standard_Real aModDSdu = aDSdu.Magnitude();
        const Standard_Real aModDSdv = aDSdv.Magnitude();
        const Standard_Real aModD2Sdu2 = aD2Sdu2.Magnitude();
        const Standard_Real aModD2Sdv2 = aD2Sdv2.Magnitude();

        if(aModDSdu > eps)
        {
          if(aModD2Sdu2 > aModDSdu / aCoeff)
          {
            UResMax = aModDSdu / (.5 * aCoeff * aModD2Sdu2);
          }
          else
          {
            //Surface seems to be "plane-like" in U direction
            UResMax = aDeltaU / aFactor;
          }
        }
        else
        {
          UResMax = aFactor * UResSt;
        }

        if(aModDSdv > eps)
        {
          if(aModD2Sdv2 > aModDSdv / aCoeff)
          {
            VResMax = aModDSdv / (5. * aModD2Sdv2);
          }
          else
          {
            //Surface seems to be "plane-like" in V direction
            VResMax = aDeltaV / aFactor;
          }
        }
        else
        {
          VResMax = aFactor * VResSt;
        }

        const Standard_Real anURes = isBaseSurfUPeriodic ? UResSt : UResMax,
                            aVRes  = isBaseSurfVPeriodic ? VResSt : VResMax;

        const Standard_Boolean  isUbound = ((uf - anURes <= CP.X()) && (CP.X() <= ul + anURes)) || isUClosedOrPeriodic,
                                isVbound = ((vf - aVRes <= CP.Y())  && (CP.Y() <= vl + aVRes))  || isVClosedOrPeriodic;

        if(isUbound && isVbound)
          continue;

#ifdef DEB
        if(!isUbound)
        {
          cout << endl << "----\nFunction Validate(...); file: "
                                            "BRepCheck_Edge.cxx" << endl;

          if(isBaseSurfUPeriodic)
            cout << "RTS from U-periodic" << endl;
          else if(theSurfIsUPeriodic)
            cout << "U-periodic surface" << endl;
          else if(isUClosedOrPeriodic)
            cout << "U-closed surface" << endl;


          cout << "Point(prm = " << aParam << "): (" << 
                                CP.X() << "; " << CP.Y() <<")."  << endl;
          cout << "u = (" << uf << ")...(" << ul << "). "
            "Delta = " << Max(uf - CP.X(),CP.X()-ul) << " Tol3D = " << Tol << 
                                  ". URes = " << anURes << "\n-----" << endl;
        }

        if(!isVbound)
        {
          cout << endl << "----\nFunction Validate(...); file: "
                                                "BRepCheck_Edge.cxx" << endl;

          if(isBaseSurfVPeriodic)
            cout << "RTS from V-periodic" << endl;
          else if(theSurfIsVPeriodic)
            cout << "V-periodic surface" << endl;
          else if(isVClosedOrPeriodic)
            cout << "V-closed surface" << endl;

          cout << "Point(prm = " << aParam << "): (" << 
                                    CP.X() << "; " << CP.Y() <<")."  << endl;
          cout << "v = (" << vf << ")...(" << vl << "). "
            "Delta = " << Max(vf - CP.Y(),CP.Y()-vl) << " Tol3D = " << Tol << 
                                  ". VRes = " << aVRes << "\n-----"  << endl;
        }
#endif

        //Expected and real point
        gp_Pnt aPe, aPf;
        //1st degree estimation
        aPe.SetXYZ(gp_XYZ(aPref.X() + (aDSdu.X()*du+aDSdv.X()*dv),
                          aPref.Y() + (aDSdu.Y()*du+aDSdv.Y()*dv),
                          aPref.Z() + (aDSdu.Z()*du+aDSdv.Z()*dv)));

        Other.GetSurface()->D0(CP.X(), CP.Y(), aPf);
        const Standard_Real aTol = Tol2;
        Standard_Real dist = aPe.SquareDistance(aPf);

        if(dist < aTol)
          continue;

#ifdef DEB
        cout << endl << "++++\nFunction Validate(...); file: "
                            "BRepCheck_Edge.cxx (1st degree)" << endl;
        cout << "Exp. point: (" << aPe.X() << ", " << aPe.Y() << ", " <<
                                              aPe.Z() << ")." << endl;
        cout << "Real point: (" << aPf.X() << ", " << aPf.Y() << ", " <<
                                              aPf.Z() << ")." << endl;
        cout << "dist**2 = " << dist << 
                              "; Tol = " << aTol << "\n-----" << endl;
#endif

        const Standard_Real dUpar2 = du*du;
        const Standard_Real dVpar2 = dv*dv;
        const Standard_Real dUVpar = du*dv;

        //2nd degree estimation
        aPe.SetXYZ(gp_XYZ(aPe.X() + (aD2Sdu2.X()*dUpar2 + 
                            2.0*aD2Sdudv.X()*dUVpar + aD2Sdv2.X()*dVpar2)/2.0,
                          aPe.Y() + (aD2Sdu2.Y()*dUpar2 + 
                            2.0*aD2Sdudv.Y()*dUVpar + aD2Sdv2.Y()*dVpar2)/2.0,
                          aPe.Z() + (aD2Sdu2.Z()*dUpar2 + 
                            2.0*aD2Sdudv.Z()*dUVpar + aD2Sdv2.Z()*dVpar2)/2.0));

        dist = aPe.SquareDistance(aPf);

        if(dist > aTol)
        {
#ifdef DEB
          cout << endl << "++++\nFunction Validate(...); file: "
            "BRepCheck_Edge.cxx (2nd degree)" << endl;
          cout << "Exp. point: (" << aPe.X() << ", " << aPe.Y() << ", " <<
                                                  aPe.Z() << ")." << endl;
          cout << "Real point: (" << aPf.X() << ", " << aPf.Y() << ", " <<
                                                  aPf.Z() << ")." << endl;
          cout << "dist**2 = " << dist << 
                                  "; Tol = " << aTol << "\n-----" << endl;
#endif
          Status = BRepCheck_PCurveIsOutOfDomainFace;
          return Status;
        }
      }//for(Standard_Integer anIndex = 0; anIndex < 2; anIndex++)
    }//if(!isIntoBoundaries)
  }//if (!proj)
  else
  {
    Extrema_LocateExtPC refd,otherd;
    Standard_Real OFirst = Other.FirstParameter();
    Standard_Real OLast  = Other.LastParameter();
    gp_Pnt pd = CRef.Value(First);
    gp_Pnt pdo = Other.Value(OFirst);
    Standard_Real distt = pd.SquareDistance(pdo);
    
    if (distt > Tol*Tol)
    {
      const gp_Pnt problematic_point(pd);
      Status = BRepCheck_InvalidCurveOnSurface ;
      const Standard_Real Error = Sqrt(distt);
      PrintProblematicPoint(problematic_point, Error, Tol);
      return Status;
      //goto FINISH ;
    }

    pd = CRef.Value(Last);
    pdo = Other.Value(OLast);
    distt = pd.SquareDistance(pdo);

    if (distt > Tol*Tol)
    {
      const gp_Pnt problematic_point(pd);
      Status = BRepCheck_InvalidCurveOnSurface ;
      const Standard_Real Error = Sqrt(distt);
      PrintProblematicPoint(problematic_point, Error, Tol);
      return Status;
      //goto FINISH ;
    }

    refd.Initialize(CRef,First,Last,CRef.Resolution(Tol));
    otherd.Initialize(Other,OFirst,OLast,Other.Resolution(Tol));

    for (Standard_Integer i = 2; i< aNbControl-1; i++)
    {
      Standard_Real rprm = ((aNbControl-1-i)*First + i*Last)/(aNbControl-1);
      gp_Pnt pref = CRef.Value(rprm);
      Standard_Real oprm = ((aNbControl-1-i)*OFirst + i*OLast)/(aNbControl-1);
      gp_Pnt pother = Other.Value(oprm);
      refd.Perform(pother,rprm);

      if (!refd.IsDone() || refd.SquareDistance() > Tol * Tol)
      {
        const gp_Pnt problematic_point(pref);
        Status = BRepCheck_InvalidCurveOnSurface ;
        
        const Standard_Real Error = refd.IsDone() ? 
                                      sqrt (refd.SquareDistance()) :
                                      RealLast();

        PrintProblematicPoint(problematic_point, Error, Tol);
        return Status;
        //goto FINISH ;
      }

      otherd.Perform(pref,oprm);

      if (!otherd.IsDone() || otherd.SquareDistance() > Tol * Tol)
      {
        const gp_Pnt problematic_point(pref);
        Status = BRepCheck_InvalidCurveOnSurface ;

        const Standard_Real Error = otherd.IsDone() ? 
                                      sqrt (otherd.SquareDistance()) :
                                      RealLast();

        PrintProblematicPoint(problematic_point, Error, Tol);
        return Status;
        //goto FINISH ;
      }
    }
  }
  
  return Status ;
}

//=======================================================================
//function : Prec
//purpose  : 
//=======================================================================
Standard_Real Prec(const Adaptor3d_Curve& aAC3D,
		   const Adaptor3d_CurveOnSurface& aACS)
{
  Standard_Real aXEmax, aXC, aXS;
  //
  aXC=PrecCurve(aAC3D);
  aXS=PrecSurface(aACS);
  aXEmax=(aXC>aXS) ? aXC: aXS;
  return aXEmax;
}
//=======================================================================
//function : PrecCurve
//purpose  : 
//=======================================================================
Standard_Real PrecCurve(const Adaptor3d_Curve& aAC3D)
{
  Standard_Real aXEmax;
  GeomAbs_CurveType aCT;
  //
  aXEmax=RealEpsilon(); 
  //
  aCT=aAC3D.GetType();
  if (aCT==GeomAbs_Ellipse) {
    Standard_Integer i;
    Standard_Real aX[5], aXE;
    //
    gp_Elips aEL3D=aAC3D.Ellipse();
    aEL3D.Location().Coord(aX[0], aX[1], aX[2]);
    aX[3]=aEL3D.MajorRadius();
    aX[4]=aEL3D.MinorRadius();
    aXEmax=-1.;
    for (i=0; i<5; ++i) {
      if (aX[i]<0.) {
	aX[i]=-aX[i];
      }
      aXE=Epsilon(aX[i]);
      if (aXE>aXEmax) {
	aXEmax=aXE;
      }
    }
  }//if (aCT=GeomAbs_Ellipse) {
  //
  return aXEmax;
}
//=======================================================================
//function : PrecSurface
//purpose  : 
//=======================================================================
Standard_Real PrecSurface(const Adaptor3d_CurveOnSurface& aACS)
{
  Standard_Real aXEmax;
  GeomAbs_SurfaceType aST;
  //
  aXEmax=RealEpsilon(); 
  //
  const Handle(Adaptor3d_HSurface)& aAHS=aACS.GetSurface();
  aST=aAHS->GetType();
  if (aST==GeomAbs_Cone) {
    gp_Cone aCone=aAHS->Cone();
    Standard_Integer i;
    Standard_Real aX[4], aXE;
    //
    aCone.Location().Coord(aX[0], aX[1], aX[2]);
    aX[3]=aCone.RefRadius();
    aXEmax=-1.;
    for (i=0; i<4; ++i) {
      if (aX[i]<0.) {
	aX[i]=-aX[i];
      }
      aXE=Epsilon(aX[i]);
      if (aXE>aXEmax) {
	aXEmax=aXE;
      }
    }
  }//if (aST==GeomAbs_Cone) {
  return aXEmax;
}
//=======================================================================
//function : PrintProblematicPoint
//purpose  : 
//=======================================================================
#ifdef DEB
void PrintProblematicPoint(const gp_Pnt& problematic_point,
			   const Standard_Real Error,
			   const Standard_Real Tol)
{
  cout << " **** probleme de SameParameter au point :" << endl;
  cout << "         " << problematic_point.Coord(1) << " " 
    << problematic_point.Coord(2) << " " << problematic_point.Coord(3) << endl ;
  cout << "   Erreur detectee :" << Error << " Tolerance :" << Tol << endl;
}
#else
void PrintProblematicPoint(const gp_Pnt&,
			   const Standard_Real,
			   const Standard_Real)
{
}
#endif

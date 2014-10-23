

#include <BRepCheck_SurfNormAnalyzer.hxx>
#include <TopExp_Explorer.hxx>
#include <TopTools_ListOfShape.hxx>
#include <TopoDS.hxx>
#include <TopoDS_Face.hxx>
#include <BRep_Tool.hxx>
#include <BRepTools.hxx>

#include <Geom_Surface.hxx>
#include <GeomAdaptor_Surface.hxx>
#include <GeomAdaptor_HSurface.hxx>
#include <Adaptor3d_TopolTool.hxx>
#include <TColStd_Array1OfReal.hxx>
#include <TColStd_Array2OfReal.hxx>
#include <gp_VectorWithNullMagnitude.hxx>

static Standard_Real AdjustExtr(const Adaptor3d_Surface& S,
                                const Standard_Real UMin,
                                const Standard_Real UMax,
                                const Standard_Real VMin,
                                const Standard_Real VMax,
                                const Standard_Real Extr0,
                                const Standard_Real Tol,
                                      Standard_Real& uextr,
                                      Standard_Real& vextr );



//=======================================================================
//function : BRepCheck_Analyzer
//purpose  : 
//=======================================================================

BRepCheck_SurfNormAnalyzer::BRepCheck_SurfNormAnalyzer ():
myAngTol(0.01)
{
  
}


//=======================================================================
//function : BRepCheck_Analyzer
//purpose  : 
//=======================================================================

BRepCheck_SurfNormAnalyzer::BRepCheck_SurfNormAnalyzer 
                                        (const TopoDS_Shape& theS,
					                               const Standard_Real theAngTol):
myShape(theS), myAngTol(theAngTol)
{
  
}



//=======================================================================
//function : IsValid
//purpose  : 
//=======================================================================

Standard_Boolean BRepCheck_SurfNormAnalyzer::IsValid() 
{
  TopLoc_Location anL;
  myBadFaces.Clear();
  Standard_Real umin, umax, vmin, vmax;
  TopExp_Explorer anEx(myShape, TopAbs_FACE);
  for(; anEx.More(); anEx.Next())
  {
    TopoDS_Face aF = TopoDS::Face(anEx.Current());
    BRepTools::UVBounds(aF, umin, umax, vmin, vmax);
    const Handle(Geom_Surface)& aSurf = BRep_Tool::Surface(aF, anL);
    if(IsProblemSurf(aSurf, umin, umax, vmin, vmax, myAngTol))
    {
      myBadFaces.Append(aF);
    }
  }  
  

  return myBadFaces.IsEmpty();
}
//=======================================================================
//function : BadFaces
//purpose  : 
//=======================================================================
const TopTools_ListOfShape& BRepCheck_SurfNormAnalyzer::BadFaces() const
{
  return myBadFaces;
}
//=======================================================================
//function : SetShape
//purpose  : 
//=======================================================================

void BRepCheck_SurfNormAnalyzer::SetShape(const TopoDS_Shape& theShape) 
{
  myBadFaces.Clear();
  myShape = theShape;
}
//=======================================================================
//function : SetTolerance
//purpose  : 
//=======================================================================

void BRepCheck_SurfNormAnalyzer::SetTolerance(const Standard_Real theAngTol) 
{
  myBadFaces.Clear();
  myAngTol = theAngTol;
}
//=======================================================================
//function : GetShape
//purpose  : 
//=======================================================================

const TopoDS_Shape& BRepCheck_SurfNormAnalyzer::GetShape() const 
{
  return myShape;
}
//=======================================================================
//function : GetTolerance
//purpose  : 
//=======================================================================

Standard_Real BRepCheck_SurfNormAnalyzer::GetTolerance() const 
{
  return myAngTol;
}
//=======================================================================
//function : IsProblemSurf
//purpose  : 
//=======================================================================

Standard_Boolean 
BRepCheck_SurfNormAnalyzer::IsProblemSurf(const Handle_Geom_Surface& theSurf, 
                                          const Standard_Real theUMin,
                                          const Standard_Real theUMax,
                                          const Standard_Real theVMin,
                                          const Standard_Real theVMax,                                                       
                                          const Standard_Real theAngTol)
{
  Standard_Boolean aStatus = Standard_False;
  GeomAdaptor_Surface aGAS(theSurf, theUMin, theUMax, theVMin, theVMax);
  GeomAbs_SurfaceType aSType = aGAS.GetType();
  //
  if(aSType <= GeomAbs_Torus)
  {
    return aStatus;
  }
  //
  Handle(GeomAdaptor_HSurface) aGAHS = new GeomAdaptor_HSurface(aGAS);
  Handle(Adaptor3d_TopolTool) aTT = new Adaptor3d_TopolTool(aGAHS);
  if(aTT->DomainIsInfinite())
  {
    return aStatus;
  }
  //
  Standard_Real aDefl = 0.1; //the same as for intersection default
  Standard_Integer aNbMinU = 10, aNbMinV = 10;
  aTT->SamplePnts(aDefl, aNbMinU, aNbMinV);
  Standard_Integer aNbU = aTT->NbSamplesU();
  Standard_Integer aNbV = aTT->NbSamplesV();
  TColStd_Array1OfReal anUPars(1, aNbU), aVPars(1, aNbV);
  aTT->UParameters(anUPars);
  aTT->VParameters(aVPars);
  //
  gp_Pnt aP;
  gp_Vec aDU, aDV;
  Standard_Real u, v, ang;
  TColStd_Array2OfReal aTabN(1, aNbU, 1, aNbV);
  Standard_Integer i, j;
  //Check singular point on boundary
  for(j = 1; j <= aNbV; j += aNbV-1)
  {
    aP = theSurf->Value(anUPars(1), aVPars(j));
    Standard_Real length = 0.;
    for(i = 2; i <= aNbU; ++i)
    {
      gp_Pnt aPcur = theSurf->Value(anUPars(i), aVPars(j));
      length += aPcur.Distance(aP);
      if(length > Precision::Confusion())
      {
        break;
      }
      aP = aPcur;
    }
    if(length <= Precision::Confusion())
    {
      if(j == 1)
      {
        aVPars(j) += (aVPars(2)-aVPars(1)) / 10.;
      }
      else
      {
        aVPars(aNbV) -= (aVPars(aNbV)-aVPars(aNbV-1)) / 10.;
      }
    }
  }
  for(j = 1; j <= aNbU; j += aNbU-1)
  {
    aP = theSurf->Value(anUPars(j), aVPars(1));
    Standard_Real length = 0.;
    for(i = 2; i <= aNbV; ++i)
    {
      gp_Pnt aPcur = theSurf->Value(anUPars(j), aVPars(i));
      length += aPcur.Distance(aP);
      if(length > Precision::Confusion())
      {
        break;
      }
      aP = aPcur;
    }
    if(length <= Precision::Confusion())
    {
      if(j == 1)
      {
        anUPars(j) += (anUPars(2)-anUPars(1)) / 10.;
      }
      else
      {
        anUPars(aNbU) -= (anUPars(aNbU)-anUPars(aNbU-1)) / 10.;
      }
    }
  }
  //
  for(i = 1; i <= aNbU; ++i)
  {
    u = anUPars(i);
    for(j = 1; j <= aNbV; ++j)
    {
      v = aVPars(j);
      theSurf->D1(u, v, aP, aDU, aDV);
      try
      {
        ang = aDU.Angle(aDV);
        if(ang > M_PI/2.)
        {
          ang = M_PI - ang;
        }
      }
      catch (gp_VectorWithNullMagnitude)
      {
        ang = 0.;
      }

      aTabN(i, j) = ang;
    }
  }
  //
  Standard_Real min = RealLast();
  Standard_Integer imin = 0, jmin = 0;
  for(i = 1; i <= aNbU; ++i)
  {
    for(j = 1; j <= aNbV; ++j)
    {
      if(aTabN(i, j) < theAngTol)
      {
        return Standard_True;
      }
      else
      {
        if(aTabN(i, j) < min)
        {
          min = aTabN(i, j);
          imin = i;
          jmin = j;
        }
      }
    }
  }
  //
  Standard_Real umin = anUPars(Max(1, imin-1));
  Standard_Real umax = anUPars(Min(aNbU, imin+1));
  Standard_Real vmin = aVPars(Max(1, jmin-1));
  Standard_Real vmax = aVPars(Min(aNbV, jmin+1));
  //
  Standard_Real min0 = min, uextr = anUPars(imin), vextr = aVPars(jmin);
  min = AdjustExtr(aGAS, umin, umax, vmin, vmax,
                      min0,  theAngTol / 10., uextr, vextr  );

  if(min < theAngTol)
  {
    aStatus = Standard_True;
  }
  return aStatus;
}
Standard_Real AdjustExtr(const Adaptor3d_Surface& S,
                         const Standard_Real UMin,
                         const Standard_Real UMax,
                         const Standard_Real VMin,
                         const Standard_Real VMax,
                         const Standard_Real Extr0,
                         const Standard_Real Tol,
                         Standard_Real& uextr,
                         Standard_Real& vextr )
{
  Standard_Integer Nu = 5, Nv = 5;
  gp_Pnt P;
  gp_Vec DU, DV;
  Standard_Integer i, j;
  Standard_Real du = (UMax-UMin)/(Nu-1);
  Standard_Real dv = (VMax-VMin)/(Nv-1);
  Standard_Real extr = Extr0;
  Standard_Real u, v, ang;
  for (i = 1, u = UMin; i <= Nu; i++, u += du){
    for (j = 1, v = VMin;j <= Nv; j++, v += dv){
      S.D1(u,v,P,DU,DV);
      try
      {
        ang = DU.Angle(DV);
        if(ang > M_PI/2.)
        {
          ang = M_PI - ang;
        }
      }
      catch (gp_VectorWithNullMagnitude)
      {
        ang = 0.;
      }
      //
      if(extr > ang)
      {
        extr = ang;
        uextr = u;
        vextr = v;
      }
    }
  }

  if(Abs(extr - Extr0) > Tol)
  {
    Standard_Real umin, umax, vmin, vmax;
    umin = Max(UMin, uextr - du);
    umax = Min(UMax, uextr + du);
    vmin = Max(VMin, vextr - dv);
    vmax = Min(VMax, vextr + dv);
    Standard_Real extr0 = extr;
    extr = AdjustExtr(S, umin, umax, vmin, vmax,
                      extr0,  Tol, uextr, vextr);
  }
  return extr;
}
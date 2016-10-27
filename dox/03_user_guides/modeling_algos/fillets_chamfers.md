Fillets and Chamfers {#user_guides__modeling_algos_fillets_chamfers}
====================

@tableofcontents

This library provides algorithms to make fillets and chamfers on shape edges.
The following cases are addressed:

  * Corners and apexes with different radii; 
  * Corners and apexes with different concavity. 

If there is a concavity, both surfaces that need to be extended and those, which do not, are processed.

@section occt_modalg_6_1 Fillets  
@subsection occt_modalg_6_1_1 Fillet on shape

A fillet is a smooth  face replacing a sharp edge.

*BRepFilletAPI_MakeFillet* class allows filleting a shape.  

To produce a fillet, it is necessary to define the filleted shape at the construction of the class and  add fillet  descriptions using the *Add* method.

A fillet description contains an edge and a  radius. The edge must be shared by two faces. The fillet is automatically extended to all edges in a smooth continuity with the original  edge. It is not an error to add a fillet twice,  the last description holds. 

@image html /03_user_guides/modeling_algos/images/modeling_algos_image038.png "Filleting two edges using radii r1 and  r2."
@image latex /03_user_guides/modeling_algos/images/modeling_algos_image038.png "Filleting two edges using radii r1 and  r2."

In the following example  a filleted box with dimensions a,b,c and radius r is created. 

### Constant  radius 


~~~~~
#include <TopoDS_Shape.hxx> 
#include <TopoDS.hxx> 
#include <BRepPrimAPI_MakeBox.hxx> 
#include <TopoDS_Solid.hxx> 
#include <BRepFilletAPI_MakeFillet.hxx> 
#include <TopExp_Explorer.hxx> 

TopoDS_Shape FilletedBox(const Standard_Real a, 
						const Standard_Real  b, 
						const Standard_Real  c, 
						const Standard_Real  r) 
{ 
	TopoDS_Solid Box =  BRepPrimAPI_MakeBox(a,b,c); 
	BRepFilletAPI_MakeFillet  MF(Box); 

	// add all the edges  to fillet 
	TopExp_Explorer  ex(Box,TopAbs_EDGE); 
	while (ex.More()) 
	{ 
	MF.Add(r,TopoDS::Edge(ex.Current())); 
	ex.Next(); 
	} 
	return MF.Shape(); 
	} 
~~~~~

@image html /03_user_guides/modeling_algos/images/modeling_algos_image039.png "Fillet with constant radius"
@image latex /03_user_guides/modeling_algos/images/modeling_algos_image039.png "Fillet with constant radius"

#### Changing radius


~~~~~
void CSampleTopologicalOperationsDoc::OnEvolvedblend1() 
{ 
	TopoDS_Shape theBox  = BRepPrimAPI_MakeBox(200,200,200); 

	BRepFilletAPI_MakeFillet  Rake(theBox); 
	ChFi3d_FilletShape  FSh = ChFi3d_Rational; 
	Rake.SetFilletShape(FSh); 

	TColgp_Array1OfPnt2d  ParAndRad(1, 6); 
	ParAndRad(1).SetCoord(0.,  10.); 
	ParAndRad(1).SetCoord(50.,  20.); 
	ParAndRad(1).SetCoord(70.,  20.); 
	ParAndRad(1).SetCoord(130.,  60.); 
	ParAndRad(1).SetCoord(160.,  30.); 
	ParAndRad(1).SetCoord(200.,  20.); 

	TopExp_Explorer  ex(theBox,TopAbs_EDGE); 
	Rake.Add(ParAndRad, TopoDS::Edge(ex.Current())); 
	TopoDS_Shape  evolvedBox = Rake.Shape(); 
} 
~~~~~

@image html /03_user_guides/modeling_algos/images/modeling_algos_image040.png	"Fillet with changing radius"
@image latex /03_user_guides/modeling_algos/images/modeling_algos_image040.png	"Fillet with changing radius"
 
@subsection occt_modalg_6_1_3 Fillet on a planar face

*BRepFilletAPI_MakeFillet2d* class allows constructing fillets and chamfers on planar faces. 
To create a fillet on planar face: define it, indicate, which vertex is  to be deleted, and give the fillet radius with *AddFillet* method. 

A chamfer can be calculated with *AddChamfer* method. It can be  described by 
  * two edges and two distances
  * one edge, one vertex, one  distance and one angle.
Fillets and chamfers are calculated when addition is  complete. 

If face F2 is created by 2D fillet and chamfer builder from face F1, the builder can be rebuilt (the  builder recovers the status it had before deletion). To do so, use the  following syntax: 
~~~~~
BRepFilletAPI_MakeFillet2d builder; 
builder.Init(F1,F2); 
~~~~~

Planar Fillet
-------------

~~~~~
#include “BRepPrimAPI_MakeBox.hxx” 
#include “TopoDS_Shape.hxx” 
#include “TopExp_Explorer.hxx” 
#include “BRepFilletAPI_MakeFillet2d.hxx” 
#include “TopoDS.hxx” 
#include “TopoDS_Solid.hxx” 

TopoDS_Shape FilletFace(const Standard_Real a, 
						const Standard_Real  b, 
						const Standard_Real c, 
						const Standard_Real  r) 

{ 
	TopoDS_Solid Box =  BRepPrimAPI_MakeBox (a,b,c); 
	TopExp_Explorer  ex1(Box,TopAbs_FACE); 

	const  TopoDS_Face& F = TopoDS::Face(ex1.Current()); 
	BRepFilletAPI_MakeFillet2d  MF(F); 
	TopExp_Explorer  ex2(F, TopAbs_VERTEX); 
	while (ex2.More()) 
	{ 
	MF.AddFillet(TopoDS::Vertex(ex2.Current()),r); 
	ex2.Next(); 
	} 
	// while... 
	return MF.Shape(); 
} 
~~~~~

@section occt_modalg_6_1_2 Chamfer

A chamfer is a rectilinear edge  replacing a sharp vertex of the face.

The use of *BRepFilletAPI_MakeChamfer* class is similar to the use of  *BRepFilletAPI_MakeFillet*, except for the following: 
* The surfaces created are  ruled and not smooth. 
* The *Add* syntax for  selecting edges requires one or two distances, one edge and one face  (contiguous to the edge).

~~~~~ 
Add(dist,  E, F) 
Add(d1,  d2, E, F) with d1 on the face F. 
~~~~~

@image html /03_user_guides/modeling_algos/images/modeling_algos_image041.png	"Chamfer"
@image latex /03_user_guides/modeling_algos/images/modeling_algos_image041.png	"Chamfer"
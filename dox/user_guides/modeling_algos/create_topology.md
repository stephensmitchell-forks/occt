Create Topology {#user_guides__modeling_algos_create_topology}
===============

@tableofcontents

The following  standard topological objects can be created:
  * Vertices
  * Edges
  * Faces
  * Wires
  * Polygonal wires
  * Shells
  * Solids.

There are two root classes for their construction and modification: 
* The deferred class  *BRepBuilderAPI_MakeShape* is the root of all *BRepBuilderAPI* classes,  which build shapes. It inherits from the class *BRepBuilderAPI_Command* and provides a field to store the constructed shape. 
* The deferred class *BRepBuilderAPI_ModifyShape* is used as a root for the shape  modifications. It inherits *BRepBuilderAPI_MakeShape* and implements the methods  used to trace the history of all sub-shapes. 

@section occt_modalg_3_1 Vertex

*BRepBuilderAPI_MakeVertex*  creates a new vertex from a 3D point from gp. 
~~~~~
gp_Pnt P(0,0,10); 
TopoDS_Vertex V = BRepBuilderAPI_MakeVertex(P); 
~~~~~

This class always creates a new vertex and has no other methods.

@section occt_modalg_3_2 Edge

@subsection occt_modalg_3_2_1 Basic edge construction method

Use *BRepBuilderAPI_MakeEdge* to create from a curve and vertices. The basic method constructs an edge from a curve, two vertices, and two parameters. 

~~~~~
Handle(Geom_Curve) C = ...; // a curve 
TopoDS_Vertex V1 = ...,V2 = ...;// two Vertices 
Standard_Real p1 = ..., p2 = ..;// two parameters 
TopoDS_Edge E = BRepBuilderAPI_MakeEdge(C,V1,V2,p1,p2); 
~~~~~

where C is the domain of the edge; V1 is the first vertex oriented FORWARD; V2 is the second vertex oriented REVERSED; p1  and p2 are the parameters for the vertices V1 and V2 on the curve. The default  tolerance is associated with this edge. 

@image html /user_guides/modeling_algos/images/modeling_algos_image022.png "Basic Edge Construction"
@image latex /user_guides/modeling_algos/images/modeling_algos_image022.png "Basic Edge Construction"

The following rules  apply to the arguments: 

**The curve**
  * Must not be a Null Handle.
  * If the curve is a trimmed  curve, the basis curve is used.

**The vertices** 
  * Can be null shapes. When V1  or V2 is Null the edge is open in the corresponding direction and the  corresponding parameter p1 or p2 must be infinite (i.e p1 is RealFirst(),  p2 is RealLast()).
  * Must be different vertices if  they have different 3d locations and identical vertices if they have the same  3d location (identical vertices are used when the curve is closed).

**The parameters**
  * Must be increasing and in the  range of the curve, i.e.:

~~~~~
  C->FirstParameter() <=  p1 < p2 <= C->LastParameter() 
~~~~~  
  
  * If the parameters are  decreasing, the Vertices are switched, i.e. V2 becomes V1 and V1 becomes V2.
  * On a periodic curve the  parameters p1 and p2 are adjusted by adding or subtracting the period to obtain  p1 in the range of the curve and p2 in the range p1 < p2 <= p1+ Period.  So on a parametric curve p2 can be greater than the second parameter,  see the figure below.
  * Can be infinite but the  corresponding vertex must be Null (see above).
  * The distance between the Vertex 3d location and the point  evaluated on the curve with the parameter must be lower than the default  precision.

The figure below  illustrates two special cases, a semi-infinite edge and an edge on a periodic  curve. 

@image html /user_guides/modeling_algos/images/modeling_algos_image023.png   "Infinite and Periodic Edges"
@image latex /user_guides/modeling_algos/images/modeling_algos_image023.png   "Infinite and Periodic Edges"

@subsection occt_modalg_3_2_2 Supplementary edge construction methods

There exist supplementary edge construction methods derived from the basic one. 

*BRepBuilderAPI_MakeEdge* class provides methods, which are all simplified calls  of the previous one: 

  * The parameters can be  omitted. They are computed by projecting the vertices on the curve.
  * 3d points (Pnt from gp) can  be given in place of vertices. Vertices are created from the points. Giving  vertices is useful when creating connected vertices.
  * The vertices or points can be  omitted if the parameters are given. The points are computed by evaluating the  parameters on the curve.
  * The vertices or points and  the parameters can be omitted. The first and the last parameters of the curve are used.

The five following  methods are thus derived from the basic construction: 

~~~~~
Handle(Geom_Curve) C = ...; // a curve 
TopoDS_Vertex V1 = ...,V2 = ...;// two Vertices 
Standard_Real p1 = ..., p2 = ..;// two parameters 
gp_Pnt P1 = ..., P2 = ...;// two points 
TopoDS_Edge E; 
// project the vertices on the curve 
E = BRepBuilderAPI_MakeEdge(C,V1,V2); 
// Make vertices from points 
E = BRepBuilderAPI_MakeEdge(C,P1,P2,p1,p2); 
// Make vertices from points and project them 
E = BRepBuilderAPI_MakeEdge(C,P1,P2); 
// Computes the points from the parameters 
E = BRepBuilderAPI_MakeEdge(C,p1,p2); 
// Make an edge from the whole curve 
E = BRepBuilderAPI_MakeEdge(C); 
~~~~~

Six methods (the five above and the basic method) are also provided for curves from the gp package in  place of Curve from Geom. The methods create the corresponding Curve from Geom  and are implemented for the following classes: 

*gp_Lin*       creates a  *Geom_Line* 
*gp_Circ*      creates a  *Geom_Circle* 
*gp_Elips*    creates a  *Geom_Ellipse* 
*gp_Hypr*    creates a  *Geom_Hyperbola* 
*gp_Parab*   creates a  *Geom_Parabola* 

There are also two  methods to construct edges from two vertices or two points. These methods  assume that the curve is a line; the vertices or points must have different  locations. 

~~~~~

TopoDS_Vertex V1 = ...,V2 = ...;// two Vertices 
gp_Pnt P1 = ..., P2 = ...;// two points 
TopoDS_Edge E; 

// linear edge from two vertices 
E = BRepBuilderAPI_MakeEdge(V1,V2); 

// linear edge from two points 
E = BRepBuilderAPI_MakeEdge(P1,P2); 
~~~~~

@subsection occt_modalg_3_2_3 Other information and error status

The class *BRepBuilderAPI_MakeEdge* can provide extra information and return an error status. 

If *BRepBuilderAPI_MakeEdge* is used as a class, it can provide two vertices. This is useful when  the vertices were not provided as arguments, for example when the edge was  constructed from a curve and parameters. The two methods *Vertex1* and *Vertex2*  return the vertices. Note that the returned vertices can be null if the edge is  open in the corresponding direction. 
The *Error* method  returns a term of the *BRepBuilderAPI_EdgeError* enumeration. It can be used to analyze the error when *IsDone* method returns False. The terms are: 

  * **EdgeDone** -- No error occurred, *IsDone* returns True. 
  * **PointProjectionFailed** -- No parameters were given, but the projection of  the 3D points on the curve failed. This happens if the point distance to the  curve is greater than the precision. 
  * **ParameterOutOfRange** -- The given parameters are not in the range  *C->FirstParameter()*, *C->LastParameter()* 
  * **DifferentPointsOnClosedCurve** --  The  two vertices or points have different locations but they are the extremities of  a closed curve. 
  * **PointWithInfiniteParameter** -- A finite coordinate point was associated with an  infinite parameter (see the Precision package for a definition of infinite  values). 
  * **DifferentsPointAndParameter**  -- The distance of the 3D point and the point  evaluated on the curve with the parameter is greater than the precision. 
  * **LineThroughIdenticPoints** -- Two identical points were given to define a line  (construction of an edge without curve), *gp::Resolution* is used to test confusion . 

The following example  creates a rectangle centered on the origin of dimensions H, L with fillets of radius R. The edges and the vertices are stored in the arrays *theEdges* and *theVertices*. We use class *Array1OfShape* (i.e. not arrays of edges or vertices).  See the image below. 

@image html /user_guides/modeling_algos/images/modeling_algos_image024.png "Creating a Wire"
@image latex /user_guides/modeling_algos/images/modeling_algos_image024.png "Creating a Wire"

~~~~~
#include <BRepBuilderAPI_MakeEdge.hxx> 
#include <TopoDS_Shape.hxx> 
#include <gp_Circ.hxx> 
#include <gp.hxx> 
#include <TopoDS_Wire.hxx> 
#include <TopTools_Array1OfShape.hxx> 
#include <BRepBuilderAPI_MakeWire.hxx> 

// Use MakeArc method to make an edge and two vertices 
void MakeArc(Standard_Real x,Standard_Real y, 
Standard_Real R, 
Standard_Real ang, 
TopoDS_Shape& E, 
TopoDS_Shape& V1, 
TopoDS_Shape& V2) 
{ 
gp_Ax2 Origin = gp::XOY(); 
gp_Vec Offset(x, y, 0.); 
Origin.Translate(Offset); 
BRepBuilderAPI_MakeEdge 
ME(gp_Circ(Origin,R),  ang, ang+PI/2); 
E = ME; 
V1 = ME.Vertex1(); 
V2 = ME.Vertex2(); 
} 

TopoDS_Wire MakeFilletedRectangle(const Standard_Real H, 
const Standard_Real L, 
const Standard_Real  R) 
{ 
TopTools_Array1OfShape theEdges(1,8); 
TopTools_Array1OfShape theVertices(1,8); 

// First create the circular edges and the vertices 
// using the MakeArc function described above. 
void MakeArc(Standard_Real, Standard_Real, 
Standard_Real, Standard_Real, 
TopoDS_Shape&, TopoDS_Shape&,  TopoDS_Shape&); 

Standard_Real x = L/2 - R, y = H/2 - R; 
MakeArc(x,-y,R,3.*PI/2.,theEdges(2),theVertices(2), 
theVertices(3)); 
MakeArc(x,y,R,0.,theEdges(4),theVertices(4), 
theVertices(5)); 
MakeArc(-x,y,R,PI/2.,theEdges(6),theVertices(6), 
theVertices(7)); 
MakeArc(-x,-y,R,PI,theEdges(8),theVertices(8), 
theVertices(1)); 
// Create the linear edges 
for (Standard_Integer i = 1; i <= 7; i += 2) 
{ 
theEdges(i) = BRepBuilderAPI_MakeEdge 
(TopoDS::Vertex(theVertices(i)),TopoDS::Vertex 
(theVertices(i+1))); 
} 
// Create the wire using the BRepBuilderAPI_MakeWire 
BRepBuilderAPI_MakeWire MW; 
for (i = 1; i <= 8; i++) 
{ 
MW.Add(TopoDS::Edge(theEdges(i))); 
} 
return MW.Wire(); 
} 
~~~~~

@section occt_modalg_3_3 Edge 2D

Use *BRepBuilderAPI_MakeEdge2d* class to make  edges on a working plane from 2d curves. The working plane is a default value  of the *BRepBuilderAPI* package (see the *Plane* methods). 

*BRepBuilderAPI_MakeEdge2d* class is strictly similar to BRepBuilderAPI_MakeEdge, but it uses 2D geometry from gp and Geom2d instead of  3D geometry. 

@section occt_modalg_3_4 Polygon

*BRepBuilderAPI_MakePolygon* class is used to build polygonal wires from vertices  or points. Points are automatically changed to vertices as in  *BRepBuilderAPI_MakeEdge*. 

The basic usage of  *BRepBuilderAPI_MakePolygon* is to create a wire by adding vertices or points  using the Add method. At any moment, the current wire can be extracted. The  close method can be used to close the current wire. In the following example, a  closed wire is created from an array of points. 

~~~~~
#include <TopoDS_Wire.hxx> 
#include <BRepBuilderAPI_MakePolygon.hxx> 
#include <TColgp_Array1OfPnt.hxx> 

TopoDS_Wire ClosedPolygon(const TColgp_Array1OfPnt&  Points) 
{ 
BRepBuilderAPI_MakePolygon MP; 
for(Standard_Integer i=Points.Lower();i=Points.Upper();i++) 
{ 
MP.Add(Points(i)); 
} 
MP.Close(); 
return MP; 
} 
~~~~~

Short-cuts are provided  for 2, 3, or 4 points or vertices. Those methods have a Boolean last argument  to tell if the polygon is closed. The default value is False. 

Two examples: 

Example of a closed  triangle from three vertices:
~~~~~ 
TopoDS_Wire W =  BRepBuilderAPI_MakePolygon(V1,V2,V3,Standard_True); 
~~~~~

Example of an open  polygon from four points:
~~~~~
TopoDS_Wire W = BRepBuilderAPI_MakePolygon(P1,P2,P3,P4); 
~~~~~

*BRepBuilderAPI_MakePolygon* class maintains a current wire. The current wire can  be extracted at any moment and the construction can proceed to a longer wire.  After each point insertion, the class maintains the last created edge and  vertex, which are returned by the methods *Edge, FirstVertex* and *LastVertex*. 

When the added point or  vertex has the same location as the previous one it is not added to the current  wire but the most recently created edge becomes Null. The *Added* method  can be used to test this condition. The *MakePolygon* class never raises an  error. If no vertex has been added, the *Wire* is *Null*. If two vertices are at  the same location, no edge is created. 

@section occt_modalg_3_5 Face

Use *BRepBuilderAPI_MakeFace* class to create a face from a surface and wires. An underlying surface is  constructed from a surface and optional parametric values. Wires can be added  to the surface. A planar surface can be constructed from a wire. An error  status can be returned after face construction. 

@subsection occt_modalg_3_5_1 Basic face construction method

A face can be  constructed from a surface and four parameters to determine a limitation of the  UV space. The parameters are optional, if they are omitted the natural bounds  of the surface are used. Up to four edges and vertices are created with a wire.  No edge is created when the parameter is infinite. 

~~~~~
Handle(Geom_Surface) S = ...; // a surface 
Standard_Real umin,umax,vmin,vmax; // parameters 
TopoDS_Face F =  BRepBuilderAPI_MakeFace(S,umin,umax,vmin,vmax); 
~~~~~

@image html /user_guides/modeling_algos/images/modeling_algos_image025.png "Basic Face Construction"
@image latex /user_guides/modeling_algos/images/modeling_algos_image025.png "Basic Face Construction"

To make a face from the  natural boundary of a surface, the parameters are not required: 

~~~~~
Handle(Geom_Surface) S = ...; // a surface 
TopoDS_Face F = BRepBuilderAPI_MakeFace(S); 
~~~~~

Constraints on the  parameters are similar to the constraints in *BRepBuilderAPI_MakeEdge*. 
  * *umin,umax (vmin,vmax)* must be  in the range of the surface and must be increasing.
  * On a *U (V)* periodic surface  *umin* and *umax (vmin,vmax)* are adjusted.
  * *umin, umax, vmin, vmax* can be  infinite. There will be no edge in the corresponding direction.

@subsection occt_modalg_3_5_2 Supplementary face construction methods

The two basic  constructions (from a surface and from a surface and parameters) are  implemented for all *gp* package surfaces, which are transformed in the corresponding Surface from Geom. 

| gp package surface | | Geom package surface |
| :------------------- | :----------- | :------------- |
| *gp_Pln*             |    | *Geom_Plane* |
| *gp_Cylinder*        |   | *Geom_CylindricalSurface* |
| *gp_Cone*            |   creates  a | *Geom_ConicalSurface* |
| *gp_Sphere*          |    | *Geom_SphericalSurface* |
| *gp_Torus*           |    | *Geom_ToroidalSurface* |

Once a face has been  created, a wire can be added using the *Add* method. For example, the following  code creates a cylindrical surface and adds a wire. 

~~~~~
gp_Cylinder C = ..; // a cylinder 
TopoDS_Wire W = ...;// a wire 
BRepBuilderAPI_MakeFace MF(C); 
MF.Add(W); 
TopoDS_Face F = MF; 
~~~~~

More than one wire can  be added to a face, provided that they do not cross each other and they define  only one area on the surface. (Note that this is not checked). The edges on a Face must have a parametric curve description. 

If there is no  parametric curve for an edge of the wire on the Face it is computed by  projection. 

For one wire, a simple  syntax is provided to construct the face from the surface and the wire. The  above lines could be written: 

~~~~~
TopoDS_Face F = BRepBuilderAPI_MakeFace(C,W); 
~~~~~

A planar face can be  created from only a wire, provided this wire defines a plane. For example, to  create a planar face from a set of points you can use *BRepBuilderAPI_MakePolygon* and *BRepBuilderAPI_MakeFace*.

~~~~~
#include <TopoDS_Face.hxx> 
#include <TColgp_Array1OfPnt.hxx> 
#include <BRepBuilderAPI_MakePolygon.hxx> 
#include <BRepBuilderAPI_MakeFace.hxx> 

TopoDS_Face PolygonalFace(const TColgp_Array1OfPnt&  thePnts) 
{ 
BRepBuilderAPI_MakePolygon MP; 
for(Standard_Integer i=thePnts.Lower(); 
i<=thePnts.Upper(); i++) 
{ 
MP.Add(thePnts(i)); 
} 
MP.Close(); 
TopoDS_Face F = BRepBuilderAPI_MakeFace(MP.Wire()); 
return F; 
} 
~~~~~

The last use of *MakeFace* is to copy an existing face to  add new wires. For example, the following code adds a new wire to a face: 

~~~~~
TopoDS_Face F = ...; // a face 
TopoDS_Wire W = ...; // a wire 
F = BRepBuilderAPI_MakeFace(F,W); 
~~~~~

To add more than one  wire an instance of the *BRepBuilderAPI_MakeFace* class can be created with the face and the first wire and the new wires inserted with the *Add* method. 

@subsection occt_modalg_3_5_3 Error status

The *Error* method returns  an error status, which is a term from the *BRepBuilderAPI_FaceError* enumeration. 

* *FaceDone* -- no  error occurred. 
* *NoFace* -- no initialization of the algorithm; an empty constructor was used. 
* *NotPlanar* -- no  surface was given and the wire was not planar. 
* *CurveProjectionFailed* -- no curve was found  in the parametric space of the surface for an edge. 
* *ParametersOutOfRange* -- the parameters  *umin, umax, vmin, vmax* are out of the surface. 

@section occt_modalg_3_6 Wire
The wire is a composite shape built not from a geometry, but by the assembly of edges. *BRepBuilderAPI_MakeWire* class can build a wire from one or more edges or connect new edges to an  existing wire. 

Up to four edges can be used directly, for example: 

~~~~~
TopoDS_Wire W = BRepBuilderAPI_MakeWire(E1,E2,E3,E4); 
~~~~~

For a higher or unknown  number of edges the Add method must be used; for example, to build a wire from  an array of shapes (to be edges). 

~~~~~
TopTools_Array1OfShapes theEdges; 
BRepBuilderAPI_MakeWire MW; 
for (Standard_Integer i = theEdge.Lower(); 
i <= theEdges.Upper(); i++) 
MW.Add(TopoDS::Edge(theEdges(i)); 
TopoDS_Wire W = MW; 
~~~~~

The class can be  constructed with a wire. A wire can also be added. In this case, all the edges  of the wires are added. For example to merge two wires: 

~~~~~
#include <TopoDS_Wire.hxx> 
#include <BRepBuilderAPI_MakeWire.hxx> 

TopoDS_Wire MergeWires (const TopoDS_Wire& W1, 
const  TopoDS_Wire& W2) 
{ 
BRepBuilderAPI_MakeWire MW(W1); 
MW.Add(W2); 
return MW; 
} 
~~~~~

*BRepBuilderAPI_MakeWire* class connects the edges to the wire. When a new edge  is added if one of its vertices is shared with the wire it is considered as  connected to the wire. If there is no shared vertex, the algorithm searches for  a vertex of the edge and a vertex of the wire, which are at the same location (the  tolerances of the vertices are used to test if they have the same location). If  such a pair of vertices is found, the edge is copied with the vertex of the  wire in place of the original vertex. All the vertices of the edge can be  exchanged for vertices from the wire. If no connection is found the wire is  considered to be disconnected. This is an error. 

BRepBuilderAPI_MakeWire class can return the last edge added to the wire (Edge  method). This edge can be different from the original edge if it was copied. 

The Error method returns  a term of the *BRepBuilderAPI_WireError* enumeration: 
*WireDone* -- no  error occurred. 
*EmptyWire* -- no  initialization of the algorithm, an empty constructor was used. 
*DisconnectedWire* -- the last added edge was not connected to the wire. 
*NonManifoldWire* -- the  wire with some singularity. 

@section occt_modalg_3_7 Shell
The shell is a composite shape built not from a geometry, but by the assembly of faces.
Use *BRepBuilderAPI_MakeShell* class  to build a Shell from a set of Faces. What may be important is that each face  should have the required continuity. That is why an initial surface is broken  up into faces. 

@section occt_modalg_3_8 Solid
The solid is a composite shape built not from a geometry, but by the assembly of shells. Use  *BRepBuilderAPI_MakeSolid* class  to build a Solid from a set of Shells. Its use is similar to the use of the  MakeWire class: shells are added to the solid in the same way that edges are  added to the wire in MakeWire. 

@section occt_modalg_3b Object Modification

@subsection occt_modalg_3b_1 Transformation
*BRepBuilderAPI_Transform* class can be used to apply a transformation to a shape (see class  *gp_Trsf*). The methods have a boolean argument to copy or share the  original shape, as long as the transformation allows (it is only possible for  direct isometric transformations). By default, the original shape is shared. 

The following example  deals with the rotation of shapes. 

~~~~~

TopoDS_Shape myShape1 = ...; 
// The original shape 1 
TopoDS_Shape myShape2 = ...; 
// The original shape2 
gp_Trsf T; 
T.SetRotation(gp_Ax1(gp_Pnt(0.,0.,0.),gp_Vec(0.,0.,1.)), 
2.*PI/5.); 
BRepBuilderAPI_Transformation theTrsf(T); 
theTrsf.Perform(myShape1); 
TopoDS_Shape myNewShape1 = theTrsf.Shape() 
theTrsf.Perform(myShape2,Standard_True); 
// Here duplication is forced 
TopoDS_Shape myNewShape2 = theTrsf.Shape() 
~~~~~

@subsection occt_modalg_3b_2 Duplication

Use the  *BRepBuilderAPI_Copy* class to duplicate a shape. A new shape is thus created. 
In the following example, a  solid is copied: 

~~~~~
TopoDS Solid MySolid; 
....// Creates a solid 

TopoDS_Solid myCopy = BRepBuilderAPI_Copy(mySolid); 
~~~~~



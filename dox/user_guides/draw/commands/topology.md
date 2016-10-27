Topology Commands {#user_guides__draw_commands_topology}
=================

@tableofcontents

Draw provides a set of commands to test @ref occt_modat_5 "OCCT Topology libraries". The Draw commands are found in the 
DRAWEXE executable or in any executable including the BRepTest commands. 

@section topology_basic Basic 
In Draw, shapes are displayed using isoparametric curves. There is color coding for the edges: 

  * a red edge is an isolated edge, which belongs to no faces.
  * a green edge is a free boundary edge, which belongs to one face,
  * a yellow edge is a shared edge, which belongs to at least two faces.

@subsection occt_draw_7_1_1  isos, discretisation
Determines or changes the number of isoparametric curves on shapes. 

> isos [name ...][nbisos] discretisation nbpoints 

The same number is used for the u and v directions. With no arguments, *isos* prints the current default value. To 
determine, the number of isos for a shape, give it name as the first argument. 

*discretisation* changes the default number of points used to display the curves. The default value is 30. 

~~~~~
# Display only the edges (the wireframe) 
isos 0 
~~~~~

@note Don't confuse *isos* and *discretisation* with the geometric commands *nbisos* and *discr*. 

@subsection occt_draw_7_1_2  orientation, complement, invert, normals, range

* **orientation** -- assigns the orientation of simple and complex shapes to one of the following four values: 
*FORWARD, REVERSED, INTERNAL, EXTERNAL*. 
> orientation name [name ...] F/R/E/I 

* **complement** -- changes the current orientation of shapes to its complement: *FORWARD* to *REVERSED* and 
*INTERNAL* to *EXTERNAL*. 
> complement name [name ...] 

* **invert** -- creates a copy of the original shape with a reversed orientation of all subshapes. For example, it may
 be useful to reverse the normals of a solid. 
> invert name 

* **normals** -- returns the assignment of colors to orientation values.
> normals s (length = 10), disp normals 

* **range** -- defines the length of a selected edge by defining the values of a starting point and an end point.
> range name value value 
 
~~~~~
# to invert normals of a box 
box b 10 20 30 
normals b 5 
invert b 
normals b 5 

# to assign a value to an edge 
box b1 10 20 30 

# to define the box as edges 
explode b1 e 
b_1 b_2 b_3 b_4 b_5 b_6 b_7 b_8 b_9 b_10 b_11 b_12 

# to define as an edge 
makedge e 1 

# to define the length of the edge as starting from 0 
and finishing at 1 
range e 0 1 
~~~~~

@subsection occt_draw_7_1_3  explode, exwire, nbshapes

**explode** extracts subshapes from an entity. The subshapes will be named *name_1*, *name_2*, ... Note that they are 
not copied but shared with the original. 
> explode name [C/So/Sh/F/W/E/V] 

With name only, **explode** will extract the first sublevel of shapes: the shells of a solid or the edges of a wire, for
 example. With one argument, **explode** will extract all subshapes of that type: *C* for compounds, *So* for solids,
 *Sh* for shells, *F* for faces, *W* for wires, *E* for edges, *V* for vertices. 

**exwire** is a special case of **explode** for wires, which extracts the edges in an ordered way, if possible. Each 
edge, for example, is connected to the following one by a vertex. 
> exwire name 

**nbshapes** counts the number of shapes of each type in an entity.
> nbshapes name  

~~~~~
# on a box 
box b 10 20 30 

# whatis returns the type and various information 
whatis b 
= b is a shape SOLID FORWARD Free Modified 

# make one shell 
explode b 
whatis b_1 
= b_1 is a shape SHELL FORWARD Modified Orientable 
Closed 

# extract the edges b_1, ... , b_12 
explode b e 
==b_1 ... b_12 

# count subshapes 
nbshapes b 
== 
Number of shapes in b 
VERTEX : 8 
EDGE : 12 
WIRE : 6 
FACE : 6 
SHELL : 1 
SOLID : 1 
COMPSOLID : 0 
COMPOUND : 0 
SHAPE : 34 
~~~~~

@subsection occt_draw_7_1_4  emptycopy, add, compound

**emptycopy** returns an empty shape with the same orientation, location, and geometry as the target shape, but with no
 sub-shapes. If the **newname** argument is not given, the new shape is stored with the same name. This command is used 
to modify a frozen shape. A frozen shape is a shape used by another one. To modify it, you must **emptycopy** it. Its 
subshape may be reinserted with the **add** command. 
> emptycopy [newname] name 

**add** inserts shape *C* into shape *S*. Verify that *C* and *S* reference compatible types of objects: 
  * Any *Shape* can be added to a *Compound*.
  * Only a *Solid* can be added to a *CompSolid*.
  * Only a *Shell* can *Edge* or a *Vertex* can be added into a *Solid*.
  * Only a *Face* can be added to a *Shell*.
  * Only a *Wire* and *Vertex* can be added in a *Solid*.
  * Only an *Edge* can be added to a *Wire*.
  * Only a *Vertex* can be added to an *Edge*.
  * Nothing can be added to a *Vertex*.

> add name toname 

**emptycopy** and **add** should be used with care. 

On the other hand, **compound** is a safe way to achieve a similar result. It creates a compound from shapes. If no
 shapes are given, the compound is empty. 
> compound [name ...] compoundname 

~~~~~
# a compound with three boxes 
box b1 0 0 0 1 1 1 
box b2 3 0 0 1 1 1 
box b3 6 0 0 1 1 1 
compound b1 b2 b3 c 
~~~~~

@subsection occt_draw_7_1_5  checkshape

> checkshape [-top] shape [result] [-short] 

Where: 
* *top* -- optional parameter, which allows checking only topological validity of a shape. 
* *shape* -- the only required parameter which represents the name of the shape to check. 
* *result* -- optional parameter which is the prefix of the output shape names. 
* *short* -- a short description of the check. 

**checkshape** examines the selected object for topological and geometric coherence. The object should be a three 
dimensional shape. 

~~~~~
# checkshape returns a comment valid or invalid 
box b1 0 0 0 1 1 1 
checkshape b1 
# returns the comment 
this shape seems to be valid 
~~~~~

@note This test is performed using the tolerance set in the algorithm.

@section topology_curve_surface Curve and Surface Topology
@subsection occt_draw_7_2_1  vertex
Creates a vertex at either a 3d location x,y,z or the point at parameter p on an edge. 

> vertex name [x y z / p edge] 

~~~~~
vertex v1 10 20 30 
~~~~~

@subsection occt_draw_7_2_2  edge, mkedge, uisoedge, visoedge

* **edge** creates a straight line edge between two vertices.
> edge name vertex1 vertex2 

* **mkedge** generates edges from curves<.Two parameters can be given for the vertices: the first and last parameters of
 the curve are given by default. Vertices can also be given with their parameters, this option allows blocking the 
creation of new vertices. If the parameters of the vertices are not given, they are computed by projection on the curve.
 Instead of a 3d curve, a 2d curve and a surface can be given.
> mkedge edge curve [surface] [pfirst plast] [vfirst [pfirst] vlast [plast]] 

* **visoedge** and **uisoedge** are commands that generate a *uiso* parameter edge or a *viso* parameter edge.
> uisoedge edge face u v1 v2 

> visoedge edge face v u1 u2 

~~~~~
# straight line edge 
vertex v1 10 0 0 
vertex v2 10 10 0 
edge e1 v1 v2 

# make a circular edge 
circle c 0 0 0 5 
mkedge e2 c 0 pi/2 

# A similar result may be achieved by trimming the curve 
# The trimming is removed by mkedge 
trim c c 0 pi/2 
mkedge e2 c 
~~~~~

~~~~~
# to create an edge between v1 and v2 at point u 
# to create the example plane 
plane p 
trim p p 0 1 0 1 
convert p p 
incudeg p 3 
incvdeg p 3 
movep p 2 2 0 0 1 
movep p 3 3 0 0 0.5 
mkface p p 
# to create the edge in the plane at the u axis point 
0.5, and between the v axis points v=0.2 and v =0.8 
uisoedge e p 0.5 0.20 0.8 
~~~~~

@subsection occt_draw_7_2_3  wire, polyline, polyvertex

**wire** creates a wire from edges or wires. The order of the elements should ensure that the wire is connected, and
 vertex locations will be compared to detect connection. If the vertices are different, new edges will be created to 
ensure topological connectivity. The original edge may be copied in the new one. 
> wire wirename e1/w1 [e2/w2 ...] 

**polyline** creates a polygonal wire from point coordinates. To make a closed wire, you should give the first point
 again at the end of the argument list. 
> polyline name x1 y1 z1 x2 y2 z2 ... 

**polyvertex** creates a polygonal wire from vertices. 
> polyvertex name v1 v2 ... 

~~~~~
# create two polygonal wires 
# glue them and define as a single wire 
polyline w1 0 0 0 10 0 0 10 10 0 
polyline w2 10 10 0 0 10 0 0 0 0 
wire w w1 w2 
~~~~~

@subsection occt_draw_7_2_4  profile
Builds a profile in a plane using a moving point and direction. By default, the profile is closed and a face 
is created. The original point is 0 0, and direction is 1 0 situated in the XY plane. 
> profile name [code values] [code values] ... 


| **Code**     |    **Values **    |       **Action** |
| :------------ | :------------- | :---------------- |
| O                 |                     X Y Z      |          Sets the origin of the plane |
| P                 |         DX DY DZ UX UY UZ  |  Sets the normal and X of the plane |
| F                 |                      X Y    |               Sets the first point |
| X                 |                      DX      |             Translates a point along X |
| Y                 |                      DY       |            Translates a point along Y |
| L                 |                      DL        |            Translates a point along direction |
| XX                |                    X           |           Sets point X coordinate |
| YY                |                    Y           |           Sets point Y coordinate |
| T                 |                      DX DY     |         Translates a point |
| TT                |                     X Y        |           Sets a point |
| R                 |                      Angle     |           Rotates direction |
| RR                |                    Angle       |         Sets direction |
| D                 |                     DX DY      |        Sets direction |
| IX                |                      X         |             Intersects with vertical |
| IY                |                      Y         |             Intersects with horizontal |
| C                 |                Radius Angle    |      Arc of circle tangent to direction |


Codes and values are used to define the next point or change the direction. When the profile changes from a straight line 
to a curve, a tangent is created. All angles are in degrees and can be negative. 

The point [code values] can be repeated any number of times and in any order to create the profile contour. 

| Suffix | Action |
| :----- | :----- |
| No suffix  |             Makes a closed face |
| W          |               Make a closed wire |
| WW         |            Make an open wire |

The profile shape definition is the suffix; no suffix produces a face, *w* is a closed wire, *ww* is an open wire. 

Code letters are not case-sensitive. 

~~~~~
# to create a trianglular plane using a vertex at the 
origin, in the xy plane 
profile p O 0 0 0 X 1 Y 0 x 1 y 1 
~~~~~

~~~~~
# to create a contour using the different code 
possibilities 

# two vertices in the xy plane 
profile p F 1 0 x 2 y 1 ww 

# to view from a point normal to the plane 
top 

# add a circular element of 45 degrees 
profile p F 1 0 x 2 y 1 c 1 45 ww 

# add a tangential segment with a length value 1 
profile p F 1 0 x 2 y 1 c 1 45 l 1 ww 

# add a vertex with xy values of 1.5 and 1.5 
profile p F 1 0 x 2 y 1 c 1 45 l 1 tt 1.5 1.5 ww 

# add a vertex with the x value 0.2, y value is constant 
profile p F 1 0 x 2 y 1 c 1 45 l 1 tt 1.5 1.5 xx 0.2 ww 

# add a vertex with the y value 2 x value is constant 
profile p F 1 0 x 2 y 1 c 1 45 l 1 tt 1.5 1.5 yy 2 ww 

# add a circular element with a radius value of 1 and a circular value of 290 degrees 
profile p F 1 0 x 2 y 1 c 1 45 l 1 tt 1.5 1.5 xx 0.2 yy 2 c 1 290 

# wire continues at a tangent to the intersubsection x = 0 
profile p F 1 0 x 2 y 1 c 1 45 l 1 tt 1.5 1.5 xx 0.2 yy 2 c 1 290 ix 0 ww 

# continue the wire at an angle of 90 degrees until it intersects the y axis at y= -o.3 
profile p F 1 0 x 2 y 1 c 1 45 l 1 tt 1.5 1.5 xx 0.2 yy 2 c 1 290 ix 0 r 90 ix -0.3 ww 

#close the wire 
profile p F 1 0 x 2 y 1 c 1 45 l 1 tt 1.5 1.5 xx 0.2 yy 2 c 1 290 ix 0 r 90 ix -0.3 w 

# to create the plane with the same contour 
profile p F 1 0 x 2 y 1 c 1 45 l 1 tt 1.5 1.5 xx 0.2 yy 2 c 1 290 ix 0 r 90 ix -0.3 
~~~~~

@subsection occt_draw_7_2_5  bsplineprof
Builds a profile in the XY plane from digitizes. By default the profile is closed and a face is built. 

> bsplineprof name [S face] [W WW] 

* for an edge : \<digitizes\> ... <mouse button 2>
* to end profile : <mouse button 3>

**bsplineprof** creates a 2d profile from bspline curves using the mouse as the input. *MB1* creates the points, *MB2* 
finishes the current curve and starts the next curve, *MB3* closes the profile. 

The profile shape definition is the suffix; no suffix produces a face, **w** is a closed wire, **ww** is an open wire. 

~~~~~
#to view the xy plane 
top 
#to create a 2d curve with the mouse 
bsplineprof res 
# click mb1 to start the curve 
# click mb1 to create the second vertex 
# click mb1 to create a curve 
== 
#click mb2 to finish the curve and start a new curve 
== 
# click mb1 to create the second curve 
# click mb3 to create the face 
~~~~~

@subsection occt_draw_7_2_6  mkoffset
Creates a parallel wire in the same plane using a face or an existing continuous set of wires as a 
reference. The number of occurences is not limited. 

The offset distance defines the spacing and the positioning of the occurences. 
    
> mkoffset result face/compound of wires nboffset stepoffset 

~~~~~
#Create a box and select a face 
box b 1 2 3 
explode b f 

#Create three exterior parallel contours with an offset 
value of 2 
mkoffset r b_1 3 2 
Create one interior parallel contour with an offset 
value of 
0.4 
mkoffset r b_1 1 -0.4 
~~~~~

@note *mkoffset* command must be used with prudence, as angular contours produce offset contours with fillets. Interior 
parallel contours can produce more than one wire, normally these are refused. In the following example, any increase in
 the offset value is refused.

~~~~~
# to create the example contour 
profile p F 0 0 x 2 y 4 tt 1 1 tt 0 4 w 

# to create an incoherent interior offset 
mkoffset r p 1 -0.50 
==p is not a FACE but a WIRE 
BRepFill_TrimEdgeTool: incoherent intersubsection 

# to create two incoherent wires 
mkoffset r p 1 -0.50 
~~~~~

@subsection occt_draw_7_2_7  mkplane, mkface
   
* <i>mkplane</i> generates a face from a planar wire. The planar surface will be constructed with an orientation which keeps
 the face inside the wire. 
> mkplane name wire 

* <i>mkface</i> generates a face from a surface. Parameter values can be given to trim a rectangular area. The default 
boundaries are those of the surface. 
> mkface name surface [ufirst ulast vfirst vlast] 

~~~~~
# make a polygonal face 
polyline f 0 0 0 20 0 0 20 10 0 10 10 0 10 20 0 0 20 0 0 0 0 
mkplane f f 

# make a cylindrical face 
cylinder g 10 
trim g g -pi/3 pi/2 0 15 
mkface g g 
~~~~~

@subsection occt_draw_7_2_8  mkcurve
**mkcurve** creates a 3d curve from an edge. The curve will be trimmed to the edge boundaries.   
   
> mkcurve curve edge 

@subsection occt_draw_7_2_9  pcurve
Extracts the 2d curve of an edge on a face. If only the face is specified, the command extracts all the curves and 
colors them according to their orientation. This is useful in checking to see if the edges in a face are correctly
 oriented, i.e. they turn counter-clockwise. To make curves visible, use a fitted 2d view. 

> pcurve [name edgename] facename 

~~~~~
# view the pcurves of a face 
plane p 
trim p p -1 1 -1 1 
mkface p p 
av2d; # a 2d view 
pcurve p 
2dfit 
~~~~~

@subsection occt_draw_7_2_10  chfi2d
Creates chamfers and fillets on 2D objects. Select two adjacent edges and: 
  * a radius value
  * two respective distance values
  * a distance value and an angle

The radius value produces a fillet between the two faces. 

The distance is the length value from the edge between the two selected faces in a normal direction.
   
> chfi2d result face [edge1 edge2 (F radius/CDD d1 d2/CDA d ang) .... 

~~~~~
# create a 2d fillet: 
top 
profile p x 2 y 2 x -2 
chfi2d cfr p . . F 0.3 
==Pick an object 
##select an edge 
==Pick an object 
##select an edge 
~~~~~
 
~~~~~
# create a 2d chamfer using two distances:
profile p x 2 y 2 x -2 
chfi2d cfr p . . CDD 0.3 0.6 
==Pick an object 
##select an edge 
==Pick an object 
##select an edge 
~~~~~

~~~~~
# create a 2d chamfer using a defined distance and angle 
top 
profile p x 2 y 2 x -2 
chfi2d cfr p . . CDA 0.3 75 
==Pick an object 
##select an edge 
==Pick an object 
##select an edge 
~~~~~

@subsection occt_draw_7_2_11  nproject    
Creates a shape projection which is normal to the target surface. 

> nproject pj e1 e2 e3 ... surf -g -d [dmax] [Tol [continuity [maxdeg [maxseg]]] 

~~~~~
# create a curved surface 
line l 0 0 0 1 0 0 
trim l l 0 2 
convert l l 

incdeg l 3 
cmovep l 1 0 0.5 0 
cmovep l 3 0 0.5 0 
copy l ll 
translate ll 2 -0.5 0 
mkedge e1 l 
mkedge e2 ll 
wire w e1 e2 
prism p w 0 0 3 
donl p 

#display in four views 
mu4 
fit 

# create the example shape 
circle c 1.8 -0.5 1 0 1 0 1 0 0 0.4 
mkedge e c 
donly p e 

# create the normal projection of the shape(circle) 
nproject r e p 
~~~~~

@section surface Surface Creation
## mksurface     
Creates a surface from a face. The surface will not be trimmed. 

> mksurface name face 


## gplate
Creates a surface from a defined boundary. The boundary can be defined using edges, points, or other surfaces. 

> gplate result nbrcurfront nbrpntconst [SurfInit] [edge 0] [edge tang (1:G1;2:G2) surf]...[point] [u v tang (1:G1;2:G2) surf] ... 

~~~~~
plane p 
trim p p -1 3 -1 3 
mkface p p 

beziercurve c1 3 0 0 0 1 0 1 2 0 0 
mkedge e1 c1 
tcopy e1 e2 
tcopy e1 e3 

ttranslate e2 0 2 0 
trotate e3 0 0 0 0 0 1 90 
tcopy e3 e4 
ttranslate e4 2 0 0 
# create the surface 
gplate r1 4 0 p e1 0 e2 0 e3 0 e4 0 
== 
======== Results =========== 
DistMax=8.50014503228635e-16 
* GEOMPLATE END* 
Calculation time: 0.33 
Loop number: 1 
Approximation results 
Approximation error : 2.06274907619957e-13 
Criterium error : 4.97600631215754e-14 

#to create a surface defined by edges and passing through a point 
# to define the border edges and the point 
plane p 
trim p p -1 3 -1 3 
mkface p p 

beziercurve c1 3 0 0 0 1 0 1 2 0 0 
mkedge e1 c1 
tcopy e1 e2 
tcopy e1 e3 

ttranslate e2 0 2 0 
trotate e3 0 0 0 0 0 1 90 
tcopy e3 e4 
ttranslate e4 2 0 0 
# to create a point 
point pp 1 1 0 
# to create the surface 
gplate r2 4 1 p e1 0 e2 0 e3 0 e4 0 pp 
== 
======== Results =========== 
DistMax=3.65622157610934e-06 
* GEOMPLATE END* 
Calculculation time: 0.27 
Loop number: 1 
Approximation results 
Approximation error : 0.000422195884750181 
Criterium error : 3.43709808053967e-05 
~~~~~

## filling, fillingparam
Creates a surface between borders. This command uses the **gplate** algorithm but creates a surface that is tangential 
to the adjacent surfaces. The result is a smooth continuous surface based on the G1 criterion. 

> filling result nbB nbC nbP [SurfInit] [edge][face]order... 
> edge[face]order... point/u v face order... 

To define the surface border: 

  * enter the number of edges, constraints, and points
  * enumerate the edges, constraints and points

The surface can pass through other points. These are defined after the border definition. 

You can use the *fillingparam* command to access the filling parameters. 

The options are: 

 * <i>-l</i> : to list current values 
 * <i>-i</i> : to set default values 
 * <i>-rdeg nbPonC nbIt anis </i> : to set filling options 
 * <i>-c t2d t3d tang tcur </i> : to set tolerances 
 * <i>-a maxdeg maxseg </i> : Approximation option 

~~~~~
# to create four curved survaces and a point 
plane p 
trim p p -1 3 -1 3 
mkface p p 

beziercurve c1 3 0 0 0 1 0 1 2 0 0 
mkedge e1 c1 
tcopy e1 e2 
tcopy e1 e3 

ttranslate e2 0 2 0 
trotate e3 0 0 0 0 0 1 90 
tcopy e3 e4 
ttranslate e4 2 0 0 

point pp 1 1 0 

prism f1 e1 0 -1 0 
prism f2 e2 0 1 0 
prism f3 e3 -1 0 0 
prism f4 e4 1 0 0 

# to create a tangential surface 
filling r1 4 0 0 p e1 f1 1 e2 f2 1 e3 f3 1 e4 f4 1 
# to create a tangential surface passing through point pp 
filling r2 4 0 1 p e1 f1 1 e2 f2 1 e3 f3 1 e4 f4 1 pp# 
# to visualise the surface in detail 
isos r2 40 
# to display the current filling parameters 
fillingparam -l 
== 
Degree = 3 
NbPtsOnCur = 10 
NbIter = 3 
Anisotropie = 0 
Tol2d = 1e-05 
Tol3d = 0.0001 
TolAng = 0.01 
TolCurv = 0.1 

MaxDeg = 8 
MaxSegments = 9 
~~~~~


@section topology_primitives Primitives
@subsection occt_draw_7_3_1  box, wedge

**box** creates a box parallel to the axes with dimensions *dx,dy,dz*. *x,y,z* is the corner of the box. It is the
 default origin.    
> box name [x y z] dx dy dz 

**wedge** creates a box with five faces called a wedge. One face is in the OXZ plane, and has dimensions *dx,dz* while 
the other face is in the plane *y = dy*. This face either has dimensions *ltx, dz* or is bounded by *xmin,zmin,xmax,zmax*. 
> wedge name dx dy dz ltx / xmin zmin xmax xmax 

The other faces are defined between these faces. The face in the *y=yd* plane may be degenerated into a line 
if *ltx = 0*, or a point if *xmin = xmax* and *ymin = ymax*. In these cases, the line and the point both have 5 faces 
each. To position the wedge use the *ttranslate* and *trotate* commands. 

~~~~~
# a box at the origin 
box b1 10 20 30 

# another box 
box b2 30 30 40 10 20 30 

# a wedge 
wedge w1 10 20 30 5 

# a wedge with a sharp edge (5 faces) 
wedge w2 10 20 30 0 

# a pyramid 
wedge w3 20 20 20 10 10 10 10 
~~~~~

@subsection occt_draw_7_3_2  pcylinder, pcone, psphere, ptorus

All these commands create solid blocks in the default coordinate system, using the Z axis as the axis of revolution and 
the X axis as the origin of the angles. To use another system, translate and rotate the resulting solid or use a plane 
as first argument to specify a coordinate system. All primitives have an optional last argument which is an angle 
expressed in degrees and located on the Z axis, starting from the X axis. The default angle is 360. 

**pcylinder** creates a cylindrical block with the given radius and height. 
> pcylinder name [plane] radius height [angle]
 
**pcone** creates a truncated cone of the given height with radius1 in the plane z = 0 and radius2 in the plane 
z = height. Neither radius can be negative, but one of them can be null.
> pcone name [plane] radius1 radius2 height [angle] 

**psphere** creates a solid sphere centered on the origin. If two angles, *angle1* and *angle2*, are given, the solid 
will be limited by two planes at latitude *angle1* and *angle2*. The angles must be increasing and in the range -90,90. 
> psphere name [plane] radius1 [angle1 angle2] [angle] 

**ptorus** creates a solid torus with the given radii, centered on the origin, which is a point along the z axis. If 
two angles increasing in degree in the range 0 -- 360 are given, the solid will be bounded by two planar surfaces at 
those positions on the circle. 
> ptorus name [plane] radius1 radius2 [angle1 angle2] [angle] 

~~~~~
# a can shape 
pcylinder cy 5 10 

# a quarter of a truncated cone 
pcone co 15 10 10 90 

# three-quarters of sphere 
psphere sp 10 270 

# half torus 
ptorus to 20 5 0 90 
~~~~~

@subsection occt_draw_7_3_3  halfspace
Creates an infinite solid volume based on a face in a defined direction. This volume can be used to
 perform the boolean operation of cutting a solid by a face or plane. 
> halfspace result face/shell x y z 

~~~~~
box b 0 0 0 1 2 3 
explode b f 
==b_1 b_2 b_3 b_4 b_5 b_6 
halfspace hr b_3 0.5 0.5 0.5 
~~~~~

@section topology_complex Complex Topology
Complex topology is the group of commands that modify the topology of shapes. This includes feature modeling. 

@subsection occt_draw_7_11_1  offsetshape, offsetcompshape

~~~~~
offsetshape r shape offset [tol] [face ...] 
offsetcompshape r shape offset [face ...] 
~~~~~

**offsetshape** and **offsetcompshape** assign a thickness to the edges of a shape. The *offset* value can be negative or positive. This value defines the thickness and direction of the resulting shape. Each face can be removed to create a hollow object. 

The resulting shape is based on a calculation of intersubsections. In case of simple shapes such as a box, only the adjacent intersubsections are required and you can use the **offsetshape** command. 

In case of complex shapes, where intersubsections can occur from non-adjacent edges and faces, use the **offsetcompshape** command. **comp** indicates complete and requires more time to calculate the result. 

The opening between the object interior and exterior is defined by the argument face or faces. 

~~~~~
box b1 10 20 30 
explode b1 f 
== b1_1 b1_2 b1_3 b1_4 b1_5 b1_6 
offsetcompshape r b1 -1 b1_3 
~~~~~

@subsection occt_draw_7_11_2  featprism, featdprism, featrevol, featlf, featrf
  
~~~~~
featprism shape element skface Dirx Diry Dirz Fuse(0/1/2) Modify(0/1) 
featdprism shape face skface angle Fuse(0/1/2) Modify(0/1) 
featrevol shape element skface Ox Oy Oz Dx Dy Dz Fuse(0/1/2) Modify(0/1) 
featlf shape wire plane DirX DirY DirZ DirX DirY DirZ Fuse(0/1/2) Modify(0/1) 
featrf shape wire plane X Y Z DirX DirY DirZ Size Size Fuse(0/1/2) Modify(0/1) 
featperform prism/revol/pipe/dprism/lf result [[Ffrom] Funtil] 
featperformval prism/revol/dprism/lf result value 
~~~~~

**featprism** loads the arguments for a prism with contiguous sides normal to the face. 

**featdprism** loads the arguments for a prism which is created in a direction normal to the face and includes a draft angle. 

**featrevol** loads the arguments for a prism with a circular evolution. 

**featlf** loads the arguments for a linear rib or slot. This feature uses planar faces and a wire as a guideline. 

**featrf** loads the arguments for a rib or slot with a curved surface. This feature uses a circular face and a wire as a guideline. 

**featperform** loads the arguments to create the feature. 

**featperformval** uses the defined arguments to create a feature with a limiting value. 

All the features are created from a set of arguments which are defined when you initialize the feature context. Negative values can be used to create depressions. 

###Let us create a feature prism with a draft angle and a normal direction :

~~~~~
# create a box with a wire contour on the upper face 
box b 1 1 1 
profil f O 0 0 1 F 0.25 0.25 x 0.5 y 0.5 x -0.5 
explode b f 

# loads the feature arguments defining the draft angle 
featdprism b f b_6 5 1 0 

# create the feature 
featperformval dprism r 1 
==BRepFeat_MakeDPrism::Perform(Height) 
BRepFeat_Form::GlobalPerform () 
 Gluer 
 still Gluer 
 Gluer result 
~~~~~

###Let us  create a feature prism with circular direction :

~~~~~
# create a box with a wire contour on the upper face 
box b 1 1 1 
profil f O 0 0 1 F 0.25 0.25 x 0.5 y 0.5 x -0.5 
explode b f 

# loads the feature arguments defining a rotation axis 
featrevol b f b_6 1 0 1 0 1 0 1 0 
featperformval revol r 45 
==BRepFeat_MakeRevol::Perform(Angle) 
BRepFeat_Form::GlobalPerform () 
 Gluer 
 still Gluer 
 Gluer result 
~~~~~

###Let us create a slot using the linear feature :

~~~~~
#create the base model using the multi viewer 
mu4 
profile p x 5 y 1 x -3 y -0.5 x -1.5 y 0.5 x 0.5 y 4 x -1 y -5 
prism pr p 0 0 1 

# create the contour for the linear feature 
vertex v1 -0.2 4 0.3 
vertex v2 0.2 4 0.3 
vertex v3 0.2 0.2 0.3 
vertex v4 4 0.2 0.3 
vertex v5 4 -0.2 0.3 
edge e1 v1 v2 
edge e2 v2 v3 
edge e3 v3 v4 
edge e4 v4 v5 
wire w e1 e2 e3 e4 

# define a plane 
plane pl 0.2 0.2 0.3 0 0 1 

# loads the linear feature arguments 
featlf pr w pl 0 0 0.3 0 0 0 0 1 
featperform lf result 
~~~~~

###Let us create a rib using the revolution feature :

~~~~~
#create the base model using the multi viewer 
mu4 
pcylinder c1 3 5 
# create the contour for the revolution feature 
profile w c 1 190 WW 
trotate w 0 0 0 1 0 0 90 
ttranslate w -3 0 1 
trotate w -3 0 1.5 0 0 1 180 
plane pl -3 0 1.5 0 1 0 
# loads the revolution feature arguments 
featrf c1 w pl 0 0 0 0 0 1 0.3 0.3 1 1 
featperform rf result 
~~~~~

@subsection occt_draw_7_11_3  draft

~~~~~
draft result shape dirx diry dirz angle shape/surf/length [-IN/-OUT] [Ri/Ro] [-Internal] 
~~~~~

Computes a draft angle surface from a wire. The surface is determined by the draft direction, the inclination of the draft surface, a draft angle, and a limiting distance. 

  * The draft angle is measured in radians.
  * The draft direction is determined by the argument -INTERNAL
  * The argument Ri/Ro deftermines wether the corner edges of the draft surfaces are angular or rounded.
  * Arguments that can be used to define the surface distance are:
   * length, a defined distance
   * shape, until the surface contacts a shape
   * surface, until the surface contacts a surface.

@note The original aim of adding a draft angle to a shape is to produce a shape which can be removed easily from a mould. The Examples below use larger angles than are used normally and the calculation results returned are not indicated.

~~~~~
# to create a simple profile 
profile p F 0 0 x 2 y 4 tt 0 4 w 
# creates a draft with rounded angles 
draft res p 0 0 1 3 1 -Ro 
# to create a profile with an internal angle 
profile p F 0 0 x 2 y 4 tt 1 1.5 tt 0 4 w 
# creates a draft with rounded external angles 
draft res p 0 0 1 3 1 -Ro 
~~~~~

@subsection occt_draw_7_11_4  deform
  
~~~~~
deform newname name CoeffX CoeffY CoeffZ
~~~~~

Modifies the shape using the x, y, and z coefficients. You can reduce or magnify the shape in the x,y, and z directions. 
 
~~~~~
pcylinder c 20 20 
deform a c 1 3 5 
# the conversion to bspline is followed by the 
deformation 
~~~~~


@subsection occt_draw_7_11_5 nurbsconvert

~~~~~
nurbsconvert result name [result name] 
~~~~~

Changes the NURBS curve definition of a shape to a Bspline curve definition. This conversion is required for assymetric 
deformation and prepares the arguments for other commands such as **deform**. The conversion can be necessary when 
transferring shape data to other applications. 

@section topolog_trsf Transformation
Transformations are applications of matrices. When the transformation is nondeforming, such as translation or rotation,
 the object is not copied. The topology localcoordinate system feature is used. 

@subsection occt_draw_7_5_1 tcopy
Copies the structure of one shape, including the geometry, into another, newer shape. 

> tcopy name toname [name toname ...] 

~~~~~
# create an edge from a curve and copy it 
beziercurve c 3 0 0 0 10 0 0 20 10 0 
mkedge e1 c 
ttranslate e1 0 5 0 
tcopy e1 e2 
ttranslate e2 0 5 0 
# now modify the curve, only e1 and e2 will be modified 
~~~~~

@subsection occt_draw_7_5_2 tmove, treset
 
**tmove** and **reset** modify the location, or the local coordinate system of a shape. 

> tmove name [name ...] shape 

**tmove** applies the location of a given shape to other shapes. **reset** restores one or several shapes it to its or 
their original coordinate system(s). 

> reset name [name ...] 

~~~~~
# create two boxes 
box b1 10 10 10 
box b2 20 0 0 10 10 10 

# translate the first box 
ttranslate b1 0 10 0 

# and apply the same location to b2 
tmove b2 b1
 
# return to original positions 
reset b1 b2 
~~~~~

@subsection occt_draw_7_5_3 ttranslate, trotate
**ttranslate** translates a set of shapes by a given vector, and **trotate** rotates them by a given angle around an 
axis. Both commands only modify the location of the shape. 
When creating multiple shapes, the same location is used for all the shapes. (See *toto.tcl* example below. Note that
 the code of this file can also be directly executed in interactive mode.)

> ttranslate [name ...] dx dy dz 

> trotate [name ...] x y z dx dy dz angle 

Locations are very economic in the data structure because multiple occurences of an object share the topological description. 

~~~~~
# make rotated copies of a sphere in between two cylinders 
# create a file source toto.tcl 
# toto.tcl code: 
for {set i 0} {$i  360} {incr i 20} { 
copy s s$i 
trotate s$i 0 0 0 0 0 1 $i 
} 

# create two cylinders 
pcylinder c1 30 5 
copy c1 c2 
ttranslate c2 0 0 20 

#create a sphere 
psphere s 3 
ttranslate s 25 0 12.5 

# call the source file for multiple copies 
source toto.tcl 
~~~~~

@subsection occt_draw_7_5_4 tmirror, tscale

* **tmirror** makes a mirror copy of a shape about a plane x,y,z dx,dy,dz. 

> tmirror name x y z dx dy dz 

* **Tscale** applies a central homotopic mapping to a shape. 

> tscale name x y z scale 

~~~~~
# mirror a portion of cylinder about the YZ plane 
pcylinder c1 10 10 270 
copy c1 c2 
tmirror c2 15 0 0 1 0 0 
# and scale it 
tscale c1 0 0 0 0.5 
~~~~~


@section topology_sweeping Sweeping
@subsection occt_draw_7_4_1  prism
Creates a new shape by sweeping a shape in a direction. Any shape can be swept: a vertex gives an edge; an edge gives a face; and a face gives a solid. 

> prism result base dx dy dz [Copy | Inf | SemiInf] 

The shape is swept along the vector *dx dy dz*. The original shape will be shared in the result unless *Copy* is specified. If *Inf* is specified the prism is infinite in both directions. If *SemiInf* is specified the prism is infinite in the *dx,dy,dz* direction, and the length of the vector has no importance. 

~~~~~
# sweep a planar face to make a solid 
polyline f 0 0 0 10 0 0 10 5 0 5 5 0 5 15 0 0 15 0 0 0 0 
mkplane f f 
~~~~~

@subsection occt_draw_7_4_2  revol
Creates a new shape by sweeping a base shape through an angle along the axis *x,y,z dx,dy,dz*. As with the prism command, the shape can be of any type and is not shared if *Copy* is specified. 

> revol result base x y z dx dy dz angle [Copy] 

~~~~~
# shell by wire rotation 
polyline w 0 0 0 10 0 0 10 5 0 5 5 0 5 15 0 0 15 0 
revol s w 20 0 0 0 1 0 90 
~~~~~


@subsection occt_draw_7_4_3  pipe
Creates a new shape by sweeping a shape known as the profile along a wire known as the spine. 

> pipe name wire_spine Profile 

~~~~~
# sweep a circle along a bezier curve to make a solid 
pipe 

beziercurve spine 4 0 0 0 10 0 0 10 10 0 20 10 0 
mkedge spine spine 
wire spine spine 
circle profile 0 0 0 1 0 0 2 
mkedge profile profile 
wire profile profile 
mkplane profile profile 
pipe p spine profile 
~~~~~

@subsection occt_draw_7_4_4  mksweep, addsweep, setsweep, deletesweep, buildsweep, simulsweep

These commands are used to create a shape from wires. One wire is designated as the contour that defines the direction;
 it is called the spine. At least one other wire is used to define the the sweep profile. 
* **mksweep** -- initializes the sweep creation and defines the wire to be used as the spine.     
> mksweep wire 

* **addsweep** -- defines the wire to be used as the profile. 
> addsweep wire[vertex][-M][-C] [auxiilaryshape]

* **deletesweep** -- cancels the choice of profile wire, without leaving the mksweep mode. You can re-select a profile wire. 
> deletesweep wire 

* **setsweep** -- commands the algorithms used for the construction of the sweep. 
> setsweep options [arg1 [arg2 [...]]] 

* **simulsweep** -- can be used to create a preview of the shape. [n] is the number of subsections that are used to simulate the sweep. 
> simulsweep r [n] [option] 

* **buildsweep** -- creates the sweep using the arguments defined by all the commands.
> buildsweep [r] [option] [Tol] 

options are : 
 * *-FR* : Tangent and Normal are defined by a Frenet trihedron 
 * *-CF* : Tangent is given by Frenet, the Normal is computed to minimize the torsion 
 * *-DX Surf* : Tangent and Normal are given by Darboux trihedron, surf must be a shell or a face 
 * *-CN dx dy dz* : BiNormal is given by *dx dy dz* 
 * *-FX Tx Ty TZ [Nx Ny Nz]* : Tangent and Normal are fixed 
 * *-G guide* 
  
~~~~~
#create a sweep based on a semi-circular wire using the 
Frenet algorithm 
#create a circular figure 
circle c2 0 0 0 1 0 0 10 
trim c2 c2 -pi/2 pi/2 
mkedge e2 c2 
donly e2 
wire w e2 
whatis w 
mksweep w 
# to display all the options for a sweep 
setsweep 
#to create a sweep using the Frenet algorithm where the 
#normal is computed to minimise the torsion 
setsweep -CF 
addsweep w -R 
# to simulate the sweep with a visual approximation 
simulsweep w 3 
~~~~~

@subsection occt_draw_7_4_5  thrusubsections
Creates a shape using wires that are positioned in different planes. Each wire selected must have the same number of edges and vertices. 
A bezier curve is generated between the vertices of each wire. The option *[-N]* means that no check is made on wires for direction.

> thrusubsections [-N] result issolid isruled wire1 wire2 [..wire..] 

~~~~~
#create three wires in three planes 
polyline w1 0 0 0 5 0 0 5 5 0 2 3 0 
polyline w2 0 1 3 4 1 3 4 4 3 1 3 3 
polyline w3 0 0 5 5 0 5 5 5 5 2 3 5 
# create the shape 
thrusubsections th issolid isruled w1 w2 w3 
==thrusubsections th issolid isruled w1 w2 w3 
Tolerances obtenues   -- 3d : 0 
-- 2d : 0 
~~~~~


@section topology_drafting_blending Drafting and Blending
Drafting is creation of a new shape by tilting faces through an angle. 

Blending is the creation of a new shape by rounding edges to create a fillet. 

  * Use the **depouille** command for drafting.
  * Use the **chamf** command to add a chamfer to an edge
  * Use the **blend** command for simple blending.
  * Use **fubl** for a fusion + blending operation.
  * Use **buildevol**, **mkevol**, **updatevol** to realize varying radius blending.


@subsection occt_draw_7_8_1  depouille

~~~~~
dep result shape dirx diry dirz face angle x y x dx dy dz [face angle...] 
~~~~~

Creates a new shape by drafting one or more faces of a shape. 

Identify the shape(s) to be drafted, the drafting direction, and the face(s) with an angle and an axis of rotation for each face. You can use dot syntax to identify the faces. 

~~~~~
# draft a face of a box 
box b 10 10 10 
explode b f 
== b_1 b_2 b_3 b_4 b_5 b_6 

dep a b 0 0 1 b_2 10 0 10 0 1 0 5 
~~~~~

@subsection occt_draw_7_8_2 chamf
    
~~~~~
chamf newname shape edge face S dist 
chamf newname shape edge face dist1 dist2 
chamf newname shape edge face A dist angle 
~~~~~

Creates a chamfer along the edge between faces using: 

  * a equal distances from the edge
  * the edge, a face and distance, a second distance
  * the edge, a reference face and an angle

Use the dot syntax to select the faces and edges. 

~~~~~
# create a chamfer based on equal distances from the edge (45 degree angle)
## create a box 
box b 1 2 3 
chamf ch b . . S 0.5 
==Pick an object 
## select an edge 
==Pick an object 
## select an adjacent face 
~~~~~

~~~~~
# create a chamfer based on different distances from the selected edge
box b 1 2 3 
chamf ch b . . 0.3 0.4 
==Pick an object 
## select an edge 
==Pick an object 
## select an adjacent face
~~~~~
 
~~~~~
# create a chamfer based on a distance from the edge and an angle
box b 1 2 3 
chamf ch b . . A 0.4 30 
==Pick an object 
# select an edge 
==Pick an object 
# select an adjacent face 
~~~~~

@subsection occt_draw_7_8_3  blend
      
~~~~~
blend result object rad1 ed1 rad2 ed2 ... [R/Q/P] 
~~~~~

Creates a new shape by filleting the edges of an existing shape. The edge must be inside the shape. You may use the dot syntax. Note that the blend is propagated to the edges of tangential planar, cylindrical or conical faces. 

~~~~~
# blend a box, click on an edge 
box b 20 20 20 
blend b b 2 . 
==tolerance ang : 0.01 
==tolerance 3d : 0.0001 
==tolerance 2d : 1e-05 
==fleche : 0.001 
==tolblend 0.01 0.0001 1e-05 0.001 
==Pick an object 
# click on the edge you want ot fillet 

==COMPUTE: temps total 0.1s dont : 
==- Init + ExtentAnalyse 0s 
==- PerformSetOfSurf 0.02s 
==- PerformFilletOnVertex 0.02s 
==- FilDS 0s 
==- Reconstruction 0.06s 
==- SetRegul 0s 
~~~~~

@subsection occt_draw_7_8_4  fubl
     
~~~~~
fubl name shape1 shape2 radius
~~~~~ 
 
Creates a boolean fusion of two shapes and then blends (fillets) the intersubsection edges using the given radius. 

~~~~~
# fuse-blend two boxes 
box b1 20 20 5 
copy b1 b2 
ttranslate b2 -10 10 3 
fubl a b1 b2 1 
~~~~~


@subsection occt_draw_7_8_5  mkevol, updatevol, buildevol
  
~~~~~
mkevol result object (then use updatevol) [R/Q/P] 
updatevol edge u1 radius1 [u2 radius2 ...] 
buildevol 
~~~~~

These three commands work together to create fillets with evolving radii. 

* **mkevol** allows specifying the shape and the name of the result. It returns the tolerances of the fillet. 
* **updatevol** allows describing the filleted edges you want to create. For each edge, you give a set of coordinates: parameter and radius and the command prompts you to pick the edge of the shape which you want to modify. The parameters will be calculated along the edges and the radius function applied to the whole edge. 
* **buildevol** produces the result described previously in **mkevol** and **updatevol**. 

~~~~~
# makes an evolved radius on a box 
box b 10 10 10 
mkevol b b 
==tolerance ang : 0.01 
==tolerance 3d : 0.0001 
==tolerance 2d : 1e-05 
==fleche : 0.001 
==tolblend 0.01 0.0001 1e-05 0.001 

# click an edge 
updatevol . 0 1 1 3 2 2 
==Pick an object 

buildevol 
==Dump of SweepApproximation 
==Error 3d = 1.28548881203818e-14 
==Error 2d = 1.3468326936926e-14 , 
==1.20292299999388e-14 
==2 Segment(s) of degree 3 

==COMPUTE: temps total 0.91s dont : 
==- Init + ExtentAnalyse 0s 
==- PerformSetOfSurf 0.33s 
==- PerformFilletOnVertex 0.53s 
==- FilDS 0.01s 
==- Reconstruction 0.04s 
==- SetRegul 0s 
~~~~~

@section topology_analisys Topology and Geometry Analysis
Analysis of shapes includes commands to compute length, area, volumes and inertial properties. 

  * Use **lprops**, **sprops**, **vprops** to compute integral properties.
  * Use **bounding** to display the bounding box of a shape.
  * Use **distmini** to calculate the minimum distance between two shapes.
  * Use **xdistef**, **xdistcs**, **xdistcc**, **xdistc2dc2dss**, **xdistcc2ds** to check the distance between two objects on even grid.


@subsection occt_draw_7_9_1  lprops, sprops, vprops
     
~~~~~
lprops shape 
sprops shape 
vprops shape 
~~~~~

* **lprops** computes the mass properties of all edges in the shape with a linear density of 1;
* **sprops** of all faces with a surface density of 1;
* **vprops** of all solids with a density of 1. 

All three commands print the mass, the coordinates of the center of gravity, the matrix of inertia and the moments. Mass is either the length, the area or the volume. The center and the main axis of inertia are displayed. 
 
~~~~~
# volume of a cylinder 
pcylinder c 10 20 
vprops c 
== results 
Mass : 6283.18529981086 

Center of gravity : 
X = 4.1004749224903e-06 
Y = -2.03392858349861e-16 
Z = 9.9999999941362 

Matrix of Inertia : 
366519.141445068                    5.71451850691484e-12 
0.257640437382627 
5.71451850691484e-12                366519.141444962 
2.26823064169991e-10                0.257640437382627 
2.26823064169991e-10                314159.265358863 

Moments : 
IX = 366519.141446336 
IY = 366519.141444962 
I.Z = 314159.265357595 
~~~~~

@subsection occt_draw_7_9_2 bounding
  
~~~~~
bounding shape 
~~~~~

Displays the bounding box of a shape. The bounding box is a cuboid created with faces parallel to the x, y, and z planes. The command returns the dimension values of the the box, *xmin ymin zmin xmax ymax zmax.* 

~~~~~
# bounding box of a torus 
ptorus t 20 5 
bounding t 
==-27.059805107309852              -27.059805107309852 - 
5.0000001000000003 
==27.059805107309852               27.059805107309852 
5.0000001000000003 
~~~~~

@subsection occt_draw_7_9_3  distmini

~~~~~
distmini name Shape1 Shape2 
~~~~~

Calculates the minimum distance between two shapes. The calculation returns the number of solutions, If more than one solution exists. The options are displayed in the viewer(red) and the results are listed in the shell window. The *distmini* lines are considered as shapes which have a value v. 

~~~~~
box b 0 0 0 10 20 30 
box b2 30 30 0 10 20 30 
distmini d1 b b2 
==the distance value is : 22.3606797749979 
==the number of solutions is :2 

==solution number 1 
==the type of the solution on the first shape is 0 
==the type of the solution on the second shape is 0 
==the coordinates of the point on the first shape are: 
==X=10 Y=20 Z=30 
==the coordinates of the point on the second shape 
are: 
==X=30 Y=30 Z=30 

==solution number 2: 
==the type of the solution on the first shape is 0 
==the type of the solution on the second shape is 0 
==the coordinates of the point on the first shape are: 
==X=10 Y=20 Z=0 
==the coordinates of the point on the second shape 
are: 
==X=30 Y=30 Z=0 

==d1_val d1 d12 
~~~~~

@subsection occt_draw_7_9_4 xdistef, xdistcs, xdistcc, xdistc2dc2dss, xdistcc2ds 

~~~~~
xdistef edge face
xdistcs curve surface firstParam lastParam [NumberOfSamplePoints]
xdistcc curve1 curve2 startParam finishParam [NumberOfSamplePoints]
xdistcc2ds c curve2d surf startParam finishParam [NumberOfSamplePoints]
xdistc2dc2dss curve2d_1 curve2d_2 surface_1 surface_2 startParam finishParam [NumberOfSamplePoints]
~~~~~

It is assumed that curves have the same parametrization range and *startParam* is less than *finishParam*.

Commands with prefix *xdist* allow checking the distance between two objects on even grid:
  * **xdistef** -- distance between edge and face;
  * **xdistcs** -- distance between curve and surface. This means that the projection of each sample point to the surface is computed;
  * **xdistcc** -- distance between two 3D curves;
  * **xdistcc2ds** -- distance between 3d curve and 2d curve on surface;
  * **xdistc2dc2dss** -- distance between two 2d curves on surface.
  
~~~~~
bopcurves b1 b2 -2d 
mksurf s1 b1
mksurf s2 b2
xdistcs c_1 s1 0 1 100
xdistcc2ds c_1 c2d2_1 s2 0 1
xdistc2dc2dss c2d1_1 c2d2_1 s1 s2 0 1 1000
~~~~~

@section old_bop Old Boolean Operations

## fuse, cut, common
     
~~~~~
fuse name shape1 shape2 
cut name shape1 shape2 
common name shape1 shape2 
~~~~~

**fuse** creates a new shape by a boolean operation on two existing shapes. The new shape contains both originals intact. 

**cut** creates a new shape which contains all parts of the second shape but only the first shape without the 
intersection of the two shapes. 

**common** creates a new shape which contains only what is in common between the two original shapes in their intersection. 

~~~~~
# all four boolean operations on a box and a cylinder 

box b 0 -10 5 20 20 10 
pcylinder c 5 20 

fuse s1 b c 
ttranslate s1 40 0 0 

cut s2 b c 
ttranslate s2 -40 0 0 

cut s3 c b 
ttranslate s3 0 40 0 

common s4 b c 
ttranslate s4 0 -40 0 
~~~~~


## section, psection
   
~~~~~
section result shape1 shape2 
psection name shape plane 
~~~~~

**section** creates a compound object consisting of the edges for the intersection curves on the faces of two shapes. 

**psection** creates a planar section consisting of the edges for the intersection curves on the faces of a shape and a plane. 

~~~~~
# section line between a cylinder and a box 
pcylinder c 10 20 
box b 0 0 5 15 15 15 
trotate b 0 0 0 1 1 1 20 
section s b c 

# planar section of a cone 
pcone c 10 30 30 
plane p 0 0 15 1 1 2 
psection s c p 
~~~~~

## sewing
 
~~~~~
sewing result [tolerance] shape1 shape2 ... 
~~~~~

**Sewing** joins shapes by connecting their adjacent or near adjacent edges. Adjacency can be redefined by modifying the tolerance value. 

~~~~~
# create two adjacent boxes 
box b 0 0 0 1 2 3 
box b2 0 2 0 1 2 3 
sewing sr b b2 
whatis sr 
sr is a shape COMPOUND FORWARD Free Modified 
~~~~~

@section new_bop New Boolean Operations
The new algorithm of Boolean operations avoids a large number of weak points and limitations presented in the old 
boolean operation algorithm. 

@subsection occt_draw_7_7_1  bparallelmode

* **bparallelmode** enable or disable parallel mode for boolean operations. Sequential computing is used by default.
 
> bparallelmode [1/0]

Without arguments, bparallelmode shows current state of parallel mode for boolean operations.

* *0* Disable parallel mode, 
* *1* Enable parallel mode 

~~~~~
# Enable parallel mode for boolean operations.
bparallelmode 1

# Show state of parallel mode for boolean operations.
bparallelmode
~~~~~

@subsection occt_draw_7_7_2  bop, bopfuse, bopcut, boptuc, bopcommon

* **bop** defines *shape1* and *shape2* subject to ulterior Boolean operations 
* **bopfuse** creates a new shape by a boolean operation on two existing shapes. The new shape contains both originals intact. 
* **bopcut** creates a new shape which contains all parts of the second shape but only the first shape without the intersection of the two shapes. 
* **boptuc** is a reverced **bopcut**. 
* **bopcommon** creates a new shape which contains only whatever is in common between the two original shapes in their intersection. 

~~~~~
bop shape1 shape2 
bopcommon result 
bopfuse result 
bopcut result 
boptuc result 
~~~~~

These commands have short variants: 

~~~~~
bcommon result shape1 shape2 
bfuse result shape1 shape2 
bcut result shape1 shape2 
~~~~~

**bop** fills data structure (DS) of boolean operation for *shape1* and *shape2*. 
**bopcommon, bopfuse, bopcut, boptuc** commands are used after **bop** command. After one **bop** command it is possible to call several commands from the list above. For example:

~~~~~
bop S1 S2
bopfuse R
~~~~~ 

~~~~~
# produce all four boolean operations on a box and a cylinder
box b 0 -10 5 20 20 10 
pcylinder c 5 20 

## fills data structure 
bop b c 

bopfuse s1 
ttranslate s1 40 0 0 

bopcut s2 
ttranslate s2 -40 0 0 

boptuc s3 
ttranslate s3 0 40 0 

bopcommon s4 
ttranslate s4 0 -40 0 
~~~~~

Now use short variants of the commands: 

~~~~~
bfuse s11 b c 
ttranslate s11 40 0 100 

bcut s12 b c 
ttranslate s12 -40 0 100 

bcommon s14 b c 
ttranslate s14 0 -40 100 
~~~~~

@subsection occt_draw_7_7_3  bopsection
    
~~~~~
bop shape1 shape2 
bopsection result 
~~~~~

* **bopsection** -- creates a compound object consisting of the edges for the intersection curves on the faces of two shapes.
* **bop** -- fills data structure (DS) of boolean operation for *shape1* and *shape2*. 
* **bopsection** -- is used after **bop** command.
 
Short variant syntax:      
~~~~~
bsection result shape1 shape2 [-2d/-2d1/-2s2] [-a] 
~~~~~

* <i>-2d</i>  --  PCurves are computed on both parts. 
* <i>-2d1</i> -- PCurves are computed on first part. 
* <i>-2d2</i> -- PCurves are computed on second part. 
* <i>-a</i>  --   built geometries  are approximated. 

Let us build a section line between a cylinder and a box 
~~~~~
pcylinder c 10 20 
box b 0 0 5 15 15 15 
trotate b 0 0 0 1 1 1 20 
bop b c 
bopsection s 
# Short variant: 
bsection s2 b c 
~~~~~

@subsection occt_draw_7_7_4 bopcheck, bopargshape
  
~~~~~
bopcheck shape 
bopargcheck shape1 [[shape2] [-F/O/C/T/S/U] [/R|F|T|V|E|I|P]] [#BF] 
~~~~~

**bopcheck** checks a shape for self-interference. 

**bopargcheck** checks the validity of argument(s) for boolean operations. 

* Boolean Operation -- (by default a section is made) : 
  * **F** (fuse) 
  * **O** (common) 
  * **C** (cut) 
  * **T** (cut21) 
  * **S** (section) 
  * **U** (unknown) 
* Test Options -- (by default all options are enabled) : 
  * **R** (disable small edges (shrink range) test) 
  * **F** (disable faces verification test) 
  * **T** (disable tangent faces searching test) 
  * **V** (disable test possibility to merge vertices) 
  * **E** (disable test possibility to merge edges) 
  * **I** (disable self-interference test) 
  * **P** (disable shape type test) 
* Additional Test Options :
  * **B** (stop test on first faulty found) -- by default it is off; 
  * **F** (full output for faulty shapes) -- by default the output is made in a short format.

@note Boolean Operation and Test Options are used only for a couple of argument shapes, except for <b>I</b> and <b>P</b> 
options that are always used to test a couple of shapes as well as a single shape.

~~~~~
# checks a shape on self-interference 
box b1 0 0 0 1 1 1 
bopcheck b1 

# checks the validity of argument for boolean cut operations 
box b2 0 0 0 10 10 10 
bopargcheck b1 b2 -C 
~~~~~

@section topology_texture Texture Mapping to Shape
Texture mapping allows you to map textures on a shape. Textures are texture image files and several are predefined. You 
can control the number of occurrences of the texture on a face, the position of a texture and the scale factor of the 
texture. 

@subsection occt_draw_7_12_1  vtexture

> vtexture NameOfShape TextureFile 

> vtexture NameOfShape 

> vtexture NameOfShape ? 

> vtexture NameOfShape IdOfTexture 

**TextureFile** identifies the file containing the texture you want. The same syntax without **TextureFile** disables texture mapping. The question-mark <b>?</b> lists available textures. **IdOfTexture** allows applying predefined textures. 

@subsection occt_draw_7_12_2  vtexscale
  
~~~~~
vtexscale NameOfShape ScaleU ScaleV 
vtexscale NameOfShape ScaleUV 
vtexscale NameOfShape 
~~~~~

*ScaleU* and *Scale V* allow scaling the texture according to the U and V parameters individually, while *ScaleUV* applies the same scale to both parameters. 

The syntax without *ScaleU*, *ScaleV* or *ScaleUV* disables texture scaling. 

@subsubsection occt_draw_7_12_3 vtexorigin

> vtexorigin NameOfShape UOrigin VOrigin 

> vtexorigin NameOfShape UVOrigin 

> vtexorigin NameOfShape 

*UOrigin* and *VOrigin* allow placing the texture according to the U and V parameters individually, while *UVOrigin* applies the same position value to both parameters. 

The syntax without *UOrigin*, *VOrigin* or *UVOrigin* disables origin positioning. 

@subsection occt_draw_7_12_4 vtexrepeat

> vtexrepeat NameOfShape URepeat VRepeat 

> vtexrepeat NameOfShape UVRepeat 

> vtexrepeat NameOfShape 

*URepeat* and *VRepeat* allow repeating the texture along the U and V parameters individually, while *UVRepeat* applies the same number of repetitions for both parameters. 

The same syntax without *URepeat*, *VRepeat* or *UVRepeat* disables texture repetition. 

@subsection occt_draw_7_12_5  vtexdefault
      
> vtexdefault NameOfShape 

*Vtexdefault* sets or resets the texture mapping default parameters. 

The defaults are: 

 * *URepeat = VRepeat = 1* no repetition 
 * *UOrigin = VOrigin = 1*  origin set at (0,0) 
 * *UScale = VScale = 1*  texture covers 100% of the face 
 

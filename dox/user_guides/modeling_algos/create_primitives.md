Create Primitives {#user_guides__modeling_algos_create_primitives}
=================

@tableofcontents

The <i> BRepPrimAPI</i> package provides an API (Application Programming Interface) for construction of primitives such as:
    * Boxes;
    * Cones;
    * Cylinders;
    * Prisms.

It is possible to create partial solids, such as a sphere limited by longitude. In real models, primitives can be used  for easy creation of specific sub-parts.

  * Construction by sweeping along a profile:
    * Linear;
    * Rotational (through an angle of rotation).

Sweeps are objects obtained by sweeping a profile along a path. The profile can be any topology and the path is usually a curve or a wire. The profile generates objects according to the following rules:
  * Vertices generate Edges
  * Edges generate Faces.
  * Wires generate Shells.
  * Faces generate Solids.
  * Shells generate Composite Solids.

It is not allowed to sweep Solids and Composite Solids. Swept constructions along complex profiles such as BSpline curves also available in the <i> BRepOffsetAPI </i> package. This API provides simple, high level calls for the most common operations.

@section occt_modalg_4_1 Making  Primitives
@subsection occt_modalg_4_1_1 Box

The class *BRepPrimAPI_MakeBox* allows building a parallelepiped box. The result is either a **Shell** or a **Solid**. There are  four ways to build a box: 

* From three dimensions *dx, dy* and *dz*. The box is parallel to the axes and extends for <i>[0,dx] [0,dy] [0,dz] </i>. 
* From a point and three  dimensions. The same as above but the point is the new origin. 
* From two points, the box  is parallel to the axes and extends on the intervals defined by the coordinates  of the two points. 
* From a system of axes *gp_Ax2* and three dimensions. Same as the first way but the box is parallel to the given system of axes. 

An error is raised if  the box is flat in any dimension using the default precision. The following  code shows how to create a box: 
~~~~~
TopoDS_Solid theBox = BRepPrimAPI_MakeBox(10.,20.,30.); 
~~~~~

The four methods to build a box are shown in the figure: 

@image html /user_guides/modeling_algos/images/modeling_algos_image026.png  "Making Boxes"
@image latex /user_guides/modeling_algos/images/modeling_algos_image026.png  "Making Boxes"

@subsection occt_modalg_4_1_2 Wedge
*BRepPrimAPI_MakeWedge* class allows building a wedge, which is a slanted box, i.e. a  box with angles. The wedge is constructed in much the same way as a box i.e.  from three dimensions dx,dy,dz plus arguments or from an axis system, three  dimensions, and arguments. 

The following figure  shows two ways to build wedges. One is to add a dimension *ltx*, which is the length in *x* of the face at *dy*. The second is to add *xmin, xmax, zmin* and *zmax* to  describe the face at *dy*. 

The first method is a  particular case of the second with *xmin = 0, xmax = ltx, zmin = 0, zmax = dz*. 
To make a centered  pyramid you can use *xmin = xmax = dx / 2, zmin = zmax = dz / 2*. 

@image html /user_guides/modeling_algos/images/modeling_algos_image027.png "Making Wedges"
@image latex /user_guides/modeling_algos/images/modeling_algos_image027.png "Making Wedges"

@subsection occt_modalg_4_1_3 Rotation object
*BRepPrimAPI_MakeOneAxis* is a deferred class used as a root class for all classes constructing rotational primitives. Rotational primitives are  created by rotating a curve around an axis. They cover the cylinder, the cone,  the sphere, the torus, and the revolution, which provides all other curves. 

The particular  constructions of these primitives are described, but they all have some common  arguments, which are: 

  * A system of coordinates,  where the Z axis is the rotation axis..
  * An angle in the range  [0,2*PI].
  * A vmin, vmax parameter range  on the curve.

The result of the  OneAxis construction is a Solid, a Shell, or a Face. The face is the face  covering the rotational surface. Remember that you will not use the OneAxis  directly but one of the derived classes, which provide improved constructions.  The following figure illustrates the OneAxis arguments. 

@image html /user_guides/modeling_algos/images/modeling_algos_image028.png  "MakeOneAxis  arguments"
@image latex /user_guides/modeling_algos/images/modeling_algos_image028.png  "MakeOneAxis  arguments"

@subsection occt_modalg_4_1_4 Cylinder
*BRepPrimAPI_MakeCylinder* class allows creating cylindrical primitives. A cylinder is created either in the  default coordinate system or in a given coordinate system *gp_Ax2*. There are  two constructions: 

  * Radius and height, to build a  full cylinder.
  * Radius, height and angle to  build a portion of a cylinder.

The following code  builds the cylindrical face of the figure, which is a quarter of cylinder along  the *Y* axis with the origin at *X,Y,Z* the length of *DY* and radius *R*. 

~~~~~

Standard_Real X = 20, Y = 10, Z = 15, R = 10, DY = 30; 
// Make the system of coordinates 
gp_Ax2 axes = gp::ZOX(); 
axes.Translate(gp_Vec(X,Y,Z)); 
TopoDS_Face F = 
BRepPrimAPI_MakeCylinder(axes,R,DY,PI/2.); 
~~~~~
@image html /user_guides/modeling_algos/images/modeling_algos_image029.png  "Cylinder"
@image latex /user_guides/modeling_algos/images/modeling_algos_image029.png  "Cylinder"

@subsection occt_modalg_4_1_5 Cone
*BRepPrimAPI_MakeCone* class allows creating conical primitives. Like a cylinder, a cone is created either in  the default coordinate system or in a given coordinate system (gp_Ax2). There  are two constructions: 

  * Two radii and height, to  build a full cone. One of the radii can be null to make a sharp cone.
  * Radii, height and angle to  build a truncated cone.

The following code  builds the solid cone of the figure, which is located in the default system  with radii *R1* and *R2* and height *H*. 

~~~~~
Standard_Real R1 = 30, R2 = 10, H = 15; 
TopoDS_Solid S = BRepPrimAPI_MakeCone(R1,R2,H); 
~~~~~

@image html /user_guides/modeling_algos/images/modeling_algos_image030.png "Cone"
@image latex /user_guides/modeling_algos/images/modeling_algos_image030.png "Cone"

@subsection occt_modalg_4_1_6 Sphere
*BRepPrimAPI_MakeSphere* class allows creating spherical primitives. Like a cylinder, a  sphere is created either in the default coordinate system or in a given  coordinate system *gp_Ax2*. There are four constructions: 

  * From a radius -- builds a full  sphere. 
  * From a radius and an angle -- builds  a lune (digon).
  * From a radius and two angles -- builds a wraparound spherical segment between two latitudes. The angles *a1* and *a2* must follow the relation: <i>PI/2 <= a1 < a2 <= PI/2 </i>. 
  * From a radius and three angles -- a combination of two previous methods builds a portion of spherical segment. 

The following code  builds four spheres from a radius and three angles. 

~~~~~
Standard_Real R = 30, ang = 
	PI/2, a1 = -PI/2.3,  a2 = PI/4; 
TopoDS_Solid S1 = BRepPrimAPI_MakeSphere(R); 
TopoDS_Solid S2 = BRepPrimAPI_MakeSphere(R,ang); 
TopoDS_Solid S3 = BRepPrimAPI_MakeSphere(R,a1,a2); 
TopoDS_Solid S4 = BRepPrimAPI_MakeSphere(R,a1,a2,ang); 
~~~~~

Note that we could  equally well choose to create Shells instead of Solids. 

@image html /user_guides/modeling_algos/images/modeling_algos_image031.png  "Examples of  Spheres"
@image latex /user_guides/modeling_algos/images/modeling_algos_image031.png  "Examples of  Spheres"

@subsection occt_modalg_4_1_7 Torus
*BRepPrimAPI_MakeTorus* class allows creating toroidal primitives. Like the other  primitives, a torus is created either in the default coordinate system or in a  given coordinate system *gp_Ax2*. There are four constructions similar to the  sphere constructions: 

  * Two radii -- builds a full  torus.
  * Two radii and an angle -- builds  an angular torus segment.
  * Two radii and two angles --  builds a wraparound torus segment between two radial planes. The angles a1, a2 must follow  the relation 0 < a2 - a1 < 2*PI. 
  * Two radii and three angles -- a combination of two previous methods builds a portion of torus segment.

@image html /user_guides/modeling_algos/images/modeling_algos_image032.png "Examples of Tori"
@image latex /user_guides/modeling_algos/images/modeling_algos_image032.png "Examples of Tori"

The following code  builds four toroidal shells from two radii and three angles. 

~~~~~
Standard_Real R1 = 30, R2 = 10, ang = PI, a1 = 0, 
	a2 = PI/2; 
TopoDS_Shell S1 = BRepPrimAPI_MakeTorus(R1,R2); 
TopoDS_Shell S2 = BRepPrimAPI_MakeTorus(R1,R2,ang); 
TopoDS_Shell S3 = BRepPrimAPI_MakeTorus(R1,R2,a1,a2); 
TopoDS_Shell S4 = 
	BRepPrimAPI_MakeTorus(R1,R2,a1,a2,ang); 
~~~~~

Note that we could  equally well choose to create Solids instead of Shells. 

@subsection occt_modalg_4_1_8 Revolution
*BRepPrimAPI_MakeRevolution* class allows building a uniaxial primitive from a curve. As other uniaxial primitives it can be created in the default coordinate system  or in a given coordinate system. 

The curve can be any  *Geom_Curve*, provided it is planar and lies in the same plane as the Z-axis of  local coordinate system. There are four modes of construction: 

  * From a curve, use the full  curve and make a full rotation.
  * From a curve and an angle of  rotation.
  * From a curve and two  parameters to trim the curve. The two parameters must be growing and within the  curve range.
  * From a curve, two parameters,  and an angle. The two parameters must be growing and within the curve range.


@section occt_modalg_4_2 Sweeping:  Prism, Revolution and Pipe
@subsection occt_modalg_4_2_1 Sweeping

Sweeps are the objects  you obtain by sweeping a **profile** along a **path**. The profile can be of any topology. The path is usually a curve or a wire. The profile generates  objects according to the following rules: 

  * Vertices generate Edges
  * Edges generate Faces.
  * Wires generate Shells.
  * Faces generate Solids.
  * Shells generate Composite Solids

It is forbidden to sweep  Solids and Composite Solids. A Compound generates a Compound with the sweep of  all its elements. 

@image html /user_guides/modeling_algos/images/modeling_algos_image033.png "Generating a  sweep"
@image latex /user_guides/modeling_algos/images/modeling_algos_image033.png "Generating a  sweep"

*BRepPrimAPI_MakeSweep class* is a deferred class used as a root of the the following sweep classes:
* *BRepPrimAPI_MakePrism* -- produces a linear sweep
* *BRepPrimAPI_MakeRevol* -- produces a rotational sweep
* *BRepPrimAPI_MakePipe* -- produces a general sweep. 

@subsection occt_modalg_4_2_2 Prism
*BRepPrimAPI_MakePrism* class allows creating a linear **prism** from a shape and a vector or a direction. 
* A vector allows creating a finite  prism;
* A direction allows creating an infinite or semi-infinite prism. The semi-infinite or infinite  prism is toggled by a Boolean argument. All constructors have a boolean argument to copy the original  shape or share it (by default). 

The following code creates a finite, an infinite and a semi-infinite solid using a face, a  direction and a length. 

~~~~~
TopoDS_Face F = ..; // The swept face 
gp_Dir direc(0,0,1); 
Standard_Real l = 10; 
// create a vector from the direction and the length 
gp_Vec v = direc; 
v *= l; 
TopoDS_Solid P1 = BRepPrimAPI_MakePrism(F,v); 
// finite 
TopoDS_Solid P2 = BRepPrimAPI_MakePrism(F,direc); 
// infinite 
TopoDS_Solid P3 =  BRepPrimAPI_MakePrism(F,direc,Standard_False); 
// semi-infinite 
~~~~~

@image html /user_guides/modeling_algos/images/modeling_algos_image034.png   "Finite, infinite, and semi-infinite prisms"
@image latex /user_guides/modeling_algos/images/modeling_algos_image034.png   "Finite, infinite, and semi-infinite prisms"

@subsection occt_modalg_4_2_3 Rotational Sweep 
*BRepPrimAPI_MakeRevol* class allows creating a rotational sweep from a shape, an axis  (gp_Ax1), and an angle. The angle has a default value of 2*PI which means a  closed revolution. 

*BRepPrimAPI_MakeRevol* constructors  have a last argument to copy or share the original shape. The following code creates a a full and a partial rotation using a face, an axis and an angle.

~~~~~
TopoDS_Face F = ...; // the profile 
gp_Ax1 axis(gp_Pnt(0,0,0),gp_Dir(0,0,1)); 
Standard_Real ang = PI/3; 
TopoDS_Solid R1 = BRepPrimAPI_MakeRevol(F,axis); 
// Full revol 
TopoDS_Solid R2 = BRepPrimAPI_MakeRevol(F,axis,ang); 
~~~~~

@image html /user_guides/modeling_algos/images/modeling_algos_image035.png "Full and partial  rotation"
@image latex /user_guides/modeling_algos/images/modeling_algos_image035.png "Full and partial  rotation"


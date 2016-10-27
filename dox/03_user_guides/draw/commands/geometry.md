Geometry Commands {#user_guides__draw_commands_geometry}
=================

@tableofcontents

Draw provides a set of commands to test geometry libraries. These commands are found in the TGEOMETRY executable, or in 
any Draw executable which includes *GeometryTest* commands. 

In the context of Geometry, Draw includes the following types of variable: 

  * 2d and 3d points
  * The 2d curve, which corresponds to *Curve* in *Geom2d*.
  * The 3d curve and surface, which correspond to *Curve* and *Surface* in 
<a href="user_guides__modeling_data.html#occt_modat_1">Geom package</a>.
  
Draw geometric variables never share data; the *copy* command will always make a complete copy of the content of the variable. 

Where possible, the commands have been made broad in application, i.e. they apply to 2d curves, 3d curves and surfaces. 
For instance, the *circle* command may create a 2d or a 3d circle depending on the number of arguments given. 

Likewise, the *translate* command will process points, curves or surfaces, depending on argument type. You may not
 always find the specific command you are looking for in the section where you expect it to be. In that case, look in 
another section. The *trim* command, for example, is described in the surface section. It can, nonetheless, be used with
 curves as well. 

@section geom_curve_creation Curve Creation
This subsection deals with both points and curves. Types of curves are: 

  * Analytical curves such as lines, circles, ellipses, parabolas, and hyperbolas.
  * Polar curves such as bezier curves and bspline curves.
  * Trimmed curves and offset curves made from other curves with the *trim* and *offset* commands. Because they are used on both curves and surfaces, the *trim* and *offset* commands are described in the *surface creation* subsection.
  * NURBS can be created from other curves using *convert* in the *Surface Creation* subsection.
  * Curves can be created from the isoparametric lines of surfaces by the *uiso* and *viso* commands.
  * 3d curves can be created from 2d curves and vice versa using the *to3d* and *to2d* commands. The *project* command computes a 2d curve on a 3d surface.

Curves are displayed with an arrow showing the last parameter. 


@subsection occt_draw_6_2_1 point
      
~~~~~
point name x y [z] 
~~~~~
  
Creates a 2d or 3d point, depending on the number of arguments. 


~~~~~
# 2d point 
point p1 1 2 

# 3d point 
point p2 10 20 -5 
~~~~~
  
@subsection occt_draw_6_2_2  line

      
~~~~~
line name x y [z] dx dy [dz]
~~~~~ 

  
Creates a 2d or 3d line. *x y z* are the coordinates of the line’s point of origin; *dx, dy, dz* give the direction vector. 

A 2d line will be represented as *x y dx dy*, and a 3d line as *x y z dx dy dz* . A line is parameterized along its length starting from the point of origin along the direction vector. The direction vector is normalized and must not be null. Lines are infinite, even though their representation is not. 

 
~~~~~
# a 2d line at 45 degrees of the X axis 
line l 2 0 1 1 

# a 3d line through the point 10 0 0 and parallel to Z 
line l 10 0 0 0 0 1 
~~~~~

@subsection occt_draw_6_2_3  circle

      
~~~~~
circle name x y [z [dx dy dz]] [ux uy [uz]] radius
~~~~~ 

Creates a 2d or a 3d circle. 

In 2d, *x, y* are the coordinates of the center and *ux, uy* define the vector towards the point of origin of the parameters. By default, this direction is (1,0). The X Axis of the local coordinate system defines the origin of the parameters of the circle. Use another vector than the x axis to change the origin of parameters. 

In 3d, *x, y, z* are the coordinates of the center; *dx, dy, dz* give the vector normal to the plane of the circle. By default, this vector is (0,0,1) i.e. the Z axis (it must not be null). *ux, uy, uz* is the direction of the origin; if not given, a default direction will be computed. This vector must neither be null nor parallel to *dx, dy, dz*. 

The circle is parameterized by the angle in [0,2*pi] starting from the origin and. Note that the specification of origin direction and plane is the same for all analytical curves and surfaces. 

 
~~~~~
# A 2d circle of radius 5 centered at 10,-2 
circle c1 10 -2 5 

# another 2d circle with a user defined origin 
# the point of parameter 0 on this circle will be 
# 1+sqrt(2),1+sqrt(2) 
circle c2 1 1 1 1 2 

# a 3d circle, center 10 20 -5, axis Z, radius 17 
circle c3 10 20 -5 17 

# same 3d circle with axis Y 
circle c4 10 20 -5 0 1 0 17 

# full 3d circle, axis X, origin on Z 
circle c5 10 20 -5 1 0 0 0 0 1 17 
~~~~~

@subsection occt_draw_6_2_4  ellipse

 
~~~~~
ellipse name x y [z [dx dy dz]] [ux uy [uz]] firstradius secondradius 
~~~~~

Creates a 2d or 3d ellipse. In a 2d ellipse, the first two arguments define the center; in a 3d ellipse, the first three. The axis system is given by *firstradius*, the major radius, and *secondradius*, the minor radius. The parameter range of the ellipse is [0,2.*pi] starting from the X axis and going towards the Y axis. The Draw ellipse is parameterized by an angle: 

~~~~~
P(u) = O + firstradius*cos(u)*Xdir + secondradius*sin(u)*Ydir 
~~~~~
where: 

  * P is the point of parameter *u*,
  * *O, Xdir* and *Ydir* are respectively the origin, *X Direction* and *Y Direction* of its local coordinate system.
 

~~~~~
# default 2d ellipse 
ellipse e1 10 5 20 10 

# 2d ellipse at angle 60 degree 
ellipse e2 0 0 1 2 30 5 

# 3d ellipse, in the XY plane 
ellipse e3 0 0 0 25 5 

# 3d ellipse in the X,Z plane with axis 1, 0 ,1 
ellipse e4 0 0 0 0 1 0 1 0 1 25 5 
~~~~~

@subsection occt_draw_6_2_5  hyperbola

      
~~~~~
hyperbola name x y [z [dx dy dz]] [ux uy [uz]] firstradius secondradius
~~~~~ 

Creates a 2d or 3d conic. The first arguments define the center. The axis system is given by *firstradius*, the major radius, and *secondradius*, the minor radius. Note that the hyperbola has only one branch, that in the X direction. 

The Draw hyperbola is parameterized as follows: 
~~~~~
P(U) = O + firstradius*Cosh(U)*XDir + secondradius*Sinh(U)*YDir 
~~~~~
where: 

  * *P* is the point of parameter *U*,
  * *O, XDir* and *YDir* are respectively the origin, *X Direction* and *YDirection* of its local coordinate system. 

 
~~~~~
# default 2d hyperbola, with asymptotes 1,1 -1,1 
hyperbola h1 0 0 30 30 

# 2d hyperbola at angle 60 degrees 
hyperbola h2 0 0 1 2 20 20 

# 3d hyperbola, in the XY plane 
hyperbola h3 0 0 0 50 50 
~~~~~

@subsection occt_draw_6_2_6  parabola

      
~~~~~
parabola name x y [z [dx dy dz]] [ux uy [uz]] FocalLength 
~~~~~

Creates a 2d or 3d parabola. in the axis system defined by the first arguments. The origin is the apex of the parabola. 

The *Geom_Parabola* is parameterized as follows: 

~~~~~
P(u) = O + u*u/(4.*F)*XDir + u*YDir 
~~~~~

where: 
  * *P* is the point of parameter *u*,
  * *O, XDir* and *YDir* are respectively the origin, *X Direction* and *Y Direction* of its local coordinate system,
  * *F* is the focal length of the parabola.

 
~~~~~
# 2d parabola 
parabola p1 0 0 50 

# 2d parabola with convexity +Y 
parabola p2 0 0 0 1 50 

# 3d parabola in the Y-Z plane, convexity +Z 
parabola p3 0 0 0 1 0 0 0 0 1 50 
~~~~~

@subsection occt_draw_6_2_7  beziercurve, 2dbeziercurve

      
~~~~~
beziercurve name nbpole pole, [weight] 
2dbeziercurve name nbpole pole, [weight]
~~~~~ 

Creates a 3d rational or non-rational Bezier curve. Give the number of poles (control points,) and the coordinates of the poles *(x1 y1 z1 [w1] x2 y2 z2 [w2])*. The degree will be *nbpoles-1*. To create a rational curve, give weights with the poles. You must give weights for all poles or for none. If the weights of all the poles are equal, the curve is polynomial, and therefore non-rational. 

 
~~~~~
# a rational 2d bezier curve (arc of circle) 
2dbeziercurve ci 3 0 0 1 10 0 sqrt(2.)/2. 10 10 1 

# a 3d bezier curve, not rational 
beziercurve cc 4 0 0 0 10 0 0 10 0 10 10 10 10 
~~~~~

@subsection occt_draw_6_2_8  bsplinecurve, 2dbsplinecurve, pbsplinecurve, 2dpbsplinecurve

      
~~~~~
bsplinecurve   name degree nbknots knot, umult pole, weight
2dbsplinecurve name degree nbknots knot, umult pole, weight

pbsplinecurve   name degree nbknots knot, umult pole, weight (periodic)
2dpbsplinecurve name degree nbknots knot, umult pole, weight (periodic)
~~~~~

Creates 2d or 3d bspline curves; the **pbsplinecurve** and **2dpbsplinecurve** commands create periodic bspline curves. 

A bspline curve is defined by its degree, its periodic or non-periodic nature, a table of knots and a table of poles (i.e. control points). Consequently, specify the degree, the number of knots, and for each knot, the multiplicity, for each pole, the weight. In the syntax above, the commas link the adjacent arguments which they fall between: knot and multiplicities, pole and weight. 

The table of knots is an increasing sequence of reals without repetition. 
Multiplicities must be lower or equal to the degree of the curve. For non-periodic curves, the first and last multiplicities can be equal to degree+1. For a periodic curve, the first and last multiplicities must be equal. 

The poles must be given with their weights, use weights of 1 for a non rational curve, the number of poles must be: 

  * For a non periodic curve: Sum of multiplicities - degree + 1
  * For a periodic curve: Sum of multiplicities - last multiplicity

 
~~~~~
# a bspline curve with 4 poles and 3 knots 
bsplinecurve bc 2 3 0 3 1 1 2 3 \ 
10 0 7 1 7 0 7 1 3 0 8 1 0 0 7 1 
# a 2d periodic circle (parameter from 0 to 2*pi !!) 
dset h sqrt(3)/2 
2dpbsplinecurve c 2 \ 
4 0 2 pi/1.5 2 pi/0.75 2 2*pi 2 \ 
0 -h/3 1 \ 
0.5 -h/3 0.5 \ 
0.25 h/6 1 \ 
0 2*h/3 0.5 \ 
-0.25 h/6 1 \ 
-0.5 -h/3 0.5 \ 
0 -h/3 1 
~~~~~

**Note** that you can create the **NURBS** subset of bspline curves and surfaces by trimming analytical curves and surfaces and executing the command *convert*. 


@subsection occt_draw_6_2_9  uiso, viso

      
~~~~~
uiso name surface u 
viso name surface u 
~~~~~

Creates a U or V isoparametric curve from a surface. 

 
~~~~~
# create a cylinder and extract iso curves 

cylinder c 10 
uiso c1 c pi/6 
viso c2 c 
~~~~~

**Note** that this cannot be done from offset surfaces.


@subsection occt_draw_6_2_10  to3d, to2d

      
~~~~~
to3d name curve2d [plane] 
to2d name curve3d [plane] 
~~~~~

Create respectively a 3d curve from a 2d curve and a 2d curve from a 3d curve. The transformation uses a planar surface to define the XY plane in 3d (by default this plane is the default OXYplane). **to3d** always gives a correct result, but as **to2d** is not a projection, it may surprise you. It is always correct if the curve is planar and parallel to the plane of projection. The points defining the curve are projected on the plane. A circle, however, will remain a circle and will not be changed to an ellipse. 

 
~~~~~
# the following commands 
circle c 0 0 5 
plane p -2 1 0 1 2 3 
to3d c c p 

# will create the same circle as 
circle c -2 1 0 1 2 3 5 
~~~~~

See also: **project** 


@subsection occt_draw_6_2_11  project

      
~~~~~
project name curve3d surface 
~~~~~

Computes a 2d curve in the parametric space of a surface corresponding to a 3d curve. This can only be used on analytical surfaces. 

If we, for example, intersect a cylinder and a plane and project the resulting ellipse on the cylinder, this will create a 2d sinusoid-like bspline. 

~~~~~
cylinder c 5 
plane p 0 0 0 0 1 1 
intersect i c p 
project i2d i c 
~~~~~

@section geom_surface Surface Creation
The following types of surfaces exist: 
  * Analytical surfaces: plane, cylinder, cone, sphere, torus;
  * Polar surfaces: bezier surfaces, bspline surfaces;
  * Trimmed and Offset surfaces;
  * Surfaces produced by Revolution and Extrusion, created from curves with the *revsurf* and *extsurf*;
  * NURBS surfaces.

Surfaces are displayed with isoparametric lines. To show the parameterization, a small parametric line with a length 1/10 of V is displayed at 1/10 of U. 

@subsection occt_draw_6_3_1  plane

      
~~~~~
plane name [x y z [dx dy dz [ux uy uz]]]
~~~~~ 

Creates an infinite plane. 

A plane is the same as a 3d coordinate system, *x,y,z* is the origin, *dx, dy, dz* is the Z direction and *ux, uy, uz* is the X direction. 

The plane is perpendicular to Z and X is the U parameter. *dx,dy,dz* and *ux,uy,uz* must not be null or collinear. *ux,uy,uz* will be modified to be orthogonal to *dx,dy,dz*. 

There are default values for the coordinate system. If no arguments are given, the global system (0,0,0), (0,0,1), (1,0,0). If only the origin is given, the axes are those given by default(0,0,1), (1,0,0). If the origin and the Z axis are given, the X axis is generated perpendicular to the Z axis. 

Note that this definition will be used for all analytical surfaces. 

 
~~~~~
# a plane through the point 10,0,0 perpendicular to X 
# with U direction on Y 
plane p1 10 0 0 1 0 0 0 1 0 

# an horixontal plane with origin 10, -20, -5 
plane p2 10 -20 -5 
~~~~~

@subsection occt_draw_6_3_2  cylinder

      
~~~~~
cylinder name [x y z [dx dy dz [ux uy uz]]] radius 
~~~~~

A cylinder is defined by a coordinate system, and a radius. The surface generated is an infinite cylinder with the Z axis as the axis. The U parameter is the angle starting from X going in the Y direction. 

 
~~~~~
# a cylinder on the default Z axis, radius 10 
cylinder c1 10 

# a cylinder, also along the Z axis but with origin 5, 
10, -3 
cylinder c2 5 10 -3 10 

# a cylinder through the origin and on a diagonal 
# with longitude pi/3 and latitude pi/4 (euler angles) 
dset lo pi/3. la pi/4. 
cylinder c3 0 0 0 cos(la)*cos(lo) cos(la)*sin(lo) 
sin(la) 10 
~~~~~

@subsection occt_draw_6_3_3  cone

      
~~~~~
cone name [x y z [dx dy dz [ux uy uz]]] semi-angle radius 
~~~~~
Creates a cone in the infinite coordinate system along the Z-axis. The radius is that of the circle at the intersubsection of the cone and the XY plane. The semi-angle is the angle formed by the cone relative to the axis; it should be between -90 and 90. If the radius is 0, the vertex is the origin. 

 
~~~~~
# a cone at 45 degrees at the origin on Z 
cone c1 45 0 

# a cone on axis Z with radius r1 at z1 and r2 at z2 
cone c2 0 0 z1 180.*atan2(r2-r1,z2-z1)/pi r1 
~~~~~

@subsection occt_draw_6_3_4  sphere

      
~~~~~
sphere name [x y z [dx dy dz [ux uy uz]]] radius 
~~~~~

Creates a sphere in the local coordinate system defined in the **plane** command. The sphere is centered at the origin. 

To parameterize the sphere, *u* is the angle from X to Y, between 0 and 2*pi. *v* is the angle in the half-circle at angle *u* in the plane containing the Z axis. *v* is between -pi/2 and pi/2. The poles are the points Z = +/- radius; their parameters are u,+/-pi/2 for any u in 0,2*pi. 


~~~~~ 
# a sphere at the origin 
sphere s1 10 
# a sphere at 10 10 10, with poles on the axis 1,1,1 
sphere s2 10 10 10 1 1 1 10 
~~~~~

@subsection occt_draw_6_3_5  torus

      
~~~~~
torus name [x y z [dx dy dz [ux uy uz]]] major minor
~~~~~ 

Creates a torus in the local coordinate system with the given major and minor radii. *Z* is the axis for the major radius. The major radius may be lower in value than the minor radius. 

To parameterize a torus, *u* is the angle from X to Y; *v* is the angle in the plane at angle *u* from the XY plane to Z. *u* and *v* are in 0,2*pi. 

 
~~~~~
# a torus at the origin 
torus t1 20 5 

# a torus in another coordinate system 
torus t2 10 5 -2 2 1 0 20 5 
~~~~~


@subsection occt_draw_6_3_6  beziersurf

      
~~~~~
beziersurf name nbupoles nbvolpes pole, [weight] 
~~~~~

Use this command to create a bezier surface, rational or non-rational. First give the numbers of poles in the u and v directions. 

Then give the poles in the following order: *pole(1, 1), pole(nbupoles, 1), pole(1, nbvpoles)* and *pole(nbupoles, nbvpoles)*. 

Weights may be omitted, but if you give one weight you must give all of them. 

 
~~~~~
# a non-rational degree 2,3 surface 
beziersurf s 3 4 \ 
0 0 0 10 0 5 20 0 0 \ 
0 10 2 10 10 3 20 10 2 \ 
0 20 10 10 20 20 20 20 10 \ 
0 30 0 10 30 0 20 30 0 
~~~~~

@subsection occt_draw_6_3_7   bsplinesurf, upbsplinesurf, vpbsplinesurf, uvpbsplinesurf

     
~~~~~
bsplinesurf name udegree nbuknots uknot umult ... nbvknot vknot 
vmult ... x y z w ... 
upbsplinesurf ... 
vpbsplinesurf ... 
uvpbsplinesurf ... 
~~~~~

* **bsplinesurf** generates bspline surfaces;
* **upbsplinesurf** creates a bspline surface periodic in u; 
* **vpbsplinesurf** creates one periodic in v; 
* **uvpbsplinesurf** creates one periodic in uv. 

The syntax is similar to the *bsplinecurve* command. First give the degree in u and the knots in u with their multiplicities, then do the same in v. The poles follow. The number of poles is the product of the number in u and the number in v. 

See *bsplinecurve* to compute the number of poles, the poles are first given in U as in the *beziersurf* command. You must give weights if the surface is rational. 

 
~~~~~
# create a bspline surface of degree 1 2 
# with two knots in U and three in V 
bsplinesurf s \ 
1 2 0 2 1 2 \ 
2 3 0 3 1 1 2 3 \ 
0 0 0 1 10 0 5 1 \ 
0 10 2 1 10 10 3 1 \ 
0 20 10 1 10 20 20 1 \ 
0 30 0 1 10 30 0 1 
~~~~~


@subsection occt_draw_6_3_8  trim, trimu, trimv

      
~~~~~
trim newname name [u1 u2 [v1 v2]] 
trimu newname name 
trimv newname name 
~~~~~

The **trim** commands create trimmed curves or trimmed surfaces. Note that trimmed curves and surfaces are classes of the *Geom* package. 
* *trim* creates either a new trimmed curve from a curve or a new trimmed surface in u and v from a surface.
* *trimu* creates a u-trimmed surface, 
* *trimv* creates a v-trimmed surface. 

After an initial trim, a second execution with no parameters given recreates the basis curve. The curves can be either 2d or 3d. If the trimming parameters decrease and if the curve or surface is not periodic, the direction is reversed. 

**Note** that a trimmed curve or surface contains a copy of the basis geometry: modifying that will not modify the trimmed geometry. Trimming trimmed geometry will not create multiple levels of trimming. The basis geometry will be used. 

 
~~~~~
# create a 3d circle 
circle c 0 0 0 10 

# trim it, use the same variable, the original is 
deleted 
trim c c 0 pi/2 

# the original can be recovered! 
trim orc c 

# trim again 
trim c c pi/4 pi/2 

# the original is not the trimmed curve but the basis 
trim orc c 

# as the circle is periodic, the two following commands 
are identical 
trim cc c pi/2 0 
trim cc c pi/2 2*pi 

# trim an infinite cylinder 
cylinder cy 10 
trimv cy cy 0 50 
~~~~~

@subsection occt_draw_6_3_9  offset

      
~~~~~
offset name basename distance [dx dy dz]
~~~~~ 

Creates offset curves or surfaces at a given distance from a basis curve or surface. Offset curves and surfaces are classes from the *Geom *package. 

The curve can be a 2d or a 3d curve. To compute the offsets for a 3d curve, you must also give a vector *dx,dy,dz*. For a planar curve, this vector is usually the normal to the plane containing the curve. 

The offset curve or surface copies the basic geometry, which can be modified later. 

 
~~~~~
# graphic demonstration that the outline of a torus 
# is the offset of an ellipse 
smallview +X+Y 
dset angle pi/6 
torus t 0 0 0 0 cos(angle) sin(angle) 50 20 
fit 
ellipse e 0 0 0 50 50*sin(angle) 
# note that the distance can be negative 
offset l1 e 20 0 0 1 
~~~~~

@subsection occt_draw_6_3_10  revsurf

      
~~~~~
revsurf name curvename x y z dx dy dz
~~~~~ 

Creates a surface of revolution from a 3d curve. 

A surface of revolution or revolved surface is obtained by rotating a curve (called the *meridian*) through a complete revolution about an axis (referred to as the *axis of revolution*). The curve and the axis must be in the same plane (the *reference plane* of the surface). Give the point of origin x,y,z and the vector dx,dy,dz to define the axis of revolution. 

To parameterize a surface of revolution: u is the angle of rotation around the axis. Its origin is given by the position of the meridian on the surface. v is the parameter of the meridian. 

 
~~~~~
# another way of creating a torus like surface 
circle c 50 0 0 20 
revsurf s c 0 0 0 0 1 0 
~~~~~

@subsection occt_draw_6_3_11  extsurf

      
~~~~~
extsurf newname curvename dx dy dz 
~~~~~

Creates a surface of linear extrusion from a 3d curve. The basis curve is swept in a given direction,the *direction of extrusion* defined by a vector. 

In the syntax, *dx,dy,dz* gives the direction of extrusion. 

To parameterize a surface of extrusion: *u* is the parameter along the extruded curve; the *v* parameter is along the direction of extrusion. 

 
~~~~~
# an elliptic cylinder 
ellipse e 0 0 0 10 5 
extsurf s e 0 0 1 
# to make it finite 
trimv s s 0 10 
~~~~~

@subsection occt_draw_6_3_12  convert

      
~~~~~
convert newname name 
~~~~~

Creates a 2d or 3d NURBS curve or a NURBS surface from any 2d curve, 3d curve or surface. In other words, conics, beziers and bsplines are turned into NURBS. Offsets are not processed.
 
 
~~~~~
# turn a 2d arc of a circle into a 2d NURBS 
circle c 0 0 5 
trim c c 0 pi/3 
convert c1 c 

# an easy way to make a planar bspline surface 
plane p 
trim p p -1 1 -1 1 
convert p1 p 
~~~~~

@note Offset curves and surfaces are not processed by this command.

@section geom_modification Curve and Surface Modification 
Draw provides commands to modify curves and surfaces, some of them are general, others restricted to bezier curves or bsplines. 

General modifications: 

  * Reversing the parametrization: **reverse**, **ureverse**, **vreverse**

Modifications for both bezier curves and bsplines: 

  * Exchanging U and V on a surface: **exchuv**
  * Segmentation: **segment**, **segsur**
  * Increasing the degree: **incdeg**, **incudeg**, **incvdeg**
  * Moving poles: **cmovep**, **movep**, **movecolp**, **moverowp**

Modifications for bezier curves: 

  * Adding and removing poles: **insertpole**, **rempole**, **remcolpole**, **remrowpole**

Modifications for bspline: 

  * Inserting and removing knots: **insertknot**, **remknot**, **insertuknot**, **remuknot**, **insetvknot**, **remvknot**
  * Modifying periodic curves and surfaces: **setperiodic**, **setnotperiodic**, **setorigin**, **setuperiodic**, **setunotperiodic**, **setuorigin**, **setvperiodic**, **setvnotperiodic**, **setvorigin**


@subsection occt_draw_6_4_1  reverse, ureverse, vreverse
            
~~~~~
reverse curvename 
ureverse surfacename 
vreverse surfacename 
~~~~~

The **reverse** command reverses the parameterization and inverses the orientation of a 2d or 3d curve. Note that the geometry is modified. To keep the curve or the surface, you must copy it before modification. 

**ureverse** or **vreverse** reverse the u or v parameter of a surface. Note that the new parameters of the curve may change according to the type of curve. For instance, they will change sign on a line or stay 0,1 on a bezier. 

Reversing a parameter on an analytical surface may create an indirect coordinate system. 

 
~~~~~
# reverse a trimmed 2d circle 
circle c 0 0 5 
trim c c pi/4 pi/2 
reverse c 

# dumping c will show that it is now trimmed between 
# 3*pi/2 and 7*pi/4 i.e. 2*pi-pi/2 and 2*pi-pi/4 
~~~~~

@subsection occt_draw_6_4_2  exchuv

                 
~~~~~
exchuv surfacename 
~~~~~

For a bezier or bspline surface this command exchanges the u and v parameters. 

 
~~~~~
# exchanging u and v on a spline (made from a cylinder) 
cylinder c 5 
trimv c c 0 10 
convert c1 c 
exchuv c1 
~~~~~

@subsection occt_draw_6_4_3  segment, segsur

                  
~~~~~
segment curve Ufirst Ulast 
segsur surface Ufirst Ulast Vfirst Vlast 
~~~~~

**segment** and **segsur** segment a bezier curve and a bspline curve or surface respectively. 

These commands modify the curve to restrict it between the new parameters: *Ufirst*, the starting point of the modified curve, and *Ulast*, the end point. *Ufirst* is less than *Ulast*. 

This command must not be confused with **trim** which creates a new geometry. 

 
~~~~~
# segment a bezier curve in half 
beziercurve c 3 0 0 0 10 0 0 10 10 0 
segment c ufirst ulast 
~~~~~

@subsection occt_draw_6_4_4  iincudeg, incvdeg

      
~~~~~
incudeg surfacename newdegree 
incvdeg surfacename newdegree 
~~~~~

**incudeg** and **incvdeg** increase the degree in the U or V parameter of a bezier or bspline surface.
 
 
~~~~~
# make a planar bspline and increase the degree to 2 3 
plane p 
trim p p -1 1 -1 1 
convert p1 p 
incudeg p1 2 
incvdeg p1 3 
~~~~~

**Note** that the geometry is modified.


@subsection occt_draw_6_4_5  cmovep, movep, movecolp, moverowp

      
~~~~~
cmovep curve index dx dy [dz] 
movep surface uindex vindex dx dy dz 
movecolp surface uindex dx dy dz 
moverowp surface vindex dx dy dz 
~~~~~

**move** methods translate poles of a bezier curve, a bspline curve or a bspline surface. 
* **cmovep** and **movep** translate one pole with a given index. 
* **movecolp** and **moverowp** translate a whole column (expressed by the *uindex*) or row (expressed by the *vindex*) of poles. 

 
~~~~~
# start with a plane 
# transform to bspline, raise degree and add relief 
plane p 
trim p p -10 10 -10 10 
convert p1 p 
incud p1 2 
incvd p1 2 
movecolp p1 2 0 0 5 
moverowp p1 2 0 0 5 
movep p1 2 2 0 0 5 
~~~~~

@subsection occt_draw_6_4_6  insertpole, rempole, remcolpole, remrowpole

                  
~~~~~
insertpole curvename index x y [z] [weight] 
rempole curvename index 
remcolpole surfacename index 
remrowpole surfacename index
~~~~~ 

**insertpole** inserts a new pole into a 2d or 3d bezier curve. You may add a weight for the pole. The default value for the weight is 1. The pole is added at the position after that of the index pole. Use an index 0 to insert the new pole before the first one already existing in your drawing. 

**rempole** removes a pole from a 2d or 3d bezier curve. Leave at least two poles in the curves. 

**remcolpole** and **remrowpole** remove a column or a row of poles from a bezier surface. A column is in the v direction and a row in the u direction The resulting degree must be at least 1; i.e there will be two rows and two columns left. 

 
~~~~~
# start with a segment, insert a pole at end 
# then remove the central pole 
beziercurve c 2 0 0 0 10 0 0 
insertpole c 2 10 10 0 
rempole c 2 
~~~~~

@subsection occt_draw_6_4_7  insertknot, insertuknot, insertvknot

                  
~~~~~
insertknot name knot [mult = 1] [knot mult ...] 
insertuknot surfacename knot mult 
insertvknot surfacename knot mult 
~~~~~

**insertknot** inserts knots in the knot sequence of a bspline curve. You must give a knot value and a target multiplicity. The default multiplicity is 1. If there is already a knot with the given value and a multiplicity lower than the target one, its multiplicity will be raised. 

**insertuknot** and **insertvknot** insert knots in a surface. 

 
~~~~~
# create a cylindrical surface and insert a knot 
cylinder c 10 
trim c c 0 pi/2 0 10 
convert c1 c 
insertuknot c1 pi/4 1 
~~~~~

@subsection occt_draw_6_4_8  remknot, remuknot, remvknot

      
~~~~~
remknot index [mult] [tol] 
remuknot index [mult] [tol] 
remvknot index [mult] [tol] 
~~~~~

**remknot** removes a knot from the knot sequence of a curve or a surface. Give the index of the knot and optionally, the target multiplicity. If the target multiplicity is not 0, the multiplicity of the knot will be lowered. As the curve may be modified, you are allowed to set a tolerance to control the process. If the tolerance is low, the knot will only be removed if the curve will not be modified. 

By default, if no tolerance is given, the knot will always be removed. 

 
~~~~~
# bspline circle, remove a knot 
circle c 0 0 5 
convert c1 c 
incd c1 5 
remknot c1 2 
~~~~~

**Note** that Curves or Surfaces may be modified.


@subsection occt_draw_6_4_9  setperiodic, setnotperiodic, setuperiodic, setunotperiodic, setvperiodic, setvnotperiodic

      
~~~~~
setperiodic curve 
setnotperiodic curve 
setuperiodic surface 
setunotperiodic surface 
setvperiodic surface 
setvnotperiodic surface
~~~~~ 

**setperiodic** turns a bspline curve into a periodic bspline curve; the knot vector stays the same and excess poles are truncated. The curve may be modified if it has not been closed. **setnotperiodic** removes the periodicity of a periodic curve. The pole table mau be modified. Note that knots are added at the beginning and the end of the knot vector and the multiplicities are knots set to degree+1 at the start and the end. 

**setuperiodic** and **setvperiodic** make the u or the v parameter of bspline surfaces periodic; **setunotperiodic**, and **setvnotperiodic** remove periodicity from the u or the v parameter of bspline surfaces. 

 
~~~~~
# a circle deperiodicized 
circle c 0 0 5 
convert c1 c 
setnotperiodic c1 
~~~~~

@subsection occt_draw_6_4_10  setorigin, setuorigin, setvorigin

      
~~~~~
setorigin curvename index 
setuorigin surfacename index 
setuorigin surfacename index 
~~~~~

These commands change the origin of the parameters on periodic curves or surfaces. The new origin must be an existing knot. To set an origin other than an existing knot, you must first insert one with the *insertknot* command. 

 
~~~~~
# a torus with new U and V origins 
torus t 20 5 
convert t1 t 
setuorigin t1 2 
setvorigin t1 2
~~~~~ 

@section trsf Transformations

Draw provides commands to apply linear transformations to geometric objects: they include translation, rotation, 
mirroring and scaling. 

@subsection occt_draw_6_5_1  translate, dtranslate

> translate name [names ...] dx dy dz 
> 2dtranslate name [names ...] dx dy 

The <i>Translate</i> command translates 3d points, curves and surfaces along a vector <i>dx,dy,dz</i>. You can translate
 more than one object with the same command. 

For 2d points or curves, use the <i>2dtranslate</i> command. 
 
~~~~~
# 3d tranlation 
point p 10 20 30 
circle c 10 20 30 5 
torus t 10 20 30 5 2 
translate p c t 0 0 15
~~~~~
 
@note Objects are modified by this command. 

@subsection occt_draw_6_5_2  rotate, 2drotate

> rotate name [name ...] x y z dx dy dz angle 
> 2drotate name [name ...] x y angle

The <i>rotate</i> command rotates a 3d point curve or surface. You must give an axis of rotation with a point *x,y,z*, a 
vector *dx,dy,dz* and an angle in degrees. 

For a 2d rotation, you need only give the center point and the angle. In 2d or 3d, the angle can be negative. 
 
~~~~~
# make a helix of circles. create a scripte file with 
this code and execute it using **source**. 
circle c0 10 0 0 3 
for {set i 1} {$i = 10} {incr i} { 
copy c[expr $i-1] c$i 
translate c$i 0 0 3 
rotate c$i 0 0 0 0 0 1 36 
} 
~~~~~

@subsection occt_draw_6_5_3  pmirror, lmirror, smirror, dpmirror, dlmirror

The mirror commands perform a mirror transformation of 2d or 3d geometry. 

* **pmirror** is the point mirror, mirroring 3d curves and surfaces about a point of symmetry. 
> pmirror name [names ...] x y z 

* **lmirror** is the line mirror commamd, mirroring 3d curves and surfaces about an axis of symmetry.
> lmirror name [names ...] x y z dx dy dz 

* **smirror** is the surface mirror, mirroring 3d curves and surfaces about a plane of symmetry. In the last case, the
 plane of symmetry is perpendicular to dx,dy,dz. 
> smirror name [names ...] x y z dx dy dz 

* **2dpmirror** is the point mirror in 2D.
> 2dpmirror name [names ...] x y 

* **2dlmirror** is the axis symmetry mirror in 2D.
> 2dlmirror name [names ...] x y dx dy 


 ~~~~~
# build 3 images of a torus 
torus t 10 10 10 1 2 3 5 1 
copy t t1 
pmirror t1 0 0 0 
copy t t2 
lmirror t2 0 0 0 1 0 0 
copy t t3 
smirror t3 0 0 0 1 0 0 
~~~~~

@subsection occt_draw_6_5_4  pscale, dpscale

The **pscale** and **2dpscale** commands transform an object by point scaling. You must give the center and the scaling
 factor. Because other scalings modify the type of the object, they are not provided. For example, a sphere may be 
transformed into an ellipsoid. Using a scaling factor of -1 is similar to using **pmirror**.   
               
> pscale name [name ...] x y z s 
> 2dpscale name [name ...] x y s 

~~~~~
# double the size of a sphere 
sphere s 0 0 0 10 
pscale s 0 0 0 2 
~~~~~

@section intersection Intersections

## intersect

This command computes intersections of surfaces.
    
> intersect name surface1 surface2

Intersects two surfaces; if there is one intersection curve it will be named *name*, if there are more than one they 
will be named *name_1*, *name_2*, ... 
 
~~~~~
# create an ellipse 
cone c 45 0 
plane p 0 0 40 0 1 5 
intersect e c p 
~~~~~

## dintersect
This command computes intersections of 2d curves.

> 2dintersect curve1 curve2 

Displays the intersection points between two 2d curves. 
 
~~~~~
# intersect two 2d ellipses 
ellipse e1 0 0 5 2 
ellipse e2 0 0 0 1 5 2 
2dintersect e1 e2 
~~~~~

@section approx Approximation

## appro, dapprox
These commands fit a curve through a set of points. First give the number of points, then choose one of the three ways
 available to get the points. If you have no arguments, click on the points. If you have a curve argument or a list of
 points, the command launches computation of the points on the curve.  
  
> appro result nbpoint [curve] 
> 2dapprox result nbpoint [curve / x1 y1 x2 y2]

~~~~~
# Let us pick points and they will be fitted 
2dapprox c 10 
~~~~~

## surfapp, grilapp

* **surfapp** fits a surface through an array of u and v points, nbupoints*nbvpoints. 
> surfapp name nbupoints nbvpoints x y z .... 

* **grilapp** has the same function, but the x,y coordinates of the points are on a grid starting at x0,y0 with steps dx,dy. 
> grilapp name nbupoints nbvpoints xo dx yo dy z11 z12 ... 

 
~~~~~
# a surface using the same data as in the beziersurf 
example sect 4.4 
surfapp s 3 4 \ 
0 0 0 10 0 5 20 0 0 \ 
0 10 2 10 10 3 20 10 2 \ 
0 20 10 10 20 20 20 20 10 \ 
0 30 0 10 30 0 20 30 0 
~~~~~

@section analysis Curve and Surface Analysis

# coord
Sets the x, y (and optionally z) coordinates of the point P. 

> coord P x y [z] 

~~~~~
# translate a point 
point p 10 5 5 
translate p 5 0 0 
coord p x y z 
# x value is 15 
~~~~~

# cvalue, 2dcvalue
Compute points and derivatives on curves.

> cvalue curve U x y z [d1x d1y d1z [d2x d2y d2z]] 
> 2dcvalue curve U x y [d1x d1y [d2x d2y]] 

For a curve at a given parameter, and depending on the number of arguments, **cvalue** computes the coordinates in
 *x,y,z*, the first derivative in *d1x,d1y,d1z* and the second derivative in *d2x,d2y,d2z*. 

Let on a bezier curve at parameter 0 the point is the first pole; the first derivative is the vector to the second pole 
multiplied by the degree; the second derivative is the difference first to the second pole, second to the third pole
 multipied by *degree-1* : 

~~~~~
2dbeziercurve c 4 0 0 1 1 2 1 3 0 
2dcvalue c 0 x y d1x d1y d2x d2y 

# values of x y d1x d1y d2x d2y 
# are 0 0 3 3 0 -6 
~~~~~

# svalue
 
Computes points and derivatives on a surface.

> svalue surfname U v x y z [dux duy duz dvx dvy dvz [d2ux d2uy d2uz d2vx d2vy d2vz d2uvx d2uvy d2uvz]] 

Computes points and derivatives on a surface for a pair of parameter values. The result depends on the number of 
arguments. You can compute the first and the second derivatives. 

~~~~~
# display points on a sphere 
sphere s 10 
for {dset t 0} {[dval t] = 1} {dset t t+0.01} { 
svalue s t*2*pi t*pi-pi/2 x y z 
point . x y z 
} 
~~~~~

# localprop, minmaxcurandinf

**localprop** computes the curvature of a curve. 
> localprop curvename U 

**minmaxcurandinf** computes and prints the parameters of the points where the curvature is minimum and maximum on a 
2d curve. 
> minmaxcurandinf curve
 
~~~~~
# show curvature at the center of a bezier curve 
beziercurve c 3 0 0 0 10 2 0 20 0 0 
localprop c 0.5 
== Curvature : 0.02 
~~~~~

# parameters      
Computes (u,v) values for a point on a surface.

> parameters surf/curve x y z U [V] 

Returns the parameters on the surface of the 3d point *x,y,z* in variables *u* and *v*. This command may only be used on
 analytical surfaces: plane, cylinder, cone, sphere and torus. 
 
~~~~~
# Compute parameters on a plane 
plane p 0 0 10 1 1 0 
parameters p 5 5 5 u v 
# the values of u and v are : 0 5 
~~~~~

# proj, dproj
Use **proj** to project a point on a 3d curve or a surface and **2dproj** for a 2d curve. 
      
> proj name x y z 
> 2dproj name xy 

The command will compute and display all points in the projection. The lines joining the point to the projections are 
created with the names *ext_1, ext_2, ... * 

~~~~~
# project a point on a torus 
torus t 20 5 
proj t 30 10 7 
== ext_1 ext_2 ext_3 ext_4 
~~~~~

# surface_radius
Computes the main curvatures of a surface at parameters *(u,v)*.

> surface_radius surface u v [c1 c2] 

If there are extra arguments, their curvatures are stored in variables *c1* and *c2*. 

~~~~~
# compute curvatures of a cylinder
cylinder c 5 
surface_radius c pi 3 c1 c2 
== Min Radius of Curvature : -5 
== Min Radius of Curvature : infinite 
~~~~~

@section Constraints

##cirtang
Constructs 2d circles tangent to curves;
 
> cirtang cname curve/point/radius curve/point/radius curve/point/radius 

Builds all circles satisfying the three constraints which are either a curve (the circle must be tangent to that curve),
 a point (the circle must pass through that point), or a radius for the circle. Only one constraint can be a radius. The
 solutions will be stored in variables *name_1*, *name_2*, etc. 
 
~~~~~
# a point, a line and a radius. 2 solutions 
point p 0 0 
line 1 10 0 -1 1 
cirtang c p 1 4 
== c_1 c_2 
~~~~~

## lintan
Constructs 2d lines tangent to curves. 

> lintan name curve curve [angle] 

Builds all 2d lines tangent to two curves. If the third angle argument is given the second curve must be a line and 
**lintan** will build all lines tangent to the first curve and forming the given angle with the line. The angle is given 
in degrees. The solutions are named *name_1*, *name_2*, etc. 

 
~~~~~
# lines tangent to 2 circles, 4 solutions 
circle c1 -10 0 10 
circle c2 10 0 5 
lintan l c1 c2 

# lines at 15 degrees tangent to a circle and a line, 2 
solutions: l1_1 l1_2 
circle c1 -10 0 1 
line l 2 0 1 1 
lintan l1 c1 l 15 
~~~~~

@section display Display

Draw provides commands to control the display of geometric objects. Some display parameters are used for all objects, 
others are valid for surfaces only, some for bezier and bspline only, and others for bspline only. 

On curves and surfaces, you can control the mode of representation with the **dmode** command. You can control the 
parameters for the mode with the **defle** command and the **discr** command, which control deflection and discretization 
respectively. 

On surfaces, you can control the number of isoparametric curves displayed on the surface with the **nbiso** command. 

On bezier and bspline curve and surface you can toggle the display of the control points with the **clpoles** and 
**shpoles** commands. 

On bspline curves and surfaces you can toggle the display of the knots with the **shknots** and **clknots** commands. 

## dmod, discr, defle

> dmode name [name ...] u/d 
> discr name [name ...] nbintervals 
> defle name [name ...] deflection 

**dmod** command allows choosing the display mode for a curve or a surface. 

In mode *u*, or *uniform deflection*, the points are computed to keep the polygon at a distance lower than the 
deflection of the geometry. The deflection is set with the *defle* command. This mode involves intensive use of 
computational power. 

In *d*, or discretization mode, a fixed number of points is computed. This number is set with the *discr* command. This 
is the default mode. On a bspline, the fixed number of points is computed for each span of the curve. (A span is the 
interval between two knots). 

If the curve or the isolines seem to present too many angles, you can either increase the discretization or lower the 
deflection, depending on the mode. This will increase the number of points. 
 
~~~~~
# increment the number of points on a big circle 
circle c 0 0 50 50 
discr 100 

# change the mode 
dmode c u 
~~~~~

## nbiso
      
> nbiso name [names...] nuiso nviso 

Changes the number of isoparametric curves displayed on a surface in the U and V directions. On a bspline surface, 
isoparametric curves are displayed by default at knot values. Use *nbiso* to turn this feature off. 
 
~~~~~ 
# display 35 meridians and 15 parallels on a sphere
sphere s 20 
nbiso s 35 15 
~~~~~

## clpoles, shpoles
      
> clpoles name 

> shpoles name 

On bezier and bspline curves and surfaces, the control polygon is displayed by default: *clpoles* erases it and *shpoles* restores it. 

~~~~~
# make a bezier curve and erase the poles 
beziercurve c 3 0 0 0 10 0 0 10 10 0 
clpoles c 
~~~~~

##  clknots, shknots

> clknots name 

> shknots name 

By default, knots on a bspline curve or surface are displayed with markers at the points with parametric value equal to 
the knots. *clknots* removes them and *shknots* restores them. 
 
~~~~~
# hide the knots on a bspline curve 
bsplinecurve bc 2 3 0 3 1 1 2 3 \ 
10 0 7 1 7 0 7 1 3 0 8 1 0 0 7 1 
clknots bc
~~~~~



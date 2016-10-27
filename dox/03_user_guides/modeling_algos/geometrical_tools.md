Geometrical Tools {#user_guides__modeling_algos_geom_tools}
=================

@tableofcontents

Open CASCADE Technology geometric tools provide algorithms to: 
  * Calculate the intersection of two 2D curves, surfaces, or a 3D curve and a surface;
  * Project points onto 2D and 3D curves, points onto surfaces, and 3D curves onto surfaces;
  * Construct lines and circles from constraints; 
  * Construct curves and surfaces from constraints; 
  * Construct curves and surfaces by interpolation.
  
@section occt_modalg_2_2 Intersections

The Intersections component is used to compute intersections between 2D or 3D geometrical objects: 
  * the intersections between two 2D curves;
  * the self-intersections of a 2D curve;
  * the intersection between a 3D curve and a surface;
  * the intersection between two surfaces.

The *Geom2dAPI_InterCurveCurve* class  allows the evaluation of the intersection points (*gp_Pnt2d*) between two  geometric curves (*Geom2d_Curve*) and the evaluation of the points  of self-intersection of a curve. 

@image html /03_user_guides/modeling_algos/images/modeling_algos_image003.png  "Intersection and self-intersection of curves"
@image latex /03_user_guides/modeling_algos/images/modeling_algos_image003.png  "Intersection and self-intersection of curves"

In both cases, the  algorithm requires a value for the tolerance (Standard_Real) for the confusion  between two points. The default tolerance value used in all constructors is *1.0e-6.* 

@image html /03_user_guides/modeling_algos/images/modeling_algos_image004.png "Intersection and tangent intersection"
@image latex /03_user_guides/modeling_algos/images/modeling_algos_image004.png "Intersection and tangent intersection"

The algorithm returns a  point in the case of an intersection and a segment in the case of tangent  intersection. 

@subsection occt_modalg_2_2_1 Intersection of two curves

*Geom2dAPI_InterCurveCurve* class may be instantiated for intersection of curves *C1* and *C2*.
~~~~~
Geom2dAPI_InterCurveCurve Intersector(C1,C2,tolerance); 
~~~~~

or for self-intersection of curve *C3*.
~~~~~
Geom2dAPI_InterCurveCurve Intersector(C3,tolerance); 
~~~~~

~~~~~
Standard_Integer N = Intersector.NbPoints(); 
~~~~~
Calls the number of intersection points

To select the desired intersection point, pass an integer index value in argument. 
~~~~~
gp_Pnt2d P = Intersector.Point(Index); 
~~~~~

To call the number of intersection segments, use
~~~~~
Standard_Integer M = Intersector.NbSegments(); 
~~~~~

To select the desired intersection segment pass integer index values in argument. 
~~~~~
Handle(Geom2d_Curve) Seg1, Seg2; 
Intersector.Segment(Index,Seg1,Seg2); 
// if intersection of 2 curves 
Intersector.Segment(Index,Seg1); 
// if self-intersection of a curve 
~~~~~

If you need access to a wider range of functionalities the following method will return the algorithmic  object for the calculation of intersections: 

~~~~~
Geom2dInt_GInter& TheIntersector = Intersector.Intersector(); 
~~~~~

@subsection occt_modalg_2_2_2 Intersection of Curves and Surfaces

The *GeomAPI_IntCS* class  is used to compute the intersection points between a curve and a surface. 

This class is  instantiated as follows: 
~~~~~
GeomAPI_IntCS Intersector(C, S); 
~~~~~

To call the number of intersection points, use:
~~~~~
Standard_Integer nb = Intersector.NbPoints(); 
~~~~~


~~~~~
gp_Pnt& P = Intersector.Point(Index); 
~~~~~

Where *Index* is an  integer between 1 and *nb*, calls the intersection points.

@subsection occt_modalg_2_2_3 Intersection of two Surfaces
The *GeomAPI_IntSS* class  is used to compute the intersection of two surfaces from *Geom_Surface* with  respect to a given tolerance. 

This class is  instantiated as follows: 
~~~~~
GeomAPI_IntSS Intersector(S1, S2, Tolerance); 
~~~~~
Once the *GeomAPI_IntSS* object has been created, it can be interpreted. 

~~~~~
Standard_Integer nb = Intersector. NbLines(); 
~~~~~
Calls the number of intersection curves.

~~~~~
Handle(Geom_Curve) C = Intersector.Line(Index) 
~~~~~
Where *Index* is an  integer between 1 and *nb*, calls the intersection curves.


@section occt_modalg_2_3  Interpolations

The Interpolation Laws component provides definitions of functions: <i> y=f(x) </i>.

In particular, it provides definitions of:
  * a linear function,
  * an <i> S </i> function, and
  * an interpolation function for a range of values.

Such functions can be used to define, for example, the evolution law of a fillet along the edge of a shape.

The validity of the function built is never checked: the Law package does not know for what application or to what end the function will be used. In particular, if the function is used as the evolution law of a fillet, it is important that the function is always positive. The user must check this.

@subsection occt_modalg_2_3_1 Geom2dAPI_Interpolate
This class is used to  interpolate a BSplineCurve passing through an array of points. If tangency is  not requested at the point of interpolation, continuity will be *C2*. If  tangency is requested at the point, continuity will be *C1*. If  Periodicity is requested, the curve will be closed and the junction will be the  first point given. The curve will then have a continuity of *C1* only. 
This class may be  instantiated as follows: 
~~~~~
Geom2dAPI_Interpolate 
(const  Handle_TColgp_HArray1OfPnt2d& Points, 
const  Standard_Boolean PeriodicFlag, 
const Standard_Real  Tolerance); 

Geom2dAPI_Interpolate Interp(Points, Standard_False, 
                                    Precision::Confusion()); 
~~~~~


It is possible to call the BSpline curve from the object defined  above it. 
~~~~~
Handle(Geom2d_BSplineCurve) C = Interp.Curve(); 
~~~~~

Note that the *Handle(Geom2d_BSplineCurve)* operator has been redefined by the method *Curve()*. Consequently, it is  unnecessary to pass via the construction of an intermediate object of the *Geom2dAPI_Interpolate* type and the following syntax is correct. 

~~~~~
Handle(Geom2d_BSplineCurve) C = 
Geom2dAPI_Interpolate(Points, 
    Standard_False, 
    Precision::Confusion()); 
~~~~~

@subsection occt_modalg_2_3_2 GeomAPI_Interpolate

This class may be  instantiated as follows: 
~~~~~
GeomAPI_Interpolate 
(const  Handle_TColgp_HArray1OfPnt& Points, 
const  Standard_Boolean PeriodicFlag, 
const Standard_Real  Tolerance); 

GeomAPI_Interpolate Interp(Points, Standard_False, 
                                    Precision::Confusion()); 
~~~~~

It is possible to call the BSpline curve from the object defined  above it. 
~~~~~
Handle(Geom_BSplineCurve) C = Interp.Curve(); 
~~~~~
Note that the *Handle(Geom_BSplineCurve)* operator has been redefined by the method *Curve()*. Thus, it is unnecessary  to pass via the construction of an intermediate object of the *GeomAPI_Interpolate*  type and the following syntax is correct. 

Handle(Geom_BSplineCurve) C = 
	GeomAPI_Interpolate(Points,  
						Standard_False,
						1.0e-7); 

Boundary conditions may  be imposed with the method Load. 
~~~~~
GeomAPI_Interpolate AnInterpolator 
(Points, Standard_False, 1.0e-5); 
AnInterpolator.Load (StartingTangent, EndingTangent); 
~~~~~

@section occt_modalg_2_4 Lines and  Circles from Constraints

@subsection occt_modalg_2_4_1 Types of constraints

The algorithms for construction of 2D circles or lines can be described with numeric or geometric constraints in relation to other curves. 

These constraints can impose the following :
  * the radius of a circle,
  * the angle that a straight line makes with another straight line,
  * the tangency of a straight line or circle in relation to a curve,
  * the passage of a straight line or circle through a point,
  * the circle with center in a point or curve.

For example, these algorithms enable to easily construct a circle of a given radius, centered on a straight line and tangential to another circle.

The implemented algorithms are more complex than those provided by the Direct Constructions component for building 2D circles or lines.

The expression of a tangency problem generally leads to several results, according to the relative positions of the solution and the circles or straight lines in relation to which the tangency constraints are expressed. For example, consider the following
case of a circle of a given radius (a small one) which is tangential to two secant circles C1 and C2:

@figure{/03_user_guides/modeling_algos/images/modeling_algos_image058.png,"Example of a Tangency Constraint"}

This diagram clearly shows that there are 8 possible solutions.

In order to limit the number of solutions, we can try to express the relative position
of the required solution in relation to the circles to which it is tangential. For
example, if we specify that the solution is inside the circle C1 and outside the
circle C2, only two solutions referenced 3 and 4 on the diagram respond to the problem
posed.

These definitions are very easy to interpret on a circle, where it is easy to identify
the interior and exterior sides. In fact, for any kind of curve the interior is defined
as the left-hand side of the curve in relation to its orientation.

This technique of qualification of a solution, in relation to the curves to which
it is tangential, can be used in all algorithms for constructing a circle or a straight
line by geometric constraints. Four qualifiers are used:
  * **Enclosing** -- the solution(s) must enclose the argument;
  * **Enclosed** -- the solution(s) must be enclosed by the argument;
  * **Outside** -- the solution(s) and the argument must be external to one another;
  * **Unqualified** -- the relative position is not qualified, i.e. all solutions apply.
    
It is possible to create expressions using the qualifiers,  for example:
~~~~~
GccAna_Circ2d2TanRad 
	Solver(GccEnt::Outside(C1), 
		GccEnt::Enclosing(C2),  Rad, Tolerance); 
~~~~~

This expression finds all circles  of radius *Rad*, which are tangent to both circle *C1* and *C2*, while *C1* is outside and *C2* is inside.
  
@subsection occt_modalg_2_4_2 Available types of lines and circles

The following analytic algorithms using value-handled entities for creation of 2D lines or circles with geometric constraints are available: 
  * circle tangent to three  elements (lines, circles, curves, points),
  * circle tangent to two  elements and having a radius,
  * circle tangent to two  elements and centered on a third element,
  * circle tangent to two  elements and centered on a point,
  * circle tangent to one element  and centered on a second,
  * bisector of two points,
  * bisector of two lines,
  * bisector of two circles,
  * bisector of a line and a  point,
  * bisector of a circle and a  point,
  * bisector of a line and a  circle,
  * line tangent to two elements  (points, circles, curves),
  * line tangent to one element  and parallel to a line,
  * line tangent to one element  and perpendicular to a line,
  * line tangent to one element  and forming angle with a line.

### Exterior/Interior
It is not hard to define  the interior and exterior of a circle. As is shown in the following diagram,  the exterior is indicated by the sense of the binormal, that is to say the  right side according to the sense of traversing the circle. The left side is  therefore the interior (or &quot;material&quot;). 

@image html /03_user_guides/modeling_algos/images/modeling_algos_image006.png "Exterior/Interior of a Circle"
@image latex /03_user_guides/modeling_algos/images/modeling_algos_image006.png "Exterior/Interior of a Circle"

By extension, the  interior of a line or any open curve is defined as the left side according to  the passing direction, as shown in the following diagram: 

@image html /03_user_guides/modeling_algos/images/modeling_algos_image007.png "Exterior/Interior of a Line and a Curve"
@image latex /03_user_guides/modeling_algos/images/modeling_algos_image007.png "Exterior/Interior of a Line and a Curve"

### Orientation of a Line
It is sometimes  necessary to define in advance the sense of travel along a line to be created.  This sense will be from first to second argument. 

The following figure shows a line, which is  first tangent to circle C1 which is interior to the line, and then passes  through point P1. 

@image html /03_user_guides/modeling_algos/images/modeling_algos_image008.png "An Oriented Line"
@image latex /03_user_guides/modeling_algos/images/modeling_algos_image008.png "An Oriented Line"


### Line tangent to two circles
The following four  diagrams illustrate four cases of using qualifiers in the creation of a line.  The fifth shows the solution if no qualifiers are given.


**Example 1 Case 1** 

@image html /03_user_guides/modeling_algos/images/modeling_algos_image009.png "Both circles outside"
@image latex /03_user_guides/modeling_algos/images/modeling_algos_image009.png "Both circles outside"

Constraints: 
Tangent and Exterior to  C1. 
Tangent and Exterior to  C2. 

Syntax: 

~~~~~
GccAna_Lin2d2Tan 
	Solver(GccEnt::Outside(C1), 
		GccEnt::Outside(C2), 
		Tolerance); 
~~~~~

**Example 1 Case 2** 

@image html /03_user_guides/modeling_algos/images/modeling_algos_image010.png "Both circles enclosed"
@image latex /03_user_guides/modeling_algos/images/modeling_algos_image010.png "Both circles enclosed"

Constraints: 
Tangent and Including  C1. 
Tangent and Including  C2. 

Syntax: 

~~~~~
GccAna_Lin2d2Tan 
	Solver(GccEnt::Enclosing(C1), 
		GccEnt::Enclosing(C2), 
		Tolerance); 
~~~~~

**Example  1 Case 3**
 
@image html /03_user_guides/modeling_algos/images/modeling_algos_image011.png "C1 enclosed, C2 outside"
@image latex /03_user_guides/modeling_algos/images/modeling_algos_image011.png "C1 enclosed, C2 outside"

Constraints: 
Tangent and Including C1. 
Tangent and Exterior to C2. 

Syntax: 
~~~~~
GccAna_Lin2d2Tan 
	Solver(GccEnt::Enclosing(C1), 
		GccEnt::Outside(C2), 
		Tolerance); 
~~~~~

**Example 1 Case 4** 

@image html /03_user_guides/modeling_algos/images/modeling_algos_image012.png "C1 outside, C2 enclosed"
@image latex /03_user_guides/modeling_algos/images/modeling_algos_image012.png "C1 outside, C2 enclosed"
Constraints: 
Tangent and Exterior to  C1. 
Tangent and Including  C2. 

Syntax: 
~~~~~
GccAna_Lin2d2Tan 
	Solver(GccEnt::Outside(C1), 
		GccEnt::Enclosing(C2), 
		Tolerance); 
~~~~~

**Example 1 Case 5** 

@image html /03_user_guides/modeling_algos/images/modeling_algos_image013.png "With no qualifiers specified"
@image latex /03_user_guides/modeling_algos/images/modeling_algos_image013.png "With no qualifiers specified"

Constraints: 
Tangent and Undefined  with respect to C1. 
Tangent and Undefined  with respect to C2. 

Syntax: 
~~~~~
GccAna_Lin2d2Tan 
	Solver(GccEnt::Unqualified(C1), 
		GccEnt::Unqualified(C2), 
		Tolerance); 
~~~~~

### Circle of given radius tangent to two circles
The following four  diagrams show the four cases in using qualifiers in the creation of a circle. 

**Example 2 Case 1** 
@image html /03_user_guides/modeling_algos/images/modeling_algos_image014.png "Both solutions outside"
@image latex /03_user_guides/modeling_algos/images/modeling_algos_image014.png "Both solutions outside"

Constraints: 
Tangent and Exterior to  C1. 
Tangent and Exterior to  C2. 

Syntax: 
~~~~~
GccAna_Circ2d2TanRad 
	Solver(GccEnt::Outside(C1), 
	GccEnt::Outside(C2),  Rad, Tolerance); 
~~~~~

**Example 2 Case 2** 

@image html /03_user_guides/modeling_algos/images/modeling_algos_image015.png "C2 encompasses C1"
@image latex /03_user_guides/modeling_algos/images/modeling_algos_image015.png "C2 encompasses C1"

Constraints: 
Tangent and Exterior to  C1. 
Tangent and Included by  C2. 

Syntax: 
~~~~~
GccAna_Circ2d2TanRad 
	Solver(GccEnt::Outside(C1), 
		GccEnt::Enclosed(C2),  Rad, Tolerance); 
~~~~~

**Example  2 Case 3**
@image html /03_user_guides/modeling_algos/images/modeling_algos_image016.png "Solutions enclose C2"
@image latex /03_user_guides/modeling_algos/images/modeling_algos_image016.png "Solutions enclose C2"

Constraints: 
Tangent and Exterior to  C1. 
Tangent and Including  C2. 

Syntax: 
~~~~~
GccAna_Circ2d2TanRad 
	Solver(GccEnt::Outside(C1), 
		GccEnt::Enclosing(C2),  Rad, Tolerance); 
~~~~~
		
**Example 2 Case 4**
@image html /03_user_guides/modeling_algos/images/modeling_algos_image017.png "Solutions enclose C1"
@image latex /03_user_guides/modeling_algos/images/modeling_algos_image017.png "Solutions enclose C1"

Constraints: 
Tangent and Enclosing  C1. 
Tangent and Enclosing  C2. 

Syntax: 
~~~~~
GccAna_Circ2d2TanRad 
	Solver(GccEnt::Enclosing(C1), 
		GccEnt::Enclosing(C2),  Rad, Tolerance); 
~~~~~

**Example 2 Case 5**

The following syntax  will give all the circles of radius *Rad*, which are tangent to *C1* and *C2* without discrimination of relative position: 

~~~~~
GccAna_Circ2d2TanRad  Solver(GccEnt::Unqualified(C1), 
							GccEnt::Unqualified(C2), 
							Rad,Tolerance); 
~~~~~							


@subsection occt_modalg_2_4_3 Types of Algorithms

OCCT implements several categories of algorithms:

* **Analytic** algorithms, where solutions are obtained by the resolution of an equation, such algorithms are used when the geometries which are worked on (tangency arguments,   position of the center, etc.) are points, lines or circles;
* **Geometric** algorithms, where the solution is generally obtained by calculating the intersection of parallel or bisecting curves built from geometric arguments;
* **Iterative** algorithms, where the solution is obtained by a process of iteration.
  
For each kind of geometric construction of a constrained line or circle, OCCT provides two types of access:

  * algorithms from the package <i> Geom2dGcc </i> automatically select the algorithm best suited to the problem, both in the general case and in all types of specific cases; the used arguments  are *Geom2d* objects, while the computed solutions are <i> gp </i> objects;
  * algorithms from the package <i> GccAna</i> resolve the problem analytically, and can only be used when the geometries to be worked on are lines or circles; both the used arguments and the computed solutions  are <i> gp </i> objects.

The provided algorithms compute all solutions, which correspond to the stated geometric problem, unless the solution is found by an iterative algorithm.

Iterative algorithms compute only one solution, closest to an initial position. They can be used in the following cases:
  * to build a circle, when an argument is more complex than a line or a circle, and where the radius is not known or difficult to determine: this is the case for a circle tangential to three geometric elements, or tangential to two geometric elements and centered on a curve;
  * to build a line, when a tangency argument is more complex than a line or a circle.

Qualified curves (for tangency arguments) are provided either by:
  * the <i> GccEnt</i> package, for direct use by <i> GccAna</i> algorithms, or
  * the <i> Geom2dGcc </i> package, for general use by <i> Geom2dGcc </i> algorithms.

The <i> GccEnt</i> and <i> Geom2dGcc</i> packages also provide simple functions for building qualified curves in a very efficient way.

The <i> GccAna </i>package also provides algorithms for constructing bisecting loci between circles, lines or points. Bisecting loci between two geometric objects are such that each of their points is at the same distance from the two geometric objects. They
are typically curves, such as circles, lines or conics for <i> GccAna</i> algorithms. 
Each elementary solution is given as an elementary bisecting locus object (line, circle, ellipse, hyperbola, parabola), described by the <i>GccInt</i> package.

Note: Curves used by <i>GccAna</i> algorithms to define the geometric problem to be solved, are 2D lines or circles from the <i> gp</i> package: they are not explicitly parameterized. However, these lines or circles retain an implicit parameterization, corresponding to that which they induce on equivalent Geom2d objects. This induced parameterization is the one used when returning parameter values on such curves, for instance with the functions <i> Tangency1, Tangency2, Tangency3, Intersection2</i> and <i> CenterOn3</i> provided by construction algorithms from the <i> GccAna </i> or <i> Geom2dGcc</i> packages.

@section occt_modalg_2_5 Curves and Surfaces from Constraints

The Curves and Surfaces from Constraints component groups together high level functions used in 2D and 3D geometry for:
  * creation of faired and minimal variation 2D curves
  * construction of ruled surfaces
  * construction of pipe surfaces
  * filling of surfaces
  * construction of plate surfaces
  * extension of a 3D curve or surface beyond its original bounds.
  
OPEN CASCADE company also provides a product known as <a href="http://www.opencascade.com/content/surfaces-scattered-points">Surfaces from Scattered Points</a>, which allows constructing surfaces from scattered points. This algorithm accepts or constructs an initial B-Spline surface and looks for its deformation (finite elements method) which would satisfy the constraints. Using optimized computation methods, this algorithm is able to construct a surface from more than 500 000 points.

SSP product is not supplied with Open CASCADE Technology, but can be purchased separately.

@subsection occt_modalg_2_5_1 Faired and Minimal Variation 2D Curves

Elastic beam curves have their origin in traditional methods of modeling applied 
in boat-building, where a long thin piece of wood, a lathe, was forced to pass
between two sets of nails and in this way, take the form of a curve based on the
two points, the directions of the forces applied at those points, and the properties
of the wooden lathe itself.

Maintaining these constraints requires both longitudinal and transversal forces to
be applied to the beam in order to compensate for its internal elasticity. The longitudinal
forces can be a push or a pull and the beam may or may not be allowed to slide over
these fixed points.

### Batten Curves

The class *FairCurve_Batten* allows  producing faired curves defined on the basis of one or more constraints on  each of the two reference points. These include point, angle of tangency and  curvature settings. 
The following constraint orders are available: 

  * 0 the curve must pass through  a point
  * 1 the curve must pass through  a point and have a given tangent
  * 2 the curve must pass through  a point, have a given tangent and a given curvature.

Only 0 and 1 constraint orders are used. 
The function Curve  returns the result as a 2D BSpline curve. 

### Minimal Variation Curves

The class *FairCurve_MinimalVariation* allows producing curves with minimal variation in  curvature at each reference point. The following constraint  orders are available: 

  * 0 the curve must pass through  a point
  * 1 the curve must pass through  a point and have a given tangent
  * 2 the curve must pass through  a point, have a given tangent and a given curvature.

Constraint orders of 0, 1 and 2 can be used. The algorithm minimizes tension, sagging and jerk energy. 

The function *Curve* returns  the result as a 2D BSpline curve. 

If you want to give a  specific length to a batten curve, use: 

~~~~~
b.SetSlidingFactor(L / b.SlidingOfReference()) 
~~~~~
where *b* is the name of  the batten curve object 

Free sliding is  generally more aesthetically pleasing than constrained sliding. However, the computation  can fail with values such as angles greater than *p/2* because in this case the length is theoretically infinite. 

In other cases, when  sliding is imposed and the sliding factor is too large, the batten can  collapse. 

The constructor parameters, *Tolerance* and *NbIterations*, control how precise the computation is,  and how long it will take. 

@subsection occt_modalg_2_5_2 Ruled Surfaces 

A ruled surface is built by ruling a line along the length of two curves.

### Creation of Bezier surfaces

The class *GeomFill_BezierCurves* allows producing a Bezier surface from contiguous Bezier curves. Note  that problems may occur with rational Bezier Curves. 

#### Creation of BSpline surfaces

The class *GeomFill_BSplineCurves* allows producing a BSpline surface from contiguous BSpline curves.  Note that problems may occur with rational BSplines. 

@subsection occt_modalg_2_5_3 Pipe Surfaces

The class *GeomFill_Pipe* allows producing a pipe by sweeping a curve (the section) along another curve  (the path). The result is a BSpline surface. 

The following types of construction are available:
  * pipes with a circular section of constant radius,
  * pipes with a constant section,
  * pipes with a section evolving between two given curves.
    
@subsection occt_modalg_2_5_4 Filling a contour

It is often convenient to create a surface from some curves, which will form the boundaries that define the new surface.
This is done by the class *GeomFill_ConstrainedFilling*, which allows filling a contour defined by three or four curves as well as by tangency constraints. The resulting surface is a BSpline. 

A case in point is the intersection of two fillets at a corner. If the radius of the fillet on one edge is different from that of the fillet on another, it becomes impossible to sew together all the edges of the resulting surfaces. This leaves a gap in the overall surface of the object which you are constructing.

@figure{/03_user_guides/modeling_algos/images/modeling_algos_image059.png,"Intersecting filleted edges with differing radiuses"}

These algorithms allow you to fill this gap from two, three or four curves. This can be done with or without constraints, and the resulting surface will be either a Bezier or a BSpline surface in one of a range of filling styles.

### Creation of a Boundary

The class *GeomFill_SimpleBound* allows you defining a boundary for the surface to be constructed. 

### Creation of a Boundary with an adjoining surface

The class *GeomFill_BoundWithSurf* allows defining a boundary for the surface to be constructed. This boundary will already be joined to another surface. 

### Filling styles

The enumerations *FillingStyle* specify the styles used to build the surface. These include: 

  * *Stretch* -- the style with the flattest patches
  * *Coons* -- a rounded style with less depth than *Curved*
  * *Curved* -- the style with the most rounded patches.

@image html /03_user_guides/modeling_algos/images/modeling_algos_image018.png "Intersecting filleted edges with different radii leave a gap, is filled by a surface"
@image latex /03_user_guides/modeling_algos/images/modeling_algos_image018.png "Intersecting filleted edges with different radii leave a gap, is filled by a surface"

@subsection occt_modalg_2_5_5 Plate surfaces

In CAD, it is often necessary to generate a surface which has no exact mathematical definition, but which is defined by respective constraints. These can be of a mathematical, a technical or an aesthetic order.

Essentially, a plate surface is constructed by deforming a surface so that it conforms to a given number of curve or point constraints. In the figure below, you can see four segments of the outline of the plane, and a point which have been used as the
curve constraints and the point constraint respectively. The resulting surface can be converted into a BSpline surface by using the function <i> MakeApprox </i>.

The surface is built using a variational spline algorithm. It uses the principle of deformation of a thin plate by localised mechanical forces. If not already given in the input, an initial surface is calculated. This corresponds to the plate prior
to deformation. Then, the algorithm is called to calculate the final surface. It looks for a solution satisfying constraints and minimizing energy input.

@figure{/03_user_guides/modeling_algos/images/modeling_algos_image061.png,"Surface generated from two curves and a point"}

The package *GeomPlate*   provides the following services for creating surfaces respecting curve and  point constraints: 

### Definition of a Framework

The class *BuildPlateSurface* allows creating a framework to build surfaces according to curve and  point constraints as well as tolerance settings. The result is returned with  the function *Surface*. 

Note that you do not have to specify an initial surface at the time of construction. It can be added later  or, if none is loaded, a surface will  be computed automatically. 

### Definition of a Curve Constraint

The class *CurveConstraint* allows defining curves as constraints to the surface, which you want  to build. 

### Definition of a Point Constraint

The class *PointConstraint* allows defining points as constraints to the surface, which you want  to build. 

### Applying Geom_Surface to Plate Surfaces

The class *Surface* allows describing the characteristics of plate surface objects returned by **BuildPlateSurface::Surface** using the methods of *Geom_Surface* 

### Approximating a Plate surface to a BSpline

The class *MakeApprox* allows converting a *GeomPlate* surface into a *Geom_BSplineSurface*. 

@figure{/03_user_guides/modeling_algos/images/modeling_algos_image060.png,"Surface generated from four curves and a point"}

Let us create a Plate surface  and approximate it from a polyline as a curve constraint and a point constraint 

~~~~~
Standard_Integer NbCurFront=4, 
NbPointConstraint=1; 
gp_Pnt P1(0.,0.,0.); 
gp_Pnt P2(0.,10.,0.); 
gp_Pnt P3(0.,10.,10.); 
gp_Pnt P4(0.,0.,10.); 
gp_Pnt P5(5.,5.,5.); 
BRepBuilderAPI_MakePolygon W; 
W.Add(P1); 
W.Add(P2); 
W.Add(P3); 
W.Add(P4); 
W.Add(P1); 
// Initialize a BuildPlateSurface 
GeomPlate_BuildPlateSurface BPSurf(3,15,2); 
// Create the curve constraints 
BRepTools_WireExplorer anExp; 
for(anExp.Init(W); anExp.More(); anExp.Next()) 
{ 
TopoDS_Edge E = anExp.Current(); 
Handle(BRepAdaptor_HCurve) C = new 
BRepAdaptor_HCurve(); 
C-ChangeCurve().Initialize(E); 
Handle(BRepFill_CurveConstraint) Cont= new 
BRepFill_CurveConstraint(C,0); 
BPSurf.Add(Cont); 
} 
// Point constraint 
Handle(GeomPlate_PointConstraint) PCont= new 
GeomPlate_PointConstraint(P5,0); 
BPSurf.Add(PCont); 
// Compute the Plate surface 
BPSurf.Perform(); 
// Approximation of the Plate surface 
Standard_Integer MaxSeg=9; 
Standard_Integer MaxDegree=8; 
Standard_Integer CritOrder=0; 
Standard_Real dmax,Tol; 
Handle(GeomPlate_Surface) PSurf = BPSurf.Surface(); 
dmax = Max(0.0001,10*BPSurf.G0Error()); 
Tol=0.0001; 
GeomPlate_MakeApprox 
Mapp(PSurf,Tol,MaxSeg,MaxDegree,dmax,CritOrder); 
Handle (Geom_Surface) Surf (Mapp.Surface()); 
// create a face corresponding to the approximated Plate 
Surface 
Standard_Real Umin, Umax, Vmin, Vmax; 
PSurf-Bounds( Umin, Umax, Vmin, Vmax); 
BRepBuilderAPI_MakeFace MF(Surf,Umin, Umax, Vmin, Vmax); 
~~~~~

@section occt_modalg_2_6 Projections

Projections provide for computing the following:
  * the projections of a 2D point onto a 2D curve
  * the projections of a 3D point onto a 3D curve or surface
  * the projection of a 3D curve onto a surface.
  * the planar curve transposition from the 3D to the 2D parametric space of an underlying plane and v. s.
  * the positioning of a 2D gp object in the 3D geometric space.

@subsection occt_modalg_2_6_1 Projection of a 2D Point on a Curve

*Geom2dAPI_ProjectPointOnCurve*  allows calculation of all normals projected from a point (*gp_Pnt2d*)  onto a geometric curve (*Geom2d_Curve*). The calculation may be restricted  to a given domain. 

@image html /03_user_guides/modeling_algos/images/modeling_algos_image020.png  "Normals from a point to a curve"
@image latex /03_user_guides/modeling_algos/images/modeling_algos_image020.png  "Normals from a point to a curve"

The  curve does not have to be a *Geom2d_TrimmedCurve*. The algorithm will function with any class inheriting *Geom2d_Curve*. 

The class *Geom2dAPI_ProjectPointOnCurve* may be instantiated as in the following example: 

~~~~~
gp_Pnt2d P; 
Handle(Geom2d_BezierCurve) C = 
	new  Geom2d_BezierCurve(args); 
Geom2dAPI_ProjectPointOnCurve Projector (P, C); 
~~~~~

To restrict the search  for normals to a given domain <i>[U1,U2]</i>, use the following constructor: 
~~~~~
Geom2dAPI_ProjectPointOnCurve Projector (P, C, U1, U2); 
~~~~~
Having thus created the *Geom2dAPI_ProjectPointOnCurve* object, we can now interrogate it. 

### Calling the number of solution points

~~~~~
Standard_Integer NumSolutions = Projector.NbPoints(); 
~~~~~

### Calling the location of a solution point

The solutions are  indexed in a range from *1* to *Projector.NbPoints()*. The point,  which corresponds to a given *Index* may be found: 
~~~~~
gp_Pnt2d Pn = Projector.Point(Index); 
~~~~~

### Calling the parameter of a solution point

For a given point  corresponding to a given *Index*: 

~~~~~
Standard_Real U = Projector.Parameter(Index); 
~~~~~

This can also be  programmed as: 

~~~~~
Standard_Real U; 
Projector.Parameter(Index,U); 
~~~~~

### Calling the distance between the start and end points

We can find the distance  between the initial point and a point, which corresponds to the given *Index*: 

~~~~~
Standard_Real D = Projector.Distance(Index); 
~~~~~

### Calling the nearest solution point

This class offers a  method to return the closest solution point to the starting point. This  solution is accessed as follows: 
~~~~~
gp_Pnt2d P1 = Projector.NearestPoint(); 
~~~~~

### Calling the parameter of the nearest solution point

~~~~~
Standard_Real U = Projector.LowerDistanceParameter(); 
~~~~~

### Calling the minimum distance from the point to the curve

~~~~~
Standard_Real D = Projector.LowerDistance(); 
~~~~~

### Redefined operators

Some operators have been  redefined to find the closest solution. 

*Standard_Real()* returns  the minimum distance from the point to the curve. 

~~~~~
Standard_Real D = Geom2dAPI_ProjectPointOnCurve (P,C); 
~~~~~

*Standard_Integer()* returns the number of solutions. 

~~~~~
Standard_Integer N = 
Geom2dAPI_ProjectPointOnCurve (P,C); 
~~~~~

*gp_Pnt2d()* returns the  nearest solution point. 

~~~~~
gp_Pnt2d P1 = Geom2dAPI_ProjectPointOnCurve (P,C); 
~~~~~

Using these operators  makes coding easier when you only need the nearest point. Thus: 
~~~~~
Geom2dAPI_ProjectPointOnCurve Projector (P, C); 
gp_Pnt2d P1 = Projector.NearestPoint(); 
~~~~~
can be written more  concisely as: 
~~~~~
gp_Pnt2d P1 = Geom2dAPI_ProjectPointOnCurve (P,C); 
~~~~~
However, note that in  this second case no intermediate *Geom2dAPI_ProjectPointOnCurve* object is created, and thus it  is impossible to have access to other solution points. 

### Access to lower-level functionalities

If you want to use the  wider range of functionalities available from the *Extrema* package, a call to  the *Extrema()* method will return the algorithmic object for calculating  extrema. For example: 

~~~~~
Extrema_ExtPC2d& TheExtrema = Projector.Extrema(); 
~~~~~

@subsubsection occt_modalg_2_6_2 Projection of a 3D Point on a Curve

The class *GeomAPI_ProjectPointOnCurve* is  instantiated as in the following example: 

~~~~~
gp_Pnt P; 
Handle(Geom_BezierCurve) C = 
	new  Geom_BezierCurve(args); 
GeomAPI_ProjectPointOnCurve Projector (P, C); 
~~~~~

If you wish to restrict  the search for normals to the given domain [U1,U2], use the following  constructor: 

~~~~~
GeomAPI_ProjectPointOnCurve Projector (P, C, U1, U2); 
~~~~~
Having thus created the  *GeomAPI_ProjectPointOnCurve* object, you can now interrogate it. 

### Calling the number of solution points

~~~~~
Standard_Integer NumSolutions = Projector.NbPoints(); 
~~~~~

### Calling the location of a solution point

The solutions are  indexed in a range from 1 to *Projector.NbPoints()*. The point, which corresponds  to a given index, may be found: 
~~~~~
gp_Pnt Pn = Projector.Point(Index); 
~~~~~

### Calling the parameter of a solution point

For a given point  corresponding to a given index: 

~~~~~
Standard_Real U = Projector.Parameter(Index); 
~~~~~

This can also be  programmed as: 
~~~~~
Standard_Real U; 
Projector.Parameter(Index,U); 
~~~~~

### Calling the distance between the start and end point

The distance between the  initial point and a point, which corresponds to a given index, may be found: 
~~~~~
Standard_Real D = Projector.Distance(Index); 
~~~~~

### Calling the nearest solution point

This class offers a  method to return the closest solution point to the starting point. This  solution is accessed as follows: 
~~~~~
gp_Pnt P1 = Projector.NearestPoint(); 
~~~~~

### Calling the parameter of the nearest solution point

~~~~~
Standard_Real U = Projector.LowerDistanceParameter(); 
~~~~~

### Calling the minimum distance from the point to the curve

~~~~~
Standard_Real D =  Projector.LowerDistance(); 
~~~~~

### Redefined  operators 

Some operators have been  redefined to find the nearest solution. 

*Standard_Real()* returns  the minimum distance from the point to the curve. 

~~~~~
Standard_Real D = GeomAPI_ProjectPointOnCurve (P,C); 
~~~~~

*Standard_Integer()* returns  the number of solutions. 
~~~~~
Standard_Integer N =  GeomAPI_ProjectPointOnCurve (P,C); 
~~~~~

*gp_Pnt2d()* returns the  nearest solution point. 

~~~~~
gp_Pnt P1 = GeomAPI_ProjectPointOnCurve (P,C); 
~~~~~
Using these operators  makes coding easier when you only need the nearest point. In this way, 

~~~~~
GeomAPI_ProjectPointOnCurve Projector (P, C); 
gp_Pnt P1 = Projector.NearestPoint(); 
~~~~~

can be written more  concisely as: 
~~~~~
gp_Pnt P1 = GeomAPI_ProjectPointOnCurve (P,C); 
~~~~~
In the second case,  however, no intermediate *GeomAPI_ProjectPointOnCurve* object is created, and it  is impossible to access other solutions points. 

### Access to lower-level functionalities

If you want to use the  wider range of functionalities available from the *Extrema* package, a call to  the *Extrema()* method will return the algorithmic object for calculating the  extrema. For example: 

~~~~~
Extrema_ExtPC& TheExtrema = Projector.Extrema(); 
~~~~~

@subsection occt_modalg_2_6_3 Projection of a Point on a Surface

The class *GeomAPI_ProjectPointOnSurf*  allows calculation of all normals  projected from a point from *gp_Pnt* onto a geometric surface from *Geom_Surface*. 

@image html /03_user_guides/modeling_algos/images/modeling_algos_image021.png  "Projection of normals from a point to a surface"
@image latex /03_user_guides/modeling_algos/images/modeling_algos_image021.png  "Projection of normals from a point to a surface"

Note that the  surface does not have to be of *Geom_RectangularTrimmedSurface* type.  
The algorithm  will function with any class inheriting *Geom_Surface*.

*GeomAPI_ProjectPointOnSurf* is instantiated as in the following  example: 
~~~~~
gp_Pnt P; 
Handle (Geom_Surface) S = new Geom_BezierSurface(args); 
GeomAPI_ProjectPointOnSurf Proj (P, S); 
~~~~~

To restrict the search  for normals within the given rectangular domain [U1, U2, V1, V2], use the  constructor <i>GeomAPI_ProjectPointOnSurf Proj (P, S, U1, U2, V1, V2)</i>

The values of *U1, U2, V1*  and *V2* lie at or within their maximum and minimum limits, i.e.: 
~~~~~
Umin <=  U1 < U2 <= Umax 
Vmin <=  V1 < V2 <= Vmax 
~~~~~
Having thus created the  *GeomAPI_ProjectPointOnSurf* object, you can interrogate it. 

### Calling the number of solution points

~~~~~
Standard_Integer NumSolutions = Proj.NbPoints(); 
~~~~~

### Calling the location of a solution point

The solutions are  indexed in a range from 1 to *Proj.NbPoints()*. The point corresponding to the  given index may be found: 

~~~~~
gp_Pnt Pn = Proj.Point(Index); 
~~~~~

### Calling the parameters of a solution point

For a given point  corresponding to the given index: 

~~~~~
Standard_Real U,V; 
Proj.Parameters(Index, U, V); 
~~~~~

### Calling the distance between the start and end point

The distance between the  initial point and a point corresponding to the given index may be found: 
~~~~~
Standard_Real D = Projector.Distance(Index); 
~~~~~

### Calling the nearest solution point

This class offers a  method, which returns the closest solution point to the starting point. This  solution is accessed as follows: 
~~~~~
gp_Pnt P1 = Proj.NearestPoint(); 
~~~~~

### Calling the parameters of the nearest solution point

~~~~~
Standard_Real U,V; 
Proj.LowerDistanceParameters (U, V); 
~~~~~

### Calling the minimum distance from a point to the surface

~~~~~
Standard_Real D = Proj.LowerDistance(); 
~~~~~

### Redefined operators

Some operators have been  redefined to help you find the nearest solution. 

*Standard_Real()* returns  the minimum distance from the point to the surface. 

~~~~~
Standard_Real D = GeomAPI_ProjectPointOnSurf (P,S); 
~~~~~

*Standard_Integer()* returns  the number of solutions. 

~~~~~
Standard_Integer N = GeomAPI_ProjectPointOnSurf (P,S); 
~~~~~

*gp_Pnt2d()* returns the  nearest solution point. 

~~~~~
gp_Pnt P1 = GeomAPI_ProjectPointOnSurf (P,S); 
~~~~~

Using these operators  makes coding easier when you only need the nearest point. In this way, 

~~~~~
GeomAPI_ProjectPointOnSurface Proj (P, S); 
gp_Pnt P1 = Proj.NearestPoint(); 
~~~~~

can be written more concisely as: 

~~~~~
gp_Pnt P1 = GeomAPI_ProjectPointOnSurface (P,S); 
~~~~~

In the second case,  however, no intermediate *GeomAPI_ProjectPointOnSurf* object is created,  and it is impossible to access other solution points. 

### Access to lower-level functionalities

If you want to use the  wider range of functionalities available from the *Extrema* package, a call to  the *Extrema()* method will return the algorithmic object for calculating the  extrema as follows: 

~~~~~
Extrema_ExtPS& TheExtrema = Proj.Extrema(); 
~~~~~

@subsection occt_modalg_2_12_8 Switching from 2d and 3d Curves

The *To2d* and *To3d* methods are used to; 

  * build a 2d curve from a 3d  *Geom_Curve* lying on a *gp_Pln* plane
  * build a 3d curve from a  *Geom2d_Curve* and a *gp_Pln* plane.

These methods are called  as follows: 
~~~~~
Handle(Geom2d_Curve) C2d = GeomAPI::To2d(C3d, Pln); 
Handle(Geom_Curve) C3d = GeomAPI::To3d(C2d, Pln); 
~~~~~


Shape Healing {#user_guides__draw_commands_shape_healing}
=============

@tableofcontents

@section occt_draw_9_1 General commands 

@subsection occt_draw_9_1_1 bsplres
    
~~~~~
bsplres <result> <shape> <tol3d> <tol2d< <reqdegree> <reqnbsegments> <continuity3d> <continuity2d> <PriorDeg> <RationalConvert>
~~~~~

Performs approximations of a given shape (BSpline curves and surfaces or other surfaces) to BSpline with given required parameters. The specified continuity can be reduced if the approximation with a specified continuity was not done successfully. Results are put into the shape, which is given as a parameter result. For a more detailed description see the ShapeHealing User’s Guide (operator: **BSplineRestriction**). 

@subsection occt_draw_9_1_2 checkfclass2d
    
~~~~~
checkfclass2d <face> <ucoord> <vcoord>
~~~~~

Shows where a point which is given by coordinates is located in relation to a given face -- outbound, inside or at the bounds. 

~~~~~
checkfclass2d f 10.5 1.1 
== Point is OUT 
~~~~~

@subsection occt_draw_9_1_3 checkoverlapedges
    
~~~~~
checkoverlapedges <edge1> <edge2> [<toler> <domaindist>]
~~~~~

Checks the overlapping of two given edges. If the distance between two edges is less than the given value of tolerance then edges are overlapped. Parameter \<domaindist\> sets length of part of edges on which edges are overlapped. 

~~~~~
checkoverlapedges e1 e2 
~~~~~

@subsection occt_draw_9_1_4 comtol
    
~~~~~
comptol <shape> [nbpoints] [prefix]
~~~~~

Compares the real value of tolerance on curves with the value calculated by standard (using 23 points). The maximal value of deviation of 3d curve from pcurve at given simple points is taken as a real value (371 is by default). Command returns the maximal, minimal and average value of tolerance for all edges and difference between real values and set values. Edges with the maximal value of tolerance and relation will be saved if the ‘prefix’ parameter is given. 
 
~~~~~    
comptol h 871 t 

==> Edges tolerance computed by 871 points: 
==> MAX=8.0001130696523449e-008 AVG=6.349346868091096e-009 MIN=0 
==> Relation real tolerance / tolerance set in edge 
==> MAX=0.80001130696523448 AVG=0.06349345591805905 MIN=0 
==> Edge with max tolerance saved to t_edge_tol 
==> Concerned faces saved to shapes t_1, t_2 
~~~~~

@subsection occt_draw_9_1_5 convtorevol
    
~~~~~
convtorevol <result> <shape>
~~~~~

Converts all elementary surfaces of a given shape into surfaces of revolution. 
Results are put into the shape, which is given as the <i>\<result\></i> parameter. 

~~~~~
convtorevol r a 
~~~~~

@subsection occt_draw_9_1_6 directfaces
    
~~~~~
directfaces <result> <shape>
~~~~~

Converts indirect surfaces and returns the results into the shape, which is given as the result parameter. 

~~~~~
directfaces r a 
~~~~~

@subsection occt_draw_9_1_7 expshape
 
~~~~~
expshape <shape> <maxdegree> <maxseg>
~~~~~

Gives statistics for a given shape. This test command is working with Bezier and BSpline entities. 

~~~~~
expshape a 10 10 
==> Number of Rational Bspline curves 128 
==> Number of Rational Bspline pcurves 48 
~~~~~

@subsection occt_draw_9_1_8 fixsmall
    
~~~~~
fixsmall <result> <shape> [<toler>=1.]
~~~~~

Fixes small edges in given shape by merging adjacent edges with agiven tolerance. Results are put into the shape, which 
is given as the result parameter. 

~~~~~
fixsmall r a 0.1 
~~~~~

@subsection occt_draw_9_1_9 fixsmalledges
    
~~~~~
fixsmalledges <result> <shape> [<toler> <mode> <maxangle>]
~~~~~

Searches at least one small edge at a given shape. If such edges have been found, then small edges are merged with a 
given tolerance. If parameter <i>\<mode\></i> is equal to *Standard_True* (can be given any values, except 2), then  
small edges, which can not be merged, are removed, otherwise they are to be kept (*Standard_False* is used by default). 
Parameter <i>\<maxangle\></i> sets a maximum possible angle for merging two adjacent edges, by default no limit angle is 
applied (-1). Results are put into the shape, which is given as parameter result. 

~~~~~
fixsmalledges r a 0.1 1 
~~~~~

@subsection occt_draw_9_1_10 fixshape

    
~~~~~
fixshape <result> <shape> [<preci> [<maxpreci>]] [{switches}]
~~~~~

Performs fixes of all sub-shapes (such as *Solids*, *Shells*, *Faces*, *Wires* and *Edges*) of a given shape. Parameter 
<i>\<preci\></i> sets a basic precision value, <i>\<maxpreci\></i> sets the maximal allowed tolerance. Results are put
 into the shape, which is given as parameter result. <b>{switches}</b> allows to tune parameters of ShapeFix 

The following syntax is used: 
* <i>\<symbol\></i> may be
  * "-" to set parameter off, 
  * "+" to set on or  
  * "*" to set default 
* <i>\<parameter\></i> is identified by  letters: 
  * l -- FixLackingMode 
  * o -- FixOrientationMode 
  * h -- FixShiftedMode 
  * m -- FixMissingSeamMode 
  * d -- FixDegeneratedMode 
  * s -- FixSmallMode 
  * i -- FixSelfIntersectionMode 
  * n -- FixNotchedEdgesMode 
For enhanced message output, use switch '+?' 

~~~~~
fixshape r a 0.001 
~~~~~

@subsection occt_draw_9_1_11 fixwgaps
    
~~~~~
fixwgaps <result> <shape> [<toler>=0]
~~~~~

Fixes gaps between ends of curves of adjacent edges (both 3d and pcurves) in wires in a given shape with a given
 tolerance. Results are put into the shape, which is given as parameter result. 

~~~~~
fixwgaps r a 
~~~~~

@subsection occt_draw_9_1_12 offsetcurve, offset2dcurve
    
~~~~~
offsetcurve <result> <curve> <offset> <direction(as point)>
offset2dcurve <result> <curve> <offset>
~~~~~

**offsetcurve** works with the curve in 3d space, **offset2dcurve** in 2d space. 

Both commands are intended to create a new offset curve by copying the given curve to distance, given by parameter
 <i>\<offset\></i>. Parameter <i>\<direction\></i> defines direction of the offset curve. It is created as a point. For 
correct work of these commands the direction of normal of the offset curve must be perpendicular to the plane, the basis
 curve is located there. Results are put into the curve, which is given as parameter <i>\<result\></i>.  

~~~~~
point pp 10 10 10 
offsetcurve r c 20 pp 
~~~~~

@subsection occt_draw_9_1_13 projcurve
    
~~~~~
projcurve <edge>|<curve3d>|<curve3d first last>  <X> <Y> <Z>
~~~~~

**projcurve** returns the projection of a given point on a given curve. The curve may be defined by three ways: by
 giving the edge name, giving the 3D curve and by giving the unlimited curve and limiting it by pointing its start and
 finish values. 
 
~~~~~    
projcurve k_1 0 1 5 
==Edge k_1 Params from 0 to 1.3 
==Precision (BRepBuilderAPI) : 9.9999999999999995e-008  ==Projection : 0  1  5 
==Result : 0  1.1000000000000001  0 
==Param = -0.20000000000000001  Gap = 5.0009999000199947 
~~~~~

@subsection occt_draw_9_1_14 projface
    
~~~~~
projface <face> <X> <Y> [<Z>]
~~~~~

Returns the projection of a given point to a given face in 2d or 3d space. If two coordinates (2d space) are given then 
returns coordinates projection of this point in 3d space and vice versa. 

~~~~~
projface a_1 10.0 0.0 
==  Point UV  U = 10  V = 0 
==   =   proj  X = -116  Y = -45  Z = 0 
~~~~~

@subsection occt_draw_9_1_15 scaleshape

~~~~~
scaleshape <result> <shape> <scale>
~~~~~

Returns a new shape, which is the result of scaling of a given shape with a coefficient equal to the parameter
 <i>\<scale\></i>. Tolerance is calculated for the  new shape as well.

~~~~~
scaleshape r a_1 0.8 
~~~~~

@subsection occt_draw_9_1_16 settolerance

    
~~~~~
settolerance <shape> [<mode>=v-e-w-f-a] <val>(fix value) or
                   <tolmin> <tolmax>
~~~~~ 

Sets new values of tolerance for a given shape. If the second parameter <i>mode</i> is given, then the tolerance value 
is set only for these sub shapes. 

~~~~~
settolerance a 0.001 
~~~~~

@subsection occt_draw_9_1_17 splitface
    
~~~~~
splitface <result> <face> [u usplit1 usplit2...] [v vsplit1 vsplit2 ...]
~~~~~

Splits a given face in parametric space and puts the result into the given parameter <i>\<result\></i>. 
Returns the status of split face. 

~~~~~
# split face f by parameter u = 5 
splitface r f u 5 
==> Splitting by   U:   ,5 
==> Status:  DONE1 
~~~~~

@subsection occt_draw_9_1_18 statshape
    
~~~~~
statshape <shape> [particul]
~~~~~

Returns the number of sub-shapes, which compose the given shape. For example, the number of solids, number of faces etc. 
 It also returns the number of geometrical objects or sub-shapes with a specified type, example, number of free faces,
 number of C0 
surfaces. The last parameter becomes out of date. 

~~~~~
statshape a 
==> Count     Item 
==> -----     ---- 
==> 402     Edge (oriented) 
==> 402     Edge (Shared) 
==> 74      Face 
==> 74      Face (Free) 
==> 804     Vertex (Oriented) 
==> 402     Vertex (Shared) 
==> 78      Wire 
==> 4      Face with more than one wire 
==> 34     bspsur: BSplineSurface 
~~~~~

@subsection occt_draw_9_1_19 tolerance

Syntax:
~~~~~
tolerance <shape> [<mode>:D v e f c] [<tolmin> <tolmax>:real]
~~~~~

Returns tolerance (maximal, avg and minimal values)  of all given shapes and tolerance of their *Faces*, *Edges* and
 *Vertices*. If parameter <i>\<tolmin\></i> or <i>\<tolmax\></i> or both of them are given, then sub-shapes are 
returned as a result of analys of this shape, which satisfy the given tolerances. If a particular value of entity 
((**D**)all shapes  (**v**) *vertices* (**e**) *edges* (**f**) *faces* (**c**) *combined* (*faces*)) is given as the 
second parameter then only this group will be analyzed for tolerance. 


~~~~~
tolerance a 
==> Tolerance MAX=0.31512672416608001 AVG=0.14901359484722074 MIN=9.9999999999999995e-08 
==> FACE    : MAX=9.9999999999999995e-08 AVG=9.9999999999999995e-08 MIN=9.9999999999999995e-08 
==> EDGE    : MAX=0.31512672416608001 AVG=0.098691334511810405 MIN=9.9999999999999995e-08 
==> VERTEX  : MAX=0.31512672416608001 AVG=0.189076074499648 MIN=9.9999999999999995e-08 

tolerance a v 0.1 0.001 
==>  Analysing Vertices gives 6 Shapes between tol1=0.10000000000000001 and tol2=0.001 , named tol_1 to tol_6 
~~~~~


@section occt_draw_9_2 Conversion commands 

@subsection occt_draw_9_2_1 DT_ClosedSplit

    
~~~~~
DT_ClosedSplit <result> <shape>
~~~~~

Divides all closed faces in the shape (for example cone) and returns result of given shape into shape, which is given as
 parameter result. Number of faces in resulting shapes will be increased. 
Note: A closed face is a face with one or more seam. 


~~~~~
DT_ClosetSplit r a 
~~~~~

@subsection occt_draw_9_2_2 DT_ShapeConvert, DT_ShapeConvertRev
    
~~~~~
DT_ShapeConvert <result> <shape> <convert2d> <convert3d>
DT_ShapeConvertRev <result> <shape> <convert2d> <convert3d>
~~~~~
 
Both commands are intended for the conversion of 3D, 2D curves to Bezier curves and surfaces to Bezier based surfaces.
 Parameters convert2d and convert3d take on a value 0 or 1. If the given value is 1, then the conversion will be 
performed, otherwise it will not be performed. The results are put into the shape, which is given as parameter Result. 
Command *DT_ShapeConvertRev* differs from *DT_ShapeConvert* by converting all elementary surfaces into surfaces of
 revolution first. 

~~~~~
DT_ShapeConvert r a 1 1 
== Status: DONE1 
~~~~~

@subsection occt_draw_9_2_3 DT_ShapeDivide
    
~~~~~
DT_ShapeDivide <result> <shape> <tol>
~~~~~

Divides the shape with C1 criterion and returns the result of geometry conversion of a given shape into the shape, which 
is given as parameter result. This command illustrates how class *ShapeUpgrade_ShapeDivideContinuity* works. This class
 allows to convert geometry with a continuity less than the specified continuity to geometry with target continuity. If 
conversion is not possible then the geometrical object is split into several ones, which satisfy the given tolerance. It 
also returns the  status shape splitting: 
 * OK      : no splitting was done 
 * Done1 : Some edges were split 
 * Done2 : Surface was split 
 * Fail1    : Some errors occurred 


~~~~~
DT_ShapeDivide r a 0.001 
== Status: OK 
~~~~~

@subsection occt_draw_9_2_4 DT_SplitAngle
    
~~~~~
DT_SplitAngle <result> <shape> [MaxAngle=95]
~~~~~

Works with all revolved surfaces, like cylinders, surfaces of revolution, etc. This command divides given revolved 
surfaces into segments so that each resulting segment covers not more than the given *MaxAngle* degrees and puts the
 result of splitting into the shape, which is given as parameter result. Values of returned status are given above. 
This command illustrates how class *ShapeUpgrade_ShapeDivideAngle* works. 

~~~~~
DT_SplitAngle r a 
== Status: DONE2 
~~~~~

@subsection occt_draw_9_2_5 DT_SplitCurve
    
~~~~~
DT_SplitCurve <curve> <tol> <split(0|1)>
~~~~~

Divides the 3d curve with C1 criterion and returns the result of splitting of the given curve into a new curve. If the 
curve had been divided by segments, then each segment is put to an individual result.  This command can correct a given
 curve at a knot with the given tolerance, if it is impossible, then the given surface is split at that knot. If the 
last parameter is 1, then 5 knots are added at the given curve, and its surface is split by segments, but this will be 
performed not for all parametric spaces. 

~~~~~
DT_SplitCurve r c 
~~~~~

@subsection occt_draw_9_2_6 DT_SplitCurve2d
    
~~~~~
DT_SplitCurve2d Curve Tol Split(0/1) 
~~~~~

Works just as **DT_SplitCurve** (see above), only with 2d curve. 

~~~~~
DT_SplitCurve2d r c 
~~~~~

@subsection occt_draw_9_2_7 DT_SplitSurface
    
~~~~~
DT_SplitSurface <result> <Surface|GridSurf> <tol> <split(0|1)>
~~~~~

Divides surface with C1 criterion and returns the result of splitting of a given surface into surface, which is given as
 parameter result. If the surface has been divided into segments, then each segment is put to an individual result. 
This command can correct a given C0 surface at a knot with a given tolerance, if it is impossible, then the given surface
 is split at that knot. If the last parameter is 1, then 5 knots are added to the given surface, and its surface is
 split by segments, but this will be performed not for all parametric spaces. 
 
~~~~~

~~~~~
# split surface with name "su"
DT_SplitSurface res su 0.1 1 
==> single surf 
==> appel a SplitSurface::Init 
==> appel a SplitSurface::Build 
==> appel a SplitSurface::GlobalU/VKnots 
==> nb GlobalU;nb GlobalV=7 2 0 1 2 3 4 5 6.2831853072 0 1 
==> appel a Surfaces 
==> transfert resultat 
==> res1_1_1 res1_2_1 res1_3_1 res1_4_1 res1_5_1 res1_6_1 
~~~~~
~~~~~

@subsection occt_draw_9_2_8 DT_ToBspl

Syntax:
~~~~~
DT_ToBspl <result> <shape>
~~~~~

Converts a surface of linear extrusion, revolution and offset surfaces into BSpline surfaces. Returns the result into 
the shape, which is given as parameter result. 
 
~~~~~    
DT_ToBspl res sh 
== error = 5.20375663162094e-08   spans = 10 
==  Surface is aproximated with continuity 2 
~~~~~


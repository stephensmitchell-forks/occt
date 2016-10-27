Math Primitives and Algorithms {#user_guides__foundations_math_support}
==============================

@tableofcontents

Math primitives and algorithms available in Open CASCADE  Technology include: 
  * Vectors and matrices
  * Geometric primitives
  * Math algorithms
  
@section occt_occt_fcug_4_2 Vectors and Matrices

The Vectors and Matrices component provides a C++  implementation of the fundamental types *Vector* and *Matrix*, which are regularly used to define more complex data structures. 

The <i> Vector</i> and <i> Matrix </i> classes provide commonly used mathematical algorithms which
include:

  * Basic calculations involving vectors and matrices; 
  * Computation of eigenvalues and eigenvectors of a square matrix; 
  * Solvers for a set of linear algebraic equations; 
  * Algorithms to find the roots of a set of non-linear equations; 
  * Algorithms to find the minimum function of one or more independent variables. 

These classes also provide a data structure to represent any expression,
relation, or function used in mathematics, including the assignment of variables.

Vectors and matrices have arbitrary ranges which must be defined at declaration time
and cannot be changed after declaration.

~~~~~
math_Vector  v(1, 3); 
//  a vector of dimension 3 with range (1..3) 
math_Matrix  m(0, 2, 0, 2); 
//  a matrix of dimension 3x3 with range (0..2, 0..2) 
math_Vector  v(N1, N2); 
//  a vector of dimension N2-N1+1 with range (N1..N2)
~~~~~

Vector and Matrix objects use value semantics. In other  words, they cannot be shared and are copied through assignment. 

~~~~~
math_Vector  v1(1, 3), v2(0, 2); 
v2  = v1; 
//  v1 is copied into v2. a modification of v1 does not affect  v2 
~~~~~

Vector and Matrix values may be initialized and obtained  using indexes which must lie within the range definition of the vector or the  matrix. 

~~~~~
math_Vector v(1, 3);
math_Matrix m(1, 3, 1, 3);
Standard_Real value;

v(2) = 1.0;
value = v(1);
m(1, 3) = 1.0;
value = m(2, 2);
~~~~~

Some operations on Vector and Matrix objects may not be  legal. In this case an exception is raised. Two standard exceptions are used: 
  * *Standard_DimensionError* exception is raised when two matrices or  vectors involved in an operation are of incompatible dimensions.
  * *Standard_RangeError* exception is raised if an access outside the  range definition of a vector or of a matrix is attempted.
  
~~~~~~
math_Vector v1(1, 3), v2(1, 2), v3(0, 2);
v1 = v2;
// error: Standard_DimensionError is raised

v1 = v3;
// OK: ranges are not equal but dimensions are
// compatible

v1(0) = 2.0;
// error: Standard_RangeError is raised
~~~~~~

@section occt_occt_fcug_4_3 Primitive Geometric Types

Open CASCADE Technology primitive geometric types are a  STEP-compliant implementation of basic geometric and algebraic entities.  
They provide: 
  * Descriptions of primitive geometric shapes, such as:
	  * Points; 
	  * Vectors; 
	  * Lines; 
	  * Circles and conics; 
	  * Planes and elementary surfaces;
  * Positioning of these shapes in space or in a plane by means of an axis or a coordinate system;
  * Definition and application of geometric transformations to these shapes:
	  * Translations; 
	  * Rotations; 
	  * Symmetries; 
	  * Scaling transformations; 
	  * Composed transformations;
  * Tools (coordinates and matrices) for algebraic computation.
  
All these functions are provided by geometric processor package <i> gp</i>. Its classes for 2d and 3d objects are handled by value rather than by reference. When this sort of object is copied, it is copied entirely. Changes in one instance will not be  reflected in another. 

The *gp* package defines the basic geometric entities used for algebraic calculation and basic analytical geometry  in 2d & 3d space. It also provides basic transformations such as identity,  rotation, translation, mirroring, scale transformations, combinations of  transformations, etc. Entities are handled by value.  

Please, note that <i> gp</i> curves and surfaces are analytic: there is no parameterization and no orientation on <i>gp</i> entities, i.e. these entities do not provide functions which work with these properties. 

If you need, you may use more evolved data structures provided by <i> Geom</i> (in 3D space) and <i> Geom2d</i> (in the plane). However, the definition of <i> gp</i> entities is identical to the one of equivalent <i> Geom</i> and <i> Geom2d</i> entities, and they are located in the plane or in space with the same kind of positioning systems. They implicitly contain the orientation, which they express on the <i> Geom </i> and <i> Geom2d </i> entities, and they induce the definition of their parameterization.

Therefore, it is easy to give an implicit parameterization to <i> gp</i> curves and surfaces, which is the parametrization of the equivalent <i> Geom</i> or <i> Geom2d</i> entity. This property is particularly useful when computing projections or intersections, or for operations involving complex algorithms where it is particularly important to manipulate the simplest data structures, i.e. those of <i> gp</i>. Thus,  <i> ElCLib</i> and <i> ElSLib</i> packages provide functions to compute:
  * the point of parameter u on a 2D or 3D gp curve,
  * the point of parameter (u,v) on a gp elementary surface, and
  * any derivative vector at this point.

@note <i> gp</i> entities cannot be shared when they are inside more complex data structures. 

@section occt_occt_fcug_4_4 Collections of Primitive Geometric Types

Before creating a geometric object, you must decide whether  you are in a 2d or in a 3d context and how you want to handle the object. 
If you do not need a single instance of a geometric  primitive but a set of them then the package which deals with collections of  this sort of object, *TColgp*, will provide the necessary functionality. 
In particular, this package provides standard and frequently  used instantiations of generic classes with geometric objects, i.e. *XY*, *XYZ*,  *Pnt*, *Pnt2d*, *Vec*, *Vec2d*, *Lin*, *Lin2d*, *Circ*,  *Circ2d.* 

@section occt_occt_fcug_4_5 Basic Geometric Libraries
There are various library packages available which offer a  range of basic computations on curves and surfaces. 
If you are dealing with objects created from the *gp* package,  the useful algorithms are in the elementary curves and surfaces libraries -- the  *ElCLib* and *ElSLib* packages.
* *EICLib* provides methods for analytic curves. This is a library of simple  computations on curves from the *gp* package (Lines, Circles and Conics).  It is possible to compute points with a given parameter or to compute the parameter for a point. 
* *EISLib* provides methods for analytic surfaces. This is a library of simple  computations on surfaces from the package *gp* (Planes, Cylinders,  Spheres, Cones, Tori). It is possible to compute points with a given pair of parameters or to compute the parameter for a point. There is a library for calculating normals  on curves and surfaces. 

Additionally, *Bnd* package provides a set of classes and tools to operate  with bounding boxes of geometric objects in 2d and 3d space. 

@section occt_occt_fcug_4_6 Common Math Algorithms
The common math algorithms library provides a C++  implementation of the most frequently used mathematical algorithms. These  include: 
  * Algorithms to solve a set of linear algebraic equations,
  * Algorithms to find the minimum of a function of one or more  independent variables,
  * Algorithms to find roots of one, or of a set, of non-linear  equations,
  * An algorithm to find the eigenvalues and eigenvectors of a square  matrix.
  
All mathematical algorithms are implemented using the same  principles. They contain: 
A constructor performing all, or most of, the calculation,  given the appropriate arguments. All relevant information is stored inside the  resulting object, so that all subsequent calculations or interrogations will be  solved in the most efficient way. 

A function *IsDone* returning the boolean true if the  calculation was successful. 
A set of functions, specific to each algorithm, enabling all  the various results to be obtained. 
Calling these functions is legal only if the function *IsDone*  answers **true**, otherwise the exception *StdFail_NotDone* is raised. 

The example below demonstrates the use of the Gauss class,  which implements the Gauss solution for a set of linear equations.The following  definition is an extract from the header file of the class *math_Gauss*: 

~~~~~~
class Gauss {
public:
  Gauss (const math_Matrix& A);
  Standard_Boolean IsDone() const;
  void Solve (const math_Vector& B,
  math_Vector& X) const;
};
~~~~~~

Now the main program uses the Gauss class to solve the  equations a*x1=b1 and a*x2=b2: 

~~~~~
#include <math_Vector.hxx> 
#include <math_Matrix.hxx>
main ()
{
  math_Vector a(1, 3, 1, 3);
  math_Vector b1(1, 3), b2(1, 3);
  math_Vector x1(1, 3), x2(1, 3);
  // a, b1 and b2 are set here to the appropriate values
  math_Gauss sol(a);              // computation of the
  // LU decomposition of A
  if(sol.IsDone()) {              // is it OK ?
    sol.Solve(b1, x1);      // yes, so compute x1
    sol.Solve(b2, x2);      // then x2
    ...
  } 
  else {                    // it is not OK:
    // fix up
    sol.Solve(b1, x1);            // error:
    // StdFail_NotDone is raised
  }
}
~~~~~

The next example demonstrates the use of the *BissecNewton* class, which implements a combination of the Newton and Bissection algorithms  to find the root of a function known to lie between two bounds. The definition is an extract from the header file of the class *math_BissecNewton*: 

~~~~~
class  BissecNewton { 
	public: 
		BissecNewton  (math_FunctionWithDerivative& f, 
			const Standard_Real bound1, 
			const Standard_Real bound2, 
			const Standard_Real tolx); 
	Standard_Boolean IsDone() const; 
	Standard_Real Root(); 
}; 
~~~~~

The abstract class  *math_FunctionWithDerivative* describes the services which have to be implemented  for the function f which is to be used by a *BissecNewton* algorithm. The  following definition corresponds to the header file of the abstract class  *math_FunctionWithDerivative*: 

~~~~~
class  math_FunctionWithDerivative { 
	public: 
		virtual Standard_Boolean Value 
			(const Standard_Real x, Standard_Real&  f) = 0; 
		virtual Standard_Boolean Derivative 
			(const Standard_Real x, Standard_Real&  d) = 0; 
		virtual Standard_Boolean Values 
			(const Standard_Real x, 
			Standard_Real& f, 
			Standard_Real& d) = 0; 
}; 
~~~~~

Now the test sample uses the *BissecNewton* class to find the  root of the equation *f(x)=x**2-4* in the interval [1.5, 2.5]: the function to  solve is implemented in the class *myFunction* which inherits from the class *math_FunctionWithDerivative*, then the main program finds the required root. 

~~~~~
#include <math_BissecNewton.hxx> 
#include <math_FunctionWithDerivative.hxx>
class myFunction : public math_FunctionWithDerivative 
{
  Standard_Real coefa, coefb, coefc;

  public:
    myFunction (const  Standard_Real a, const Standard_Real b,
                const Standard_Real c) :
      coefa(a), coefb(b), coefc(c)
    {}

    virtual  Standard_Boolean Value (const Standard_Real x, 
                                    Standard_Real& f)
    {
      f = coefa * x * x + coefb * x + coefc;
    }

    virtual  Standard_Boolean Derivative (const Standard_Real x,
                                         Standard_Real&  d)
    {
      d = coefa * x * 2.0 + coefb;
    }

    virtual  Standard_Boolean Values (const Standard_Real x,
                                     Standard_Real& f, Standard_Real& d)
    {
      f = coefa * x * x + coefb * x + coefc;
      d = coefa * x *  2.0 + coefb;
    }
};

main()
{
  myFunction f(1.0, 0.0, 4.0);
  math_BissecNewton sol(F, 1.5, 2.5, 0.000001);
  if(Sol.IsDone()) { // is it OK ?
    Standard_Real x = sol.Root(); // yes.
  }
  else { // no
  }
~~~~~

@section occt_occt_fcug_4_7 Precision

On the OCCT platform, each object stored in the database  should carry its own precision value. This is important when dealing with  systems where objects are imported from other systems as well as with various  associated precision values. 

The *Precision* package addresses the daily problem of  the geometric algorithm developer: what precision setting to use to compare two  numbers. Real number equivalence is clearly a poor choice. The difference  between the numbers should be compared to a given precision setting. 

Do not write _if  (X1 == X2),_  instead write _if  (Abs(X1-X2) < Precision)._ 

Also, to order real numbers, keep in mind that _if  (X1 < X2 - Precision)_ is incorrect. 
_if  (X2 - X1 > Precision)_ is far better when *X1* and *X2* are high numbers. 

This package proposes a set of methods providing precision  settings for the most commonly encountered situations. 

In Open CASCADE Technology, precision is usually not  implicit; low-level geometric algorithms accept precision settings as  arguments. Usually these should not refer directly to this package. 

High-level modeling algorithms have to provide a precision  setting to the low level geometric algorithms they call. One way is to use the  settings provided by this package. The high-level modeling algorithms can also  have their own strategy for managing precision. As an example the Topology Data  Structure stores precision values which are later used by algorithms. When a  new topology is created, it takes the stored value. 
Different precision settings offered by this package cover  the most common needs of geometric algorithms such as *Intersection* and *Approximation*. 
The choice of a precision value depends both on the algorithm  and on the geometric space. The geometric space may be either: 
  * a real space, 3d or 2d where the lengths are measured  in meters, micron, inches, etc.
  * a parametric space, 1d on a curve or 2d on a surface  where numbers have no dimension.
The choice of precision value for parametric space depends  not only on the accuracy of the machine, but also on the dimensions of the  curve or the surface. 
This is because it is desirable to link parametric precision  and real precision. If you are on a curve defined by the equation *P(t)*,  you would want to have equivalence between the following: 

~~~~~
Abs(t1-t2)  < ParametricPrecision 
Distance  (P(t1),P(t2)) < RealPrecision. 
~~~~~

@subsection occt_occt_fcug_4_7_1 Precision Package
The *Precision* package offers a number of package methods and  default precisions for use in dealing with angles, distances, intersections,  approximations, and parametric space. 
It provides values to use in comparisons to test for real  number equalities. 
  * Angular precision compares angles.
  * Confusion precision compares distances.
  * Intersection precision is used by intersection algorithms.
  * Approximation precision is used by approximation algorithms.
  * Parametric precision gets a parametric space precision from a 3D  precision.
  * *Infinite* returns a high number that can be considered to  be infinite. Use <i>-Infinite</i> for a high negative number. 
  
@subsection occt_occt_fcug_4_7_2 Standard Precision Values
This package provides a set of real space precision values  for algorithms. The real space precisions are designed for precision to *0.1*  nanometers. The only unit available is the millimeter. 
The parametric precisions are derived from the real  precisions by the *Parametric* function. This applies a scaling factor which  is the length of a tangent to the curve or the surface. You, the user, provide  this length. There is a default value for a curve with <i>[0,1]</i> parameter  space and a length less than 100 meters. 
The geometric packages provide Parametric precisions for the  different types of curves. 
The *Precision* package provides methods to test whether a  real number can be considered to be infinite. 

### Precision::Angular

This method is used to compare two angles. Its current value is *Epsilon(2 *  PI)* i.e. the smallest number *x* such that *2*PI + x* is  different of *2\*PI*. 

It can be used to check confusion  of two angles as follows:
_Abs(Angle1  - Angle2) < Precision::Angular()_ 

It is also possible to check parallelism  of two vectors (_Vec_ from _gp_) as follows _V1.IsParallel(V2,Precision::Angular())_ 

Note that *Precision::Angular()* can be used on both  dot and cross products because for small angles the *Sine* and the *Angle* are equivalent. So to test if two directions of type *gp_Dir* are  perpendicular, it is legal to use the following code: 
_Abs(D1 * D2) < Precision::Angular()_ 

### Precision::Confusion

This method is used to test 3D distances. The current value is *1.e-7*, in other words, 1/10 micron if the unit used is the millimeter. 

It can be used to check confusion  of two points (_Pnt_ from _gp_) as follows: 
_P1.IsEqual(P2,Precision::Confusion())_ 

It is also possible to find a  vector of null length (_Vec_ from _gp_) :
_V.Magnitude() <  Precision::Confusion()_ 

### Precision::Intersection

This is reasonable precision to pass to an Intersection process as  a limit of refinement of Intersection Points. *Intersection* is high  enough for the process to converge quickly. *Intersection* is lower than *Confusion* so that you still get a point on the intersected geometries. The current  value is *Confusion() / 100*. 

### Precision::Approximation

This is a reasonable precision to pass to an approximation process  as a limit of refinement of fitting. The approximation is greater than the other  precisions because it is designed to be used when the time is at a premium. It has  been provided as a reasonable compromise by the designers of the Approximation  algorithm. The current value is *Confusion() * 10*. 
Note that Approximation is greater than Confusion, so care  must be taken when using Confusion in an approximation process. 




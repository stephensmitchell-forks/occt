Foundation Classes  {#user_guides__foundation_classes}
==================

This manual explains how to use OCCT **Foundation Classes**. <i>Foundation Classes</i> provide a variety of 
general-purpose  services such as automated dynamic memory management (manipulation of objects  by handle), 
collections, exception handling, genericity by down-casting and  plug-in creation. 

<i>Foundation Classes</i> include the following: 

* @subpage user_guides__foundations_basics chapter providing:
 
  * fundamental types such as Boolean, Character, Integer or Real,
  * safe handling of dynamically created objects, ensuring automatic deletion of unreferenced objects (see
 *Standard_Transient* class),
  * configurable optimized memory manager increasing the performance of applications that intensively use dynamically
 created objects,
  * extended run-time type information (RTTI) mechanism facilitating the creation of complex programs,
  * management of exceptions,
  * encapsulation of C++ streams.

The listed fetures are mainly implemented in *Standard* and *MMgt* packages. 

* @subpage user_guides__foundations_types chapter describing:

    * <b>Strings</b>. Strings are classes that handle dynamically sized sequences  of characters based 
on both ASCII (normal 8-bit character type) and Unicode (16-bit character type). Strings may also be manipulated by
 handles, and consequently  be shared. Strings are implemented in the *TCollection* package. <br><br>

    * <b>Collections</b>. Collections are the classes that handle dynamically sized  aggregates of data.
 Collection classes are *generic*, that is, they define  a structure and algorithms allowing to hold a variety of 
objects which do not  necessarily inherit from a unique root class (similarly to C++ templates). When  you need to use a
collection of a given type of object, you must *instantiate* it for this specific type of element. Once this declaration
 is compiled, all functions available on the generic collection are available on your *instantiated  class*. Collections 
include a wide range of generic classes such as  run-time sized arrays, lists, stacks, queues, sets and hash maps. 
Collections are implemented in the *TCollection* and *NCollection* packages. <br><br>
    * <b>Collections of Standard Objects</b>. The *TColStd* package provides frequently used 
instantiations of generic classes from the *TCollection* package with objects from the *Standard* package or
 strings from the *TCollection* package. <br><br>

* @subpage user_guides__foundations_math_support chapter describing:

    * <b>Vectors and Matrices</b>. These classes provide commonly used mathematical algorithms 
and basic calculations (addition, multiplication, transposition, inversion,  etc.) involving vectors and matrices.<br><br>
    * <b>Primitive Geometric Types</b>. Open CASCADE Technology primitive geometric types are a 
STEP-compliant implementation of basic geometric and algebraic entities:
      * Descriptions of elementary geometric shapes:
      * Points,
      * Vectors,
      * Lines,
      * Circles and conics,
      * Planes and elementary surfaces,
      * Positioning of these shapes in space or in a plane by means of an  axis or a coordinate system,
      * Definition and application of geometric transformations to these  shapes:
      * Translations
      * Rotations
      * Symmetries
      * Scaling transformations
      * Composed transformations
      * Tools (coordinates and matrices) for algebraic computation.<br><br>  
    * <b>Common Math Algorithms</b>. Open CASCADE Technology common math algorithms provide a C++ 
implementation of the most frequently used mathematical algorithms:
      * Algorithms to solve a set of linear algebraic equations,
      * Algorithms to find the minimum of a function of one or more  independent variables,
      * Algorithms to find roots of one, or of a set, of non-linear  equations,
      * Algorithms to find the eigen-values and eigen-vectors of a square  matrix.<br><br>
    * <b>Exceptions</b>. A hierarchy of commonly used exception classes is provided, all based on class 
Failure, the  root of exceptions. Exceptions describe exceptional situations, which can arise during the execution of a
 function. With the raising of an exception, the  normal course of program execution is abandoned. The execution of
 actions in  response to this situation is called the treatment of the exception. <br><br>
    * <b>Quantities</b>. These are various classes supporting date and time  information and fundamental
 types representing most physical quantities such as  length, area, volume, mass, density, weight, temperature,
 pressure etc. <br><br>
    * <b>Application services</b>. Foundation Classes also include implementation of several low-level
 services that facilitate the creation of customizable and user-friendly applications with Open CASCADE Technology: 
      * Unit conversion tools, providing a uniform mechanism for dealing  with quantities and associated physical units:
     check unit compatibility,  perform conversions of values between different units and so on (see package  *UnitsAPI*);
      * Basic interpreter of expressions that facilitates the creation of  customized scripting tools, generic definition of
     expressions and so on (see  package *ExprIntrp*);
      * Tools for dealing with configuration resource files (see package  *Resource*) and customizable message files (see
     package *Message*), making it easy  to provide a multi-language support in applications;
      * Progress indication and user break interfaces, giving a  possibility even for low-level algorithms to communicate 
    with the user in a  universal and convenient way.
  
For  advanced information on foundation classes and their applications, 
see our <a href="http://www.opencascade.com/content/tutorial-learning">E-learning & Training</a> offerings.

OCCT Basics {#user_guides__foundations_basics}
===========

@tableofcontents

This chapter deals with basic services such as library organization, persistence, data types, memory management, 
programming with handles, exception handling, genericity by downcasting and plug-in creation. 

The corresponding classes are mainly implemented in *Standard* and  *MMgt* packages.

@section general_organization Library Organization

##Modules and Toolkits

The whole OCCT library is organized in a set of modules. The  first module, providing most basic services and used by 
all other modules, is  called Foundation Classes and described by this manual. 

Every module consists primarily of one or several toolkits  (though it can also contain executables, resource units
 etc.). Physically a toolkit  is represented by a shared library (e.g. .so or .dll). The toolkit is built  from one or
 several packages. 

##Packages
A **package** groups together a number of classes which  have semantic links. For example, a geometry package would
 contain Point, Line,  and Circle classes. A package can also contain enumerations, exceptions and  package methods 
(functions). In practice, a class name is prefixed with the  name of its package e.g.  
*Geom_Circle*. 
Data types described in a package may include one or  more of the following data types: 
  * Enumerations
  * Object classes
  * Exceptions
  * Pointers to other object classes
Inside a package, two data types cannot bear the same  name. 

@image html /user_guides/foundation_classes/images/foundation_classes_image003.png  "Contents of a package"
@image latex /user_guides/foundation_classes/images/foundation_classes_image003.png  "Contents of a package"

**Methods** are either **functions** or **procedures**.  Functions return an object, whereas procedures only communicate
 by passing arguments.  In both cases, when the transmitted object is an instance manipulated by a  handle, its 
identifier is passed. There are three categories of methods: 
* **Object  constructor** Creates an instance of the described class. A class  will have one or more object constructors 
with various different arguments or none. 
* **Instance method** Operates on the  instance which owns it. 
* **Class  method** Does not work on individual  instances, only on the class itself. 

##Classes
The fundamental software component in object-oriented software  development is the class. A class is the implementation 
of a **data type**.  It defines its **behavior** (the services offered by its functions) and its **representation** (the 
data structure of the class -- the fields, which store its data). 

Classes fall into three categories: 
* Ordinary classes.
* Abstract classes. An **abstract class** cannot be instantiated. The purpose  of having such classes is to have a given
 behavior shared by a hierarchy of  classes and dependent on the implementation of the descendants. This is a way  of
 guaranteeing a certain base of inherited behavior common to all the classes  based on a particular deferred class. 
* Template classes. A **template class** offers a set of functional behaviors  to manipulate other data types.
 Instantiation of a template class requires that  a data type is given for its argument(s).

##Inheritance
The purpose of inheritance is to reduce the development  workload. The inheritance mechanism allows a new class to be 
declared already  containing the characteristics of an existing class. This new class can then be  rapidly specialized
 for the task in hand. This avoids the necessity of  developing each component “from scratch”. 
For example, having already developed a class *BankAccount* you  could quickly specialize new classes: *SavingsAccount,
 LongTermDepositAccount,  MoneyMarketAccount, RevolvingCreditAccount*, etc.... 

The corollary of this is that when two or more classes  inherit from a parent (or ancestor) class, all these classes 
guarantee as a  minimum the behavior of their parent (or ancestor). For example, if the parent  class BankAccount 
contains the method Print which tells it to print itself out,  then all its descendant classes guarantee to offer the 
same service. 

One way of ensuring the use of inheritance is to declare  classes at the top of a hierarchy as being **abstract**. In 
such classes,  the methods are not implemented. This forces the user to create a new class  which redefines the methods.
 This is a way of guaranteeing a certain minimum of  behavior among descendant classes. 

@section general_data_types Data Types

An object-oriented language structures a system around data types rather than around the actions carried out on this 
data. In this context,  an **object** is an **instance** of a data type and its definition  determines how it can be
 used. Each data type is implemented by one or more  classes, which make up the basic elements of the system. 

The data types in Open CASCADE Technology fall into two  categories: 
  * Data types manipulated by handle (or reference)
  * Data types manipulated by value
  
@image html /user_guides/foundation_classes/images/foundation_classes_image004.png  "Manipulation of data types"
@image latex /user_guides/foundation_classes/images/foundation_classes_image004.png  "Manipulation of data types"
  
A data type is implemented as a class. The class not only  defines its data representation and the methods available on 
instances, but it  also suggests how the instance will be manipulated. 
  * A variable of a type manipulated by value contains the instance  itself.
  * A variable of a type manipulated by handle contains a reference  to the instance.
The first examples of types manipulated by values are the  predefined **primitive types**: *Boolean, Character, Integer,
 Real*, etc. 

A variable of a type manipulated by handle which is not  attached to an object is said to be **null**. To reference an
 object, we  instantiate the class with one of its constructors. For example, in C++: 

~~~~~
Handle(myClass)  m = new myClass; 
~~~~~

In Open CASCADE Technology, the Handles are specific classes that are used to safely manipulate objects  allocated in 
the dynamic memory by reference, providing reference counting  mechanism and automatic destruction of the object when 
it is not referenced. 

@subsection general_data_types_valued Types Manipulated by Value
There are three categories of types which are manipulated by  value: 
  * Primitive types
  * Enumerated types
  * Types defined by classes not inheriting from *Standard_Transient*, whether directly or not.
Types which are manipulated by value behave in a more direct  fashion than those manipulated by handle and thus can be 
expected to perform  operations faster, but they cannot be stored independently in a file. 

@image html /user_guides/foundation_classes/images/foundation_classes_image005.png "Manipulation of a data type by value"
@image latex /user_guides/foundation_classes/images/foundation_classes_image005.png  "Manipulation of a data type by value"

@subsection general_data_types_referenced Types Manipulated by Reference (Handle)

These are types defined by classes inheriting from the *Transient* class.
  
@image html /user_guides/foundation_classes/images/foundation_classes_image006.png "Manipulation of a data type by reference"
@image latex /user_guides/foundation_classes/images/foundation_classes_image006.png "Manipulation of a data type by reference"
  
###When is it necessary to use a handle?

When you design an object, it can be difficult to choose how to manipulate that
object: by value or by handle. The following ideas can help you to make up your mind: 

* If your object may have a long lifetime within the application and you want to make multiple 
references to it, it would be preferable to manipulate this object with a handle. The memory for the 
object will be allocated on the heap. The handle which points to that memory is a light object which 
can be rapidly passed in argument. This avoids the penalty of copying a large object. 
* If your object will have a limited lifetime, for example, used within a single algorithm, it would 
be preferable to manipulate this object by value, non-regarding its size, because this object is 
allocated on the stack and the allocation and de-allocation of memory is extremely rapid, which 
avoids the implicit calls to *new* and *delete* occasioned by allocation on the heap.
* Finally, if an object will be created only once during, but will exist throughout the lifetime of 
the application, the best choice may be a class manipulated by handle or a value declared as a 
global variable. 

@subsection general_data_types_handles Programming with Handles

A handle is OCCT implementation of a smart pointer.
Several handles  can reference the same object.
Also, a single handle may reference several  objects, but only one at a time.
To have access to the object it refers to, the  handle must be de-referenced just as with a C++ pointer. 

Class *Standard_Transient* is a root of a big hierarchy of OCCT  classes that are said to be operable by handles. 
It provides a reference  counter field, inherited by all its descendant classes, that is used by  associated *Handle()* 
classes to track a number of handles pointing to this  instance of the object. 

Objects of classes derived (directly or indirectly) from *Transient*, are normally allocated in dynamic memory using 
operator **new**, and manipulated by handle.
Handle is defined as template class *opencascade::handle<>*.
Open CASCADE Technology  provides preprocessor macro *Handle()* that is historically used throughout OCCT code to name
 a handle:
~~~~~{.cpp}
Handle(Geom_Line) aLine; // "Handle(Geom_Line)" is expanded to "opencascade::handleL<Geom_Line>"
~~~~~

In addition, for standard OCCT classes additional *typedef* is defined for a handle, as the name of a class prefixed 
by *Handle_*.
For instance, the above example can be also coded as:
~~~~~{.cpp}
Handle_Geom_Line aLine; // "Handle_Geom_Line" is typedef to "opencascade::handleL<Geom_Line>"
~~~~~

###Using a Handle

A handle is characterized by the object it references. 

Before performing any operation on a transient object, you  must declare the handle. For example, if Point and Line are
 two transient classes  from the Geom package, you  would write: 
~~~~~
Handle(Geom_Point)  p1, p2; 
~~~~~
Declaring a handle creates a null handle that does not refer  to any object. The handle may be checked to be null by its
 method *IsNull()*. To  nullify a handle, use method *Nullify()*. 

To initialize a handle, either a new object should be  created or the value of another handle can be assigned to it, on 
condition that  their types are compatible. 

@note Handles should only be used for object sharing. For all local operations, it is advisable to use classes 
manipulated by values. 

@subsubsection handles_management Type Management

Open CASCADE Technology provides a means to describe the hierarchy  of data types in a generic way, with a possibility 
to check the exact type of  the given object at run-time (similarly to C++ RTTI). 

To enable this feature, a class declaration should include the declaration of OCCT RTTI.
Header *Standard_Type.hxx* provides two variants of preprocessor macros facilitating this:

* Inline variant, which declares and defines RTTI methods by a single line of code:
~~~~~{.cpp}
#include <Geom_Surface.hxx>
class Appli_ExtSurface : public Geom_Surface
{
. . .
public:
  DEFINE_STANDARD_RTTIEXT(Appli_ExtSurface,Geom_Surface)
};
~~~~~

* Out-of line variant, which uses one macro in the declaration (normally in the header file), and another in the 
implementation (in C++ source):

  In *Appli_ExtSurface.hxx* file:
~~~~~{.cpp}
#include <Geom_Surface.hxx>
class Appli_ExtSurface : public Geom_Surface
{
. . .
public:
  DEFINE_STANDARD_RTTIEXT(Appli_ExtSurface,Geom_Surface)
};
~~~~~

   In *Appli_ExtSurface.cxx* file:
~~~~~{.cpp}
#include <Appli_ExtSurface.hxx>
IMPLEMENT_STANDARD_RTTIEXT(Appli_ExtSurface,Geom_Surface)
~~~~~

These macros define method *DynamicType()* that returns a type descriptor - handle to singleton instance of the class 
*Standard_Type* describing the class.
The type descriptor stores the name of the class and the descriptor of its parent class.

Note that while inline version is easier to use, for widely used classes this method may lead to bloating of binary code
 of dependent libraries, due to multiple instantiations of inline method.

To get the type descriptor for a given class type, use macro *STANDARD_TYPE()* with the name of the class as argument.

Example of usage:
~~~~~{.cpp}
if (aCurve->IsKind(STANDARD_TYPE(Geom_Line))) // equivalent to "if (dynamic_cast<Geom_Line>(aCurve.get()) != 0)"
{
...
}
~~~~~

#### Type Conformity

The type used in the declaration of a handle is the static  type of the object, the type seen by the compiler.
 A handle can reference an  object instantiated from a subclass of its static type. Thus, the dynamic type  of an object
 (also called the actual type of an object) can be a descendant of  the type which appears in the handle declaration 
through which it is  manipulated. 

Consider the class *CartesianPoint*, a  sub-class of *Point*; the rule of type conformity can be illustrated as  follows: 

~~~~~
Handle (Geom_Point) p1;
Handle (Geom_CartesianPoint) p2;
p2 = new Geom_CartesianPoint;
p1 = p2;  // OK,  the types are compatible
~~~~~


The compiler sees p1 as a handle to *Point* though the  actual object referenced by *p1* is of the *CartesianPoint* type. 

#### Explicit Type Conversion

According to the rule of type conformity, it is always  possible to go up the class hierarchy through successive 
assignments of  handles. On the other hand, assignment does not authorize you to go down the  hierarchy. Consequently, 
an explicit type conversion of handles is required. 

A handle can be converted explicitly into one of its  sub-types if the actual type of the referenced object is a 
descendant of the  object used to cast the handle. If this is not the case, the handle is  nullified (explicit type 
conversion is sometimes called a “safe cast”).  Consider the example below. 

~~~~~~
Handle (Geom_Point) p1;
Handle (Geom_CartesianPoint) p2, p3;
p2 = new Geom_CartesianPoint;
p1 = p2; // OK, standard assignment
p3 = Handle (Geom_CartesianPoint)::DownCast (p1);
// OK, the actual type of p1 is CartesianPoint, although the static type of the handle is Point
~~~~~~

If conversion is not compatible with the actual type of the  referenced object, the handle which was “cast” becomes
 null (and no exception  is raised). So, if you require reliable services defined in a sub-class of the  type seen by
 the handle (static type), write as follows: 

~~~~~~
void MyFunction (const Handle(A) & a)
{
  Handle(B) b =  Handle(B)::DownCast(a);
  if (! b.IsNull()) {
    // we can use “b” if class B inherits from A
  }
  else {
    // the types are incompatible
  }
}
~~~~~~
Downcasting is used particularly with collections of objects  of different types; however, these objects should inherit
 from the same root  class. 

For example, with a sequence of transient objects *SequenceOfTransient* and two classes  A and B that both inherit from 
*Standard_Transient*, you get the  following syntax: 

~~~~~
Handle (A) a;
Handle (B) b;
Handle (Standard_Transient) t;
SequenceOfTransient s;
a = new A;
s.Append (a);
b = new B;
s.Append (b);
t = s.Value (1);
// here, you cannot write:
// a = t; // ERROR !
// so you downcast:
a = Handle (A)::Downcast (t)
if (! a.IsNull()) {
        // types are compatible, you can use a
}
else {
       // the types are incompatible
}
~~~~~

@subsubsection occt_fcug_2_2_3 Using  Handles to Create Objects

To create an object which is manipulated by handle, declare  the handle and initialize it with the standard C++ **new** 
operator,  immediately followed by a call to the constructor. The constructor can be any  of those specified in the
 source of the class from which the object is  instanced. 

~~~~~
Handle (Geom_CartesianPoint) p;
p = new Geom_CartesianPoint (0, 0, 0);
~~~~~

Unlike for a pointer, the **delete** operator does not  work on a handle; the referenced object is automatically 
destroyed when no  longer in use. 

@subsubsection handles_invoking Invoking Methods
Once you have a handle to an object,  you can use it like a pointer in C++. To invoke a method which acts on the 
referenced object, you translate this method by the standard *arrow* operator, or  alternatively, by function call 
syntax when this is available. 

To test or to modify the state of the handle, the method is  translated by the *dot* operator. 
The example below illustrates how to access the coordinates  of an (optionally initialized) point object: 

~~~~~
Handle (Geom_CartesianPoint) centre;
Standard_Real x, y, z;
if (centre.IsNull()) {
  centre = new PGeom_CartesianPoint (0, 0, 0);
}
centre->Coord(x, y, z);
~~~~~

The example below illustrates how to access the type object  of a Cartesian point: 

~~~~~
Handle(Standard_Transient)  p = new Geom_CartesianPoint(0.,0.,0.);
if ( p->DynamicType() ==  STANDARD_TYPE(Geom_CartesianPoint) )
  cout  << ;Type check OK;  << endl; 
else 
  cout << ;Type check FAILED; <<  endl;   
~~~~~

*NullObject* exception will be raised if  a field or a method of an object is accessed via a *Null* handle. 

#### Invoking Class Methods

A class method is called like a static C++ function, i.e. it  is called by the name of the class of which it is a 
member, followed by the “::” operator and the name of the  method. 

For example, we can find the maximum degree of a Bezier curve:

~~~~~
Standard_Integer  n; 
n = Geom_BezierCurve::MaxDegree();
~~~~~

@subsubsection handles_deallocation Handle deallocation

Before you delete an object, you must ensure it is no longer  referenced. To reduce the programming load related to this
 management of object  life, the delete function in Open CASCADE Technology is secured by a **reference counter** of 
classes manipulated by handle. A handle automatically deletes an object when it is no  longer referenced. Normally you 
never call the delete operator explicitly on  instances of subclasses of *Standard_Transient*. 

When a new handle to the same object is created, the  reference counter is incremented. When the handle is destroyed, 
nullified, or  reassigned to another object, that counter is decremented. The object is  automatically deleted by the 
handle when reference counter becomes 0. 

The principle of allocation can be seen in the example  below. 

~~~~~
...
{
Handle (TColStd_HSequenceOfInteger) H1 = new TColStd_HSequenceOfInteger;
  // H1 has one reference and corresponds to 48 bytes of  memory
  {
    Handle (TColStd_HSequenceOfInteger) H2;
    H2 = H1; // H1 has two references
    if (argc == 3) {
      Handle (TColStd_HSequenceOfInteger) H3;
      H3 = H1;
      // Here, H1 has three references
      ...
    }
    // Here, H1 has two references
  }
  // Here, H1 has 1 reference
}
// Here, H1 has no reference and the referred TColStd_HSequenceOfInteger object is deleted. 
~~~~~

You can easily cast a reference to the handle object to <i> void* </i> by defining the following:

~~~~
    void *pointer;
    Handle(Some_class) aHandle;
    // Here only a pointer will be copied
    Pointer = &aHandle;
    // Here the Handle object will be copied
    aHandle = * (Handle(Some_Class) *)pointer;
~~~~

@subsubsection handles_cycles  Cycles

Cycles appear if two or more objects reference each other by  handles (stored as fields). In this condition automatic 
destruction will not work. 

Consider for example a graph, whose objects (primitives)  have to know the graph object to which they belong, i.e. a 
primitive must have  a reference to complete graph object. If both primitives and the graph are  manipulated by handle 
and they refer to each other by keeping a handle as a  field, the cycle appears.  

The graph object will not be deleted when the last handle to  it is destructed in the application, since there are 
handles to it stored  inside its own data structure (primitives). 

There are two approaches how to avoid such situation: 
  * Use C++ pointer for one kind of references, e.g. from a primitive  to the graph
  * Nullify one set of handles (e.g. handles to a graph in  primitives) when a graph object needs to be destroyed
  
@section general_memory_management Memory Management 

In a work session, geometric modeling  applications create and delete a considerable number of C++ objects allocated 
in the dynamic memory (heap). In this context, performance of standard  functions for allocating and deallocating memory
 may be not sufficient. For this reason, Open CASCADE Technology employs a specialized memory manager implemented in the
 *Standard* package. 

The Memory Manager is based on the following principles:

* small memory arrays are grouped into clusters and then recycled (clusters are never released to the system),
* large arrays are allocated and de-allocated through the standard functions of the system (the arrays are released to
 system when they are no longer used).

As a general rule, it is advisable to allocate memory through significant blocks. In this way, the user can work with 
blocks of contiguous data and it facilitates memory page manager processing.

@subsection mem_usage Memory Manager Usage

To  allocate memory in a C code with Open CASCADE Technology memory manager, simply use method *Standard::Allocate()* 
instead of  *malloc()* and method *Standard::Free()* instead of *free()*. In addition, method *Standard::Reallocate()* 
is provided to replace C function *realloc()*. 

In C++, operators *new()* and *delete()* for a class may be  defined so as to allocate memory using 
*Standard::Allocate()* and free it using  *Standard::Free()*. In that case all objects of that class and all inherited 
classes will be allocated using the OCCT memory manager. 

Preprocessor macro *DEFINE_STANDARD_ALLOC* provided by header *Standard_DefineAlloc.hxx* defines *new()* and *delete()* 
in this way.
It is used for all OCCT classes (apart from a few exceptions) which thus are allocated using the OCCT memory manager. 
Since operators *new()* and *delete()* are inherited, this is  also true for any class derived from an OCCT class, for 
instance, for all  classes derived from *Standard_Transient*. 

@note It is possible (though not  recommended unless really unavoidable) to redefine *new()* and *delete()* functions
 for a class inheriting *Standard_Transient*. If that is done, the method  *Delete()* should be also redefined to apply 
operator *delete* to this pointer. This will ensure that appropriate *delete()* function will be called, even if the
 object is manipulated by a handle to a base class.

@subsection mem_configure Memory Manager Configuration

The OCCT memory manager may be configured to apply different  optimization techniques to different memory blocks 
(depending on their size),  or even to avoid any optimization and use C functions *malloc()* and *free()*  directly. 
The configuration is defined by numeric values of the  following environment variables: 
  * *MMGT_OPT*: if set to 0 (default) every memory block is allocated  in C memory heap directly (via *malloc()* and 
*free()* functions). In this case,  all other options except for *MMGT_CLEAR* are ignored; if set to 1 the memory manager
 performs optimizations as described below; if set to 2, Intel ® TBB optimized  memory manager is used.
  * *MMGT_CLEAR*: if set to 1 (default), every allocated memory block  is cleared by zeros; if set to 0, memory block
 is returned as it is.
  * *MMGT_CELLSIZE*: defines the maximal size of blocks allocated in  large pools of memory. Default is 200.
  * *MMGT_NBPAGES*: defines the size of memory chunks allocated for  small blocks in pages (operating-system dependent).
 Default is 1000.
  * *MMGT_THRESHOLD*: defines the maximal size of blocks that are  recycled internally instead of being returned to 
the heap. Default is 40000.
  * *MMGT_MMAP*: when set to 1 (default), large memory blocks are  allocated using memory mapping functions of the 
operating system; if set to 0,  they will be allocated in the C heap by *malloc()*.

@subsection mem_optimization Optimization Techniques

When *MMGT_OPT* is set to 1, the following optimization  techniques are used: 
  * Small blocks with a size less than *MMGT_CELLSIZE*, are not  allocated separately. Instead, a large pools of memory
 are allocated (the size  of each pool is *MMGT_NBPAGES* pages). Every new memory block is arranged in a  spare place of
 the current pool. When the current memory pool is completely  occupied, the next one is allocated, and so on.
  
In the current version memory  pools are never returned to the system (until the process finishes). However,  memory 
blocks that are released by the method *Standard::Free()* are remembered  in the free lists and later reused when the 
next block of the same size is  allocated (recycling). 

  * Medium-sized blocks, with a size greater than *MMGT_CELLSIZE* but  less than *MMGT_THRESHOLD*, are allocated 
directly in the C heap (using *malloc()*  and *free()*). When such blocks are released by the method *Standard::Free()*
 they  are recycled just like small blocks.
  
However, unlike small blocks, the  recycled medium blocks contained in the free lists (i.e. released by the  program but
 held by the memory manager) can be returned to the heap by method  *Standard::Purge()*. 

  * Large blocks with a size greater than *MMGT_THRESHOLD*, including  memory pools used for small blocks, are allocated
 depending on the value of  *MMGT_MMAP*: if it is 0, these blocks are allocated in the C heap; otherwise they  are 
allocated using operating-system specific functions managing memory mapped  files. Large blocks are returned to the
 system immediately when *Standard::Free()* is called. 

@subsection mem_benefits_drawbacks Benefits and drawbacks

The major benefit of the OCCT memory manager is explained  by its recycling of small and medium blocks that makes an
 application work much  faster when it constantly allocates and frees multiple memory blocks of similar sizes. In 
practical situations, the real gain on the application performance  may be up to 50%. 

The associated drawback is that recycled memory is not  returned to the operating system during program execution. 
This may lead to  considerable memory consumption and even be misinterpreted as a memory leak. To  minimize this effect
 it is necessary to call the method *Standard::Purge* after the completion  of memory-intensive operations. 

The overhead expenses induced by the OCCT memory manager  are: 
  * size of every allocated memory block is rounded up to 8 bytes  (when *MMGT_OPT* is 0 (default), the rounding is 
defined by the CRT; the typical  value for 32-bit platforms is 4 bytes)
  * additional 4 bytes (or 8 on 64-bit platforms) are allocated in  the beginning of every memory block to hold its 
size (or address of the next  free memory block when recycled in free list) only when *MMGT_OPT* is 1.
  
Note that these overheads may be greater or less than  overheads induced by the C heap memory manager, so overall memory
 consumption  may be greater in either optimized or standard modes, depending on  circumstances. 

As a general rule, it is advisable to allocate memory  through significant blocks. In this way, you can work with blocks
 of contiguous  data, and processing is facilitated for the memory page manager. 

OCCT memory manager uses mutex to lock access to free lists, therefore it may have less  performance than non-optimized
 mode in situations when different threads often  make simultaneous calls to the memory manager.
The reason is that modern  implementations of *malloc()* and *free()* employ several allocation arenas and  thus avoid 
delays waiting mutex release, which are possible in such situations. 

@section general_exceptions Exceptions 

The behavior of any object is implemented by the methods,  which were defined in its class declaration. The definition 
of these methods  includes not only their signature (their programming interface) but also their domain of validity.  

This domain is expressed by **exceptions**. Exceptions  are raised under various error conditions to protect software 
quality. 

Exception handling provides a means of transferring control  from a given point in a program being executed to an
 **exception handler** associated  with another point previously executed. 

A method may raise an exception which interrupts its normal  execution and transfers control to the handler catching
 this exception. 

A hierarchy of commonly used exception classes is provided. The root class is *Standard_Failure* from the *Standard*
 package.  So each exception inherits from *Standard_Failure* either directly or by inheriting from another exception.
 Exception classes list all  exceptions, which can be raised by any OCCT function. 

Open CASCADE Technology also provides  support for converting system signals (such as access violation or division by 
zero) to exceptions, so that such situations can be safely handled with the  same uniform approach. 
 
However, in order to support this functionality on various  platforms, some special methods and workarounds are used. 
Though the  implementation details are hidden and handling of OCCT exceptions is done  basically in the same way as with
 C++, some peculiarities of this approach  shall be taken into account and some rules must be respected. 

The following paragraphs describe recommended approaches for  using exceptions when working with Open CASCADE Technology.  

@subsection ex_raising Raising Exception

## “C++ like” Syntax

To raise an exception of a definite type method Raise() of  the appropriate exception class shall be used. 
~~~~~
DomainError::Raise(“Cannot cope with this condition”);
~~~~~
raises an exception of *DomainError* type with the associated  message “Cannot cope with this condition”, the message 
being optional. This  exception may be caught by a handler of a *DomainError* type as follows: 
~~~~~
try {
  OCC_CATCH_SIGNALS
  // try block
}
catch(DomainError) {
// handle DomainError exceptions here
}
~~~~~

## Regular usage

Exceptions should not be used as a programming technique, to  replace a “goto” statement for example, but as a way to
 protect methods against  misuse. The caller must make sure its condition is such that the method can  cope with it. 

Thus, 
  * No exception should be raised during normal execution of an  application.
  * A method which may raise an exception should be protected by  other methods allowing the caller to check on the 
validity of the call.
  
For example, if you consider the *TCollection_Array1* class  used with: 
  * *Value*  function to extract an element
  * *Lower*  function to extract the lower bound of the array
  * *Upper*  function  to extract the upper bound of the array. 
  
then, the *Value*  function may be implemented as follows: 

~~~~~
Item  TCollection_Array1::Value (const Standard_Integer&index) const
{
  // where r1 and r2 are  the lower and upper bounds of the array
  if(index < r1 || index > r2) {
    OutOfRange::Raise(“Index  out of range in Array1::Value”);
  }
  return contents[index];
}
~~~~~

Here validity of the index is first verified using the Lower and Upper functions in order to protect the call. 
Normally the caller ensures the index being in the valid  range before calling <i>Value()</i>. In this case the above 
implementation of *Value* is not optimal since the  test done in *Value* is  time-consuming and redundant. 
 
It is a widely used practice to include that kind of  protections in a debug build of the program and exclude in release
 (optimized)  build. To support this practice, the macros <i>Raise_if()</i> are provided for every OCCT  exception class: 
~~~~~
<ErrorTypeName>_Raise_if(condition,  “Error message”); 
~~~~~
where *ErrorTypeName* is the exception type, *condition*  is the logical expression leading to the raise of the exception,
 and *Error message* is the associated  message. 
  
The entire call may be removed by defining one of the preprocessor symbols *No_Exception* or <i>No_<ErrorTypeName></i> 
at compile-time: 

~~~~~
#define  No_Exception /* remove all raises */ 
~~~~~

Using this syntax, the *Value* function becomes: 

~~~~~
Item  TCollection_Array1::Value (const Standard_Integer&index) const
     { 
  OutOfRange_Raise_if(index < r1 || index > r2,
                      “index out of range in  Array1::Value”);
  return contents[index];
}
~~~~~

@subsection ex_handling Handling Exception

When an exception is raised, control is transferred to the  nearest handler of a given type in the call stack, that is: 
  * the handler whose try block was most recently entered and not yet  exited,
  * the handler whose type matches the raise expression.
  
A handler of T exception type is a match for a raise  expression with an exception type of E if: 
  * T and E are of the same type, or
  * T is a supertype of E.
  
In order to handle system signals as exceptions, make sure  to insert macro *OCC_CATCH_SIGNALS* somewhere in the 
beginning of the relevant  code. The recommended location for it is first statement after opening brace of <i>try {}</i> block. 

As an example, consider the exceptions of type *NumericError, Overflow, Underflow* and *ZeroDivide*, where *NumericError* 
is the parent type of the three others. 

~~~~~
void f(1)
 {
  try {
    OCC_CATCH_SIGNALS
    // try block
  }
  catch(Standard_Overflow) { // first handler
    // ...
  }
  catch(Standard_NumericError) { // second handler
    // ...
  }
}
~~~~~

Here, the first handler will catch exceptions of *Overflow* type and the second one -- exceptions of *NumericError* type 
and all exceptions derived from it, including *Underflow* and *ZeroDivide*. 

The handlers are checked in order of appearance, from the nearest to the try block to the most distant from it, until one 
matches the raise expression.  For a try block, it would be a mistake to place a handler for a base exception type ahead 
of a handler for its derived type since that would ensure that the  handler for the derived exception would never 
be invoked.  

~~~~~
void f(1)
{
  int i = 0;
  {
    try {
      OCC_CATCH_SIGNALS
      g(i);// i is accessible
    }
    // statement here will produce compile-time errors !
    catch(Standard_NumericError) {
      // fix up with possible reuse of i
    }
    // statement here may produce unexpected side effect 
  }
  . . .
}
~~~~~

The exceptions form a hierarchy tree completely separated  from other user defined classes. One exception of type 
*Failure* is the root of the entire exception  hierarchy. Thus, using a handler with *Failure* type catches any OCCT
 exception. It is recommended to set up such a handler in  the main routine.  

The main routine of a program would look like this: 

~~~~~
#include <Standard_ErrorHandler.hxx>
#include <Standard_Failure.hxx>
#include <iostream.h>
int main (int argc, char* argv[])
{
  try {
    OCC_CATCH_SIGNALS
    // main block
    return 0;
  }
  catch(Standard_Failure) {
    Handle(Standard_Failure) error = Standard_Failure::Caught  ();
    cout  error  end1;
  }
  return 1;
}
~~~~~

In this example function *Caught* is a static member of *Failure* that  returns an exception object containing the error
 message built in the raise  expression. Note that this method of accessing a raised object is used in Open CASCADE
 Technology instead of usual C++ syntax (receiving the exception in  catch argument). 

Though standard C++ scoping  rules and syntax apply to try block and handlers, note that on some platforms Open  CASCADE 
Technology may be compiled in compatibility mode when exceptions are  emulated by long jumps (see below). In this mode 
it is required that no  statement precedes or follows any handler. Thus it is highly recommended to  always include a 
try block into additional {} braces. Also this mode requires  that header file *Standard_ErrorHandler.hxx* be included 
in your program before a  try block, otherwise it may fail to handle Open CASCADE Technology exceptions; furthermore 
*catch()* statement does not allow passing exception object as  argument. 

## Catching signals

In order for the application to be able to catch system  signals (access violation, division by zero, etc.) in the same 
way as other  exceptions, the appropriate signal handler shall be installed in the runtime by  the method *OSD::SetSignal()*.
 
Normally this method is called in the beginning of the  main() function. It installs a handler that will convert system 
signals into OCCT  exceptions. 

In order to actually convert signals to exceptions, macro *OCC_CATCH_SIGNALS* needs to be inserted in the source code.
 The typical place where  this macro is put is beginning of the *try{}* block which catches such exceptions.   

@subsection ex_implementation Implementation on Various Platforms. 

The exception handling mechanism in Open CASCADE Technology  is implemented in different ways depending on the 
preprocessor macros *NO_CXX_EXCEPTIONS*  and *OCC_CONVERT_SIGNALS*, which shall be consistently defined by compilation
 procedures for both Open CASCADE Technology and user applications: 

1. On  Windows, these macros are not defined by default, and normal C++  exceptions are used in all cases, including 
throwing from signal handler. Thus the  behavior is as expected in C++. 

2. On  Linux, macro *OCC_CONVERT_SIGNALS* is defined by default. The C++  exception mechanism is used for catching
 exceptions and for throwing them from  normal code. Since it is not possible to throw C++ exception from system signal
 handler function, that function makes a long jump to the nearest (in the  execution stack) invocation of macro
 *OCC_CATCH_SIGNALS*, and only there the C++  exception gets actually thrown. The macro *OCC_CATCH_SIGNALS* is defined 
in the  file *Standard_ErrorHandler.hxx*. Therefore, including this file is necessary for  successful compilation of a
 code containing this macro. 

   This mode differs from standard  C++ exception handling only for signals:
 
   * macro *OCC_CATCH_SIGNALS* is necessary (besides call to  *OSD::SetSignal()* described above) for conversion of 
signals into exceptions;
   * the destructors for automatic C++ objects created in the code  after that macro and till the place where signal is
 raised will not be called in  case of signal, since no C++ stack unwinding is performed by long jump.

3. On  Linux Open CASCADE Technology can also be compiled in compatibility  mode. In that case macro *NO_CXX_EXCEPTIONS*
 is defined and the C++ exceptions are simulated with C long  jumps. As a consequence, the behavior is slightly different
 from that expected  in the C++ standard.  

While exception handling with *NO_CXX_EXCEPTIONS* is very similar to C++ by syntax, it has a number of peculiarities 
that should be taken into account: 

* try and catch are actually macros defined in the file *Standard_ErrorHandler.hxx*. Therefore, including this file is 
necessary for  handling OCCT exceptions;
* due to being a macro, catch cannot contain a declaration of the  exception object after its type; only type is allowed
 in the catch statement.  Use method *Standard_Failure::Caught()* to access an exception object;
* catch macro may conflict with some STL classes that might use  catch(...) statements in their header files. So STL
 headers should not be  included after *Standard_ErrorHandler.hxx*;
* Open CASCADE Technology try/catch block will not handle normal  C++ exceptions; however this can be achieved using
 special workarounds;
* the try macro defines a C++ object that holds an entry point in the  exception handler. Therefore if exception is
 raised by code located immediately  after the try/catch block but on the same nesting level as *try*, it may be
 handled by that *catch*. This may lead to unexpected behavior,  including infinite loop. To avoid that, always surround 
the try/catch block with curved brackets;
* the destructors of C++ objects allocated on the stack after  handler initialization are not called by exception raising.

In general, for writing platform-independent code it is recommended  to insert macros *OCC_CATCH_SIGNALS* in try {} 
blocks or other code where signals  may happen. For compatibility with previous versions of Open CASCADE Technology
 the limitations described above for *NO_CXX_EXCEPTIONS* shall be assumed. 

@section general_plugin Plug-In Management

A plug-in is a component that can be loaded dynamically into  a client application, not requiring to be directly linked
 to it. The plug-in is  not bound to its client, i.e. the plug-in knows only how its connection  mechanism is defined 
and how to call the corresponding services. 

A plug-in can be used to: 
  * implement the mechanism of a *driver*, i.e dynamically  changing a driver implementation according to the current 
transactions (for  example, retrieving a document stored in another version of an application),
  * restrict processing resources to the minimum required (for  example, it does not load any application services at 
run-time as long as the  user does not need them),
  * facilitate modular development  (an application can be  delivered with base functions while some advanced 
capabilities will be added as  plug-ins when they are available).
  
The plug-in is identified with the help of the global  universal identifier (GUID). The GUID includes lower case 
characters and cannot  end with a blank space. 

Once it has been loaded, the call to the services provided  by the plug-in is direct (the client is implemented in the 
same language as the  plug-in). 

## C++ Plug-In  Implementation

The C++ plug-in implements a service as an object with  functions defined in an abstract class (this abstract class and 
its parent  classes with the GUID are the only information about the plug-in implemented in  the client application). 
The plug-in consists of a sharable library including a  method named Factory which  creates the C++ object (the client 
cannot instantiate this object because the  plug-in implementation is not visible). 
Foundation classes provide in the package *Plugin* a  method named *Load()*, which enables the client to access the 
required service  through a library.  

That method reads the information regarding available  plug-ins and their locations from the resource file *Plugin* 
found by environment  variable *CSF_PluginDefaults*:

~~~~~ 
$CSF_PluginDefaults/.Plugin 
~~~~~

The *Load* method looks for the library name in the resource file or registry  through its GUID, for example, on UNIX:
~~~~~
! METADATADRIVER whose value must be OS or DM.

! FW
a148e300-5740-11d1-a904-080036aaa103.Location:
 
libFWOSPlugin.so
a148e300-5740-11d1-a904-080036aaa103.CCL:
/adv_44/CAS/BAG/FW-K4C/inc/FWOS.ccl

! FWDM
a148e301-5740-11d1-a904-080036aaa103.Location:
libFWDMPlugin.so
a148e301-5740-11d1-a904-080036aaa103.CCL:
/adv_44/CAS/BAG/DESIGNMANAGER-K4C/inc/DMAccess.ccl|/
adv_44/CAS/BAG/DATABASE-K4C/inc/FWDMCommands.ccl
a148e301-5740-11d1-a904-080036aaa103.Message:  /adv_44/CAS/
BAG/DESIGNMANAGER-K4C/etc/locale/DMAccess

! Copy-Paste
5ff7dc00-8840-11d1-b5c2-00a0c9064368.Location:
libCDMShapeDriversPlugin.so
5ff7dc01-8840-11d1-b5c2-00a0c9064368.Location:
libCDMShapeDriversPlugin.so
5ff7dc02-8840-11d1-b5c2-00a0c9064368.Location:
libCDMShapeDriversPlugin.so
5ff7dc03-8840-11d1-b5c2-00a0c9064368.Location:
libCDMShapeDriversPlugin.so
5ff7dc04-8840-11d1-b5c2-00a0c9064368.Location:
libCDMShapeDriversPlugin.so

! Plugs 2d plotters:
d0d722a2-b4c9-11d1-b561-0000f87a4710.location: FWOSPlugin
d0d722a2-b4c9-11d1-b561-0000f87a4710.CCL: /adv_44/CAS/BAG/
VIEWERS-K4C/inc/CCLPlotters.ccl
d0d722a2-b4c9-11d1-b561-0000f87a4710.Message: /adv_44/CAS/
BAG/VIEWERS-K4C/etc/locale/CCLPlotters

!SHAPES
e3708f72-b1a8-11d0-91c2-080036424703.Location:
libBRepExchangerPlugin.so
e3708f72-b1a8-11d0-91c2-080036424703.CCL: /adv_44/CAS/BAG/
FW-K4C/inc/BRep.ccl
~~~~~

Then the *Load* method loads the library according to the rules of the operating system  of the host machine (for 
example, by using environment variables such as  *LD_LIBRARY_PATH* with Unix and *PATH* with Windows). After that it 
invokes the *Factory*  method to return the object which supports the required service.
The client may then call the functions supported by this  object. 

## C++ Client Plug-In Implementation

To invoke one of the services provided by the plug-in, you  may call the *Plugin::ServiceFactory* global function with
 the *Standard_GUID* of the requested service as follows: 

~~~~~
Handle(FADriver_PartStorer)::DownCast 
(PlugIn::ServiceFactory 
(PlugIn_ServiceId(yourStandardGUID))) 
~~~~~

Let us take *FAFactory.cxx* as an example:

~~~~~
#include <FAFactory.ixx>

#include <FADriver_PartRetriever.hxx>
#include <FADriver_PartStorer.hxx>
#include <FirstAppSchema.hxx>
#include <Standard_GUID.hxx>
#include <Standard_Failure.hxx>
#include <FACDM_Application.hxx>
#include <Plugin_Macro.hxx>

PLUGIN(FAFactory)

static Standard_GUID 
       StorageDriver(“45b3c690-22f3-11d2-b09e-0000f8791463”);
static Standard_GUID 
       RetrievalDriver(“45b3c69c-22f3-11d2-b09e-0000f8791463”);
static Standard_GUID 
       Schema(“45b3c6a2-22f3-11d2-b09e-0000f8791463”);

//======================================================
// function : Factory
// purpose :
//======================================================
 
Handle(Standard_Transient)  FAFactory::Factory(const Standard_GUID& aGUID) 
{
  if(aGUID == StorageDriver) {
    cout  “FAFactory : Create store driver”   endl;
    static  Handle(FADriver_PartStorer) sd = new FADriver_PartStorer();
    return sd;
  }

  if(aGUID == RetrievalDriver) {
    cout  “FAFactory : Create retrieve driver”   endl;
    static Handle(FADriver_PartRetriever)
    rd = new FADriver_PartRetriever();
    return rd;
  }

  if(aGUID == Schema) {
    cout  “FAFactory : Create schema”   endl;
    static Handle(FirstAppSchema) s = new  FirstAppSchema();
    return s;
  }

  Standard_Failure::Raise(“FAFactory: unknown GUID”);
  Handle(Standard_Transient) t;
  return t;
}
~~~~~

## Without using the Software Factory

To create a factory without using the Software Factory,  define a *dll* project under Windows or a library under UNIX by
 using a  source file as specified above. The *FAFactory* class is implemented as follows: 

~~~~~
#include <Handle_Standard_Transient.hxx>
#include <Standard_Macro.hxx>
class Standard_Transient;
class Standard_GUID;
class FAFactory {
public:
  Standard_EXPORT  static Handle_Standard_Transient
                  Factory(const Standard_GUID& aGUID)  ;
  . . .
};
~~~~~
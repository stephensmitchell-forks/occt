Topology API {#user_guides__modeling_algos_topology_api}
============

@tableofcontents
 
The Topology  API of Open  CASCADE Technology (**OCCT**) includes the following six packages: 
  * *BRepAlgoAPI*
  * *BRepBuilderAPI*
  * *BRepFilletAPI*
  * *BRepFeat*
  * *BRepOffsetAPI*
  * *BRepPrimAPI*

The classes provided by the API have the following features:
  * The constructors of classes provide different construction methods;
  * The class retains different tools used to build objects as fields;
  * The class provides a casting method to obtain the result automatically with a function-like call.   
  
Let us use the class *BRepBuilderAPI_MakeEdge* to create a linear edge from two  points. 

~~~~~
gp_Pnt P1(10,0,0), P2(20,0,0); 
TopoDS_Edge E = BRepBuilderAPI_MakeEdge(P1,P2);
~~~~~

This is the simplest way to create edge E from two  points P1, P2, but the developer can test for errors when he is not as  confident of the data as in the previous example. 

~~~~~
#include <gp_Pnt.hxx> 
#include <TopoDS_Edge.hxx> 
#include <BRepBuilderAPI_MakeEdge.hxx> 
void EdgeTest() 
{ 
gp_Pnt P1; 
gp_Pnt P2; 
BRepBuilderAPI_MakeEdge ME(P1,P2); 
if (!ME.IsDone()) 
{ 
// doing ME.Edge() or E = ME here 
// would raise StdFail_NotDone 
Standard_DomainError::Raise 
("ProcessPoints::Failed to createan edge"); 
} 
TopoDS_Edge E = ME; 
} 
~~~~~

In this example an  intermediary object ME has been introduced. This can be tested for the  completion of the function before accessing the result. More information on **error  handling** in the topology programming interface can be found in the next section. 

*BRepBuilderAPI_MakeEdge*  provides valuable information. For example, when creating an edge from two  points, two vertices have to be created from the points. Sometimes you may be  interested in getting these vertices quickly without exploring the new edge.  Such information can be provided when using a class. The following example  shows a function creating an edge and two vertices from two points. 

~~~~~
void MakeEdgeAndVertices(const gp_Pnt& P1, 
const gp_Pnt& P2, 
TopoDS_Edge& E, 
TopoDS_Vertex& V1, 
TopoDS_Vertex& V2) 
{ 
BRepBuilderAPI_MakeEdge ME(P1,P2); 
if (!ME.IsDone()) { 
Standard_DomainError::Raise 
("MakeEdgeAndVerices::Failed  to create an edge"); 
} 
E = ME; 
V1 = ME.Vextex1(); 
V2 = ME.Vertex2(); 
~~~~~

The class *BRepBuilderAPI_MakeEdge*  provides two methods *Vertex1* and  *Vertex2*, which return two vertices used to create the edge. 

How can *BRepBuilderAPI_MakeEdge* be both a function and a class? It can do this  because it uses the casting capabilities of C++. The *BRepBuilderAPI_MakeEdge* class has a method called Edge; in the previous  example the line <i>E = ME</i> could have been written. 

~~~~~
E = ME.Edge(); 
~~~~~

This instruction tells  the C++ compiler that there is an **implicit casting** of a *BRepBuilderAPI_MakeEdge* into a *TopoDS_Edge* using the *Edge* method. It means this method is automatically called when a *BRepBuilderAPI_MakeEdge* is found where a *TopoDS_Edge* is required. 

This feature allows you  to provide classes, which have the simplicity of function calls when required  and the power of classes when advanced processing is necessary. All the  benefits of this approach are explained when describing the topology programming  interface classes. 

@section topapi_errors Error Handling in the Topology API

A method can report an  error in the two following situations: 
  * The data or arguments of the  method are incorrect, i.e. they do not respect the restrictions specified by  the methods in its specifications. Typical example: creating a linear edge from  two identical points is likely to lead to a zero divide when computing the  direction of the line.
  * Something unexpected  happened. This situation covers every error not included in the first category.  Including: interruption, programming errors in the method or in another method  called by the first method, bad specifications of the arguments (i.e. a set of  arguments that was not expected to fail).

The second situation is  supposed to become increasingly exceptional as a system is debugged and it is  handled by the **exception mechanism**. Using exceptions avoids handling  error statuses in the call to a method: a very cumbersome style of programming. 

In the first situation,  an exception is also supposed to be raised because the calling method should  have verified the arguments and if it did not do so, there is a bug. For example, if before calling *MakeEdge* you are not sure that the two points are  non-identical, this situation must be tested. 

Making those validity  checks on the arguments can be tedious to program and frustrating as you have  probably correctly surmised that the method will perform the test twice. It  does not trust you. 
As the test involves a  great deal of computation, performing it twice is also time-consuming. 

Consequently, you might be tempted to adopt the highly inadvisable style of programming  illustrated in the following example: 

~~~~~
#include <Standard_ErrorHandler.hxx> 
try { 
TopoDS_Edge E = BRepBuilderAPI_MakeEdge(P1,P2); 
// go on with the edge 
} 
catch { 
// process the error. 
} 
~~~~~

To help the user, the  Topology API classes only raise the exception *StdFail_NotDone*. Any other  exception means that something happened which was unforeseen in the design of  this API. 

The *NotDone* exception  is only raised when the user tries to access the result of the computation and  the original data is corrupted. At the construction of the class instance, if  the algorithm cannot be completed, the internal flag *NotDone* is set. This flag  can be tested and in some situations a more complete description of the error  can be queried. If the user ignores the *NotDone* status and tries to access the  result, an exception is raised. 

~~~~~
BRepBuilderAPI_MakeEdge ME(P1,P2); 
if (!ME.IsDone()) { 
// doing ME.Edge() or E = ME here 
// would raise StdFail_NotDone 
Standard_DomainError::Raise 
("ProcessPoints::Failed to create an edge"); 
} 
TopoDS_Edge E = ME; 
~~~~~

@section topapi_modification Object Modification

@subsection modif_trsf_1 Transformation
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

@subsection modif_duplicate Duplication

Use the  *BRepBuilderAPI_Copy* class to duplicate a shape. A new shape is thus created. 
In the following example, a  solid is copied: 

~~~~~
TopoDS Solid MySolid; 
....// Creates a solid 

TopoDS_Solid myCopy = BRepBuilderAPI_Copy(mySolid); 
~~~~~
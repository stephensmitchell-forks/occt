Function Services {#user_guides__ocaf_func_services}
=================

@tableofcontents

Function services aggregate data necessary for regeneration of a model. The function mechanism -- available in the 
package *TFunction* -- provides links between functions and any execution algorithms, which take their arguments from
 the data framework, and write their results inside the same framework. 

When you edit any application model, you have to regenerate the model by propagating the modifications. Each propagation
 step calls various algorithms. To make these algorithms independent of your application model, you need to use function
 services. 

@image html /user_guides/ocaf/images/ocaf_image008.png "Document structure"
@image latex /user_guides/ocaf/images/ocaf_image008.png "Document structure"

Take, for example, the case of a modeling sequence made up of a box with the application of a fillet on one of its edges.
 If you change the height of the box, the fillet will need to be regenerated as well. 

@section occt_ocaf_8_1 Functions, Their Owners and Roots

The class *TFunction_Function* is an attribute, which stores a link to a function driver in the data framework. In the 
static table *TFunction_DriverTable* correspondence links between function attributes and drivers are stored. 

You can write your function attribute, a driver for such attribute, which updates the function result in accordance to a
 given map of changed labels, and set your driver with the GUID to the driver table. 

Then the solver algorithm of a data model can find the *Function* attribute on a corresponding label and call the 
*Execute* driver method to update the result of the function. 

@section occt_ocaf_8_2 Function Status

For updating algorithm optimization, each function driver has access to the *TFunction_Logbook* object that is a
 container for a set of touched, impacted and valid labels. Using this object a driver gets to know which arguments of 
the function were modified. 

@section occt_ocaf_8_3 Propagating Modifications

An application must implement its functions, function drivers and the common solver for parametric model creation. For 
example, check the following model: 

@image html /user_guides/ocaf/images/ocaf_image017.png
@image latex /user_guides/ocaf/images/ocaf_image017.png

The procedure of its creation is as follows:
  * create a rectangular planar face *F* with height 100 and width 200;
  * create prism *P* using face *F* as a basis;
  * create fillet *L* at the edge of the prism;
  * change the width of *F* from 200 to 300;
  * the solver for the function of face *F* starts;
  * the solver detects that an argument of the face *F* function has been modified;
  * the solver calls the driver of the face *F* function for a  regeneration of the face;
  * the driver rebuilds face *F* and adds the label of the face *width* argument to the logbook as touched and the label
 of the function of face *F* as impacted;

  * the solver detects the function of *P* -- it depends on the function of *F*;
  * the solver calls the driver of the prism *P* function;
  * the driver rebuilds prism *P* and adds the label of this prism to the logbook as  impacted;
  * the solver detects the function of *L*  -- it depends on the function of *P*;
  * the solver calls the *L* function driver;
  * the driver rebuilds fillet *L* and adds the label of the fillet to the logbook as impacted.
  
 If interested see the code snippets of @ref samples__ocaf_func "function mechanism usage".
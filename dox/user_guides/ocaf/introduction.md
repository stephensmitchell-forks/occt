Introduction {#user_guides__ocaf_intro}
============

@tableofcontents

This manual explains how to use the Open CASCADE Application Framework (OCAF).
It provides basic documentation on using OCAF.

@section occt_ocaf_1_1 Purpose of OCAF

OCAF (the Open CASCADE Application Framework) is an  easy-to-use platform for rapidly developing
  sophisticated domain-specific  design applications. 
  A typical application developed using OCAF deals with two or three-dimensional (2D or 3D) geometric modeling 
  in trade-specific Computer Aided Design (CAD) systems, manufacturing or analysis applications, 
  simulation  applications or illustration tools. 
  
  Developing a design application requires addressing many technical aspects. 
  In particular, given the functional specification of your application, you must at least:  
  
  * Design the  architecture of the application— definition of the software components and the way they cooperate;
  * Define the  data model able to support the functionality required — a design application  operates on data
 maintained during the whole end-user working session;
  * Structure  the software in order to:
    * synchronize the display with the  data — commands modifying objects must update the views;  
    * support generalized undo-redo  commands — this feature has to be taken into account very early in the design
 process;  
  * Implement  the function for saving the data — if the application has a long life cycle,  the compatibility of data
 between versions of the application has to be  addressed;
  * Build the  application user interface.
 
Architectural guidance and ready-to-use solutions provided by OCAF offer you the following benefits:
  * You can concentrate on the functionality specific for your application;  
  * The underlying mechanisms required to support the application are already provided; 
  * The application can be rapidly be prototyped thanks to the coupling the other Open CASCADE Technology modules;
  * The final application can be developed by industrializing the prototype — you don't need to restart the development 
from scratch. 
  * The Open Source nature of the platform guarantees the long-term usefulness of your development.   

OCAF is much more than just one toolkit among many in the CAS.CADE Object Libraries. Since it can handle any data and 
algorithms in these libraries -- be it modeling algorithms, topology or geometry -- OCAF is their logical supplement. 

The table below contrasts the design of a modeling application using object libraries alone and using OCAF. 

**Table 1: Services provided by OCAF** 

|Development tasks	|Comments | Without OCAF	| With OCAF |
|------------------:|---------:|---------------:|-----------:|
|Creation of geometry| Algorithm Calling the modeling libraries |	To be created by the user	| To be created by the user| 
| Data organization | Including specific attributes and modeling process | To be created by the user |	Simplified| 
| Saving data in a file | Notion of document | To be created by the user | Provided |
| Document-view management |    |  To be created by the user | Provided |
| Application infrastructure | New, Open, Close, Save and Save As File menus | To be created by the user | Provided | 
| Undo-Redo | Robust, multi-level	| To be created by the user	| Provided |
| Application-specific dialog boxes	|    | To be created by the user  |	To be created by the user |

OCAF uses other modules of Open CASCADE Technology — the Shape  is implemented with the geometry supported by the
 @ref user_guides__modeling_data module and the viewer is the one provided with the @ref user_guides__visualization module.
 Modeling functions can be implemented using the @ref user_guides__modeling_algos module.

The relationship between OCAF and OCCT Object Libraries can be seen in the image below. 

@figure{/user_guides/ocaf/images/ocaf_image003.svg, "OCCT Architecture"}

In the image, the OCAF (Open CASCADE Application Framework) is shown with black rectangles and OCCT Object Libraries
 required by OCAF are shown with white rectangles. 
 
The subsequent chapters of this document explain the concepts and show how to use the services of OCAF.

@section occt_ocaf_1_2 Architecture Overview

OCAF provides you with an object-oriented Application-Document-Attribute model consisting of C++ class libraries. 

@image html ocaf_wp_image003.png "The Application-Document-Attribute model"
@image latex ocaf_wp_image003.png "The Application-Document-Attribute model"

## Application

The *Application* is an abstract class in charge of handling documents during the working session, namely:  
  * Creating new  documents;
  * Saving documents and opening them;
  * Initializing document views.
  
## Document   
 
  The document, implemented by the concrete class  *Document*, is the container for the application data. Documents
 offer access to the data framework and serve the following purposes: 

  * Manage the notification of changes
  * Update external links
  * Manage the saving and restoring of data
  * Store the names of software extensions.
  * Manage command transactions
  * Manage Undo and Redo options. 
  
  Each  document is saved in a single flat ASCII file defined by its format and  extension (a ready-to-use format is
 provided with  OCAF).  

  Apart from their role as a container of application data, documents can refer to each other; Document A, for example, 
can refer to a specific label in Document B. This functionality is made possible by means of the reference key.   
  
## Attribute

  Application data is described by **Attributes**, which are instances of  classes derived from the *Attribute* abstract
 class, organized according to the OCAF Data  Framework. 
  
  The @ref user_guides__ocaf_data_framework "OCAF Data Framework" references aggregations of attributes using  persistent identifiers in a
 single hierarchy. A wide range of  attributes come with OCAF, including:    
  
  * @ref user_guides__ocaf_std_attribs "Standard attributes" allow operating with simple common data in the data framework (for example:
 integer, real, string, array kinds of data), realize auxiliary functions (for example: tag sources attribute for the 
children of the label counter), create dependencies (for example: reference, tree node)....;
  * @ref user_guides__ocaf_shape_attribs "Shape attributes" contain the geometry of the whole model or its elements including reference to
 the shapes and tracking of shape evolution;
  * Other  geometric attributes such as **Datums** (points, axis and plane) and **Constraints** (*tangent-to, 
at-a-given-distance, from-a-given-angle, concentric,* etc.)
  * User  attributes, that is, attributes typed by the application  
  * @ref user_guides__ocaf_visu_attribs "Visualization attributes" allow placing viewer information to the data framework, visual 
representation of objects and other auxiliary visual information, which is needed for graphical data representation.
  * @ref user_guides__ocaf_func_services "Function services" — the purpose of these attributes is to rebuild  objects after they have been
 modified (parameterization of models). While the document manages the notification of changes, a function manages 
propagation of these changes. The function mechanism provides links between functions and calls to various algorithms. 
  
In addition,  application-specific data can be added by defining new attribute classes; naturally, this changes the 
standard file format. The only functions that have to be implemented are:
    * Copying the  attribute
    * Converting  it from and persistent data storage
	
@section occt_ocaf_1_3  Reference-key model

  In most existing geometric modeling systems, the data are topology driven. 
  They usually use a boundary representation (BRep), where geometric models 
  are defined by a collection of faces, edges and vertices, 
  to which application data are attached. Examples of data include:  
 
  * a color;
  * a material;
  * information that a particular edge is blended.
 
  When the geometric model is parameterized, that is, when you can change 
  the value of parameters used to build the model (the radius of a  blend, the thickness of a rib, etc.), 
  the geometry is highly subject to change. 
  In order to maintain the attachment of application data, the geometry must be  distinguished from other data.  
   
  In OCAF, the data are reference-key driven. It is a  uniform model in which reference-keys 
  are the persistent identification of  data. All **accessible** data, including the geometry, 
  are implemented as attributes attached to reference-keys. 
  The geometry becomes the  value of the Shape attribute, just as a number is the value 
  of the Integer and  Real attributes and a string that of the Name attribute.  
   
  On a single reference-key, many attributes can be  aggregated; 
  the application can ask at runtime which attributes are available. 
  For example, to associate a texture to a face in a geometric model, 
  both the face and the texture are attached to the same reference-key.  
 
@image html ocaf_image004.png "Topology driven versus reference-key driven approaches" 
@image latex ocaf_image004.png "Topology driven versus reference-key driven approaches" 

 Reference-keys can be created in two ways:   
 
  * At  programming time, by the application
  * At runtime,  by the end-user of the application (providing that you include this capability  in the application)
 
  As an application developer, you generate reference-keys in order to give semantics to the data. 
  For example, a function building a  prism may create three reference-keys: 
  one for the base of the prism, a second  for the lateral faces and a third for the top face. 
  This makes up a semantic  built-in the application's prism feature.
  On the other hand, in a command  allowing the end-user to set a texture to a face he/she selects, 
  you must create  a reference-key to the selected face 
  if it has not previously been referenced in any feature 
  (as in the case of one of the lateral faces of the prism).  
 
  When you create a reference-key to selected topological  elements (faces, edges or vertices), 
  OCAF attaches to the reference-key  information defining the selected topology — the Naming attribute. 
  For example,  it may be the faces to which a selected edge is common to. 
  This information, as  well as information about the evolution of the topology at each modeling step 
  (the modified, updated and deleted faces), is used by the naming algorithm to  maintain the topology 
  attached to the reference-key. As such, on a  parametrized model, 
  after modifying the value of a parameter, the reference-keys still address the appropriate faces, 
  even if their geometry has  changed. 
  Consequently, you change the size of the cube shown in the figure above, 
  the user texture stay attached to the right face.  
 
  <b>Note</b> As Topological naming is based on the reference-key and attributes such as Naming 
  (selection information) and Shape (topology evolution  information), 
  OCAF is not coupled to the underlying modeling libraries. 
  The  only modeling services required by OCAF are the following:  

  * Each  algorithm must provide information about the evolution of the topology 
   (the  list of faces modified, updated and deleted by the algorithm)
  * Exploration  of the geometric model must be available 
   (a 3D model is made of faces bounded  by close wires, 
   themselves composed by a sequence of edges connected by their  vertices)
 
  Currently, OCAF uses the Open CASCADE Technology  modeling libraries.   
 
  To design an OCAF-based data model, the application  developer is encouraged to aggregate 
  ready-to-use attributes instead of defining new attributes by inheriting from an abstract root class.  
  There are two major advantages in using aggregation  rather than inheritance:
  
  * As you don't  implement data by defining new classes, the format of saved data 
    provided with OCAF doesn't change; so you don't have to write the Save and Open functions
  * The application can query the data at runtime if a particular attribute is  available
  
###Summary

  * OCAF is  based on a uniform reference-key model in which:
    * Reference-keys provide  persistent identification of data;  
    * Data, including geometry, are  implemented as attributes attached to reference-keys;  
    * Topological naming maintains the  selected geometry attached to reference-keys in parametrized models; 
  * In many  applications, the data format provided with OCAF doesn't need to be extended;
  * OCAF is not  coupled to the underlying modeling libraries.

For advanced information on OCAF 
and its applications, see our <a href="http://www.opencascade.com/content/tutorial-learning">E-learning & Training</a>
 offerings.

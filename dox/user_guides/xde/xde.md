 Extended Data Exchange (XDE)  {#occt_user_guides__xde}
============================

@tableofcontents

@section occt_xde_1 Introduction

For hierarchically organizing engineering data, OCCT provides application framework (OCAF). A specialization of OCAF for representing CAD product information is XDE (eXtended Data Exchange) toolset. XDE is capable of associating geometry with metadata (PMI, names, colors, layers, etc.). This framework uses the component-level abstraction of an assembly (see \ref occt_xde_references_ShahMantyla95 "[Shah and Mantyla 1995]" for the overview of possible abstractions). The leaf nodes in the XDE hierarchy are the _part instances_. The interior nodes are the _subassemblies_. The root node represents the _entire assembly_ (several roots are allowed).

@figure{/user_guides/xde/images/646_xde_11_400.png,"Shape imported using XDE (courtesy of Dave Goetsch via GrabCAD).",240}

@subsection occt_xde_1_1 Basic terms

We use the following terminology throughout this documentation:

* **Assembly:** a hierarchy of elements representing a complex product.

* **Subassembly:** a group of elements inside an assembly. Typically, a subassembly is a set of components that are physically assembled into a unit and then assembled with other components.

* **Part:** a piece of data representing a simple individual product.

* **Instance:** a *usage occurrence* of a part or a subassembly. An instance is usually mounted to a parent component by specifying relative transformation. It should be noted that whenever term "instance" is used, it may refer to either a part or a subassembly. When an individual part is mentioned, we use a more precise "part instance" term.

* **Assembly item:** any element in the component hierarchy of an assembly. If visualization is employed, an assembly item typically corresponds to an element in a scene graph.

* **Component:** any *nested* assembly item.

The key terms are illustrated in the figure below.

@figure{/user_guides/xde/images/xde_assembly_design.png,"Parts and instances.",240}

The example above assumes that two parts (a plate and a cylinder) are created independently in some part design system. Then an assembly is constructed by instantiating these parts with some user-defined mutual placements (probably driven by assembly constraints). In this example, the root assembly item "MyAssembly" contains the following components:

* "Plate" as the only instance of the plate part.
* "Cylinders" as the subassembly which includes three instances of a cylinder part.

A CAD part in a design system has usually no link to other parts in a product unless it is dropped to an assembly structure. To become an assembly component, the part has to be instantiated. An instance is technically a reference to the part with a proper location. The distinction between *parts* and *instances* is one of the fundamental principles in assembly design. A single part may have multiple instances all sharing the same product information (geometry and metadata). It is also valid to associate metadata with the instances themselves, though the latter is not very common (and not fully supported in OCCT).

\note The important point is the distinction between parts and instances. A part is a single product which may several times occur in an assembly. An instance is a particular occurrence of a part in an assembly.

@subsection occt_xde_1_2 XDE Organization

@subsubsection occt_xde_1_2_1 Assemblies

Assembly components are represented as nodes in a directed graph. The arcs of the graph represent "part-of" relations between the components (read like "component A is part of component B"). The following figure illustrates a conventional assembly from \ref occt_xde_references_cax_if "[CAx-IF STEP Library]" composed of five unique parts.

@figure{/user_guides/xde/images/xde_as1.png,"Sample assembly.",240}

Each part is defined in its local coordinate system and dropped to an assembly with a suitable placement. As such, the bolt has six instances, while a nut is instantiated eight times.

@figure{/user_guides/xde/images/xde_as1_parts.png,"Parts of sample assembly.",240}

The "part-of" graph is sketched in the following figure.

@figure{/user_guides/xde/images/xde_as1_part_of_graph.png,"\"Part-of\" assembly graph.",240}

There are as many component occurrences in an assembly as many paths exist to each leaf from the root nodes. Starting from the root node "as1", 18 unique ways are giving the total number of part occurrences in the component structure.

Technically, each node of the graph is represented by a label in the underlying OCAF structure. However, an OCAF label (TDF_Label) cannot be used as an identifier of an assembly item. In general case, one label can be reached from several "directions" of traversal. Thus, to address a single component occurrence unambiguously, one should use a full path to the occurrence in a component graph as an identifier. The following figure illustrates the result of assembly graph traversal which can be represented as a user-friendly tree (scene graph).

@figure{/user_guides/xde/images/xde_as1_part_of_tree.png,"\"Part-of\" assembly tree.",240}

Such a tree stands for the hierarchy of the components which can be shown to the user. A child-parent relation has "part-of" meaning. Notice that an individual node in the assembly tree is associated with a unique path to the item in the component graph. The following figure illustrates a simple bracket subassembly which is used twice as a component in the sample model.

@figure{/user_guides/xde/images/xde_as1_bracket_subassembly.png,"Bracket subassembly instantiated twice in sample model.",240}

The following figure outlines the component graph for this subassembly.

@figure{/user_guides/xde/images/xde_as1_bracket_subassembly_graph.png,"\"Part-of\" graph for bracket subassembly.",240}

Assembly structure is not directly related to any geometry. Therefore, assembly structure gives only a conceptual view of a product. According to \ref occt_xde_references_ShahMantyla95 "[Shah and Mantyla 1995]" (p. 9), many design tasks are not related to geometry at all, but to other characteristics of a product. Some problems involve idealized or simplified geometry, and it often happens that a detailed geometry is not even required (at least during some design phases). At the same time, the geometric model for a subassembly in XDE can be directly accessed at any time in the form of *TopoDS_Compound*.

XDE defines the specific organization of the assembly content. All elements are stored at sub-labels of label 0:1:1. There can be one or more roots. The hierarchy of labels is always two-levels nested. The first level (direct children of 0:1:1) declares all products within an assembly. The second level determines the "part-of" relations between the assembly elements.

@figure{/user_guides/xde/images/xde_assembly_ocaf.png,"Internal (OCAF) representation of XDE document.",240}

@subsubsection occt_xde_1_2_2 How to explore assembly structure

To explore the assembly structure, you may use a specialized *XCAFDoc_AssemblyGraph* tool. The following code snippet illustrates the construction of explicit assembly graph from XDE document.

\code
  Handle(TDocStd_Document) doc = ...; // Your XDE document

  // Prepare assembly graph
  Handle(XCAFDoc_AssemblyGraph) asmGraph = new XCAFDoc_AssemblyGraph(doc, true);
\endcode

*XCAFDoc_AssemblyGraph* does not contain geometry of the assembly being explored. It is a formal graph structure which makes explicit the "part-of" relations between the assembly components. Each node in a graph is associated with one of the following types:

* **Root:**
* **Subassembly**
* **Part occurrence**
* **Part**

A node contains a persistent ID which points to the corresponding OCAF label. The persistent ID is a key to access geometric representations and metadata associated with the corresponding element of assembly.

@subsubsection occt_xde_1_2_3 Multiple component occurrences

Consider two parts: wheel and axle shown in the picture below.

@figure{/user_guides/xde/images/assm_wheel_axle.png,"Two parts: wheel and axle.",240}

Consider a wheel-axle subassembly composed of one axle and two wheels.

@figure{/user_guides/xde/images/assm_wheel_axle_sa.png,"Subassembly composed of two wheels and one axle.",240}

A higher-level chassis assembly is in turn composed of two wheel-axle subassemblies, the front, and the rear.

@figure{/user_guides/xde/images/assm_wheel_axle_root.png,"Chassis assembly with multiple component occurrences.",240}

There is a requirement to identify individual components such as the left-front wheel. However, a wheel part has only two instances in the owner wheel-axle subassembly. The necessity to uniquely identify all component occurrences requires introducing a *transient* key for the corresponding assembly item. This key is essentially a path in the component graph which unambiguously addresses the element in question.

\note As reported by \ref occt_xde_references_Ungerer02 "[Ungerer and Buchanan 2002]", STEP PDM Schema is capable of tracking specific usages of assembly components. However, OCCT makes no assumption on the used STEP schema, so multiple component occurrences should be implemented at the level of XDE-based application.

\todo mention Grab Instance

//////////////////////////////////////////////////////////

Basic elements used by XDE are introduced in the XCAF sub-module by the package XCAFDoc. These elements consist in descriptions of commonly used data structures (apart from the shapes themselves) in normalized data exchanges. They are not attached to specific applications and do not bring specific semantics, but are structured according to the use and needs of data exchanges. 
The Document used by XDE usually starts as a *TDocStd_Document*. 

@subsection occt_xde_1_3 Assemblies
XDE supports assemblies by separating shape definitions and their locations. Shapes are simple OCAF objects without a location definition. An assembly consists of several components. Each of these components references one and the same specified shape with different locations. All this provides an increased flexibility in working on multi-level assemblies. 

For example, a mechanical assembly can be defined as follows: 
@figure{/user_guides/xde/images/xde_image003.png,"Assembly Description",240}


@figure{/user_guides/xde/images/xde_image004.png,"Assembly View",240}




@subsection occt_xde_1_4 Validation Properties
Validation properties are geometric characteristics of Shapes (volume, centroid, surface area) written to STEP files by the sending system. These characteristics are read by the receiving system to validate the quality of the translation. This is done by comparing the values computed by the original system with the same values computed by the receiving system on the resulting model. 

Advanced Data Exchange supports both reading and writing of validation properties, and provides a tool to check them. 

@figure{/user_guides/xde/images/xde_image005.png,"Validation Property Descriptions",240}

Check logs contain deviations of computed values from the values stored in a STEP file. A typical example appears as follows: 

| Label	| Area defect	| Volume defect	| dX	| dY	| DZ	| Name |
| :---- | :----- | :----- | :----- | :---- | :---- | :---- |
| 0:1:1:1 | 312.6 (0%) | -181.7 (0%) | 0.00 | 0.00 | 0.00	| "S1" |
| 0:1:1:2 |  -4.6 (0%) | -191.2 (0%)	| -0.00	 | 0.00	| -0.00 | "MAINBODY" |
| 0:1:1:3 | -2.3 (0%) | -52.5 (0%)	| -0.00	| 0.00	| 0.00 | "MAIN_BODY_BACK" |
| 0:1:1:4 | -2.3 (0%) | -51.6 (0%) | 0.00 |	0.00 | -0.00 | "MAIN_BODY_FRONT" |
| 0:1:1:5 | 2.0 (0%) | 10.0 (0%) | -0.00 |	0.00 |	-0.00 |	"HEAD" |
| 0:1:1:6 | 0.4 (0%) | 0.0 (0%) |	0.00	| -0.00	| -0.00	| "HEAD_FRONT" |
| 0:1:1:7 |  0.4 (0%) |	0.0 (0%) | 0.00 | -0.00 | -0.00	| "HEAD_BACK" |
| 0:1:1:8 | -320.6 (0%)	| 10.9 (0%)	| -0.00 | 0.00 | 0.00 |	"TAIL" |
| 0:1:1:9 | 0.0 (0%) | 0.0 (0%) | -0.00	| -0.00 | 0.00 | "TAIL_MIDDLE" |
| 0:1:1:10 | -186.2 (0%) | 	4.8 (0%) | 	-0.00 |	0.00 | -0.00 | "TAIL_TURBINE" |
| 0:1:1:11 | 0.3 (0%) |	-0.0 (0%) |	-0.00 | -0.00 |	0.00 |	"FOOT" |
| 0:1:1:12	| 0.0 (0%) | -0.0 (0%) | 0.00 |	-0.00 | -0.00 |"FOOT_FRONT" |
| 0:1:1:13 | 0.0 (0%) |	0.0 (0%) | -0.00 | 0.00	| 0.00 | "FOOT_BACK" |


In our example, it can be seen that no errors were detected for either area, volume or positioning data. 

@subsection occt_xde_1_5 Names

XDE supports reading and writing the names of shapes to and from IGES and STEP file formats. This functionality can be switched off if you do not need this type of data, thereby reducing the size of the document. 

@figure{/user_guides/xde/images/614_xde_04_400.png,"Instance Names",360}

@subsection occt_xde_1_6 Colors and Layers
XDE can read and write colors and layers assigned to shapes or their subparts (down to the level of faces and edges) to and from both IGES and STEP formats. Three types of colors are defined in the enumeration *XCAFDoc_ColorType*: 
  * generic color <i>(XCAFDoc_ColorGen)</i>
  * surface color <i>(XCAFDoc_ColorSurf)</i>
  * curve color <i>(XCAFDoc_ColorCurv)</i>
  
 @figure{/user_guides/xde/images/xde_image006.png,"Colors and Layers",240}


@section occt_xde_2 Working with XDE

@subsection occt_xde_2_1 Getting started

As explained in the last chapter, XDE uses *TDocStd_Documents* as a starting point. The general purpose of XDE is: 
  * Checking if an existing document is fit for XDE;
  * Getting an application and initialized document;
  * Initializing a document to fit it for XDE;
  * Adding, setting and finding data;
  * Querying and managing shapes;
  * Attaching properties to shapes.
  
The Document used by XDE usually starts as a *TDocStd_Document*. 

@subsubsection occt_xde_2_1_1 Environment variables
To use XDE you have to set the environment variables properly. Make sure that two important environment variables are set as follows: 
  * *CSF_PluginDefaults* points to sources of  <i>\%CASROOT%/src/XCAFResources ($CASROOT/src/XCAFResources)</i>.
  * *CSF_XCAFDefaults* points to sources of <i>\%CASROOT%/src/XCAFResources ($CASROOT/src/XCAFResources)</i>.

@subsubsection occt_xde_2_1_2 General Check
Before working with shapes, properties, and other types of information, the global organization of an XDE Document can be queried or completed to determine if an existing Document is actually structured for use with XDE. 

To find out if an existing *TDocStd_Document* is suitable for XDE, use: 
~~~~~
Handle(TDocStd_Document) doc... 
if ( XCAFDoc_DocumentTool::IsXCAFDocument (doc) ) { .. yes .. } 
~~~~~
If the Document is suitable for XDE, you can perform operations and queries explained in this guide. However, if a Document is not fully structured for XDE, it must be initialized. 

@subsubsection occt_xde_2_1_3 Get an Application or an Initialized Document
If you want to retrieve an existing application or an existing document (known to be correctly structured for XDE), use: 
~~~~~
Handle(TDocStd_Document) aDoc; 
Handle(XCAFApp_Application) anApp = XCAFApp_Application::GetApplication(); 
anApp->NewDocument(;MDTV-XCAF;,aDoc); 
~~~~~

@subsection occt_xde_2_2 Shapes and Assemblies

@subsubsection occt_xde_2_2_1 Initialize an XDE Document (Shapes)
An XDE Document begins with a *TDocStd_Document*. Assuming you have a *TDocStd_Document* already created, you can ensure that it is correctly structured for XDE by initializing the XDE structure as follows: 
~~~~~
Handle(TDocStd_Document) doc... 
Handle (XCAFDoc_ShapeTool) myAssembly = 
XCAFDoc_DocumentTool::ShapeTool (Doc->Main()); 
TDF_Label aLabel = myAssembly->NewShape() 
~~~~~
**Note** that the method *XCAFDoc_DocumentTool::ShapeTool* returns the *XCAFDoc_ShapeTool*. The first time this method is used, it creates the *XCAFDoc_ShapeTool*. In our example, a handle is used for the *TDocStd_Document*.

@subsubsection occt_xde_2_2_2 Get a Node considered as an Assembly
To get a node considered as an Assembly from an XDE structure, you can use the Label of the node. Assuming that you have a properly initialized *TDocStd_Document*, use: 
~~~~~
Handle(TDocStd_Document) doc... 
Handle(XCAFDoc_ShapeTool) myAssembly = XCAFDoc_DocumentTool::ShapeTool (aLabel); 
~~~~~
In the previous example, you can also get the Main Item of an XDE document, which records the root shape representation (as a Compound if it is an Assembly) by using *ShapeTool(Doc->Main())* instead of *ShapeTool(aLabel)*. 

You can then query or edit this Assembly node, the Main Item or another one (*myAssembly* in our examples). 

**Note** that for the examples in the rest of this guide, *myAssembly* is always presumed to be accessed this way, so this information will not be repeated.

@subsubsection occt_xde_2_2_3 Updating the Assemblies after Filling or Editing
Some actions in this chapter affect the content of the document, considered as an Assembly. As a result, you will sometimes need to update various representations (including the compounds). 

To update the representations, use: 
~~~~~
myAssembly->UpdateAssemblies(); 
~~~~~
This call performs a top-down update of the Assembly compounds stored in the document.

**Note** that you have to run this method manually to actualize your Assemblies after any low-level modifications on shapes.

@subsubsection occt_xde_2_2_4 Adding or Setting Top Level Shapes

Shapes can be added as top-level shapes. Top level means that they can be added to an upper level assembly or added on their own at the highest level as a component or referred by a located instance. Therefore two types of top-level shapes can be added: 
  * shapes with upper level references
  * free shapes (that correspond to roots) without any upper reference

**Note** that several top-level shapes can be added to the same component.
 
A shape to be added can be defined as a compound (if required), with the following interpretations: 
  * If the Shape is a compound, according to the user choice, it may or may not be interpreted as representing an Assembly. If it is an Assembly, each of its sub-shapes defines a sub-label. 
  * If the Shape is not a compound, it is taken as a whole, without breaking it down. 
  
To break down a Compound in the assembly structure, use: 
~~~~~
Standard_Boolean makeAssembly; 
// True to interpret a Compound as an Assembly, 
// False to take it as a whole 
aLabel = myAssembly->AddShape(aShape, makeAssembly); 
~~~~~
Each node of the assembly therefore refers to its sub-shapes. 

Concerning located instances of sub-shapes, the corresponding shapes, (without location) appear at distinct sub-labels. They are referred to by a shape instance, which associates a location. 

@subsubsection occt_xde_2_2_5 Setting a given Shape at a given Label
A top-level shape can be changed. In this example, no interpretation of compound is performed: 
~~~~~
Standard_CString LabelString ...; 
// identifies the Label (form ;0:i:j...;) 
TDF_Label aLabel...; 
// A label must be present 
myAssembly->SetShape(aLabel, aShape); 
~~~~~

@subsubsection occt_xde_2_2_6 Getting a Shape from a Label
To get a shape from its Label from the top-level, use: 
~~~~~
TDF_Label aLabel...
// A label must be present
if (aLabel.IsNull()) { 
  // no such label : abandon
}
TopoDS_Shape aShape;
aShape = myAssembly->GetShape(aLabel);
if (aShape.IsNull()) {
  // this label is not for a Shape
}
~~~~~
**Note** that if the label corresponds to an assembly, the result is a compound. 

@subsubsection occt_xde_2_2_7 Getting a Label from a Shape
To get a Label, which is attached to a Shape from the top-level, use: 
~~~~~
Standard_Boolean findInstance = Standard_False; 
// (this is default value)
aLabel = myAssembly->FindShape(aShape [,findInstance]);
if (aLabel.IsNull()) { 
  // no label found for this shape
}
~~~~~
If *findInstance* is True, a search is made for the shape with the same location. If it is False (default value), a search is made among original, non-located shapes. 

@subsubsection occt_xde_2_2_8 Other Queries on a Label

Various other queries can be made from a Label within the Main Item of XDE: 
#### Main Shapes

To determine if a Shape is recorded (or not), use: 
~~~~~
if ( myAssembly->IsShape(aLabel) ) { .. yes .. } 
~~~~~

To determine if the shape is top-level, i.e. was added by the *AddShape* method, use: 
~~~~~
if ( myAssembly->IsTopLevel(aLabel) ) { .. yes .. } 
~~~~~

To get a list of top-level shapes added by the *AddShape* method, use: 
~~~~~
TDF_LabelSequence frshapes; 
myAssembly->GetShapes(frshapes); 
~~~~~

To get all free shapes at once if the list above has only one item, use: 
~~~~~
TopoDS_Shape result = myAssembly->GetShape(frshapes.Value(1)); 
~~~~~

If there is more than one item, you must create and fill a compound, use: 

~~~~~
TopoDS_Compound C; 
BRep_Builder B; 
B.MakeCompound(C); 
for(Standard_Integer i=1; i=frshapes.Length(); i++) { 
  TopoDS_Shape S = myAssembly->GetShape(frshapes.Value(i)); 
  B.Add(C,S); 
} 
~~~~~

In our example, the result is the compound C. 
To determine if a shape is a free shape (no reference or super-assembly), use: 

~~~~~
if ( myAssembly->IsFree(aLabel) ) { .. yes .. } 
~~~~~

To get a list of Free Shapes (roots), use: 

~~~~~
TDF_LabelSequence frshapes; 
myAssembly->GetFreeShapes(frshapes); 
~~~~~

To get the shapes, which use a given shape as a component, use: 
~~~~~
TDF_LabelSequence users; 
Standard_Integer nbusers = myAssembly->GetUsers(aLabel,users); 
~~~~~
The count of users is contained with *nbusers*. It contains 0 if there are no users. 

#### Assembly and Components
To determine if a label is attached to the main part or to a sub-part (component), use: 
~~~~~
if (myAssembly->IsComponent(aLabel)) { .. yes .. } 
~~~~~
To determine whether a label is a node of a (sub-) assembly or a simple shape, use: 
~~~~~
if ( myAssembly->IsAssembly(aLabel) ) { .. yes .. } 
~~~~~

If the label is a node of a (sub-) assembly, you can get the count of components, use: 
~~~~~
Standard_Boolean subchilds = Standard_False; //default 
Standard_Integer nbc = myAssembly->NbComponents (aLabel [,subchilds]); 
~~~~~

If *subchilds* is True, commands also consider sub-levels. By default, only level one is checked. 

To get component Labels themselves, use: 
~~~~~
Standard_Boolean subchilds = Standard_False; //default 
TDF_LabelSequence comps; 
Standard_Boolean isassembly = myAssembly->GetComponents 
(aLabel,comps[,subchilds]); 
~~~~~
@subsubsection occt_xde_2_2_9 Instances and References for Components
To determine if a label is a simple shape, use: 
~~~~~
if ( myAssembly->IsSimpleShape(aLabel) ) { .. yes .. } 
~~~~~
To determine if a label is a located reference to another one, use: 
~~~~~
if ( myAssembly->IsReference(aLabel) ) { .. yes .. } 
~~~~~
If the label is a located reference, you can get the location, use: 
~~~~~
TopLoc_Location loc = myAssembly->GetLocation (aLabel); 
~~~~~
To get the label of a referenced original shape (also tests if it is a reference), use: 
~~~~~
Standard_Boolean isref = myAssembly->GetReferredShape 
(aLabel, refLabel); 
~~~~~

**Note** *isref* returns False if *aLabel* is not for a reference. 

@subsection occt_xde_2_3 Editing Shapes
In addition to the previously described *AddShape* and *SetShape*, several shape edits are possible. 

To remove a Shape, and all its sub-labels, use: 
~~~~~
Standard_Boolean remsh = myAssembly->RemoveShape(aLabel); 
// remsh is returned True if done 
~~~~~
This operation will fail if the shape is neither free nor top level.

To add a Component to the Assembly, from a new shape, use: 
~~~~~
Standard_Boolean expand = Standard_False; //default 
TDF_Label aLabel = myAssembly->AddComponent (aShape [,expand]); 
~~~~~
If *expand* is True and *aShape* is a Compound, *aShape* is broken down to produce sub-components, one for each of its sub-shapes. 

To add a component to the assembly, from a previously recorded shape (the new component is defined by the label of the reference shape, and its location), use: 
~~~~~
TDF_Label refLabel ...; // the label of reference shape 
TopLoc_Location loc ...; // the desired location 
TDF_Label aLabel = myAssembly->AddComponent (refLabel, loc); 
~~~~~
To remove a component from the assembly, use: 
~~~~~
myAssembly->RemoveComponent (aLabel); 
~~~~~

@subsection occt_xde_2_4 Management of Sub-Shapes
In addition to components of a (sub-)assembly, it is possible to have individual identification of some sub-shapes inside any shape. Therefore, you can attach specific attributes such as Colors. Some additional actions can be performed on sub-shapes that are neither top-level, nor components: 
To add a sub-shape to a given Label, use: 
~~~~~
TDF_Label subLabel = myAssembly->AddSubShape (aLabel, subShape); 
~~~~~

To find the Label attached to a given sub-shape, use: 
~~~~~
TDF_Label subLabel; // new label to be computed 
if ( myAssembly-> FindSubShape (aLabel, subShape, subLabel)) { .. yes .. } 
~~~~~
If the sub-shape is found (yes), *subLabel* is filled by the correct value. 

To find the top-level simple shape (not a compound whether free or not), which contains a given sub-shape, use: 
~~~~~
TDF_Label mainLabel = myAssembly->FindMainShape(subShape); 
~~~~~
**Note** that there should be only one shape for a valid model. In any case, the search stops on the first one found. 

To get the sub-shapes of a shape, which are recorded under a label, use: 
~~~~~
TDF_LabelSequence subs; 
Standard_Boolean hassubs = myAssembly->GetSubShapes (aLabel,subs); 
~~~~~
@subsection occt_xde_2_5 Properties
Some properties can be attached directly to shapes. These properties are: 
  * Name (standard definition from OCAF)
  * Centroid (for validation of transfer)
  * Volume (for validation of transfer)
  * Area (for validation of transfer)
Some other properties can also be attached, and are also managed by distinct tools for Colors and Layers. Colors and Layers are managed as an alternative way of organizing data (by providing a way of identifying groups of shapes). 
Colors are put into a table of colors while shapes refer to this table. There are two ways of attaching a color to a shape: 
  * By attaching an item from the table.
  * Adding the color directly.
When the color is added directly, a search is performed in the table of contents to determine if it contains the requested color. Once this search and initialize operation is done, the first way of attaching a color to a shape is used. 
@subsubsection occt_xde_2_5_1 Name
Name is implemented and used as a *TDataStd_Name*, which can be attached to any label. Before proceeding, consider that: 
  * In IGES, every entity can have a name with an optional numeric part called a Subscript Label. For example, *MYCURVE* is a name, and *MYCURVE(60)* is a name with a Subscript Label.
  * In STEP, there are two levels: Part Names and Entity Names: 
		* Part Names are attached to ;main shapes; such as parts and assemblies. These Part Names are specifically supported by XDE. 
		* Entity Names can be attached to every Geometric Entity. This option is rarely used, as it tends to overload the exploitation of the data structure. Only some specific cases justify using this option: for example, when the sending system can really ensure the stability of an entity name after each STEP writing. If such stability is ensured, you can use this option to send an Identifier for external applications using a database. 
**Note** that both IGES or STEP files handle names as pure ASCII strings.
 
These considerations are not specific to XDE. What is specific to data exchange is the way names are attached to entities. 

To get the name attached to a label (as a reminder using OCAF), use: 
~~~~~
Handle(TDataStd_Name) N; 
if ( !aLabel.FindAttribute(TDataStd_Name::GetID(),N)) { 
  // no name is attached 
} 
TCollection_ExtendedString name = N->Get(); 
~~~~~

Don't forget to consider Extended String as ASCII, for the exchange file. 

To set a name to a label (as a reminder using OCAF), use: 
~~~~~
TCollection_ExtendedString aName ...; 
// contains the desired name for this Label (ASCII) 
TDataStd_Name::Set (aLabel, aName); 
~~~~~

@subsubsection occt_xde_2_5_2 Centroid
A Centroid is defined by a Point to fix its position. It is handled as a property, item of the class *XCAFDoc_Centroid*, sub-class of *TDF_Attribute*. However, global methods give access to the position itself. 

This notion has been introduced in STEP, together with that of Volume, and Area, as defining the Validation Properties: this feature allows exchanging the geometries and some basic attached values, in order to perform a synthetic checking on how they are maintained after reading and converting the exchange file. This exchange depends on reliable exchanges of Geometry and Topology. Otherwise, these values can be considered irrelevant. 

A centroid can be determined at any level of an assembly, thereby allowing a check of both individual simple shapes and their combinations including locations. 

To get a Centroid attached to a Shape, use: 
~~~~~
gp_Pnt pos; 
Handle(XCAFDoc_Centroid) C; 
aLabel.FindAttribute ( XCAFDoc_Centroid::GetID(), C ); 
if ( !C.IsNull() ) pos = C->Get(); 
~~~~~

To set a Centroid to a Shape, use: 
~~~~~
gp_Pnt pos (X,Y,Z); 
// the position previously computed for the centroid 
XCAFDoc_Centroid::Set ( aLabel, pos ); 
~~~~~

@subsubsection occt_xde_2_5_3 Area
An Area is defined by a Real, it corresponds to the computed Area of a Shape, provided that it contains surfaces. It is handled as a property, item of the class *XCAFDoc_Area*, sub-class of *TDF_Attribute*. 
This notion has been introduced in STEP but it is usually disregarded for a Solid, as Volume is used instead. In addition, it is attached to simple shapes, not to assemblies. 

To get an area attached to a Shape, use: 
~~~~~
Standard_Real area; 
Handle(XCAFDoc_Area) A; 
L.FindAttribute ( XCAFDoc_Area::GetID(), A ); 
if ( !A.IsNull() ) area = A->Get(); 

To set an area value to a Shape, use: 
Standard_Real area ...; 
// value previously computed for the area 
XCAFDoc_Area::Set ( aLabel, area ); 
~~~~~
@subsubsection occt_xde_2_5_4 Volume
A Volume is defined by a Real and corresponds to the computed volume of a Shape, provided that it contains solids. It is handled as a property, an item of the class *XCAFDoc_Volume*, sub-class of *TDF_Attribute*. 
This notion has been introduced in STEP. It may be attached to simple shapes or their assemblies for computing cumulated volumes and centers of gravity. 

To get a Volume attached to a Shape, use: 
~~~~~
Standard_Real volume; 
Handle(XCAFDoc_Volume) V; 
L.FindAttribute ( XCAFDoc_Volume::GetID(), V ); 
if ( !V.IsNull() ) volume = V->Get(); 
~~~~~

To set a volume value to a Shape, use: 
~~~~~
Standard_Real volume ...; 
// value previously computed for the volume 
XCAFDoc_Volume::Set ( aLabel, volume ); 
~~~~~
@subsection occt_xde_2_6 Colors and Layers

XDE can read and write colors and layers assigned to shapes or their subparts (down to level of faces and edges) to and from both IGES and STEP formats. 

@figure{/user_guides/xde/images/239_xde_12_400.png,"Motor Head",240}

In an XDE document, colors are managed by the class *XCAFDoc_ColorTool*. This is done with the same principles as for ShapeTool with Shapes, and with the same capability of having a tool on the Main Label, or on any sub-label. The Property itself is defined as an *XCAFDoc_Color*, sub-class of *TDF_Attribute*.
 
Colors are stored in a child of the starting document label: it is the second level (0.1.2), while Shapes are at the first level. Each color then corresponds to a dedicated label, the property itself is a Quantity_Color, which has a name and value for Red, Green, Blue. A Color may be attached to Surfaces (flat colors) or to Curves (wireframe colors), or to both. A Color may be attached to a sub-shape. In such a case, the sub-shape (and its own sub-shapes) takes its own Color as a priority.

Layers are handled using the same principles as Colors. In all operations described below you can simply replace **Color** with **Layer** when dealing with Layers. Layers are supported by the class *XCAFDoc_LayerTool*. 

The class of the property is *XCAFDoc_Layer*, sub-class of *TDF_Attribute* while its definition is a *TCollection_ExtendedString*. Integers are generally used when dealing with Layers. The general cases are: 
  * IGES has *LevelList* as a list of Layer Numbers (not often used)
  * STEP identifies a Layer (not by a Number, but by a String), to be more general.

Colors and Shapes are related to by Tree Nodes. 

These definitions are common to various exchange formats, at least for STEP and IGES. 

@subsubsection occt_xde_2_6_1 Initialization
To query, edit, or initialize a Document to handle Colors of XCAF, use: 
~~~~~
Handle(XCAFDoc_ColorTool) myColors = 
XCAFDoc_DocumentTool::ColorTool(Doc->Main ()); 
~~~~~
This call can be used at any time. The first time it is used, a relevant structure is added to the document. This definition is used for all the following color calls and will not be repeated for these. 

@subsubsection occt_xde_2_6_2 Adding a Color
There are two ways to add a color. You can: 
  * add a new Color defined as *Quantity_Color* and then directly set it to a Shape (anonymous Color)
  * define a new Property Color, add it to the list of Colors, and then set it to various shapes.
When the Color is added by its value *Quantity_Color*, it is added only if it has not yet been recorded (same RGB values) in the Document. 

To set a Color to a Shape using a label, use:
~~~~~
Quantity_Color Col (red,green,blue); 
XCAFDoc_ColorType ctype ..; 
// can take one of these values : 
// XCAFDoc_ColorGen : all types of geometries 
// XCAFDoc_ColorSurf : surfaces only 
// XCAFDoc_ColorCurv : curves only 
myColors->SetColor ( aLabel, Col, ctype ); 
~~~~~
Alternately, the Shape can be designated directly, without using its label, use: 
~~~~~
myColors->SetColor ( aShape, Col, ctype ); 
// Creating and Adding a Color, explicitly 
Quantity_Color Col (red,green,blue); 
TDF_Label ColLabel = myColors->AddColor ( Col ); 
~~~~~
**Note** that this Color can then be named, allowing later retrieval by its Name instead of its Value. 

To set a Color, identified by its Label and already recorded, to a Shape, use: 
~~~~~
XCAFDoc_ColorType ctype ..; // see above
if ( myColors->SetColors ( aLabel, ColLabel, ctype) ) {.. it is done .. }
~~~~~
In this example, *aLabel* can be replaced by *aShape* directly. 

@subsubsection occt_xde_2_6_3 Queries on Colors
Various queries can be performed on colors. However, only specific queries are included in this section, not general queries using names. 

To determine if a Color is attached to a Shape, for a given color type (ctype), use: 
~~~~~
if ( myColors->IsSet (aLabel , ctype)) { 
  // yes, there is one .. 
} 
~~~~~
In this example, *aLabel* can be replaced by *aShape* directly. 

To get the Color attached to a Shape (for any color type), use: 
~~~~~
Quantity_Color col; 
// will receive the recorded value (if there is some)
if ( !myColors->GetColor(aLabel, col) ) { 
// sorry, no color .. 
}
~~~~~

Color name can also be queried from *col.StringName* or *col.Name*. 
In this example, *aLabel* can be replaced by *aShape* directly. 

To get the Color attached to a Shape, with a specific color type, use: 
~~~~~
XCAFDoc_ColorType ctype ..; 
Quantity_Color col; 
// will receive the recorded value (if there is some) 
if ( !myColors->GetColor(aLabel, ctype, col) ) { 
// sorry, no color .. 
} 
~~~~~


To get all the Colors recorded in the Document, use: 

~~~~~
Quantity_Color col; // to receive the values 
TDF_LabelSequence ColLabels; 
myColors->GetColors(ColLabels); 
Standard_Integer i, nbc = ColLabels.Length(); 
for (i = 1; i = nbc; i ++) { 
  aLabel = Labels.Value(i); 
  if ( !myColors->GetColor(aLabel, col) ) continue; 
  // col receives the color n0 i .. 
} 
~~~~~

To find a Color from its Value, use: 
~~~~~
Quantity_Color Col (red,green,blue); 
TDF_Label ColLabel = myColors-FindColor (Col); 
if ( !ColLabel.IsNull() ) { .. found .. } 
~~~~~

@subsubsection occt_xde_2_6_4 Editing Colors
Besides adding colors, the following attribute edits can be made: 

To unset a Color on a Shape, use: 
~~~~~
XCAFDoc_ColorType ctype ...; 
// desired type (XCAFDoc_ColorGen for all ) 
myColors->UnSetColor (aLabel,ctype); 
~~~~~
To remove a Color and all the references to it (so that the related shapes will become colorless), use: 
~~~~~
myColors->RemoveColor(ColLabel); 
~~~~~

  
@subsection occt_xde_2_8 Reading and Writing STEP or IGES
Note that saving and restoring the document itself are standard OCAF operations. As the various previously described definitions enter into this frame, they will not be explained any further. 
The same can be said for Viewing: presentations can be defined from Shapes and Colors. 

There are several important points to consider: 
  * Previously defined Readers and Writers for dealing with Shapes only, whether Standard or Advanced, remain unchanged in their form and in their dependencies. In addition, functions other than mapping are also unchanged.
  * XDE provides mapping with data other than Shapes. Names, Colors, Layers, Validation Properties (Centroid, Volume, Area), and Assembly Structure are hierarchic with rigid motion.
  * XDE mapping is relevant for use within the Advanced level of Data Exchanges, rather than Standard ones, because a higher level of information is better suited to a higher quality of shapes. In addition, this allows to avoid the multiplicity of combinations between various options. Note that this choice is not one of architecture but of practical usage and packaging.
  * Reader and Writer classes for XDE are generally used like those for Shapes. However, their use is adapted to manage a Document rather than a Shape.
  
The packages to manage this are *IGESCAFControl* for IGES, and *STEPCAFControl* for STEP. 
@subsubsection occt_xde_2_8_1 Reading a STEP file
To read a STEP file by itself, use: 

~~~~~
STEPCAFControl_Reader reader; 
IFSelect_ReturnStatus readstat = reader.ReadFile(filename); 
// The various ways of reading a file are available here too : 
// to read it by the reader, to take it from a WorkSession ... 
Handle(TDocStd_Document) doc... 
// the document referred to is already defined and 
// properly initialized. 
// Now, the transfer itself 
if ( !reader.Transfer ( doc ) ) { 
  cout;Cannot read any relevant data from the STEP file;endl; 
  // abandon .. 
} 
// Here, the Document has been filled from a STEP file, 
// it is ready to use 
~~~~~

In addition, the reader provides methods that are applicable to document transfers and for directly querying of the data produced. 
@subsubsection occt_xde_2_8_2 Writing a STEP file
To write a STEP file by itself, use: 

~~~~~
STEPControl_StepModelType mode = 
STEPControl_AsIs; 
// Asis is the recommended value, others are available 
// Firstly, perform the conversion to STEP entities 
STEPCAFControl_Writer writer; 
//(the user can work with an already prepared WorkSession or create a //new one) 
Standard_Boolean scratch = Standard_False; 
STEPCAFControl_Writer writer ( WS, scratch ); 
// Translating document (conversion) to STEP 
if ( ! writer.Transfer ( Doc, mode ) ) { 
  cout;The document cannot be translated or gives no result;endl; 
  // abandon .. 
} 
// Writing the File 
IFSelect_ReturnStatus stat = writer.Write(file-name); 
~~~~~

@subsubsection occt_xde_2_8_3 Reading an IGES File
Use the same procedure as for a STEP file but with IGESCAFControl instead of STEPCAFControl. 
@subsubsection occt_xde_2_8_4 Writing an IGES File
Use the same procedure as for a STEP file but with IGESCAFControl instead of STEPCAFControl.
 
@subsection occt_xde_2_9 Using an XDE Document
There are several ways of exploiting XDE data from an application, you can: 
 1. Get the data relevant for the application by mapping XDE/Appli, then discard the XDE data once it has been used.
 2. Create a reference from the Application Document to the XDE Document, to have its data available as external data.
 3. Embed XDE data inside the Application Document (see the following section for details).
 4. Directly exploit XDE data such as when using file checkers.
@subsubsection occt_xde_2_91 XDE Data inside an Application Document
To have XCAF data elsewhere than under label 0.1, you use the DocLabel of XDE. The method DocLabel from XCAFDoc_DocumentTool determines the relevant Label for XCAF. However, note that the default is 0.1. 

In addition, as XDE data is defined and managed in a modular way, you can consider exclusively Assembly Structure, only Colors, and so on. 

As XDE provides an extension of the data structure, for relevant data in standardized exchanges, note the following: 
  * This data structure is fitted for data exchange, rather than for use by the final application.
  * The provided definitions are general, for common use and therefore do not bring strongly specific semantics.
  
As a result, if an application works on Assemblies, on Colors or Layers, on Validation Properties (as defined in STEP), it can rely on all or a part of the XDE definitions, and include them in its own data structure. 

In addition, if an application has a data structure far from these notions, it can get data (such as Colors and Names on Shapes) according to its needs, but without having to consider the whole. 

@section occt_xde_references References

-# \anchor occt_xde_references_ShahMantyla95
  Shah J., Mantyla M. Parametric and Feature Based CAD/CAM. 1995.

-# \anchor occt_xde_references_cax_if
  CAx-IF STEP File Library // URL: https://cax-if.org/library/index.html

-# \anchor occt_xde_references_Ungerer02
  Ungerer, M. and Buchanan, K. 2002. Usage Guide for the STEP PDM Schema Release 4.3. PDM Implementor Forum, Darmstadt January.


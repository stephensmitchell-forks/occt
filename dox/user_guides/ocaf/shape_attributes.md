Shape Attributes {#user_guides__ocaf_shape_attribs}
================

@tableofcontents

A topological attribute can be seen as a hook into the topological structure. It is possible to attach data to define
 references to it.

OCAF shape attributes are used for topology objects and their evolution access. All topological objects are stored in 
one *TNaming_UsedShapes* attribute at the root label of the data framework. This attribute contains a map with all 
topological shapes used in a given document. 

The user can add the *TNaming_NamedShape* attribute to other labels. This attribute contains references (hooks) to
 shapes from the *TNaming_UsedShapes* attribute and an evolution of these shapes. The *TNaming_NamedShape* attribute
 contains a set of pairs of hooks: to the *Old* shape and to a *New* shape (see the following figure). It allows not 
only to get the topological shapes by the labels, but also to trace the evolution of the shapes and to correctly update
 dependent shapes by the changed one. 

If a shape is newly created, then the old shape of a corresponding named shape is an empty shape. If a shape is deleted, 
then the new shape in this named shape is empty. 

@image html /user_guides/ocaf/images/ocaf_image013.png
@image latex /user_guides/ocaf/images/ocaf_image013.png

@section occt_ocaf_5_2 Shape Attributes in Data Framework

Different algorithms may dispose sub-shapes of the result shape at the individual labels depending on whether it is 
necessary to do so: 

* If a sub-shape must have some extra attributes (material of each face or color of each edge). In this case a specific 
sub-shape is placed to a separate label (usually to a sub-label of the result shape label) with all attributes of this
 sub-shape.
* If the topological naming algorithm is needed, a necessary and sufficient set of sub-shapes is placed to child labels 
of the result shape label. As usual, for a basic solid and closed shells, all faces of the shape are disposed.

*TNaming_NamedShape* may contain a few pairs of hooks with the same evolution. In this case the topology shape, which 
belongs to the named shape is a compound of new shapes. 

Consider the following example. Two boxes (solids) are fused into one solid (the result one). Initially each box was
 placed to the result label as a named shape, which has evolution PRIMITIVE and refers to the corresponding shape of
 the *TNaming_UsedShapes* map. The box result label has a material attribute and six child labels containing named 
shapes of Box faces. 

@image html /user_guides/ocaf/images/ocaf_image014.png "Resulting box"
@image latex /user_guides/ocaf/images/ocaf_image014.png "Resulting box"

After the fuse operation a modified result is placed to a separate label as a named shape, which refers to the old shape 
(one of the boxes) and to the new shape resulting from the fuse operation, and has evolution MODIFY (see the following 
figure). 

Named shapes, which contain information about modified faces, belong to the fuse result sub-labels: 
* sub-label with tag 1 -- modified faces from box 1, 
* sub-label with tag 2 -- modified faces from box 2. 

@image html /user_guides/ocaf/images/ocaf_image015.png
@image latex /user_guides/ocaf/images/ocaf_image015.png

This is necessary and sufficient information for the functionality of the right naming mechanism: any sub-shape of the
 result can be identified unambiguously by name type and set of labels, which contain named shapes: 

  * face F1’ as a modification of face F11  
  * face F1’’ as generation of face F12 
  * edges as an intersection of two contiguous faces
  * vertices as an intersection of three contiguous faces

After any modification of source boxes the application must automatically rebuild the naming entities: recompute the 
named shapes of the boxes (solids and faces) and fuse the resulting named shapes (solids and faces) that reference to 
the new named shapes. 

@section occt_ocaf_5_3 Registering Shapes and Their Evolution

When using TNaming_NamedShape to create attributes, the following fields of an attribute are filled: 

* A list of shapes called the "old" and the "new" shapes A new shape is recomputed as the value of the named shape. The
 meaning of this pair depends on the type of evolution.
* The type of evolution, which is a term of the *TNaming_Evolution* enumeration used for the selected shapes that are 
placed to the separate label: 
	* PRIMITIVE -- newly created topology, with no previous history;
	* GENERATED -- as usual, this evolution of a  named shape means, that the new shape is created from a low-level old
 shape ( a prism face from an edge, for example );
	* MODIFY -- the new shape is a modified old shape;
	* DELETE -- the new shape is empty; the named shape with this evolution just indicates that the old shape topology
 is deleted from the model;
	* SELECTED -- a named shape with this evolution has no effect on the history of the topology.

Only pairs of shapes with equal evolution can be stored in one named shape. 

@section occt_ocaf_5_4 Using Naming Resources

The class *TNaming_Builder* allows creating a named shape attribute. It has a label of a future attribute as an argument
 of the constructor. Respective methods are used for the evolution and setting of shape pairs. If for the same
 TNaming_Builder object a lot of pairs of shapes with the same evolution are given, then these pairs would be placed in
 the resulting named shape. After the creation of a new object of the TNaming_Builder class, an empty named shape is 
created at the given label. 

~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~{.cpp}
// a new empty named shape is created at "label" 
TNaming_Builder builder(label); 
// set a pair of shapes with evolution GENERATED 
builder.Generated(oldshape1,newshape1); 
// set another pair of shapes with the same evolution 
builder.Generated(oldshape2,newshape2); 
// get the result - TNaming_NamedShape attribute 
Handle(TNaming_NamedShape) ns = builder.NamedShape(); 
~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

@section occt_ocaf_5_5 Reading Contents of Named Attribute

You can use the method <i>TNaming_NamedShape::Evolution()</i> to get the evolution of this named shape and the method 
<i>TNaming_NamedShape::Get()</i> to get a compound of new shapes of all pairs of this named shape.
  
More detailed information about the contents of the named shape or about the modification history of a topology can be
 obtained with the following: 
* *TNaming_Tool* provides a common high-level functionality for access to the named shapes contents:
	* The method <i>GetShape(Handle(TNaming_NamedShape)) </i>  returns a compound of new shapes of the given named shape;
	* The method <i>CurrentShape(Handle(TNaming_NamedShape))</i>  returns a compound of the shapes, which are latest 
versions of the shapes from the given named shape;
	* The method <i>NamedShape(TopoDS_Shape,TDF_Label) </i> returns a named shape, which contains a given shape as a new 
shape. A given label is any label from the data framework -- it just gives access to it.
* *TNaming_Iterator* gives access to the named shape and hooks pairs.

~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~{.cpp}
// create an iterator for a named shape 
TNaming_Iterator iter(namedshape); 
// iterate while some pairs are not iterated 
while(iter.More()) { 
// get the new shape from the current pair 
TopoDS_Shape newshape = iter.NewShape(); 
// get the old shape from the current pair 
TopoDS_Shape oldshape = iter.OldShape(); 
// do something... 

// go to the next pair 
iter.Next(); 
} 
~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~


@section occt_ocaf_5_6 Topological Naming

@subsection top_naming_intro Introduction
**Topological naming** is a mechanism of OCCT aimed to keep reference to the selected shape. If, for example, we select 
a vertex of a solid shape and “ask” the topological naming to keep reference to this vertex, it will refer to the vertex
 whatever happens with the shape (translations, scaling, fusion with another shape, etc.).

Let us consider an example: imagine a wooden plate. The job is to drive several nails in it:

@figure{/user_guides/ocaf/images/ocaf_image020.png, "A nail driven in a wooden plate"}

There may be several nails with different size and position. A **Hammer** should push each **Nail** exactly in the center 
point of the top surface. For this the user does the following:
*	Makes several Nails of different height and diameter (according to the need),
*	Chooses (selects) the upper surface of each Nail for the Hammer.

The job is done. The application should do the rest -- the Hammer calculates a center point for each selected surface of
 the Nail and “strikes” each Nail driving it into the wooden plate.

What happens if the user changes the position of some Nails? How will the Hammer know about it? It keeps reference to 
the surface of each Nail. However, if a Nail is relocated, the Hammer should know the new position of the selected 
surface. Otherwise, it will “strike” at the old position (keep the fingers away!)…

Topological naming mechanism should help the Hammer to obtain the relocated surfaces. The Hammer “asks” the mechanism 
to “resolve” the selected shapes by calling method *TNaming_Selection::Solve()* and the mechanism “returns” the modified
 surfaces located at the new position by calling  *TNaming_Selector::NamedShape()*.

The topological naming is represented as a “black box” in the example above. Now it is time to make the box a little
 more “transparent”.

The application contains 3 functions:
* **Nail** -- produces a shape representing a nail,
* **Translator** -- translates a shape along the wooden plate,
* **Hammer** -- drives the nail in the wooden plate.

Each function gives the topological naming some hints how to “re-solve” the selected sub-shapes:
* The Nail constructs a solid shape and puts each face of the shape into sub-labels: 

@figure{/user_guides/ocaf/images/ocaf_image021.png, "Distribution of faces through sub-labels of the Nail"}

* The **Translator** moves a shape and registers modification for each face: it puts a pair: “old” shape -- “new” shape at a sub-label of each moving Nail. The “old” shape represents a face of the Nail at the initial position. The “new” shape -- is the same face, but at a new position:

@figure{/user_guides/ocaf/images/ocaf_image022.png, "Registration of relocation of faces of a Nail"}

How does it work?
* The Hammer selects a face of a Nail calling *TNaming_Selector::Select()*. This call makes a unique name for the selected shape. In our example, it will be a direct reference to the label of the top face of the Nail (Face 1).
* When the user moves a Nail along the wooden plate, the Translator registers this modification by putting the pairs: “old” face of the Nail -- new face of the Nail into its sub-labels. 
* When the Hammer calls *TNaming::Solve()*, the topological naming “looks” at the unique name of the selected shape and tries to re-solve it:
	* It finds the 1st appearance of the selected shape in the data tree -- it is a label under the Nail function *Face 1*.
	* It follows the evolution of this face. In our case, there is only one evolution -- the translation: *Face 1* (top face) -- <i>Face 1’</i> (relocated top face). So, the last evolution is the relocated top face.
* Calling the method *TNaming_Selector::NamedShape()* the Hammer obtains the last evolution of the selected face -- the relocated top face.

The job is done.

P.S. Let us say a few words about a little more complicated case -- selection of a wire of the top face. Its topological name is an “intersection” of two faces. We remember that the **Nail** puts only faces under its label. So, the selected wire will represent an “intersection” of the top face and the conic face keeping the “head” of the nail. Another example is a selected vertex. Its unique name may be represented as an “intersection” of three or even more faces (depends on the shape).

The Topological Naming mechanism is based on 3 components:
* History of the used modeling operation algorithm;
* Registering of the built result in Data Framework (i.e. loading the necessary elements of the extracted history in OCAF document);
* Selection / Recomputation of a "selected" sub-shape of the algorithm result.

To get the expected result the work of the three components should be synchronized and the rules of each component should be respected.

@subsection top_naming_history Algorithm history

The "correct" history of a used modeling operation serves the basis of naming mechanism. It should be provided by the algorithm supporting the operation. The history content depends on the type of the topological result. The purpose of the history is to provide all entities for consistent and correct work of the Selection / Recomputation mechanism. The table below presents expected types of entities depending on the result type.

| Result type |	Type of sub-shapes to be returned by history of algorithm | Comments |
| :---------- | :-------------------------------------------------------- | :------- |
| Solid or closed shell	| Faces | All faces |
| Open shell or single face | Faces and edges of opened boundaries only	| All faces plus all edges of opened boundaries |
| Closed wire | Edges | All edges |
| Opened wire |	Edges and ending vertexes | All edges plus ending vertexes of the wire |
| Edge | Vertexes |	Two vertexes are expected |
| Compound or CompSolid	| To be used consequentially the above declared rule applied to all sub-shapes of the first level | Compound/CompSolid to be explored level by level until any the mentioned above types will be met | 

The history should return (and track) only elementary types of sub-shapes, i.e. Faces, Edges and Vertexes, while other so-called aggregation types: Compounds, Shells, Wires, are calculated by Selection mechanism automatically.

There are some simple exceptions for several cases. For example, if the Result contains a seam edge -- in conical, cylindrical or spherical surfaces -- this seam edge should be tracked by the history and in addition should be defined before the types. All degenerated entities should be filtered and excluded from consideration.

@subsection top_naming_loading Loading History in Data Framework

All elements returned by the used algorithm according to the aforementioned rules should be put in the Data Framework (or OCAF document in other words) consequently in linear order under the so-called **Result Label**. 

The "Result Label" is *TDF_label* used to keep the algorithm result *Shape* from *TopoDS* in *NamedShape* attribute. During loading sub-shapes of the result in Data Framework should be used the rules of chapter @ref occt_ocaf_5_3.  These rules are also applicable for loading the main shape, i.e. the resulting shape produced by the modeling algorithm.

@subsection top_naming_selection Selection / Re-computation Mechanism

When the Data Framework is filled with all impacted entities (including the data structures resulting from the current modeling operation and the data structures resulting from the previous modeling operations, on which the current operation depends) any sub-shape of the current result can be **selected**, i.e. the corresponding new naming data structures, which support this functionality, can be produced and kept in the Data Framework.

One of the user interfaces for topological naming is the class *TNaming_Selector*. It implements the above mentioned sub-shape "selection" functionality as an additional one. I.e. it can be used for:
* Storing the selected shape on a label -- its **Selection**;
* Accessing the named shape -- check the kept value of the shape
* Update of this naming -- recomputation of an earlier selected shape.

The selector places a new named shape with evolution **SELECTED** to the given label. The selector creates a **name** of the selected shape, which is a unique description (data structure) of how to find the selected topology using as resources:
* the given context shape, i.e. the main shape kept on **Result Label**, which contains a selected sub-shape, 
* its evolution and
* naming structure.

After any modification of a context shape and update of the corresponding naming structure, it is necessary to call method *TNaming_Selector::Solve*. If the naming structure, i.e. the above mentioned **name**, is correct, the selector automatically updates the selected sub-shape in the corresponding named shape, else it fails.

@section occt_ocaf_5_7 Exploring Shape Evolution

The class *TNaming_Tool* provides a toolkit to read current data contained in the attribute. 

If you need to create a topological attribute for existing data, use the method *NamedShape*. 

~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~{.cpp}
class MyPkg_MyClass 
{ 
public: Standard_Boolean SameEdge (const Handle(CafTest_Line)& L1, const Handle(CafTest_Line)& L2); 
}; 

Standard_Boolean CafTest_MyClass::SameEdge (const Handle(CafTest_Line)& L1, const Handle(CafTest_Line)& L2) 
{ 
  Handle(TNaming_NamedShape) NS1 = L1->NamedShape(); 
  Handle(TNaming_NamedShape) NS2 = L2->NamedShape(); 
  return BRepTools::Compare(NS1,NS2); 
} 
~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~



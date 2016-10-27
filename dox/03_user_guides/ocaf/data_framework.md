Data Framework {#user_guides__ocaf_data_framework}
========================

@tableofcontents

@section df_structure Data Structure

The OCAF Data Framework is the OCCT  realization   of the reference-key model in a tree structure. It offers a single 
environment where data from different application components can be handled. This allows exchanging and modifying data
 simply, consistently, with a maximum level of information and stable semantics.  

The building blocks of this approach are: 

  * Tag
  * Label
  * Attribute

As it has been mentioned earlier, the first label in a framework is the root label of the tree. Each label has a tag 
expressed as an integer value, and a label is uniquely defined by an entry expressed as a list of tags from the root,
 0:1:2:1, for example. 

Each label can have a list of attributes, which contain data, and several attributes can be attached to a label. Each 
attribute is identified by a GUID, and although a label may have several attributes attached to it, it must not have 
more than one attribute of a single GUID. 

The sub-labels of a label are called its children. Conversely, each label, which is not the root, has a father. Brother
 labels cannot share the same tag. 

The most important property is that a label’s entry is its persistent address in the data framework. 
  
@image html /03_user_guides/ocaf/images/ocaf_image005.png "A simple framework model"
@image latex /03_user_guides/ocaf/images/ocaf_image005.png "A simple framework model"

In this image the circles contain tags of the corresponding labels. The lists of tags are located under the circles. 
The root label always has a zero tag. 

The children of a root label are middle-level labels with tags 1 and 3. These labels are brothers. 

List of tags of the right-bottom label is "0:3:4": this label has tag 4, its father (with entry "0:3") has tag 3, father
 of father has tag 0 (the root label always has "0" entry). 

##Examples of a Data Structure

Let's have a look at the example:
  
@image html ocaf_wp_image007.png "The coffee machine"  
@image latex ocaf_wp_image007.png "The coffee machine"    
  
   In the image the application for designing coffee  machines first allocates 
  a label for the machine unit. It then adds sub-labels  for the main features 
  (glass coffee pot, water receptacle and filter) which it  refines as needed 
  (handle and reservoir of the coffee pot and spout of the  reservoir). 
  
  You now attach technical data describing the handle — its geometry and color — 
  and the reservoir — its geometry and material. 
  Later on, you can  modify the handle's geometry without changing its color — 
  both remain attached  to the same label.  
  
@image html ocaf_wp_image005.png "The data structure of the coffee machine"  
@image latex ocaf_wp_image005.png "The data structure of the coffee machine"  
 
  The nesting of labels is key to OCAF. This allows a  label to have its own structure 
  with its local addressing scheme which can be  reused in a more complex structure. 
  Take, for example, the coffee machine.  Given that the coffee pot's handle has a label of tag [1], 
  the entry for the handle in  the context of the coffee pot only (without the machine unit) is [0:1:1]. 
  If you now model a coffee  machine with two coffee pots, one at the label [1], 
  the second at the label [4] in the machine unit, 
  the handle  of the first pot would have the entry [0:1:1:1] 
  whereas the handle of the second pot would be [0:1:4:1]. 
  This way, we avoid any  confusion between coffee pot handles.

Another example is the application for designing table lamps. The first label is allocated to the lamp unit. 

@image html /03_user_guides/ocaf/images/ocaf_image006.png
@image latex /03_user_guides/ocaf/images/ocaf_image006.png

The root label cannot have brother labels. Consequently, various lamps in the framework allocation correspond to the
 sub-labels of the root label. This allows avoiding any confusion between table lamps in the data framework. Different
 lamp parts have different material, color and other attributes, so a child label of the lamp with the specified tags is
 allocated for each sub-unit of the lamp: 

  * a lamp-shade label with tag 1
  * a bulb label with tag 2
  * a stem label with tag 3

Label tags are chosen at will. They are only identifiers of the lamp parts. Now you can refine all units: by setting
 geometry, color, material and other information about the lamp or its parts to the specified label. This information
 is placed into special attributes of the label: the pure label contains no data -- it is only a key to access data. 

Remember that tags are private addresses without any meaning outside the data framework. It would, for instance, be an 
error to use part names as tags. These might change or be removed from production in next versions of the application, 
whereas the exact form of that part might be reused in your design, the part name could be integrated into the framework
 as an attribute. 

So, after the user changes the lamp design, only corresponding attributes are changed, but the label structure is 
maintained. The lamp shape must be recreated by new attribute values and attributes of the lamp shape must refer to a
 new shape. 

@image html /03_user_guides/ocaf/images/ocaf_image007.png
@image latex /03_user_guides/ocaf/images/ocaf_image007.png

The previous figure shows the table-lamps document structure: each child of the root label contains a lamp shape
 attribute and refers to the sub-labels, which contain some design information about corresponding sub-units. 

The data framework structure allows to create more complex structures: each lamp label sub-label may have children 
labels with more detailed information about parts of the table lamp and its components. 

Note that the root label can have attributes too, usually global attributes: the name of the document, for example. 

@subsection occt_ocaf_3_3 Tag

A tag is an integer, which identifies a label in two ways: 

  * Relative identification
  * Absolute identification.

In relative identification, a label’s tag has a meaning relative to the father label only. For a specific label, you 
might, for example, have four child labels identified by the tags 2, 7, 18, 100. In using relative identification, you 
ensure that you have a safe scope for setting attributes. 

In absolute identification, a label’s place in the data framework is specified unambiguously by a colon-separated list 
of tags of all the labels from the one in question to the root of the data framework. This list is called an entry.
 *TDF_Tool::TagList* allows retrieving the entry for a specific label. 

In both relative and absolute identification, it is important to remember that the value of an integer has no intrinsic 
semantics whatsoever. In other words, the natural sequence that integers suggest, i.e. 0, 1, 2, 3, 4 ... -- has no 
importance here. The integer value of a tag is simply a key. 

The tag can be created in two ways: 

  * Random delivery
  * User-defined delivery

As the names suggest, in random delivery, the tag value is generated by the system in a random manner. In user-defined 
delivery, you assign it by passing the tag as an argument to a method. 

### occt_ocaf_3_3_1 Creating child labels using random delivery of tags

To append and return a new child label, you use *TDF_TagSource::NewChild*. In the example below, the argument *level2*, 
which is passed to *NewChild,* is a *TDF_Label*. 


~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
TDF_Label child1 = TDF_TagSource::NewChild (level2); 
TDF_Label child2 = TDF_TagSource::NewChild (level2); 
~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

### occt_ocaf_3_3_2 Creation of a child label by user delivery from a tag

The other way to create a child label from a tag is by user delivery. In other words, you specify the tag, which you 
want your child label to have. 

To retrieve a child label from a tag which you have specified yourself, you need to use *TDF_Label::FindChild* and 
*TDF_Label::Tag* as in the example below. Here, the integer 3 designates the tag of the label you are interested in, and 
the Boolean false is the value for the argument *create*. When this argument is set to *false*, no new child label is
 created. 


~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
TDF_Label achild = root.FindChild(3,Standard_False); 
if (!achild.IsNull()) { 
Standard_Integer tag = achild.Tag(); 
} 
~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

@subsection occt_ocaf_3_4 Label

The tag gives a persistent address to a label. The label -- the semantics of the tag -- is a place in the data framework 
where attributes, which contain data, are attached. The data framework is, in fact, a tree of labels with a root as the 
ultimate father label.

Label can not be deleted from the data framework, so, the structure of the data framework that has been created can not 
be removed while the document is opened. Hence any kind of reference to an existing label will be actual while an 
application is working with the document. 

### Label creation

Labels can be created on any labels, compared with brother labels and retrieved. You can also find their depth in the 
data framework (depth of the root label is 0, depth of child labels of the root is 1 and so on), whether they have
 children or not, relative placement of labels, data framework of this label. The class *TDF_Label* offers the above
 services. 

### Creating child labels

To create a new child label in the data framework using explicit delivery of tags, use *TDF_Label::FindChild*. 

~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~{.cpp}
//creating a label with tag 10 at Root 
TDF_Label lab1 = aDF->Root().FindChild(10); 

//creating labels 7 and 2 on label 10 
TDF_Label lab2 = lab1.FindChild(7); 

TDF_Label lab3 = lab1.FindChild(2); 
~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
You could also use the same syntax but add the Boolean *true* as a value of the argument **create**. This ensures that a
 new child label will be created if none is found. Note that in the previous syntax, this was also the case since 
**create** is *true* by default. 


~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~{.cpp}
TDF_Label level1 = root.FindChild(3,Standard_True); 
TDF_Label level2 = level1.FindChild(1,Standard_True); 
~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
### Retrieving child labels

You can retrieve child labels of your current label by iteration on the first level in the scope of this label. 

~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~{.cpp}
TDF_Label current; 
// 
for (TDF_ChildIterator it1 (current,Standard_False); it1.More(); it1.Next()) { 
achild = it1.Value(); 
// 
// do something on a child (level 1) 
// 
} 
~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
You can also retrieve all child labels in every descendant generation of your current label by iteration on all levels
 in the scope of this label. 
~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~{.cpp}
for (TDF_ChildIterator itall (current,Standard_True); itall.More(); itall.Next()) { 
achild = itall.Value(); 
// 
// do something on a child (all levels) 
// 
} 
~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
Using *TDF_Tool::Entry* with *TDF_ChildIterator* you can retrieve the entries of your current label’s child labels as
 well. 

 
~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~{.cpp}
void DumpChildren(const TDF_Label& aLabel) 
{ 
  TDF_ChildIterator it; 
  TCollection_AsciiString es; 
  for (it.Initialize(aLabel,Standard_True); it.More(); it.Next()){ 
    TDF_Tool::Entry(it.Value(),es); 
    cout  <<  as.ToCString()  <<  endl; 
  } 
} 
~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

### Retrieving the father label

Retrieving the father label of a current label. 

~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~{.cpp}
TDF_Label father = achild.Father(); 
isroot = father.IsRoot(); 
~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

@subsection occt_ocaf_3_5 Attribute

The label itself contains no data. All data of any type whatsoever -- application or non-application -- is contained in
 attributes. These are attached to labels, and there are different types for different types of data. OCAF provides many
 ready-to-use standard attributes such as integer, real, constraint, axis and plane. There are also attributes for
 topological naming, functions and visualization. Each type of attribute is identified by a GUID. 

The advantage of OCAF is that all of the above attribute types are handled in the same way. Whatever the attribute type 
is, you can create new instances of them, retrieve them, attach them to and remove them from labels, "forget" and
 "remember" the attributes of a particular label. 

### Retrieving an attribute from a label

To retrieve an attribute from a label, you use *TDF_Label::FindAttribute*. In the example below, the GUID for integer 
attributes, and *INT*, a handle to an attribute are passed as arguments to *FindAttribute* for the current label. 


~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~{.cpp}
if(current.FindAttribute(TDataStd_Integer::GetID(),INT)) 
{ 
  // the attribute is found 
} 
else 
{ 
  // the attribute is not found 
} 
~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
### Identifying an attribute using a GUID

You can create a new instance of an attribute and retrieve its GUID. In the example below, a new integer attribute is
 created, and its GUID is passed to the variable *guid* by the method ID inherited from *TDF_Attribute*. 


~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~{.cpp}
Handle(TDataStd_Integer) INT = new TDataStd_Integer(); 
Standard_GUID guid = INT->ID(); 
~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

### Attaching an attribute to a label

To attach an attribute to a label, you use *TDF_Label::Add*. Repetition of this syntax raises an error message because 
there is already an attribute with the same GUID attached to the current label. 

*TDF_Attribute::Label* for *INT* then returns the label *attach* to which *INT* is attached. 


~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~{.cpp}
current.Add (INT); // INT is now attached to current 
current.Add (INT); // causes failure 
TDF_Label attach = INT->Label(); 
~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
### Testing the attachment to a label

You can test whether an attribute is attached to a label or not by using *TDF_Attribute::IsA* with the GUID of the 
attribute as an argument. In the example below, you test whether the current label has an integer attribute, and then,
 if that is so, how many attributes are attached to it. *TDataStd_Integer::GetID* provides the GUID argument needed by
 the method IsAttribute. 

*TDF_Attribute::HasAttribute* tests whether there is an attached attribute, and *TDF_Tool::NbAttributes* returns the
 number of attributes attached to the label in question, e.g. *current*. 


~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~{.cpp}
// Testing of attribute attachment 
// 
if (current.IsA(TDataStd_Integer::GetID())) { 
// the label has an Integer attribute attached 
} 
if (current.HasAttribute()) { 
// the label has at least one attribute attached 
Standard_Integer nbatt = current.NbAttributes(); 
// the label has nbatt attributes attached 
} 
~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

### Removing an attribute from a label

To remove an attribute from a label, you use *TDF_Label::Forget* with the GUID of the deleted attribute. To remove all 
attributes of a label, *TDF_Label::ForgetAll*. 


~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~{.cpp}
current.Forget(TDataStd_Integer::GetID()); 
// integer attribute is now not attached to current label 
current.ForgetAll(); 
// current has now 0 attributes attached 
~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
### Specific attribute creation

If the set of existing and ready to use attributes implementing standard data types does  not cover the needs of a 
specific data presentation task, the user can build his own data type and the corresponding new specific attribute 
implementing this new data type. 

There are two ways to implement a new data type: create a new attribute (standard approach), or use the notion of User
 Attribute by means of a combination of standard attributes  (alternative way) 

In order to create a new attribute in the standard way, create a class inherited from *TDF_Attribute* and implement all
 purely virtual and necessary virtual methods:
* **ID()** -- returns a unique GUID of a given attribute 
* **Restore(attribute)** -- sets fields of this attribute equal to the fields of a given attribute of the same type 
* **Paste(attribute, relocation_table)** -- sets fields of a given attribute equal to the field values of this attribute; 
if the attribute has references to some objects of the data framework  and relocation_table has this element, then the
 given attribute must also refer to this object . 
* **NewEmpty()** -- returns a new attribute of this class with empty fields 
* **Dump(stream)** --  outputs information about a given attribute to a given stream debug (usually outputs an attribute
 of type string only) 

Methods *NewEmpty, Restore* and *Paste* are used for the common transactions mechanism (Undo/Redo commands). If you 
don’t need this attribute to react to undo/redo commands, you can write only stubs of these methods, else you must call 
the Backup method of the *TDF_Attribute* class every time attribute fields are changed. 

To enable possibility to save / restore the new attribute in XML format, do the following: 
  1. Create a new package with the name Xml[package name] (for example *XmlMyAttributePackage*) containing  class
 *XmlMyAttributePackage_MyAttributeDriver*. The new class inherits *XmlMDF_ADriver* class and contains the translation
 functionality: from transient to persistent and vice versa (see the realization of the standard attributes in the
 packages *XmlMDataStd*, for example). Add package method AddDrivers which adds your class to a driver table (see below).
  2. Create a new package (or do it in the current one) with two package methods: 
	  * *Factory*, which loads the document storage and retrieval drivers; and 
	  * *AttributeDrivers*, which calls the methods AddDrivers for all packages responsible for persistence of the document.
  3. Create a plug-in implemented as an executable (see example *XmlPlugin*). It calls a macro PLUGIN with the package
 name where you implemented the method Factory.

To enable possibility to save / restore the new attribute in binary format, do the following: 
  1. Create a new package with name <i> Bin[package name] </i> (for example *BinMyAttributePackage*) containing a class
 *BinMyAttributePackage_MyAttributeDriver*. The new class inherits *BinMDF_ADriver* class and contains the translation
 functionality: from transient to persistent and vice versa (see the realization of the standard attributes in the
 packages *BinMDataStd*, for example). Add package method *AddDrivers*, which adds your class to a driver table.
  2. Create a new package (or do it in the current one) with two package methods: 
	  * Factory, which loads the document storage and retrieval drivers; and 
	  * AttributeDrivers, which calls the methods AddDrivers for all packages responsible for persistence of the document.
  3. Create a plug-in implemented as an executable (see example *BinPlugin*). It calls a macro PLUGIN with the package 
name where you implemented the method Factory.
See \ref anchor_std_services__save_document "Saving the document" and \ref anchor_std_services__open_document "Opening the document from a file" for the 
description of document save/open mechanisms. 

If you decided to use the alternative way (create a new attribute by means of *UAttribute* and a combination of other 
standard attributes), do the following: 
  1. Set a *TDataStd_UAttribute* with a unique GUID attached to a label. This attribute defines the semantics of the 
data type (identifies the data type).
  2. Create child labels and allocate all necessary data through standard attributes at the child labels.
  3. Define an interface class for access to the data of the child labels.

Choosing the alternative way of implementation of new data types allows to forget about creating persistence classes for 
your new data type. Standard persistence classes will be used instead. Besides, this way allows separating the data and
 the methods for access to the data (interfaces). It can be used for rapid development in all cases when requirements to 
application performance are not very high. 

Let’s study the implementation of the same data type in both ways by the example of transformation represented by 
*gp_Trsf* class. The class *gp_Trsf* defines the transformation according to the type (*gp_TrsfForm*) and a set of 
parameters of the particular type of transformation (two points or a vector for translation, an axis and an angle for 
rotation, and so on). 

1. The first way: creation of a new attribute. The implementation of the transformation by creation of a new attribute 
is represented in the @ref samples__ocaf "OCAF Sample". 

2. The second way: creation of a new data type by means of combination of standard attributes. Depending on the type of
 transformation it may be kept in data framework by different standard attributes. For example, a translation is defined
 by two points. Therefore the data tree for translation looks like this: 
  * Type of transformation <i>(gp_Translation)</i> as *TDataStd_Integer*;
  * First point as *TDataStd_RealArray* (three values: X1, Y1 and Z1);
  * Second point as *TDataStd_RealArray* (three values: X2, Y2 and Z2).

@image html /03_user_guides/ocaf/images/ocaf_image010.png "Data tree for translation"
@image latex /03_user_guides/ocaf/images/ocaf_image010.png "Data tree for translation"

If the type of transformation is changed to rotation, the data tree looks like this: 
  * Type of transformation <i>(gp_Rotation)</i> as *TDataStd_Integer*;
  * Point of axis of rotation as *TDataStd_RealArray* (three values: X, Y and Z);
  * Axis of rotation as *TDataStd_RealArray* (three values: DX, DY and DZ);
  * Angle of rotation as *TDataStd_Real*.

@image html /03_user_guides/ocaf/images/ocaf_image011.png "Data tree for rotation"
@image latex /03_user_guides/ocaf/images/ocaf_image011.png "Data tree for rotation"

The attribute *TDataStd_UAttribute* with the chosen unique GUID identifies the data type. The interface class
 initialized by the label of this attribute allows access to the data container (type of transformation and the data of
 transformation according to the type). 
  
@subsection occt_ocaf_3_6 Compound documents
 
  As the identification of data is persistent, one document can reference data contained in another document, 
  the referencing and  referenced documents being saved in two separate files.  
 
  Lets look at the coffee machine application again. The  coffee pot can be placed in one document. 
  The coffee machine document then  includes an *occurrence* — a positioned copy — of the coffee pot. 
  This occurrence is defined by an XLink attribute (the external Link) 
  which references the coffee pot of the first document 
  (the XLink contains the relative path of the coffee pot document and the entry of the coffee pot data [0:1] ).  

@image html ocaf_wp_image006.png "The coffee machine compound document"
@image latex ocaf_wp_image006.png "The coffee machine compound document"
 
  In this context, the end-user of the coffee machine application can open the coffee pot document, 
  modify the geometry of, for  example, the reservoir, and overwrite the document without worrying 
  about the impact of the modification in the coffee machine document. 
  To deal with this  situation, OCAF provides a service which allows the application to check 
  whether a document is up-to-date. This service is based on a modification counter included in each document: 
  when an external link is created, a copy of  the referenced document counter is associated to the XLink 
  in the referencing  document. Providing that each modification of the referenced document increments its own counter,   
  we can detect that the referencing document has to  be updated by comparing the two counters 
  (an update function importing the data  referenced by an XLink into the referencing document is also provided).  
 
@subsection occt_ocaf_3_7 Transaction mechanism
 
  The Data Framework also provides a transaction mechanism inspired from database management systems: 
  the data are modified within a transaction which is terminated either by a Commit 
  if the modifications are validated  or by an Abort if the modifications are abandoned — 
  the data are then restored  to the state it was in prior to the transaction. 
  This mechanism is extremely useful for:

  * Securing  editing operations (if an error occurs, the transaction is abandoned and the  structure retains its 
integrity)
  * Simplifying  the implementation of the **Cancel** function (when the end-user begins a command, 
  the application may launch a transaction and operate directly in the data structure; 
  abandoning the action causes the transaction to Abort)
  * Executing  **Undo** (at commit time, the modifications are recorded in order to be able to  restore the data to 
their previous state)
 
  The transaction mechanism simply manages a  backup copy of attributes. 
  During a transaction, attributes are copied before  their first modification. 
  If the transaction is validated, the copy is  destroyed. 
  If the transaction is abandoned, the attribute is restored to its initial value 
  (when attributes are added or deleted, the operation is simply  reversed).

  Transactions are document-centered, that is, the application starts a transaction on a document. 
  So, modifying a referenced  document and updating one of its referencing documents requires 
  two transactions, even if both operations are done in the same working session.

Standard Attributes {#user_guides__ocaf_std_attribs}
===================

@tableofcontents

Standard attributes are ready-to-use attributes, which allow creating and modifying attributes for many basic data types. 
They are available in the packages *TDataStd, TDataXtd* and *TDF*. Each attribute belongs to one of four types: 

  * Geometric attributes;
  * General attributes;
  * Relationship attributes;
  * Auxiliary attributes.   

### Geometric attributes

  * **Axis** -- simply identifies, that the concerned *TNaming_NamedShape* attribute with an axis shape inside belongs
 to the same label; 
  * **Constraint** -- contains information about a constraint between geometries: used geometry attributes, type, value 
(if exists), plane (if exists), "is reversed", "is inverted" and "is verified" flags;
  * **Geometry** -- simply identifies, that the concerned *TNaming_NamedShape* attribute with a specified-type geometry
 belongs to the same label; 
  * **Plane** -- simply identifies, that the concerned *TNaming_NamedShape* attribute with a plane shape inside belongs
 to the same label;
  * **Point** -- simply identifies, that the concerned *TNaming_NamedShape* attribute with a  point shape inside belongs
 to the same label;
  * **Shape** -- simply identifies, that the concerned *TNaming_NamedShape* attribute belongs to the same label;
  * **PatternStd**  -- identifies one of five available pattern models (linear, circular, rectangular, circular 
rectangular and mirror);
  * **Position** -- identifies the position in 3d global space.

### General attributes

  * **AsciiString** -- contains AsciiString value;
  * **BooleanArray** -- contains an array of Boolean;
  * **BooleanList** -- contains a list of Boolean;
  * **ByteArray** -- contains an array of Byte (unsigned char) values;
  * **Comment** -- contains a string -- the comment for a given label (or attribute);
  * **Expression** -- contains an expression string and a list of used variables attributes;
  * **ExtStringArray** -- contains an array of *ExtendedString* values;
  * **ExtStringList** -- contains a list of *ExtendedString* values;
  * **Integer** -- contains an integer value;
  * **IntegerArray** -- contains an array of integer values;
  * **IntegerList** -- contains a list of integer values;
  * **IntPackedMap** -- contains a packed map of integers;
  * **Name** -- contains a string -- the name of a given label (or attribute);
  * **NamedData** -- may contain up to 6 of the following named data sets (vocabularies): *DataMapOfStringInteger, 
DataMapOfStringReal, DataMapOfStringString, DataMapOfStringByte, DataMapOfStringHArray1OfInteger* or 
*DataMapOfStringHArray1OfReal*;
  * **NoteBook** -- contains a *NoteBook* object attribute;
  * **Real** -- contains a real value;
  * **RealArray** -- contains an array of  real values;
  * **RealList** -- contains a list of real values;
  * **Relation** -- contains a relation string and a list of used variables attributes;
  * **Tick** -- defines a boolean attribute;
  * **Variable** -- simply identifies, that a variable belongs to this label; contains the flag *is constraint* and a 
string of used units ("mm", "m"...);
  * **UAttribute** -- attribute with a user-defined GUID. As a rule, this attribute is used as a marker, which is 
independent of attributes at the same label (note, that attributes with the same GUIDs can not belong to the same label).
  
### Relationship attributes 

  * **Reference** -- contains reference to the label of its own data framework;
  * **ReferenceArray** -- contains an array of references;
  * **ReferenceList** -- contains a list of references;
  * **TreeNode** -- this attribute allows to create an internal tree in the data framework; this tree consists of nodes 
with the specified tree ID; each node contains references to the father, previous brother, next brother, first child 
nodes and tree ID.

### Auxiliary attributes

  * **Directory** -- high-level tool attribute for sub-labels management;
  * **TagSource** -- this attribute is used for creation of new children: it stores the tag of the last-created child of 
the label and gives access to the new child label creation functionality.

All attributes inherit class *TDF_Attribute*, so, each attribute has its own GUID and standard methods for attribute 
creation, manipulation, getting access to the data framework. 

@section occt_ocaf_6_2 Common Services

## Accessing GUIDs

To access the GUID of an attribute, you can use two methods: 
  * Method *GetID* is the static method of a class. It returns the GUID of any attribute, which is an object of a 
specified class (for example, *TDataStd_Integer* returns the GUID of an integer attribute). Only two classes from the
 list of standard attributes do not support these methods: *TDataStd_TreeNode* and *TDataStd_Uattribute*, because the 
GUIDs of these attributes are variable.
  * Method *ID* is the method of an object of an attribute class. It returns the GUID of this attribute. Absolutely all
 attributes have this method: only by this identifier you can discern the type of an attribute.
  
To find an attribute attached to a specific label, you use the GUID of the attribute type you are looking for. This 
information can be found using the method  <i> GetID</i> and the method <i> Find</i> for the label as follows:

~~~~
    Standard_GUID anID = MyAttributeClass::GetID();
    Standard_Boolean HasAttribute = aLabel.Find(anID,anAttribute);
~~~~

## Conventional Interface of Standard Attributes

It is usual to create standard named methods for the attributes: 

  * Method *Set(label, [value])* is the static method, which allows to add an attribute to a given label. If an
 attribute is characterized by one value this method may set it.
  * Method *Get()* returns the value of an attribute if it is characterized by one value.
  * Method *Dump(Standard_OStream)* outputs debug information about a given attribute to a given stream.
  
@section occt_ocaf_6_3 Standard vs Custom Attributes

When you start to design an application  based on OCAF, usually it is necessary to choose, which attribute will be used 
for allocation of data in the OCAF document: standard or newly-created?

It is possible to describe any model by means of standard OCAF attributes. 
  However, it is still a question if  this description will be  efficient in terms of memory and speed, and, at the same
 time, convenient to use.
  
  This depends on a particular model.  
   
  OCAF imposes the restriction that only one attribute type may be allocated to one label. 
  It is necessary to take into  account the design of the application data tree. 
  For example, if a label should  possess several double values, 
  it is necessary to distribute them through several child sub-labels or use an array of double values.   
   
  Let us consider several boundary implementations of the same model in OCAF tree and analyze the advantages and 
disadvantages of each approach.  
  
## Comparison and Analysis of Approaches

  Below are described two different model implementations: 
  one is based on standard OCAF attributes and the other is based 
  on the creation of a new attribute possessing all data of the model.  
   
  A load is distributed through the shape. The measurements are taken at particular points defined by (x, y and z) 
co-ordinates. The load is represented as a projection onto X, Y and Z axes of the local co-ordinate system at each point 
of measurement. A matrix of transformation is needed 
  to convert the local co-ordinate system to the global one, but this is optional.   
   
  So, we have 15 double values at each point  of measurement. 
  If the number of such points is 100 000, for example, it means 
  that we have to store 1 500 000 double values in the OCAF document.  
   
  The first  approach consists in using standard OCAF attributes. 
  Besides, there are several  variants of how the standard attributes may be used:  
  * Allocation of all 1 500 000 double values as one array of double values attached to one label;
  * Allocation of values of one measure of load (15 values) as one array of double values and attachment of one point of
 measure to one label;
  * Allocation of each point of measure as an array of 3 double values attached to one label, the projection of load 
onto the local co-ordinate system  axes as another array of 3 double values attached to a sub-label, and the matrix of 
projection (9 values) as the third array also attached to a sub-label.  
  
  Certainly, other variants are also  possible.   
 
@image html ocaf_tree_wp_image003.png "Allocation of all data as one  array of double values"
@image latex ocaf_tree_wp_image003.png "Allocation of all data as one  array of double values"
 
  The first approach to allocation of all  data represented as one array of double values 
  saves initial memory and is easy to implement. 
  But access to the data is difficult because the values are stored in a flat array. 
  It will be necessary to implement a class with several methods giving access 
  to particular fields like the measurement points, loads and so  on.  
   
  If the values may be edited in the  application, 
  it means that the whole array will be backed-up on each edition. 
  The memory usage will increase very fast! 
  So, this approach may be considered only in case of non-editable data.  
   
  Let’s consider the allocation of data of  each measurement point per label (the second case). 
  In this case we create 100  000 labels -- one label for each measurement point 
  and attach an array of double  values to these labels:  
 
@image html ocaf_tree_wp_image004.png "Allocation of data of each  measurement point as arrays of double values"
@image latex ocaf_tree_wp_image004.png "Allocation of data of each  measurement point as arrays of double values"
 
  Now edition of data is safer as far as  memory usage is concerned. 
  Change of value for one measurement point (any  value: point co-ordinates, load, and so on) 
  backs-up only one small array of double values. 
  But this structure (tree) requires more memory space (additional  labels and attributes).  
   
  Besides, access to the values is still difficult and it is necessary 
  to have a class with methods of access to the  array fields.  
   
  The third case of allocation of data  through OCAF tree is represented below:  

@image html ocaf_tree_wp_image005.png "Allocation of data into separate arrays of double values"
@image latex ocaf_tree_wp_image005.png "Allocation of data into separate arrays of double values"

  In this case sub-labels are involved and we  can easily access the values of each measurement point, 
  load or matrix. We don’t need an interface class with methods of access to the data 
  (if it exists, it would help to use the data structure, but this is optional).  

  On the one hand, this approach requires more  memory for allocation of the attributes (arrays of double values). 
  On the other  hand, it saves memory during the edition of data 
  by backing-up only the small array containing the modified data. 
  So, if the data is fully modifiable, this  approach is more preferable.  

  Before making a conclusion, let’s consider the same model implemented through a newly created OCAF attribute.  

  For example, we might allocate all data  belonging to one measurement point as one OCAF attribute. 
  In this case we  implement the third variant of using the standard attributes (see picture 3), 
  but we use less memory (because we use only one attribute instead of three):  
 
@image html ocaf_tree_wp_image006.png "Allocation of data into newly  created OCAF attribute"
@image latex ocaf_tree_wp_image006.png "Allocation of data into newly  created OCAF attribute"

  The second variant of using standard OCAF attributes still has drawbacks: 
  when data is edited, OCAF backs-up all values  of the measurement point.   
   
  Let’s imagine that we have some  non-editable data. 
  It would be better for us to allocate this data separately from editable data. 
  Back-up will not affect non-editable data and memory will not increase so much during data edition.  
  
## Conclusion

  When deciding which variant of data model implementation to choose, 
  it is necessary to take into account the application  response time, 
  memory allocation and memory usage in transactions.   
   
  Most of the models may be implemented using only standard OCAF attributes. 
  Some other models need special treatment and require implementation of new OCAF attributes.
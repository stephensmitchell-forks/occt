Data Types {#user_guides__foundations_types}
==========
@tableofcontents

@section occt_fcug_2_1_1 Primitive Types

The primitive types are predefined in the language and they  are **manipulated by value**. 

* **Boolean** is used to represent logical  data. It may have only two values: *Standard_True*  and *Standard_False*. 
* **Character** designates any ASCII  character. 
* **ExtCharacter** is an extended character. 
* **Integer** is a whole number. 
* **Real** denotes  a real number (i.e. one with whole and a fractional part, either of which may  be null). 
* **ShortReal** is a real with a smaller choice of  values and memory size. 
* **CString** is used for literal  constants. 
* **ExtString** is an extended string. 
* **Address** represents a byte address of  undetermined size. 

The services offered by each of these types are described in  the **Standard** Package. 
The table below presents the equivalence existing between  C++ fundamental types and OCCT primitive types. 

**Table 1: Equivalence between C++ Types and OCCT Primitive  Types** 

| C++ Types	| OCCT Types |
| :--------- | :----------- |
| int	| Standard_Integer |
| double 	| Standard_Real |
| float	| Standard_ShortReal |
| unsigned int	| Standard_Boolean |
| char	| Standard_Character |
| short	| Standard_ExtCharacter |
| char\*	| Standard_CString |
| void\*	| Standard_Address |
| short\*	| Standard_ExtString |

\* The types with asterisk are pointers. 

**Reminder of the classes listed above:** 

* **Standard_Integer** : fundamental type representing 32-bit integers yielding  negative, positive or null values. *Integer* is implemented as a *typedef* of the C++ *int* fundamental type. As such, the algebraic operations  +, -, *, / as well as the ordering and equivalence relations <, <=, ==, !=, >=, >  are defined on it. 
* **Standard_Real** : fundamental type representing real numbers with finite  precision and finite size. **Real** is implemented as a *typedef* of  the C++ *double* (double precision) fundamental type. As such, the  algebraic operations +, -, *, /, unary- and the ordering and equivalence  relations <, <=, ==, !=, >=, >  are defined on reals. 
* **Standard_ShortReal** : fundamental type representing real numbers with finite  precision and finite size. *ShortReal* is implemented as a *typedef* of  the C++ *float* (simple precision) fundamental type. As such, the  algebraic operations +, -, *, /, unary- and the ordering and equivalence  relations <, <=, ==, !=, >=, >  are defined on reals. 
* **Standard_Boolean** : fundamental type representing logical  expressions. It has two values: *false* and *true*. *Boolean* is implemented  as a *typedef* of the C++ *unsigned int* fundamental type. As such,  the algebraic operations *and, or, xor* and *not* as well as equivalence relations == and != are defined on Booleans. 
* **Standard_Character** : fundamental type representing the  normalized ASCII character set. It may be assigned the values of the 128 ASCII  characters. *Character* is implemented as a *typedef* of the C++ *char* fundamental type. As such, the ordering and equivalence relations <, <=, ==, !=, >=, >  are defined on characters using the order of the  ASCII chart (ex: A B). 
* **Standard_ExtCharacter** : fundamental type representing the  Unicode character set. It is a 16-bit character type. *ExtCharacter* is  implemented as a *typedef* of the C++ *short* fundamental type. As  such, the ordering and equivalence relations <, <=, ==, !=, >=, >   are defined on extended characters using the order of the UNICODE chart (ex:  A B). 
* **Standard_CString** : fundamental type representing string  literals. A string literal is a sequence of ASCII (8 bits) characters enclosed  in double quotes. *CString* is implemented as a *typedef* of the C++ *char* fundamental type. 
* **Standard_Address** : fundamental type representing a generic  pointer. *Address* is implemented as a *typedef* of the C++ *void* fundamental  type. 
* **Standard_ExtString** is a fundamental type representing string  literals as sequences of Unicode (16 bits) characters. *ExtString* is  implemented as a *typedef* of the C++ *short* fundamental type. 

@section occt_fcug_3_1 Collections

The **Collections** component contains the classes that  handle dynamically sized aggregates of data. They include a wide range of  collections such as arrays, lists and maps. 

Collections classes are *generic* (C++ template-like), that is, they define  a structure and algorithms allowing to hold a variety of objects which do not  necessarily inherit from a unique root class (similarly to C++ templates). 

When you need to use a collection of a given type of object you must *instantiate* it for this specific type of element. Once this declaration is compiled,  all the functions available on the generic collection are available on your *instantiated  class*. 

However, note that: 
  * Each collection directly used as an argument in OCCT public  syntax is instantiated in an OCCT component.
  * The *TColStd* package (**Collections of Standard Objects** component)  provides numerous instantiations of these generic collections with objects from  the **Standard** package or from the **Strings** component.
The **Collections** component provides a wide range of  generic collections: 
  * **Arrays** are generally used for a quick access to the item,  however an array is a fixed sized aggregate.
  * **Sequences** are variable-sized structures, they avoid the  use of large and quasi-empty arrays. A sequence item is longer to access  than an array item: only an exploration in sequence is effective (but sequences  are not adapted for numerous explorations). Arrays and sequences are commonly  used as data structures for more complex objects.
  * **Maps** are dynamic structures, where the size is constantly adapted to the number of inserted items and access to an item is the fastest. Maps structures are commonly used in cases of numerous explorations: they are typically internal data structures for complex algorithms.
  * **Lists** are similar to sequences but have different algorithms to explore them. 
  * Specific iterators for sequences and maps. 
  
@subsection occt_fcug_3_1_2 Generic general-purpose Aggregates

### TCollection_Array1

These are unidimensional arrays similar to C arrays, i.e. of fixed  size but dynamically dimensioned at construction time. 
As with a C array, the access time for an *Array1* indexed  item is constant and is independent of the array size. Arrays are commonly used  as elementary data structures for more complex objects. 

*Array1* is a generic class which depends on *Item*,  the type of element in the array. 

*Array1* indexes start and end at a user-defined  position. Thus, when accessing an item, you must base the index on the lower  and upper bounds of the array. 

### TCollection_Array2

These are bi-dimensional arrays of fixed size but dynamically  dimensioned at construction time. 

As with a C array, the access time for an *Array2* indexed  item is constant and is independent of the array size. Arrays are commonly used  as elementary data structures for more complex objects. 

*Array2* is a generic class which depends on *Item*,  the type of element in the array. 

*Array2* indexes start and end at a user-defined  position. Thus, when accessing an item, you must base the index on the lower  and upper bounds of the array. 

### TCollection_HArray1

These are unidimensional arrays similar to C arrays, i.e. of fixed  size but dynamically dimensioned at construction time. 
As with a C array, the access time for an *HArray1* or *HArray2* indexed item is constant and is independent of the array size. Arrays are  commonly used as elementary data structures for more complex objects. 

*HArray1* objects are **handles** to arrays. 
  * *HArray1* arrays may be shared by several objects.
  * You may use a *TCollection_Array1* structure to have the  actual array.
  
*HArray1* is a generic class which depends on two parameters: 
  * **Item**, the type of element in the array,
  * **Array**, the actual type of array handled by *HArray1*.  This is an instantiation with **Item** of the *TCollection_Array1* generic  class.
  
*HArray1* indexes start and end at a user-defined  position. Thus, when accessing an item, you must base the index on the lower  and upper bounds of the array. 

### TCollection_HArray2

These are bi-dimensional arrays of fixed size but dynamically  dimensioned at construction time. 

As with a C array, the access time for an *HArray2* indexed  item is constant and is independent of the array size. Arrays are commonly used  as elementary data structures for more complex objects. 

*HArray2* objects are **handles** to arrays. 
  * *HArray2* arrays may be shared by several objects.
  * You may use a *TCollection_Array2* structure to have the  actual array.
  
*HArray2* is a generic class which depends on two  parameters: 
  * *Item*, the type of element in the array,
  * *Array*, the actual type of array handled by *HArray2*.  This is an instantiation with *Item* of the *TCollection_Array2* generic  class.
  
### TCollection_HSequence

This is a sequence of items indexed by an integer. 

Sequences have about the same goal as unidimensional arrays  *TCollection_HArray1*: they are commonly used as elementary data  structures for more complex objects. But a sequence is a structure of *variable  size*: sequences avoid the use of large and quasi-empty arrays. Exploring a  sequence data structure is effective when the exploration is done in  sequence; elsewhere a sequence item is longer to read than an array item.  Note also that sequences are not effective when they have to support numerous  algorithmic explorations: a map is better for that. 

*HSequence* objects are **handles** to sequences. 
  * *HSequence* sequences may be shared by several objects.
  * You may use a *TCollection_Sequence* structure to have the  actual sequence.
  
*HSequence* is a generic class which depends on two  parameters: 
  * *Item*, the type of element in the sequence,
  * *Seq*, the actual type of sequence handled by *HSequence*.  This is an instantiation with *Item* of the *TCollection_Sequence* generic  class.
  
### TCollection_List

These are ordered lists of non-unique objects which can be accessed  sequentially using an iterator. 
Item insertion in a list is very fast at any position. But  searching for items by value may be slow if the list is long, because it  requires a sequential search. 

*List* is a generic class, which depends on *Item*,  the type of element in the structure. 
Use a *ListIterator* iterator to explore a *List* structure. 

An iterator class is automatically instantiated from the *TCollection_ListIterator* class at the time of instantiation of a *List* structure. 

A sequence is a better structure when searching for items by  value. 

Queues and stacks are other kinds of list with a different  access to data. 

### TCollection_Sequence

This is a sequence of items indexed by an integer. 
Sequences have about the same goal as unidimensional arrays  (*TCollection_Array1*): they are commonly used as elementary data  structures for more complex objects. But a sequence is a structure of *variable  size*: sequences avoid the use of large and quasi-empty arrays. Exploring a  sequence data structure is effective when the exploration is done *in sequence*; elsewhere a sequence item is longer to read than an array item.  Note also that sequences are not effective when they have to support numerous  algorithmic explorations: a map is better for that. 

*Sequence* is a generic class which depends on *Item*,  the type of element in the sequence. 

@subsection occt_fcug_3_1_3 Generic Maps

Maps are dynamically extended data structures where data is  quickly accessed with a key. *TCollection_BasicMap* is a root class for maps. 

### General properties of maps

Map items may contain complex non-unitary data, thus it can be difficult to manage them with an array. The map allows a data structure to be  indexed by complex data. 

The size of a map is dynamically extended. So a map may be  first dimensioned for a little number of items. Maps avoid the use of large and  quasi-empty arrays. 

The access time for a map item is much better than the one  for a sequence, list, queue or stack item. It is  comparable with the  access time for an array item. It depends on the size of the map and on the quality of the user redefinable function (the *hashing  function*) to find quickly where is the item. 

The performance of a map exploration may be better of an array exploration because the size of the map is adapted to the  number of inserted items. 

That is why maps are commonly used as  internal data structures for algorithms. 

### Definitions 

A map is a data structure for which data are addressed by *keys*. 

Once inserted in the map, a map item is referenced as an *entry* of the map. 

Each entry of the map is addressed by a key. Two different  keys address two different entries of the map. 
The position of an entry in the map is called a *bucket*. 

A map is dimensioned by its number of buckets, i.e. the  maximum number of entries in the map. The performance of a map is conditioned  by the number of buckets. 

The *hashing function* transforms a key into a bucket  index. The number of values that can be computed by the hashing function is  equal to the number of buckets of the map. 

Both the hashing function and the equality test between two  keys are provided by a *hasher* object. 

A map may be explored by a *map iterator*. This  exploration provides only inserted entries in the map (i.e. non empty buckets). 

### Collections of generic maps 

The *Collections* component provides numerous generic  derived maps. 

These maps include automatic management of the number of *buckets*:  they are automatically resized when the number of *keys* exceeds the  number of buckets. If you have a fair idea of the number of items in your map,  you can save on automatic resizing by specifying a number of buckets at the  time of construction, or by using a resizing function. This may be considered  for crucial optimization issues. 

*Keys, items* and *hashers* are parameters of  these generic derived maps. 

*TCollection_MapHasher* class describes the functions  required by any *hasher*, which is to be used with a map instantiated from  the **Collections** component. 

An iterator class is automatically instantiated at the time  of instantiation of a map provided by the *Collections* component if this  map is to be explored with an iterator. Note that some provided generic maps  are not to be explored with an iterator but with indexes (*indexed maps*). 

### TCollection_DataMap

This is a map used to store keys with associated items. An entry of **DataMap** is composed of both the key and the item. 
The *DataMap* can be seen as an extended array where  the keys are the indexes.
 
*DataMap* is a generic class which depends on three  parameters: 
  * *Key* is the type of key for an entry in the map,
  * *Item* is the type of element associated with a key in the  map,
  * *Hasher* is the type of hasher on keys.
  
Use a *DataMapIterator* iterator to explore a *DataMap*  map. 

An iterator class is automatically instantiated from the *TCollection_DataMapIterator* generic class at the time  of instantiation of a *DataMap* map. 

*TCollection_MapHasher* class describes the functions required for a *Hasher* object. 

### TCollection_DoubleMap

This is a map used to bind pairs of keys (Key1,Key2) and retrieve them in linear time. 

*Key1* is  referenced as the first key of the *DoubleMap* and *Key2* as the second key. 

An entry of a *DoubleMap* is composed of a pair of two  keys: the first key and the second key. 

*DoubleMap* is a generic class which depends on four  parameters: 
  * *Key1* is the type of the first key for an entry in the map,
  * *Key2* is the type of the second key for an entry in the  map,
  * *Hasher1* is the type of hasher on first keys,
  * *Hasher2* is the type of hasher on second keys.
  
Use *DoubleMapIterator* to explore a *DoubleMap* map. 

An iterator class is automatically instantiated from the *TCollection_DoubleMapIterator* class at the time of instantiation of a *DoubleMap* map. 

*TCollection_MapHasher* class describes the functions  required for a *Hasher1* or a *Hasher2* object. 

### TCollection_IndexedDataMap

This is  map to store keys with associated items and to bind an  index to them. 

Each new key stored in the map is assigned an index. Indexes  are incremented as keys (and items) stored in the map. A key can be found by  the index, and an index can be found by the key. No key but the last can be  removed, so the indexes are in the range 1...Upper,  where *Upper* is the number of  keys stored in the map. An item is stored with each key. 

An entry of an *IndexedDataMap* is composed of both the  key, the item and the index. An *IndexedDataMap* is an ordered map, which  allows a linear iteration on its contents. It combines the interest: 
  * of an array because data may be accessed with an index,
  * and of a map because data may also be accessed with a key.

*IndexedDataMap* is a generic class which depends on  three parameters: 
	* *Key* is the type of key for an entry in the map,
	* *Item* is the type of element associated with a key in the map,
	* *Hasher* is the type of hasher on keys.

### TCollection_IndexedMap

This is map used to store keys and to bind an index to them. 

Each new key stored in the map is assigned an index. Indexes  are incremented as keys stored in the map. A key can be found by the index, and  an index by the key. No key but the last can be removed, so the indexes are in  the range 1...Upper where Upper is the number of keys stored  in the map. 

An entry of an *IndexedMap* is composed of both the key  and the index. An *IndexedMap* is an ordered map, which allows a linear  iteration on its contents. But no data is attached to the key. An *IndexedMap* is typically used by an algorithm to know if some action is still performed  on components of a complex data structure. 

*IndexedMap* is a generic class which depends on two  parameters: 
  * *Key* is the type of key for an entry in the map,
  * *Hasher* is the type of hasher on keys.

### TCollection_Map

This is a basic hashed map, used to store and retrieve keys in  linear time. 

An entry of a *Map* is composed of the key only. No  data is attached to the key. A *Map* is typically used by an algorithm to  know if some action is still performed on components of a complex data  structure. 

*Map* is a generic class which depends on two  parameters: 
  * *Key* is the type of key in the map,
  * *Hasher* is the type of hasher on keys.

Use a *MapIterator* iterator to explore a *Map* map. 

### TCollection_MapHasher

This is a hasher on the *keys* of a map instantiated from the *Collections* component. 

A hasher provides two functions: 
* *HashCode()* function transforms a key into a bucket index in the map. The number of values that can be  computed by the hashing function is equal to the number of buckets in the map.
* *IsEqual* is the equality test between two keys. Hashers are  used as parameters in generic maps provided by the **Collections** component.

*MapHasher* is a generic class which depends on the  type of keys, providing that *Key* is a type from the *Standard* package.  In such cases *MapHasher* may be directly instantiated with *Key*.  Note that the package *TColStd* provides some of these instantiations. 

Elsewhere, if *Key* is not a type from the *Standard* package  you must consider *MapHasher* as a template and build a class which  includes its functions, in order to use it as a hasher in a map instantiated  from the *Collections* component. 

Note that *TCollection_AsciiString* and *TCollection_ExtendedString* classes correspond to these specifications, in consequence they may be used  as hashers: when *Key* is one of these two types you may just define the  hasher as the same type at the time of instantiation of your map. 

@subsection occt_fcug_3_1_4 Iterators

### TCollection_BasicMapIterator

This is a root class for map iterators. A map iterator provides a step  by step exploration of all the entries of a map. 

### TCollection_DataMapIterator

These are functions used for iterating the contents of a *DataMap* map. 

A map is a non-ordered data structure. The order in which  entries of a map are explored by the iterator depends on its contents and  change when the map is edited. It is not recommended to modify the contents of a map during  the iteration: the result is unpredictable. 

### TCollection_DoubleMapIterator

These are functions used for iterating the contents of a *DoubleMap* map. 

### TCollection_ListIterator

These are unctions used for iterating the contents of a *List* data  structure. 

A *ListIterator* object can be used to go through a  list sequentially, and as a bookmark to hold a position in a list. It is not an  index, however. Each step of the iteration gives the current position of  the iterator, to which corresponds the current item in the list. The current  position is not defined if the list is empty, or when the exploration  is finished. 

An iterator class is automatically instantiated from this  generic class at the time of instantiation of a *List* data structure. 

### TCollection_MapIterator

These are functions used for iterating the contents of a *Map* map. 
An iterator class is automatically instantiated from this  generic class at the time of instantiation of a *Map* map. 

### TCollection_SetIterator

These are functions used for iterating the contents of a *Set* data  structure. 
An iterator class is automatically instantiated from this  generic class at the time of instantiation of a *Set* structure. 

### TCollection_StackIterator

These are functions used for iterating the contents of a **Stack** data  structure. 

An iterator class is automatically instantiated from this  generic class at the time of instantiation of a *Stack* structure. 

@section occt_fcug_3_2 Collections of Standard Objects

While generic classes of the *TCollection* package are the  root classes that describe the generic purpose of every type of collection, classes effectively used are extracted from the *TColStd* package. 
The *TColStd* and *TShort* packages provide  frequently used instantiations of generic classes with objects from the *Standard* package or strings from the *TCollection* package. 

These instantiations are the following: 
  * Unidimensional arrays: instantiations of the *TCollection_Array1* generic class with *Standard* Objects and *TCollection* strings.
  * Bidimensional arrays: instantiations of the *TCollection_Array2* generic class with *Standard* Objects.
  * Unidimensional arrays manipulated by handles: instantiations of  the *TCollection_HArray1* generic class with *Standard* Objects and *TCollection* strings.
  * Bidimensional arrays manipulated by handles: instantiations of  the *TCollection_HArray2* generic class with *Standard* Objects.
  * Sequences: instantiations of the *TCollection_Sequence* generic  class with *Standard* objects and *TCollection* strings.
  * Sequences manipulated by handles: instantiations of the *TCollection_HSequence* generic class with *Standard* objects and *TCollection* strings.
  * Lists: instantiations of the *TCollection_List* generic  class with *Standard* objects.
  * Queues: instantiations of the *TCollection_Queue* generic  class with *Standard* objects.
  * Sets: instantiations of the *TCollection_Set* generic class  with *Standard* objects.
  * Sets manipulated by handles: instantiations of the *TCollection_HSet* generic class with *Standard* objects.
  * Stacks: instantiations of the *TCollection_Stack* generic  class with *Standard* objects.
  * Hashers on map keys: instantiations of the *TCollection_MapHasher* generic class with *Standard* objects.
  * Basic hashed maps: instantiations of the *TCollection_Map* generic  class with *Standard* objects.
  * Hashed maps with an additional item: instantiations of the *TCollection_DataMap* generic class with *Standard* objects.
  * Basic indexed maps: instantiations of the *TCollection_IndexedMap* generic class with *Standard* objects.
  * Indexed maps with an additional item: instantiations of the *TCollection_IndexedDataMap* generic class with *Standard_Transient* objects.
  * Class *TColStd_PackedMapOfInteger* provides alternative  implementation of map of integer numbers, optimized for both performance and  memory usage (it uses bit flags to encode integers, which results in spending  only 24 bytes per 32 integers stored in optimal case). This class also provides  Boolean operations with maps as sets of integers (union, intersection,  subtraction, difference, checks for equality and containment).
  
@section occt_fcug_3_3 NCollections
  
The *NCollection* package provides a set of template collection classes used throughout OCCT.

Macro definitions of these classes are stored in *NCollection_Define\*.hxx* files. These definitions are now obsolete though still can be used, particularly for compatibility with the existing code.

@subsection occt_fcug_3_3_2 Instantiation of collection classes

Now we are going to implement the definitions from *NCollection* in the code, taking as an example a sequence of points (analogue of *TColgp_SequenceOfPnt*).
  
### Definition of a new collection class

Let the header file be *MyPackage_SequenceOfPnt.hxx* :

Template class instantiaton
~~~~~
#include <NCollection_Sequence.hxx>
#include <gp_Pnt.hxx>
typedef NCollection_Sequence<gp_Pnt> MyPackage_SequenceOfPnt;
~~~~~

Macro instantiation
~~~~~
#include <NCollection_DefineSequence.hxx>
#include <gp_Pnt.hxx>
~~~~~

The following line defines the class "base collection of points"
~~~~~
DEFINE_BASECOLLECTION(MyPackage_BaseCollPnt, gp_Pnt)
~~~~~

The following line defines the class *MyPackage_SequenceOfPnt*

~~~~~
DEFINE_SEQUENCE (MyPackage_SequenceOfPnt, MyPackage_BaseCollPnt , gp_Pnt)  
~~~~~

### Definition of a new collection class managed by Handle

It is necessary to provide relevant statements both in the header ( .hxx file) and the C++ source ( .cxx file). 

Header file MyPackage_HSequenceOfPnt.hxx:

~~~~~
#include <NCollection_DefineHSequence.hxx>
#include <gp_Pnt.hxx>
~~~~~

The following line defines the class "base collection of points"

~~~~~
DEFINE_BASECOLLECTION(MyPackage_BaseCollPnt, gp_Pnt)
~~~~~

The following line defines the class *MyPackage_SequenceOfPnt*

~~~~~
DEFINE_SEQUENCE (MyPackage_SequenceOfPnt, MyPackage_BaseCollPnt, gp_Pnt)
~~~~~

The following line defines the classes *MyPackage_HSequenceOfPnt* and *Handle(MyPackage_HSequenceOfPnt)*

~~~~~
DEFINE_HSEQUENCE (MyPackage_HSequenceOfPnt, MyPackage_SequenceOfPnt)
~~~~~

Source code file will be *MyPackage_HSequenceOfPnt.cxx* or any other .cxx file (once in the whole project):

~~~~~
IMPLEMENT_HSEQUENCE (MyPackage_HSequenceOfPnt)
~~~~~

@subsection occt_fcug_3_3_3 Arrays and sequences

Standard collections provided by OCCT are:
* *NCollection_Array1* -- fixed-size (at initialization) one-dimensional array; note that the index can start at any value, usually 1;
* *NCollection_Array2* -- fixed-size (at initialization) two-dimensional array; note that the index can start at any value, usually 1;
* *NCollection_List* -- plain list;
* *NCollection_Sequence* -- double-connected list with access by index; note that the index starts at 1.

These classes provide STL-style iterators (methods begin() and end()) and thus can be used in STL algorithms.

@subsection occt_fcug_3_3_3x Maps

NCollection provides several classes for storage of objects by value, providing fast search due to use of hash:
* *NCollection_Map* -- hash set;
* *NCollection_IndexedMap* -- set with a prefixed order of elements, allowing fast access by index or by value (hash-based);
* *NCollection_DataMap* -- hash map;
* *NCollection_IndexedDataMap* -- map with a prefixed order of elements, allowing fast access by index or by value (hash-based);
* *NCollection_DoubleMap* -- two-side hash map (with two keys).

@subsection occt_fcug_3_3_4 Other collection types

There are 4 collection types provided as template classes:
* *NCollection_Vector*
* *NCollection_UBTree*
* *NCollection_SparseArray*
* *NCollection_CellFilter*

### Vector

This type is implemented internally as a list of arrays of the same size. Its properties:
* Direct (constant-time) access to members like in Array1 type; data are allocated in compact blocks, this provides faster iteration.
* Can grow without limits, like List, Stack or Queue types.
* Once having the size LEN, it cannot be reduced to any size less than LEN -- there is no operation of removal of items.

Insertion in a Vector-type class is made by two methods:
* _SetValue(ind, theValue)_ -- array-type insertion, where ind is the index of the inserted item, can be any non-negative number. If it is greater than or equal to Length(), then the vector is enlarged (its Length() grows).
* _Append(theValue)_ -- list-type insertion equivalent to _myVec.SetValue(myVec.Length(), theValue)_, incrementing the size of the collection.

Other essential properties coming from List and Array1 type collections:
* Like in *List*, the method *Clear()* destroys all contained objects and releases the allocated memory.
* Like in *Array1*, the methods *Value()* and *ChangeValue()* return a contained object by index. Also, these methods have the form of overloaded operator ().

### UBTree

The name of this type stands for "Unbalanced Binary Tree". It stores the members in a binary tree of overlapped bounding objects (boxes or else).
Once the tree of boxes of geometric objects is constructed, the algorithm is capable of fast geometric selection of objects. The tree can be easily updated by adding to it a new object with bounding box. 
The time of adding to the tree of one object is O(log(N)), where N is the total number of objects, so the time of building a tree of N objects is O(N(log(N)). The search time of one object is O(log(N)). 

Defining various classes inheriting *NCollection_UBTree::Selector* we can perform various kinds of selection over the same b-tree object.  

The object may be of any type allowing copying. Among the best suitable solutions there can be a pointer to an object, handled object or integer index of object inside some collection. The bounding object may have any dimension and geometry. The minimal interface of *TheBndType* (besides public empty and copy constructor and operator =) used in UBTree algorithm as follows: 

~~~~~
   class MyBndType
   {
    public:
     inline void                   Add (const MyBndType& other);
     // Updates me with other bounding type instance

     inline Standard_Boolean       IsOut (const MyBndType& other) const;
     // Classifies other bounding type instance relatively me

     inline Standard_Real          SquareExtent() const;
     // Computes the squared maximal linear extent of me (for a box it is the squared diagonal of the box).
   };
~~~~~
   

This interface is implemented in types of Bnd package: *Bnd_Box, Bnd_Box2d, Bnd_B2x, Bnd_B3x*.

To select objects you need to define a class derived from *UBTree::Selector* that should redefine the necessary virtual methods to maintain the selection condition. Usually this class instance is also used to retrieve selected objects after search.
The class *UBTreeFiller* is used to randomly populate a *UBTree* instance. The quality of a tree is better (considering the speed of searches) if objects are added to it in a random order trying to avoid the addition of a chain of nearby objects one following another. 
Instantiation of *UBTreeFiller* collects objects to be added, and then adds them at once to the given UBTree instance in a random order using the Fisher-Yates algorithm.
Below is the sample code that creates an instance of *NCollection_UBTree* indexed by 2D boxes (Bnd_B2f), then a selection is performed returning the objects whose bounding boxes contain the given 2D point.

~~~~~
typedef NCollection_UBTree<MyData, Bnd_B2f> UBTree;
typedef NCollection_List<MyData> ListOfSelected;
//! Tree Selector type
class MyTreeSelector : public UBTree::Selector
{
public:
  // This constructor initializes the selection criterion (e.g., a point)

  MyTreeSelector (const gp_XY& thePnt) : myPnt(thePnt) {}
  // Get the list of selected objects

  const ListOfSelected& ListAccepted () const
  { return myList; }
  // Bounding box rejection - definition of virtual method.  @return True if theBox is outside the selection criterion.

  Standard_Boolean Reject  (const Bnd_B2f& theBox) const
  { return theBox.IsOut(myPnt); }
  // Redefined from the base class. Called when the bounding of theData conforms to the selection criterion. This method updates myList.

  Standard_Boolean Accept  (const MyData& theData)
  { myList.Append(theData); }
  private:
  gp_XY          myPnt;
  ListOfSelected myList;
};
. . .
// Create a UBTree instance and fill it with data, each data item having the corresponding 2D box.

UBTree aTree;
NCollection_UBTreeFiller <MyData, Bnd_B2f> aTreeFiller(aTree);
for(;;) {
  const MyData& aData = …;
  const Bnd_B2d& aBox = aData.GetBox();
  aTreeFiller.Add(aData, aBox);
}
aTreeFiller.Fill();
. . .
// Perform selection based on ‘aPoint2d’
MyTreeSelector aSel(aPoint2d);
aTree.Select(aSel);
const ListOfSelected = aSel.ListAccepted();
~~~~~


### SparseArray

This type has almost the same features as Vector but it allows to store items having scattered indices. In Vector, if you set an item with index 1000000, the container will allocate memory for all items with indices in the range 0-1000000. In SparseArray, only one small block of items will be reserved that contains the item with index 1000000.

This class can be also seen as equivalence of *DataMap<int,TheItemType>* with the only one practical difference: it can be much less memory-expensive if items are small (e.g. Integer or Handle). 

This type has both interfaces of DataMap and Vector to access items.

### CellFilter

This class represents a data structure for sorting geometric objects in n-dimensional space into cells, with associated algorithm for fast checking of coincidence (overlapping, intersection, etc.) with other objects. It can be considered as a functional alternative to UBTree, as in the best case it provides the direct access to an object like in an n-dimensional array, while search with UBTree provides logarithmic law access time.

@subsection occt_fcug_3_3_5 Features

NCollection defines some specific features, in addition to the public API inherited from TCollection classes. 

### Iterators

Every collection defines its Iterator class capable of iterating the members in some predefined order. Every Iterator is defined as a subtype of the particular collection type (e.g., MyPackage_StackOfPnt::Iterator ). The order of iteration is defined by a particular collection type. The methods of Iterator are: 

* _void Init (const MyCollection&)_ -- initializes the iterator on the collection object;
* _Standard_Boolean More () const_ -- makes a query if there is another non-iterated member;
* _void Next ()_ -- increments the iterator; 
* _const ItemType& Value () const_ -- returns the current member; 
* _ItemType& ChangeValue () const_ -- returns the mutable current member

~~~~~
typedef Ncollection_Sequence<gp_Pnt>
MyPackage_SequenceOfPnt
void Perform (const MyPackage_SequenceOfPnt& theSequence)
{
	MyPackage_SequenceOfPnt::Iterator anIter (theSequence);
	for (; anIter.More(); anIter.Next()) {
		const gp_Pnt aPnt& = anIter.Value();
....
	}
}
~~~~~

This feature is present only for some classes in *TCollection (Stack, List, Set, Map, DataMap, DoubleMap)*. In *NCollection* it is generalized. 

### Class BaseCollection

There is a common abstract base class for all collections for a given item type (e.g., gp_Pnt). Developer X can arbitrarily name this base class like *MyPackage_BaseCollPnt* in the examples above. This name is further used in the declarations of any (non-abstract) collection class to designate the C++ inheritance. 

This base class has the following public API:
* abstract class Iterator as the base class for all Iterators descried above; 
* _Iterator& CreateIterator () const_ -- creates and returns the Iterator on this collection;
* _Standard_Integer Size () const_ -- returns the number of items in this collection;
* *void Assign (const NCollection_BaseCollection& theOther)* -- copies the contents of the Other to this collection object; 

These members enable accessing any collection without knowing its exact type. In particular, it makes possible to implement methods receiving objects of the abstract collection type: 

~~~~~
#include <NColection_Map.hxx>
typedef NCollection_Map<gp_Pnt> MyPackage_MapOfPnt;
typedef NCollection_BaseCollection<gp_Pnt> MyPackage_BaseCollPnt;
MyPackage_MapOfPnt aMapPnt;
....
gp_Pnt aResult = COG (aMapPnt);
....
gp_Pnt COG(const MyPackage_BaseCollPnt& theColl)
{
  gp_XYZ aCentreOfGravity(0., 0., 0.);
// create type-independent iterator (it is abstract type instance)
  MyPackage_BaseCollString::Iterator& anIter = theColl.CreateIterator();
  for (; anIter.More(); anIter.Next()) {
    aCentreOfGravity += anIter.Value().XYZ();
  }
  return aCentreOfGravity / theColl.Size();
}
~~~~~

Note that there are fundamental differences between the shown type-independent iterator and the iterator belonging to a particular non-abstract collection:
* Type-independent iterator can only be obtained via the call <i>CreateIterator()</i>; the typed iterator -- only via the explicit construction. 
* Type-independent iterator is an abstract class, so it is impossible to copy it or to assign it to another collection object; the typed iterators can be copied and reassigned using the method <i>Init()</i>. 
* Type-independent iterator is actually destroyed when its collection object is destroyed; the typed iterator is destroyed as any other C++ object in the corresponding C++ scope. 

The common point between them is that it is possible to create any number of both types of iterators on the same collection object. 

### Heterogeneous Assign

The semantics of the method *Assign()* has been changed in comparison to *TCollection*. In *NCollection* classes the method *Assign()* is virtual and it receives the object of the abstract *BaseCollection* class (see the previous section). Therefore this method can be used to assign any collection type to any other if only these collections are instantiated on the same *ItemType*. 

For example, conversion of *Map* into *Array1* is performed like this:

~~~~~
#include <NCollection_Map.hxx>
#include <NCollection_Array1.hxx>
typedef NCollection_Map<gp_Pnt> MyPackage_MapOfPnt;
typedef NCollection_Array1<gp_Pnt> MyPackage_Array1OfPnt;
....
MyPackage_MapOfPnt aMapPnt;
....
MyPackage_Array1OfPnt anArr1Pnt (1, aMapPnt.Size());
anArr1Pnt.Assign (aMapPnt); // heterogeneous assignment
~~~~~

There are some aspects to mention:
* Unlike in *TCollection*, in *NCollection* the methods *Assign* and operator= do not coincide. The former is a virtual method defined in the *BaseCollection* class. The latter is always defined in instance classes as a non-virtual inline method and it corresponds exactly to the method *Assign* in *TCollection* classes. Therefore it is always profitable to use operator= instead of *Assign* wherever the types on both sides of assignment are known. 
* If the method *Assign* copies to *Array1* or *Array2* structure, it first checks if the size of the array is equal to the number of items in the copied collection object. If the sizes differ, an exception is thrown, as in *TCollection_Array1.gxx*. 
* Copying to *Map, IndexedMap, DataMap* and *IndexedDataMap* can bring about a loss of data: when two or more copied data items have the same key value, only one item is copied and the others are discarded. It can lead to an error in the code like the following:

~~~~~
MyPackage_Array1OfPnt anArr1Pnt (1, 100); 
MyPackage_MapOfPnt aMapPnt;
....
aMapPnt.Assign(anArr1Pnt);
anArr1Pnt.Assign(aMapPnt);
~~~~~

Objects of classes parameterised with two types (*DoubleMap, DataMap* and  *IndexedDataMap*) cannot be assigned. Their method *Assign* throws the exception *Standard_TypeMismatch* (because it is impossible to check if the passed *BaseCollection* parameter belongs to the same collection type).

### Allocator

All constructors of *NCollection* classes receive the *Allocator* Object as the last parameter. This is an object of a type managed by Handle, inheriting *NCollection_BaseAllocator*, with the following (mandatory) methods redefined:

~~~~~
Standard_EXPORT virtual void* Allocate (const size_t size); 
Standard_EXPORT virtual void Free (void * anAddress);
~~~~~

It is used internally every time when the collection allocates memory for its item(s) and releases this memory. The default value of this parameter (empty *Handle*) designates the use of *NCollection_BaseAllocator* X where the functions *Standard::Allocate* and *Standard::Free* are called. Therefore if the user of *NCollection* does not specify any allocator as a parameter to the constructor of his collection, the memory management will be identical to the one in *TCollection* and other Open CASCADE Technology classes.

Nevertheless, the it is possible to define a custom *Allocator* type to manage the memory in the most optimal or convenient way for his algorithms.

As one possible choice, the class *NCollection_IncAllocator* is included. Unlike *BaseAllocator*, it owns all memory it allocates from the system. Memory is allocated in big blocks (about 20kB) and the allocator keeps track of the amount of occupied memory. The method *Allocate* just increments the pointer to non-occupied memory and returns its previous value. Memory is only released in the destructor of *IncAllocator*, the method *Free* is empty. If used efficiently, this Allocator can greatly improve the performance of OCCT collections.

@section occt_fcug_3_4 Strings

Strings are classes that handle dynamically sized sequences of characters based on
ASCII/Unicode UTF-8 (normal 8-bit character type) and UTF-16/UCS-2 (16-bit character type). They provide editing operations with built-in memory management which make the relative objects easier to use than ordinary character arrays.

String classes provide the following services to manipulate character strings:
 * Editing operations on string objects, using a built-in string manager 
 * Handling of dynamically-sized sequences of characters 
 * Conversion from/to ASCII and UTF-8 strings. 

Strings may also be manipulated by handles and therefore shared.

### TCollection_AsciiString

A variable-length sequence of ASCII characters (normal 8-bit  character type). It provides editing operations with built-in memory management  to make *AsciiString* objects easier to use than ordinary character  arrays. 
*AsciiString* objects follow value  semantics;, that is, they are the actual strings, not handles to strings,  and are copied through assignment. You may use *HAsciiString* objects to  get handles to strings. 

### TCollection_ExtendedString

A variable-length sequence of "extended" (UNICODE)  characters (16-bit character type). It provides editing operations with  built-in memory management to make *ExtendedString* objects easier to use  than ordinary extended character arrays. 

*ExtendedString* objects follow value  semantics;, that is, they are the actual strings, not handles to strings,  and are copied through assignment. You may use *HExtendedString* objects  to get handles to strings. 

### TCollection_HAsciiString

A variable-length sequence of ASCII characters (normal 8-bit  character type). It provides editing operations with built-in memory management  to make *HAsciiString* objects easier to use than ordinary character  arrays. 
*HAsciiString* objects are *handles* to strings. 
  * *HAsciiString* strings may be shared by several objects.
  * You may use an *AsciiString* object to get the actual  string.
*HAsciiString* objects use an *AsciiString* string  as a field. 

### TCollection_HExtendedString

A variable-length sequence of extended; (UNICODE)  characters (16-bit character type). It provides editing operations with  built-in memory management to make *ExtendedString* objects easier to use  than ordinary extended character arrays. 
*HExtendedString* objects are *handles* to  strings. 
  * *HExtendedString* strings may be shared by several objects.
  * You may use an *ExtendedString* object to get the actual  string.
*HExtendedString* objects use an *ExtendedString* string  as a field. 

@subsection occt_fcug_3_4_2 Conversion

*Resource_Unicode* provides functions to convert a non-ASCII *C string* given  in ANSI, EUC, GB or SJIS format, to a Unicode string of extended characters, and vice  versa. 

@section occt_fcug_3_5 Quantities

Quantities are various classes supporting date and time information and fundamental types representing most physical quantities such as  length, area, volume, mass, density, weight, temperature, pressure etc. 

Quantity classes provide the following services:
  * Definition of primitive types representing most of mathematical and physical quantities;
  * Unit conversion tools providing a uniform mechanism for dealing  with quantities and associated physical units: check unit compatibility, perform conversions of values between different units, etc. (see package  *UnitsAPI*)    
  * Resources to manage time information such as dates and time periods 
  * Resources to manage color definition 

A mathematical quantity is characterized by the name and the value (real).

A physical quantity is characterized by the name, the value (real) and the unit. The unit may be either an international unit complying with the International Unit System (SI) or a user defined unit. The unit is managed by the physical quantity user.

The fact that both physical and mathematical quantities are manipulated as real values means that :
  * They are defined as aliases of real values, so all functions provided by the <i>Standard_Real</i> class are available on each quantity.
  * It is possible to mix several physical quantities in a mathematical or physical formula involving real values.

<i>Quantity</i> package includes all commonly used basic physical quantities. 

@section occt_fcug_3_6 Unit Conversion

The *UnitsAPI* global functions are used to convert a  value from any unit into another unit. Conversion is executed among three unit  systems: 
  * the **SI System**,
  * the user’s **Local System**,
  * the user’s **Current System**.
The **SI System** is the standard international unit  system. It is indicated by *SI* in the signatures of the *UnitsAPI* functions. 

The OCCT (former MDTV) System corresponds to the SI  international standard but the length unit and all its derivatives use the millimeter instead of the meter.  

Both systems are proposed by Open CASCADE Technology; the SI  System is the standard option. By selecting one of these two systems, you  define your **Local System** through the *SetLocalSystem* function. The  **Local System** is indicated by *LS* in the signatures of the *UnitsAPI* functions. 
The Local System units can be modified in the working  environment. You define your **Current System** by modifying its units  through the *SetCurrentUnit* function. The Current System is indicated by *Current* in the signatures of the *UnitsAPI* functions. 
A physical quantity is defined by a string (example:  LENGTH). 


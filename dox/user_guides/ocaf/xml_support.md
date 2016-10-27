XML Support {#user_guides__ocaf_xml_support}
===========

@tableofcontents

Writing and reading XML files in OCCT is provided by LDOM package, which constitutes an integral part
of XML OCAF persistence, which is the optional component provided on top of Open CASCADE Technology.

The Light DOM (LDOM) package contains classes maintaining a data structure whose main principles conform to W3C DOM 
Level 1 Recommendations. The purpose of these classes as required by XML OCAF persistence schema is to: 
* Maintain a tree structure of objects in memory representing the XML document. The root of the structure is an object 
of the *LDOM_Document* type. This object contains all the data corresponding to a given XML document and contains one 
object of the *LDOM_Element* type named "document element". The document element contains other *LDOM_Element* objects 
forming a tree. Other types of nodes: *LDOM_Attr, LDOM_Text, LDOM_Comment* and *LDOM_CDATASection* -- represent the
 corresponding XML types and serve as branches of the tree of elements. 
* Provide class *LDOM_Parser* to read XML files and convert them to *LDOM_Document* objects.
* Provide class *LDOM_XmlWriter* to convert *LDOM_Document* to a character stream in XML format and store it in file. 

This package covers the functionality provided by numerous products known as "DOM parsers". Unlike most of them, LDOM 
was specifically developed to meet the following requirements: 
* To minimize the virtual memory allocated by DOM data structures. In average, the amount of memory of LDOM is the same 
as the XML file size (UTF-8). 
* To minimize the time required for parsing and formatting XML, as well as for access to DOM data structures. 

Both these requirements are important when XML files are processed by applications if these files are relatively large 
(occupying megabytes and even hundreds of megabytes). To meet the requirements, some limitations were imposed on the DOM 
Level 1 specification; these limitations are insignificant in applications like OCAF. Some of these limitations can be 
overridden in the course of future developments. The main limitations are:
* No Unicode support as well as various other encodings; only ASCII strings are used in DOM/XML. Note: There is a data 
type *TCollection_ExtendedString* for wide character data. This type is supported by *LDOM_String* as a sequence of
 numbers. 
* Some superfluous methods are deleted: *getPreviousSibling, getParentNode,* etc. 
* No resolution of XML Entities of any kind 
* No support for DTD: the parser just checks for observance of general XML rules and never validates documents. 
* Only 5 available types of DOM nodes: *LDOM_Element, LDOM_Attr, LDOM_Text, LDOM_Comment* and *LDOM_CDATASection*. 
* No support of Namespaces; prefixed names are used instead of qualified names. 
* No support of the interface *DOMException* (no exception when attempting to remove a non-existing node). 

LDOM is dependent on Kernel OCCT classes only. Therefore, it can be used outside OCAF persistence in various algorithms 
where DOM/XML support may be required. 

@section occt_ocaf_9_1 Document Drivers

The drivers for document storage and retrieval  manage  conversion between a transient OCAF
Document in memory and its persistent reflection in a container (disk, memory, network). For XML Persistence, they are
 defined in the package XmlDrivers. 

The main methods (entry points) of these drivers are: 
* *Write()* -- for a storage driver; 
* *Read()* -- for a retrieval driver. 

The most common case (which is implemented in XML Persistence) is writing/reading document to/from a regular OS file. 
Such conversion is performed in two steps: 

First it is necessary to convert the transient document into another form (called persistent), suitable for writing into 
a file, and vice versa. 
In XML Persistence LDOM_Document is used as the persistent form of an OCAF Document and the DOM_Nodes are the persistent
 objects. 
An OCAF Document is a tree of labels with attributes. Its transformation into a persistent form can be functionally
 divided into two parts: 
* Conversion of the labels structure, which is performed by the method XmlMDF::FromTo()
* Conversion of the attributes and their underlying objects, which is performed by the corresponding attribute drivers
 (one driver per attribute type). 

The driver for each attribute is selected from a table of drivers, either by attribute
type (on storage) or by the name of the corresponding DOM_Element (on retrieval).
The table of drivers is created by by methods *XmlDrivers_DocumentStorageDriver::AttributeDrivers()*
and *XmlDrivers_DocumentRetrievalDriver::AttributeDrivers()*. 

Then the persistent document is written into a file (or read from a file). 
In standard persistence Storage and FSD packages contain classes for writing/reading the persistent document into a 
file. In XML persistence *LDOMParser* and *LDOM_XmlWriter* are used instead.

Usually, the library containing document storage and retrieval drivers is loaded at run time by a plugin mechanism. To 
support this in XML Persistence, there is a plugin *XmlPlugin* and a *Factory()* method in the *XmlDrivers* package. 
This method compares passed GUIDs with known GUIDs and returns the corresponding driver or generates an exception if the
 GUID is unknown. 

The application defines which GUID is needed for document storage or retrieval and in which library it should be found. 
This depends on document format and application resources. Resources for XML Persistence and also for standard 
persistence are found in the StdResource unit. They are written for the XmlOcaf document format. 

@section occt_ocaf_9_2 Attribute Drivers

There is one attribute driver for XML persistence for each transient attribute from a set of standard OCAF attributes, 
with the exception of attribute types, which are never stored (pure transient). Standard OCAF attributes are collected 
in six packages, and their drivers also follow this distribution. Driver for attribute <i>T*_*</i> is called 
<i>XmlM*_*</i>. Conversion between transient and persistent form of attribute is performed by two methods *Paste()* of 
attribute driver. 

*XmlMDF_ADriver* is the root class for all attribute drivers.

At the beginning of storage/retrieval process, one instance of each attribute driver is created and appended to driver 
table implemented as *XmlMDF_ADriverTable*.  During OCAF Data storage, attribute drivers are retrieved from the driver 
table by the type of attribute. In the retrieval step, a data map is created linking names of *DOM_Elements* and 
attribute drivers, and then attribute drivers are sought in this map by *DOM_Element* qualified tag names. 

Every transient attribute is saved as a *DOM_Element* (root element of OCAF attribute) with attributes and possibly
 sub-nodes. The name of the root element can be defined in the attribute driver as a string passed to the base class
 constructor. The default is the attribute type name. Similarly, namespace prefixes for each attribute can be set.
 There is no default value, but it is possible to pass NULL or an empty string to store attributes without namespace
 prefixes. 

The basic class *XmlMDF_ADriver* supports errors reporting via the method 
*WriteMessage(const TCollection_ExtendedString&)*. It sends a message string to its message driver which is initialized
 in the constructor with a *Handle(CDM_MessageDriver)* passed from the application by Document Storage/Retrieval Driver. 

@section occt_ocaf_9_3 XML Document Structure

Every XML Document has one root element, which may have attributes and contain other nodes. In OCAF XML Documents the 
root element is named "document" and has attribute "format" with the name of the OCAF Schema used to generate the file. 
The standard XML format is "XmlOcaf". The following elements are sub-elements of \<document\> and should be unique 
entries as its sub-elements, in a specific order. The order is:
* **Element info** -- contains strings identifying the format version and other parameters of the OCAF XML document. 
Normally, data under the element is used by persistence algorithms to correctly retrieve and initialize an OCAF document. 
The data also includes a copyright string. 
* **Element comments** -- consists of an unlimited number of \<comment\> sub-elements containing necessary comment
 strings. 
* **Element label** -- the root label of the document data structure, with the XML attribute "tag" equal to 0. It 
contains all the OCAF data (labels, attributes) as tree of XML elements. Every sub-label is identified by a tag 
(positive integer) defining a unique key for all sub-labels of a label. Every label can contain any number of elements 
representing OCAF attributes (see OCAF Attributes Representation below).
* **Element shapes** -- contains geometrical and topological entities in BRep format. These entities being referenced
 by OCAF attributes written under the element \<label\>. This element is empty if there are no shapes in the document. 
It is only output if attribute driver *XmlMNaming_NamedShapeDriver* has been added to drivers table by 
the *DocumentStorageDriver*.

## OCAF Attributes Representation 

In XML documents, OCAF attributes are elements whose name identifies the OCAF attribute type. These elements may have a
 simple (string or number) or complex (sub-elements) structure, depending on the architecture of OCAF attribute. Every 
XML type for OCAF attribute possesses a unique positive integer "id" XML attribute identifying the OCAF attribute 
throughout the document. To ensure "id" uniqueness, the attribute name "id" is reserved and is only used to indicate and
 identify elements which may be referenced from other parts of the OCAF XML document. 
For every standard OCAF attribute, its XML name matches the name of a C++ class in Transient data model. Generally, the 
XML name of OCAF attribute can be specified in the corresponding attribute driver. 
XML types for OCAF attributes are declared with XML W3C Schema in a few XSD files where OCAF attributes are grouped by
 the package where they are defined. 

## Example of resulting XML file 

The following example is a sample text from an XML file obtained by storing an OCAF document with two labels 
(0: and 0:2) and two attributes -- *TDataStd_Name* (on label 0:) and *TNaming_NamedShape* (on label 0:2). The \<shapes\>
 section contents are replaced by an ellipsis. 

~~~~
<?xml version="1.0" encoding="UTF-8"?> 
<document format="XmlOcaf" xmlns="http://www.opencascade.org/OCAF/XML" xmlns:xsi="http://www.w3.org/2001/XMLSchema-instance"
xsi:schemaLocation="http://www.opencascade.org/OCAF/XML http://www.opencascade.org/OCAF/XML/XmlOcaf.xsd">

<info date="2001-10-04" schemav="0" objnb="3"> 
<iitem>Copyright: Open Cascade, 2001</iitem> 
<iitem>STORAGE_VERSION: PCDM_ReadWriter_1</iitem> 
<iitem>REFERENCE_COUNTER: 0</iitem> 
<iitem>MODIFICATION_COUNTER: 1</iitem> 
</info> 
<comments/> 
<label tag="0"> 
<TDataStd_Name id="1">Document_1</TDataStd_Name> 
<label tag="2"> 
<TNaming_NamedShape id="2" evolution="primitive"> 
<olds/> 
<news> 
<shape tshape="+34" index="1"/> 
</news> 
</TNaming_NamedShape> 
</label> 
</label> 
\<shapes\> 
... 
</shapes> 
</document> 

~~~~

@section occt_ocaf_9_4 XML Schema

The XML Schema defines the class of a document. 

The full structure of OCAF XML documents is described as a set of XML W3C Schema files with definitions of all XML 
element types. The definitions provided cannot be overridden. If any application defines new persistence schemas, it 
can use all the definitions from the present XSD files but if it creates new or redefines existing types, the definition
 must be done under other namespace(s). 

There are other ways to declare XML data, different from W3C Schema, and it should be possible to use them to the extent
 of their capabilities of expressing the particular structure and constraints of our XML data model. However, it must be
 noted that the W3C Schema is the primary format for declarations and as such, it is the format supported for future 
improvements of Open CASCADE Technology, including the development of specific applications using OCAF XML persistence. 

The Schema files (XSD) are intended for two purposes: 
* documenting the data format of files generated by OCAF; 
* validation of documents when they are used by external (non-OCAF) applications, e.g., to generate reports. 

The Schema definitions are not used by OCAF XML Persistence algorithms when saving and restoring XML documents. There
 are internal checks to ensure validity when processing every type of data. 

## Management of Namespaces 

Both the XML format and the XML OCAF persistence code are extensible in the sense that every new development can reuse 
everything that has been created in previous projects. For the XML format, this extensibility is supported by assigning
 names of XML objects (elements) to different XML Namespaces. Hence, XML elements defined in different projects (in
 different persistence libraries) can easily be combined into the same XML documents. An example is the XCAF XML 
persistence built as an extension to the Standard OCAF XML persistence <i>[File XmlXcaf.xsd]</i>. For the correct 
management of Namespaces it is necessary to: 
* Define *targetNamespace* in the new XSD file describing the format. 
* Declare (in *XSD* files) all elements and types in the targetNamespace to appear without a namespace prefix; all other
 elements and types use the appropriate prefix (such as "ocaf:"). 
* Add (in the new *DocumentStorageDriver*) the *targetNamespace* accompanied with its prefix, using method
 *XmlDrivers_DocumentStorageDriver::AddNamespace*. The same is done for all namespaces objects which are used by the new 
persistence, with the exception of the "ocaf" namespace. 
* Pass (in every OCAF attribute driver) the namespace prefix of the *targetNamespace* to the constructor
 of *XmlMDF_ADriver*. 


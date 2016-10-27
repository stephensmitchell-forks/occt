Glossary {#user_guides__ocaf_glossary}
========

* **Application** -- a document container holding all documents containing all application data. <br><br>
* **Application data** -- the data produced by an application, as opposed to data referring to it. <br><br>
* **Associativity of data** -- the ability to propagate modifications made to one document to other documents,
 which refer to such document. Modification propagation is: 
  * unidirectional, that is, from the referenced to the referencing document(s), or
  * bi-directional, from the referencing to the referenced document and vice-versa.<br><br>
* **Attribute** -- a container for application data. An attribute is attached to a label in the hierarchy of the data 
framework. <br><br>
* **Child** -- a label created from another label, which by definition, is the father label. <br><br>
* **Compound document** -- a set of interdependent documents, linked to each other by means of external references. 
These references provide the associativity of data. <br><br>
* **Data framework** -- a tree-like data structure which in OCAF, is a tree of labels with data attached to them in the 
form of attributes. This tree of labels is accessible through the services of the *TDocStd_Document* class. <br><br>
* **Document** -- a container for a data framework which grants access to the data, and is, in its turn, contained by an
 application. A document also allows you to: 
	* Manage modifications, providing Undo and Redo functions 
	* Manage command transactions 
	* Update external links 
	* Manage save and restore options 
	* Store the names of software extensions. <br><br>
* **Driver** -- an abstract class, which defines the communications protocol with a system. <br><br>
* **Entry** -- an ASCII character string containing the tag list of a label. For example:
~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~{.cpp}
0:3:24:7:2:7 
~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
<br>
* **External links** -- references from one data structure to another data structure in another document.
To store these references properly, a label must also contain an external link attribute.<br><br>
* **Father** -- a label, from which other labels have been created. The other labels are, by definition, the children of
 this label. <br><br>
* **Framework** -- a group of co-operating classes which enable a design to be re-used for a given category of problem.
 The framework guides the architecture of the application by breaking it up into abstract  classes, each of which has
 different responsibilities and collaborates in a predefined way. Application developer creates a specialized 
framework by: 
  * defining new classes which inherit from these abstract classes
  * composing framework class instances
  * implementing the services required by the framework.

In C++, the application behavior is implemented in virtual functions redefined in these derived classes. This is known 
as overriding. 

* **GUID** -- Global Universal ID. A string of 37 characters intended to uniquely identify an object. For example:
~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~{.cpp}
2a96b602-ec8b-11d0-bee7-080009dc3333 
~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
<br>
* **Label** -- a point in the data framework, which allows data to be attached to it by means of attributes. It has a
 name in the form of an entry, which identifies its place in the data framework. <br><br>
* **Modified label** -- containing attributes whose data has been modified. <br><br>
* **Reference key** -- an invariant reference, which may refer to any type of data used in an application. In its 
transient form, it is a label in the data framework, and the data is attached to it in the form of attributes. In its 
persistent form, it is an entry of the label. It allows an application to recover any entity in the current session or
 in a previous session. <br><br>
* **Resource file** -- a file containing a list of each document’s schema name and the storage and retrieval plug-ins
 for that document. <br><br>
* **Root** -- the starting point of the data framework. This point is the top label in the framework. It is represented 
by the [0] entry and is created at the same time with the document you are working on. <br><br>
* **Scope** -- the set of all the attributes and labels which depend on a given label. <br><br>
* **Tag list** -- a list of integers, which identify the place of a label in the data framework. This list is displayed 
in an entry. <br><br>
* **Topological naming** -- systematic referencing of topological entities so that these entities can still be
 identified after the models they belong to have gone through several steps in modeling. In other words, topological 
naming allows you to track entities through the steps in the modeling process. This referencing is needed when a model
 is edited and regenerated, and can be seen as a mapping of labels and name attributes of the entities in the old
 version of a model to those of the corresponding entities in its new version. Note that if the topology of a model 
changes during the modeling, this mapping may not fully coincide. A Boolean operation, for example, may split edges.<br><br> 
* **Topological tracking** -- following a topological entity in a model through the steps taken to edit and regenerate
 that model. <br><br>
* **Valid label** -- in a data framework, this is a label, which is already recomputed in the scope of regeneration 
sequence and includes the label containing a feature which is to be recalculated. Consider the case of a box to which 
you first add a fillet, then a protrusion feature. For recalculation purposes, only valid labels of each construction 
stage are used. In recalculating a fillet, they are only those of the box and the fillet, not the protrusion feature 
which was added afterwards. <br><br>  

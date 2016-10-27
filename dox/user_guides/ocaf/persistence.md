Persistent Data Storage {#user_guides__ocaf_persistence}
=======================

In OCAF, persistence, that is, the mechanism used to  save a document in a file, is based on an explicit formal
 description of the  data saved.  
 
When you open a document, the application reads the corresponding file and first creates a memory representation of it.
 This representation is then converted to the application data model —  the OCAF-based data structure the application 
operates on. The file's memory representation  consists of objects defined by classes known as persistent. 
   
OCAF includes a ready-to-use schema suitable for most  applications. 
However, it can be extended if needed. 
  
Applications using compound documents extensively (saving data in many files linked together) should implement data
 management services. It is out the scope of OCAF to provide functions such as:
* Version and configuration management of compound documents;
* Querying a referenced document for its referencing documents.

In order to ease the delegation of document management to a data management application, OCAF encapsulates the file
 management functions in a driver (the meta-data driver). You have to implement this driver for your application to
 communicate with the data management system of your choice.

## Schemes of Persistence

There are three schemes of persistence, which you can use to store and retrieve OCAF data (documents):

  * <i> Standard</i> persistence schema, compatible with previous OCAF applications. This schema is deprecated and 
supports only reading of standard attributes (no writing).
  * <i> XmlOcaf</i> persistence, allowing the storage of all OCAF data in XML form
  * <i> BinOcaf</i> persistence, allowing the storage of all OCAF data in binary format form


In an OCAF application you can use any persistence schema or
even all three of them. The choice is made depending on the *Format* string of stored OCAF documents
or automatically by the file header data -- on retrieval.
  
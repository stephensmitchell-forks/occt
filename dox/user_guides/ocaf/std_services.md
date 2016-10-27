Standard Document Services {#user_guides__ocaf_std_services}
==========================

@tableofcontents

Standard documents offer ready-to-use documents containing a TDF-based data framework. Each document can contain only one framework. 

The documents themselves are contained in the instantiation of a class inheriting from *TDocStd_Application*. This application manages the creation, storage and retrieval of documents. 

You can implement undo and redo in your document, and refer from the data framework of one document to that of another one. This is done by means of external link attributes, which store the path and the entry of external links. 

To sum up, standard documents alone provide access to the data framework. They also allow you to: 

  * Update external links
  * Manage the saving and opening of data
  * Manage the undo/redo functionality.


@section occt_ocaf_4_2 Application

As a container for your data framework, you need a document, and your document must be contained in your application. This application will be a class inheriting from *TDocStd_Application*.

## Creating Application

To create an application, use the following syntax. 

~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~{.cpp}
Handle(TDocStd_Application) app 
= new MyApplication_Application (); 
~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
Note that *MyApplication_Application* is a class, which you have to create and which will inherit from *TDocStd_Application*. 

## Creating Document

To the application which you declared in the previous example (4.2.1), you must add the document *doc* as an argument of *TDocStd_Application::NewDocument*. 

~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~{.cpp}
Handle(TDocStd_Document) doc; 
app->NewDocument("NewDocumentFormat", doc); 
~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

## Retrieving Application of Document

To retrieve the application containing your document, you use the syntax below. 

~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~{.cpp}
app = Handle(TDocStd_Application)::DownCast 
(doc->Application()); 
~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
@section occt_ocaf_4_3 The Document

The document contains your data framework, and allows you to retrieve this framework, recover its main label, save it in a file, and open or close this file. 

## Accessing Main Label

To access the main label in the data framework, you use *TDocStd_Document::Main* as in the example below. The main label is the first child of the root label in the data framework, and has the entry 0:1. 

~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~{.cpp}
TDF_Label label = doc->Main(); 
~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

## Retrieving Document from Label

To retrieve the document from a label in its data framework, you use *TDocStd_Document::Get* as in the example below. The argument *label* passed to this method is an instantiation of *TDF_Label*. 
~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~{.cpp}
doc = TDocStd_Document::Get(label); 
~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

\anchor anchor_std_services__save_document
## Saving Document

If in your document you use only standard attributes (from the packages *TDF, TDataStd, TNaming, TFunction, TPrsStd* and *TDocStd*), you just do the following steps: 

* In your application class (which inherits class *TDocStd_Application*) implement two methods:
	+ Formats (TColStd_SequenceOfExtendedString& theFormats), which append to a given sequence <i>\<theFormats\></i> your document format string, for example, "NewDocumentFormat" -- this string is also set in the document creation command 
	+ ResourcesName(), which returns a string with a name of resources file (this file contains a description about the extension of the document, storage/retrieval drivers GUIDs...), for example, "NewFormat" 
* Create the resource file (with name, for example, "NewFormat") with the following strings:

~~~~~
formatlist:NewDocumentFormat 
NewDocumentFormat: New Document Format Version 1.0 
NewDocumentFormat.FileExtension: ndf 
NewDocumentFormat.StoragePlugin: bd696000-5b34-11d1-b5ba-00a0c9064368 
NewDocumentFormat.RetrievalPlugin: bd696001-5b34-11d1-b5ba-00a0c9064368 
NewDocumentFormatSchema: bd696002-5b34-11d1-b5ba-00a0c9064368 
NewDocumentFormat.AttributeStoragePlugin:57b0b826-d931-11d1-b5da-00a0c9064368 
NewDocumentFormat.AttributeRetrievalPlugin:57b0b827-d931-11d1-b5da-00a0c9064368 
~~~~~

* Copy the resource file "Plugin" from $CASROOT/src/StdResource

In order to set the paths for these files it is necessary to set the environments: *CSF_PluginDefaults* and *CSF_NewFormatDefaults*. For example, set the files in the directory *MyApplicationPath/MyResources*: 

~~~~~
setenv CSF_PluginDefaults MyApplicationPath/MyResources 
setenv CSF_NewFormatDefaults MyApplicationPath/MyResources 
~~~~~

Once these steps are taken you may run your application, create documents and Save/Open them.

\anchor anchor_std_services__open_document
## Opening Document from File

To open the document from a file where it has been previously saved, you can use *TDocStd_Application::Open* as in the example below. The arguments are the path of the file and the document saved in this file. 

~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~{.cpp}
app->Open("/tmp/example.caf", doc); 
~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

## Cutting, Copying and Pasting

To cut, copy and paste inside a document, use the class *TDF_CopyLabel*.

In fact, you must define a *Label*, which contains the temporary value of a cut or 
copy operation (say, in <i> Lab_Clipboard</i>). You must also define two other labels:

* The data container (e.g. <i> Lab_source</i>)
* The destination of the copy (e.g. <i> Lab_ Target</i> )

~~~~
    Copy = copy (Lab_Source => Lab_Clipboard)
    Cut = copy + Lab_Source.ForgetAll() // command clear the contents of LabelSource.
    Paste = copy (Lab_Clipboard => Lab_target)
~~~~

So we need a tool to copy all (or a part) of the content of a label and its sub-label,
to another place defined by a label.

~~~~
    TDF_CopyLabel aCopy;
    TDF_IDFilter aFilter (Standard_False);

    //Don't copy TDataStd_TreeNode attribute

     aFilter.Ignore(TDataStd_TreeNode::GetDefaultTreeID());
     aCopy.Load(aSource, aTarget); aCopy.UseFilter(aFilter); aCopy.Perform();

    // copy the data structure to clipboard 

    return aCopy.IsDone(); }
~~~~

The filter is used to forbid copying a specified type of attribute. 

You can also have a look at the class *TDF_Closure*, which can be useful to determine the dependencies of the part you
 want to cut from the document.

@section stdservices_links External Links

External links refer from one document to another. They allow you to update the copy of data  framework later on. 

@image html /user_guides/ocaf/images/ocaf_image012.png  "External links between documents"
@image latex /user_guides/ocaf/images/ocaf_image012.png  "External links between documents"

Note that documents can be copied with or without a possibility of updating an external link. 

## Copying the document

### With the possibility of updating it later

To copy a document with a possibility of updating it later, you use *TDocStd_XLinkTool::CopyWithLink*. 

~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~{.cpp}
Handle(TDocStd_Document) doc1; 
Handle(TDocStd_Document) doc2; 

TDF_Label source = doc1->GetData()->Root(); 
TDF_Label target = doc2->GetData()->Root(); 
TDocStd_XLinkTool XLinkTool; 

XLinkTool.CopyWithLink(target,source); 
~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

Now the target document has a copy of the source document. The copy also has a link in order to update the content of
 the copy if the original changes. 

In the example below, something has changed in the source document. As a result, you need to update the copy in the 
target document. This copy is passed to *TDocStd_XLinkTool::UpdateLink* as the argument *target*. 

~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~{.cpp}
XLinkTool.UpdateLink(target); 
~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

### Without any link between the copy and the original

You can also create a copy of the document with no link between the original and the copy. The syntax to use this option
 is *TDocStd_XLinkTool::Copy*. The copied document is again represented by the argument *target*, and the original 
-- by *source.* 

~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~{.cpp}
XLinkTool.Copy(target, source); 

~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

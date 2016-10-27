OCAF Commands {#user_guides__draw_commands_ocaf}
=============

@tableofcontents

This chapter contains a set of commands for Open CASCADE Technology @ref user_guides__ocaf "Application Framework (OCAF)." 
The Draw commands are found in the DRAWEXE executable.

@section ocaf_application Application
@subsection occt_draw_5_1_1 NewDocument
       
~~~~~
NewDocument docname [format]
~~~~~ 

Creates a new **docname** document with MDTV-Standard or described format. 

~~~~~
# Create new document with default (MDTV-Standard) format 
NewDocument D 

# Create new document with BinOcaf format 
NewDocument D2 BinOcaf 
~~~~~

@subsection occt_draw_5_1_2 IsInSession
      
~~~~~
IsInSession path
~~~~~ 

Returns *0*, if **path** document is managed by the application session, *1* -- otherwise. 
 
~~~~~
IsInSession /myPath/myFile.std 
~~~~~

@subsection occt_draw_5_1_3 ListDocuments
       
~~~~~
ListDocuments
~~~~~ 

Makes a list of documents handled during the session of the application. 

@subsection occt_draw_5_1_4 Open
  
~~~~~
Open path docname [-stream]
~~~~~ 

Retrieves the document of file **docname** in the path **path**. Overwrites the document, if it is already in session. 

option <i>-stream</i> activates usage of alternative interface of OCAF persistence working with C++ streams instead of file names.
 
~~~~~
Open /myPath/myFile.std D
~~~~~ 

@subsection occt_draw_5_1_5 Close
       
~~~~~
Close docname
~~~~~ 

Closes **docname** document. The document is no longer handled by the applicative session. 
 
~~~~~
Close D 
~~~~~

@subsection occt_draw_5_1_6 Save
       
~~~~~
Save docname
~~~~~ 

Saves **docname** active document. 
 
~~~~~
Save D 
~~~~~

@subsection occt_draw_5_1_7 SaveAs
       
~~~~~
SaveAs docname path [-stream]
~~~~~ 

Saves the active document in the file **docname** in the path **path**. Overwrites the file if it already exists.

option <i>-stream</i> activates usage of alternative interface of OCAF persistence working with C++ streams instead of file names.
 
~~~~~
SaveAs D /myPath/myFile.std
~~~~~ 

@section ocaf_basic Basic
@subsection occt_draw_5_2_1 Label
   

~~~~~
Label docname entry
~~~~~

Creates the label expressed by <i>\<entry\></i> if it does not exist.

Example
~~~~~
Label D 0:2
~~~~~

@subsection occt_draw_5_2_2 NewChild
   
~~~~~
NewChild docname [taggerlabel = Root label]
~~~~~
Finds (or creates) a *TagSource* attribute located at father label of <i>\<taggerlabel\></i> and makes a new child label.

Example
~~~~~
# Create new child of root label
NewChild D

# Create new child of existing label
Label D 0:2
NewChild D 0:2
~~~~~

@subsection occt_draw_5_2_3 Children

~~~~~
Children docname label
~~~~~
Returns the list of attributes of label.

Example
~~~~~
Children D 0:2
~~~~~

@subsection occt_draw_5_2_4 ForgetAll
   
~~~~~
ForgetAll docname label
~~~~~
Forgets all attributes of the label.

Example
~~~~~
ForgetAll D 0:2
~~~~~

@subsection occt_draw_5_3 Application commands

@subsection occt_draw_5_3_1  Main
       
~~~~~
Main docname
~~~~~ 

Returns the main label of the framework. 

~~~~~
Main D 
~~~~~

@subsection occt_draw_5_3_2  UndoLimit
       
~~~~~
UndoLimit docname [value=0]
~~~~~ 

Sets the limit on the number of Undo Delta stored. **0** will disable Undo on the document. A negative *value* means that there is no limit. Note that by default Undo is disabled. Enabling it will take effect with the next call to *NewCommand*. Of course, this limit is the same for Redo 
 
~~~~~
UndoLimit D 100 
~~~~~

@subsection occt_draw_5_3_3  Undo
       
~~~~~
Undo docname [value=1]
~~~~~ 

Undoes **value** steps. 
 
~~~~~
Undo D 
~~~~~

@subsection occt_draw_5_3_4  Redo
       
~~~~~
Redo docname [value=1]
~~~~~ 

Redoes **value** steps.
  
~~~~~
Redo D 
~~~~~

@subsection occt_draw_5_3_5  OpenCommand
       
~~~~~
OpenCommand docname
~~~~~ 

Opens a new command transaction. 

~~~~~ 
OpenCommand D
~~~~~ 

@subsection occt_draw_5_3_6  CommitCommand
       
~~~~~
CommitCommand docname
~~~~~ 

Commits the Command transaction. 
 
~~~~~
CommitCommand D
~~~~~ 

@subsection occt_draw_5_3_7  NewCommand
       
~~~~~
NewCommand docname
~~~~~ 

This is a shortcut for Commit and Open transaction. 
 
~~~~~
NewCommand D 
~~~~~

@subsection occt_draw_5_3_8  AbortCommand
       
~~~~~
AbortCommand docname
~~~~~ 

Aborts the Command transaction. 
 
~~~~~
AbortCommand D 
~~~~~

@subsection occt_draw_5_3_9  Copy
       
~~~~~
Copy docname entry Xdocname Xentry
~~~~~ 

Copies the contents of *entry* to *Xentry*. No links are registered. 
 
~~~~~
Copy D1 0:2 D2 0:4 
~~~~~

@subsection occt_draw_5_3_10  UpdateLink
       
~~~~~
UpdateLink docname [entry] 
~~~~~

Updates external reference set at *entry*. 
 
~~~~~
UpdateLink D 
~~~~~

@subsection occt_draw_5_3_11  CopyWithLink
       
~~~~~
CopyWithLink docname entry Xdocname Xentry
~~~~~ 

Aborts the Command transaction. 
Copies the content of *entry* to *Xentry*. The link is registered with an *Xlink* attribute at *Xentry*  label. 
 
~~~~~
CopyWithLink D1 0:2 D2 0:4
~~~~~ 

@subsection occt_draw_5_3_12  UpdateXLinks
       
~~~~~
UpdateXLinks docname entry
~~~~~ 

Sets modifications on labels impacted by external references to the *entry*. The *document* becomes invalid and must be recomputed 
 
~~~~~
UpdateXLinks D 0:2 
~~~~~

@subsection occt_draw_5_3_13  DumpDocument
    
~~~~~
DumpDocument docname
~~~~~ 

Displays parameters of *docname* document. 
 
~~~~~
DumpDocument D 
~~~~~

@section ocaf_data_framework Data Framework
@subsection occt_draw_5_4_1  MakeDF
   
~~~~~
MakeDF dfname
~~~~~ 

Creates a new data framework. 

~~~~~
MakeDF D 
~~~~~

@subsection occt_draw_5_4_2  ClearDF
       
~~~~~
ClearDF dfname
~~~~~ 

Clears a data framework. 
 
~~~~~
ClearDF D 
~~~~~

@subsection occt_draw_5_4_3  CopyDF
       
~~~~~
CopyDF dfname1 entry1 [dfname2] entry2
~~~~~ 

Copies a data framework. 
 
~~~~~
CopyDF D 0:2 0:4 
~~~~~

@subsection occt_draw_5_4_4  CopyLabel
       
~~~~~
CopyLabel dfname fromlabel tolablel
~~~~~ 

Copies a label. 
 
~~~~~
CopyLabel D1 0:2 0:4 
~~~~~

@subsection occt_draw_5_4_5  MiniDumpDF
       
~~~~~
MiniDumpDF dfname
~~~~~ 

Makes a mini-dump of a data framework. 
 
~~~~~
MiniDumpDF D 
~~~~~

@subsection occt_draw_5_4_6  XDumpDF
    
~~~~~
XDumpDF dfname
~~~~~ 

Makes an extended dump of a data framework. 
 
~~~~~
XDumpDF D
~~~~~ 

@section ocaf_att_general General Attributes
@subsection occt_draw_5_5_1  SetInteger
       
~~~~~
SetInteger dfname entry value
~~~~~ 

Finds or creates an Integer attribute at *entry* label and sets *value*. 
 
~~~~~
SetInteger D 0:2 100 
~~~~~

@subsection occt_draw_5_5_2  GetInteger
       
~~~~~
GetInteger dfname entry [drawname]
~~~~~ 

Gets a value of an Integer attribute at *entry* label and sets it to *drawname* variable, if it is defined. 
 
~~~~~
GetInteger D 0:2 Int1 
~~~~~

@subsection occt_draw_5_5_3  SetReal
     
~~~~~
SetReal dfname entry value
~~~~~ 

Finds or creates a Real attribute at *entry* label and sets *value*. 
 
~~~~~
SetReal D 0:2 100. 
~~~~~

@subsection occt_draw_5_5_4  GetReal
       
~~~~~
GetReal dfname entry [drawname]
~~~~~ 

Gets a value of a Real attribute at *entry* label and sets it to *drawname* variable, if it is defined. 
 
~~~~~
GetReal D 0:2 Real1 
~~~~~

@subsection occt_draw_5_5_5  SetIntArray
       
~~~~~
SetIntArray dfname entry lower upper value1 value2 … 
~~~~~

Finds or creates an IntegerArray attribute at *entry* label with lower and upper bounds and sets **value1*, *value2*... 
 
~~~~~
SetIntArray D 0:2 1 4 100 200 300 400
~~~~~ 

@subsection occt_draw_5_5_6  GetIntArray
       
~~~~~
GetIntArray dfname entry
~~~~~ 

Gets a value of an *IntegerArray* attribute at *entry* label. 
 
~~~~~
GetIntArray D 0:2
~~~~~ 

@subsection occt_draw_5_5_7  SetRealArray
       
~~~~~
SetRealArray dfname entry lower upper value1 value2 …
~~~~~ 

Finds or creates a RealArray attribute at *entry* label with lower and upper bounds and sets *value1*, *value2*… 
 
~~~~~
GetRealArray D 0:2 1 4 100. 200. 300. 400. 
~~~~~

@subsection occt_draw_5_5_8  GetRealArray
       
~~~~~
GetRealArray dfname entry
~~~~~ 

Gets a value of a RealArray attribute at *entry* label. 
 
~~~~~
GetRealArray D 0:2 
~~~~~

@subsection occt_draw_5_5_9  SetComment
       
~~~~~
SetComment dfname entry value
~~~~~ 

Finds or creates a Comment attribute at *entry* label and sets *value*. 
 
~~~~~
SetComment D 0:2 "My comment"
~~~~~ 

@subsection occt_draw_5_5_10  GetComment
       
~~~~~
GetComment dfname entry
~~~~~ 

Gets a value of a Comment attribute at *entry* label. 
 
~~~~~
GetComment D 0:2
~~~~~ 

@subsection occt_draw_5_5_11  SetExtStringArray
       
~~~~~
SetExtStringArray dfname entry lower upper value1 value2 …
~~~~~ 

Finds or creates an *ExtStringArray* attribute at *entry* label with lower and upper bounds and sets *value1*, *value2*… 

~~~~~
SetExtStringArray D 0:2 1 3 *string1* *string2* *string3*
~~~~~ 

@subsection occt_draw_5_5_12  GetExtStringArray
       
~~~~~
GetExtStringArray dfname entry
~~~~~ 

Gets a value of an ExtStringArray attribute at *entry* label. 
 
~~~~~
GetExtStringArray D 0:2 
~~~~~

@subsection occt_draw_5_5_13  SetName
   
~~~~~
SetName dfname entry value 
~~~~~

Finds or creates a Name attribute at *entry* label and sets *value*. 
 
~~~~~
SetName D 0:2 *My name* 
~~~~~

@subsection occt_draw_5_5_14  GetName
       
~~~~~
GetName dfname entry 
~~~~~

Gets a value of a Name attribute at *entry* label. 
 
~~~~~
GetName D 0:2 
~~~~~

@subsection occt_draw_5_5_15  SetReference
       
~~~~~
SetReference dfname entry reference 
~~~~~

Creates a Reference attribute at *entry* label and sets *reference*. 
 
~~~~~
SetReference D 0:2 0:4 
~~~~~

@subsection occt_draw_5_5_16  GetReference
       
~~~~~
GetReference dfname entry 
~~~~~

Gets a value of a Reference attribute at *entry* label. 
 
~~~~~
GetReference D 0:2 
~~~~~

@subsection occt_draw_5_5_17  SetUAttribute
       
~~~~~
SetUAttribute dfname entry localGUID 
~~~~~

Creates a UAttribute attribute at *entry* label with *localGUID*. 
 
~~~~~
set localGUID "c73bd076-22ee-11d2-acde-080009dc4422" 
SetUAttribute D 0:2 ${localGUID} 
~~~~~

@subsection occt_draw_5_5_18  GetUAttribute
       
~~~~~
GetUAttribute dfname entry loacalGUID 
~~~~~

Finds a *UAttribute* at *entry* label with *localGUID*. 
 
~~~~~
set localGUID "c73bd076-22ee-11d2-acde-080009dc4422" 
GetUAttribute D 0:2 ${localGUID} 
~~~~~

@subsection occt_draw_5_5_19  SetFunction
       
~~~~~
SetFunction dfname entry ID failure 
~~~~~

Finds or creates a *Function* attribute at *entry* label with driver ID and *failure* index. 
 
~~~~~
set ID "c73bd076-22ee-11d2-acde-080009dc4422" 
SetFunction D 0:2 ${ID} 1 
~~~~~

@subsection occt_draw_5_5_20  GetFunction
     
~~~~~
GetFunction dfname entry ID failure 
~~~~~

Finds a Function attribute at *entry* label and sets driver ID to *ID* variable and failure index to *failure* variable. 
 
~~~~~
GetFunction D 0:2 ID failure 
~~~~~

@subsection occt_draw_5_5_21  NewShape
       
~~~~~
NewShape dfname entry [shape] 
~~~~~

Finds or creates a Shape attribute at *entry* label. Creates or updates the associated *NamedShape* attribute by *shape* if *shape* is defined. 
 
~~~~~
box b 10 10 10 
NewShape D 0:2 b 
~~~~~

@subsection occt_draw_5_5_22  SetShape
       
~~~~~
SetShape dfname entry shape 
~~~~~

Creates or updates a *NamedShape* attribute at *entry* label by *shape*. 
 
~~~~~
box b 10 10 10 
SetShape D 0:2 b 
~~~~~

@subsection occt_draw_5_5_23  GetShape
       
~~~~~
GetShape2 dfname entry shape 
~~~~~

Sets a shape from NamedShape attribute associated with *entry* label to *shape* draw variable. 
 
~~~~~
GetShape2 D 0:2 b 
~~~~~

@section ocaf_att_geom Geometric Attributes
@subsection occt_draw_5_6_1  SetPoint
       
~~~~~
SetPoint dfname entry point
~~~~~ 

Finds or creates a Point attribute at *entry* label and sets *point* as generated in the associated *NamedShape* attribute. 
 
~~~~~
point p 10 10 10 
SetPoint D 0:2 p 
~~~~~

@subsection occt_draw_5_6_2  GetPoint
       
~~~~~
GetPoint dfname entry [drawname] 
~~~~~

Gets a vertex from *NamedShape* attribute at *entry* label and sets it to *drawname* variable, if it is defined. 
 
~~~~~
GetPoint D 0:2 p 
~~~~~

@subsection occt_draw_5_6_3  SetAxis

~~~~~
SetAxis dfname entry axis 
~~~~~

Finds or creates an Axis attribute at *entry* label and sets *axis* as generated in the associated *NamedShape* attribute. 
 
~~~~~
line l 10 20 30 100 200 300 
SetAxis D 0:2 l 
~~~~~

@subsection occt_draw_5_6_4  GetAxis
       
~~~~~
GetAxis dfname entry [drawname] 
~~~~~

Gets a line from *NamedShape* attribute at *entry* label and sets it to *drawname* variable, if it is defined. 
 
~~~~~
GetAxis D 0:2 l 
~~~~~

@subsection occt_draw_5_6_5  SetPlane
       
~~~~~
SetPlane dfname entry plane 
~~~~~

Finds or creates a Plane attribute at *entry* label and sets *plane* as generated in the associated *NamedShape* attribute. 
 
~~~~~
plane pl 10 20 30 -1 0 0 
SetPlane D 0:2 pl 
~~~~~

@subsection occt_draw_5_6_6  GetPlane
       
~~~~~
GetPlane dfname entry [drawname] 
~~~~~

Gets a plane from *NamedShape* attribute at *entry* label and sets it to *drawname* variable, if it is defined. 
 
~~~~~
GetPlane D 0:2 pl 
~~~~~

@subsection occt_draw_5_6_7  SetGeometry
       
~~~~~
SetGeometry dfname entry [type] [shape] 
~~~~~

Creates a Geometry attribute at *entry* label and sets *type* and *shape* as generated in the associated *NamedShape* attribute if they are defined. *type* must be one of the following: *any, pnt, lin, cir, ell, spl, pln, cyl*. 
 
~~~~~
point p 10 10 10 
SetGeometry D 0:2 pnt p 
~~~~~

@subsection occt_draw_5_6_8  GetGeometryType
       
~~~~~
GetGeometryType dfname entry
~~~~~ 

Gets a geometry type from Geometry attribute at *entry* label. 
 
~~~~~
GetGeometryType D 0:2 
~~~~~

@subsection occt_draw_5_6_9  SetConstraint
       
~~~~~
SetConstraint dfname entry keyword geometrie [geometrie …] 
SetConstraint dfname entry "plane" geometrie 
SetConstraint dfname entry "value" value
~~~~~  

1. Creates a Constraint attribute at *entry* label and sets *keyword* constraint between geometry(ies). 
*keyword* must be one of the following: 
*rad, dia, minr, majr, tan, par, perp, concentric, equal, dist, angle, eqrad, symm, midp, eqdist, fix, rigid,* or *from, axis, mate, alignf, aligna, axesa, facesa, round, offset* 
2. Sets plane for the existing constraint. 
3. Sets value for the existing constraint. 
 
~~~~~
SetConstraint D 0:2 "value" 5 
~~~~~

@subsection occt_draw_5_6_10  GetConstraint
       
~~~~~
GetConstraint dfname entry
~~~~~ 

Dumps a Constraint attribute at *entry* label 

 
~~~~~
GetConstraint D 0:2 
~~~~~

@subsection occt_draw_5_6_11  SetVariable

       
~~~~~
SetVariable dfname entry isconstant(0/1) units 
~~~~~

Creates a Variable attribute at *entry* label and sets *isconstant* flag and *units* as a string. 

 
~~~~~
SetVariable D 0:2 1 "mm" 
~~~~~

@subsection occt_draw_5_6_12  GetVariable

       
~~~~~
GetVariable dfname entry isconstant units 
~~~~~

Gets an *isconstant* flag and units of a Variable attribute at *entry* label. 

 
~~~~~
GetVariable D 0:2 isconstant units 
puts "IsConstant=${isconstant}" 
puts "Units=${units}" 
~~~~~

@section ocaf_att_tree Tree Attributes
@subsection occt_draw_5_7_1  RootNode

       
~~~~~
RootNode dfname treenodeentry [ID]
~~~~~ 

Returns the ultimate father of *TreeNode* attribute identified by its *treenodeentry* and its *ID* (or default ID, if *ID* is not defined). 


@subsection occt_draw_5_7_2  SetNode

       
~~~~~
SetNode dfname treenodeentry [ID]
~~~~~ 

Creates a *TreeNode* attribute on the *treenodeentry* label with its tree *ID* (or assigns a default ID, if the *ID* is not defined). 


@subsection occt_draw_5_7_3  AppendNode

       
~~~~~
AppendNode dfname fatherentry childentry [fatherID]
~~~~~ 


Inserts a *TreeNode* attribute with its tree *fatherID* (or default ID, if *fatherID* is not defined) on *childentry* as last child of *fatherentry*. 




@subsection occt_draw_5_7_4  PrependNode

       
~~~~~
PrependNode dfname fatherentry childentry [fatherID]
~~~~~ 


Inserts a *TreeNode* attribute with its tree *fatherID* (or default ID, if *fatherID* is not defined) on *childentry* as first child of *fatherentry*. 


@subsection occt_draw_5_7_5  InsertNodeBefore

       
~~~~~
InsertNodeBefore dfname treenodeentry beforetreenode [ID]
~~~~~ 

Inserts a *TreeNode* attribute with tree *ID* (or default ID, if *ID* is not defined) *beforetreenode* before *treenodeentry*. 


@subsection occt_draw_5_7_6  InsertNodeAfter

       
~~~~~
InsertNodeAfter dfname treenodeentry aftertreenode [ID]
~~~~~ 

Inserts a *TreeNode* attribute with tree *ID* (or default ID, if *ID* is not defined) *aftertreenode* after *treenodeentry*. 


@subsection occt_draw_5_7_7  DetachNode

       
~~~~~
DetachNode dfname treenodeentry [ID]
~~~~~ 

Removes a *TreeNode* attribute with tree *ID* (or default ID, if *ID* is not defined) from *treenodeentry*. 


@subsection occt_draw_5_7_8  ChildNodeIterate

       
~~~~~
ChildNodeIterate dfname treenodeentry alllevels(0/1) [ID]
~~~~~ 


Iterates on the tree of *TreeNode* attributes with tree *ID* (or default ID, if *ID* is not defined). If *alllevels* is set to *1* it explores not only the first, but all the sub Step levels.
 
 
~~~~~
Label D 0:2 
Label D 0:3 
Label D 0:4 
Label D 0:5 
Label D 0:6 
Label D 0:7 
Label D 0:8 
Label D 0:9 

# Set root node 
SetNode D 0:2 

AppendNode D 0:2 0:4 
AppendNode D 0:2 0:5 
PrependNode D 0:4 0:3 
PrependNode D 0:4 0:8 
PrependNode D 0:4 0:9 

InsertNodeBefore D 0:5 0:6 
InsertNodeAfter D 0:4 0:7 

DetachNode D 0:8 


# List all levels 
ChildNodeIterate D 0:2 1 

==0:4 
==0:9 
==0:3 
==0:7 
==0:6 
==0:5 


# List only first levels 
ChildNodeIterate D 0:2 1 

==0:4 
==0:7 
==0:6 
==0:5 
~~~~~

@subsection occt_draw_5_7_9  InitChildNodeIterator

       
~~~~~
InitChildNodeIterator dfname treenodeentry alllevels(0/1) [ID]
~~~~~ 


Initializes the iteration on the tree of *TreeNode* attributes with tree *ID* (or default ID, if *ID* is not defined). If *alllevels* is set to *1* it explores not only the first, but also all sub Step levels. 

 
~~~~~
InitChildNodeIterate D 0:5 1 
set aChildNumber 0 
for {set i 1} {$i  100} {incr i} { 
    if {[ChildNodeMore] == *TRUE*} { 
        puts *Tree node = [ChildNodeValue]* 
        incr aChildNumber 
        ChildNodeNext 
    } 
} 
puts "aChildNumber=$aChildNumber"
~~~~~ 

@subsection occt_draw_5_7_10  ChildNodeMore

       
~~~~~
ChildNodeMore
~~~~~ 

Returns TRUE if there is a current item in the iteration. 


@subsection occt_draw_5_7_11  ChildNodeNext

       
~~~~~
ChildNodeNext
~~~~~ 

Moves to the next Item. 


@subsection occt_draw_5_7_12  ChildNodeValue

       
~~~~~
ChildNodeValue
~~~~~ 

Returns the current treenode of *ChildNodeIterator*. 


@subsection occt_draw_5_7_13  ChildNodeNextBrother

       
~~~~~
ChildNodeNextBrother
~~~~~ 

Moves to the next *Brother*. If there is none, goes up. This method is interesting only with *allLevels* behavior. 

@section ocaf_std_presentation Standard Presentation
@subsection occt_draw_5_8_1  AISInitViewer
       
~~~~~
AISInitViewer docname
~~~~~ 

Creates and sets *AISViewer* attribute at root label, creates AIS viewer window. 

 
~~~~~
AISInitViewer D 
~~~~~

@subsection occt_draw_5_8_2  AISRepaint

       
~~~~~
AISRepaint docname 
~~~~~

Updates the AIS viewer window. 

 
~~~~~
AISRepaint D 
~~~~~

@subsection occt_draw_5_8_3  AISDisplay

       
~~~~~
AISDisplay docname entry [not_update] 
~~~~~

Displays a presantation of *AISobject* from *entry* label in AIS viewer. If *not_update* is not defined then *AISobject* is recomputed and all visualization settings are applied. 

 
~~~~~
AISDisplay D 0:5 
~~~~~

@subsection occt_draw_5_8_4  AISUpdate

       
~~~~~
AISUpdate docname entry 
~~~~~

Recomputes a presentation of *AISobject* from *entry* label and applies the visualization setting in AIS viewer. 

 
~~~~~
AISUpdate D 0:5 
~~~~~

@subsection occt_draw_5_8_5  AISErase

       
~~~~~
AISErase docname entry 
~~~~~

Erases *AISobject* of *entry* label in AIS viewer. 

 
~~~~~
AISErase D 0:5 
~~~~~

@subsection occt_draw_5_8_6  AISRemove

       
~~~~~
AISRemove docname entry 
~~~~~

Erases *AISobject* of *entry* label in AIS viewer, then *AISobject* is removed from *AIS_InteractiveContext*. 

 
~~~~~
AISRemove D 0:5 
~~~~~

@subsection occt_draw_5_8_7  AISSet

       
~~~~~
AISSet docname entry ID 
~~~~~

Creates *AISPresentation* attribute at *entry* label and sets as driver ID. ID must be one of the following: *A* (*axis*), *C* (*constraint*), *NS* (*namedshape*), *G* (*geometry*), *PL* (*plane*), *PT* (*point*). 

 
~~~~~
AISSet D 0:5 NS 
~~~~~

@subsection occt_draw_5_8_8  AISDriver

       
~~~~~
AISDriver docname entry [ID] 
~~~~~

Returns DriverGUID stored in *AISPresentation* attribute of an *entry* label or sets a new one. ID must be one of the following: *A* (*axis*), *C* (*constraint*), *NS* (*namedshape*), *G* (*geometry*), *PL* (*plane*), *PT* (*point*). 

 
~~~~~
# Get Driver GUID 
AISDriver D 0:5 
~~~~~

@subsection occt_draw_5_8_9  AISUnset

       
~~~~~
AISUnset docname entry 
~~~~~

Deletes *AISPresentation* attribute (if it exists) of an *entry* label. 

 
~~~~~
AISUnset D 0:5 
~~~~~

@subsection occt_draw_5_8_10  AISTransparency

       
~~~~~
AISTransparency docname entry [transparency] 
~~~~~

Sets (if *transparency* is defined) or gets the value of transparency for *AISPresentation* attribute of an *entry* label. 

 
~~~~~
AISTransparency D 0:5 0.5 
~~~~~

@subsection occt_draw_5_8_11  AISHasOwnTransparency

       
~~~~~
AISHasOwnTransparency docname entry 
~~~~~

Tests *AISPresentation* attribute of an *entry* label by own transparency. 

 
~~~~~
AISHasOwnTransparency D 0:5 
~~~~~

@subsection occt_draw_5_8_12  AISMaterial

       
~~~~~
AISMaterial docname entry [material] 
~~~~~

Sets (if *material* is defined) or gets the value of transparency for *AISPresentation* attribute of an *entry* label. *material* is integer from 0 to 20 (see @ref occt_draw_4_5_6 "meshmat" command). 

 
~~~~~
AISMaterial D 0:5 5 
~~~~~

@subsection occt_draw_5_8_13  AISHasOwnMaterial

       
~~~~~
AISHasOwnMaterial docname entry 
~~~~~

Tests *AISPresentation* attribute of an *entry* label by own material. 

 
~~~~~
AISHasOwnMaterial D 0:5 
~~~~~

@subsection occt_draw_5_8_14  AISColor

       
~~~~~
AISColor docname entry [color] 
~~~~~

Sets (if *color* is defined) or gets value of color for *AISPresentation* attribute of an *entry* label. *color* is integer from 0 to 516 (see color names in *vsetcolor*). 

 
~~~~~
AISColor D 0:5 25 
~~~~~

@subsection occt_draw_5_8_15  AISHasOwnColor

       
~~~~~
AISHasOwnColor docname entry 
~~~~~

Tests *AISPresentation* attribute of an *entry* label by own color. 

 
~~~~~
AISHasOwnColor D 0:5 
~~~~~

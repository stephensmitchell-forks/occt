Data Exchange Commands {#user_guides__draw_commands_data_exchange}
======================

@tableofcontents

This chapter presents some general information about @ref user_guides__data_exchange "Data Exchange" operations. 

Data Exchange commands are intended for translation files of various formats (IGES,STEP) into OCCT shapes with their 
attributes (colors, layers etc.) 

@section de_general General
These are auxilary commands used for the analysis of result of translation of IGES and STEP files. 

@subsection occt_draw_8_3_1  count

Syntax:      
~~~~~
count <counter> [<selection>] 
~~~~~

Calculates statistics on the entities in the model and outputs a count of entities. 

The optional selection argument, if specified, defines a subset of entities, which are to be taken into account. The first argument should be one of the currently defined counters. 

| Counter | Operation |
| :-------- | :-------- | 
| xst-types | Calculates how many entities of each OCCT type exist | 
| step214-types | Calculates how many entities of each STEP type exist |

**Example:**
~~~~~
count xst-types 
~~~~~

@subsection occt_draw_8_3_2 data

Syntax:      
~~~~~
data <symbol>
~~~~~

Obtains general statistics on the loaded data. 
The information printed by this command depends on the symbol specified. 

**Example:**
~~~~~
# print full information about warnings and fails 
data c 
~~~~~

| Symbol | Output |
| :------ | :------ |
| g | Prints the information contained in the header of the file |
| c or f | Prints messages generated during the loading of the STEP file (when the procedure of the integrity of the loaded data check is performed) and the resulting statistics (f works only with fail messages while c with both fail and warning messages) |
| t | The same as c or f, with a list of failed or warned entities |
| m or l | The same as t but also prints a status for each entity | 
| e | Lists all entities of the model with their numbers, types, validity status etc. |
| R | The same as e but lists only root entities |



@subsection occt_draw_8_3_3  elabel

Syntax:      
~~~~~
elabel <num>
~~~~~

Entities in the IGES and STEP files are numbered in the succeeding order. An entity can be identified either by its number or by its label. Label is the letter ‘#'(for STEP, for IGES use ‘D’) followed by the rank. This command gives us a label for an entity with a known number. 

**Example:**
~~~~~
elabel 84 
~~~~~

@subsection occt_draw_8_3_4  entity

Syntax:      
~~~~~
entity <#(D)>_or_<num> <level_of_information>
~~~~~

The content of an IGES or STEP entity can be obtained by using this command. 
Entity can be determined by its number or label. 
<i>\<level_of_information\></i> has range [0-6]. You can get more information about this level using this command without parameters. 

**Example:**
~~~~~
# full information for STEP entity with label 84 
entity #84 6 
~~~~~

@subsection occt_draw_8_3_5  enum

Syntax:      
~~~~~
enum <#(D)> 
~~~~~

Prints a number for the entity with a given label. 

**Example:**
~~~~~
# give a number for IGES entity with label 21 
enum D21 
~~~~~

@subsection occt_draw_8_3_6  estatus

Syntax:      
~~~~~
estatus <#(D)>_or_<num>
~~~~~

The list of entities referenced by a given entity and the list of entities referencing to it can be obtained by this command. 

**Example:**
~~~~~
estatus #315 
~~~~~

@subsection occt_draw_8_3_7  fromshape

Syntax:      
~~~~~
fromshape <shape_name>
~~~~~

Gives the number of an IGES or STEP entity corresponding to an OCCT shape. If no corresponding entity can be found and if OCCT shape is a compound the command explodes it to subshapes and try to find corresponding entities for them. 

**Example:**
~~~~~
fromshape a_1_23 
~~~~~

@subsection occt_draw_8_3_8  givecount

Syntax:
~~~~~
givecount <selection_name> [<selection_name>]
~~~~~


Prints a number of loaded entities defined by the selection argument.
Possible values of \<selection_name\> you can find in the "IGES FORMAT Users’s Guide".

**Example:**
~~~~~
givecount xst-model-roots 
~~~~~

@subsection occt_draw_8_3_9  givelist

Syntax:      
~~~~~
givelist <selection_name>
~~~~~

Prints a list of a subset of loaded entities defined by the selection argument: 
| Selection | Description |
| :-------- | :----------- |
| xst-model-all | all entities of the model |
| xst-model-roots | all roots |
| xst-pointed | (Interactively) pointed entities (not used in DRAW) |
| xst-transferrable-all | all transferable (recognized) entities |
| xst-transferrable-roots | Transferable roots | 


**Example:**
~~~~~
# give a list of all entities of the model 
givelist xst-model-all 
~~~~~

@subsection occt_draw_8_3_10  listcount

Syntax:     listcount \<counter\> [\<selection\> ...]

Prints a list of entities per each type matching the criteria defined by arguments. 
Optional <i>\<selection\></i> argument, if specified, defines a subset of entities, which are to be taken into account. Argument <i>\<counter\></i>  should be one of the currently defined counters: 

| Counter     | Operation |
| :-----      | :------   |
| xst-types   | Calculates how many entities of each OCCT type exist |
| iges-types  | Calculates how many entities of each IGES type and form exist |
| iges-levels | Calculates how many entities lie in different IGES levels |

**Example:**
~~~~~
listcount xst-types 
~~~~~

@subsection occt_draw_8_3_11  listitems

Syntax:      
~~~~~
listitems 
~~~~~

This command prints a list of objects (counters, selections etc.) defined in the current session. 


@subsection occt_draw_8_3_12  listtypes

Syntax:      
~~~~~
listtypes [<selection_name> ...]
~~~~~

Gives a list of entity types which were encountered in the last loaded file (with a number of entities of each type). The list can be shown not for all entities but for a subset of them. This subset is defined by an optional selection argument. 


@subsection occt_draw_8_3_13  newmodel

Syntax:      
~~~~~
newmodel 
~~~~~

Clears the current model. 


@subsection occt_draw_8_3_14  param

Syntax:      
~~~~~
param [<parameter>] [<value>]
~~~~~

This command is used to manage translation parameters. 
Command without arguments gives a full list of parameters with current values. 
Command with <i>\<parameter\></i> (without <i><value></i>) gives us the current value of this parameter and all possible values for it. Command with <i><value></i> sets this new value to <i>\<parameter\></i>.

**Example:**

Let us get the information about possible schemes for writing STEP file :

~~~~~
param write.step.schema 
~~~~~

@subsection occt_draw_8_3_15  sumcount

Syntax:      
~~~~~
sumcount <counter> [<selection> ...]
~~~~~

Prints only a number of entities per each type matching the criteria defined by arguments. 

**Example:**
~~~~~
sumcount xst-types 
~~~~~

@subsection occt_draw_8_3_16  tpclear

Syntax:      
~~~~~
tpclear  
~~~~~

Clears the map of correspondences between IGES or STEP entities and OCCT shapes. 



@subsection occt_draw_8_3_17  tpdraw

Syntax:      
~~~~~
tpdraw <#(D)>_or_<num>
~~~~~

**Example:**
~~~~~
tpdraw 57 
~~~~~

@subsection occt_draw_8_3_18  tpent

Syntax:      
~~~~~
tpent <#(D)>_or_<num>
~~~~~

Get information about the result of translation of the given IGES or STEP entity.

**Example:**
~~~~~
tpent \#23 
~~~~~

@subsection occt_draw_8_3_19  tpstat

Syntax:      
~~~~~
tpstat [*|?]<symbol> [<selection>]
~~~~~


Provides all statistics on the last transfer, including a list of transferred entities with mapping from IGES or STEP to OCCT types, as well as fail and warning messages. The parameter <i>\<symbol\></i> defines what information will be printed: 

* *g* -- General statistics (a list of results and messages)
* *c* -- Count of all warning and fail messages
* *C* -- List of all warning and fail messages
* *f* -- Count of all fail messages
* *F* -- List of all fail messages
* *n* -- List of all transferred roots
* *s* -- The same, with types of source entity and the type of result
* *b* -- The same, with messages
* *t* -- Count of roots for geometrical types
* *r* -- Count of roots for topological types
* *l* -- The same, with the type of the source entity

The sign \* before parameters *n, s, b, t, r* makes it work on all entities (not only on roots).

The sign ? before *n, s, b, t* limits the scope of information to invalid entities. 

Optional argument \<selection\> can limit the action of the command to the selection, not to all entities. 

To get help, run this command without arguments. 

**Example:**
~~~~~
# translation ratio on IGES faces 
tpstat *l iges-faces 
~~~~~

@subsection occt_draw_8_3_20  xload

Syntax:      
~~~~~
xload <file_name>
~~~~~

This command loads an IGES or STEP file into memory (i.e. to fill the model with data from the file) without creation of an OCCT shape. 

**Example:**
~~~~~
xload /disk1/tmp/aaa.stp 
~~~~~

@section de_iges IGES commands 

@subsection occt_draw_8_1_1  igesread

Reads an IGES file to an OCCT shape. This command will interactively ask the user to select a set of entities to be 
converted. 

> igesread file_name result_shape_name [selection]

| N | Mode | Description |
| :-- | :-- | :---------- |
| 0 | End | finish conversion and exit igesbrep |
| 1 | Visible roots | convert only visible roots |
| 2 | All roots | convert all roots |
| 3 | One entity | convert entity with number provided by the user |
| 4 | Selection | convert only entities contained in selection |


After the selected set of entities is loaded the user will be asked how loaded entities should be converted into OCCT
 shapes (e.g., one shape per root or one shape for all the entities). It is also possible to save loaded shapes in files,
 and to cancel loading. 

The second parameter of this command defines the name of the loaded shape. If several shapes are created, they will get 
indexed names. For instance, if the last parameter was *s*, they will be *s_1, ... s_N*. 

<i>\<selection\></i> specifies the scope of selected entities in the model, by default it is *xst-transferrable-roots*.  
If we use symbol <i>*</i> as <i>\<selection\></i> all roots will be translated. 

See also the detailed description of @ref occt_iges_2_3_4.

~~~~~
# translation all roots from file 
igesread /disk01/files/model.igs a  * 
~~~~~

@subsection occt_draw_8_1_2   tplosttrim
Sometimes the trimming contours of IGES faces (i.e., entity 141 for 143, 142 for 144) can be lost during translation due
 to fails. This command gives us a number of lost trims and the number of corresponding IGES entities. 
It outputs the rank and numbers of faces that lost their trims and their numbers for each type (143, 144, 510) and their
 total number. If a face lost several of its trims it is output only once. 
> tplosttrim [IGES_type] 

Optional parameter <i>\<IGES_type\></i> can be *0TrimmedSurface, BoundedSurface* or *Face* to specify the only type of
 IGES faces. 

~~~~~
tplosttrim TrimmedSurface 
~~~~~

@subsection occt_draw_8_1_3  brepiges
Writes an OCCT shape to an IGES file. 
> brepiges shape_name filename.igs

~~~~~    
# write shape with name aa to IGES file 
brepiges aa /disk1/tmp/aaa.igs 
== unit (write) : MM 
== mode  write  : Faces 
==   To modifiy : command  param 
== 1 Shapes written, giving 345 Entities 
==  Now, to write a file, command : writeall filename 
==  Output on file : /disk1/tmp/aaa.igs 
==  Write OK 
~~~~~

@section occt_draw_8_2  STEP commands 
These commands are used during the translation of STEP models. 

@subsection occt_draw_8_2_1  stepread
Read a STEP file to an OCCT shape. 
> stepread file_name result_shape_name [selection] 

This command will interactively ask the user to select a set of entities to be converted: 

| N | Mode | Description |
| :---- | :---- | :---- |  
| 0 | End | Finish transfer and exit stepread | 
| 1 | root with rank 1 | Transfer first root | 
| 2 | root by its rank | Transfer root specified by its rank | 
| 3 | One entity | Transfer entity with a number provided by the user | 
| 4 | Selection | Transfer only entities contained in selection | 

After the selected set of entities is loaded the user will be asked how loaded entities should be converted into OCCT 
shapes. 
The second parameter of this command defines the name of the loaded shape. If several shapes are created, they will get 
indexed names. For instance, if the last parameter was *s*, they will be *s_1, ... s_N*. 
<i>\<selection\></i> specifies the scope of selected entities in the model. If we use symbol
 <i>*</i> as <i>\<selection\></i> all roots will be translated. 

See also the detailed description of @ref occt_step_2_3_6.

~~~~~
# translation all roots from file 
stepread /disk01/files/model.stp a  * 
~~~~~

@subsection occt_draw_8_2_2   stepwrite
Writes an OCCT shape to a STEP file. 
> stepwrite mode shape_name file_name 


The following  modes are available : 
    * *a* -- as is -- the mode is selected automatically depending on the type & geometry of the shape; 
    * *m* -- *manifold_solid_brep* or *brep_with_voids* 
    * *f* -- *faceted_brep* 
    * *w* -- *geometric_curve_set* 
    * *s* -- *shell_based_surface_model* 
 
For further information see @ref occt_step_3. 

~~~~~
# write shape *a* to a STEP file in mode *0*. 
stepwrite 0 a /disk1/tmp/aaa.igs 
~~~~~

@section de_xde_overview Overview XDE Commands

These commands are used for translation of IGES and STEP files into an XCAF document (special document is inherited from
 CAF document and is intended for Extended Data Exchange (XDE) ) and working with it. XDE translation allows reading and 
writing of shapes with additional attributes -- colors, layers etc. All commands can be divided into the following groups: 
  * XDE general commands
  * XDE shape's commands
  * XDE color's commands
  * XDE layer's commands
  * XDE property's commands

Reminding: All operations of translation are performed with parameters managed by command @ref occt_draw_8_3_14 "param".

@subsection occt_draw_8_4_1  ReadIges

Reads information from an IGES file to an XCAF document. 
> ReadIges document file_name 

~~~~~
ReadIges D /disk1/tmp/aaa.igs 
==> Document saved with name D 
~~~~~

@subsection occt_draw_8_4_2  ReadStep
Reads information from a STEP file to an XCAF document. 

> ReadStep document file_name

~~~~~
ReadStep D /disk1/tmp/aaa.stp 
== Document saved with name D 
~~~~~

@subsection occt_draw_8_4_3  WriteIges

> WriteIges document file_name

~~~~~
WriteIges D /disk1/tmp/aaa.igs 
~~~~~

@subsection occt_draw_8_4_4  WriteStep
Writes information from an XCAF document to a STEP file. 

> WriteStep document file_name

~~~~~
WriteStep D /disk1/tmp/aaa.stp 
~~~~~

@subsection occt_draw_8_4_5  XFileCur
Returns the name of file which is set as the current one in the Draw session. 

> XFileCur  

~~~~~
XFileCur 
== *as1-ct-203.stp* 
~~~~~

@subsection occt_draw_8_4_6  XFileList
Returns a list all files that were transferred by the last transfer. This command is  meant (assigned) for the assemble
 step file. 

> XFileList  

~~~~~
XFileList 
==> *as1-ct-Bolt.stp* 
==> *as1-ct-L-Bracktet.stp* 
==> *as1-ct-LBA.stp* 
==> *as1-ct-NBA.stp* 
==> … 
~~~~~

@subsection occt_draw_8_4_7  XFileSet
Sets the current file taking it from the components list of the assemble file. 

> XFileSet filename 

~~~~~
XFileSet as1-ct-NBA.stp 
~~~~~

@subsection occt_draw_8_4_8  XFromShape
This command is similar to the command @ref occt_draw_8_3_7 "fromshape", but gives additional information about the file 
name. It is useful if a shape was translated from several files. 

> XFromShape shape

~~~~~
XFromShape a 
==> Shape a: imported from entity 217:#26 in file as1-ct-Nut.stp 
~~~~~

@section de_xde_general XDE General
@subsection occt_draw_8_5_1  XNewDoc
Creates a new XCAF document. 

> XNewDoc document

~~~~~
XNewDoc D 
~~~~~

@subsection occt_draw_8_5_2  XShow
Shows a shape from a given label in the 3D viewer. If the label is not given -- shows all shapes from the document. 

> XShow document [ label1 … ]

~~~~~
# show shape from label 0:1:1:4 from document D 
XShow D 0:1:1:4 
~~~~~

@subsection occt_draw_8_5_3  XStat
Prints common information from an XCAF document. 

> XStat document

~~~~~
XStat D 
==>Statistis of shapes in the document: 
==>level N 0 : 9 
==>level N 1 : 18 
==>level N 2 : 5 
==>Total number of labels for shapes in the document = 32 
==>Number of labels with name = 27 
==>Number of labels with color link = 3 
==Number of labels with layer link = 0 
==>Statistis of Props in the document: 
==>Number of Centroid Props = 5 
==>Number of Volume Props = 5 
==>Number of Area Props = 5 
==>Number of colors = 4 
==>BLUE1 RED YELLOW BLUE2 
==>Number of layers = 0 
~~~~~

@subsection occt_draw_8_5_4  XWdump
Saves the contents of the viewer window as an image (XWD, png or BMP file). 
<i>\<filename\></i> must have a corresponding extention. 

> XWdump document filename

~~~~~
XWdump D /disk1/tmp/image.png 
~~~~~

@subsection occt_draw_8_5_5  Xdump

Prints information about the tree structure of the document. If parameter 1 is given, then the tree is printed with a
 link to shapes. 

> Xdump document [int deep {0|1}]

~~~~~
Xdump D 1 
==> ASSEMBLY 0:1:1:1 L-BRACKET(0xe8180448) 
==> ASSEMBLY 0:1:1:2 NUT(0xe82151e8) 
==> ASSEMBLY 0:1:1:3 BOLT(0xe829b000) 
==> ASSEMBLY 0:1:1:4 PLATE(0xe8387780) 
==> ASSEMBLY 0:1:1:5 ROD(0xe8475418) 
==> ASSEMBLY 0:1:1:6 AS1(0xe8476968) 
==>    ASSEMBLY 0:1:1:7 L-BRACKET-ASSEMBLY(0xe8476230) 
==>       ASSEMBLY 0:1:1:1 L-BRACKET(0xe8180448) 
==>       ASSEMBLY 0:1:1:8 NUT-BOLT-ASSEMBLY(0xe8475ec0) 
==>               ASSEMBLY 0:1:1:2 NUT(0xe82151e8) 
==>               ASSEMBLY 0:1:1:3 BOLT(0xe829b000) 
etc. 
~~~~~

@section de_xde_shape XDE Shape Commands
@subsection occt_draw_8_6_1  XAddComponent
Adds a component shape to assembly. 

> XAddComponent document label shape 

~~~~~
# add shape b as component shape to assembly shape from label *0:1:1:1* 
XAddComponent D 0:1:1:1 b 
~~~~~

@subsection occt_draw_8_6_2  XAddShape
Adds a shape (or an assembly) to a document. If this shape already exists in the document, then prints the label which 
points to it. By default, a new shape is added as an assembly (i.e. last parameter 1), otherwise it is necessary to pass
 0 as the last parameter. 

> XAddShape document shape [makeassembly=1]

~~~~~
# add shape b to document D 
XAddShape D b 0 
== 0:1:1:10 
# if pointed shape is compound and last parameter in 
# XAddShape command is used by default (1), then for 
# each subshapes new label is created 
~~~~~

@subsection occt_draw_8_6_3  XFindComponent
Prints a sequence of labels of the assembly path. 

> XFindComponent document shape

~~~~~
XFindComponent D b 
~~~~~

@subsection occt_draw_8_6_4  XFindShape
Finds and prints a label with an indicated top-level shape. 

> XFindShape document shape

~~~~~
XFindShape D a 
~~~~~

@subsection occt_draw_8_6_5  XGetFreeShapes
Print labels or create DRAW shapes for all free shapes in the document. 
If *shape_prefix* is absent -- prints labels, else -- creates DRAW shapes with names 
<i>shape_prefix</i>_num (i.e. for example: there are 3 free shapes and *shape_prefix* = a therefore shapes will be 
created with names a_1, a_2 and a_3). 

> XGetFreeShapes document [shape_prefix]

@note Free shape is a shape to which no other shape refers to. 

~~~~~
XGetFreeShapes D 
== 0:1:1:6 0:1:1:10 0:1:1:12 0:1:1:13 

XGetFreeShapes D sh 
== sh_1 sh_2 sh_3 sh_4 
~~~~~

@subsection occt_draw_8_6_6  XGetOneShape
Creates one DRAW shape for all free shapes from a document. 

> XGetOneShape shape document

~~~~~
XGetOneShape a D 
~~~~~

@subsection occt_draw_8_6_7  XGetReferredShape
Prints a label that contains a top-level shape that corresponds to a shape at a given label. 

> XGetReferredShape document label

~~~~~
XGetReferredShape D 0:1:1:1:1 
~~~~~

@subsection occt_draw_8_6_8  XGetShape
Puts a shape from the indicated label in document to result. 

> XGetShape result document label

~~~~~
XGetShape b D 0:1:1:3 
~~~~~

@subsection occt_draw_8_6_9  XGetTopLevelShapes
Prints labels that contain top-level shapes. 

> XGetTopLevelShapes document

~~~~~
XGetTopLevelShapes D 
== 0:1:1:1 0:1:1:2 0:1:1:3 0:1:1:4 0:1:1:5 0:1:1:6 0:1:1:7 
0:1:1:8 0:1:1:9 
~~~~~

@subsection occt_draw_8_6_10  XLabelInfo
Prints information about a shape, stored at an indicated label. 

> XLabelInfo document label

~~~~~    
XLabelInfo D 0:1:1:6 
==> There are TopLevel shapes. There is an Assembly. This Shape is not used. 
~~~~~

@subsection occt_draw_8_6_11  XNewShape
Creates a new empty top-level shape. 

> XNewShape document

~~~~~
XNewShape D 
~~~~~

@subsection occt_draw_8_6_12  XRemoveComponent
Removes a component from the components label. 

> XRemoveComponent document label

~~~~~
XRemoveComponent D 0:1:1:1:1 
~~~~~

@subsection occt_draw_8_6_13  XRemoveShape
Removes a shape from a document (by it’s label). 

> XRemoveShape document label

~~~~~
XRemoveShape D 0:1:1:2 
~~~~~

@subsection occt_draw_8_6_14  XSetShape
Sets a shape at the indicated label. 

> XSetShape document label shape

~~~~~
XSetShape D 0:1:1:3 b 
~~~~~

@section de_xde_color XDE Color Commands
@subsection occt_draw_8_7_1  XAddColor
Adds color in document to the color table. Parameters R,G,B are real. 

> XAddColor document R G B

~~~~~
XAddColor D 0.5 0.25 0.25 
~~~~~

@subsection occt_draw_8_7_2  XFindColor
Finds a label where the indicated color is situated. 

> XFindColor document R G B

~~~~~
XFindColor D 0.25 0.25 0.5 
==> 0:1:2:2 
~~~~~

@subsection occt_draw_8_7_3  XGetAllColors
Prints all colors that are defined in the document. 

> XGetAllColors document

~~~~~
XGetAllColors D 
==> RED DARKORANGE BLUE1 GREEN YELLOW3 
~~~~~

@subsection occt_draw_8_7_4  XGetColor
Returns a color defined at the indicated label from the color table. 

> XGetColor document label

~~~~~
XGetColor D 0:1:2:3 
== BLUE1 
~~~~~

@subsection occt_draw_8_7_5  XGetObjVisibility
Returns the visibility of a shape. 

> XGetObjVisibility document {label|shape}

~~~~~
XGetObjVisibility D 0:1:1:4 
~~~~~

@subsection occt_draw_8_7_6  XGetShapeColor
Returns the color defined by label. If <i>colortype</i>=’s’ -- returns surface color, else -- returns curve color. 

> XGetShapeColor document label colortype(s|c)

~~~~~
XGetShapeColor D 0:1:1:4 c 
~~~~~

@subsection occt_draw_8_7_7  XRemoveColor
Removes a color from the color table in a document. 

> XRemoveColor document label

~~~~~
XRemoveColor D 0:1:2:1 
~~~~~

@subsection occt_draw_8_7_8  XSetColor
Sets an RGB color to a shape given by label. 

> XSetColor document {label|shape} R G B

~~~~~
XsetColor D 0:1:1:4 0.5 0.5 0. 
~~~~~

@subsection occt_draw_8_7_9  XSetObjVisibility
Sets the visibility of a shape. 

> XSetObjVisibility document {label|shape} {0|1}

~~~~~
# set shape from label 0:1:1:4 as invisible 
XSetObjVisibility D 0:1:1:4 0 
~~~~~

@subsection occt_draw_8_7_10  XUnsetColor
Unset a color given type (‘s’ or ‘c’) for the indicated shape. 

> XUnsetColor document {label|shape} colortype

~~~~~
XUnsetColor D 0:1:1:4 s 
~~~~~

@section de_xde_layer XDE Layer Commands
@subsection occt_draw_8_8_1  XAddLayer
Adds a new layer in an XCAF document. 

> XAddLayer document layer

~~~~~
XAddLayer D layer2 
~~~~~

@subsection occt_draw_8_8_2  XFindLayer
Prints a label where a layer is situated. 

> XFindLayer document layer

~~~~~
XFindLayer D Bolt 
== 0:1:3:2 
~~~~~

@subsection occt_draw_8_8_3  XGetAllLayers
Prints all layers in an XCAF document. 

> XGetAllLayers document 

~~~~~
XGetAllLayers D 
== *0:1:1:3* *Bolt* *0:1:1:9* 
~~~~~

@subsection occt_draw_8_8_4  XGetLayers
Returns names of layers, which are pointed to by links of an indicated shape. 

> XGetLayers document {shape|label}

~~~~~
XGetLayers D 0:1:1:3 
== *bolt* *123* 
~~~~~

@subsection occt_draw_8_8_5  XGetOneLayer
Prints the name of a layer at a given label. 

> XGetOneLayer document label

~~~~~
XGetOneLayer D 0:1:3:2 
~~~~~

@subsection occt_draw_8_8_6  XIsVisible
Returns 1 if the indicated layer is visible, else returns 0. 

> XIsVisible document {label|layer}

~~~~~
XIsVisible D 0:1:3:1 
~~~~~

@subsection occt_draw_8_8_7  XRemoveAllLayers
Removes all layers from an XCAF document. 

> XRemoveAllLayers document

~~~~~
XRemoveAllLayers D 
~~~~~

@subsection occt_draw_8_8_8  XRemoveLayer
Removes the indicated layer from an XCAF document. 

> XRemoveLayer document {label|layer}

~~~~~
XRemoveLayer D layer2 
~~~~~

@subsection occt_draw_8_8_9  XSetLayer
Sets a reference between a shape and a layer (adds a layer if it is necessary). 
Parameter <i>\<shape_in_one_layer\></i> shows whether a shape could be in a number of layers or only in one (0 by
 default). 

> XSetLayer XSetLayer document {shape|label} layer [shape_in_one_layer {0|1}]
 
~~~~~
XSetLayer D 0:1:1:2 layer2 
~~~~~

@subsection occt_draw_8_8_10  XSetVisibility
Sets the visibility of a layer. 

> XSetVisibility document {label|layer} {isvisible {0|1}}

~~~~~
# set layer at label 0:1:3:2 as invisible 
XSetVisibility D 0:1:3:2 0 
~~~~~

@subsection occt_draw_8_8_11  XUnSetAllLayers
Unsets a shape from all layers. 

> XUnSetAllLayers document {label|shape}

~~~~~
XUnSetAllLayers D 0:1:1:2 
~~~~~

@subsection occt_draw_8_8_12  XUnSetLayer
Unsets a shape from the indicated layer. 

> XUnSetLayer document {label|shape} layer

~~~~~
XUnSetLayer D 0:1:1:2 layer1 
~~~~~

@section de_xde_property XDE Property Commands
@subsection occt_draw_8_9_1  XCheckProps
Gets properties for a given shape (*volume*, *area* and <i>centroid</i>) and compares them with the results after
 internal calculations. If the second parameter is 0, the standard OCCT tool is used for the computation of properties.
 If the second parameter is not 0, it is processed as a deflection. If the deflection is positive the computation is
 done by triangulations, if it is negative -- meshing is forced. 

> XCheckProps document [ {0|deflection} [shape|label] ]

~~~~~
# check properties for shapes at label 0:1:1:1 from 
# document using standard Open CASCADE Technology tools 
XCheckProps D 0 0:1:1:1 
== Label 0:1:1:1      ;L-BRACKET* 
==  Area defect:        -0.0 (  0%) 
==  Volume defect:       0.0 (  0%) 
==  CG defect: dX=-0.000, dY=0.000, dZ=0.000 
~~~~~

@subsection occt_draw_8_9_2  XGetArea
Returns the area of a given shape. 

> XGetArea document {shape|label}

~~~~~
XGetArea D 0:1:1:1 
== 24628.31815094999 
~~~~~

@subsection occt_draw_8_9_3  XGetCentroid
Returns the center of gravity coordinates of a given shape. 

> XGetCentroid document {shape|label}

~~~~~
XGetCentroid D 0:1:1:1 
~~~~~

@subsection occt_draw_8_9_4  XGetVolume
Returns the volume of a given shape. 

> XGetVolume document {shape|label}

~~~~~
XGetVolume D 0:1:1:1 
~~~~~

@subsection occt_draw_8_9_5  XSetArea
Sets new area to attribute list ??? given shape. 

> XSetArea document {shape|label} area


~~~~~
XSetArea D 0:1:1:1 2233.99 
~~~~~

@subsection occt_draw_8_9_6  XSetCentroid
Sets new center of gravity  to the attribute list given shape. 

> XSetCentroid document {shape|label} x y z

~~~~~
XSetCentroid D 0:1:1:1 0. 0. 100. 
~~~~~

@subsection occt_draw_8_9_7  XSetMaterial
Adds a new label with material into the material table in a document, and adds a link to this material to the attribute 
list of a given shape or a given label. The last parameter sets the density of a pointed material. 

> XSetMaterial document {shape|label} name density(g/cu sm)

~~~~~
XSetMaterial D 0:1:1:1 Titanium 8899.77 
~~~~~

@subsection occt_draw_8_9_8  XSetVolume
Sets new volume to the attribute list ??? given shape. 

> XSetVolume document {shape|label} volume

~~~~~
XSetVolume D 0:1:1:1 444555.33 
~~~~~

@subsection occt_draw_8_9_9  XShapeMassProps
Computes and returns real mass and real center of gravity for a given shape or for all shapes in a document. The second 
parameter is used for calculation of the volume and CG(center of gravity). If it is 0, then the standard CASCADE tool 
(geometry) is used for computation, otherwise -- by triangulations with a given deflection. 

> XShapeMassProps document [ deflection [{shape|label}] ]

~~~~~
XShapeMassProps D 
== Shape from label : 0:1:1:1 
== Mass = 193.71681469282299 
== CenterOfGravity X = 14.594564763807696,Y = 
    20.20271885211281,Z = 49.999999385313245 
== Shape from label : 0:1:1:2 not have a mass 
etc. 
~~~~~

@subsection occt_draw_8_9_10  XShapeVolume
Calculates the real volume of a pointed shape with a given deflection. 

> XShapeVolume shape deflection

~~~~~
XShapeVolume a 0 
~~~~~




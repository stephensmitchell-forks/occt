Inspector  {#occt_user_guides__inspector}
===============================

@tableofcontents
 
@section occt_inspector_1 Introduction

This manual explains how to use Inspector.

@subsection occt_inspector_1_1 Overview
Inspector is a Qt-based library that provides functionality to interactively inspect low-level content of the OCAF data model, OCCT viewer, etc.
This component is intended primarily as debugging tool for developers of OCCT based applications, as effective means to analyze situations that occur in their applications.

Inspector has a plugin-oriented architecture. That means that for each of mentioned OCCT components there is a plugin, that is embeded in the common framework.
The needed plugin may be defined manually. Each plugin implements logic of a corresponded OCCT component.
This architecture allows extending number of plugins covering OCCT.

Current release contain the following plugins:

| Plugin | OCCT component | Root class of OCCT investigated component |
| :----- | :----- | :----- |
| @ref occt_inspector_2_2 "DFBrowser"| OCAF | TDocStd_Application |
| @ref occt_inspector_2_3 "VInspector"| Visualization | AIS_InteractiveContext |
| @ref occt_inspector_2_4 "ShapeView"| Modelisation Data | TopoDS_Shape |


@subsection occt_inspector_1_2 Contents of this documentation
This documentation describes:
  * TInspector Plugins
  * Common controls
  * TInspectorEXE sample
  * Launch in DRAW Test Harness
  * Using in a custom application
  * Build procedure
  * Sources and packaging
  * Glossary

@subsection occt_inspector_1_3 Getting started
Compile OCCT with switched ON option to build Inspector.
Two modes of lauch are possible:
1. Launch **TInspectorEXE** executable sample. For more details see @ref occt_inspector_6 "TInspectorEXE" section;
2. Launch DRAW, load plugin TOOLS, and use **tinspector** command.
   For more details see @ref occt_inspector_7 "Launch in DRAW Test Harness" section.


@section occt_inspector_2 TInspector Plugins

@subsection occt_inspector_2_1 Overview

TInspector is a framework. It consists of the following components:
  * buttons line to activate the corresponded plugin;
  * view to visualize the plugin content.

@figure{tinspector.png,"Plugins placement in TInspector"}


Each plugin is a toolkit in OCCT.
It should be registeed in Inspector by DLL library plugin name.
It is possible to give into plugin the following parameters to be interpreted inside:
  * container of OCCT handle objects to initialize plugin, e.g. AIS_InteractiveContext for VInspector;
  * container of file names to be opened, e.g. *.BREP files to fill ShapeView
  * container of OCCT handle objects to select element in a plugin if found;
  * container of object names to select element in a plugin, e.g. entry of TDF_Label for DFBrowser

Only one plugin may be active at the moment. Activation happens by a click on the plugin button in the buttons line.


@subsection occt_inspector_2_2 DFBrowser

@subsubsection occt_inspector_2_2_1 Overview

@figure{dfbrowser.png, "DFBrowser"}

This plugin visualizes content of TDocStd_Application in a tree view. It shows documents of the application,
hierarchy of TDF_Labels, content of TDF_Attributes and interconnection between attributes (e.g. references).
Additionally it has 3D view to visualize TopoDS_Shape elements kept in the document.

@subsubsection occt_inspector_2_2_2 Elements

@figure{dfbrowser_elements.png,"DFBrowser Elements"}

<b>OCAF tree view</b>

OCAF element hierarchy is shown in tree view of items. Each OCAF elements has own tree view item:

| Type | Tree item | Text | Description |
| :----- | :----- | :----- | :----- |
| TDocStd_Application | Application | TDocStd_Application | It is the root of tree view. Children are documents.|
| TDocStd_Document | Document | entry : name | It is a child of Application item. Children are Labels and Attributes items.<br> Text view is an entry of the root label and the value of TDataStd_Name attribute for the label if it exists. |
| TDF_Label | Label | entry : name | It is a child of a Document or another Label item. Children and text view are the same as for Document item. |
| TDF_Attribute | Attribute | attribute type [additional information] | It is a child of a Label. It has no children. <br> Text view is the attribute type (DynamicType()->Name() of TDF_Attribute) and additional information (a combination of attribute values) |


Additional information of TDF_Attributes in tree view:

| Type | Text |
| :----- | :----- |
| TDocStd_Owner | [storage format] |
| TDataStd_AsciiString,<br> TDataStd_Name,<br> TDataStd_Real,<br> @ref occt_attribute_simple_types "other Simple types" | [value] |
| TDataStd_BooleanList,<br> TDataStd_ExtStringList,<br> @ref occt_attribute_list_types "other List types" | [value_1 ... value_n] |
| TDataStd_BooleanArray,<br> TDataStd_ByteArray,<br> @ref occt_attribute_array_types "other Array types" | [value_1 ... value_n] |
| TDataStd_TreeNode | [tree node ID  ==> Father()->Label()] (if it has father) or <br> [tree node ID <== First()->Label()] (if it has NO father)|
| TDataStd_TreeNode(XDE) | [@ref occt_attribute_xde_tree_node_id "XDE tree node ID"  ==> Father()->Label()] (if it has father), <br> [@ref occt_attribute_xde_tree_node_id "XDE tree Node ID" <== label_1, ..., label_n] (if it has NO father)|
| TNaming_NamedShape | [shape type : evolution] |
| TNaming_UsedShapes | [map extent] |


Custom color of tree view items:

| OCAF element Type | Color |
| :----- | :----- |
| TDF_Label | <b>dark green</b>, if the label has TDataStd_Name attribute, <br><b>light grey</b> if the label is empty (has no attributes on all levels of hierarchy),<br> <b>black</b> otherwise |
| TNaming_NamedShape | <b>dark gray</b> for TopAbs_FORWARD orientation of TopoDS_Shape, <br> <b>gray</b> for TopAbs_REVERSED orientation of TopoDS_Shape, <br> <b>black</b> for other orientation |


Context popup menu in tree view:
| Action | Functionality |
| :----- | :----- |
| Expand | Expands next two levels under the selected item |
| Expand All | Expand whole tree of the selected item |
| Collapse All | Collapse whole tree of the selected item |


<b>Property Panel</b>

This control is used for content of Label or Attribute tree view items or Search result view.
Information is usually shown in one or several tables.

| Type | Description | Content |
| :----- | :----- | :----- |
| TDF_Label | a table of [entry or attribute name, value] | @figure{property_panel_label.png, ""}

TDF_Attribute has the following content in Property Panel:Z

<table>
<tr><th>Type</th><th>Description</th><th>Content</th></tr>
<tr><td>TDocStd_Owner,<br> @ref occt_attribute_simple_types "Simple types", <br> @ref occt_attribute_list_types "List types"</td>
    <td>a table of [method name, value]</td>
    <td>@figure{property_panel_simple_type.png, ""}</td></tr>
<tr><td>TDataStd_BooleanArray,<br> TDataStd_ByteArray,<br> @ref occt_attribute_array_types "other Array types"</td>
    <td>2 controls: <br>  * a table of [array bound, value], <br>  * table of [method name, value]</td>
    <td>@figure{property_panel_array.png, ""}</td></tr>
<tr><td>TDataStd_TreeNode</td>
    <td>2 controls: <br>  * a table of [Tree ID, value] (visible only if Tree ID() != ID()), <br>  * a tree view of tree nodes starting from Root() of the tree node. The current tree node has <b>dark blue</b> text.</td>
    <td>@figure{property_panel_tree_node.png, ""} </td></tr>
<tr><td>TDataStd_NamedData</td>
    <td>tab bar of attribute elements, each tab has a table of [name, value]</td>
    <td>@figure{property_panel_named_data.png, ""}</td></tr>
<tr><td>TNaming_UsedShapes</td>
    <td>a table of all  the shapes handled by the framework</td>
    <td>@figure{property_panel_tnaming_used_shapes.png, ""}</td></tr>
<tr><td>TNaming_NamedShape</td>
    <td>2 controls: <br>  * a table of [method name, value] including CurrentShape/CurrentShape methods result of TNaming_Tools, <br>  * an evolution table. <br> Tables contain buttons for @ref occt_shape_export "TopoDS_Shape export".</td>
    <td>@figure{property_panel_tnaming_named_shape.png, ""}</td></tr>
<tr><td>TNaming_Naming</td>
    <td>2 controls: <br>  * a table of TNaming_Name vlaues,<br>  * a table of [method name, value]</td>
    <td>@figure{property_panel_tnaming_naming.png, ""}</td></tr>
</table>


<b>Dump view</b>

@figure{dump_attribute.png, "Dump of TDF_Attribute"}

It shows result of <b>Dump()</b> for TDF_Attribute or TDF_Label of selected tree item.

<b>3D view</b>

It visualizes TopoDS_Shape elements of OCAF attribute.
AIS_Shape interactive object is used to visualize this topological shape.

Depending on the selection place, DFBrowser creates two kinds presentations:

<table>
<tr><th>Kind</th><th>Source object</th><th>Visualization propeties</th><th>View</th></tr>
<tr><td>Main presentation</td>
    <td>Tree view item</td>
    <td>Color: a default color for shape type of the current TopoDS_Shape</td>
    <td>@figure{display_main_presentation.png, ""}</td></tr>
<tr><td>Additional presentation</td>
    <td>References in Property panel</td>
    <td>Color: white</td>
    <td>@figure{display_additional_presentation.png, ""}</td></tr>
</table>


Main presentations exist for the next TDF_Attributes:
| Type | Presentation |
| :----- | :----- |
| TPrsStd_AISPresentation | AIS_InteractiveObject of the attribute |
| TNaming_NamedShape | new AIS_Shape for TopoDS_Shape of the attribute  |
| TNaming_Naming | new AIS_Shape for compound of selected TNaming_NamedShape Arguments |


<b>Tree Navigation</b>

This control shows the path to the item selected in tree view. Path is a sequence of label entries and attribute type name leading
to the selected item. Each element in the path is selectable. Click on element in the path makes the corresponded tree view item selected.

Navigation line stores the of selection and has buttons to go to the previous and the next selected items. The buttons are enabled only if
these items are stored in the history.

Update of browser cleans this history.


<b>Update</b>

It synchronizes content of tree view to the current content of OCAF. If OCAF content is different from tree view, the tree is rebult.
If these update influences to the current selected item, the item selection in tree view will be lost.

<b>Search</b>

It is possible to find OCAF element using "Search" control. It accepts the next strings:
  * TDF_Label entry,
  * Attribute dynamic type name
  * TDataStd_Name and TDataStd_Comment attributes value.

@figure{search.png,"Search"}

Typing a value in search line will suggest all possible values filtered by the current input text.
Selection of an item or enter in search line fills property panel with search values.
Click of an item highlights the item in the tree with scroll to item, double click selects the item and fill property
panel by the value.


@subsubsection occt_inspector_2_2_3 Elements cooperation

<b>Tree item selection</b>

Selection of tree view item influences on the following controls:
  * Navigation line
  * Property Panel
  * 3D View (if it is possible to create interactive presentation)
  * Dump View

@figure{dfbrowser_selection_in_tree_view.png,""}

<b>Property Panel item selection </b>

If property panel shows content of TDF_Label:
  * selection of the table row hightlights the corresponding item in tree view,
  * double click of the table row selects this item in tree view.

If property panel shows content of TDF_Attribute that has reference to another attribute, selection of this reference:
  * highlights the referenced item in TreeView,
  * displays additional presentation in 3D view if it can be created.

@figure{property_panel_item_selection.png,""}

Attributes having references:

| Type | Reference | Additional presentation
| :----- | :----- | :----- |
| TDF_Reference | TDF_Label | |
| TDataStd_ReferenceArray, <br> TDataStd_ReferenceList, <br> TNaming_Naming | one or several TDF_Label in a container | |
| TDataStd_TreeNode | TDF_Label | |
| TNaming_NamedShape | TDF_Label in Evolution table | selected TopoDS_Shapes in property panel tables |
| TNaming_UsedShapes | one or several TNaming_NamedShape | TopoDS_Shapes of selected TNaming_NamedShape |


@subsubsection occt_shape_export TopoDS_Shape export

Property panel of TNaming_NamedShape attribute has controls to export TopoDS_Shape to:
  * BREP. The save file dialog is started to enter the result file name.,
  * ShapeView Inspector plugin. Dialog about exporting element to ShapeView is shown with a possibility to activate this plugin immediatelly.


@subsection occt_inspector_2_3 VInspector

@subsubsection occt_inspector_2_3_1 Overview

@figure{vinspector.png, "VInspector"}

It visualizes interactive objects displayed in AIS_InteractiveContext in a tree view with columputed selection
components for each presentation. It shows the selected elements in the context and even allows to select these elements.

@subsubsection occt_inspector_2_3_2 Elements

@figure{vinspector_elements.png,"VInspector Elements"}

<b>Presentations tree view</b>

It shows presentations and selection computed of them. Also, the view has columns with information about state of visualization elements.

VInspector tree items.

| Type | Description |
| :----- | :----- |
| AIS_InteractiveContext | It is the root of tree view. Children are interactive objects obtained by *DisplayedObjects* and *ErasedObjects* methods.|
| AIS_InteractiveObject | It is a child of AIS_InteractiveContext item. Children are SelectMgr_Selection obtained by iteration on *CurrentSelection*  |
| SelectMgr_Selection | It is a child of AIS_InteractiveObject. Children are SelectMgr_SensitiveEntity obtaining by iteration on *Sensitive* |
| SelectMgr_SensitiveEntity | It is a child of SelectMgr_Selection. Children are SelectMgr_SensitiveEntity obtaining by iteration on *OwnerId* |
| SelectBasics_EntityOwner | It is a child SelectMgr_SensitiveEntity. It has no children. |


Custom color of tree view items:

| OCAF element Type | Column | What | Color |
| :----- | :----- | :----- | :----- |
| AIS_InteractiveObject | 0 | Text | <b>dark gray</b>, it is in *ErasedObjects* list of AIS_InteractiveContext,<br> <b>black</b> otherwise |
| AIS_InteractiveObject, <br> SelectMgr_SensitiveEntity, <br> SelectBasics_EntityOwner| 1 | Background | <b>dark blue</b>, if there is a selected owner under the item, <br> <b>black</b> otherwise |
| SelectMgr_Selection,<br> SelectMgr_SensitiveEntity,<br> SelectBasics_EntityOwner| all | Text | <b>dark gray</b>, if *SelectionState* of SelectMgr_Selection is not *SelectMgr_SOS_Activated*,<br> <b>black</b> otherwise |


Context popup menu in tree view:
| Action | Item | Functionality |
| :----- | :----- | :----- |
| Export to ShapeView | AIS_InteractiveObject | Exports TopoDS_Shape of AIS_Interactive presentation to ShapeView plugin. <br> It should be AIS_Shape presentation and ShapeView plugin should be registered in Inspector<br> Dialog about exporting element to ShapeView is shown with a possibility to activate this plugin immediatelly. |
| Show | AIS_InteractiveObject | *Display* presentation in AIS_InteractiveContext |
| Hide | AIS_InteractiveObject | *Erase* presentation from AIS_InteractiveContext |

<b>Update</b>

It synchronizes content of the plugin to the current state of AIS_InteractiveContext.
It updates the presence of items and the current selection for the items.

<b>Selection controls</b>

This controls switch on/off the posibility to set selection in the context from VInspector plugin.

| Action | Tree view item | Functionality |
| :----- | :----- | :----- |
| Select Presentations | AIS_InteractiveObject | Calls *AddOrRemoveSelected* for interactive object of the selected item |
| Select Owners | SelectMgr_EntityOwner or <br> SelectMgr_SensitiveEntity | Calls *AddOrRemoveSelected* for SelectMgr_EntityOwner of the selected item |

Please note, that the initial selection in context will be cleared.
If the button is toggled, the button selection is active. Only one button may be toggled at the moment.


<b>History view</b>

At present this control is under implementation and may be used only in a custom application where inspector is loaded.
To fill this view, VInspectorAPI_CallBack should be redefined in the application and send signals about some actions applyed to context.
After, the call back should be given as parameter in the plugin.
If done, new items will be created in the history view for each action.

@subsubsection occt_inspector_2_3_3 Elements cooperation

Vinspector markes current selected presentations in AIS_InteractiveContext with blue background in tree items. Use "Update" button to synchronize VInspector selected items state to the context.

It is also possible to perform selection in context using "Selection controls" VInspector. However, it should be performed carefully as
it clears the current selection in AIS_InteractiveContext.

| From | To | Action | Result |
| :----- | :----- | :----- | :----- |
| AIS_InteractiveContext | VInspector | perform selection in AIS_InteractiveContext | Click "Update" button in VInspector and check "Selection" column: <br> AIS_InteractiveContext item has anount of selected objects,<br> some of AIS_InteractiveObject have filled value if it selection happens for this presentation or entity owner of it |
| VInspector | AIS_InteractiveContext | activate Selection controls and select one or several elements in tree view | The objects become selected in AIS_InteractiveContext |

@subsection occt_inspector_2_4 ShapeView

@subsubsection occt_inspector_2_4_1 Overview

@figure{shapeview.png, "ShapeView"}

This plugin visualizes content of TopoDS_Shape in a tree view.

@subsubsection occt_inspector_2_4_2 Elements

@figure{shapeview_elements.png,"ShapeView Elements"}

<b>TopoDS_Shape View</b>

Elements of the view are TopoDS_Shape objects.
This shape is exploded into sub-shapes using TopoDS_Iterator to the TopoDS_Shape.
Child sub-shapes are presented in the view as children of the initial shape.
Iterating recursively by all shapes we obtain a tree view of items shown in the ShapeView.


Columns of the View show some information about TopoDS_Shape of the item.
The most informative column is the last column of TopoDS_Vertex and TopoDS_Edge shape types.

For TopoDS_Vertex it contains the point coordinates,

for TopoDS_Edge it contains the first and the last point coordinates, the edge lenght and someother parameters.


Context popup menu in tree view:
| Action | Functionality |
| :----- | :----- |
| Load BREP file | Opens selected file and appends the result TopoDS_Shape into tree view |
| Remove all shape items | Clears tree view |
| BREP view | Shows text view with BREP content of the selected item. It creates BREP file in temporary directory of the plugin. |
| Close All BREP views | Closes all opened text views |
| BREP directory | Displays folder where temporary BREP files have been stored.  |

@subsubsection occt_inspector_2_4_3 Elements cooperation

Selection of one or several items in TopoDS_Shape View creates AIS_Shape presentation for it and displays it in the 3D View.



@section occt_inspector_3 Common controls
@subsection occt_inspector_3_1 3D View

@subsubsection occt_inspector_3_1_1 Overview

@figure{3DView.png, "3D View"}

Control for OCCT 3D viewer. It creates visualization view components with possibilities to perform some
user acions for the view.


@subsubsection occt_inspector_3_1_2 Elements

@figure{3DView_elements.png,"3DView Elements"}

3D View contains:
| Element | Functionality |
| :----- | :----- |
| 3D view | V3d viewer with mouse events processing |
| Context | choice of another context that should be used in the Plugin. <br> It is possible to use the next contexts:<br> Own - context of this view, <br> Exteral - context come with parameters which intialized plugin, <br> None - do not perform visualization at all |
| Multi/Single | Buttons defined what to do with the previous displayed objects: <br> Multi displays new presentations in additional to already displayed, <br> Single removes all previuos displayed presentations |
| Clean | Removes all displayed presentations |
| Fit All,<br> Fit Area, <br> Zoom, Pan, Rotation | Scene manipulation actions |
| Display Mode | Sets *AIS_Shading* or *AIS_WireFrame* display mode for all presentations |


@section occt_inspector_6 TInspectorEXE sample

Inspector functionality may be tested using the sample. It is started by the *inspector.bat* script file.
This script accepts plugin DLL library names in arguments that should be loaded, by default it loads all described above plugins.

@figure{TStandaloneEXE.png, "TStandaloneEXE"}

Click on the Open button shows the dialog to select a file:
@figure{TStandaloneEXE_open.png, ""}

Depending on the active plugin, the following files should be selected in the dialog:
OCAF document or STEP files for DFBRowser and BREP files for VInspector and ShapeView plugins.

It is possible to click the file name in the proposed directory, enter it manually or using Browser button.
The last Loading icon becomes enabled if file name is correct.


By default on TInspectorEXE start the next files are opened in the plugins:
| Plugin DLL library name | Files |
| :----- | :----- |
| TKDFBrowser | step/screw.step |
| TKVInspector | occ/hammer.brep |
| TKShapeView | occ/face1.brep, <br> occ/face2.brep |

These files are found by the relative *CSF_OCCTDataPath*.


Source code of TIspectorEXE is a good sample for @ref occt_inspector_8 "Using inspector in a custom application".


@section occt_inspector_7 Launch in DRAW Test Harness

TKToolsDraw project is created to provide DRAW commands for Qt tools. Use INSPECTOR parameter of pload command to download
commands of this library. It contains tinspector command to start Inspector under DRAW.
See more detailed description of the @ref occt_draw_13_1 "tinspector" command.

The simple code to start TInspector with all plugins loaded:

~~~~~
pload INSPECTOR
tinspector
~~~~~

@figure{drawexe_tinspector.png,"tinspector"}

Result of this command is the next:
- all available Plugins are presented in the inspector. These are @ref occt_inspector_2_2 "DFBrowser", @ref occt_inspector_2_3 "VInspector" and  @ref occt_inspector_2_4 "ShapeView".
- DFBrowser is an active plugin
- tree of OCAF is empty.

After, we should create objects in DRAW and update tinspector.

@section occt_inspector_8 Using in a custom application

To use inspector in an application, the next steps should be done:
* Set dependencies to OCCT and Qt in application (Header and Link)
* Create an instance of TINspector_Communicator.
* Register plugins of interest in the communicator by DLL library name
* Initialize communicator with objects that will be investigated
* Set visible true for commumicator


C++ code is similar:
~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~{.cpp}

#include <inspector/TInspector_Communicator.hxx>

static TInspector_Communicator* MyTCommunicator;

void CreateInspector()
{
  NCollection_List<Handle(Standard_Transient)> aParameters;
  //... append parameters in the list

  if (!MyTCommunicator)
  {
    MyTCommunicator = new TInspector_Communicator();

    MyTCommunicator->RegisterPlugin ("TKDFBrowser");
    MyTCommunicator->RegisterPlugin ("TKVInspector");
    MyTCommunicator->RegisterPlugin ("TKShapeView");

    MyTCommunicator->Init (aParameters);
    MyTCommunicator->Activate ("TKDFBrowser");
  }
  MyTCommunicator->SetVisible (true);
}

~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

| Plugin | to be initialized by |
| :----- | :----- |
| TKDFBrowser | TDocStd_Application |
| TKVInspector | AIS_InteractiveContext |
| TKShapeView | TopoDS_TShape |



@section occt_inspector_4 Build procedure

By default the inspector compilation is off.
To compile inspector, set the <b>BUILD_Inspector</b> flag to "ON". See @ref build_cmake_conf "Configuration process".

When this option is switched On, MS Visual Studio project has an additional tree of folders:

@figure{VStudio_projects.png,"Inspector packages in MS Visual Studio"}


@section occt_inspector_5 Sources and packaging

OCCT sources are extended by the /tools directory.

Distribution of packages participated in plugins:
| Sources packages| Plugin |
| :----- | :----- |
| DFBrowser, <br> DFBrowserPane, <br> DFBrowserPaneXDE, <br> TKDFBrowser | DFBrowser |
| VInspector, <br> TKVInspector  | VInspector |
| ShapeView, <br> TKShapeView | ShapeView |

Other packages:
| Sources packages| Used in |
| :----- | :----- |
| TInspectorAPI, <br> TKInspectorAPI | Iterface for connection to plugin. |
| TreeModel, <br> TKTreeView | Items-oriented model to simplify work with GUI tree control. |
| View, <br> TKView | 3D View component |
| TInspector, <br> TKTInspector  | Inspector window where plugins are placed |
| ToolsDraw, <br> TKToolsDraw | Plugin for DRAW to start Inspector |


In MSVC studio the separate folder contains Inspector projects.

@section occt_inspector_9 Glossary
* **Component** -- OCCT part, e.g. OCAF, VISUALIZATION, MODELING and others. 
* **Plugin** -- library that is loaded in some executable/library. Here, the plugins are:
  * DFBrowser,
  * ShapeView,
  * VInspector.

@subsection occt_attribute_simple_types TDF_Attribute Simple types
Types where the content is a single value

| Type | Kind of value |
| :----- | :----- |
| TDataStd_AsciiString | TDataStd_AsciiString |
| TDataStd_Comment | TCollection_ExtendedString |
| TDataStd_Integer | Standard_Integer |
| TDataStd_Name | TCollection_ExtendedString |
| TDataStd_Real | Standard_Real |
| TDF_Reference | TDF_Label |
| TDF_TagSource | Standard_Integer |


@subsection occt_attribute_list_types TDF_Attribute List types

| Type | Kind of value (container of) |
| :----- | :----- |
| TDataStd_BooleanList | Standard_Boolean |
| TDataStd_ExtStringList | TCollection_ExtendedString |
| TDataStd_IntegerList | Standard_Integer |
| TDataStd_RealList | Standard_Real |
| TDataStd_ReferenceList | TDF_Label |

@subsection occt_attribute_array_types TDF_Attribute Array types

| Type | Kind of value (container of) |
| :----- | :----- |
| TDataStd_BooleanArray | Standard_Boolean |
| TDataStd_ByteArray | Standard_Byte |
| TDataStd_ExtStringArray | TCollection_ExtendedString |
| TDataStd_IntegerArray | Standard_Integer |
| TDataStd_RealArray | Standard_Real |
| TDataStd_ReferenceArray | TDF_Label |

@subsection occt_attribute_xde_tree_node_id XDE tree node ID description
| GUID | Text |
| :----- | :----- |
| XCAFDoc::ShapeRefGUID() | Shape Instance Link |
| XCAFDoc::ColorRefGUID (XCAFDoc_ColorGen) | Generic Color Link |
| XCAFDoc::ColorRefGUID (XCAFDoc_ColorSurf) | Surface Color Link |
| XCAFDoc::ColorRefGUID (XCAFDoc_ColorCurv) | Curve Color Link |
| XCAFDoc::DimTolRefGUID() | DGT Link |
| XCAFDoc::DatumRefGUID() | Datum Link |
| XCAFDoc::MaterialRefGUID() | Material Link |


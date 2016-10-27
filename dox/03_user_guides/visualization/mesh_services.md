Mesh Visualization Services {#user_guides__visu_mesh}
==================
@tableofcontents

<i>MeshVS</i> (Mesh Visualization Service) component extends 3D visualization capabilities of Open CASCADE Technology.
 It provides flexible means of displaying meshes along with associated pre- and post-processor data.

From a developer's point of view, it is easy to integrate the *MeshVS* component into any mesh-related application with
 the following guidelines:

* Derive a data source class from the *MeshVS_DataSource* class.<br><br>
* Re-implement its virtual methods, so as to give the <i>MeshVS</i> component access to the application data model.
 This is the most important part of the job, since visualization performance is affected by performance of data 
retrieval methods of your data source class.<br><br>
* Create an instance of <i>MeshVS_Mesh</i> class.<br><br>
* Create an instance of your data source class and pass it to a <i>MeshVS_Mesh</i> object through the
 <i>SetDataSource()</i> method. However, the *DataSource* does not provide any  information on attributes, for example
 nodal colors, but you can apply them in  a special way -- by choosing the appropriate presentation builder.<br><br>
* The presentations of *MeshVS_Mesh* are built with the  presentation builders *MeshVS_PrsBuilder*. Create one or 
several objects of <i>MeshVS_PrsBuilder</i>-derived classes (standard, included in the <i>MeshVS</i> package, or your
 custom ones).<br><br>
* Each <i>PrsBuilder</i> is responsible for drawing a <i> MeshVS_Mesh</i> presentation in a certain display mode(s)
 specified as a <i>PrsBuilder</i> constructor's argument. Display mode is treated by <i>MeshVS</i> classes as a 
combination of bit flags (two least significant bits are used to encode standard display modes: wireframe, shading
 and shrink).<br><br>
* Pass these objects to the <i>MeshVS_Mesh::AddBuilder()</i> method. <i>MeshVS_Mesh</i> takes advantage of improved 
selection highlighting mechanism: it highlights its selected entities itself, with the help of so called "highlighter"
 object. You can set one of <i>PrsBuilder</i> objects to act as a highlighter with the help of a corresponding argument
 of the <i>AddBuilder()</i> method.<br><br>

Visual attributes of the <i>MeshVS_Mesh</i> object (such as shading color, shrink coefficient and so on)  are controlled
 through <i>MeshVS_Drawer</i> object. It maintains a map "Attribute ID --> attribute value" and can be easily extended 
with any number of custom attributes.

In all other respects, <i>MeshVS_Mesh</i> is very similar to any other class derived from <i>AIS_InteractiveObject</i>
 and it should be used accordingly (refer to the description of <i>AIS package</i> in the documentation).

##Code snippets

Add/remove builders using the following methods: 
~~~~~
  MeshVS_Mesh::AddBuilder (const Handle (MeshVS_PrsBuilder) &Builder, Standard_Boolean TreatAsHilighter) 
  MeshVS_Mesh::RemoveBuilder (const Standard_Integer  Index) 
  MeshVS_Mesh::RemoveBuilderById (const Standard_Integer Id) 
~~~~~

There is a set of reserved display and highlighting mode flags for *MeshVS_Mesh*. Mode value is a number of bits that 
allows selecting additional display parameters and combining the following mode flags, which allow displaying mesh in 
wireframe, shading and shrink modes: 
~~~~~
  MeshVS_DMF_WireFrame
  MeshVS_DMF_Shading
  MeshVS_DMF_Shrink
~~~~~  

It is also possible to display  deformed mesh in wireframe, shading or shrink modes usung :  
~~~~~
	MeshVS_DMF_DeformedPrsWireFrame
	MeshVS_DMF_DeformedPrsShading  
	MeshVS_DMF_DeformedPrsShrink
~~~~~  

The following methods represent  different kinds of data :
~~~~~
  MeshVS_DMF_VectorDataPrs  
  MeshVS_DMF_NodalColorDataPrs
  MeshVS_DMF_ElementalColorDataPrs
  MeshVS_DMF_TextDataPrs
  MeshVS_DMF_EntitiesWithData
~~~~~  

The following methods provide selection  and highlighting :
~~~~~
  MeshVS_DMF_SelectionPrs
  MeshVS_DMF_HilightPrs
~~~~~  

*MeshVS_DMF_User* is a user-defined  mode.  

These values will be used by the  presentation builder. 
There is also a set of selection modes flags that can be grouped in a combination of bits: 
  * *MeshVS_SMF_0D*
  * *MeshVS_SMF_Link*
  * *MeshVS_SMF_Face*
  * *MeshVS_SMF_Volume*
  * *MeshVS_SMF_Element* -- groups *0D, Link, Face* and *Volume*  as a bit mask ;
  * *MeshVS_SMF_Node*
  * *MeshVS_SMF_All* -- groups *Element* and *Node* as a bit mask; 
  * *MeshVS_SMF_Mesh*
  * *MeshVS_SMF_Group*

Such an object, for example, can be used for displaying the object and stored in the STL file format: 

~~~~~
// read the data and create a data source
Handle  (StlMesh_Mesh) aSTLMesh = RWStl::ReadFile (aFileName); 
Handle  (XSDRAWSTLVRML_DataSource) aDataSource = new XSDRAWSTLVRML_DataSource (aSTLMesh); 

// create mesh 
Handle  (MeshVS_Mesh) aMesh = new MeshVS(); 
aMesh->SetDataSource  (aDataSource); 

// use default presentation builder
Handle  (MeshVS_MeshPrsBuilder) aBuilder =  new  MeshVS_MeshPrsBuilder (aMesh); 
aMesh->AddBuilder  (aBuilder, Standard_True); 
~~~~~

*MeshVS_NodalColorPrsBuilder* allows representing a mesh  with a color scaled texture mapped on it. 
To do this you should define a color  map for the color scale, pass this map to the presentation builder, 
and define an appropriate value in the range of 0.0 - 1.0 for every node. 

The following example demonstrates how you can do this (check if the view has been set up to display textures): 

~~~~~
// assign nodal builder to the mesh
Handle  (MeshVS_NodalColorPrsBuilder) aBuilder = new MeshVS_NodalColorPrsBuilder 
    	(aMesh,MeshVS_DMF_NodalColorDataPrs | MeshVS_DMF_OCCMask); 
aBuilder->UseTexture  (Standard_True); 

// prepare color map
Aspect_SequenceOfColor  aColorMap; 
aColorMap.Append  ((Quantity_NameOfColor) Quantity_NOC_RED); 
aColorMap.Append  ((Quantity_NameOfColor) Quantity_NOC_BLUE1); 

// assign color scale map  values (0..1) to nodes
TColStd_DataMapOfIntegerReal  aScaleMap; 
...
// iterate through the  nodes and add an node id and an appropriate value to the map
aScaleMap.Bind  (anId, aValue); 
  
// pass color map and color scale values to the builder
aBuilder->SetColorMap  (aColorMap); 
aBuilder->SetInvalidColor  (Quantity_NOC_BLACK); 
aBuilder->SetTextureCoords  (aScaleMap); 
aMesh->AddBuilder  (aBuilder, Standard_True); 
~~~~~
<br>
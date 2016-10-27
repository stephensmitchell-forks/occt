Application Interactive Services {#user_guides__visu_ais}
================================
@tableofcontents 

@section occt_visu_3_1 Introduction 

Application Interactive Services allow managing presentations and dynamic selection in a viewer in a simple and 
transparent manner. 

The central entity for management of visualization and selections is the **Interactive Context**. It is connected to 
the main viewer (and if  need be, the trash bin viewer). It has two operating modes: the Neutral Point and  the local 
visualization and selection context. 

The neutral point, which is the  default mode, allows easily visualizing and selecting interactive objects loaded into
 the context. 

**Local Contexts** can be opened to prepare and use a temporary selection environment without disturbing
the neutral point. It is possible to choose the interactive objects, which you want to act on, the selection modes,
 which you want to activate, and the temporary visualizations, which you will execute. 

When the operation is finished, you close the current local context and return to the state 
in which you were before opening it (neutral point or previous local context).  

**Interactive Objects** are the entities, which are visualized and selected. You can use classes of standard interactive
 objects for which all necessary functions have already been programmed, or you can implement your own classes of 
interactive objects, by respecting a certain number of rules and  conventions described below.  

@image html visualization_image016.png 
@image latex visualization_image016.png 

An Interactive Object is a "virtual" entity, which can be  presented and selected. An Interactive Object can have a 
certain number of specific graphic attributes, such as visualization mode, color and  material. 

When an Interactive Object is visualized, the required graphic attributes are taken from its own **Drawer** if it has 
the required custom attributes or otherwise from the context drawer. 

@image html visualization_image017.png 
@image latex visualization_image017.png 

It can be necessary to filter the entities to be selected. Consequently there are **Filter** entities, which allow 
refining the dynamic detection context.  Some of these filters can be used at the Neutral Point, others only in an
 open local context. It is possible to program custom filters and load them into the interactive context.  

@section occt_visu_3_2 Interactive objects

Entities which are visualized and selected in the AIS viewer are objects. They connect the underlying reference geometry
 of a model to its graphic representation in *AIS*. You can use the predefined OCCT classes of standard interactive 
objects, for which all necessary functions have already been programmed, or, if you are an advanced user, you can
 implement your own classes of interactive objects.

@subsection occt_visu_3_2_1 Presentations

An interactive object can have as many  presentations as its creator wants to give it. 

3D presentations are managed by PresentationManager3D. As this is  transparent in AIS, the user does not have to worry 
about it. 

A presentation is identified by an index and by the reference to  the Presentation Manager which it depends on. 

By convention, the default mode of  representation for the Interactive Object has index 0. 

@image html visualization_image018.png 
@image latex visualization_image018.png 

Calculation of different presentations of an interactive  object is done by the *Compute* functions inheriting from 
*PrsMgr_  PresentableObject::Compute* functions. They are automatically called by *PresentationManager*  at a 
visualization or an update request.  

If you are creating your own type of interactive object, you  must implement the Compute function in one of the
 following ways:  

### For 3D:

~~~~~
void PackageName_ClassName::Compute  
	(const  Handle(PrsMgr_PresentationManager3d)& aPresentationManager,   
	 const  Handle(Prs3d_Presentation)& aPresentation,  
	 const Standard_Integer aMode =  0);  
~~~~~

###  For hidden line removal (HLR) mode in 3D:
~~~~~
void PackageName_ClassName::Compute  
	(const  Handle(Prs3d_Projector)& aProjector,  
	 const  Handle(Prs3d_Presentation)& aPresentation);  
~~~~~

@subsection occt_visu_3_2_2 Hidden Line Removal

The view can have two states: the normal mode or the computed  mode (Hidden Line Removal mode). When the latter is
 active, the view looks for all presentations displayed in the normal mode, which have been signalled  as accepting 
HLR mode. An internal mechanism allows calling the interactive  object's own *Compute*, that is projector function. 

By  convention, the Interactive Object accepts or rejects the  representation of HLR mode. It is possible to make this
 declaration in one of two ways:

* Initially by using one of the values of the enumeration  *PrsMgr_TypeOfPresentation*: 
  * *PrsMgr_TOP_AllView*,
  * *PrsMgr_TOP_ProjectorDependant*

* Later by using the function *PrsMgr_PresentableObject::SetTypeOfPresentation* 

*AIS_Shape* class is an example of an interactive object that supports  HLR representation. It supports two types of 
the HLR algorithm:  
* the  polygonal algorithm based on the shape's triangulation; 
* the  exact algorithm that works with the shape's real geometry. 

The type of the HLR algorithm is stored in *AIS_Drawer* of the  shape. It is a value of the *Prs3d_TypeOfHLR* 
enumeration and can be set to: 
	* *Prs3d_TOH_PolyAlgo* for a polygonal algorithm; 
	* *Prs3d_TOH_Algo*  for an exact algorithm; 
	* *Prs3d_TOH_NotSet*  if the type of algorithm is not set for the given interactive object instance. 

The type of the HLR algorithm used for *AIS_Shape* can be changed by calling the *AIS_Shape::SetTypeOfHLR()* method. 

The current  HLR algorithm type can be obtained using *AIS_Shape::TypeOfHLR()* method  is to be used. 

These methods get the  value from the drawer of *AIS_Shape*. If the HLR algorithm type in the *AIS_Drawer* is set to 
*Prs3d_TOH_NotSet*, the *AIS_Drawer* gets the value from the default  drawer of *AIS_InteractiveContext*.

So it is possible to change the default HLR algorithm used by all newly displayed interactive objects. The  value of 
the HLR algorithm type stored in the context drawer can be *Prs3d_TOH_Algo* or *Prs3d_TOH_PolyAlgo*. The polygonal 
algorithm is the default one. 

@subsection occt_visu_3_2_3 Presentation modes

There are four types of interactive objects in AIS:  
  * the "construction element" or Datum, 
  * the Relation (dimensions and constraints) 
  * the Object 
  * the None type (when the object is  of an unknown type). 

Inside these categories, additional characterization is  available by means of a signature (an index.) By default, the 
interactive  object has a NONE type and a signature of 0 (equivalent to NONE.) If you want  to give a particular type 
and signature to your interactive object, you must  redefine two virtual functions:  
  * *AIS_InteractiveObject::Type*  
  * *AIS_InteractiveObject::Signature*.  

@note Some signatures are already used by "standard" objects provided in AIS (see the @ref occt_visu_3_5 "List of 
Standard Interactive Object Classes".  

The interactive context can have a  default mode of representation for the set of interactive objects. This mode may 
not be accepted by a given class of objects. 

Consequently, to get information about this class it is necessary to use virtual function 
*AIS_InteractiveObject::AcceptDisplayMode*.  

### Display Mode

The functions *AIS_InteractiveContext::SetDisplayMode* and *AIS_InteractiveContext::UnsetDisplayMode* allow setting a
 custom display mode for an objects, which can be different from that proposed by the interactive context.

### Highlight Mode

At dynamic detection, the presentation echoed  by the Interactive Context, is by default the presentation already on
 the screen. 

The functions  *AIS_InteractiveObject::SetHilightMode* and *AIS_InteractiveObject::UnSetHilightMode* allow specifying
 the display mode used for highlighting (so called highlight mode), which is valid independently from the active 
representation of the object. It makes no difference whether this choice is  temporary or definitive. 

Note that the same presentation (and consequently the same  highlight mode) is used for highlighting *detected* 
objects and for highlighting *selected* objects, the latter being drawn with a special *selection  color* (refer to the
 section related to *Interactive Context*  services). 

For example, you want to systematically highlight  the wireframe presentation of a shape - non regarding if it is
 visualized in  wireframe presentation or with shading. Thus, you set the highlight mode to *0* in the constructor of
 the interactive object. Do not forget to implement this representation mode in the *Compute* functions.  

### Infinite Status
If you do not want an object to be affected  by a *FitAll* view, you must declare it infinite; you can cancel its 
"infinite" status using *AIS_InteractiveObject::SetInfiniteState* and *AIS_InteractiveObject::IsInfinite* functions. 

Let us take for example the class called *IShape* representing  an interactive object :

~~~~~ 
myPk_IShape::myPK_IShape  
	(const TopoDS_Shape& SH,  PrsMgr_TypeOfPresentation aType):  
	AIS_InteractiveObject(aType), myShape(SH), myDrwr(new AIS_Drawer())  {SetHilightMode(0);}   
void myPk_IShape::Compute  
	(const Handle(PrsMgr_PresentationManager3d)  & PM,  
	 const Handle(Prs3d_Presentation)& P,   
	 const Standard_Integer TheMode) 
{ 
	switch (TheMode){  
	case 0: 
	StdPrs_WFDeflectionShape::Add  (P,myShape,myDrwr);  //algo for  calculation of wireframe presentation break;  
	case 1: 
	StdPrs_ShadedShape::Add  (P,myShape,myDrwr);   //algo for calculation of shading presentation.  
	break; 
	}  
}  
void myPk_IsShape::Compute  
	(const  Handle(Prs3d_Projector)& Prj,  
	const  Handle(Prs3d_Presentation) P)  
{ 
	StdPrs_HLRPolyShape::Add(P,myShape,myDrwr); 
	//Hidden line mode calculation algorithm  
}  
~~~~~

@subsection occt_visu_3_2_4 Selection 

An interactive object can have an indefinite number of selection modes, each representing a "decomposition" into
 sensitive primitives. Each primitive has an <i>owner</i> (*SelectMgr_EntityOwner*) which allows identifying the exact
 interactive object or shape which has been detected (see @ref occt_visu_2_2 "Selection" chapter).  

The set of sensitive primitives, which correspond to a given mode, is stocked in a <b>selection</b>
 (*SelectMgr_Selection*).

Each selection mode is identified by an index. By convention, the default selection mode that allows us to grasp the 
interactive object in its entirety is mode *0*. However, it can be modified in the custom interactive objects using 
method <i>SelectMgr_SelectableObject::setGlobalSelMode()</i>.

The calculation of selection primitives (or sensitive entities) is done by the intermediary of a virtual function, 
*ComputeSelection*. It should be implemented for each type of interactive object that is assumed to have different 
selection modes using the function *AIS_ConnectedInteractive::ComputeSelection*.  

A detailed explanation of the mechanism and the manner of implementing this function has been given in
 @ref occt_visu_2_2 "Selection" chapter.

There are some examples of selection mode calculation for the most widely used interactive object in OCCT -- *AIS_Shape*
 (selection by vertex, by edges, etc). To create new classes of interactive objects with the same selection behavior as
 *AIS_Shape* -- such as vertices and edges -- you must redefine the virtual function 
*AIS_InteractiveObject::AcceptShapeDecomposition*.  

You can change the default selection mode index of a custom interactive object using the following functions:
  * *AIS_InteractiveObject::setGlobalSelMode* sets global selection mode;
  * *AIS_InteractiveObject::GlobalSelectionMode* returns global selection mode of the object;
  * *AIS_InteractiveObject::GlobalSelOwner* returns an entity owner that corresponds to a global selection mode.
  
You also can temporarily change the priority of some interactive objects for selection of the global mode to facilitate
 their graphic detection using the following functions:
  * *AIS_InteractiveObject::HasSelectionPriority* checks if there is a selection priority setting for the  owner; 
  * *AIS_InteractiveObject::SelectionPriority* checks the current priority; 
  * *AIS_InteractiveObject::SetSelectionPriority* sets a priority; 
  * *AIS_InteractiveObject::UnsetSelectionPriority* unsets the priority.
    
@subsection occt_visu_3_2_5 Graphic attributes

Graphic attributes manager, or *AIS Drawer*, stores graphic attributes for specific interactive objects and for
 interactive objects controlled by interactive context.

Initially, all drawer attributes are filled out with the predefined values which will define the default 3D object
 appearance.

When an interactive object is visualized, the required graphic attributes are first taken from its own drawer if one
 exists, or from the context drawer if no specific drawer for that type of object exists.

Keep in mind the following points concerning graphic attributes:
  * Each interactive object can have its own visualization  attributes.<br><br>
  * The set of graphic attributes of an interactive object is stocked  in an *AIS_Drawer*, which is only a
 *Prs3d_Drawer* with the  possibility of a link to another drawer <br><br>
  * By default, the interactive object takes the graphic attributes  of the context in which it is visualized 
(visualization mode, deflection values  for the calculation of presentations, number of isoparameters, color, type of 
 line, material, etc.) <br><br>
  * In the *AIS_InteractiveObject* abstract class, standard attributes including color, line thickness, material, and
 transparency have been privileged. Consequently, there is a certain number of virtual  functions, which allow acting
 on these attributes. Each  new class of interactive object can redefine these functions and change the behavior of the class. 

@image html visualization_image019.png "Figure 13. Redefinition of virtual functions for changes in AIS_Point"
@image latex visualization_image019.png "Figure 13. Redefinition of virtual functions for changes in AIS_Point"

@image html visualization_image020.png "Figure 14. Redefinition of virtual functions for changes in AIS_Shape."
@image latex visualization_image020.png "Figure 14. Redefinition of virtual functions for changes in AIS_Shape."

The  following virtual functions provide settings for color, width, material and transparency:  
  * *AIS_InteractiveObject::UnsetColor* 
  * *AIS_InteractiveObject::SetWidth* 
  * *AIS_InteractiveObject::UnsetWidth* 
  * *AIS_InteractiveObject::SetMaterial   (const Graphic3d_NameOfPhysicalMaterial & aName)* 
  * *AIS_InteractiveObject::SetMaterial   (const Graphic3d_MaterialAspect & aMat)* 
  * *AIS_InteractiveObject::UnsetMaterial* 
  * *AIS_InteractiveObject::SetTransparency* 
  * *AIS_InteractiveObject::UnsetTransparency* 

For other types of attribute, it is  appropriate to change the Drawer of the object directly using:  
  * *AIS_InteractiveObject::SetAttributes*
  * *AIS_InteractiveObject::UnsetAttributes*

It is important to know which functions may imply the recalculation of  presentations of the object. 

If the presentation mode of an interactive object is to be updated, a flag from *PrsMgr_PresentableObject* indicates this. 

The mode can be updated using the functions *Display*  and *Redisplay* in *AIS_InteractiveContext*.  

@subsection occt_visu_3_2_6 Complementary  Services

When you use complementary services for interactive objects, pay special attention to the cases mentioned below. 

### Change the location of an interactive object

The following functions  allow temporarily "moving" the representation and selection of  Interactive Objects in a
 view without recalculation.  
  * *AIS_InteractiveContext::SetLocation*  
  * *AIS_InteractiveContext::ResetLocation*  
  * *AIS_InteractiveContext::HasLocation*  
  * *AIS_InteractiveContext::Location*  
 
### Connect an interactive object to an applicative  entity

Each Interactive Object has functions that allow attributing it an *Owner* in form of a *Transient*.  
  * *AIS_InteractiveObject::SetOwner*  
  * *AIS_InteractiveObject::HasOwner*  
  * *AIS_InteractiveObject::Owner*  

An interactive object can therefore be associated or not with an applicative entity, without affecting its behavior. 

### Resolving coincident topology

Due to the fact that the accuracy of three-dimensional  graphics coordinates has a finite resolution the elements of
 topological  objects can coincide producing the effect of "popping" some elements one over  another. 

To the problem when the elements of two or  more Interactive Objects are coincident you can apply the polygon offset. 
It is a  sort of graphics computational offset, or depth buffer offset, that allows you  to arrange elements (by
 modifying their depth value) without changing their  coordinates. The graphical elements that accept this kind of
 offsets are solid  polygons or displayed as boundary lines and points. The polygons could be  displayed as lines or
 points by setting the appropriate interior style. 

The method *AIS_InteractiveObject::SetPolygonOffsets (const Standard_Integer aMode, const Standard_Real aFactor, const
 Standard_Real aUnits)* allows setting up the polygon  offsets.

The  parameter *aMode* can contain various combinations of *Aspect_PolygonOffsetMode* enumeration elements: 
  * *Aspect_POM_None*
  * *Aspect_POM_Off*
  * *Aspect_POM_Fill*
  * *Aspect_POM_Line*
  * *Aspect_POM_Point*
  * *Aspect_POM_All*

The combination  of these elements defines the polygon display modes that will use the given  offsets. You can switch 
off the polygon offsets by passing *Aspect_POM_Off*.  Passing *Aspect_POM_None* allows changing the *aFactor* and 
*aUnits* values  without changing the mode. If *aMode* is different from *Aspect_POM_Off*, the  *aFactor* and *aUnits* 
arguments are used by the graphics renderer to calculate the  depth offset value: 
~~~~~
 offset  = aFactor * m + aUnits * r
~~~~~
where  *m* is the maximum depth slope for the currently displayed polygons, r is the minimum  depth resolution 
(implementation-specific). 

Negative  offset values move polygons closer to the viewer while positive values shift  polygons away. 

@note This  method has a side effect -- it creates its own shading aspect if not yet  created, so it is better to set 
up the object shading aspect first. 

You can use the following  functions to obtain the current settings for polygon offsets: 
~~~~~
  void  AIS_InteractiveObject::PolygonOffsets
		(Standard_Integer &aMode, 
		 Standard_Real &aFactor, 
		 Standard_Real &aUnits) 
  Standard_Boolean AIS_InteractiveObject::HasPolygonOffsets() 
~~~~~

The same  operation could be performed for the interactive object known by the *AIS_InteractiveContext* with the 
following methods: 
~~~~~
void  AIS_InteractiveContext::SetPolygonOffsets
	(const Handle(AIS_InteractiveObject) &anObj, 
	 const Standard_Integer aMode,  
	 const Standard_Real aFactor, 
	 const Standard_Real aUnits) 
void  AIS_InteractiveContext::PolygonOffsets
	(const Handle(AIS_InteractiveObject) &anObj, 
	 Standard_Integer &aMode,  
	 Standard_Real &aFactor, 
	 Standard_Real &aUnits) 
Standard_Boolean      AIS_InteractiveContext::HasPolygonOffsets
	(const Handle(AIS_InteractiveObject) &anObj) 
~~~~~

@section occt_visu_3_3 Interactive Context 

@subsection occt_visu_3_3_1 Rules 

The Interactive Context allows managing in a transparent  way the graphic and **selectable** behavior of interactive
 objects in one or  more viewers. Most functions which allow modifying the attributes of  interactive objects, and
 which were presented in the preceding chapter, will be  looked at again here.  

There is one essential rule to follow: the modification of  an interactive object, which is already known by the Context,
 must be done  using Context functions. You can only directly call the functions available for  an interactive object if
 it has not been loaded into an Interactive Context.  

~~~~~
Handle (AIS_Shape) TheAISShape = new AIS_Shape (ashape);  
	myIntContext->Display(TheAISShape);  
	myIntContext->SetDisplayMode(TheAISShape ,1);  
	myIntContext->SetColor(TheAISShape,Quantity_NOC_RED);  
~~~~~

You can also write  

~~~~~
Handle (AIS_Shape) TheAISShape = new AIS_Shape (ashape);  
	TheAISShape->SetColor(Quantity_NOC_RED);  
	TheAISShape->SetDisplayMode(1);  
	myIntContext->Display(TheAISShape);  
~~~~~

@subsection occt_visu_3_3_2 Groups of functions 

**Neutral Point** and **Local Context** constitute the two operating modes or states of the **Interactive Context**, 
which is the central entity which pilots visualizations and selections.

The **Neutral Point**, which is the default mode, allows easily visualizing and selecting interactive objects, which
 have been loaded into the context. Opening **Local contexts** allows preparing and using a temporary selection 
environment without disturbing the neutral point.

A set of functions allows choosing the interactive objects which you want to act on, the selection modes which you want
 to activate, and the temporary visualizations which you will execute. When the operation is finished, you close the 
current local context and return to the state in which you were before opening it (neutral point or previous local 
context).

The Interactive Context is composed of many  functions, which can be conveniently grouped according to the theme:  
  * management proper to the context; 
  * management in the local context; 
  * presentations and selection in open/closed context; 
  * selection strictly speaking. 

Some functions can only be used in open Local Context; others in closed local context; others do not have the same
 behavior in one state as in the other.

@subsection occt_visu_3_3_3 Management of the Interactive Context 

The **Interactive Context** is made up of a **Principal Viewer** and, optionally, a trash bin or **Collector Viewer**.

An interactive object can have a certain number of specific graphic attributes, such as visualization mode, color, and 
material. Correspondingly, the interactive context has a set of graphic attributes, the *Drawer*, which is valid by 
default for the objects it controls.

When an interactive object is visualized, the required graphic attributes are first taken from the object's own
 <i>Drawer</i> if one exists, or from the context drawer for the others.

The following adjustable settings allow personalizing the behavior of presentations  and selections:
  * Default Drawer, containing all the color and line attributes  which can be used by interactive objects, which do not
 have their own attributes.
  * Default Visualization Mode for interactive objects.  By default: *mode  0* ;  
  * Highlight color of entities detected by mouse movement. By default: *Quantity_NOC_CYAN1*;  
  * Pre-selection color. By default:  *Quantity_NOC_GREEN*;  
  * Selection color (when you click on a detected object). By default:  *Quantity_NOC_GRAY80*;  
  * Sub-Intensity color. By default: *Quantity_NOC_GRAY40*.

All of these settings can be modified by functions proper to  the Context.  

When you change a graphic attribute pertaining to the  Context (visualization mode, for example), all interactive 
objects, which do  not have the corresponding appropriate attribute, are updated.  

Let us examine the case of two interactive objects: *obj1* and *obj2*:  

~~~~~
TheCtx->Display(obj1,Standard_False); // False = no viewer update 
TheCtx->Display(obj2,Standard_True); // True = viewer update  
TheCtx->SetDisplayMode(obj1,3,Standard_False); 
TheCtx->SetDisplayMode(2); 
// obj2 is visualised in mode 2 (if it accepts this mode) 
// obj1 stays visualised in its mode 3.  
~~~~~

*PresentationManager3D*  and *Selector3D*, which manage the presentation and selection of present  interactive objects, 
 are associated to the main Viewer. The same is true of the optional Collector. 

@section occt_visu_3_4 Local Context 

@subsection occt_visu_3_4_1 Rules and Conventions 

  * Opening a local context allows preparing an environment for  temporary presentations and selections, which will
 disappear once the local  context is closed. 
  * It is possible to open several local contexts, but only the last  one will be active. 
  * When you close a local context, the previous one, which is still on  the stack, is activated again. If none is left,
 you return to Neutral Point. 
  * Each local context has an index created when the context opens. You  should close the local context, which you have
 opened. 

The interactive object, which is used the most by  applications, is *AIS_Shape*. Consequently, standard functions are 
available which allow you to easily prepare selection operations on the  constituent elements of shapes (selection of 
vertices, edges, faces etc) in an  open local context. The selection modes specific to "Shape" type objects are  called 
**Standard Activation Mode**. These modes are only taken into account  in open local context and only act on interactive 
objects which have redefined  the virtual function *AcceptShapeDecomposition()* so that it returns *TRUE*.   
  * Objects, which are temporarily in a local context, are not  recognized by other local contexts a priori. Only objects
 visualized in Neutral  Point are recognized by all local contexts. 
  * The state of a temporary interactive object in a local context  can only be modified while another local context is
 open. 

@note The specific modes of selection only concern the interactive  objects, which are present in the Main Viewer. In 
the Collector, you can only  locate interactive objects, which answer positively to the positioned filters  when a local
 context is open, however, they are never decomposed in  standard mode.  

@subsection occt_visu_3_4_2 Management of Local Context

The local context can be opened using method *AIS_InteractiveContext::OpenLocalContext*. The following options are
 available:
  * *UseDisplayedObjects*: allows loading the  interactive objects visualized at Neutral Point in the opened local 
context. If *FALSE*, the local context is empty after being opened. If *TRUE*,  the objects at Neutral Point are
 modified by their default selection mode. 
  * *AllowShapeDecomposition*: *AIS_Shape* allows or prevents  decomposition in standard shape location mode of objects
 at Neutral Point,  which are type-privileged (see @ref occt_visu_3_2_4 "Selection" chapter). This Flag is only taken 
 into account when *UseDisplayedObjects* is *TRUE*. 
  * *AcceptEraseOfObjects*: authorizes other local contexts to erase  the interactive objects present in this context.
 This option is rarely used. The last option has no current use.

This function returns the index of the created local context.  It should be kept and used when the context is closed.  

To load objects visualized at Neutral Point into a local  context or remove them from it use methods
~~~~~ 
  AIS_InteractiveContext::UseDisplayedObjects  
  AIS_InteractiveContext::NotUseDisplayedObjects  
~~~~~
Closing Local Contexts is done by:  
~~~~~
  AIS_InteractiveContext::CloseLocalContext  
  AIS_InteractiveContext::CloseAllContexts  
~~~~~

@note When the index is not specified in the first function, the  current Context is closed. This option can be 
dangerous, as other Interactive  Functions can open local contexts without necessarily warning the user. For  greater
 security, you have to close the context with the index given on  opening. 

To get the index of the current context, use function *AIS_InteractiveContext::IndexOfCurrentLocal*. It allows closing 
all open local  contexts at one go. In this case, you find yourself directly at Neutral Point.  

When you close a local context, all temporary interactive  objects are deleted, all selection modes concerning the
 context are canceled, and all content filters are emptied.


@subsection occt_visu_3_4_3 Presentation in a Neutral Point 

You must distinguish between the **Neutral Point** and the **Open Local Context** states. Although the majority of
 visualization  functions can be used in both situations, their behavior is different.

Neutral Point should be used to visualize the interactive  objects, which represent and select an applicative entity.
 Visualization and  Erasing orders are straightforward: 

~~~~~
AIS_InteractiveContext::Display  
	(const  Handle(AIS_InteractiveObject)& anIobj, 
	 const  Standard_Boolean updateviewer=Standard_True);  

AIS_InteractiveContext::Display  
	(const  Handle(AIS_InteractiveObject)& anIobj,  
	 const  Standard_Integer amode, 
	 const  Standard_Integer aSelectionMode, 
	 const  Standard_Boolean updateviewer = Standard_True,  
	 const  Standard_Boolean allowdecomposition =  Standard_True);  

  AIS_InteractiveContext::Erase  
  AIS_InteractiveContext::EraseMode  
  AIS_InteractiveContext::ClearPrs  
  AIS_InteractiveContext::Redisplay  
  AIS_InteractiveContext::Remove  
  AIS_InteractiveContext::EraseAll  
  AIS_InteractiveContext::Hilight  
  AIS_InteractiveContext::HilightWithColor  
~~~~~
  
Bear in mind the following points:  
  * It is recommended to display and erase interactive objects when  no local context is opened, and open a local 
context for local selection only.
  * The first *Display* function among the two ones available in  *InteractiveContext* visualizes the object in its 
default mode (set with  help of SetDisplayMode() method of InteractiveObject prior to Display() call),  or in the 
default context mode, if applicable. If it has neither, the function  displays it in 0 presentation mode. The object's
 default selection mode is  automatically activated (0 mode by convention). 
  * Activating the displayed object by default can be turned off with  help of *SetAutoActivateSelection()* method. 
This might be efficient if  you are not interested in selection immediately after displaying an object.
  * The second *Display* function should only be used in Neutral  Point to visualize a supplementary mode for the 
object, which you can erase by  *EraseMode (...)*. You activate the selection mode. This is passed as an argument.  
By convention, if you do not want to activate a selection mode, you must set  the *SelectionMode* argument to -1. This 
function is  especially interesting in open local context, as we will see below. 
  * In Neutral Point, it is not advisable to activate other selection  modes than the default selection one. It is 
preferable to open a local context  in order to activate particular selection modes. 
  * When you call *Erase(Interactive object)* function, the *PutIncollector*  argument, which is *FALSE* by default, 
allows you to visualize the object  directly in the Collector and makes it selectable (by activation of 0 mode).  You 
can nonetheless block its passage through the Collector by changing the  value of this option. In this case, the object 
is present in the Interactive  Context, but is not seen anywhere. 
  * *Erase()* with *putInCollector = Standard_True*  might be slow as it recomputes the object presentation in the 
Collector. Set *putInCollector* to *Standard_False* if you simply want to hide the object's  presentation temporarily.
  * Visualization attributes and graphic behavior can be modified through a set of functions similar to those for the
 interactive object (color, thickness of line, material, transparency,  locations, etc.) The context then manages 
immediate and deferred updates. 
  * Call *Remove()* method of *InteractiveContext* as soon  as the interactive object is no longer needed and you want
 to destroy it..  Otherwise, references to *InteractiveObject* are kept by *InteractiveContext*,  and the *Object* is
 not destroyed, which results in memory leaks. In  general, if the presentation of an  interactive object can be
 computed quickly, it  is recommended to *Remove()* it instead of using *Erase()* method.

@subsection occt_visu_3_4_4 Presentation in the Local Context

In open local context, the *Display* functions presented above can be as well.  

@note The function *AIS_InteractiveObject::Display* automatically  activates the object's default selection mode.
 When you only want to visualize  an Interactive Object in open Context, you must call the function
 *AIS_InteractiveContext::Display*.  

You can activate or deactivate specific selection modes in the local open context in several different ways:  
Use the Display functions with the appropriate modes.
   
~~~~~
  AIS_InteractiveContext::ActivateStandardMode
  //can be used only if a  Local Context is opened.   
  AIS_InteractiveContext::DeactivateStandardMode  
  AIS_InteractiveContext::ActivatedStandardModes  
  AIS_InteractiveContext::SetShapeDecomposition  
~~~~~

This activates the corresponding  selection mode for all objects in Local Context, which accept decomposition into 
sub-shapes. Every new Object which has been loaded into the interactive  context and which meets the decomposition 
criteria is automatically  activated according to these modes.  

@note If you have opened a local context by loading an object with  the default options
 <i>(AllowShapeDecomposition = Standard_True)</i>, all objects of  the "Shape" type are also activated with the same
 modes. You can change the  state of these "Standard" objects by using *SetShapeDecomposition(Status)*.  

Load an interactive object by the function *AIS_InteractiveContext::Load*.  

This function allows loading an Interactive Object  whether it is visualized or not with a given selection mode, and/or
 with the necessary decomposition option. If *AllowDecomp=TRUE* and obviously, if the  interactive object is of the 
"Shape" type, these "standard" selection modes  will be automatically activated as a function of the modes present in
 the Local  Context.  

Use *AIS_InteractiveContext::Activate* and *AIS_InteractiveContext::Deactivate* to directly activate/deactivate 
selection modes on an object.

@subsection occt_visu_3_4_5 Filters 

To define an environment of dynamic detection, you can use standard filter classes or create your own.
A filter questions the owner of the sensitive primitive in local context to determine if it has the desired qualities.
 If it answers positively, it is kept. If not, it is rejected.

The root class of objects is *SelectMgr_Filter*.  The principle behind it is straightforward: a filter tests to see 
whether the  owners <i>(SelectMgr_EntityOwner)</i> detected in mouse position by the Local  context selector answer 
*OK*. If so, it is kept, otherwise it is rejected.  

You can create a custom class of filter objects by  implementing the deferred function *IsOk()*:  

~~~~~
class MyFilter : public SelectMgr_Filter { };  
virtual Standard_Boolean MyFilter::IsOk  
	(const Handle(SelectMgr_EntityOwner)&  anObj) const = 0; 
~~~~~

In *SelectMgr*, there are also Composition filters (AND  Filters, OR Filters), which allow combining several filters.
 In  InteractiveContext , all filters that you add are stocked in an OR filter  (which answers *OK* if at least one 
filter answers *OK*).  

There are Standard filters, which have already been  implemented in several packages:  
  * *StdSelect_EdgeFilter* -- for edges, such as lines and circles;  
  * *StdSelect_FaceFilter* -- for faces, such as planes, cylinders and spheres;  
  * *StdSelect_ShapeTypeFilter* -- for shape types, such as compounds, solids, shells and wires;
  * *AIS_TypeFilter* -- for types  of interactive objects;  
  * *AIS_SignatureFilter* -- for types  and signatures of interactive objects; 
  * *AIS_AttributeFilter* -- for attributes of Interactive Objects, such as color and width.  

As there are specific behaviors on shapes, each new *Filter* class must, if necessary, redefine 
*AIS_LocalContext::ActsOn* function, which informs the Local Context if it acts on specific types of sub-shapes. 
By default, this  function answers *FALSE*.

@note Only type filters are activated in Neutral Point to  make it possible to identify a specific type of visualized 
object. For filters to come into play, one or more object selection modes must be activated.  

There are several functions to manipulate filters:  
* *AIS_InteractiveContext::AddFilter* adds a filter passed as an argument.  
* *AIS_InteractiveContext::RemoveFilter* removes a filter passed as an argument.  
* *AIS_InteractiveContext::RemoveFilters* removes all present filters.  
* *AIS_InteractiveContext::Filters* gets the list of filters active in a local context.  
  
###Example

~~~~~
myContext->OpenLocalContext(Standard_False);  
// no object in neutral point is  loaded  

myContext->ActivateStandardMode(TopAbs_Face);  
//activates decomposition of  shapes into faces.  
Handle (AIS_Shape) myAIShape = new AIS_Shape (  ATopoShape);  

myContext->Display(myAIShape,1,-1,Standard_True,Standard_True);  

//shading visualization mode,  no specific mode, authorization for decomposition into sub-shapes. At this  Stage,
 myAIShape is decomposed into faces...  

Handle(StdSelect_FaceFilter) Fil1= new  
	StdSelect_FaceFilter(StdSelect_Revol);   
Handle(StdSelect_FaceFilter) Fil2= new 
      StdSelect_FaceFilter(StdSelect_Plane);  

myContext->AddFilter(Fil1);   
myContext->AddFilter(Fil2);  

//only faces of revolution or  planar faces will be selected 
      
myContext->MoveTo( xpix,ypix,Vue);  
// detects the mouse position  
~~~~~

@subsection occt_visu_3_4_6 Selection in the Local Context
 
Dynamic detection and selection are put into effect in a  straightforward way. There are only a few conventions and 
functions to be  familiar with. The functions are the same in neutral point and in open local context:   
  * *AIS_InteractiveContext::MoveTo* -- passes  mouse position to Interactive Context selectors  
  * *AIS_InteractiveContext::Select* -- stocks  what has been detected on the last *MoveTo*. Replaces the previously 
selected  object. Empties the stack if nothing has been detected at the last move  
  * *AIS_InteractiveContext::ShiftSelect* -- if  the object detected at the last move was not already selected, it is
 added to the list of the selected objects. If not, it is withdrawn. Nothing happens if you  click on an empty area.  
  * *AIS_InteractiveContext::Select* -- selects  everything found in the surrounding area.  
  * *AIS_InteractiveContext::ShiftSelect* -- selects  what was not previously in the list of selected, deselects those 
already present.  

Highlighting of detected and selected entities is  automatically managed by the Interactive Context, whether you are in 
neutral  point or Local Context. The Highlight colors are those dealt with above. You  can nonetheless disconnect this 
automatic mode if you want to manage this part yourself :  
~~~~~
  AIS_InteractiveContext::SetAutomaticHilight  
  AIS_InteractiveContext::AutomaticHilight  
~~~~~

If there is no open local context, the objects selected are called **current objects**. If there is a  local context,
 they are called **selected objects**. Iterators allow  entities to be recovered in either case. A set of functions 
allows manipulating the objects, which have been placed in these different lists.  

@note When a Local Context is open, you can select entities other  than interactive objects (vertices, edges etc.) 
from decompositions in standard  modes, or from activation in specific modes on specific interactive objects. Only 
interactive objects are stocked in the list of selected objects. 

You can  question the Interactive context by moving the mouse. The following functions can be used:  
  * *AIS_InteractiveContext::HasDetected* informs if something has been detected; 
  * *AIS_InteractiveContext::HasDetectedShape* informs if it is a shape; 
  * *AIS_InteractiveContext::DetectedShape* gets the shape if the detected entity is an object;
  * *AIS_InteractiveContext::DetectedInteractive* gets the interactive object if the detected entity is an object. 

After using the *Select* and *ShiftSelect* functions in Neutral Point, you can explore the list of selections, referred
 to as current objects  in this context. The following functions can be used:    
  * *AIS_InteractiveContext::InitCurrent* initiates a scan of this list; 
  * *AIS_InteractiveContext::MoreCurrent* extends the scan; 
  * *AIS_InteractiveContext::NextCurrent* resumes the scan; 
  * *AIS_InteractiveContext::Current* gets the name of the current object  detected in the scan; 
  * *AIS_InteractiveContext::FirstCurrentObject* gets the first current interactive object; 
  * *AIS_InteractiveContext::HilightCurrents* highlights current objects; 
  * *AIS_InteractiveContext::UnhilightCurrents* removes highlight from current objects; 
  * *AIS_InteractiveContext::ClearCurrents* empties the list of current objects in order to update it; 
  * *AIS_InteractiveContext::IsCurrent* finds the current object. 

In the Local Context, you can explore the list of selected  objects available. The following functions can be used:    
  * *AIS_InteractiveContext::InitSelected* initiates the list of objects; 
  * *AIS_InteractiveContext::MoreSelected* extends the list of objects; 
  * *AIS_InteractiveContext::NextSelected* resumes a scan;
  * *AIS_InteractiveContext::SelectedShape* gets the name of the selected object; 
  * *AIS_InteractiveContext::HasSelectedShape* checks if the selected shape is obtained; 
  * *AIS_InteractiveContext::Interactive* gets the picked interactive object; 
  * *AIS_InteractiveContext::HasApplicative* checks if the applicative object has an owner from  Interactive attributed
 to it; 
  * *AIS_InteractiveContext::Applicative* gets the owner of the detected applicative entity; 
  * *AIS_InteractiveContext::IsSelected* gets the name of the selected object. 


###Example

~~~~~
myAISCtx->InitSelected();  
while (myAISCtx->MoreSelected())  
	{ 
	if  (myAISCtx->HasSelectedShape) 
		{ 
		TopoDS_Shape  ashape = myAISCtx->SelectedShape(); 
			// to be able  to use the picked shape 
			            }  
		else  
		{ 
		Handle_AIS_InteractiveObject  anyobj = myAISCtx->Interactive(); 
		// to be able to use the picked interactive object  
		}  
myAISCtx->NextSelected();  
}     
~~~~~

You have to ask whether you have selected a shape  or an interactive object before you can recover the entity in the
 Local Context or in the iteration loop. If you have  selected a Shape from *TopoDS* on decomposition in standard mode,
 the *Interactive()* function returns the interactive object, which provided the selected  shape. Other functions allow 
you to manipulate the content of Selected or  Current Objects:  
  * *AIS_InteractiveContext::EraseSelected* erases the selected objects; 
  * *AIS_InteractiveContext::DisplaySelected* displays them; 
  * *AIS_InteractiveContext::SetSelected* puts the objects in the list of  selections;
  * *AIS_InteractiveContext::SetSelectedCurrent* takes the list of selected objects from a local context and puts it 
into the list of current objects in Neutral Point;
  * *AIS_InteractiveContext::AddOrRemoveSelected* adds or removes an object from the list of selected entities; 
  * *AIS_InteractiveContext::HilightSelected* highlights the selected object;
  * *AIS_InteractiveContext::UnhilightSelected* removes highlighting from the selected object; 
  * *AIS_InteractiveContext::ClearSelected* empties the list of selected objects. 


You can highlight and remove highlighting from a current  object, and empty the list of current objects using the
 following functions:  
~~~~~
  AIS_InteractiveContext::HilightCurrents  
  AIS_InteractiveContext::UnhilightCurrents  
  AIS_InteractiveContext::ClearCurrents  
~~~~~
When you are in an open Local Context, you may need to keep  "temporary" interactive objects. This is possible using the 
following functions: 
  * *AIS_InteractiveContext::KeepTemporary* transfers the characteristics of the  interactive object seen in its local 
context (visualization mode, etc.) to the  neutral point. When the local context is closed, the object does not disappear.  
  * *AIS_InteractiveContext::SetSelectedCurrent* allows the selected object to become the current object when you close
 the local context.  

You can also want to use function *AIS_InteractiveContext::ClearLocalContext* to modify in a general way the state of
 the local context before continuing a selection (emptying objects, removing  filters, standard activation modes). 

@subsection occt_visu_3_4_7 Recommendations 

The possibilities of use for local contexts are numerous  depending on the type of operation that you want to perform:  
  * working on all visualized interactive objects, 
  * working on only a few objects, 
  * working on a single object. 

When you want to work on one type of entity, you should  open a local context with the option *UseDisplayedObjects* set
 to FALSE. Some  functions which allow you to recover the visualized interactive objects, which  have a given Type, and 
Signature from the "Neutral Point" are:  

~~~~~
AIS_InteractiveContext::DisplayedObjects (AIS_ListOfInteractive&  aListOfIO) const;  
AIS_InteractiveContext::DisplayedObjects (const AIS_KindOfInteractive  WhichKind, const Standard_Integer WhichSignature;
AIS_ListOfInteractive&  aListOfIO) const;  
~~~~~

At this stage, you only have to load the functions *Load, Activate,* and so on.  

When you open a Local Context with default options, you  must keep the following points in mind:  
* The Interactive  Objects visualized at Neutral Point are activated with their default selection  mode. You must 
deactivate those, which you do not want to use.  
* The Shape Type  Interactive Objects are automatically decomposed into sub-shapes when standard  activation modes are
 launched.  
* The "temporary"  Interactive Objects present in the Local Contexts are not automatically taken  into account. You have
 to load them manually if you want to use them.  

The stages could be the following:
  1. Open  a Local Context with the right options;  
  2. Load/Visualize  the required complementary objects with the desired activation modes.  
  3. Activate  Standard modes if necessary  
  4. Create  its filters and add them to the Local Context  
  5. Detect/Select/recover  the desired entities  
  6. Close  the Local Context with the adequate index.  

It is useful to create an **interactive editor**, to which you  pass the Interactive Context. This allow setting up different
 contexts of selection/presentation according to the operation, which you want  to perform. 
 
Let us assume that you have visualized several types of interactive objects: *AIS_Points*,  *AIS_Axes*,
 *AIS_Trihedrons*, and *AIS_Shapes*.  

For your applicative function, you need an axis to create a  revolved object. You could obtain this axis by identifying:  
  * an axis which is already visualized,
  * 2 points,
  * a rectilinear edge on the shapes which are present,
  * a cylindrical face on the shapes (You will take the axis of this  face) 

~~~~~
myIHMEditor::myIHMEditor  
	(const  Handle(AIS_InteractiveContext)& Ctx, 
	 ....) : 
	 myCtx(Ctx),  
	...  
{ 
} 

myIHMEditor::PrepareContext() 
{ 
myIndex =myCtx->OpenLocalContext();  

//the filters  

Handle(AIS_SignatureFilter) F1 = new  AIS_SignatureFilter(AIS_KOI_Datum,AIS_SD_Point);   
//filter on the points  

Handle(AIS_SignatureFilter) F2 = new AIS_SignatureFilter(AIS_KOI_Datum,AIS_SD_Axis);   
//filters on the axes.  

Handle(StdSelect_FaceFilter) F3 = new StdSelect_FaceFilter(AIS_Cylinder);   
//cylindrical face filters  
//... 
// activation of standard modes  on the shapes..  
myCtx->ActivateStandardMode(TopAbs_FACE); 
myCtx->ActivateStandardMode(TopAbs_VERTEX); 
myCTX->Add(F1); 
myCTX->Add(F2); 
myCTX->Add(F3);  

// at  this point, you can call the selection/detection function  
} 

void myIHMEditor::MoveTo(xpix,ypix,Vue) 

{  myCTX->MoveTo(xpix,ypix,vue);  
// the highlight of what is  detected is automatic.  
}      
Standard_Boolean myIHMEditor::Select()  
{
// returns true if you should  continue the selection      
		myCTX->Select();   
		myCTX->InitSelected();  
		if(myCTX->MoreSelected()) 
		 {
		 if(myCTX->HasSelectedShape())     
		{ const  TopoDS_Shape& sh = myCTX->SelectedShape(); 
		if(  vertex){  
			if(myFirstV...)   
			{  
			//if it is the  first vertex, you stock it, then you deactivate the faces and only keep the  filter on the
 points: 
			mypoint1 =  ....; 
			myCtx->RemoveFilters(); 
			myCTX->DeactivateStandardMode(TopAbs_FACE);   
			myCtx->Add(F1);   
			// the filter  on the AIS_Points  
			myFirstV = Standard_False;  
			return Standard_True;  
			 }
			else
			 { 
			mypoint2 =...;  
			//  construction of the axis return Standard_False;  
			} 
		 } 
		 else  
		  {  
		//it is a  cylindrical face : you recover the axis; visualize it; and stock it.  
		return  Standard_False;  
		}  
		  }  
		// it is not  a shape but is no doubt a point.  
		else  
		{  
		Handle(AIS_InteractiveObject)   
		SelObj =  myCTX->SelectedInteractive(); 
		if(SelObj->Type()==AIS_KOI_Datum)   
		{ 
			if(SelObj->Signature()==1)   
			{ 
				if  (firstPoint) 
				{ 
				mypoint1  =...  
				return  Standard_True;  
				} 
				else 
				{  
				mypoint2 =  ...;  
				//construction  of the axis, visualization, stocking  
				return  Standard_False;  
		} 
		} 

		else  
		{  
		// you have  selected an axis; stock the axis  
		return  Standard_False; 
		} 
		} 
		} 
		} 
		}  
void myIHMEditor::Terminate()
{
myCtx->CloseLocalContext(myIndex); 
...
}
~~~~~

@section occt_visu_3_5 Standard Interactive Object Classes 

Interactive Objects are selectable and viewable objects connecting graphic representation and the underlying reference 
geometry.

They are divided into four types:
  * the **Datum** -- a construction geometric element;
  * the **Relation** -- a constraint on the interactive shape and the corresponding reference geometry;
  * the **Object** -- a topological shape or connection between shapes;
  * **None** -- a token, that instead of eliminating the object, tells the application to look further until it finds an
 acceptable object definition in its generation.

Inside these categories, there is a possibility of additional characterization by means of a signature. The signature 
provides an index to the further characterization. By default, the **Interactive Object** has a *None* type and a 
signature of 0 (equivalent to *None*).
If you want to give a particular type and signature to your interactive object, you must redefine the two virtual
 methods: <i>Type</i> and <i>Signature</i>.

@subsection occt_visu_3_5_1 Datum

The **Datum** groups together the construction elements such as lines, circles, points, trihedrons, plane trihedrons,
 planes and axes.
  
*AIS_Point,  AIS_Axis,  AIS_Line,  AIS_Circle,  AIS_Plane* and *AIS_Trihedron* have four selection modes: 
  * mode 0 : selection of a trihedron; 
  * mode 1 : selection of the origin of the trihedron; 
  * mode 2 : selection of the axes; 
  * mode 3 : selection of the planes XOY, YOZ, XOZ. 

when you activate one of modes: 1 2 3 4, you pick AIS  objects of type:  
  * *AIS_Point* 
  * *AIS_Axis* (and information on the type of axis) 
  * *AIS_Plane* (and information on the type of plane). 

*AIS_PlaneTrihedron* offers three selection modes:  
  * mode 0 : selection of the whole trihedron; 
  * mode 1 : selection of the origin of the trihedron; 
  * mode 2 : selection of the axes -- same remarks as for the Trihedron.

For the presentation of planes and trihedra, the default  unit of length is millimeter, and the default value for the 
representation of  axes is 100. If you modify these dimensions, you must temporarily recover the  object **Drawer**. 
From it, take the *Aspects* in which the values for length  are stored (*PlaneAspect* for the plane, *FirstAxisAspect* 
for trihedra), and  change these values inside these Aspects. Finally, recalculate the  presentation.  

@subsection occt_visu_3_5_2 Object

The **Object** type includes topological shapes, and connections between shapes.

*AIS_Shape* has three visualization modes :
  * mode 0 : Line (default mode) 
  * mode 1 : Shading (depending on the type of shape) 
  * mode 2 : Bounding Box 

And at maximum seven selection modes, depending on the shape complexity:
  * mode 0 : selection of the *AIS_Shape*; 
  * mode 1 : selection of the vertices; 
  * mode 2 : selection of the edges; 
  * mode 3 : selection of the wires; 
  * mode 4 : selection of the faces; 
  * mode 5 : selection of the shells; 
  * mode 6 : selection of the constituent solids. 

  * *AIS_Triangulation* is a simple interactive object for displaying  triangular mesh contained in *Poly_Triangulation*
 container. 
  * *AIS_ConnectedInteractive* is an Interactive Object connecting to  another interactive object reference, and located
 elsewhere in the viewer makes  it possible not to calculate presentation and selection, but to deduce them  from your
 object reference.  
  * *AIS_ConnectedShape* is an object connected to interactive objects  having a shape; this class has the same 
decompositions as *AIS_Shape*. Furthermore, it allows a presentation of hidden parts, which are calculated 
automatically from the shape of its reference.  
  * *AIS_MultipleConnectedInteractive* is an object connected to a list  of interactive objects (which can also be 
Connected objects. It does not  require memory hungry calculations of presentation) 
  * *AIS_MultipleConnectedShape* is an interactive Object connected to  a list of interactive objects having a Shape
 <i>(AIS_Shape, AIS_ConnectedShape,  AIS_MultipleConnectedShape)</i>. The presentation of hidden parts is calculated 
automatically.  
  * *AIS_TexturedShape* is an Interactive Object that supports texture  mapping. It is constructed as a usual AIS_Shape, 
but has additional methods  that allow to map a texture on it. 
  * *MeshVS_Mesh* is an Interactive Object that represents meshes, it  has a data source that provides geometrical 
information (nodes, elements) and  can be built up from the source data with a custom presentation builder. 
See more details @ref user_guides__visu_mesh "here".


The class *AIS_ColoredShape* allows using custom colors and line widths for *TopoDS_Shape* objects and their sub-shapes.

~~~~~
  AIS_ColoredShape aColoredShape = new AIS_ColoredShape (theShape);

  // setup color of entire shape
  aColoredShape->SetColor (Quantity_Color (Quantity_NOC_RED));

  // setup line width of entire shape
  aColoredShape->SetWidth (1.0);

  // set transparency value
  aColoredShape->SetTransparency (0.5);

  // customize color of specified sub-shape
  aColoredShape->SetCustomColor (theSubShape, Quantity_Color (Quantity_NOC_BLUE1));

  // customize line width of specified sub-shape
  aColoredShape->SetCustomWidth (theSubShape, 0.25);
~~~~~

The presentation class *AIS_PointCloud* can be used for efficient drawing of large arbitrary sets of colored points. 
It uses *Graphic3d_ArrayOfPoints* to pass point data into OpenGl graphic driver to draw a set points as an array of 
"point sprites". The point data is packed into vertex buffer object for performance.
- The type of point marker used to draw points can be specified as a presentation aspect.
- The presentation provides selection by a bounding box of the visualized set of points. It supports two display / 
highlighting modes: points or bounding box.

@image html point_cloud.png "A random colored cloud of points"

~~~~~
Handle(Graphic3d_ArrayOfPoints) aPoints = new Graphic3d_ArrayOfPoints (2000, Standard_True);
aPoints->AddVertex (gp_Pnt(-40.0, -40.0, -40.0), Quantity_Color (Quantity_NOC_BLUE1));
aPoints->AddVertex (gp_Pnt (40.0,  40.0,  40.0), Quantity_Color (Quantity_NOC_BLUE2));

Handle(AIS_PointCloud) aPntCloud = new AIS_PointCloud();
aPntCloud->SetPoints (aPoints);
~~~~~

The draw command *vpointcloud* builds a cloud of points from shape triangulation.
This command can also draw a sphere surface or a volume with a large amount of points (more than one million).

@subsection occt_visu_3_5_3 Relations 

The **Relation** is made up of constraints on one or more interactive shapes and the corresponding reference geometry.
 For example, you might want to constrain two edges in a parallel relation. This constraint is considered as an object 
in its own right, and is shown as a sensitive primitive. This takes the graphic form of a perpendicular arrow marked 
with the || symbol and lying between the two edges.

The following relations are provided by *AIS*:
  * *AIS_ConcentricRelation* 
  * *AIS_FixRelation*
  * *AIS_IdenticRelation* 
  * *AIS_ParallelRelation*
  * *AIS_PerpendicularRelation*
  * *AIS_Relation*
  * *AIS_SymmetricRelation*
  * *AIS_TangentRelation*

The list of relations is not exhaustive.    

@subsection occt_visu_3_5_4 Dimensions
  * *AIS_AngleDimension* 
  * *AIS_Chamf3dDimension* 
  * *AIS_DiameterDimension* 
  * *AIS_DimensionOwner*
  * *AIS_LengthDimension*
  * *AIS_OffsetDimension*
  * *AIS_RadiusDimension*

@section occt_visu_3_6 Dynamic Selection 

The dynamic selection represents the topological shape, which you want to select, by decomposition of <i>sensitive 
primitives</i> -- the sub-parts of the shape that will be detected and highlighted. The sets of these primitives are 
handled by the powerful three-level BVH tree selection algorithm. 

For more details on the algorithm and examples of usage, please, refer to @ref occt_visu_2_2 "Selection" chapter.


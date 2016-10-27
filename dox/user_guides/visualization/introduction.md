Introduction {#user_guides__visu_intro}
=============
@tableofcontents 

Visualization in Open CASCADE Technology is based on the  separation of:
  * on the one hand -- the data which stores the geometry and topology  of the entities you want to display and select, and 
  * on the other hand -- its **presentation** (what you see  when an object is displayed in a scene) and **selection** (possibility to choose the whole object or its sub-parts interactively to apply application-defined operations to the selected entities). 

Presentations are managed through the **Presentation** component, and selection through the **Selection** component.

**Application Interactive Services** (AIS) provides the means to create links between an application GUI viewer and the packages, which are used to manage selection and presentation, which makes management of these functionalities in 3D more  intuitive and consequently, more transparent.

*AIS* uses the notion of the *interactive  object*, a displayable and selectable entity, which represents an element  from the application data. As a result, in 3D, you, the user, have no need to  be familiar with any functions underlying AIS unless you want to create your  own interactive objects or selection filters.  

If, however, you require types of interactive objects and  filters other than those provided, you will need to know the mechanics of  presentable and selectable objects, specifically how to implement their virtual  functions. To do this requires familiarity with such fundamental concepts as the sensitive primitive and the presentable object.  

The the following packages are used to display 3D objects:
  * *AIS*; 
  * *StdPrs*; 
  * *Prs3d*; 
  * *PrsMgr*; 
  * *V3d*; 
  * *Graphic3d*.

The packages used to display 3D objects are also applicable for visualization of 2D objects.

The figure below presents a schematic overview of the relations between the key concepts and packages in visualization. Naturally, "Geometry & Topology" is just an example of application data that can be handled by *AIS*, and application-specific interactive objects can deal with any  kind of data. 

@image html visualization_image003.png "Key concepts and packages in visualization"
@image latex visualization_image003.png "Key concepts and packages in visualization"

For advanced information on visualization algorithms, see our <a href="http://www.opencascade.com/content/tutorial-learning">E-learning & Training</a> offerings.  


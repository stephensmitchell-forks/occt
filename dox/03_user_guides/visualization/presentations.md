3D Presentations {#user_guides__visu_presentation}
================

@tableofcontents 

@section occt_visu_4_1 Glossary of 3D terms 

* **Anti-aliasing**	This mode attempts to improve the screen resolution by drawing lines and curves in a mixture of colors so that to the human eye the line or curve is smooth. The quality of the result is linked to the quality of the algorithm used by the workstation hardware.
* **Depth-cueing**	Reduces the color intensity for the portion of an object further away from the eye to give the impression of depth. This is used for wireframe objects. Shaded objects do not require this.
* **Group**	-- a set of primitives and attributes on those primitives. Primitives and attributes may be added to a group but cannot be removed from it, unless erased globally. A group can have a pick identity.
* **Light** There are five kinds of light source -- ambient, headlight, directional, positional and spot. The light is only activated in a shading context in a view.
* **Primitive**  -- a drawable element. It has a definition in 3D space. Primitives can either be lines, faces, text, or markers. Once displayed markers and text remain the same size. Lines and faces can be modified e.g. zoomed. Primitives must be stored in a group.
* **Structure** -- manages a set of groups. The groups are mutually exclusive. A structure can be edited, adding or removing groups. A structure can reference other structures to form a hierarchy. It has a default (identity) transformation and other transformations may be applied to it (rotation, translation, scale, etc). It has no default attributes for the primitive lines, faces, markers, and text. Attributes may be set in a structure but they are overridden by the attributes in each group. Each structure has a display priority associated with it, which rules the order in which it is redrawn in a 3D viewer. If the visualization mode is incompatible with the view it is not displayed in that view, e.g. a shading-only object is not visualized in a wireframe view. 
* **View**	-- is defined by a view orientation, a view mapping, and a context view.
* **Viewer** -- manages a set of views.
* **View orientation** -- defines the manner in which the observer looks at the scene in terms of View Reference Coordinates.
* **View mapping** -- defines the transformation from View Reference Coordinates to the Normalized Projection Coordinates. This follows the Phigs scheme.
* **Z-Buffering** -- a form of hidden surface removal in shading mode only. This is always active for a view in the shading mode. It cannot be suppressed.

@section occt_visu_4_2 Graphic primitives

The *Graphic3d* package is used to create 3D graphic objects in a 3D viewer. These objects called **structures** are made up of groups of primitives and attributes, such as polylines, planar polygons with or without holes, text and markers, and attributes, such as color, transparency, reflection, line type, line width, and text font. A group is the smallest editable element of a structure. A transformation can be applied to a structure. Structures can be connected to form a tree of structures, composed by transformations. Structures are globally manipulated by the viewer.

Graphic structures can be:  
  * Displayed, 
  * Highlighted, 
  * Erased, 
  * Transformed, 
  * Connected to form a tree hierarchy of structures, created by transformations.
  
There are classes for: 
  * Visual attributes for lines, faces, markers, text, materials, 
  * Vectors and vertices, 
  * Graphic objects, groups, and structures. 

@subsection occt_visu_4_2_2 Structure hierarchies

The root is the top of a structure hierarchy or structure network. The attributes of a parent structure are passed to its descendants. The attributes of the descendant structures do not affect the parent. Recursive structure networks are not supported.

@subsection occt_visu_4_2_3 Graphic primitives
* **Markers** 
  * Have one or more vertices, 
  * Have a type, a scale factor, and a color, 
  * Have a size, shape, and orientation independent of  transformations. 
* **Polygons** 
  * Have one closed boundary, 
  * Have at least three vertices, 
  * Are planar and have a normal, 
  * Have interior attributes -- style, color, front and back material,  texture and reflection ratio, 
  * Have a boundary with the following attributes -- type, width scale  factor, color. The boundary is only drawn when the interior style is hollow. 

* **Polygons with holes** 
  * Have multiple closed boundaries, each one with at least three  vertices, 
  * Are planar and have a normal, 
  * Have interior attributes -- style, color, front and back material,  
  * Have a boundary with the following attributes -- type, width scale  factor, color. The boundary is only drawn when the interior style is hollow. 

* **Polylines** 
  * Have two or more vertices, 
  * Have the following attributes -- type, width scale factor, color. 

* **Text** 
  * Has geometric and non-geometric attributes, 
  * Geometric attributes -- character height, character up vector,  text path, horizontal and vertical alignment, orientation, three-dimensional  position, zoomable flag
  * Non-geometric attributes -- text font, character spacing,  character expansion factor, color. 

@subsection occt_visu_4_2_4 Primitive  arrays

Primitive arrays are a more efficient approach to describe  and display the primitives from the aspects of memory usage and graphical  performance. The key feature of the primitive arrays is that the primitive data  is not duplicated. For example, two polygons could share the same vertices, so  it is more efficient to keep the vertices in a single array and specify the  polygon vertices with indices of this array. In addition to such kind of memory  savings, the OpenGl graphics driver provides the Vertex Buffer Objects (VBO).  VBO is a sort of video memory storage that can be allocated to hold the  primitive arrays, thus making the display operations more efficient and  releasing the RAM memory. 

The Vertex Buffer Objects are enabled by default, but VBOs  availability depends on the implementation of OpenGl. If the VBOs are  unavailable or there is not enough video memory to store the primitive arrays,  the RAM memory will be used to store the arrays.  

The Vertex Buffer Objects can be disabled at the application  level. You can use the method *Graphic3d_GraphicDriver::EnableVBO (const Standard_Boolean status)* to enable/disable VBOs: 
  
The following example shows how to disable the VBO support:  

~~~~~
// get the graphic  driver
Handle (Graphic3d_GraphicDriver) aDriver =  
  myAISContext->CurrentViewer()->Driver(); 

// disable VBO support
aDriver->EnableVBO (Standard_False); 
~~~~~

**Note** that the use of Vertex Buffer Objects  requires the application level primitive data provided by the  *Graphic3d_ArrayOfPrimitives* to be transferred to the video memory. *TKOpenGl* transfers the data and releases the *Graphic3d_ArrayOfPrimitives* internal  pointers to the primitive data. Thus it might be necessary to pay attention to  such kind of behaviour, as the pointers could be modified (nullified) by the *TKOpenGl*. 

The different types of primitives could be presented with  the following primitive arrays: 
  * *Graphic3d_ArrayOfPoints,*
  * *Graphic3d_ArrayOfPolygons,*
  * *Graphic3d_ArrayOfPolylines,*
  * *Graphic3d_ArrayOfQuadrangles,*
  * *Graphic3d_ArrayOfQuadrangleStrips,*
  * *Graphic3d_ArrayOfSegments,*
  * *Graphic3d_ArrayOfTriangleFans,*
  * *Graphic3d_ArrayOfTriangles,*
  * *Graphic3d_ArrayOfTriangleStrips.*

The *Graphic3d_ArrayOfPrimitives* is a base class for these  primitive arrays. 

Method *Graphic3d_ArrayOfPrimitives::AddVertex* allows adding There is a set of similar methods to add vertices to the  primitive array.

These methods take vertex coordinates as an argument and  allow you to define the color, the normal and the texture coordinates assigned  to the vertex. The return value is the actual number of vertices in the array. 

You can also modify the values assigned to the vertex or  query these values by the vertex index: 
  * *void Graphic3d_ArrayOfPrimitives::SetVertice*
  * *void  Graphic3d_ArrayOfPrimitives::SetVertexColor*
  * *void  Graphic3d_ArrayOfPrimitives::SetVertexNormal*
  * *void  Graphic3d_ArrayOfPrimitives::SetVertexTexel*
  * *gp_Pnt  Graphic3d_ArrayOfPrimitives::Vertices*
  * *gp_Dir   Graphic3d_ArrayOfPrimitives::VertexNormal*
  * *gp_Pnt3d  Graphic3d_ArrayOfPrimitives::VertexTexel*
  * *Quantity_Color  Graphic3d_ArrayOfPrimitives::VertexColor*
  * *void  Graphic3d_ArrayOfPrimitives::Vertices*
  * *void  Graphic3d_ArrayOfPrimitives::VertexNormal*
  * *void  Graphic3d_ArrayOfPrimitives::VertexTexel*
  * *void  Graphic3d_ArrayOfPrimitives::VertexColor*

The following example shows how to define an array of  points: 

~~~~~
// create an array
Handle (Graphic3d_ArrayOfPoints) anArray = new Graphic3d_ArrayOfPoints (aVerticiesMaxCount); 

// add vertices to the array
anArray->AddVertex  (10.0, 10.0, 10.0); 
anArray->AddVertex  (0.0, 10.0, 10.0); 

// add the array to the structure
Handle (Graphic3d_Group) aGroup  =  Prs3d_Root::CurrentGroup (aPrs); 
aGroup->BeginPrimitives (); 
aGroup->AddPrimitiveArray (anArray); 
aGroup->EndPrimitives (); 
~~~~~

If the primitives share the same vertices (polygons,  triangles, etc.) then you can define them as indices of the vertices array. 

The method *Graphic3d_ArrayOfPrimitives::AddEdge* allows defining the primitives by indices. This method adds an "edge" in the range <i> [1, VertexNumber() ] </i>  in the array. 

It is also possible to query the vertex defined by an edge using method *Graphic3d_ArrayOfPrimitives::Edge*

The following example shows how to define an array of  triangles: 

~~~~~
// create an array
Standard_Boolean  IsNormals     = Standard_False;  
Standard_Boolean  IsColors      = Standard_False; 
Standard_Boolean  IsTextureCrds = Standard_False; 
Handle (Graphic3d_ArrayOfTriangles) anArray =  
          new Graphic3d_ArrayOfTriangles (aVerticesMaxCount, 
                                          aEdgesMaxCount, 
                                          IsNormals, 
                                          IsColors, 
                                          IsTextureCrds); 
// add vertices to the array
anArray->AddVertex  (-1.0, 0.0, 0.0);   // vertex 1
anArray->AddVertex  ( 1.0, 0.0, 0.0);   // vertex 2
anArray->AddVertex  ( 0.0, 1.0, 0.0);   // vertex 3 
anArray->AddVertex  ( 0.0,-1.0, 0.0);   // vertex 4 

// add edges to the array
anArray->AddEdge  (1);  // first triangle
anArray->AddEdge  (2); 
anArray->AddEdge  (3); 
anArray->AddEdge  (1);  // second triangle
anArray->AddEdge  (2); 
anArray->AddEdge  (4); 

// add the array to the structure
Handle  (Graphic3d_Group) aGroup =  Prs3d_Root::CurrentGroup (aPrs); 
aGroup->BeginPrimitives  (); 
aGroup->AddPrimitiveArray  (anArray); 
aGroup->EndPrimitives  (); 
~~~~~

If the primitive array presents primitives built from  sequential sets of vertices, for example polygons, then you can specify the  bounds, or the number of vertices for each primitive. You can use the method *Graphic3d_ArrayOfPrimitives::AddBound* to define the bounds and the color for each bound. This method returns the actual number of bounds. 

It is also possible to set the color and query the number of  edges in the bound and bound color. 
~~~~~
  Standard_Integer  Graphic3d_ArrayOfPrimitives::Bound
  Quantity_Color  Graphic3d_ArrayOfPrimitives::BoundColor
  void  Graphic3d_ArrayOfPrimitives::BoundColor
~~~~~

The following example shows how to define an array of  polygons: 

~~~~~
// create an array
Standard_Boolean  IsNormals      = Standard_False;  
Standard_Boolean  IsVertexColors = Standard_False; 
Standard_Boolean  IsFaceColors   = Standard_False; 
Standard_Boolean  IsTextureCrds  = Standard_False; 
Handle (Graphic3d_ArrayOfPolygons) anArray =  
          new Graphic3d_ArrayOfPolygons (aVerticesMaxCount, 
                                         aBoundsMaxCount, 
                                         aEdgesMaxCount, 
                                         IsNormals, 
                                         IsVertexColors, 
                                         IsFaceColors, 
                                         IsTextureCrds); 

// add bounds to the array, first polygon
anArray->AddBound (3);  
anArray->AddVertex (-1.0,  0.0, 0.0);    
anArray->AddVertex  ( 1.0, 0.0, 0.0);    
anArray->AddVertex  ( 0.0, 1.0, 0.0);    

// add bounds to the array, second polygon
anArray->AddBound (4); 
anArray->AddVertex (-1.0,  0.0, 0.0);    
anArray->AddVertex  ( 1.0, 0.0, 0.0);    
anArray->AddVertex  ( 1.0,-1.0, 0.0);    
anArray->AddVertex  (-1.0,-1.0, 0.0);    

// add the array to the structure 
Handle  (Graphic3d_Group) aGroup = Prs3d_Root::CurrentGroup  (aPrs); 
aGroup->BeginPrimitives  (); 
aGroup->AddPrimitiveArray  (anArray); 
aGroup->EndPrimitives  (); 
~~~~~

There are also several helper methods. You can get the type  of the primitive array: 
~~~~~
  Graphic3d_TypeOfPrimitiveArray 
  Graphic3d_ArrayOfPrimitives::Type
  Standard_CString  Graphic3d_ArrayOfPrimitives::StringType
~~~~~

and check if the primitive array provides normals, vertex colors and vertex texels (texture coordinates): 

~~~~~
  Standard_Boolean Graphic3d_ArrayOfPrimitives::HasVertexNormals
  Standard_Boolean Graphic3d_ArrayOfPrimitives::HasVertexColors
  Standard_Boolean Graphic3d_ArrayOfPrimitives::HasVertexTexels
~~~~~
or get the number of vertices, edges and bounds: 
~~~~~
  Standard_Integer Graphic3d_ArrayOfPrimitives::VertexNumber
  Standard_Integer Graphic3d_ArrayOfPrimitives::EdgeNumber
  Standard_Integer Graphic3d_ArrayOfPrimitives::BoundNumber
~~~~~
  
@subsection occt_visu_4_2_5 Text primitive

The OpenGl graphics driver uses advanced text rendering  powered by FTGL library. This library provides vector text rendering, as a  result the text can be rotated and zoomed without quality loss.  
*Graphic3d* text primitives have the following features: 
  * fixed size (non-zoomable) or zoomable, 
  * can be rotated to any angle in the view plane,
  * support unicode charset.

The text attributes for the group could be defined with the *Graphic3d_AspectText3d* attributes group. 
To add any text to the graphic structure you can use the  following methods: 
~~~~~
 void Graphic3d_Group::Text
			(const Standard_CString AText, 
			 const Graphic3d_Vertex& APoint, 
			 const Standard_Real AHeight, 
			 const Quantity_PlaneAngle AAngle, 
			 const Graphic3d_TextPath ATp, 
			 const Graphic3d_HorizontalTextAlignment  AHta, 
			 const Graphic3d_VerticalTextAlignment  AVta, 
			 const Standard_Boolean EvalMinMax), 
~~~~~			
*AText*  parameter is the text string, *APoint* is the three-dimensional position of the  text, *AHeight* is the text height, *AAngle* is the orientation of the text (at the  moment, this parameter has no effect, but you can specify the text orientation through  the *Graphic3d_AspectText3d* attributes). 

*ATp* parameter defines the text path, *AHta* is the horizontal alignment of the text,  *AVta* is the vertical alignment of the text. 

You can  pass *Standard_False* as *EvalMinMax* if you do not want the graphic3d structure  boundaries to be affected by the text position.  

**Note** that the text orientation angle can be defined by *Graphic3d_AspectText3d* attributes. 
~~~~~
  void  Graphic3d_Group::Text
			(const Standard_CString AText, 
			 const Graphic3d_Vertex& APoint, 
			 const Standard_Real AHeight, 
			 const Standard_Boolean EvalMinMax) 
  void  Graphic3d_Group::Text
			(const TCcollection_ExtendedString  &AText,  
			 const Graphic3d_Vertex& APoint, 
			 const Standard_Real AHeight, 
			 const Quantity_PlaneAngle AAngle, 
			 const Graphic3d_TextPath ATp, 
			 const Graphic3d_HorizontalTextAlignment  AHta, 
			 const Graphic3d_VerticalTextAlignment  AVta, 
			 const Standard_Boolean EvalMinMax) 
  void  Graphic3d_Group::Text
			(const TCcollection_ExtendedString  &AText,  
			 const Graphic3d_Vertex& APoint, 
			 const Standard_Real AHeight, 
			 const Standard_Boolean EvalMinMax) 
~~~~~

See the example:
~~~~~
// get the group
Handle (Graphic3d_Group) aGroup =  Prs3d_Root::CurrentGroup  (aPrs);  

// change the text  aspect
Handle(Graphic3d_AspectText3d) aTextAspect =  new Graphic3d_AspectText3d (); 
aTextAspect->SetTextZoomable  (Standard_True); 
aTextAspect->SetTextAngle  (45.0); 
aGroup->SetPrimitivesAspect (aTextAspect);   

// add a text primitive  to the structure
Graphic3d_Vertex aPoint (1, 1, 1); 
aGroup->Text (Standard_CString ("Text"), aPoint, 16.0); 
~~~~~

@subsection occt_visu_4_2_6 Materials

A *Graphic3d_MaterialAspect* is defined by:
  * Transparency;
  * Diffuse reflection -- a component of the object color;
  * Ambient reflection;
  * Specular reflection -- a component of the color of the light source;
  * Refraction index.

The following items are required to determine the three colors of reflection:
  * Color;
  * Coefficient of diffuse reflection;
  * Coefficient of ambient reflection;
  * Coefficient of specular reflection.

@subsection occt_visu_4_2_7 Textures

A *texture* is defined by a name.
Three types of texture are available:
  * 1D;
  * 2D;
  * Environment mapping.

@subsection occt_visu_4_2_8 Shaders

OCCT visualization core supports GLSL shaders. Currently OCCT supports only vertex and fragment GLSL shader. Shaders can be assigned to a generic presentation by its drawer attributes (Graphic3d aspects). To enable custom shader for a specific AISShape in your application, the following API functions are used:

~~~~~
// Create shader program
Handle(Graphic3d_ShaderProgram) aProgram = new Graphic3d_ShaderProgram();

// Attach vertex shader
aProgram->AttachShader (Graphic3d_ShaderObject::CreateFromFile(
                               Graphic3d_TOS_VERTEX, "<Path to VS>"));

// Attach fragment shader
aProgram->AttachShader (Graphic3d_ShaderObject::CreateFromFile(
                               Graphic3d_TOS_FRAGMENT, "<Path to FS>"));

// Set values for custom uniform variables (if they are)
aProgram->PushVariable ("MyColor", Graphic3d_Vec3(0.0f, 1.0f, 0.0f));

// Set aspect property for specific AISShape
theAISShape->Attributes()->ShadingAspect()->Aspect()->SetShaderProgram (aProgram);
~~~~~

@section occt_visu_4_3 Graphic attributes

@subsection occt_visu_4_3_1 Aspect package overview

The *Aspect* package provides classes for the graphic elements in the viewer:
  * Groups of graphic attributes;
  * Edges, lines, background;
  * Window;
  * Driver;
  * Enumerations for many of the above.

@section occt_visu_4_4 3D view facilities

@subsection occt_visu_4_4_1 Overview

The *V3d* package provides the resources to define a 3D  viewer and the views attached to this viewer (orthographic, perspective). This  package provides the commands to manipulate the graphic scene of any 3D object  visualized in a view on screen.  

A set of high-level commands allows the separate  manipulation of parameters and the result of a projection (Rotations, Zoom,  Panning, etc.) as well as the visualization attributes (Mode, Lighting,  Clipping, Depth-cueing, etc.) in any particular view.  

The *V3d* package is basically a set of tools directed by  commands from the viewer front-end. This tool set contains methods for creating  and editing classes of the viewer such as:  
  * Default parameters of the viewer, 
  * Views (orthographic, perspective), 
  * Lighting (positional, directional, ambient, spot, headlight), 
  * Clipping planes (note that only Z-clipping planes can work with  the Phigs interface), 
  * Instantiated sequences of views, planes, light sources, graphic  structures, and picks, 
  * Various package methods. 

@subsection occt_visu_4_4_2 A programming example

This sample TEST program for the *V3d* Package uses primary  packages *Xw* and *Graphic3d* and secondary packages *Visual3d, Aspect, Quantity,  Phigs* and *math*.  

~~~~~
//Create a default display  connection
Handle(Aspect_DisplayConnection)  aDisplayConnection = new  Aspect_DisplayConnection(); 

//Create a Graphic Driver from  the default Aspect_DisplayConnection
Handle(OpenGl_GraphicDriver)  GD = new OpenGl_GraphicDriver (aDisplayConnection);

//Create a Viewer to this Driver 
Handle(V3d_Viewer)  VM = new V3d_Viewer(GD, 400.,  
	// Space size  
	V3d_Xpos,
	// Default projection  
	Quantity_NOC_DARKVIOLET, 
	// Default  background  
	V3d_ZBUFFER, 
	// Type of  visualization  
	V3d_GOURAUD, 
	// Shading  model  
	V3d_WAIT); 
	// Update mode   
// Create a structure in this  Viewer 
Handle(Graphic3d_Structure) S =  new  Graphic3d_Structure(VM->Viewer()) ;  

// Type of structure 
S->SetVisual (Graphic3d_TOS_SHADING);  

// Create a group of primitives  in this structure
Handle(Graphic3d_Group) G = new Graphic3d_Group(S) ;  

// Fill this group with  one polygon of size 100  
Graphic3d_Array1OfVertex Points(0,3) ;  
Points(0).SetCoord(-100./2.,-100./2.,-100./2.) ;  
Points(1).SetCoord(-100./2., 100./2.,-100./2.) ;  
Points(2).SetCoord( 100./2., 100./2.,-100./2.) ;  
Points(3).SetCoord( 100./2.,-100./2.,-100./2.) ;  
Normal.SetCoord(0.,0.,1.) ;  
G->Polygon(Points,Normal) ;  

//  Create Ambient and Infinite Lights in this Viewer
Handle(V3d_AmbientLight) L1  = new V3d_AmbientLight  
	(VM,Quantity_NOC_GRAY50)  ;  
Handle(V3d_DirectionalLight)  L2 = new V3d_DirectionalLight  
	(VM,V3d_XnegYnegZneg,Quantity_NOC_WHITE)  ;  

// Create a 3D quality  Window with the same DisplayConnection
Handle(Xw_Window) W =  new Xw_Window(aDisplayConnection,"Test  V3d",0.5,0.5,0.5,0.5) ;  

// Map this Window to  this screen
W->Map() ;  

// Create a Perspective  View in this Viewer
Handle(V3d_View) aView = new V3d_View(VM);
aView->Camera()->SetProjectionType (Graphic3d_Camera::Projection_Perspective);
// Associate this View with the Window
aView ->SetWindow(W);
// Display ALL structures in this View
VM->Viewer()->Display();
// Finally update the Visualization in this View
aView->Update();
~~~~~

As an alternative to manual setting of perspective parameters the *V3d_View::ZfitAll()* and *V3d_View::FitAll()* functions can be used:

~~~~~
// Display shape in Viewer VM
Handle(AIS_InteractiveContext) aContext = new AIS_InteractiveContext (VM);
aContext->Display(shape);
// Create a Perspective View in Viewer VM
Handle(V3d_View) V = new V3d_View (VM);
aview->Camera()->SetProjectionType (Graphic3d_Camera::Projection_Perspective);
// Change Z-min and Z-max planes of projection volume to match the displayed objects
V->ZFitAll();
// Fit view to object size
V->FitAll();
~~~~~

@subsection occt_visu_4_4_3 Define viewing parameters

View projection and orientation in OCCT *v3d* view are driven by camera. The camera calculates and supplies projection and view orientation matrices for rendering by OpenGL. The allows to the user to control all projection parameters. The camera is defined by the following properties:

* **Eye** -- defines the observer (camera) position. Make sure the Eye point never gets between the Front and Back clipping planes.

* **Center** -- defines the origin of View Reference Coordinates (where camera is aimed at).

* **Direction** -- defines the direction of camera view (from the Eye to the Center).

* **Distance** -- defines the distance between the Eye and the Center.

* **Front** Plane -- defines the position of the front clipping plane in View Reference Coordinates system.

* **Back** Plane -- defines the position of the back clipping plane in View Reference Coordinates system.

* **ZNear** -- defines the distance between the Eye and the Front plane.

* **ZFar** -- defines the distance between the Eye and the Back plane.

Most common view manipulations (panning, zooming, rotation) are implemented as convenience methods of *V3d_View* class, however *Graphic3d_Camera* class can also be used directly by application developers:

Example:
~~~~~
// rotate camera by X axis on 30.0 degrees
gp_Trsf aTrsf;
aTrsf.SetRotation (gp_Ax1 (gp_Pnt (0.0, 0.0, 0.0), gp_Dir (1.0, 0.0, 0.0)), 30.0);
aView->Camera()->Transform (aTrsf);
~~~~~

@subsection occt_visu_4_4_4 Orthographic Projection

@image html view_frustum.png "Perspective and orthographic projection"

The following code configures the camera for orthographic rendering:

~~~~~
// Create an orthographic View in this Viewer
Handle(V3d_View) aView = new V3d_View (VM);
aView->Camera()->SetProjectionType (Graphic3d_Camera::Projection_Orthographic);
// update the Visualization in this View
aView->Update();
~~~~~

@subsection occt_visu_4_4_5 Perspective Projection

**Field of view (FOVy)** -- defines the field of camera view by y axis in degrees (45° is default).

@image html camera_perspective.png "Perspective frustum"

The following code configures the camera for perspective rendering:

~~~~~
// Create a perspective View in this Viewer
Handle(V3d_View) aView = new V3d_View(VM);
aView->Camera()->SetProjectionType (Graphic3d_Camera::Projection_Perspective);
aView->Update();
~~~~~


@subsection occt_visu_4_4_6 Stereographic Projection

**IOD** -- defines the intraocular distance (in world space units).

There are two types of IOD:
* _IODType_Absolute_ : Intraocular distance is defined as an absolute value.
* _IODType_Relative_ : Intraocular distance is defined relative to the camera focal length (as its coefficient).

**Field of view (FOV)** -- defines the field of camera view by y axis in degrees (45° is default).

**ZFocus** -- defines the distance to the point of stereographic focus.

@image html stereo.png "Stereographic projection"

To enable stereo projection, your workstation should meet the following requirements:

* The graphic card should support quad buffering.
* You need active 3D glasses (LCD shutter glasses).
* The graphic driver needs to be configured to impose quad buffering for newly created OpenGl contexts; the viewer and the view should be created after that.

In stereographic projection mode the camera prepares two projection matrices to display different stereo-pictures for the left and for the right eye. In a non-stereo camera this effect is not visible because only the same projection is used for both eyes.

To enable quad buffering support you should provide the following settings to the graphic driver *opengl_caps*:

~~~~~
Handle(OpenGl_GraphicDriver) aDriver = new OpenGl_GraphicDriver();
OpenGl_Caps& aCaps = aDriver->ChangeOptions();
aCaps.contextStereo = Standard_True;
~~~~~

The following code configures the camera for stereographic rendering:

~~~~~
// Create a Stereographic View in this Viewer
Handle(V3d_View) aView = new V3d_View(VM);
aView->Camera()->SetProjectionType (Graphic3d_Camera::Projection_Stereo);
// Change stereo parameters
aView->Camera()->SetIOD (IODType_Absolute, 5.0);
// Finally update the Visualization in this View
aView->Update();
~~~~~

@subsection occt_visu_4_4_7 View frustum culling

The algorithm of frustum culling on CPU-side is activated by default for 3D viewer. This algorithm allows skipping the presentation outside camera at the rendering stage, providing better performance. The following features support this method:
* *Graphic3d_Structure::CalculateBoundBox()* is used to calculate axis-aligned bounding box of a presentation considering its transformation.
* *V3d_View::SetFrustumCulling* enables or disables frustum culling for the specified view.
* Classes *OpenGl_BVHClipPrimitiveSet* and *OpenGl_BVHTreeSelector* handle the detection of outer objects and usage of acceleration structure for frustum culling.
* *BVH_BinnedBuilder* class splits several objects with null bounding box.

@subsection occt_visu_4_4_8 Underlay and overlay layers management

In addition to interactive 3d graphics displayed in the view  you can display underlying and overlying graphics: text, color scales and drawings.

All *V3d* view graphical objects in the overlay are  managed by the default layer manager (*V3d_LayerMgr*). The *V3d* view has a  basic layer manager capable of displaying the color scale, but you can redefine  this class to provide your own overlay and underlay graphics.  

The method *V3d_View::SetLayerMgr(const Handle (V3d_LayerMgr)& aMgr)* allows assigning a custom layer manager to the *V3d* view.

There are three virtual methods to prepare graphics in the  manager for further drawing: setting up layer dimensions and drawing static graphics. These methods can be redefined:

~~~~~
   void  V3d_LayerMgr::Begin ()
   void  V3d_LayerMgr::Redraw ()
   void V3d_LayerMgr::End  ()
~~~~~

The layer manager controls layers (*Visual3d_Layer*)  and layer items (*Visual3d_LayerItem*). Both the overlay and  underlay layers can be created by the layer manager.  

The layer entity is presented by the *Visual3d_Layer* class. This entity provides drawing services in the layer, for example:
~~~~~ 
   void  Visual3d_Layer::DrawText
   void Visual3d_Layer::DrawRectangle
   void  Visual3d_Layer::SetColor
   void  Visual3d_Layer::SetViewport
~~~~~

The following example demonstrates how to draw overlay graphics by the *V3d_LayerMgr*:

~~~~~
// redefined method of  V3d_LayerMgr
void  MyLayerMgr::Redraw () 
{ 
   Quantity_Color aRed (Quantity_NOC_RED); 
   myOverlayLayer->SetColor (aRed); 
   myOverlayLayer->DrawRectangle (0, 0, 100, 100); 
} 
~~~~~

The layer contains layer items that will be displayed on  view redraw. Such items are *Visual3d_LayerItem* entities. To manipulate *Visual3d_LayerItem* entities assigned to the layer's internal list you can use the following methods: 

~~~~~
   void  Visual3d_Layer::AddLayerItem (const Handle (Visual3d_LayerItem)&  Item)  
   void  Visual3d_Layer::RemoveLayerItem (const Handle (Visual3d_LayerItem)&  Item)   
   void  Visual3d_Layer::RemoveAllLayerItems ()
   const  Visual3d_NListOfLayerItem& Visual3d_Layer::GetLayerItemList ()    
~~~~~
The layer's items are rendered when the method *void  Visual3d_Layer::RenderLayerItems()* is  called by the graphical driver.

The *Visual3d_LayerItem* has virtual methods that are  used to render the item: 
~~~~~
   void  Visual3d_LayerItem::RedrawLayerPrs () 
   void  Visual3d_LayerItem::ComputeLayerPrs () 
~~~~~

The item presentation can be computed before drawing by the *ComputeLayerPrs* method to save time on redraw. It also has an additional  flag that is used to tell that the presentation should be recomputed: 
~~~~~
   void  Visual3d_LayerItem::SetNeedToRecompute (const Standard_Boolean NeedToRecompute)  
   Standard_Boolean  Visual3d_LayerItem::IsNeedToRecompute 
~~~~~

An example of *Visual3d_LayerItem* is *V3d_ColorScaleLayerItem*  that represents the color scale entity as the layer's item.  
The *V3d_ColorScaleLayerItem* sends render requests to  the color scale entity represented by it. As this entity (*V3d_ColorScale*)  is assigned to the *V3d_LayerMgr* it uses its overlay layer's services for  drawing: 

<h4>Example </h4>

~~~~~
// tell V3d_ColorScale to draw itself
void  V3d_ColorScaleLayerItem::RedrawLayerPrs () 
{ 
   Visual3d_LayerItem::RedrawLayerPrs () 
  if  (!MyColorScale.IsNull ()) 
    MyColorScale->DrawScale  (); 
} 

// V3d_ColorScale has a reference to a LayerMgr
void  V3d_ColorScale::DrawScale () 
{ 
    // calls V3d_ColorScale::PaintRect,  V3d_ColorScale::PaintText, etc. 
} 

// PaintRect method uses overlay layer of LayerMgr to  draw a rectangle 
void V3d_ColorScale::PaintRect   
       (const  Standard_Integer X, const Standard_Integer Y, 
        const  Standard_Integer W, const Standard_Integer H, 
        const  Quantity_Color aColor, 
        const  Standard_Boolean aFilled) 
{ 
  const Handle  (Visual3d_Layer)& theLayer = myLayerMgr->Overlay (); 
   ...
   theLayer->SetColor (aColor); 
   theLayer->DrawRectangle (X, Y, W, H); 
   ... 
} 
~~~~~

@subsection occt_visu_4_4_9 View background styles
There are three types of  background styles available for *V3d_view*: solid color, gradient color and  image.  

To set solid color for  the background you can use the following methods: 
~~~~~
   void  V3d_View::SetBackgroundColor
		(const Quantity_TypeOfColor Type,  
		 const Quantity_Parameter V1, 
		 const Quantity_Parameter V2, 
		 const Quantity_Parameter V3) 
~~~~~

This method allows you to specify the background color in RGB (red,  green, blue) or HLS (hue, lightness, saturation) color spaces, so the  appropriate values of the Type parameter are *Quantity_TOC_RGB* and  *Quantity_TOC_HLS*. 

**Note** that the color  value parameters *V1,V2,V3* should be in the range between *0.0-1.0.* 

~~~~~
  void  V3d_View::SetBackgroundColor(const Quantity_Color &Color)  
  void  V3d_View::SetBackgroundColor(const Quantity_NameOfColor Name)  
~~~~~

The gradient background  style could be set up with the following methods: 
~~~~~
  void  V3d_View::SetBgGradientColors
		(const Quantity_Color& Color1,  
		 const Quantity_Color& Color2, 
		 const Aspect_GradientFillMethod  FillStyle, 
		 const Standard_Boolean update) 
		
   void  V3d_View::SetBgGradientColors
		(const Quantity_NameOfColor Color1,  
		 const Quantity_NameOfColor Color2, 
		 const Aspect_GradientFillMethod  FillStyle, 
		 const Standard_Boolean update) 
~~~~~

The *Color1* and *Color2* parameters define the boundary colors of  interpolation, the *FillStyle* parameter defines the direction of interpolation.  You can pass *Standard_True* as the last parameter to update the view.  

The fill style can be also set with the method *void  V3d_View::SetBgGradientStyle(const Aspect_GradientFillMethod AMethod, const Standard_Boolean update)*. 

To get the current  background color you can use the following methods: 
~~~~~
   void  V3d_View::BackgroundColor
		(const Quantity_TypeOfColor Type,  
		 Quantity_Parameter &V1, 
		 Quantity_Parameter &V2, 
		 Quantity_Parameter &V3) 
   Quantity_Color  V3d_View::BackgroundColor()
   void V3d_View::GradientBackgroundColors(Quantity_Color& Color1, Quantity_Color& Color2) 
   Aspect_GradientBackground  GradientBackground()
~~~~~
   
To set the image as a  background and change the background image style you can use the following  methods: 
~~~~~  
  void V3d_View::SetBackgroundImage
		(const Standard_CString FileName,  
		 const Aspect_FillMethod FillStyle, 
		 const Standard_Boolean update) 
  void  V3d_View::SetBgImageStyle
		(const Aspect_FillMethod FillStyle,  
		 const Standard_Boolean update) 
~~~~~

The *FileName* parameter defines the image file name and the path to it,  the *FillStyle* parameter defines the method of filling the background with the  image. The methods are:  
  * *Aspect_FM_NONE* --  draws the image in the default position;
  * *Aspect_FM_CENTERED* -- draws the image at the center of the view;
  * *Aspect_FM_TILED* -- tiles the view with the image;
  * *Aspect_FM_STRETCH* -- stretches the image over the view.


@subsection occt_visu_4_4_10 Dumping a 3D scene into an image file

The 3D scene displayed in the view can be dumped in high resolution into an image file. The high resolution (8192x8192 on some implementations) is achieved using the Frame Buffer Objects (FBO) provided by the graphic driver. Frame Buffer Objects enable off-screen rendering into a virtual view to produce images in the background mode (without displaying any graphics on the screen).

The *V3d_View* has the following methods for  dumping the 3D scene: 
~~~~
Standard_Boolean  V3d_View::Dump 
	(const Standard_CString theFile, 
	 const Image_TypeOfImage theBufferType)
~~~~
Dumps the scene into an image file with the view  dimensions.

~~~~
Standard_Boolean  V3d_View::Dump 
	(const Standard_CString theFile, 
	 const Aspect_FormatOfSheetPaper  theFormat, 
	 const Image_TypeOfImage theBufferType)
~~~~
Makes the dimensions of the output image compatible to a certain format of printing paper passed by *theFormat* argument.  
  
These methods dump the 3D scene into an image file passed by its name  and path as theFile.  

The raster image data handling algorithm is based on the *Image_PixMap* class. The supported extensions are ".png", ".bmp", ".png", ".png".

The value passed as *theBufferType* argument defines the type of the  buffer for an output image <i>(RGB, RGBA, floating-point, RGBF, RGBAF)</i>. Both  methods return *Standard_True* if the scene has been successfully dumped.  

There is also class *Image_AlienPixMap* providing import / export from / to external image files in formats supported by **FreeImage** library.

**Note** that dumping the image for a paper format with  large dimensions is a memory consuming operation, it might be necessary to take  care of preparing enough free memory to perform this operation. 

~~~~
Handle_Image_PixMap  V3d_View::ToPixMap 
	(const Standard_Integer theWidth, 
	const Standard_Integer theHeight, 
	const Image_TypeOfImage theBufferType, 
	const Standard_Boolean theForceCentered) 
~~~~
Dumps the displayed 3d scene into a pixmap  with a width and height passed as *theWidth* and theHeight arguments.  

The value passed as *theBufferType* argument defines the type of the  buffer for a pixmap <i>(RGB, RGBA, floating-point, RGBF, RGBAF)</i>.  The last parameter allows centering the 3D scene on dumping. 

All these methods assume that you have  created a view and displayed a 3d scene in it. However, the window used for  such a view could be virtual, so you can dump the 3d scene in the background  mode without displaying it on the screen. To use such an opportunity you can  perform the following steps: 
* Create display connection; 
* Initialize graphic driver; 
* Create a window; 
* Set up the window as virtual, *Aspect_Window::SetVirtual()* ; 
* Create a view and an interactive context; 
* Assign the virtual window to the view; 
* Display a 3D scene; 
*  Use one of the functions described above to dump the 3D scene. 

The following example demonstrates this  procedure for *WNT_Window* : 

~~~~~
// create a dummy display  connection
Handle(Aspect_DisplayConnection)  aDisplayConnection;

// create a graphic driver
Handle  (Graphic3d_GraphicDriver) aDriver = Graphic3d::InitGraphicDriver (aDisplayConnection); 

// create a window
Standard_Integer  aDefWidth  = 800; 
Standard_Integer  aDefHeight = 600; 
Handle  (WNT_WClass) aWClass =  new WNT_WClass ("Virtual Class",DefWindowProc, 
                              CS_VREDRAW | CS_HREDRAW, 0, 0,  
                              ::LoadCursor (NULL, IDC_ARROW)); 
Handle  (WNT_Window) aWindow =  new WNT_Window ("VirtualWnd",  aWClass,  
                              WS_OVERLAPPEDWINDOW, 0, 0,  
                              aDefWidth, aDefHeight); 

// set up the window as  virtual
aWindow->SetVirtual  (Standard_True); 

// create a view and an  interactive context
Handle  (V3d_Viewer) aViewer =  new V3d_Viewer (aDriver, 
                              Standard_ExtString ("Virtual")); 
Handle  (AIS_InteractiveContext) aContext = new AIS_InteractiveContext (aViewer); 
Handle  (V3d_View) aView = aViewer->CreateView (); 

// assign the virtual window  to the view
aView->SetWindow  (aWindow); 

// display a 3D scene
Handle (AIS_Shape) aBox = new AIS_Shape (BRepPrimAPI_MakeBox (5, 5, 5)); 
aContext->Display  (aBox); 
aView->FitAll(); 

// dump the 3D scene into an  image file
aView->Dump  ("3dscene.png"); 
~~~~~

@subsection occt_visu_4_4_11 Printing a 3D scene

The contents of a view can be printed out. Moreover, the OpenGl graphic driver used by the v3d view supports printing in high  resolution. The print method uses the OpenGl frame buffer (Frame Buffer Object)  for rendering the view contents and advanced print algorithms that allow  printing in, theoretically, any resolution.  

The method *void V3d_View::Print(const Aspect_Handle hPrnDC, const Standard_Boolean showDialog, const Standard_Boolean showBackground, const Standard_CString  filename, const Aspect_PrintAlgo printAlgorithm)* prints the view  contents: 

*hPrnDC* is the printer device handle. You can pass your own printer handle  or *NULL* to select the printer by the default dialog. In that case you can use  the default dialog or pass *Standard_False* as the *showDialog* argument to  select the default printer automatically.  

You can define  the filename for the printer driver if you want to print out the result into a  file.  
If you do not want to print the  background, you can pass *Standard_False* as the *showBackground* argument. 
The *printAlgorithm* argument allows choosing between two print algorithms that  define how the 3d scene is mapped to the print area when the maximum dimensions of the frame buffer are smaller than the dimensions of the print area by choosing *Aspect_PA_STRETCH* or *Aspect_PA_TILE*

The first value  defines the stretch algorithm: the scene is drawn with the maximum possible  frame buffer dimensions and then is stretched to the whole printing area. The  second value defines *TileSplit* algorithm: covering the whole printing area by  rendering multiple parts of the viewer. 

**Note** that at the moment the printing is implemented only for Windows.

@subsection occt_visu_4_4_12 Vector image export

The 3D content of a view can be exported to  the vector image file format. The vector image export is powered by the *GL2PS*  library. You can export 3D scenes into a file format supported by the  GL2PS library: PostScript (PS), Encapsulated PostScript (EPS), Portable  Document Format (PDF), Scalable Vector Graphics (SVG), LaTeX file format and  Portable LaTeX Graphics (PGF).  

The method   *void  Visual3d_View::Export (const Standard_CString FileName, const Graphic3d_ExportFormat Format, const Graphic3d_SortType aSortType, const Standard_Real Precision, const Standard_Address ProgressBarFunc, const Standard_Address ProgressObject)* of *Visual3d_View* class  allows exporting a 3D scene: 

The *FileName*  defines the output image file name and the *Format* argument defines the output  file format:  
  * *Graphic3d_EF_PostScript  (PS)*,
  * *Graphic3d_EF_EhnPostScript  (EPS)*,
  * *Graphic3d_EF_TEX  (TEX)*,
  * *Graphic3d_EF_PDF  (PDF)*,
  * *Graphic3d_EF_SVG  (SVG)*,
  * *Graphic3d_EF_PGF  (PGF)*.

The *aSortType* parameter defines *GL2PS* sorting algorithm for the  primitives. The *Precision, ProgressBarFunc* and *ProgressObject* parameters are  implemented for future uses and at the moment have no effect. 

The *Export* method supports only basic 3d  graphics and has several limitations: 
  * Rendering large scenes could be slow and  can lead to large output files;
  * Transparency is only supported for PDF and  SVG output;
  * Textures and some effects are not supported by the *GL2PS* library.

@subsection occt_visu_4_4_13 Ray tracing support

OCCT visualization provides rendering by real-time ray tracing technique. It is allowed to switch easily between usual rasterization and ray tracing rendering modes. The core of OCCT ray tracing is written using GLSL shaders. The ray tracing has a wide list of features:
* Hard shadows
* Refractions
* Reflection
* Transparency
* Texturing
* Support of non-polygon objects, such as lines, text, highlighting, selection.
* Performance optimization using 2-level bounding volume hierarchy (BVH).

The ray tracing algorithm is recursive (Whitted's algorithm). It uses BVH effective optimization structure. The structure prepares optimized data for a scene geometry for further displaying it in real-time. The time-consuming re-computation of the BVH is not necessary for view operations, selections, animation and even editing of the scene by transforming location of the objects. It is only necessary when the list of displayed objects or their geometry changes.
To make the BVH reusable it has been added into an individual reusable OCCT package *TKMath/BVH*.

There are several ray-tracing options that user can switch on/off:
* Maximum ray tracing depth
* Shadows rendering
* Specular reflections
* Adaptive anti aliasing
* Transparency shadow effects

Example:
~~~~~
Graphic3d_RenderingParams& aParams = aView->ChangeRenderingParams();
// specifies rendering mode
aParams.Method = Graphic3d_RM_RAYTRACING;
// maximum ray-tracing depth
aParams.RaytracingDepth = 3;
// enable shadows rendering
aParams.IsShadowEnabled = Standard_True;
// enable specular reflections.
aParams.IsReflectionEnabled = Standard_True;
// enable adaptive anti-aliasing
aParams.IsAntialiasingEnabled = Standard_True;
// enable light propagation through transparent media.
aParams.IsTransparentShadowEnabled = Standard_True;
// update the view
aView->Update();
~~~~~

@subsection occt_visu_4_4_14 Display priorities

Structure display priorities control the order, in which structures are drawn. When you display a structure you specify its priority.  The lower is the value, the lower is the display priority. When the display is regenerated, the structures with the lowest priority are drawn first. The structures with the same display priority are drawn in the same order as they have been displayed. OCCT supports eleven structure display priorities.

@subsection occt_visu_4_4_15 Z-layer support

OCCT features depth-arranging functionality called z-layer. A graphical presentation can be put into a z-layer. In general, this function can be used for implementing "bring to front" functionality in a graphical application.

Example:

~~~~~
// set z-layer to an interactive object
Handle(AIS_InteractiveContext) aContext = ...
Handle(AIS_InteractiveObject) anInterObj = ...
Standard_Integer anId = 3;
aViewer->AddZLayer (anId);
aContext->SetZLayer (anInterObj, anId);
~~~~~

For each z-layer, it is allowed to:
* Enable / disable depth test for layer.
* Enable / disable depth write for layer.
* Enable / disable depth buffer clearing.
* Enable / disable polygon offset.

The corresponding method *SetZLayerOption (...)* is available in *Graphic3d_GraphicDriver* interface. You can get the options using getter from *Visual3d_ViewManager* and *V3d_Viewer*. It returns *Graphic3d_ZLayerSettings* cached in *Visual3d_ViewManager* for a given *LayerId*.

Example:
~~~~~
// change z-layer settings
Graphic3d_ZLayerSettings aSettings = aViewer->ZLayerSettings (anId);
aSettings.EnableSetting (Graphic3d_ZLayerDepthTest);
aSettings.EnableSetting (Graphic3d_ZLayerDepthWrite);
aSettings.EnableSetting (Graphic3d_ZLayerDepthClear);
aSettings.EnableSetting (Graphic3d_ZLayerDepthOffset);
aViewer->SetZLayerSettings (anId, aSettings);
~~~~~


@subsection occt_visu_4_4_16 Clipping planes

The ability to define custom clipping planes could be very useful for some tasks. OCCT provides such an opportunity.

The *Graphic3d_ClipPlane* class provides the services for clipping planes: it holds the plane equation coefficients and provides its graphical representation. To set and get plane equation coefficients you can use the following methods:

~~~~~
Graphic3d_ClipPlane::Graphic3d_ClipPlane(const gp_Pln& thePlane)
void Graphic3d_ClipPlane::SetEquation (const gp_Pln& thePlane)
Graphic3d_ClipPlane::Graphic3d_ClipPlane(const Equation& theEquation)
void Graphic3d_ClipPlane::SetEquation (const Equation& theEquation)
gp_Pln Graphic3d_ClipPlane::ToPlane() const
~~~~~

The clipping planes can be activated with the following method:
~~~~~
void Graphic3d_ClipPlane::SetOn (const Standard_Boolean theIsOn)
~~~~~

The number of clipping planes is limited. You can check the limit value via method *Graphic3d_GraphicDriver::InquirePlaneLimit()*;

~~~~~
// get the limit of clipping planes for the current view
Standard_Integer aMaxClipPlanes = aView->Viewer()->Driver()->InquirePlaneLimit();
~~~~~

Let us see for example how to create a new clipping plane with custom parameters and add it to a view or to an object:
~~~~~
// create a new clipping plane
const Handle(Graphic3d_ClipPlane)& aClipPlane = new Graphic3d_ClipPlane();
// change equation of the clipping plane
Standard_Real aCoeffA = ...
Standard_Real aCoeffB = ...
Standard_Real aCoeffC = ...
Standard_Real aCoeffD = ...
aClipPlane->SetEquation (gp_Pln (aCoeffA, aCoeffB, aCoeffC, aCoeffD));
// set capping
aClipPlane->SetCapping (aCappingArg == "on");
// set the material with red color of clipping plane
Graphic3d_MaterialAspect aMat = aClipPlane->CappingMaterial();
Quantity_Color aColor (1.0, 0.0, 0.0, Quantity_TOC_RGB);
aMat.SetAmbientColor (aColor);
aMat.SetDiffuseColor (aColor);
aClipPlane->SetCappingMaterial (aMat);
// set the texture of clipping plane
Handle(Graphic3d_Texture2Dmanual) aTexture = ...
aTexture->EnableModulate();
aTexture->EnableRepeat();
aClipPlane->SetCappingTexture (aTexture);
// add the clipping plane to an interactive object
Handle(AIS_InteractiveObject) aIObj = ...
aIObj->AddClipPlane (aClipPlane);
// or to the whole view
aView->AddClipPlane (aClipPlane);
// activate the clipping plane
aClipPlane->SetOn(Standard_True);
// update the view
aView->Update();
~~~~~


@subsection occt_visu_4_4_17 Automatic back face culling

Back face culling reduces the rendered number of triangles (which improves the performance) and eliminates artifacts at shape boundaries. However, this option can be used only for solid objects, where the interior is actually invisible from any point of view. Automatic back-face culling mechanism is turned on by default, which is controlled by *V3d_View::SetBackFacingModel()*.

The following features are applied in *StdPrs_ToolShadedShape::IsClosed()*, which is used for definition of back face culling in *ShadingAspect*:
* disable culling for free closed Shells (not inside the Solid) since reversed orientation of a free Shell is a valid case;
* enable culling for Solids packed into a compound;
* ignore Solids with incomplete triangulation.

Back face culling is turned off at TKOpenGl level in the following cases:
* clipping/capping planes are in effect;
* for translucent objects;
* with hatching presentation style.

@section occt_visu_4_5 Examples: creating a 3D scene

To create 3D graphic objects and display them in the screen,  follow the procedure below:
1. Create attributes.
2. Create a 3D viewer.
3. Create a view.
4. Create an interactive context.
5. Create interactive objects.
6. Create primitives in the interactive  object.
7. Display the interactive object.

@subsection occt_visu_4_5_1 Create attributes

Create colors.

~~~~~
Quantity_Color aBlack (Quantity_NOC_BLACK);
Quantity_Color aBlue (Quantity_NOC_MATRABLUE);
Quantity_Color aBrown (Quantity_NOC_BROWN4);
Quantity_Color aFirebrick (Quantity_NOC_FIREBRICK);
Quantity_Color aForest (Quantity_NOC_FORESTGREEN);
Quantity_Color aGray (Quantity_NOC_GRAY70);
Quantity_Color aMyColor (0.99, 0.65, 0.31, Quantity_TOC_RGB);
Quantity_Color aBeet (Quantity_NOC_BEET);
Quantity_Color aWhite (Quantity_NOC_WHITE);
~~~~~

Create line attributes.

~~~~~
Handle(Graphic3d_AspectLine3d) anAspectBrown = new Graphic3d_AspectLine3d();
Handle(Graphic3d_AspectLine3d) anAspectBlue = new Graphic3d_AspectLine3d();
Handle(Graphic3d_AspectLine3d) anAspectWhite = new Graphic3d_AspectLine3d();
anAspectBrown->SetColor (aBrown);
anAspectBlue ->SetColor (aBlue);
anAspectWhite->SetColor (aWhite);
~~~~~

Create marker attributes.
~~~~~
Handle(Graphic3d_AspectMarker3d aFirebrickMarker = new Graphic3d_AspectMarker3d();
// marker attributes
aFirebrickMarker->SetColor (Firebrick);
aFirebrickMarker->SetScale (1.0);
aFirebrickMarker->SetType (Aspect_TOM_BALL);
// or this
// it is a preferred way (supports full-color images on modern hardware).
aFirebrickMarker->SetMarkerImage (theImage)
~~~~~

Create facet attributes.
~~~~~
Handle(Graphic3d_AspectFillArea3d) aFaceAspect =  new Graphic3d_AspectFillArea3d();
Graphic3d_MaterialAspect aBrassMaterial (Graphic3d_NOM_BRASS);
Graphic3d_MaterialAspect aGoldMaterial  (Graphic3d_NOM_GOLD);
aFaceAspect->SetInteriorStyle (Aspect_IS_SOLID);
aFaceAspect->SetInteriorColor (aMyColor);
aFaceAspect->SetDistinguishOn ();
aFaceAspect->SetFrontMaterial (aGoldMaterial);
aFaceAspect->SetBackMaterial  (aBrassMaterial);
aFaceAspect->SetEdgeOn();
~~~~~

Create text attributes.
~~~~~
Handle(Graphic3d_AspectText3d) aTextAspect = new Graphic3d_AspectText3d (aForest, Graphic3d_NOF_ASCII_MONO, 1.0, 0.0);
~~~~~

@subsection occt_visu_4_5_2 Create a 3D Viewer (a Windows example)

~~~~~
// create a default connection
Handle(Aspect_DisplayConnection) aDisplayConnection;
// create a graphic driver from default connection
Handle(OpenGl_GraphicDriver) aGraphicDriver = new OpenGl_GraphicDriver (GetDisplayConnection());
// create a viewer
TCollection_ExtendedString aName ("3DV");
myViewer = new V3d_Viewer (aGraphicDriver,aName.ToExtString(), "");
// set parameters for V3d_Viewer
// defines default lights -
//   positional-light 0.3 0.0 0.0
//   directional-light V3d_XnegYposZpos
//   directional-light V3d_XnegYneg
//   ambient-light
a3DViewer->SetDefaultLights();
// activates all the lights defined in this viewer
a3DViewer->SetLightOn();
// set background color to black
a3DViewer->SetDefaultBackgroundColor (Quantity_NOC_BLACK);
~~~~~


@subsection occt_visu_4_5_3 Create a 3D view (a Windows example)

It is assumed that a valid Windows window may already be  accessed via the method *GetSafeHwnd()*.
~~~~~
Handle (WNT_Window) aWNTWindow = new WNT_Window (GetSafeHwnd());
myView = myViewer->CreateView();
myView->SetWindow (aWNTWindow);
~~~~~

@subsection occt_visu_4_5_4 Create an interactive context

~~~~~
myAISContext = new AIS_InteractiveContext (myViewer);
~~~~~

You are now able to display interactive objects such as an *AIS_Shape*.

~~~~~
TopoDS_Shape aShape = BRepAPI_MakeBox (10, 20, 30).Solid();
Handle(AIS_Shape) anAISShape = new AIS_Shape(aShape);
myAISContext->Display (anAISShape);
~~~~~

@subsection occt_visu_4_5_5 Create your own interactive object

Follow the procedure below to compute the presentable object:

1. Build a presentable object inheriting from *AIS_InteractiveObject* (refer to the Chapter on @ref occt_visu_2_1 "Presentable Objects").
2. Reuse the *Prs3d_Presentation* provided as an argument of the compute methods.

@note There are two compute methods: one for a standard representation, and the other for a degenerated representation, i.e. in hidden line removal and wireframe modes.


Let us look at the example of compute methods

~~~~~
Void
myPresentableObject::Compute
  (const Handle(PrsMgr_PresentationManager3d)& thePrsManager,
   const Handle(Prs3d_Presentation)& thePrs,
   const Standard_Integer theMode)
(
  //...
)

void
myPresentableObject::Compute (const Handle(Prs3d_Projector)& ,
                              const Handle(Prs3d_Presentation)& thePrs)
(
  //...
)
~~~~~

@subsection occt_visu_4_5_6 Create primitives in the interactive object

Get the group used in *Prs3d_Presentation*.

~~~~~
Handle(Graphic3d_Group) aGroup = Prs3d_Root::CurrentGroup (thePrs);
~~~~~

Update the group attributes.

~~~~~
aGroup->SetPrimitivesAspect (anAspectBlue);
~~~~~

Create two triangles in *aGroup*.

~~~~~
Standard_Integer aNbTria = 2;
Handle(Graphic3d_ArrayOfTriangles) aTriangles = new Graphic3d_ArrayOfTriangles (3 * aNbTria, 0, Standard_True);
Standard_Integer anIndex;
for (anIndex = 1; anIndex <= aNbTria; nt++)
{
  aTriangles->AddVertex (anIndex * 5.,      0., 0., 1., 1., 1.);
  aTriangles->AddVertex (anIndex * 5 + 5,   0., 0., 1., 1., 1.);
  aTriangles->AddVertex (anIndex * 5 + 2.5, 5., 0., 1., 1., 1.);
}
aGroup->BeginPrimitives();
aGroup->AddPrimitiveArray (aTriangles);
aGroup->EndPrimitives();
~~~~~

The methods *BeginPrimitives()* and *EndPrimitives()* are used  when creating a set of various primitives in the same group.
Use the polyline function to create a boundary box for the *thePrs* structure in group *aGroup*.

~~~~~
Standard_Real Xm, Ym, Zm, XM, YM, ZM;
thePrs->MinMaxValues (Xm, Ym, Zm, XM, YM, ZM);

Handle(Graphic3d_ArrayOfPolylines) aPolylines = new Graphic3d_ArrayOfPolylines (16, 4);
aPolylines->AddBound (4);
aPolylines->AddVertex (Xm,  Ym, Zm);
aPolylines->AddVertex (Xm,  Ym, ZM);
aPolylines->AddVertex (Xm,  YM, ZM);
aPolylines->AddVertex (Xm,  YM, Zm);
aPolylines->AddBound (4);
aPolylines->AddVertex (Xm,  Ym, Zm);
aPolylines->AddVertex (XM,  Ym, Zm);
aPolylines->AddVertex (XM,  Ym, ZM);
aPolylines->AddVertex (XM,  YM, ZM);
aPolylines->AddBound (4);
aPolylines->AddVertex (XM,  YM, Zm);
aPolylines->AddVertex (XM,  Ym, Zm);
aPolylines->AddVertex (XM,  YM, Zm);
aPolylines->AddVertex (Xm,  YM, Zm);
aPolylines->AddBound (4);
aPolylines->AddVertex (Xm,  YM, ZM);
aPolylines->AddVertex (XM,  YM, ZM);
aPolylines->AddVertex (XM,  Ym, ZM);
aPolylines->AddVertex (Xm,  Ym, ZM);

aGroup->BeginPrimitives();
aGroup->AddPrimitiveArray(aPolylines);
aGroup->EndPrimitives();
~~~~~

Create text and markers in group *aGroup*.

~~~~~
static char* texte[3] =
{
  "Application title",
  "My company",
  "My company address."
};
Handle(Graphic3d_ArrayOfPoints) aPtsArr = new Graphic3d_ArrayOfPoints (2, 1);
aPtsArr->AddVertex (-40.0, -40.0, -40.0);
aPtsArr->AddVertex (40.0, 40.0, 40.0);
aGroup->BeginPrimitives();
aGroup->AddPrimitiveArray (aPtsArr);
aGroup->EndPrimitives();

Graphic3d_Vertex aMarker (0.0, 0.0, 0.0);
for (i=0; i <= 2; i++)
{
  aMarker.SetCoord (-(Standard_Real )i * 4 + 30,
                     (Standard_Real )i * 4,
                    -(Standard_Real )i * 4);
  aGroup->Text (texte[i], Marker, 20.);
}

~~~~~


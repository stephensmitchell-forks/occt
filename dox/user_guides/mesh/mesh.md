 Mesh Component  {#occt_user_guides__mesh}
============================

@tableofcontents

@section occt_mesh_1 Mesh presentations


In addition to support of exact geometrical representation of 3D objects Open CASCADE Technology provides functionality to work with tessellated  representations of objects in form of meshes.

Open CASCADE Technology mesh functionality provides:
- data structures to store surface mesh data associated to shapes, and some basic algorithms to handle these data
- data structures and algorithms to build surface triangular mesh from *BRep* objects (shapes).
- tools to extend 3D visualization capabilities of Open CASCADE Technology with displaying meshes along with associated pre- and post-processor data.

Open CASCADE Technology includes two mesh converters:
- VRML converter translates Open CASCADE shapes to VRML 1.0 files (Virtual Reality Modeling Language). Open CASCADE shapes may be translated in two representations: shaded or wireframe. A shaded representation present shapes as sets of triangles computed by a mesh algorithm while a wireframe representation present shapes as sets of curves.
- STL converter translates Open CASCADE shapes to STL files. STL (STtereoLithography) format is widely used for rapid prototyping.

Open CASCADE SAS also offers Advanced Mesh Products:
- <a href="http://www.opencascade.com/content/mesh-framework">Open CASCADE Mesh Framework (OMF)</a>
- <a href="http://www.opencascade.com/content/express-mesh">Express Mesh</a>

Besides, we can efficiently help you in the fields of surface and volume meshing algorithms, mesh optimization algorithms etc. If you require a qualified advice about meshing algorithms, do not hesitate to benefit from the expertise of our team in that domain.

The projects dealing with numerical simulation can benefit from using SALOME - an Open Source Framework for CAE with CAD data interfaces, generic Pre- and Post- F.E. processors and API for integrating F.E. solvers.

Learn more about SALOME platform on http://www.salome-platform.org

@section occt_mesh_2 Meshing algorithm

The algorithm of shape triangulation is provided by the functionality of *BRepMesh_IncrementalMesh* class, which adds a triangulation of the shape to its topological data structure. This triangulation is used to visualize the shape in shaded mode.

~~~~~
const Standard_Real aRadius = 10.0; 
const Standard_Real aHeight = 25.0; 
BRepBuilderAPI_MakeCylinder aCylinder(aRadius, aHeight); 
TopoDS_Shape aShape = aCylinder.Shape();
 
const Standard_Real aLinearDeflection   = 0.01;
const Standard_Real anAngularDeflection = 0.5;

BRepMesh_IncrementalMesh aMesh(aShape, aLinearDeflection, Standard_False, anAngularDeflection);
~~~~~

The default meshing algorithm *BRepMesh_IncrementalMesh* has two major options to define triangulation – linear and angular deflections. 

At the first step all edges from a face are discretized according to the specified parameters. 

At the second step, the faces are tessellated. Linear deflection limits the distance between a curve and its tessellation, whereas angular deflection limits the angle between subsequent segments in a polyline.

@figure{/user_guides/modeling_algos/images/mesh_image056.png, "Deflection parameters of BRepMesh_IncrementalMesh algorithm"}

Linear deflection limits the distance between triangles and the face interior.

@figure{/user_guides/modeling_algos/images/mesh_image057.png, "Linear deflection"}

Note that if a given value of linear deflection is less than shape tolerance then the algorithm will skip this value and will take into account the shape tolerance.

The application should provide deflection parameters to compute a satisfactory mesh. Angular deflection is relatively simple and allows using a default value (12-20 degrees). Linear deflection has an absolute meaning and the application should provide the correct value for its models. Giving small values may result in a too huge mesh (consuming a lot of memory, which results in a  long computation time and slow rendering) while big values result in an ugly mesh.

For an application working in dimensions known in advance it can be reasonable to use the absolute linear deflection for all models. This provides meshes according to metrics and precision used in the application (for example, it it is known that the model will be stored in meters, 0.004 m is enough for most tasks).

However, an application that imports models created in other applications may not use the same deflection for all models. Note that actually this is an abnormal situation and this application is probably just a viewer for CAD models with  dimensions varying by an order of magnitude. This problem can be solved by introducing the concept of a relative linear deflection with some  LOD (level of detail). The level of detail is a scale factor for absolute deflection, which is applied to model dimensions.

Meshing covers a shape with a triangular mesh. Other than hidden line removal, you can use meshing to transfer the shape to another tool: a manufacturing tool, a shading algorithm, a finite element algorithm, or a collision algorithm. 

You can obtain information on the shape by first exploring it. To access triangulation of a face in the shape later, use *BRepTool::Triangulation*. To access a polygon, which is the approximation of an edge of the face, use *BRepTool::PolygonOnTriangulation*.

@figure{/user_guides/xde/images/646_xde_11_400.png,"Shape imported using XDE"}


@section occt_mesh_3 Mesh Visualization Services

<i>MeshVS</i> (Mesh Visualization Service) component extends 3D visualization capabilities of Open CASCADE Technology. It provides flexible means of displaying meshes along with associated pre- and post-processor data.

From a developer's point of view, it is easy to integrate the *MeshVS* component into any mesh-related application with the following guidelines:

* Derive a data source class from the *MeshVS_DataSource* class.
* Re-implement its virtual methods, so as to give the <i>MeshVS</i> component access to the application data model. This is the most important part of the job, since visualization performance is affected by performance of data retrieval methods of your data source class.
* Create an instance of <i>MeshVS_Mesh</i> class.
* Create an instance of your data source class and pass it to a <i>MeshVS_Mesh</i> object through the <i>SetDataSource()</i> method.
* Create one or several objects of <i>MeshVS_PrsBuilder</i>-derived classes (standard, included in the <i>MeshVS</i> package, or your custom ones).
* Each <i>PrsBuilder</i> is responsible for drawing a <i> MeshVS_Mesh</i> presentation in a certain display mode(s) specified as a <i>PrsBuilder</i> constructor's argument. Display mode is treated by <i>MeshVS</i> classes as a combination of bit flags (two least significant bits are used to encode standard display modes: wireframe, shading and shrink).
* Pass these objects to the <i>MeshVS_Mesh::AddBuilder()</i> method. <i>MeshVS_Mesh</i> takes advantage of improved selection highlighting mechanism: it highlights its selected entities itself, with the help of so called "highlighter" object. You can set one of <i>PrsBuilder</i> objects to act as a highlighter with the help of a corresponding argument of the <i>AddBuilder()</i> method.

Visual attributes of the <i>MeshVS_Mesh</i> object (such as shading color, shrink coefficient and so on)  are controlled through <i>MeshVS_Drawer</i> object. It maintains a map "Attribute ID --> attribute value" and can be easily extended with any number of custom attributes.

In all other respects, <i>MeshVS_Mesh</i> is very similar to any other class derived from <i>AIS_InteractiveObject</i> and it should be used accordingly (refer to the description of <i>AIS package</i> in the documentation).

@section occt_mesh_4 Shape Translation 

@subsection occt_mesh_4_1 STL Format

OCCT includes a module for translating OCCT shapes to STL (Stereo-lithography) format. 
STL is a format designed for rapid prototyping. 
It is intended to send geometric data (volumic) to stereo-lithography machines, 
which can read and interpret such data. These machines can transform a volumic model 
to a physical prototype made of plastic, by using laser to coagulate material, 
which corresponds to the volume, and set free the material around. 
STL defines these surfaces by triangles. 
Thus, no machining is required to switch from a virtual model to a physical one.

Since STL files can only include solids described by their mesh structures, 
OCCT shapes, which are intended to be written, must be solids, 
components of solids or closed shells with a correct orientation.

When translating shapes to STL format, remember that all references 
to shapes mean references to OCCT shapes unless otherwise explicitly defined. 
In addition, sets of faces or unclosed shells may also be translated but visualization in foreign viewers may be incorrect.

@subsection occt_mesh_4_2 VRML Format

The Virtual Reality Modeling Language (VRML) is a language for describing multi-participant interactive simulations, virtual worlds networked via the Internet. VRML is a format designed for animated visualization of solids.
OCCT includes a module for translating OCCT shapes to VRML (Virtual Reality Modeling Language). 
OCCT shapes may be translated in two representations (states): shaded or wire-frame. 
Since shaded VRML format files include only solids described by their mesh structures, the OCCT shapes intended to be written must be solids, components of solids or closed shells with a correct orientation.

Please, note:

  * all references to shapes indicate OCCT shapes unless otherwise explicitly stated;
  * sets of faces or unclosed shells may also be translated to shaded VRML format but visualization with foreign viewers may be incorrect.
  
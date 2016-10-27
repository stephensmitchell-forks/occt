Meshing {#user_guides__modeling_algos_meshing}
=======

@tableofcontents

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

## Meshing algorithm

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

The default meshing algorithm *BRepMesh_IncrementalMesh* has two major options to define triangulation -- linear and angular deflections. 

At the first step all edges from a face are discretized according to the specified parameters. 

At the second step, the faces are tessellated. Linear deflection limits the distance between a curve and its tessellation, whereas angular deflection limits the angle between subsequent segments in a polyline.

@figure{/03_user_guides/modeling_algos/images/modeling_algos_image056.png, "Deflection parameters of BRepMesh_IncrementalMesh algorithm"}

Linear deflection limits the distance between triangles and the face interior.

@figure{/03_user_guides/modeling_algos/images/modeling_algos_image057.png, "Linear deflection"}

Note that if a given value of linear deflection is less than shape tolerance then the algorithm will skip this value and will take into account the shape tolerance.

The application should provide deflection parameters to compute a satisfactory mesh. Angular deflection is relatively simple and allows using a default value (12-20 degrees). Linear deflection has an absolute meaning and the application should provide the correct value for its models. Giving small values may result in a too huge mesh (consuming a lot of memory, which results in a  long computation time and slow rendering) while big values result in an ugly mesh.

For an application working in dimensions known in advance it can be reasonable to use the absolute linear deflection for all models. This provides meshes according to metrics and precision used in the application (for example, it it is known that the model will be stored in meters, 0.004 m is enough for most tasks).

However, an application that imports models created in other applications may not use the same deflection for all models. Note that actually this is an abnormal situation and this application is probably just a viewer for CAD models with  dimensions varying by an order of magnitude. This problem can be solved by introducing the concept of a relative linear deflection with some  LOD (level of detail). The level of detail is a scale factor for absolute deflection, which is applied to model dimensions.

Meshing covers a shape with a triangular mesh. Other than hidden line removal, you can use meshing to transfer the shape to another tool: a manufacturing tool, a shading algorithm, a finite element algorithm, or a collision algorithm. 

You can obtain information on the shape by first exploring it. To access triangulation of a face in the shape later, use *BRepTool::Triangulation*. To access a polygon, which is the approximation of an edge of the face, use *BRepTool::PolygonOnTriangulation*.
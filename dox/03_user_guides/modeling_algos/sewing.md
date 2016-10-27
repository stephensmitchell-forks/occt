Sewing {#user_guides__modeling_algos_sewing}
======

@tableofcontents

Sewing allows creation of connected topology (shells and wires) from a set of separate topological elements (faces and edges). For example, Sewing can be used to create of shell from a compound of separate faces. 

@image html /03_user_guides/modeling_algos/images/modeling_algos_image045.png "Shapes with partially shared edges"
@image latex /03_user_guides/modeling_algos/images/modeling_algos_image045.png "Shapes with partially shared edges"

It is important to distinguish between sewing and other procedures, which modify the geometry, such as filling holes or gaps, gluing, bending curves and surfaces, etc.

Sewing does not change geometrical representation of the shapes. Sewing applies to topological elements (faces, edges) which are not connected but can be connected because they are geometrically coincident : it adds the information about topological connectivity. Already connected elements are left untouched in case of manifold sewing.

Let us define several terms:
* **Floating edges** do not belong to any face;
* **Free boundaries** belong to one face only;
* **Shared edges** belong to several faces, (i.e. two faces in a manifold topology).
* **Sewn faces** should have edges shared with each other.
* **Sewn edges** should have vertices shared with each other.

@section occt_modalg_8_2 Sewing Algorithm

The sewing algorithm is one of the basic algorithms used for shape processing, therefore its quality is very important.

Sewing algorithm is implemented in the class *BRepBuilder_Sewing*. This class provides the following methods: 
* loading initial data for global or local sewing; 
* setting customization parameters, such as special operation modes, tolerances and output results;
* applying analysis methods that can be used to obtain connectivity data required by external algorithms;
* sewing of the loaded shapes. 

Sewing supports working mode with big value tolerance. It is not necessary to repeat sewing step by step while smoothly increasing tolerance.

It is also possible to sew edges to wire and to sew locally separate faces and edges from a shape.

The Sewing algorithm can be subdivided into several independent stages, some of which can be turned on or off using Boolean or other flags. 

In brief, the algorithm should find a set of merge candidates for each free boundary, filter them according to certain criteria, and finally merge the found candidates and  build the resulting sewn shape.

Each stage of the algorithm or the whole algorithm can be adjusted with the following parameters: 
* **Working tolerance** defines the maximal distance between topological elements which can be sewn. It is not ultimate that such elements will  be actually sewn as many other criteria are applied to make the final decision.
* **Minimal tolerance** defines the size of the smallest element (edge) in the resulting shape. It is declared that no edges with size less than this value are created after sewing. If encountered, such topology becomes degenerated.
* **Non-manifold mode** enables sewing of non-manifold topology. 

## Example

To connect a set of *n* contiguous but independent faces, do the following: 

~~~~~
    BRepBuilderAPI_Sewing Sew;
    Sew.Add(Face1); 
    Sew.Add(Face2); 
    ...
    Sew.Add(Facen); 
    Sew.Perform();
    TopoDS_Shape result= Sew.SewedShape();
~~~~~

If all faces have been sewn correctly, the result is a shell. Otherwise, it is a compound. After a successful sewing operation all faces have a coherent orientation.

@section occt_modalg_8_3 Tolerance Management

To produce a closed shell, Sewing allows specifying the value of working tolerance, exceeding the size of small faces belonging to the shape.

However, if we produce an open shell, it is possible to get incorrect sewing results if the value of working tolerance is too large (i.e. it exceeds the size of faces lying on an open boundary).

The following recommendations can be proposed for tuning-up the sewing process:
- Use as small working tolerance as possible. This will reduce the sewing time and, consequently, the number of incorrectly sewn edges for shells with free boundaries.
- Use as large minimal tolerance as possible. This will reduce the number of small geometry in the shape, both original and appearing after cutting.
- If it is expected to obtain a shell with holes (free boundaries) as a result of sewing, the working tolerance should be set to a value not greater than the size of the smallest element (edge) or smallest distance between elements of such free boundary. Otherwise the free boundary may be sewn only partially.
- It should  be mentioned that the Sewing algorithm is unable to understand which small (less than working tolerance) free boundary should be kept and which should be sewn.

@section occt_modalg_8_4 Manifold and Non-manifold Sewing

To create one or several shells from a set of faces, sewing merges edges, which belong to different faces or one closed face. 

Face sewing supports manifold and non manifold modes. Manifold mode can produce only a manifold shell. Sewing should be used in the non manifold mode to create non manifold shells.

Manifold sewing of faces merges only two nearest edges belonging to different faces or one closed face with each other. Non manifold sewing of faces merges all edges at a distance less than the specified tolerance.

For a complex topology it is advisable to apply first the manifold sewing and then the non manifold sewing a minimum possible working tolerance. However, this is not necessary for a easy topology. 

Giving a large tolerance value to non manifold sewing will cause a lot of incorrectness since all nearby geometry will be sewn.

@section occt_modalg_8_5 Local Sewing

If a shape still has some non-sewn faces or edges after sewing, it is possible to use local sewing with a greater tolerance.

Local sewing is especially good for open shells. It allows sewing an unwanted hole in one part of the shape and keeping a required hole, which is smaller than the working tolerance specified for the local sewing in the other part of the shape. Local sewing is much faster than sewing on the whole shape.

All preexisting connections of the whole shape are kept after local sewing. 

For example, if you want to sew two open shells having coincided free edges using local sewing, it is necessary to create a compound from two shells then load the full compound using method *BRepBuilderAPI_Sewing::Load()*. After that it is necessary to add local sub-shapes, which should be sewn using method *BRepBuilderAPI_Sewing::Add()*. The result of sewing can be obtained using method *BRepBuilderAPI_Sewing::SewedShape()*.

See the example:

~~~~

//initial sewn shapes
TopoDS_Shape aS1, aS2;  // these shapes are expected to be well sewn shells
TopoDS_Shape aComp;
BRep_Builder aB;
aB.MakeCompound(aComp);
aB.Add(aComp, aS1);
aB.Add(aComp, aS2);
................................
aSewing.Load(aComp);

//sub shapes which should be locally sewed
aSewing.Add(aF1);
aSewing.Add(aF2);
//performing sewing
aSewing.Perform();
//result shape
TopoDS_Shape aRes = aSewing.SewedShape();

~~~~

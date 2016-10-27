Hidden Line Removal {#user_guides__modeling_algos_hidden_line_removal}
===================

@tableofcontents

To provide the  precision required in industrial design, drawings need to offer the possibility  of removing lines, which are hidden in a given projection. 

For this the Hidden Line Removal component provides two algorithms: *HLRBRep_Algo*  and *HLRBRep_PolyAlgo*. 

These algorithms are  based on the principle of comparing each edge of the shape to be visualized  with each of its faces, and calculating the visible and the hidden parts of  each edge. Note that these are not the  algorithms used in generating  shading, which calculate the visible and hidden parts of each face in a shape  to be visualized by comparing each face in the shape with every other face in  the same shape. 
These algorithms operate  on a shape and remove or indicate edges hidden by faces. For a given  projection, they calculate a set of lines characteristic of the object being  represented. They are also used in conjunction with extraction utilities, which  reconstruct a new, simplified shape from a selection of the results of the  calculation. This new shape is made up of edges, which represent the shape  visualized in the projection. 

*HLRBRep_Algo* allows working with the shape itself, whereas *HLRBRep_PolyAlgo* works with a polyhedral simplification of the shape. When you use *HLRBRep_Algo*, you obtain an exact result, whereas, when you use *HLRBRep_PolyAlgo*, you reduce the computation time, but obtain polygonal segments.

No smoothing algorithm  is provided. Consequently, a polyhedron will be treated as such and the  algorithms will give the results in  form of line segments conforming to the  mathematical definition of the polyhedron. This is always the case with *HLRBRep_PolyAlgo*. 

*HLRBRep_Algo* and *HLRBRep_PolyAlgo* can deal with any kind of object, for example, assemblies of  volumes, surfaces, and lines, as long as there are no unfinished  objects or points within it. 

However, there some restrictions in HLR use:
  * Points are not processed;
  * Z-clipping planes are not used;
  * Infinite faces or lines are not processed.

  
@image html /03_user_guides/modeling_algos/images/modeling_algos_image052.png "Sharp, smooth and sewn edges  in a simple screw shape"
@image latex /03_user_guides/modeling_algos/images/modeling_algos_image052.png "Sharp, smooth and sewn edges  in a simple screw shape"

@image html /03_user_guides/modeling_algos/images/modeling_algos_image053.png "Outline edges  and isoparameters in the same shape"
@image latex /03_user_guides/modeling_algos/images/modeling_algos_image053.png "Outline edges  and isoparameters in the same shape"

@image html /03_user_guides/modeling_algos/images/modeling_algos_image054.png "A simple screw shape seen with shading"
@image latex /03_user_guides/modeling_algos/images/modeling_algos_image054.png "A simple screw shape seen with shading"

@image html /03_user_guides/modeling_algos/images/modeling_algos_image055.png "An extraction  showing hidden sharp edges"
@image latex /03_user_guides/modeling_algos/images/modeling_algos_image055.png "An extraction  showing hidden sharp edges"

The following services are related to Hidden Lines Removal : 

## Loading Shapes

To pass a *TopoDS_Shape* to an *HLRBRep_Algo*  object, use *HLRBRep_Algo::Add*. With an *HLRBRep_PolyAlgo* object, use *HLRBRep_PolyAlgo::Load*. If you wish to add several shapes, use Add or Load as often  as necessary. 

## Setting view parameters

*HLRBRep_Algo::Projector* and *HLRBRep_PolyAlgo::Projector* set a projector object which defines the  parameters of the view. This object is an *HLRAlgo_Projector*. 

## Computing the projections

*HLRBRep_PolyAlgo::Update* launches the calculation of outlines of the shape visualized by the *HLRBRep_PolyAlgo* framework. 

In the case of *HLRBRep_Algo*, use *HLRBRep_Algo::Update*. With this algorithm, you must also call the method *HLRBRep_Algo::Hide* to calculate visible and hidden lines of the  shape to be visualized. With an *HLRBRep_PolyAlgo* object, visible and hidden lines are computed by *HLRBRep_PolyHLRToShape*. 

## Extracting edges

The classes *HLRBRep_HLRToShape* and *HLRBRep_PolyHLRToShape* present a range of extraction filters for an *HLRBRep_Algo object* and an *HLRBRep_PolyAlgo* object, respectively. They highlight the type of  edge from the results calculated by the algorithm on a shape. With  both extraction classes, you can highlight the following types of output: 
  * visible/hidden sharp edges;
  * visible/hidden smooth edges;
  * visible/hidden sewn edges;
  * visible/hidden outline edges. 

To perform extraction on an *HLRBRep_PolyHLRToShape* object, use *HLRBRep_PolyHLRToShape::Update*  function. 

For an *HLRBRep_HLRToShape* object built from an *HLRBRepAlgo* object you can also highlight: 
  * visible isoparameters and
  * hidden isoparameters.

@section occt_modalg_10_1 Examples

## HLRBRep_Algo

~~~~~
// Build The algorithm object 
myAlgo = new HLRBRep_Algo(); 

// Add Shapes into the algorithm 
TopTools_ListIteratorOfListOfShape anIterator(myListOfShape); 
for (;anIterator.More();anIterator.Next()) 
myAlgo-Add(anIterator.Value(),myNbIsos); 

// Set The Projector (myProjector is a 
HLRAlgo_Projector) 
myAlgo-Projector(myProjector); 

// Build HLR 
myAlgo->Update(); 

// Set The Edge Status 
myAlgo->Hide(); 

// Build the extraction object : 
HLRBRep_HLRToShape aHLRToShape(myAlgo); 

// extract the results : 
TopoDS_Shape VCompound           = aHLRToShape.VCompound(); 
TopoDS_Shape Rg1LineVCompound                            = 
aHLRToShape.Rg1LineVCompound(); 
TopoDS_Shape RgNLineVCompound                            = 
aHLRToShape.RgNLineVCompound(); 
TopoDS_Shape OutLineVCompound                            = 
aHLRToShape.OutLineVCompound(); 
TopoDS_Shape IsoLineVCompound                            = 
aHLRToShape.IsoLineVCompound(); 
TopoDS_Shape HCompound           = aHLRToShape.HCompound(); 
TopoDS_Shape Rg1LineHCompound                            = 
aHLRToShape.Rg1LineHCompound(); 
TopoDS_Shape RgNLineHCompound                            = 
aHLRToShape.RgNLineHCompound(); 
TopoDS_Shape OutLineHCompound                            = 
aHLRToShape.OutLineHCompound(); 
TopoDS_Shape IsoLineHCompound                            = 
aHLRToShape.IsoLineHCompound(); 
~~~~~

## HLRBRep_PolyAlgo


~~~~~

// Build The algorithm object 
myPolyAlgo = new HLRBRep_PolyAlgo(); 

// Add Shapes into the algorithm 
TopTools_ListIteratorOfListOfShape 
anIterator(myListOfShape); 
for (;anIterator.More();anIterator.Next()) 
myPolyAlgo-Load(anIterator.Value()); 

// Set The Projector (myProjector is a 
HLRAlgo_Projector) 
myPolyAlgo->Projector(myProjector); 

// Build HLR 
myPolyAlgo->Update(); 

// Build the extraction object : 
HLRBRep_PolyHLRToShape aPolyHLRToShape; 
aPolyHLRToShape.Update(myPolyAlgo); 

// extract the results : 
TopoDS_Shape VCompound = 
aPolyHLRToShape.VCompound(); 
TopoDS_Shape Rg1LineVCompound = 
aPolyHLRToShape.Rg1LineVCompound(); 
TopoDS_Shape RgNLineVCompound = 
aPolyHLRToShape.RgNLineVCompound(); 
TopoDS_Shape OutLineVCompound = 
aPolyHLRToShape.OutLineVCompound(); 
TopoDS_Shape HCompound = 
aPolyHLRToShape.HCompound(); 
TopoDS_Shape Rg1LineHCompound = 
aPolyHLRToShape.Rg1LineHCompound(); 
TopoDS_Shape RgNLineHCompound = 
aPolyHLRToShape.RgNLineHCompound(); 
TopoDS_Shape OutLineHCompound = 
aPolyHLRToShape.OutLineHCompound(); 
~~~~~


General Fuse {#user_guides__draw_commands_fuse}
============

@tableofcontents

This chapter describes existing commands of Open CASCADE Draw that are used for debugging of General Fuse Algorithm (GFA). 
It is also applicable for Boolean Operations Algorithm (BOA) and Partition Algorithm (PA) because these algorithms are subclasses of GFA. 

See @ref debug__boolean_operations "Boolean operations" implementation guide for the description of these algorithms.

@section occt_draw_20_1 Definitions

The following terms and definitions are used in this document:
* **Objects** -- list of shapes that are arguments of the algorithm.
* **Tools** -- list of shapes that are arguments of the algorithm. Difference between Objects and Tools is defined by specific requirements of the operations (Boolean Operations, Partition Operation).
* **DS** -- internal data structure used by the algorithm (*BOPDS_DS* object).
* **PaveFiller** -- intersection part of the algorithm (*BOPAlgo_PaveFiller* object).
* **Builder** -- builder part of the algorithm (*BOPAlgo_Builder* object).
* **IDS Index** -- the index of the vector *myLines*.

@section occt_draw_20_2 General commands

* **bclearobjects** -- clears the list of Objects;	
* **bcleartools**	-- clears the list of Tools;	
* **baddobjects** *S1 S2...Sn*	-- adds shapes *S1, S2, ... Sn* as Objects;	
* **baddtools** *S1 S2...Sn* -- adds shapes *S1, S2, ... Sn* as Tools;
* **bfillds** -- performs the Intersection Part of the Algorithm;	
* **bbuild** *r* -- performs the Building Part of the Algorithm; *r* is the resulting shape.

@section occt_draw_20_3 Commands for Intersection Part

All commands listed below  are available when the Intersection Part of the algorithm is done (i.e. after the command *bfillds*).

@subsection occt_draw_20_3_1 bopds
	
Syntax: 
~~~~
bopds -v [e, f]	
~~~~

Displays:
* all BRep shapes of arguments that are in the DS [default];
* <i>-v</i> : only vertices of arguments that are in the DS;
* <i>-e</i> : only edges of arguments that are in the DS;
* <i>-f</i> : only faces of arguments that are in the DS.

@subsection occt_draw_20_3_2 bopdsdump

Prints contents of the DS. 

Example:

~~~~
 Draw[28]> bopdsdump
 *** DS ***
 Ranges:2			number of ranges
 range: 0 33		indices for range 1
 range: 34 67		indices for range 2
 Shapes:68		total number of source shapes
 0 : SOLID { 1 }
 1 : SHELL { 2 12 22 26 30 32 }
 2 : FACE { 4 5 6 7 8 9 10 11 }
 3 : WIRE { 4 7 9 11 }
 4 : EDGE { 5 6 }
 5 : VERTEX { }
 6 : VERTEX { }
 7 : EDGE { 8 5 }
 8 : VERTEX { }
~~~~

@code 0 : SOLID { 1 } @endcode has the following meaning:
* *0* -- index in the DS;
* *SOLID* -- type of the shape;
* <i>{ 1 }</i> -- a DS index of the successors.

@subsection occt_draw_20_3_3 bopindex

Syntax:
~~~~
bopindex S
~~~~
Prints DS index of shape *S*.

@subsection occt_draw_20_3_4 bopiterator

Syntax:
~~~~~
bopiterator [t1 t2]
~~~~~

Prints pairs of DS indices of source shapes that are intersected in terms of bounding boxes.

<i>[t1 t2]</i> are types of the shapes:
* *7* -- vertex;
* *6* -- edge;
* *4* -- face.

Example:
~~~~
 Draw[104]> bopiterator 6 4
 EF: ( z58 z12 )
 EF: ( z17 z56 )
 EF: ( z19 z64 )
 EF: ( z45 z26 )
 EF: ( z29 z36 )
 EF: ( z38 z32 )
~~~~

* *bopiterator 6 4* prints pairs of indices for types: edge/face;
* *z58 z12* -- DS indices of intersecting edge and face.


@subsection occt_draw_20_3_5 bopinterf

Syntax: 
~~~~
bopinterf t
~~~~

Prints contents of *myInterfTB* for the type of interference *t*:
* *t=0* : vertex/vertex;
* *t=1* : vertex/edge;
* *t=2* : edge/edge;
* *t=3* : vertex/face;
* *t=4* : edge/face.

Example:
~~~~
 Draw[108]> bopinterf 4
 EF: (58, 12, 68), (17, 56, 69), (19, 64, 70), (45, 26, 71), (29, 36, 72), (38, 32, 73), 6 EF found.
~~~~

Here, record <i>(58, 12, 68)</i> means:
* *58* -- a DS index of the edge;
* *12* -- a DS index of the face;
* *68* -- a DS index of the new vertex.

@subsection occt_draw_20_3_6 bopsp	

Displays split edges. 

Example:
~~~~
 Draw[33]> bopsp
 edge 58 : z58_74 z58_75
 edge 17 : z17_76 z17_77
 edge 19 : z19_78 z19_79
 edge 45 : z45_80 z45_81
 edge 29 : z29_82 z29_83
 edge 38 : z38_84 z38_85
~~~~

* *edge 58* -- 58 is a DS index of the original edge.
* *z58_74 z58_75* -- split edges, where 74, 75 are DS indices of the split edges.

@subsection occt_draw_20_3_7 bopcb

Syntax:
~~~~
bopcb [nE]
~~~~

Prints Common Blocks for:
* all source edges (by default);
* the source edge with the specified index *nE*.

Example:
~~~~
 Draw[43]> bopcb 17
 -- CB:
 PB:{ E:71 orE:17 Pave1: { 68 3.000 } Pave2: { 18 10.000 } }
 Faces: 36
~~~~

This command dumps common blocks for the source edge with index 17. 
* *PB* -- information about the Pave Block;
	* *71* -- a DS index of the split edge
	* *17* -- a DS index of the original edge
* <i>Pave1 : { 68 3.000 }</i> -- information about the Pave:
	* *68* -- a DS index of the vertex of the pave
	* *3.000* -- a parameter of vertex 68 on edge 17
* *Faces: 36* -- 36 is a DS index of the face the common block belongs to. 


@subsection occt_draw_20_3_8 bopfin

Syntax:

~~~~
bopfin nF	
~~~~
Prints Face Info about IN-parts for the face with DS index *nF*.

Example:
~~~~
 Draw[47]> bopfin 36
 pave blocks In:
 PB:{ E:71 orE:17 Pave1: { 68 3.000 } Pave2: { 18 10.000 } }
 PB:{ E:75 orE:19 Pave1: { 69 3.000 } Pave2: { 18 10.000 } }
 vrts In:
 18
~~~~


* <i>PB:{ E:71 orE:17 Pave1: { 68 3.000 } Pave2: { 18 10.000 } }</i> -- information about the Pave Block; 
* <i>vrts In ... 18 </i> -- a DS index of the vertex IN the face.

@subsection occt_draw_20_3_9 bopfon

Syntax:
~~~~
bopfon nF
~~~~
Print Face Info about ON-parts for the face with DS index *nF*.

Example:
~~~~
 Draw[58]> bopfon 36
 pave blocks On:
 PB:{ E:72 orE:38 Pave1: { 69 0.000 } Pave2: { 68 10.000 } }
 PB:{ E:76 orE:45 Pave1: { 69 0.000 } Pave2: { 71 10.000 } }
 PB:{ E:78 orE:43 Pave1: { 71 0.000 } Pave2: { 70 10.000 } }
 PB:{ E:74 orE:41 Pave1: { 68 0.000 } Pave2: { 70 10.000 } }
 vrts On:
 68 69 70 71
~~~~

* <i>PB:{ E:72 orE:38 Pave1: { 69 0.000 } Pave2: { 68 10.000 } }</i> -- information about the Pave Block; 
* <i>vrts On: ... 68 69 70 71</i> -- DS indices of the vertices ON the face.

@subsection occt_draw_20_3_10 bopwho

Syntax:
~~~~
bopwho nS
~~~~

Prints the information about the shape with DS index *nF*.

Example:
~~~~
 Draw[116]> bopwho 5
 rank: 0
~~~~

* *rank: 0* -- means that shape 5 results from the Argument with index 0.

Example:
~~~~
 Draw[118]> bopwho 68
 the shape is new
 EF: (58, 12),
 FF curves: (12, 56),
 FF curves: (12, 64),
~~~~

This means that shape 68 is a result of the following interferences:
* *EF: (58, 12)* -- edge 58 / face 12
* *FF curves: (12, 56)* -- edge from the intersection curve between faces 12 and 56
* *FF curves: (12, 64)* -- edge from the intersection curve between faces 12 and 64

@subsection occt_draw_20_3_11 bopnews

Syntax:
~~~~
bopnews -v [-e]
~~~~

* <i>-v</i> -- displays all new vertices produced during the operation;
* <i>-e</i> -- displays all new edges produced during the operation.

@section occt_draw_20_4	Commands for the Building Part

The commands listed below are available when the Building Part of the algorithm is done (i.e. after the command *bbuild*).

@subsection occt_draw_20_4_1 bopim

Syntax: 

~~~~
bopim S
~~~~
Shows the compound of shapes that are images of shape *S* from the argument.


Performance {#user_guides__draw_commands_performance}
===========

##VDrawSphere
 
~~~~~
vdrawsphere shapeName Fineness [X=0.0 Y=0.0 Z=0.0] [Radius=100.0] [ToEnableVBO=1] [NumberOfViewerUpdate=1] [ToShowEdges=0] 
~~~~~

Calculates and displays in a given number of steps a sphere with given coordinates, radius and fineness. Returns the 
information about the properties of the sphere, the time and the amount of memory required to build it. 

This command can be used for visualization performance evaluation instead of the outdated Visualization Performance Meter. 

~~~~~
vdrawsphere s 200 1 1 1 500 1 
== Compute Triangulation... 
== NumberOfPoints: 39602 
== NumberOfTriangles: 79200 
== Amount of memory required for PolyTriangulation without Normals: 2 Mb 
== Amount of memory for colors: 0 Mb 
== Amount of memory for PolyConnect: 1 Mb 
== Amount of graphic card memory required: 2 Mb 
== Number of scene redrawings: 1 
== CPU user time: 15.6000999999998950 msec 
== CPU system time: 0.0000000000000000 msec 
== CPU average time of scene redrawing: 15.6000999999998950 msec 
~~~~~
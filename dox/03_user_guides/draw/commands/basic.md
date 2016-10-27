Basic {#user_guides__draw_commands_basic}
======

@tableofcontents

This chapter describes all the commands defined in the basic Draw package. Some are TCL commands, but most of them have been formulated in Draw. These commands are found in all Draw applications. The commands are grouped into four sections: 

  * General commands, which are used for Draw and TCL management.
  * Variable commands, which are used to manage Draw variables such as storing and dumping.
  * Graphic commands, which are used to manage the graphic system, and so pertain to views.
  * Variable display commands, which are used to manage the display of objects within given views.

@note Draw also features a GUI task bar providing an alternative way to give certain general, graphic and display commands 

@section occt_draw_3_1 General commands

This section describes several useful commands:

  * **help** to get information, 
  * **source** to eval a script from a file, 
  * **spy** to capture the commands in a file,
  * **cpulimit** to limit the process cpu time, 
  * **wait** to waste some time, 
  * **chrono** to time commands. 

@subsection occt_draw_3_1_1 help

Syntax:                  

~~~~~
help [command [helpstring group]] 
~~~~~

Provides help or modifies the help information. 

**help** without arguments lists all groups and the commands in each group. 

Specifying the command returns its syntax and in some cases, information on the command, The joker \* is automatically added at the end so that all completing commands are returned as well. 

**Example:** 
~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~{.cpp}
# Gives help on all commands starting with *a* 
~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~


@subsection occt_draw_3_1_2 source

Syntax:

~~~~~
source filename 
~~~~~
Executes a file. 

The **exit** command will terminate the file. 

@subsection occt_draw_3_1_3 spy

Syntax:                  

~~~~~
spy [filename] 
~~~~~

Saves interactive commands in the file. If spying has already been performed, the current file is closed. **spy** without an argument closes the current file and stops spying. If a file already exists, the file is overwritten. Commands are not appended. 

If a command returns an error it is saved with a comment mark. 

The file created by **spy** can be executed with the **source** command. 

**Example:** 
~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~{.cpp}
# all commands will be saved in the file ;session; 
spy session 
# the file ;session; is closed and commands are not saved 
spy 
~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~



@subsection occt_draw_3_1_4 cpulimit

Syntax:                  

~~~~~
cpulimit [nbseconds] 
~~~~~

**cpulimit**limits a process after the number of seconds specified in nbseconds. It is used in tests to avoid infinite loops. **cpulimit** without arguments removes all existing limits. 

**Example:** 
~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~{.cpp}
#limit cpu to one hour 
cpulimit 3600 
~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

@subsection occt_draw_3_1_5 wait

Syntax:
~~~~~
wait [nbseconds] 
~~~~~
Suspends execution for the number of seconds specified in *nbseconds*. The default value is ten (10) seconds. This is a useful command for a slide show. 

~~~~~
# You have ten seconds ... 
wait 
~~~~~

@subsection occt_draw_3_1_6 chrono

Syntax:                  

~~~~~
chrono [ name start/stop/reset/show] 
~~~~~

Without arguments, **chrono** activates Draw chronometers. The elapsed time ,cpu system and cpu user times for each command will be printed. 

With arguments, **chrono** is used to manage activated chronometers. You can perform the following actions with a chronometer. 
  * run the chronometer (start).
  * stop the chronometer (stop).
  * reset the chronometer to 0 (reset).
  * display the current time (show).

**Example:** 
~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~{.cpp}
chrono 
==Chronometers activated. 
ptorus t 20 5 
==Elapsed time: 0 Hours 0 Minutes 0.0318 Seconds 
==CPU user time: 0.01 seconds 
==CPU system time: 0 seconds 
~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

@section occt_draw_3_2  Variable management commands

@subsection occt_draw_3_2_1 isdraw, directory

Syntax:                  
~~~~~
isdraw varname 
directory [pattern] 
~~~~~

**isdraw** tests to see if a variable is a Draw variable. **isdraw** will return 1 if there is a Draw value attached to the variable. 

Use **directory** to return a list of all Draw global variables matching a pattern. 

**Example:** 
~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~{.cpp}
set a 1 
isdraw a 
=== 0 

dset a 1 
isdraw a 
=== 1 

circle c 0 0 1 0 5 
isdraw c 
=== 1 

# to destroy all Draw objects with name containing curve 
foreach var [directory *curve*] {unset $var} 
~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~


@subsection occt_draw_3_2_2 whatis, dump

Syntax:

~~~~~
whatis varname [varname ...] 
dump varname [varname ...] 
~~~~~

**whatis** returns short information about a Draw variable. This is usually the type name. 

**dump** returns a brief type description, the coordinates, and if need be, the parameters of a Draw variable. 

**Example:** 
~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~{.cpp}
circle c 0 0 1 0 5 
whatis c 
c is a 2d curve 

dump c 

***** Dump of c ***** 
Circle 
Center :0, 0 
XAxis :1, 0 
YAxis :-0, 1 
Radius :5 
~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

**Note** The behavior of *whatis* on other variables (not Draw) is not excellent. 


@subsection occt_draw_3_2_3 rename, copy

Syntax:      
~~~~~
rename varname tovarname [varname tovarname ...] 
copy varname tovarname [varname tovarname ...] 
~~~~~

  * **rename** changes the name of a Draw variable. The original variable will no longer exist. Note that the content is not modified. Only the name is changed. 
  * **copy** creates a new variable with a copy of the content of an existing variable. The exact behavior of **copy** is type dependent; in the case of certain topological variables, the content may still be shared. 

**Example:** 
~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~{.cpp}
circle c1 0 0 1 0 5 
rename c1 c2 

# curves are copied, c2 will not be modified 
copy c2 c3 
~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

@subsection occt_draw_3_2_4 datadir, save, restore

Syntax:
~~~~~
datadir [directory] 
save variable [filename] 
restore filename [variablename] 
~~~~~

  * **datadir** without arguments prints the path of the current data directory. 
  * **datadir** with an argument sets the data directory path. \

If the path starts with a dot (.) only the last directory name will be changed in the path. 

  * **save** writes a file in the data directory with the content of a variable. By default the name of the file is the name of the variable. To give a different name use a second argument. 
  * **restore** reads the content of a file in the data directory in a local variable. By default, the name of the variable is the name of the file. To give a different name, use a second argument. 

The exact content of the file is type-dependent. They are usually ASCII files and so, architecture independent. 

**Example:** 
~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~{.cpp}
# note how TCL accesses shell environment variables 
# using $env() 
datadir 
==. 

datadir $env(WBCONTAINER)/data/default 
==/adv_20/BAG/data/default 

box b 10 20 30 
save b theBox 
==/adv_20/BAG/data/default/theBox 

# when TCL does not find a command it tries a shell command 
ls [datadir] 
== theBox 

restore theBox 
== theBox 
~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

@section occt_draw_3_3 User defined commands

*DrawTrSurf* provides commands to create and display a Draw **geometric** variable from a *Geom_Geometry* object and also get a *Geom_Geometry* object from a Draw geometric variable name. 

*DBRep* provides commands to create and display a Draw **topological** variable from a *TopoDS_Shape* object and also get a *TopoDS_Shape* object from a Draw topological variable name. 

@subsection occt_draw_3_3_1 set

#### In *DrawTrSurf* package:

~~~~~
void Set(Standard_CString& Name,const gp_Pnt& G) ; 
void Set(Standard_CString& Name,const gp_Pnt2d& G) ; 
void Set(Standard_CString& Name, 
const Handle(Geom_Geometry)& G) ; 
void Set(Standard_CString& Name, 
const Handle(Geom2d_Curve)& C) ; 
void Set(Standard_CString& Name, 
const Handle(Poly_Triangulation)& T) ; 
void Set(Standard_CString& Name, 
const Handle(Poly_Polygon3D)& P) ; 
void Set(Standard_CString& Name, 
const Handle(Poly_Polygon2D)& P) ; 
~~~~~

#### In *DBRep* package:

~~~~~
void Set(const Standard_CString Name, 
const TopoDS_Shape& S) ; 
~~~~~

Example of *DrawTrSurf*

~~~~~
Handle(Geom2d_Circle) C1 = new Geom2d_Circle 
(gce_MakeCirc2d (gp_Pnt2d(50,0,) 25)); 
DrawTrSurf::Set(char*, C1); 
~~~~~

Example of *DBRep* 

~~~~~
TopoDS_Solid B; 
B = BRepPrimAPI_MakeBox (10,10,10); 
DBRep::Set(char*,B); 
~~~~~

@subsection occt_draw_3_3_2 get

#### In *DrawTrSurf* package:
 
~~~~~
Handle_Geom_Geometry Get(Standard_CString& Name) ; 
~~~~~

#### In *DBRep* package:

~~~~~
TopoDS_Shape Get(Standard_CString& Name, 
const TopAbs_ShapeEnum Typ = TopAbs_SHAPE, 
const Standard_Boolean Complain 
= Standard_True) ; 
~~~~~

Example of *DrawTrSurf*

~~~~~
Standard_Integer MyCommand 
(Draw_Interpretor& theCommands, 
Standard_Integer argc, char** argv) 
{...... 
// Creation of a Geom_Geometry from a Draw geometric 
// name 
Handle (Geom_Geometry) aGeom= DrawTrSurf::Get(argv[1]); 
} 
~~~~~

Example of *DBRep*

~~~~~
Standard_Integer MyCommand 
(Draw_Interpretor& theCommands, 
Standard_Integer argc, char** argv) 
{...... 
// Creation of a TopoDS_Shape from a Draw topological 
// name 
TopoDS_Solid B = DBRep::Get(argv[1]); 
} 
~~~~~


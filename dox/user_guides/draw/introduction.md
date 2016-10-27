Introduction {#user_guides__draw_intro}
============

@tableofcontents

Draw is a command interpreter based on TCL and a graphical system providing a flexible and easy to use means of 
testing and demonstrating the OCCT libraries.

Draw can be used interactively to create, display and modify objects such as curves, surfaces and topological shapes.

@image html /user_guides/draw/images/overview_draw.png
@image latex /user_guides/draw/images/overview_draw.png

Testing commands grouped in toolkits can be loaded at run-time thereby implementing dynamically loaded plug-ins. 
Thus you can work only with the commands that suit your needs adding the commands dynamically without leaving Draw session. 
Learn more about @ref contribution__test_system "OCCT Draw test organization and usage".

You can also add new custom test commands and new types of objects to Draw in order to test or demonstrate a new
 functionality, which you are developing. Learn more about how to extend Draw @ref user_guides__draw_extension "here".

## Running Draw

**On Linux:**

1. If OCCT was built by Code::Blocks  use <i>$CASROOT/draw_cbp.sh</i> file to launch *DRAWEXE* executable;
2. If OCCT was built by Automake    use <i>$CASROOT/draw_amk.sh</i> file to launch *DRAWEXE* executable;

After that  prompt appears in the command window:
> Draw[1]>

To start work load all available plugins typing the following command:   
> Draw[1] pload ALL

**On Windows:**

Launch Draw executable from Open CASCADE Technology\\Test Harness\\Draw Test Harness 
item of the Start\\Programs menu or Use <i>$CASROOT\\draw.bat</i> file to launch *DRAWEXE* executable.

Draw[1]> prompt appears in the command window

To start work load all available plugins typing the following command:
> Draw[1] pload ALL

Now you are ready to start experimenting with Draw using its various tcl @ref draw__commands "commands".

@note
* The DRAWEXE executable is delivered with the installation procedure on Windows platform only.
* To start it, launch DRAWEXE executable from Open CASCADE Technology/Draw Test Harness item of the Start\\Programs menu.

## Experimenting with Draw

**Creating your first geometric objects**

1. In the command window, type *axo* to create an axonometric view
2. Type *box b -10 -10 -10 20 20 20* to create a cube *b* of size 20, parallel to the X Y Z axis and centered on the origin. The cube will be displayed in the axonometric view in wireframe mode.
3. Type *fit* to fill the viewer with the cube
4. Type *pcylinder c 2 30* to create a cylinder *c* of radius 2 and height 30. The cylinder will be displayed in addition to the cube

**Manipulating the view**

1. Type *clear* to erase the view
2. Type *donly c* to display the cylinder only
3. Type *donly b* to display the cube only
4. Type *hlr hlr b* to display the cube in the hidden line removal mode

**Getting Help**

1. Type *help* to see all available commands
2. Type *help \<command_name\>* to find out the arguments for a given command

**Running demonstration files**

How to run the Draw demo scripts please see @ref samples__draw_scripts "here".
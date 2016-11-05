Graphic Commands {#user_guides__draw_commands_graphic}
================

@tableofcontents

Graphic commands are used to manage the Draw graphic system. Draw provides a 2d and a 3d viewer with up to 30 views. Views are numbered and the index of the view is displayed in the window’s title. Objects are displayed in all 2d views or in all 3d views, depending on their type. 2d objects can only be viewed in 2d views while 3d objects -- only in 3d views correspondingly. 

@section graphic_axo Axonometric Viewer
@subsection occt_draw_4_1_1 view, delete
                  
~~~~~
view index type [X Y W H] 
delete [index] 
~~~~~

**view** is the basic view creation command: it creates a new view with the given index. If a view with this index already exits, it is deleted. The view is created with default parameters and X Y W H are the position and dimensions of the window on the screen. Default values are 0, 0, 500, 500. 

As a rule it is far simpler either to use the procedures **axo**, **top**, **left** or to click on the desired view type in the menu under *Views* in the task bar.. 

**delete** deletes a view. If no index is given, all the views are deleted. 

Type selects from the following range: 

  * *AXON* : Axonometric view
  * *PERS* : Perspective view
  * <i>+X+Y</i> : View on both axes (i.e. a top view), other codes are <i>-X+Y</i>, <i>+Y-Z</i>, etc.
  * <i>-2D-</i> : 2d view

The index, the type, the current zoom are displayed in the window title . 


~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~{.cpp}
# this is the content of the mu4 procedure 
proc mu4 {} { 
delete 
view 1 +X+Z 320 20 400 400 
view 2 +X+Y 320 450 400 400 
view 3 +Y+Z 728 20 400 400 
view 4 AXON 728 450 400 400 
} 
~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

See also: **axo, pers, top, bottom, left, right, front, back, mu4, v2d, av2d, smallview** 

@subsection occt_draw_4_1_2  axo, pers, top, ...

      

~~~~~
axo 
pers 
... 
smallview type 
~~~~~

All these commands are procedures used to define standard screen layout. They delete all existing views and create new ones. The layout usually complies with the European convention, i.e. a top view is under a front view. 

  * **axo** creates a large window axonometric view;
  * **pers** creates a large window perspective view;
  * **top**, **bottom**, **left**, **right**, **front**, **back** create a large window axis view;
  * **mu4** creates four small window views: front, left, top and axo.
  * **v2d** creates a large window 2d view.
  * **av2d** creates two small window views, one 2d and one axo
  * **smallview** creates a view at the bottom right of the screen of the given type. 

See also: **view**, **delete** 

@subsection occt_draw_4_1_3 mu, md, 2dmu, 2dmd, zoom, 2dzoom



~~~~~
    mu [index] value 
    2dmu [index] value 
    zoom [index] value 
    wzoom 
~~~~~

* **mu** (magnify up) increases the zoom in one or several views by a factor of 10%. 
* **md** (magnify down) decreases the zoom by the inverse factor. **2dmu** and **2dmd** 
perform the same on one or all 2d views. 
* **zoom** and **2dzoom** set the zoom factor to a value specified by you. The current zoom factor is always displayed in the window’s title bar. Zoom 20 represents a full screen view in a large window; zoom 10, a full screen view in a small one. 
* **wzoom** (window zoom) allows you to select the area you want to zoom in on with the mouse. You will be prompted to give two of the corners of the area that you want to magnify and the rectangle so defined will occupy the window of the view. 


~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~{.cpp}
    # set a zoom of 2.5 
    zoom 2.5 

    # magnify by 10% 
    mu 1 

    # magnify by 20% 
~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
See also: **fit**, **2dfit** 


@subsection occt_draw_4_14 pu, pd, pl, pr, 2dpu, 2dpd, 2dpl, 2dpr

                  

~~~~~
pu [index] 
pd [index] 
~~~~~

The <i>p_</i> commands are used to pan. **pu** and **pd** pan up and down respectively; **pl** and **pr** pan to the left and to the right respectively. Each time the view is displaced by 40 pixels. When no index is given, all views will pan in the direction specified. 
~~~~~
# you have selected one anonometric view
pu
# or
pu 1

# you have selected an mu4 view; the object in the third view will pan up
pu 3
~~~~~
See also: **fit**, **2dfit** 


@subsection occt_draw_4_1_5 fit, 2dfit

      

~~~~~
fit [index] 
2dfit [index] 
~~~~~

**fit** computes the best zoom and pans on the content of the view. The content of the view will be centered and fit the whole window. 

When fitting all views a unique zoom is computed for all the views. All views are on the same scale. 


~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~{.cpp}
# fit only view 1 
fit 1 
# fit all 2d views 
2dfit 
~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
See also: **zoom**, **mu**, **pu** 


@subsection occt_draw_4_1_6 u, d, l, r

      

~~~~~
u [index] 
d [index] 
l [index] 
r [index] 
~~~~~

**u**, **d**, **l**, **r** Rotate the object in view around its axis by five degrees up, down, left or right respectively. This command is restricted to axonometric and perspective views. 


~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~{.cpp}
# rotate the view up 
u 
~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

@subsection occt_draw_4_1_7 focal, fu, fd

                  
~~~~~
focal [f] 
fu [index] 
fd [index] 
~~~~~

* **focal** changes the vantage point in perspective views. A low f value increases the perspective effect; a high one give a perspective similar to that of an axonometric view. The default value is 500. 
* **fu** and **fd** increase or decrease the focal value by 10%. **fd** makes the eye closer to the object. 


~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~{.cpp}
pers 
repeat 10 fd 
~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

**Note**: Do not use a negative or null focal value. 

See also: **pers** 

@subsection occt_draw_4_1_8 color

 

~~~~~
color index name 
~~~~~

**color** sets the color to a value. The index of the *color* is a value between 0 and 15. The name is an X window color name. The list of these can be found in the file *rgb.txt* in the X library directory. 

The default values are: 0 White, 1 Red, 2 Green, 3 Blue, 4 Cyan, 5 Gold, 6 Magenta, 7 Marron, 8 Orange, 9 Pink, 10 Salmon, 11 Violet, 12 Yellow, 13 Khaki, 14 Coral. 


~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~{.cpp}
# change the value of blue 
color 3 "navy blue" 
~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~


**Note** that the color change will be visible on the next redraw of the views, for example, after *fit* or *mu*, etc. 

@subsection occt_draw_4_1_9 dtext

      
~~~~~
dtext [x y [z]] string 
~~~~~

**dtext** displays a string in all 3d or 2d views. If no coordinates are given, a graphic selection is required. If two coordinates are given, the text is created in a 2d view at the position specified. With 3 coordinates, the text is created in a 3d view. 

The coordinates are real space coordinates. 


~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~{.cpp}
# mark the origins 
dtext 0 0 bebop 
dtext 0 0 0 bebop 
~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

@subsection occt_draw_4_1_10 hardcopy, hcolor, xwd

      
~~~~~
hardcopy [index] 
hcolor index width gray 
xwd [index] filename 
~~~~~

* **hardcopy** creates a postcript file called a4.ps in the current directory. This file contains the postscript description of the view index, and will allow you to print the view. 
* **hcolor** lets you change the aspect of lines in the postscript file. It allows to specify a width and a gray level for one of the 16 colors. **width** is measured in points with default value as 1, **gray** is the gray level from 0 = black to 1 = white with default value as 0. All colors are bound to the default values at the beginning. 
* **xwd** creates an X window xwd file from an active view. By default, the index is set to1. To visualize an xwd file, use the unix command **xwud**. 


~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~{.cpp}
# all blue lines (color 3) 
# will be half-width and gray 
hcolor 3 0.5 

# make a postscript file and print it 
hardcopy 
lpr a4.ps 

# make an xwd file and display it 
xwd theview 
xwud -in theview 
~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

**Note:** When more than one view is present, specify the index of the view. 

Only use a postscript printer to print postscript files. 

See also: **color** 


@subsection occt_draw_4_1_11 wclick, pick

      
~~~~~
wclick 
pick index X Y Z b [nowait] 
~~~~~

**wclick** defers an event until the mouse button is clicked. The message <code>just click</code> is displayed. 

Use the **pick** command to get graphic input. The arguments must be names for variables where the results are stored. 
  * index: index of the view where the input was made.
  * X,Y,Z: 3d coordinates in real world.
  * b: b is the mouse button 1,2 or 3.

When there is an extra argument, its value is not used and the command does not wait for a click; the value of b may then be 0 if there has not been a click. 

This option is useful for tracking the pointer. 

**Note** that the results are stored in Draw numeric variables.


~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~{.cpp}
# make a circle at mouse location 
pick index x y z b 
circle c x y z 0 0 1 1 0 0 0 30 

# make a dynamic circle at mouse location 
# stop when a button is clicked 
# (see the repaint command) 

dset b 0 
while {[dval b] == 0} { 
pick index x y z b nowait 
circle c x y z 0 0 1 1 0 0 0 30 
repaint 
} 
~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
See also: **repaint** 


Draw provides commands to manage the display of objects. 
* **display**, **donly** are used to display, 
* **erase**, **clear**, **2dclear** to erase. 
* **autodisplay** command is used to check whether variables are displayed when created. 

The variable name "." (dot) has a special status in Draw. Any Draw command expecting a Draw object as argument can be passed a dot. The meaning of the dot is the following. 
  * If the dot is an input argument, a graphic selection will be made. Instead of getting the object from a variable, Draw will ask you to select an object in a view.
  * If the dot is an output argument, an unnamed object will be created. Of course this makes sense only for graphic objects: if you create an unnamed number you will not be able to access it. This feature is used when you want to create objects for display only.
  * If you do not see what you expected while executing loops or sourcing files, use the **repaint** and **dflush** commands.


~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~{.cpp}
# OK use dot to dump an object on the screen 
dump . 

point . x y z 

#Not OK. display points on a curve c 
# with dot no variables are created 
for {set i 0} {$i = 10} {incr i} { 
cvalue c $i/10 x y z 
point . x y z 
} 

# point p x y z 
# would have displayed only one point 
# because the precedent variable content is erased 

# point p$i x y z 
# is an other solution, creating variables 
# p0, p1, p2, .... 

# give a name to a graphic object 
rename . x 
~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~


@subsection occt_draw_4_1_12 autodisplay

      

~~~~~
autodisplay [0/1] 
~~~~~

By default, Draw automatically displays any graphic object as soon as it is created. This behavior known as autodisplay can be removed with the command **autodisplay**. Without arguments, **autodisplay** toggles the autodisplay mode. The command always returns the current mode. 

When **autodisplay** is off, using the dot return argument is ineffective. 


~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~{.cpp}
# c is displayed 
circle c 0 0 1 0 5 

# toggle the mode 
autodisplay 
== 0 
circle c 0 0 1 0 5 

# c is erased, but not displayed 
display c 
~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

@subsection occt_draw_4_1_13 display, donly

      
~~~~~
display varname [varname ...] 
donly varname [varname ...] 
~~~~~

* **display** makes objects visible. 
* **donly** *display only* makes objects visible and erases all other objects. It is very useful to extract one object from a messy screen. 


~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~{.cpp}
\# to see all objects 
foreach var [directory] {display $var} 

\# to select two objects and erase the other ones 
donly . . 
~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~


@subsection occt_draw_4_1_14 erase, clear, 2dclear

      

~~~~~
erase [varname varname ...] 
clear 
2dclear 
~~~~~

**erase** removes objects from all views. **erase** without arguments erases everything in 2d and 3d. 

**clear** erases only 3d objects and **2dclear** only 2d objects. **erase** without arguments is similar to  **clear; 2dclear**.



~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~{.cpp}
# erase eveerything with a name starting with c_ 
foreach var [directory c_*] {erase $var} 

# clear 2d views 
2dclear 
~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

@subsection occt_draw_4_1_15 repaint, dflush




~~~~~
repaint 
dflush 
~~~~~

* **repaint** forces repainting of views. 
* **dflush** flushes the graphic buffers. 

These commands are useful within loops or in scripts. 

When an object is modified or erased, the whole view must be repainted. To avoid doing this too many times, Draw sets up a flag and delays the repaint to the end of the command in which the new prompt is issued. In a script, you may want to display the result of a change immediately. If the flag is raised, **repaint** will repaint the views and clear the flag. 

Graphic operations are buffered by Draw (and also by the X system). Usually the buffer is flushed at the end of a command and before graphic selection. If you want to flush the buffer from inside a script, use the **dflush** command. 

See also: @ref occt_draw_4_1_11 "pick" command.  

@section graphic_ais_viewer AIS viewer -- View
@subsection occt_draw_4_2_1 vinit

                  
~~~~~
vinit 
~~~~~
Creates a new View window with the specified *view_name*.
By default the view is created in the viewer and in the graphic driver shared with the active view.

~~~~
name = {driverName/viewerName/viewName | viewerName/viewName | viewName}
~~~~

If *driverName* is not specified the driver will be shared with the active view.
If *viewerName* is not specified the viewer will be shared with the active view.

@subsection occt_draw_4_2_2 vhelp


~~~~~
vhelp 
~~~~~
Displays help in the 3D viewer window. The help consists in a list of hotkeys and their functionalities. 

@subsection occt_draw_4_2_3 vtop


~~~~~
vtop 
~~~~~

Displays top view in the 3D viewer window. Orientation +X+Y.


~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~{.cpp}
vinit 
box b 10 10 10 
vdisplay b 
vfit 
vtop 
~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

@subsection occt_draw_4_2_4 vaxo

                  
~~~~~
vaxo 
~~~~~

Displays axonometric view in the 3D viewer window. Orientation +X-Y+Z.


~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~{.cpp}
vinit 
box b 10 10 10 
vdisplay b 
vfit 
vaxo 
~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

@subsection occt_draw_4_2_5 vsetbg

                  
~~~~~
vsetbg imagefile [filltype] 
~~~~~

Loads image file as background. *filltype* must be NONE, CENTERED, TILED or STRETCH. 


~~~~~
vinit 
vsetbg myimage.brep CENTERED 
~~~~~

@subsection occt_draw_4_2_6 vclear

                  
~~~~~
vclear 
~~~~~
Removes all objects from the viewer. 

@subsection occt_draw_4_2_7 vrepaint

                  
~~~~~
vrepaint 
~~~~~
Forcibly redisplays the shape in the 3D viewer window. 

@subsection occt_draw_4_2_8 vfit

                  
~~~~~
vfit 
~~~~~
Automatic zoom/panning. Objects in the view are visualized to occupy the maximum surface. 

@subsection occt_draw_4_2_9 vzfit

                  
~~~~~
vzfit 
~~~~~

Automatic depth panning. Objects in the view are visualized to occupy the maximum 3d space. 

@subsection occt_draw_4_2_10  vreadpixel

     
~~~~~
vreadpixel xPixel yPixel [{rgb|rgba|depth|hls|rgbf|rgbaf}=rgba] [name] 
~~~~~
Read pixel value for active view.


@subsection occt_draw_4_2_11  vselect

     
~~~~~
vselect x1 y1 [x2 y2 [x3 y3 ... xn yn]] [-allowoverlap 0|1] [shift_selection = 0|1]
~~~~~

Emulates different types of selection:

  * single mouse click selection
  * selection with a rectangle having the upper left and bottom right corners in <i>(x1,y1)</i> and <i>(x2,y2)</i> respectively
  * selection with a polygon having the corners in pixel positions <i>(x1,y1), (x2,y2),…, (xn,yn)</i>
  * <i> -allowoverlap </i> manages overlap and inclusion detection in rectangular selection. If the flag is set to 1, both sensitives that were included completely and overlapped partially by defined rectangle will be detected, otherwise algorithm will chose only fully included sensitives. Default behavior is to detect only full inclusion.
  * any of these selections if shift_selection is set to 1.

@subsection occt_draw_4_2_12  vmoveto

     

~~~~~
vmoveto x y
~~~~~
Emulates cursor movement to pixel position (x,y).

@subsection occt_draw_4_2_13  vviewparams

     
~~~~~
vviewparams [-scale [s]] [-eye [x y z]] [-at [x y z]] [-up [x y z]] [-proj [x y z]] [-center x y] [-size sx]
~~~~~
Gets or sets the current view parameters.
* If called without arguments, all view parameters are printed.
* The options are:
*   -scale [s]    : prints or sets the relative scale of viewport.
*   -eye [x y z]  : prints or sets the eye location.
*   -at [x y z]   : prints or sets the view center.
*   -up [x y z]   : prints or sets the up vector direction.
*   -proj [x y z] : prints or sets the view direction.
*   -center x y   : sets the screen center location in pixels.
*   -size [sx]    : prints viewport projection width and height sizes or changes the size of its maximum dimension.

@subsection occt_draw_4_2_14  vchangeselected

     
~~~~~
vchangeselected shape
~~~~~
Adds a shape to selection or removes one from it.

@subsection occt_draw_4_2_15  vzclipping

     
~~~~~
vzclipping [mode] [depth width]
~~~~~
Gets or sets ZClipping mode, width and depth, where
 - *mode = OFF|BACK|FRONT|SLICE*
 - *depth* is a real value from segment [0,1]
 - *width* is a real value from segment [0,1]

@subsection occt_draw_4_2_16  vnbselected

     
~~~~~
vnbselected
~~~~~
Returns the number of selected objects in the interactive context.

@subsection occt_draw_4_2_18  vpurgedisplay

     
~~~~~
vpurgedisplay [CollectorToo = 0|1]
~~~~~
Removes structures which do not belong to objects displayed in neutral point.

@subsection occt_draw_4_2_19  vhlr

     
~~~~~
vhlr is_enabled={on|off} [show_hidden={1|0}]
~~~~~
Hidden line removal algorithm:
 * <i>is_enabled</i> applies HLR algorithm.
 * <i>show_hidden</i> if equals to 1, hidden lines are drawn as dotted ones.

@subsection occt_draw_4_2_20  vhlrtype

     
~~~~~
vhlrtype  algo_type={algo|polyalgo} [shape_1 ... shape_n]
~~~~~

Changes the type of HLR algorithm used for shapes.
If the algo_type is algo, the exact HLR algorithm is used, otherwise the polygonal algorithm is used for defined shapes. 

If no shape is specified through the command arguments, the given HLR algorithm_type is applied to all *AIS_Shape* isntances in the current context, and the command also changes the default HLR algorithm type.

**Note** that this command works with instances of *AIS_Shape* or derived classes only, other interactive object types are ignored.

@subsection occt_draw_4_2_21 vcamera


~~~~~
vcamera [-ortho] [-projtype]
        [-persp]
        [-fovy   [Angle]] [-distance [Distance]]
        [-stereo] [-leftEye] [-rightEye]
        [-iod [Distance]] [-iodType    [absolute|relative]]
        [-zfocus [Value]] [-zfocusType [absolute|relative]]
~~~~~

Manages camera parameters.
Prints the current value when the option is called without argument.

Orthographic camera:
 * -ortho -- activates orthographic projection.
 
Perspective camera:
 * -persp -- activated perspective  projection (mono);
 * -fovy  -- field of view in y axis, in degrees;
 * -distance -- distance of eye from the camera center.
 
Stereoscopic camera:
 * -stereo -- perspective  projection (stereo);
 * -leftEye -- perspective  projection (left  eye);
 * -rightEye -- perspective  projection (right eye);
 * -iod -- intraocular distance value;
 * -iodType -- distance type, absolute or relative;
 * -zfocus -- stereographic focus value;
 * -zfocusType -- focus type, absolute or relative.

**Example:**
~~~~~
vinit
box b 10 10 10
vdisplay b
vfit
vcamera -persp
~~~~~

@subsection occt_draw_4_2_22 vstereo


~~~~~
vstereo [0|1] [-mode Mode] [-reverse {0|1}] [-anaglyph Filter]
~~~~~

Defines the stereo output mode. The following modes are available:
 * quadBuffer -- OpenGL QuadBuffer stereo, requires driver support. Should be called BEFORE *vinit*!
 * anaglyph         -- Anaglyph glasses;
 * rowInterlaced    -- row-interlaced display;
 * columnInterlaced -- column-interlaced display;
 * chessBoard       -- chess-board output;
 * sideBySide       -- horizontal pair;
 * overUnder        -- vertical pair;
Available Anaglyph filters for -anaglyph:
 * redCyan, redCyanSimple, yellowBlue, yellowBlueSimple, greenMagentaSimple.

**Example:**
~~~~~
vinit
box b 10 10 10
vdisplay b
vstereo 1
vfit
vcamera -stereo -iod 1
vcamera -lefteye
vcamera -righteye
~~~~~

@subsection occt_draw_4_2_23 vfrustumculling


~~~~~
vfrustumculling [toEnable]
~~~~~

Enables/disables objects clipping.

@section graphic_ais_display AIS viewer -- Display
@subsection occt_draw_4_3_1 vdisplay

 
~~~~~                 
vdisplay [-noupdate|-update] [-local] [-mutable] [-neutral]
         [-trsfPers {pan|zoom|rotate|trihedron|full|none}=none] [-trsfPersPos X Y [Z]] [-3d|-2d|-2dTopDown]
         [-dispMode mode] [-highMode mode]
         [-layer index] [-top|-topmost|-overlay|-underlay]
         [-redisplay]
         name1 [name2] ... [name n]
~~~~~

Displays named objects.
Option <i>-local</i> enables display of objects in the local selection context.
Local selection context will be opened if there is not any.

* *noupdate* suppresses viewer redraw call.
* *mutable* enables optimization for mutable objects.
* *neutral* draws objects in the main viewer.
* *layer* sets z-layer for objects. It can use <i>-overlay|-underlay|-top|-topmost</i> instead of <i>-layer index</i> for the default z-layers.
* *top* draws objects on top of main presentations but below the topmost level.
* *topmost* draws in overlay for 3D presentations with independent Depth.
* *overlay* draws objects in overlay for 2D presentations (On-Screen-Display).
* *underlay* draws objects in underlay for 2D presentations (On-Screen-Display).
* *selectable|-noselect* controls selection of objects.
* *trsfPers* sets transform persistence flags. Flag *full* allows to pan, zoom and rotate.
* *trsfPersPos* sets an anchor point for transform persistence.
* *2d|-2dTopDown* displays object in screen coordinates.
* *dispmode* sets display mode for objects.
* *highmode* sets highlight mode for objects.
* *redisplay* recomputes presentation of objects.


~~~~~ 
vinit 
box b 40 40 40 10 10 10 
psphere s 20 
vdisplay s b 
vfit 
~~~~~ 

@subsection occt_draw_4_3_2 vdonly

                  
~~~~~
vdonly [-noupdate|-update] [name1] ...  [name n]
~~~~~ 

Displays only selected or named objects. If there are no selected or named objects, nothing is done. 


~~~~~ 
vinit 
box b 40 40 40 10 10 10 
psphere s 20 
vdonly b 
vfit
~~~~~ 
 
@subsection occt_draw_4_3_3 vdisplayall

                  
~~~~~ 
vdisplayall [-local]
~~~~~ 

Displays all erased interactive objects (see vdir and vstate).
Option <i>-local</i> enables displaying objects in the local selection context.


~~~~~ 
vinit 
box b 40 40 40 10 10 10 
psphere s 20 
vdisplayall 
vfit 
~~~~~ 

@subsection occt_draw_4_3_4 verase

                  
~~~~~
verase [name1] [name2] … [name n]
~~~~~ 

Erases some selected or named objects. If there are no selected or named objects, the whole viewer is erased. 


~~~~~
vinit 
box b1 40 40 40 10 10 10 
box b2 -40 -40 -40 10 10 10 
psphere s 20 
vdisplayall 
vfit 
# erase only first box 
verase b1 
# erase second box and sphere 
verase
~~~~~ 

@subsection occt_draw_4_3_5 veraseall

                  
~~~~~
veraseall
~~~~~ 

Erases all objects displayed in the viewer. 

**Example:**
~~~~~ 
vinit 
box b1 40 40 40 10 10 10 
box b2 -40 -40 -40 10 10 10 
psphere s 20 
vdisplayall 
vfit 
# erase only first box 
verase b1 
# erase second box and sphere 
verseall
~~~~~ 

@subsection occt_draw_4_3_6 vsetdispmode

                  
~~~~~
vsetdispmode [name] mode(0,1,2,3)
~~~~~ 

Sets display mode for all, selected or named objects. 
* *0* (*WireFrame*), 
* *1* (*Shading*), 
* *2* (*Quick HideLineremoval*), 
* *3* (*Exact HideLineremoval*). 


~~~~~
vinit 
box b 10 10 10 
vdisplay b 
vsetdispmode 1 
vfit
~~~~~
 
@subsection occt_draw_4_3_7 vdisplaytype

                  
~~~~~
vdisplaytype type
~~~~~ 

Displays all objects of a given type. 
The following types are possible: *Point*, *Axis*, *Trihedron*, *PlaneTrihedron*, *Line*, *Circle*, *Plane*, *Shape*, *ConnectedShape*, *MultiConn.Shape*, *ConnectedInter.*, *MultiConn.*, *Constraint* and *Dimension*. 

@subsection occt_draw_4_3_8 verasetype

                  
~~~~~
verasetype type
~~~~~ 

Erases all objects of a given type. 
Possible type is *Point*, *Axis*, *Trihedron*, *PlaneTrihedron*, *Line*, *Circle*, *Plane*, *Shape*, *ConnectedShape*, *MultiConn.Shape*, *ConnectedInter.*, *MultiConn.*, *Constraint* and *Dimension*. 

@subsection occt_draw_4_3_9 vtypes

                  
~~~~~
vtypes
~~~~~ 

Makes a list of known types and signatures in AIS. 

@subsection occt_draw_4_3_10 vaspects


~~~~~
vaspects [-noupdate|-update] [name1 [name2 [...]] | -defaults]
         [-setVisibility 0|1]
         [-setColor ColorName] [-setcolor R G B] [-unsetColor]
         [-setMaterial MatName] [-unsetMaterial]
         [-setTransparency Transp] [-unsetTransparency]
         [-setWidth LineWidth] [-unsetWidth]
         [-setLineType {solid|dash|dot|dotDash}] [-unsetLineType]
         [-freeBoundary {off/on | 0/1}]
         [-setFreeBoundaryWidth Width] [-unsetFreeBoundaryWidth]
         [-setFreeBoundaryColor {ColorName | R G B}] [-unsetFreeBoundaryColor]
         [-subshapes subname1 [subname2 [...]]]
         [-isoontriangulation 0|1]
         [-setMaxParamValue {value}]

~~~~~

Manages presentation properties of all, selected or named objects.
* *-subshapes* -- assigns presentation properties to the specified sub-shapes.
* *-defaults* -- assigns presentation properties to all objects that do not have their own specified properties and to all objects to be displayed in the future.
If *-defaults* option is used there should not be any names of objects and *-subshapes* specifier.

Aliases:
~~~~~
vsetcolor [-noupdate|-update] [name] ColorName

~~~~~


Manages presentation properties (color, material, transparency) of all objects, selected or named.

**Color**. The *ColorName* can be: *BLACK*, *MATRAGRAY*, *MATRABLUE*, *ALICEBLUE*, *ANTIQUEWHITE*, *ANTIQUEWHITE1*, *ANTIQUEWHITE2*, *ANTIQUEWHITE3*, *ANTIQUEWHITE4*, *AQUAMARINE1*, *AQUAMARINE2*, *AQUAMARINE4*, *AZURE*, *AZURE2*, *AZURE3*, *AZURE4*, *BEIGE*, *BISQUE*, *BISQUE2*, *BISQUE3*, *BISQUE4*, *BLANCHEDALMOND*, *BLUE1*, *BLUE2*, *BLUE3*, *BLUE4*, *BLUEVIOLET*, *BROWN*, *BROWN1*, *BROWN2*, *BROWN3*, *BROWN4*, *BURLYWOOD*, *BURLYWOOD1*, *BURLYWOOD2*, *BURLYWOOD3*, *BURLYWOOD4*, *CADETBLUE*, *CADETBLUE1*, *CADETBLUE2*, *CADETBLUE3*, *CADETBLUE4*, *CHARTREUSE*, *CHARTREUSE1*, *CHARTREUSE2*, *CHARTREUSE3*, *CHARTREUSE4*, *CHOCOLATE*, *CHOCOLATE1*, *CHOCOLATE2*, *CHOCOLATE3*, *CHOCOLATE4*, *CORAL*, *CORAL1*, *CORAL2*, *CORAL3*, *CORAL4*, *CORNFLOWERBLUE*, *CORNSILK1*, *CORNSILK2*, *CORNSILK3*, *CORNSILK4*, *CYAN1*, *CYAN2*, *CYAN3*, *CYAN4*, *DARKGOLDENROD*, *DARKGOLDENROD1*, *DARKGOLDENROD2*, *DARKGOLDENROD3*, *DARKGOLDENROD4*, *DARKGREEN*, *DARKKHAKI*, *DARKOLIVEGREEN*, *DARKOLIVEGREEN1*, *DARKOLIVEGREEN2*, *DARKOLIVEGREEN3*, *DARKOLIVEGREEN4*, *DARKORANGE*, *DARKORANGE1*, *DARKORANGE2*, *DARKORANGE3*, *DARKORANGE4*, *DARKORCHID*, *DARKORCHID1*, *DARKORCHID2*, *DARKORCHID3*, *DARKORCHID4*, *DARKSALMON*, *DARKSEAGREEN*, *DARKSEAGREEN1*, *DARKSEAGREEN2*, *DARKSEAGREEN3*, *DARKSEAGREEN4*, *DARKSLATEBLUE*, *DARKSLATEGRAY1*, *DARKSLATEGRAY2*, *DARKSLATEGRAY3*, *DARKSLATEGRAY4*, *DARKSLATEGRAY*, *DARKTURQUOISE*, *DARKVIOLET*, *DEEPPINK*, *DEEPPINK2*, *DEEPPINK3*, *DEEPPINK4*, *DEEPSKYBLUE1*, *DEEPSKYBLUE2*, *DEEPSKYBLUE3*, *DEEPSKYBLUE4*, *DODGERBLUE1*, *DODGERBLUE2*, *DODGERBLUE3*, *DODGERBLUE4*, *FIREBRICK*, *FIREBRICK1*, *FIREBRICK2*, *FIREBRICK3*, *FIREBRICK4*, *FLORALWHITE*, *FORESTGREEN*, *GAINSBORO*, *GHOSTWHITE*, *GOLD*, *GOLD1*, *GOLD2*, *GOLD3*, *GOLD4*, *GOLDENROD*, *GOLDENROD1*, *GOLDENROD2*, *GOLDENROD3*, *GOLDENROD4*, *GRAY*, *GRAY0*, *GRAY1*, *GRAY10*, *GRAY11*, *GRAY12*, *GRAY13*, *GRAY14*, *GRAY15*, *GRAY16*, *GRAY17*, *GRAY18*, *GRAY19*, *GRAY2*, *GRAY20*, *GRAY21*, *GRAY22*, *GRAY23*, *GRAY24*, *GRAY25*, *GRAY26*, *GRAY27*, *GRAY28*, *GRAY29*, *GRAY3*, *GRAY30*, *GRAY31*, *GRAY32*, *GRAY33*, *GRAY34*, *GRAY35*, *GRAY36*, *GRAY37*, *GRAY38*, *GRAY39*, *GRAY4*, *GRAY40*, *GRAY41*, *GRAY42*, *GRAY43*, *GRAY44*, *GRAY45*, *GRAY46*, *GRAY47*, *GRAY48*, *GRAY49*, *GRAY5*, *GRAY50*, *GRAY51*, *GRAY52*, *GRAY53*, *GRAY54*, *GRAY55*, *GRAY56*, *GRAY57*, *GRAY58*, *GRAY59*, *GRAY6*, *GRAY60*, *GRAY61*, *GRAY62*, *GRAY63*, *GRAY64*, *GRAY65*, *GRAY66*, *GRAY67*, *GRAY68*, *GRAY69*, *GRAY7*, *GRAY70*, *GRAY71*, *GRAY72*, *GRAY73*, *GRAY74*, *GRAY75*, *GRAY76*, *GRAY77*, *GRAY78*, *GRAY79*, *GRAY8*, *GRAY80*, *GRAY81*, *GRAY82*, *GRAY83*, *GRAY85*, *GRAY86*, *GRAY87*, *GRAY88*, *GRAY89*, *GRAY9*, *GRAY90*, *GRAY91*, *GRAY92*, *GRAY93*, *GRAY94*, *GRAY95*, *GREEN*, *GREEN1*, *GREEN2*, *GREEN3*, *GREEN4*, *GREENYELLOW*, *GRAY97*, *GRAY98*, *GRAY99*, *HONEYDEW*, *HONEYDEW2*, *HONEYDEW3*, *HONEYDEW4*, *HOTPINK*, *HOTPINK1*, *HOTPINK2*, *HOTPINK3*, *HOTPINK4*, *INDIANRED*, *INDIANRED1*, *INDIANRED2*, *INDIANRED3*, *INDIANRED4*, *IVORY*, *IVORY2*, *IVORY3*, *IVORY4*, *KHAKI*, *KHAKI1*, *KHAKI2*, *KHAKI3*, *KHAKI4*, *LAVENDER*, *LAVENDERBLUSH1*, *LAVENDERBLUSH2*, *LAVENDERBLUSH3*, *LAVENDERBLUSH4*, *LAWNGREEN*, *LEMONCHIFFON1*, *LEMONCHIFFON2*, *LEMONCHIFFON3*, *LEMONCHIFFON4*, *LIGHTBLUE*, *LIGHTBLUE1*, *LIGHTBLUE2*, *LIGHTBLUE3*, *LIGHTBLUE4*, *LIGHTCORAL*, *LIGHTCYAN1*, *LIGHTCYAN2*, *LIGHTCYAN3*, *LIGHTCYAN4*, *LIGHTGOLDENROD*, *LIGHTGOLDENROD1*, *LIGHTGOLDENROD2*, *LIGHTGOLDENROD3*, *LIGHTGOLDENROD4*, *LIGHTGOLDENRODYELLOW*, *LIGHTGRAY*, *LIGHTPINK*, *LIGHTPINK1*, *LIGHTPINK2*, *LIGHTPINK3*, *LIGHTPINK4*, *LIGHTSALMON1*, *LIGHTSALMON2*, *LIGHTSALMON3*, *LIGHTSALMON4*, *LIGHTSEAGREEN*, *LIGHTSKYBLUE*, *LIGHTSKYBLUE1*, *LIGHTSKYBLUE2*, *LIGHTSKYBLUE3*, *LIGHTSKYBLUE4*, *LIGHTSLATEBLUE*, *LIGHTSLATEGRAY*, *LIGHTSTEELBLUE*, *LIGHTSTEELBLUE1*, *LIGHTSTEELBLUE2*, *LIGHTSTEELBLUE3*, *LIGHTSTEELBLUE4*, *LIGHTYELLOW*, *LIGHTYELLOW2*, *LIGHTYELLOW3*, *LIGHTYELLOW4*, *LIMEGREEN*, *LINEN*, *MAGENTA1*, *MAGENTA2*, *MAGENTA3*, *MAGENTA4*, *MAROON*, *MAROON1*, *MAROON2*, *MAROON3*, *MAROON4*, *MEDIUMAQUAMARINE*, *MEDIUMORCHID*, *MEDIUMORCHID1*, *MEDIUMORCHID2*, *MEDIUMORCHID3*, *MEDIUMORCHID4*, *MEDIUMPURPLE*, *MEDIUMPURPLE1*, *MEDIUMPURPLE2*, *MEDIUMPURPLE3*, *MEDIUMPURPLE4*, *MEDIUMSEAGREEN*, *MEDIUMSLATEBLUE*, *MEDIUMSPRINGGREEN*, *MEDIUMTURQUOISE*, *MEDIUMVIOLETRED*, *MIDNIGHTBLUE*, *MINTCREAM*, *MISTYROSE*, *MISTYROSE2*, *MISTYROSE3*, *MISTYROSE4*, *MOCCASIN*, *NAVAJOWHITE1*, *NAVAJOWHITE2*, *NAVAJOWHITE3*, *NAVAJOWHITE4*, *NAVYBLUE*, *OLDLACE*, *OLIVEDRAB*, *OLIVEDRAB1*, *OLIVEDRAB2*, *OLIVEDRAB3*, *OLIVEDRAB4*, *ORANGE*, *ORANGE1*, *ORANGE2*, *ORANGE3*, *ORANGE4*, *ORANGERED*, *ORANGERED1*, *ORANGERED2*, *ORANGERED3*, *ORANGERED4*, *ORCHID*, *ORCHID1*, *ORCHID2*, *ORCHID3*, *ORCHID4*, *PALEGOLDENROD*, *PALEGREEN*, *PALEGREEN1*, *PALEGREEN2*, *PALEGREEN3*, *PALEGREEN4*, *PALETURQUOISE*, *PALETURQUOISE1*, *PALETURQUOISE2*, *PALETURQUOISE3*, *PALETURQUOISE4*, *PALEVIOLETRED*, *PALEVIOLETRED1*, *PALEVIOLETRED2*, *PALEVIOLETRED3*, *PALEVIOLETRED4*, *PAPAYAWHIP*, *PEACHPUFF*, *PEACHPUFF2*, *PEACHPUFF3*, *PEACHPUFF4*, *PERU*, *PINK*, *PINK1*, *PINK2*, *PINK3*, *PINK4*, *PLUM*, *PLUM1*, *PLUM2*, *PLUM3*, *PLUM4*, *POWDERBLUE*, *PURPLE*, *PURPLE1*, *PURPLE2*, *PURPLE3*, *PURPLE4*, *RED*, *RED1*, *RED2*, *RED3*, *RED4*, *ROSYBROWN*, *ROSYBROWN1*, *ROSYBROWN2*, *ROSYBROWN3*, *ROSYBROWN4*, *ROYALBLUE*, *ROYALBLUE1*, *ROYALBLUE2*, *ROYALBLUE3*, *ROYALBLUE4*, *SADDLEBROWN*, *SALMON*, *SALMON1*, *SALMON2*, *SALMON3*, *SALMON4*, *SANDYBROWN*, *SEAGREEN*, *SEAGREEN1*, *SEAGREEN2*, *SEAGREEN3*, *SEAGREEN4*, *SEASHELL*, *SEASHELL2*, *SEASHELL3*, *SEASHELL4*, *BEET*, *TEAL*, *SIENNA*, *SIENNA1*, *SIENNA2*, *SIENNA3*, *SIENNA4*, *SKYBLUE*, *SKYBLUE1*, *SKYBLUE2*, *SKYBLUE3*, *SKYBLUE4*, *SLATEBLUE*, *SLATEBLUE1*, *SLATEBLUE2*, *SLATEBLUE3*, *SLATEBLUE4*, *SLATEGRAY1*, *SLATEGRAY2*, *SLATEGRAY3*, *SLATEGRAY4*, *SLATEGRAY*, *SNOW*, *SNOW2*, *SNOW3*, *SNOW4*, *SPRINGGREEN*, *SPRINGGREEN2*, *SPRINGGREEN3*, *SPRINGGREEN4*, *STEELBLUE*, *STEELBLUE1*, *STEELBLUE2*, *STEELBLUE3*, *STEELBLUE4*, *TAN*, *TAN1*, *TAN2*, *TAN3*, *TAN4*, *THISTLE*, *THISTLE1*, *THISTLE2*, *THISTLE3*, *THISTLE4*, *TOMATO*, *TOMATO1*, *TOMATO2*, *TOMATO3*, *TOMATO4*, *TURQUOISE*, *TURQUOISE1*, *TURQUOISE2*, *TURQUOISE3*, *TURQUOISE4*, *VIOLET*, *VIOLETRED*, *VIOLETRED1*, *VIOLETRED2*, *VIOLETRED3*, *VIOLETRED4*, *WHEAT*, *WHEAT1*, *WHEAT2*, *WHEAT3*, *WHEAT4*, *WHITE*, *WHITESMOKE*, *YELLOW*, *YELLOW1*, *YELLOW2*, *YELLOW3*, *YELLOW4* and *YELLOWGREEN*.
~~~~~
vaspects    [name] [-setcolor ColorName] [-setcolor R G B] [-unsetcolor]
vsetcolor   [name] ColorName
vunsetcolor [name]
~~~~~

**Transparency. The *Transp* may be between 0.0 (opaque) and 1.0 (fully transparent).
**Warning**: at 1.0 the shape becomes invisible.
~~~~~
vaspects           [name] [-settransparency Transp] [-unsettransparency]
vsettransparency   [name] Transp
vunsettransparency [name]
~~~~~

**Material**. The *MatName* can be *BRASS*, *BRONZE*, *COPPER*, *GOLD*, *PEWTER*, *PLASTER*, *PLASTIC*, *SILVER*, *STEEL*, *STONE*, *SHINY_PLASTIC*, *SATIN*, *METALIZED*, *NEON_GNC*, *CHROME*, *ALUMINIUM*, *OBSIDIAN*, *NEON_PHC*, *JADE*, *WATER*, *GLASS*, *DIAMOND* or *CHARCOAL*.
~~~~~
vaspects       [name] [-setmaterial MatName] [-unsetmaterial]
vsetmaterial   [name] MatName
vunsetmaterial [name]
~~~~~

**Line width**. Specifies width of the edges. The *LineWidth* may be between 0.0 and 10.0.
~~~~~
vaspects    [name] [-setwidth LineWidth] [-unsetwidth]
vsetwidth   [name] LineWidth
vunsetwidth [name]
~~~~~

**Example:**
~~~~~
vinit
box b 10 10 10
vdisplay b
vfit

vsetdispmode b 1
vaspects -setcolor red -settransparency 0.2
vrotate 10 10 10
~~~~~






@subsection occt_draw_4_3_11 vsetshading

                  
~~~~~
vsetshading shapename [coefficient]
~~~~~ 

Sets deflection coefficient that defines the quality of the shape’s representation in the shading mode. Default coefficient is 0.0008. 


~~~~~
vinit 
psphere s 20 
vdisplay s 
vfit 
vsetdispmode 1 
vsetshading s 0.005
~~~~~
 
@subsection occt_draw_4_3_12 vunsetshading

                  
~~~~~
vunsetshading [shapename]
~~~~~ 

Sets default deflection coefficient (0.0008) that defines the quality of the shape’s representation in the shading mode.

@subsection occt_draw_4_3_13 vsetam

                  
~~~~~
vsetam [shapename] mode
~~~~~ 

Activates selection mode for all selected or named shapes: 
* *0* for *shape* itself, 
* *1* (*vertices*), 
* *2* (*edges*), 
* *3* (*wires*), 
* *4* (*faces*), 
* *5* (*shells*),
* *6* (*solids*),
* *7* (*compounds*).
 

~~~~~
vinit 
box b 10 10 10 
vdisplay b 
vfit 
vsetam b 2
~~~~~
 
@subsection occt_draw_4_3_14 vunsetam

                  
~~~~~
vunsetam
~~~~~ 

Deactivates all selection modes for all shapes. 

@subsection occt_draw_4_3_15 vdump

                  
~~~~~
vdump <filename>.{png|bmp|jpg|gif} [-width Width -height Height]
      [-buffer rgb|rgba|depth=rgb]
      [-stereo mono|left|right|blend|sideBySide|overUnder=mono]

~~~~~ 

Extracts the contents of the viewer window to a image file.

@subsection occt_draw_4_3_16 vdir

                  
~~~~~
vdir
~~~~~ 

Displays the list of displayed objects. 

@subsection occt_draw_4_3_17 vsub

                  
~~~~~
vsub 0/1(on/off)[shapename]
~~~~~ 

Hilights/unhilights named or selected objects which are displayed at neutral state with subintensity color.
 

~~~~~
vinit 
box b 10 10 10 
psphere s 20 
vdisplay b s 
vfit 
vsetdispmode 1 
vsub b 1
~~~~~ 

@subsection occt_draw_4_3_20 vsensdis

                  
~~~~~
vsensdis
~~~~~ 

Displays active entities (sensitive entities of one of the standard types corresponding to active selection modes). 

Standard entity types are those defined in Select3D package: 
  * sensitive box
  * sensitive face
  * sensitive curve
  * sensitive segment
  * sensitive circle
  * sensitive point
  * sensitive triangulation
  * sensitive triangle
Custom (application-defined) sensitive entity types are not processed by this command. 

@subsection occt_draw_4_3_21 vsensera

                  
~~~~~
vsensera
~~~~~ 

Erases active entities. 

@subsection occt_draw_4_3_22 vperf

                  
~~~~~
vperf shapename 1/0 (Transformation/Loacation) 1/0 (Primitives sensibles ON/OFF)
~~~~~ 

Tests the animation of an object along a predefined trajectory. 


~~~~~
vinit 
box b 10 10 10 
psphere s 20 
vdisplay b s 
vfit 
vsetdispmode 0 
vperf b 1 1
~~~~~
 
@subsection occt_draw_4_3_23 vr

                  
~~~~~
vr filename
~~~~~ 

Reads shape from BREP-format file and displays it in the viewer. 


~~~~~
vinit 
vr myshape.brep
~~~~~
 
@subsection occt_draw_4_3_24 vstate

                  
~~~~~
vstate [-entities] [-hasSelected] [name1] ... [nameN]
~~~~~ 

Reports show/hidden state for selected or named objects:
 * *entities* -- prints low-level information about detected entities;
 * *hasSelected* -- prints 1 if the context has a selected shape and 0 otherwise.

@subsection occt_draw_4_3_25 vraytrace


~~~~~
vraytrace [0/1]
~~~~~

Turns on/off ray tracing renderer.

@subsection occt_draw_4_3_26 vrenderparams


~~~~~
vrenderparams [-rayTrace|-raster] [-rayDepth 0..10] [-shadows {on|off}]
              [-reflections {on|off}] [-fsaa {on|off}] [-gleam {on|off}]
              [-gi {on|off}] [-brng {on|off}] [-env {on|off}]
              [-shadin {color|flat|gouraud|phong}]
~~~~~

Manages rendering parameters:
* rayTrace     -- Enables  GPU ray-tracing
* raster       -- Disables GPU ray-tracing
* rayDepth     -- Defines maximum ray-tracing depth
* shadows      -- Enables/disables shadows rendering
* reflections  -- Enables/disables specular reflections
* fsaa         -- Enables/disables adaptive anti-aliasing
* gleam        -- Enables/disables transparency shadow effects
* gi           -- Enables/disables global illumination effects
* brng         -- Enables/disables blocked RNG (fast coherent PT)
* env          -- Enables/disables environment map background
* shadingModel -- Controls shading model from enumeration color, flat, gouraud, phong

Unlike *vcaps*, these parameters dramatically change visual properties.
The command is intended to control presentation quality depending on hardware capabilities and performance.

**Example:**
~~~~~
vinit
box b 10 10 10
vdisplay b
vfit
vraytrace 1
vrenderparams -shadows 1 -reflections 1 -fsaa 1
~~~~~
@subsection occt_draw_4_3_27 vshaderprog


~~~~~
   'vshaderprog [name] pathToVertexShader pathToFragmentShader'
or 'vshaderprog [name] off'   to disable GLSL program
or 'vshaderprog [name] phong' to enable per-pixel lighting calculations
~~~~~

Enables rendering using a shader program.

@subsection occt_draw_4_3_28 vsetcolorbg


~~~~~
vsetcolorbg r g b
~~~~~

Sets background color.

**Example:**
~~~~~
vinit
vsetcolorbg 200 0 200
~~~~~

@section graphic_ais_object AIS viewer -- Object
@subsection occt_draw_4_4_1 vtrihedron

                  
~~~~~
vtrihedron name [X0] [Y0] [Z0] [Zu] [Zv] [Zw] [Xu] [Xv] [Xw]
~~~~~ 

Creates a new *AIS_Trihedron* object. If no argument is set, the default trihedron (0XYZ) is created.
 

~~~~~
vinit 
vtrihedron tr
~~~~~ 

@subsection occt_draw_4_4_2 vplanetri

                  
~~~~~
vplanetri name
~~~~~ 

Creates a plane from a trihedron selection. If no arguments are set, the default plane is created. 


@subsection occt_draw_4_4_3 vsize

                  
~~~~~
vsize [name] [size]
~~~~~ 

Changes the size of a named or selected trihedron. If the name is not defined: it affects the selected trihedrons otherwise nothing is done. If the value is not defined, it is set to 100 by default.
 

~~~~~
vinit 
vtrihedron tr1 
vtrihedron tr2 0 0 0 1 0 0 1 0 0 
vsize tr2 400
~~~~~ 

@subsection occt_draw_4_4_4 vaxis

                  
~~~~~
vaxis name [Xa Ya Za Xb Yb Zb]
~~~~~ 

Creates an axis. If  the values are not defined, an axis is created by interactive selection of two vertices or one edge
 

~~~~~
vinit 
vtrihedron tr 
vaxis axe1 0 0 0 1 0 0 
~~~~~

@subsection occt_draw_4_4_5 vaxispara

                  
~~~~~
vaxispara name
~~~~~ 

Creates an axis by interactive selection of an edge and a vertex. 

@subsection occt_draw_4_4_6 vaxisortho

                  
~~~~~
vaxisotrho name
~~~~~ 

Creates an axis by interactive selection of an edge and a vertex. The axis will be orthogonal to the selected edge. 

@subsection occt_draw_4_4_7 vpoint

                  
~~~~~
vpoint name [Xa Ya Za]
~~~~~ 

Creates a point from coordinates. If the values are not defined, a point is created by interactive selection of a vertice or an edge (in the center of the edge). 


~~~~~
vinit 
vpoint p 0 0 0 
~~~~~

@subsection occt_draw_4_4_8 vplane

                  
~~~~~
vplane name [AxisName] [PointName] 
vplane name [PointName] [PointName] [PointName] 
vplane name [PlaneName] [PointName]
~~~~~ 

Creates a plane from named or interactively selected entities.
TypeOfSensitivity:
 * 0 -- Interior
 * 1 -- Boundary


~~~~~
vinit 
vpoint p1 0 50 0 
vaxis axe1 0 0 0 0 0 1 
vtrihedron tr 
vplane plane1 axe1 p1 
~~~~~

@subsection occt_draw_4_4_9 vplanepara

                  
~~~~~
vplanepara name
~~~~~ 

Creates a plane from interactively selected vertex and face. 

@subsection occt_draw_4_4_10 vplaneortho

                  
~~~~~
vplaneortho name
~~~~~ 

Creates a plane from interactive selected face and coplanar edge. 

@subsection occt_draw_4_4_11 vline

                  
~~~~~
vline name [PointName] [PointName] 
vline name [Xa Ya Za Xb Yb Zb]
~~~~~ 

Creates a line from coordinates, named or interactively selected vertices. 


~~~~~
vinit 
vtrihedron tr 
vpoint p1 0 50 0 
vpoint p2 50 0 0 
vline line1 p1 p2 
vline line2 0 0 0 50 0 1 
~~~~~

@subsection occt_draw_4_4_12 vcircle

      
~~~~~
vcircle name [PointName PointName PointName IsFilled] 
vcircle name [PlaneName PointName Radius IsFilled] 
~~~~~

Creates a circle from named or interactively selected entities.  Parameter IsFilled is defined as 0 or 1.
 

~~~~~
vinit 
vtrihedron tr 
vpoint p1 0 50 0 
vpoint p2 50 0 0 
vpoint p3 0 0 0 
vcircle circle1 p1 p2 p3 1
~~~~~ 

@subsection occt_draw_4_4_13 vtri2d

                  
~~~~~
vtri2d name
~~~~~ 

Creates a plane with a 2D trihedron from an interactively selected face. 

@subsection occt_draw_4_4_14 vselmode

                  
~~~~~
vselmode [object] mode_number is_turned_on=(1|0)
~~~~~ 

Sets the selection mode for an object. If the object value is not defined, the selection mode is set for all displayed objects. 
*Mode_number* is a non-negative integer encoding different interactive object classes.
For shapes the following *mode_number* values are allowed:
 * 0 -- shape
 * 1 -- vertex
 * 2 -- edge
 * 3 -- wire
 * 4 -- face
 * 5 -- shell
 * 6 -- solid
 * 7 -- compsolid
 * 8 -- compound
*is_turned_on* is:
 * 1 if mode is to be switched on
 * 0 if mode is to be switched off


~~~~~
vinit 
vpoint p1 0 0 0 
vpoint p2 50 0 0 
vpoint p3 25 40 0 
vtriangle triangle1 p1 p2 p3 
~~~~~

@subsection occt_draw_4_4_15 vconnect

                  
~~~~~
vconnect vconnect name Xo Yo Zo object1 object2 ... [color=NAME]
~~~~~ 

Creates *AIS_ConnectedInteractive* object from the input object and location and displays it.


~~~~~
vinit 
vpoint p1 0 0 0 
vpoint p2 50 0 0 
vsegment segment p1 p2 
restore CrankArm.brep obj 
vdisplay obj 
vconnect new obj 100100100 1 0 0 0 0 1
~~~~~ 

@subsection occt_draw_4_4_16 vtriangle

                  
~~~~~
vtriangle name PointName PointName PointName
~~~~~ 

Creates and displays a filled triangle from named points. 


~~~~~
vinit 
vpoint p1 0 0 0 
vpoint p2 50 0 0 
vpoint p3 25 40 0 
vtriangle triangle1 p1 p2 p3
~~~~~ 

@subsection occt_draw_4_4_17 vsegment

                  
~~~~~
vsegment name PointName PointName 
~~~~~

Creates and displays a segment from named points. 


~~~~~
Vinit 
vpoint p1 0 0 0 
vpoint p2 50 0 0 
vsegment segment p1 p2 
~~~~~

@subsection occt_draw_4_4_18 vpointcloud


~~~~~
vpointcloud name shape [-randColor] [-normals] [-noNormals]
~~~~~

Creates an interactive object for an arbitrary set of points from the triangulated shape.
Additional options:
 * *randColor* -- generates a random color per point;
 * *normals*   -- generates a normal per point (default);
 * *noNormals* -- does not generate a normal per point.

~~~~~
vpointcloud name x y z r npts {surface|volume} [-randColor] [-normals] [-noNormals]
~~~~~
Creates an arbitrary set of points (npts) randomly distributed on a spheric surface or within a spheric volume (x y z r).
Additional options:
 * *randColor* -- generates a random color per point;
 * *normals*   -- generates a normal per point (default);
 * *noNormals* -- does not generate a normal per point.

**Example:**
~~~~~
vinit
vpointcloud pc 0 0 0 100 100000 surface -randColor
vfit
~~~~~

@subsection occt_draw_4_4_19 vclipplane


~~~~~
vclipplane maxplanes <view_name> -- gets plane limit for the view.
vclipplane create <plane_name> -- creates a new plane.
vclipplane delete <plane_name> -- deletes a plane.
vclipplane clone <source_plane> <plane_name> -- clones the plane definition.
vclipplane set/unset <plane_name> object <object list> -- sets/unsets the plane for an IO.
vclipplane set/unset <plane_name> view <view list> -- sets/unsets plane for a view.
vclipplane change <plane_name> on/off -- turns clipping on/off.
vclipplane change <plane_name> equation <a> <b> <c> <d> -- changes plane equation.
vclipplane change <plane_name> capping on/off -- turns capping on/off.
vclipplane change <plane_name> capping color <r> <g> <b> -- sets color.
vclipplane change <plane name> capping texname <texture> -- sets texture.
vclipplane change <plane_name> capping texscale <sx> <sy> -- sets texture scale.
vclipplane change <plane_name> capping texorigin <tx> <ty> -- sets texture origin.
vclipplane change <plane_name> capping texrotate <angle> -- sets texture rotation.
vclipplane change <plane_name> capping hatch on/off/<id> -- sets hatching mask.
~~~~~

Manages clipping planes

**Example:**
~~~~~
vinit
vclipplane create pln1
vclipplane change pln1 equation 1 0 0 -0.1
vclipplane set pln1 view Driver1/Viewer1/View1
box b 100 100 100
vdisplay b
vsetdispmode 1
vfit
vrotate 10 10 10
vselect 100 100
~~~~~

@subsection occt_draw_4_4_20 vdimension


~~~~~
vdimension name {-angle|-length|-radius|-diameter} -shapes shape1 [shape2 [shape3]]
                [-text 3d|2d wf|sh|wireframe|shading IntegerSize]
                [-label left|right|hcenter|hfit top|bottom|vcenter|vfit]
                [-arrow external|internal|fit] [{-arrowlength|-arlen} RealArrowLength]
                [{-arrowangle|-arangle} ArrowAngle(degrees)] [-plane xoy|yoz|zox]
                [-flyout FloatValue -extension FloatValue]
                [-autovalue] [-value CustomRealValue] [-textvalue CustomTextValue]
                [-dispunits DisplayUnitsString]
                [-modelunits ModelUnitsString] [-showunits | -hideunits]
~~~~~

Builds angle, length, radius or diameter dimension interactive object **name**.

**Attension:** length dimension can't be built without working plane.

~~~~~
vinit
vpoint p1 0 0 0
vpoint p2 50 50 0
vdimension dim1 -length -plane xoy -shapes p1 p2

vpoint p3 100 0 0
vdimension dim2 -angle -shapes p1 p2 p3

vcircle circle p1 p2 p3 0
vdimension dim3 -radius -shapes circle
vfit
~~~~~

@subsection occt_draw_4_4_21 vdimparam


~~~~~
vdimparam name [-text 3d|2d wf|sh|wireframe|shading IntegerSize]
               [-label left|right|hcenter|hfit top|bottom|vcenter|vfit]
               [-arrow external|internal|fit]
               [{-arrowlength|-arlen} RealArrowLength]
               [{-arrowangle|-arangle} ArrowAngle(degrees)]
               [-plane xoy|yoz|zox]
               [-flyout FloatValue -extension FloatValue]
               [-autovalue]
               [-value CustomRealValue]
               [-textvalue CustomTextValue]
               [-dispunits DisplayUnitsString]
               [-modelunits ModelUnitsString]
               [-showunits | -hideunits]
~~~~~

Sets parameters for angle, length, radius and diameter dimension **name**.


~~~~~
vinit
vpoint p1 0 0 0
vpoint p2 50 50 0
vdimension dim1 -length -plane xoy -shapes p1 p2
vdimparam dim1 -flyout -15 -arrowlength 4 -showunits -value 10
vfit
vdimparam dim1 -textvalue "w_1"
vdimparam dim1 -autovalue
~~~~~

@subsection occt_draw_4_4_22 vdimangleparam

~~~~~
vangleparam name [-type interior|exterior]
               [-showarrow first|second|both|none]
~~~~~

Sets parameters for angle dimension **name**.

~~~~~
vinit
vpoint p1 0 0 0
vpoint p2 10 0 0
vpoint p3 10 5 0
vdimension dim1 -angle -plane xoy -shapes p1 p2 p3
vfit
vangleparam dim1 -type exterior -showarrow first
~~~~~

@subsubsection occt_draw_4_4_23 vmovedim

~~~~~
vmovedim [name] [x y z]
~~~~~

Moves picked or named (if **name** parameter is defined) dimension
to picked mouse position or input point with coordinates **x**,**y**,**z**.
Text label of dimension **name** is moved to position, another parts of dimension
are adjusted.

~~~~~
vinit
vpoint p1 0 0 0
vpoint p2 50 50 0
vdimension dim1 -length -plane xoy -shapes p1 p2
vmovedim dim1 -10 30 0
~~~~~

@section graphic_ais_mesh   AIS viewer -- Mesh Visualization Service    
<b>MeshVS</b> (Mesh Visualization Service) component provides flexible means of displaying meshes with associated pre- and post- processor data.

@subsection occt_draw_4_5_1 meshfromstl

                  
~~~~~
meshfromstl meshname file
~~~~~ 

Creates a *MeshVS_Mesh* object based on STL file data. The object will be displayed immediately.
 
**Example:**
~~~~~ 
meshfromstl mesh myfile.stl
~~~~~ 

@subsection occt_draw_4_5_2 meshdispmode

                  
~~~~~
meshdispmode meshname displaymode
~~~~~ 

Changes the display mode of object **meshname**. The **displaymode** is integer, which can be:
* *1* for *wireframe*, 
* *2* for *shading* mode, or
* *3* for *shrink* mode. 


~~~~~
vinit 
meshfromstl mesh myfile.stl 
meshdispmode mesh 2
~~~~~ 

@subsection occt_draw_4_5_3 meshselmode

                  
~~~~~
meshselmode meshname selectionmode
~~~~~ 

Changes the selection mode of object **meshname**. The *selectionmode* is integer OR-combination of mode flags. The basic flags are the following: 
* *1* -- node selection;
* *2* -- 0D elements (not supported in STL); 
* *4* -- links (not supported in STL); 
* *8* -- faces.
 

~~~~~
vinit 
meshfromstl mesh myfile.stl 
meshselmode mesh 1
~~~~~ 

@subsection occt_draw_4_5_4 meshshadcolor

                  
~~~~~
meshshadcolor meshname red green blue
~~~~~ 

Changes the face interior color of object **meshname**. The *red*, *green* and *blue* are real values between *0* and *1*.
 

~~~~~
vinit 
meshfromstl mesh myfile.stl 
meshshadcolormode mesh 0.5 0.5 0.5
~~~~~ 

@subsection occt_draw_4_5_5 meshlinkcolor

                  
~~~~~
meshlinkcolor meshname red green blue
~~~~~ 

Changes the color of face borders for object **meshname**. The *red*, *green* and *blue* are real values between *0* and *1*.
 

~~~~~
vinit 
meshfromstl mesh myfile.stl 
meshlinkcolormode mesh 0.5 0.5 0.5
~~~~~ 

@subsection occt_draw_4_5_6 meshmat

                  
~~~~~
meshmat meshname material
~~~~~ 

Changes the material of object **meshname**.

*material* is represented with an integer value as follows (equivalent to enumeration *Graphic3d_NameOfMaterial*): 
* *0 -- BRASS,* 
* *1 -- BRONZE,* 
* *2 -- COPPER,* 
* *3 -- GOLD,* 
* *4 -- PEWTER,* 
* *5 -- PLASTER,* 
* *6 -- PLASTIC,* 
* *7 -- SILVER,* 
* *8 -- STEEL,* 
* *9 -- STONE,* 
* *10 -- SHINY_PLASTIC,* 
* *11 -- SATIN,*
* *12 -- METALIZED,* 
* *13 -- NEON_GNC,* 
* *14 -- CHROME,*
* *15 -- ALUMINIUM,*
* *16 -- OBSIDIAN,* 
* *17 -- NEON_PHC,* 
* *18 -- JADE,*
* *19 -- DEFAULT,* 
* *20 -- UserDefined*
 

~~~~~
vinit 
meshfromstl mesh myfile.stl 
meshmat mesh JADE 
~~~~~

@subsection occt_draw_4_5_7 meshshrcoef

                  
~~~~~
meshshrcoef meshname shrinkcoefficient
~~~~~ 

Changes the value of shrink coefficient used in the shrink mode. In the shrink mode the face is shown as a congruent part of a usual face, so that *shrinkcoefficient* controls the value of this part. The *shrinkcoefficient* is a positive real number.
 

~~~~~
vinit 
meshfromstl mesh myfile.stl 
meshshrcoef mesh 0.05
~~~~~ 

@subsection occt_draw_4_5_8 meshshow

                  
~~~~~
meshshow meshname
~~~~~ 

Displays **meshname** in the viewer (if it is erased).
 

~~~~~
vinit 
meshfromstl mesh myfile.stl 
meshshow mesh
~~~~~ 

@subsection occt_draw_4_5_9 meshhide

                  
~~~~~
meshhide meshname
~~~~~ 

Hides **meshname** in the viewer. 


~~~~~
vinit 
meshfromstl mesh myfile.stl 
meshhide mesh
~~~~~ 

@subsection occt_draw_4_5_10 meshhidesel

                  
~~~~~
meshhidesel meshname
~~~~~ 

Hides only selected entities. The other part of **meshname** remains visible. 

@subsection occt_draw_4_5_11 meshshowsel

                  
~~~~~
meshshowsel meshname
~~~~~ 

Shows only selected entities. The other part of **meshname** becomes invisible. 

@subsection occt_draw_4_5_12 meshshowall

                  
~~~~~
meshshowall meshname
~~~~~ 

Changes the state of all entities to visible for **meshname**. 

@subsection occt_draw_4_5_13 meshdelete

                  
~~~~~
meshdelete meshname
~~~~~ 

Deletes MeshVS_Mesh object **meshname**. 


~~~~~
vinit 
meshfromstl mesh myfile.stl 
meshdelete mesh 
~~~~~

@section graphic_vis VIS Viewer
A specific plugin with alias *VIS* should be loaded to have access to VIS functionality in DRAW Test Harness:

~~~~
\> pload VIS
~~~~

@subsection occt_draw_4_6_1	ivtkinit


~~~~~
ivtkinit
~~~~~

Creates a window for VTK viewer.

@figure{/user_guides/draw/images/draw_image001.png}

@subsection occt_draw_4_6_2	ivtkdisplay


~~~~~
ivtkdisplay name1 [name2] …[name n]
~~~~~

Displays named objects.


~~~~~
ivtkinit
# create cone
pcone c 5 0 10
ivtkdisplay c
~~~~~

@figure{/user_guides/draw/images/draw_image002.png}

@subsection occt_draw_4_6_3	ivtkerase


~~~~~
ivtkerase [name1] [name2] … [name n]
~~~~~

Erases named objects. If no arguments are passed, erases all displayed objects.

**Example:**
~~~~~
ivtkinit
# create a sphere
psphere s 10
# create a cone
pcone c 5 0 10
# create a cylinder
pcylinder cy 5 10
# display objects
ivtkdisplay s c cy
# erase only the cylinder
ivtkerase cy
# erase the sphere and the cone
ivtkerase s c
~~~~~

@subsection occt_draw_4_6_4	 ivtkfit


~~~~~
ivtkfit
~~~~~

Automatic zoom/panning.

@subsection occt_draw_4_6_5	ivtkdispmode


~~~~~
ivtksetdispmode [name] {0|1}
~~~~~

Sets display mode for a named object. If no arguments are passed, sets the given display mode for all displayed objects
The possible modes are: 0 (WireFrame) and 1 (Shading).

**Example:**
~~~~~
ivtkinit
# create a cone
pcone c 5 0 10
# display the cone
ivtkdisplay c
# set shading mode for the cone
ivtksetdispmode c 1
~~~~~

@figure{/user_guides/draw/images/draw_image003.png}
 
@subsection occt_draw_4_6_6	ivtksetselmode


~~~~~
ivtksetselmode [name] mode {0|1}
~~~~~

Sets selection mode for a named object. If no arguments are passed, sets the given selection mode for all the displayed objects.

**Example:**
~~~~~
ivtkinit
# load a shape from file
restore CrankArm.brep a
# display the loaded shape
ivtkdisplay a
# set the face selection mode
ivtksetselmode a 4 1
~~~~~

@figure{/user_guides/draw/images/draw_image004.png}
 
@subsection occt_draw_4_6_7	ivtkmoveto


~~~~~
ivtkmoveto x y
~~~~~

Imitates mouse cursor moving to point with the given display coordinates **x**,**y**.

**Example:**
~~~~~
ivtkinit
pcone c 5 0 10
ivtkdisplay c
ivtkmoveto 40 50
~~~~~

@subsection occt_draw_4_6_8	ivtkselect


~~~~~
ivtkselect x y
~~~~~

Imitates mouse cursor moving to point with the given display coordinates and performs selection at this point.

**Example:**
~~~~~
ivtkinit
pcone c 5 0 10
ivtkdisplay c
ivtkselect 40 50
~~~~~

@subsection occt_draw_4_6_9	ivtkdump


~~~~~
ivtkdump *filename* [buffer={rgb|rgba|depth}] [width height] [stereoproj={L|R}]
~~~~~

Dumps the contents of VTK viewer to image. It supports:
* dumping in different raster graphics formats: PNG, BMP, JPEG, TIFF or PNM.
* dumping of different buffers: RGB, RGBA or depth buffer.
* defining of image sizes (width and height in pixels).
* dumping of stereo projections (left or right).

**Example:**
~~~~~
ivtkinit
pcone c 5 0 10
ivtkdisplay c
ivtkdump D:/ConeSnapshot.png rgb 768 768
~~~~~

@subsection occt_draw_4_6_10	ivtkbgcolor



~~~~~
ivtkbgcolor r g b [r2 g2 b2]
~~~~~

Sets uniform background color or gradient background if second triple of parameters is set. Color parameters r,g,b have to be chosen in the interval  [0..255].

**Example:**
~~~~~
ivtkinit
ivtkbgcolor 200 220 250
~~~~~
 
@figure{/user_guides/draw/images/draw_image005.png}

~~~~~
ivtkbgcolor 10 30 80 255 255 255
~~~~~

@figure{/user_guides/draw/images/draw_image006.png}



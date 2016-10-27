Introduction {#mainpage}
============

@tableofcontents

@htmlonly<center>@endhtmlonly 
@image html /resources/occt_logo.png
@image latex /resources/occt_logo.png
@htmlonly</center>@endhtmlonly

Welcome to Open CASCADE Technology (OCCT), a software development platform 
providing services for 3D surface and solid modeling, CAD data exchange, and 
visualization. Most of OCCT functionality is available in the form of C++ 
libraries. OCCT can be best applied in development of software dealing with 3D 
modeling (CAD), manufacturing / measuring (CAM) or numerical simulation (CAE).

@section intro_legal Legal Notes

Open CASCADE Technology is free software; you can redistribute it and / or 
modify it under the terms of the @ref license_lgpl_21 "GNU Lesser General Public License (LGPL) version 2.1", 
with additional @ref occt_lgpl_exception "exception".

Alternatively, Open CASCADE Technology may be used under the terms of Open CASCADE commercial license or contractual agreement.

Note that Open CASCADE Technology is provided on an "AS IS" basis, WITHOUT WARRANTY OF ANY KIND. The entire risk 
related to any use of the OCCT code and materials is on you. See the @ref occt_public_license "license" text for formal 
disclaimer.

Open CASCADE Technology and all materials, including this documentation, is Copyright (c) 1999-2016 by OPEN CASCADE S.A.S. 
All rights reserved. You are hereby informed that all software is a property of its respective authors and is protected by 
international and domestic laws on intellectual property and trademarks. **CAS.CADE** and **Open CASCADE** are 
registered trademarks of OPEN CASCADE S.A.S.

@htmlonly<center>@endhtmlonly
@image html /resources/occ_logo.png
@image latex /resources/occ_logo.png
@htmlonly</center>@endhtmlonly

Should you need further information, please directly contact the authors http://www.opencascade.com.

@section intro_overview OCCT Modules Overview

OCCT is an object-oriented C++ class library designed for rapid production of sophisticated
 domain-specific CAD/CAM/CAE applications. 

A typical application developed using OCCT deals with two or three-dimensional (2D or 3D) geometric modeling
in general-purpose or specialized Computer Aided Design (CAD) systems, manufacturing
or analysis applications, simulation applications, or even illustration tools. 

@figure{/01_introduction/images/over.png}

OCCT library is designed to be truly modular and extensible, providing C++ classes for:
  * Basic data structures (geometric modeling, visualization, interactive selection and application specific services); 
  * Modeling algorithms; 
  * Working with mesh (faceted) data; 
  * Data interoperability with neutral formats (IGES, STEP); 
  * Shape healing

The C++ classes and other types are grouped into packages. Packages are organized into toolkits (libraries), to which
 you can link your application. Finally, toolkits are grouped into seven modules.

This modular structure is illustrated in the diagram below.

@figure{/01_introduction/images/schema.png}

* @ref intro_ovw_foundation module underlies all other OCCT classes; 
* @ref intro_ovw_modeling_data module supplies data structures to represent 2D and 3D geometric primitives and their
 compositions into CAD models; 
* @ref intro_ovw_modeling_algo module contains a vast range of geometrical and topological algorithms;
* @ref intro_ovw_mesh module implements tessellated  representations of objects;
* @ref intro_ovw_visu module provides complex mechanisms for graphical data representation including tessellated 
representations of objects;
* @ref intro_ovw_data_exchange module inter-operates with popular data formats and relies on
 \ref anchor_intro_ovw_shape_healing "Shape Healing Algorithms" to improve compatibility between CAD software 
of different vendors;
* @ref intro_ovw_ocaf module offers ready-to-use solutions for handling application-specific data (user attributes) and
 commonly used functionality (save/restore, undo/redo, copy/paste, tracking CAD modifications, etc). 
* @ref intro_ovw_draw

@subsection intro_ovw_foundation Foundation Classes

**Foundation Classes** module contains data structures and services used by higher-level Open CASCADE Technology classes:

  * Primitive types, such as Boolean, Character, Integer or Real;
  * String classes that handle ASCII and Unicode strings;
  * Collection classes that handle statically or dynamically sized aggregates of data, such as arrays, lists, queues, 
sets and hash tables (data maps).
  * Classes providing commonly used numerical algorithms and basic linear algebra calculations (addition, multiplication,
 transposition of vectors and matrices, solving linear systems etc).
  * Fundamental types representing physical quantities and supporting date and time information;
  * Primitive geometry types providing implementation of basic geometric and algebraic entities that define and 
manipulate elementary data structures. 
  * Exception classes that describe situations, when the normal execution of program is abandoned;

This module also provides a variety of general-purpose services, such as:
  * Safe handling of dynamically created objects, ensuring automatic deletion of unreferenced objects (smart pointers);
  * Configurable optimized memory manager increasing the performance of applications that intensively use dynamically 
created objects;
  * Extended run-time type information (RTTI) mechanism maintaining a full type hierarchy and providing means to iterate 
over it;
  * Encapsulation of C++ streams;
  * Automated management of heap memory by means of specific allocators;
  * Basic interpreter of expressions facilitating the creation of customized scripting tools, generic definition of 
expressions, etc.;
  * Tools for dealing with configuration resource files and customizable message files facilitating  multi-language 
support in applications;
  * Progress indication and user break interfaces, giving a possibility even for low-level algorithms to communicate 
with the user in a universal and convenient way;
  * and many others...

Please, see the details in @ref user_guides__foundation_classes "Foundation Classes User's Guide"

See also: our <a href="http://www.opencascade.com/content/tutorial-learning">E-learning & Training</a> offerings.

@subsection intro_ovw_modeling_data Modeling Data

**Modeling Data** supplies data structures to implement boundary representation (BRep) of objects in 3D. In BRep the
 shape is represented as an aggregation of geometry within topology. The geometry  is understood as a mathematical 
description of a shape, e.g. as curves and surfaces (simple or canonical, Bezier, NURBS, etc). The topology is a data 
structure binding geometrical objects together.

Geometry types and utilities provide geometric data structures and services for:
 * Description of points, vectors, curves and surfaces:
	 * their positioning in 3D space using axis or coordinate systems, and
	 * their geometric transformation, by applying translations, rotations, symmetries, scaling transformations and 
combinations thereof.
 * Creation of parametric curves and surfaces by interpolation and approximation;
 * Algorithms of direct construction; 
 * Conversion of curves and surfaces to NURBS form;
 * Computation of point coordinates on 2D and 3D curves; 
 * Calculation of extrema between geometric objects. 
  
Topology defines relationships between simple geometric entities.  A shape, which is a basic topological entity, can be
 divided into components (sub-shapes):
 * Vertex -- a zero-dimensional shape corresponding to a point;
 * Edge -- a shape corresponding to a curve and bounded by a vertex at each extremity;
 * Wire -- a sequence of edges connected by their vertices;
 * Face -- a part of a plane (in 2D) or a surface (in 3D) bounded by wires;
 * Shell -- a collection of faces connected by edges of their wire boundaries;
 * Solid -- a finite closed part of 3D space bounded by shells;
 * Compound solid -- a collection of solids connected by faces of their shell boundaries.

Complex shapes can be defined as assemblies of simpler entities.

Please, see the details in @ref user_guides__modeling_data "Modeling Data User's Guide"

See also: our <a href="http://www.opencascade.com/content/tutorial-learning">E-learning & Training</a> offerings.

@subsection intro_ovw_modeling_algo Modeling Algorithms

**Modeling Algorithms** module groups a wide range of topological and geometric algorithms used in geometric modeling. 
Basically, there are two groups of algorithms in Open CASCADE Technology:

* High-level modeling routines used in the real design;<br><br>

* Low-level mathematical support functions used as a groundwork for the modeling API;<br><br>

* Low-level geometric tools provide the algorithms, which:
	* Calculate the intersection of two curves, surfaces, or a curve and a surface;
	* Project points onto 2D and 3D curves, points onto surfaces and 3D curves onto surfaces;
	* Construct lines and circles from constraints;
	* Construct free-form curves and surfaces from constraints (interpolation, approximation, skinning, gap filling, 
etc);<br><br>
  
* Low-level topological tools provide the algorithms, which:
	* Tessellate shapes; 
	* Check correct definition of shapes; 
	* Determine the local and global properties of shapes (derivatives, mass-inertia properties, etc); 
	* Perform affine transformations;
	* Find planes in which edges are located;
	* Convert shapes to NURBS geometry;
	* Sew connected topologies (shells and wires) from separate topological elements (faces and edges).

Top-level API provides the following functionality: 

* Construction of Primitives:  
	* Boxes;
	* Prisms;
	* Cylinders;
	* Cones;
	* Spheres;
	* Toruses.<br><br>

* Kinematic Modeling: 
	* Prisms -- linear sweeps;
	* Revolutions -- rotational sweeps;
	* Pipes -- general-form sweeps;
	* Lofting.

@figure{/01_introduction/images/0001.png "Shapes containing pipes with variable radius produced by sweeping"}  

* <b>Boolean Operations</b>, which allow creating new shapes from the combinations of source shapes. For two shapes *S1*
 and *S2*:
  * *Common* contains all points that are in *S1* and *S2*;
  * *Fuse* contains all points that are in *S1* or *S2*;
  * *Cut* contains all points in that are in *S1* and not in *S2*
   
* Algorithms for local modifications such as: 
  * Hollowing; 
  * Shelling; 
  * Creation of tapered shapes using draft angles;
  * Algorithms to make fillets and chamfers on shape edges, including those with variable radius (chord). <br><br>

* Algorithms for creation of mechanical features, i.e. depressions, protrusions, ribs and grooves or slots along planar
 or revolution surfaces. 

@figure{/01_introduction/images/0004.png}  
 
\anchor anchor_intro_ovw_shape_healing
* <b>Shape Healing</b>, algorithms to correct and adapt the geometry and topology of shapes imported to OCCT from other
 CAD systems. <br><br>

    * analyze shape characteristics and, in particular, identify the shapes that do not comply with OCCT geometry and
     topology  validity rules by analyzing geometrical objects and topology: 
	    - check edge and wire consistency;
	    - check edge order in a wire;
	    - check the orientation of face boundaries;
	    - analyze shape tolerances;
	    - identify closed and open wires in a boundary.<br><br>

    * fix incorrect or incomplete shapes:
	    - provide consistency between a 3D curve and its corresponding parametric curve;
	    - repair defective wires;
	    - fit the shapes to a user-defined tolerance value;
	    - fill gaps between patches and edges. <br><br>

    * upgrade and change shape characteristics:
	    - reduce curve and surface degree;
	    - split shapes to obtain C1 continuity;
	    - convert any types of curves or surfaces to Bezier or B-Spline curves or surfaces and back;
	    - split closed surfaces and revolution surfaces.

Each sub-domain of Shape Healing has its own scope of functionality:

| Sub-domain | Description | Impact on the shape |
| :--- | :---- | :---- |
| Analysis | Explores shape properties, computes shape features, detects violation of OCCT requirements. | The shape itself is not modified. |
| Fixing  | Fixes the shape to meet the OCCT requirements. | The shape may change its original form: modification, removal or creation of sub-shapes, etc.) |
| Upgrade | Improves the shape to fit some particular algorithms. | The shape is replaced with a new one, but geometrically they are the same. |
| Customization | Modifies the shape representation to fit specific needs. | The shape is not modified, only the mathematical form of its internal representation is changed. |
| Processing | Mechanism of shape modification via a user-editable resource file. | |

Please, see the details in @ref user_guides__modeling_algos "Modeling Algorithms User's Guide".

See also: our <a href="http://www.opencascade.com/content/tutorial-learning">E-learning & Training</a> offerings.

@subsection intro_ovw_mesh Mesh 

**Mesh** module provides the functionality to work with tessellated  representations of objects in form of triangular facets. This module contains:
- data structures to store surface mesh data associated to shapes and basic algorithms to handle them;
- data structures and algorithms to a build triangular surface mesh from *BRep* objects (shapes);
- tools for displaying meshes with associated pre- and post-processor data (scalars or vectors).

Open CASCADE Technology includes two mesh converters:
- VRML converter translates Open CASCADE shapes to VRML 1.0 files (Virtual Reality Modeling Language). Two representation modes are possible: shaded, which presents shapes as sets of triangles computed by the mesh algorithm, or wireframe, which presents shapes as sets of curves.
- STL converter translates Open CASCADE shapes to STL files. STL (STtereoLithography) format is widely used for rapid prototyping (3D printing).

Open CASCADE SAS also offers Advanced Mesh Products:
- <a href="http://www.opencascade.com/content/mesh-framework">Open CASCADE Mesh Framework (OMF)</a>
- <a href="http://www.opencascade.com/content/express-mesh">Express Mesh</a>

@figure{/01_introduction/images/0003.png}

@note Currently, <i>Mesh</i> is not a separate module inside OCCT library. All related functionality can be found in 
Modeling Data, Modeling Algorithms and Data Exchange modules.

@subsection intro_ovw_visu Visualization

**Visualization** module provides ready-to-use algorithms to create graphic presentations from various objects: shapes, 
meshes, etc. 

In Open CASCADE Technology visualization is based on the separation of CAD data and its graphical presentation. 
The presentations can be customized to take the specificity of your application into account.

The module also supports a fast and powerful interactive selection mechanism. 

The view facilities provided by OCCT range from low-level tools working with basic geometry and topology (such as NURBS 
visualization with control points and nodes, rendering of isolines to estimate speed and quality of parameterization, or
 rendering of a parametric profile of edges) to high-level tools for real time quality rendering of models using ray 
tracing: shades, reflections, transparency, anti-aliasing, etc.

Here are just a few examples:

* Camera-driven view projection and orientation. It is possible to choose between perspective, orthographic and 
stereographic projection. <br><br>

* Real-time ray tracing technique using recursive Whitted's algorithm and Bounded Volume Hierarchy effective
 optimization structure. 

@figure{/01_introduction/images/0002.png, "Real time visualization by ray tracing method"}

* Support of GLSL shaders. The shader management is fully automatic, like with any other OpenGL resource.

@figure{/01_introduction/images/0013.png, "Fragment shader implementing custom clipping surface"}

* Support of standard and custom materials, defined by transparency, diffuse, ambient and specular reflection and 
refraction index. The latter allows implementing transparent materials, such as glass, diamond and water. 

@figure{/01_introduction/images/0012.png, "Simulation of a glass cover"}

* Optimization of rendering performance through the algorithms of: 
	* View frustum culling, which skips the presentation outside camera at the rendering stage and  <br><br>
	* Back face culling, which reduces the rendered number of triangles and eliminates artifacts at shape boundaries.<br><br> 
	
* Definition of clipping planes through the plane equation coefficients. Ability to define visual attributes for
 cross-section at the level or individual clipping planes. In the image below different parts of the rocket are clipped 
with different planes and hatched.<br><br>

* Possibility to flexibly adjust appearance of dimensions in a 3D view. The 3D text object represents a given text 
string as a true 3D object in the model space.<br><br>

@figure{/01_introduction/images/0008.png, "Display of shape cross-section and dimensions"}

* The visualization of OCCT topological shapes by means of VTK library provided by VIS component

For more details see @ref user_guides__visualization "Visualization User's Guide".

See also: our <a href="http://www.opencascade.com/content/tutorial-learning">E-learning & Training</a> offerings.

@subsection intro_ovw_data_exchange Data Exchange

**Data Exchange** allows developing OCCT-based applications that can interact with other CAD systems by writing and 
reading CAD models to and from external data. The exchanges run smoothly regardless of the quality of external data or
 requirements to its internal representation, for example, to the data types, accepted geometric inaccuracies, etc.

@figure{/01_introduction/images/0014.png,"Shape imported from STEP"}

**Data Exchange** is organized in a modular way as a set of interfaces that comply with various CAD formats: IGES, STEP,
 STL, VRML, etc. The interfaces allow software based on OCCT to exchange data with various CAD/PDM software packages, 
maintaining a good level of interoperability.

* **Standardized Data Exchange** interfaces allow querying and examining the input file, converting its contents to a 
CAD model and running validity checks on a fully translated shape. The following formats are currently supported.<br>
	* "STEP" (AP203 : Mechanical Design, this covers General 3D CAD; AP214: Automotive Design) 
	* "IGES" (up to 5.3) 
	* VRML and STL meshes. <br><br>

* "Extended data exchange" (XDE) allows translating additional attributes attached to geometric 
data (colors, layers, names, materials etc).<br><br>

<a href="http://www.opencascade.com/content/advanced-data-exchange-components">Advanced Data Exchange Components</a> 
are available in addition to standard Data Exchange interfaces to support interoperability and data adaptation 
(also using \ref anchor_intro_ovw_shape_healing "Shape Healing Algorithms") with CAD software using the following 
proprietary formats:<br>
	* <a href="http://www.opencascade.com/content/acis-sat-import-export">ACIS SAT</a>
	* <a href="http://www.opencascade.com/content/parasolid-import">Parasolid</a>
	* <a href="http://www.opencascade.com/content/dxf-import-export">DXF</a> 

These components are based on the same architecture as interfaces with STEP and IGES.

For more details see @ref user_guides__data_exchange chapter.

@subsection intro_ovw_ocaf Application Framework

**Open CASCADE Application Framework** (OCAF) handles Application Data basing on the Application/Document paradigm. 
It uses an associativity engine to simplify the development of a CAD application thanks to the following ready-to-use
 features and services:

* Data attributes managing the application data, which can be organized according to the development needs; 
* Data storage and persistence (open/save); 
* Possibility to modify and recompute attributes in documents. With OCAF it is easy to represent the history of 
modification and parametric dependencies within your model;
* Possibility to manage multiple documents;  
* Predefined attributes common to CAD/CAM/CAE applications (e.g. to store dimensions);
* Undo-Redo and Copy-Paste functions.

Since OCAF handles the application structure, the only development task is the creation of application-specific data
 and GUIs. 

OCAF differs from any other CAD framework in the organization of application data, as there the data structures are 
based on reference keys rather than on shapes. In a model, such attributes as shape data, color and material are 
attached to an invariant structure, which is deeper than the shapes. A shape object becomes the value of *Shape* 
attribute, in the same way as an integer number is the value of *Integer* attribute and a string is the value of 
*Name* attribute.

OCAF organizes and embeds these attributes in a document. OCAF documents, in their turn, are managed by an OCAF
 application.

For more details see @ref user_guides__ocaf "OCAF User's Guide". 

See also: our <a href="http://www.opencascade.com/content/tutorial-learning">E-learning & Training</a> offerings.

@subsection intro_ovw_draw Draw

**Draw** is a convenient testing tool for OCCT libraries. It can be used to test and prototype various algorithms before 
building an entire application. It includes:
- A command interpreter based on the TCL language;
- A number of 2D and 3D viewers;
- A set of predefined commands.

The viewers support operations such as zoom, pan, rotation and full-screen views.

The basic commands provide general-purpose services such as:
- Getting help;
- Evaluating a script from a file;
- Capturing commands in a file;
- Managing views;
- Displaying objects.

In addition, **Draw** provides commands to create and manipulate curves and surfaces (geometry) and shapes, access 
visualization services, work with OCAF documents, perform data exchange, etc.

You can add custom commands to test or demonstrate any new functionalities, which you develop.

For more details see @ref user_guides__draw "Draw Manual".

@section intro_requirements Requirements

Open CASCADE Technology is designed to be highly portable and is known to 
work on wide range of platforms (UNIX, Linux, Windows, Mac OS X, Android). 
Current version is officially certified on Windows (IA-32 and x86-64), 
Linux (x86-64), MAC OS X (x86-64) and Android (4.0.4 armv7) platforms. 

The tables below describe the recommended hardware and software configurations
for which OCCT is certified to work.

##Supported OS

| OS        | Versions |
| --------- | ----------- |
| Windows   | 10, 8.1, 7 SP1, Vista SP2 |
| Linux     | Mandriva 2010, CentOS 6.3, Fedora 18, Ubuntu 14.10 - 15.10, Debian 6.0, Debian 7.0 |
| OS X      | 10.10 Yosemite / 10.9 Mavericks / 10.8 Mountain Lion / 10.7 Lion |
| Android   | 6.x, 5.x, 4.0.4+ |
| iOS       | iOS 7 |

##Compiler / IDE

| OS        | Compiler |
| --------- | ----------- |
| Windows   | Microsoft Visual Studio: 2010 SP1<sup>1</sup>, 2012 Update 4, 2013 Update 5, 2015 <br> Intel C++ Composer XE 2013 SP1 <br> GCC 4.3+ (Mingw-w64)|
| Linux     | GNU gcc 4.3+ <br> LLVM CLang 3.6+ |
| OS X      | XCode 6 or newer |
| Android   | NDK r10, GNU gcc 4.8 or newer |

1) VC++ 10 64-bit is used for regular testing and for building 
  binary package of official release of OCCT on Windows.
  
\anchor anchor_req_3rdparty
##Third-party libraries

| Component | Requirement |
| --------- | ----------- |
| Graphic library | OpenGL 3.3+, OpenGL ES 2.0+ <br> Direct3D 9 |
| Qt (for samples and demos) | Desktop: Qt 4.8.6+ http://www.qt.io/download/ <br> Android: Qt 5.3.2+ http://www.qt.io/download/ |
| TCL (for testing tools)    | Tcl/Tk 8.6.3+ http://www.tcl.tk/software/tcltk/download.html <br> or ActiveTcl 8.6 http://www.activestate.com/activetcl/downloads (for Windows)| 
| Freetype (for text rendering) | FreeType 2.4.11-2.5.5 http://sourceforge.net/projects/freetype/files/ |
| FreeImage (optional, for support of common 2D graphic formats) | FreeImage 3.17.0+ http://sourceforge.net/projects/freeimage/files |
| gl2ps (optional, for export contents of OCCT viewer to vector formats) | gl2ps-1.3.8+  http://geuz.org/gl2ps/ |
| Intel TBB (optional, for multithreaded algorithms) | TBB 4.x or 5.x http://www.threadingbuildingblocks.org/ |
| VTK (for VTK Integration Services | VTK 6.1+ http://www.vtk.org/VTK/resources/software.html |
| Doxygen (optional for building documentation) | Doxygen 1.8.5+ http://www.stack.nl/~dimitri/doxygen/download.html |

The easiest way to install third-party libraries is to download archive with pre-built binaries, corresponding to
 version of Visual Studio you are using, from http://www.opencascade.com/content/3rd-party-components.

You can also build third-party libraries from their sources, see @ref build_upgrade__building_3rdparty for instructions.

##Hardware

| Component | Requirement |
| --------- | ----------- |
| Minimum memory    | 512 MB, 1 GB recommended |
| Free disk space (complete installation) | 600 MB approx. |

On desktop, 3D viewer for optimal performance requires graphics processing unit (GPU) supporting OpenGL 3.3 or above. 
Ray tracing requires OpenGL 4.0+ or OpenGL 3.3+ with *GL_ARB_texture_buffer_object_rgb32* extension.
Textures within ray tracing will be available only when *GL_ARB_bindless_texture extension* is provided by driver.

On mobile platforms, OpenGL ES 2.0+ is required for 3D viewer. The ray tracing is not yet available on mobile platforms.
Some old hardware might be unable to execute complex GLSL programs (e.g. with high number of light sources, clipping
 planes).

The following table lists graphic cards tested to work with OCCT.

| Graphic card  | Driver | OS | OpenGL (fixed pipeline) | OpenGL (shaders) | OpenGL (ray tracing) |
| ---- | ---- | ---- | :----: | :----: | :----: |
| NVIDIA GeForce GTX 650     | Driver 340.52, OpenGL 4.4        | Windows 7 64 bit           | OK | OK  | OK |
| AMD/ATI RadeOn HD 7870     | Driver 14.100, OpenGL 4.4        | Windows 7 64-bit           | OK | OK  | OK |
| Intel(R) HD Graphics 2500  | Driver 10.18.10.3621, OpenGL 4.0 | Windows 7 64 bit           | OK | OK  | limited (no textures) |
| NVIDIA GeForce 320         | N/A                              | Mac OS X 10.6 / OS X 10.10 | OK | OK  | not yet supported by OCCT |
| Apple software OpenGL      | N/A                              | Mac OS X 10.6 / OS X 10.10 | OK | OK  | N/A |
| Mesa 10.2.4 (software emulator) | "Gallium 0.4 on llvmpipe (LLVM 3.4, 256 bits)" OpenGL 3.0 | Windows 7 64 bit   | OK | OK  | unsupported by software |

@section intro_install Installation on Windows

In most cases you need to rebuild OCCT on your platform (OS, compiler) before
using it in your project, to ensure binary compatibility.
See @ref build_upgrade__building_occt for instructions on
building OCCT from sources on supported platforms.

On Windows Open CASCADE Technology can be installed with binaries precompiled by 
Visual C++ 2010 with installation procedure using Windows installer.

##Recommendation:

If you have a previous version of OCCT installed on your station, 
and you do not plan to use it along with the new version, you might want to uninstall 
the previous version (using Control Panel, Add/Remove Programs) before 
the installation of this new version, to avoid possible problems 
(conflict of system variables, paths, etc).

##Attention:
For full installation OCCT requires approximately 650 Mb of disk space, 
but during the installation process you will need 1,2 Gb of free disk space.

OCCT installation with reference documentation requires 1,4 Gb on disk.

  * Download the OCCT installer from OPEN CASCADE web site using the link. you have been provided
  * Launch the installer and follow the instructions.

The includes and binaries of third-party libraries necessary for building and launching 
OCCT are included into binary distribution (built with Visual C++ 2010). 
When the installation is complete, you will find the directories for 3rd party products 
(some might be absent in case of custom installation) and the main **OCCT** directory:

@image html /01_introduction/images/3rdparty.png 
@image latex /01_introduction/images/3rdparty.png 

The contents of the OCCT-7.0.0 directory (called further "OCCT root", or $CASROOT) are as follows:

@image html /01_introduction/images/installation.png
@image latex /01_introduction/images/installation.png

  * **adm**   This folder contains administration files, which allow rebuilding OCCT;
  * **adm/cmake**  This folder contains files of CMake building procedure;
  * **adm/msvc**  This folder contains Visual Studio projects for Visual C++  2005, 2008, 2010, 2012 and 2013 which 
allow rebuilding OCCT under Windows platform in 32 and 64-bit mode;
  * **data**  This folder contains CAD files in different formats, which can be used to test the OCCT functionality;
  * **doc**  This folder contains OCCT documentation in HTML and PDF format;
  * **dox**  This folder contains sources of OCCT documentation in plain text (MarkDown) format;
  * **inc**  This folder contains copies of all OCCT header files;
  * **samples**  This folder contains sample applications.
  * **src**  This folder contains OCCT source files. They are organized in folders, one per development unit;
  * **tests**  This folder contains scripts for OCCT testing.
  * **win64/vc10**  This folder contains executable and library files built in optimize mode for Windows platform by
 Visual C++  2010;
  
@section into_offline_doc Generate Offline Documentation

To generate HTML documentation from sources contained in *dox* subdirectory, 
you need to have Tcl and Doxygen 1.8.5 (or above) installed on your system.

Use script **gendoc** (batch file on Windows, shell script on Linux / Mac OSX) to generate documentation.

To generate Overview documentation:

    cmd> gendoc -overview

To generate Reference manual:

    cmd> gendoc -refman

Run this command without arguments to get help on supported options.

See @ref contribution__documentation for prerequisites and details on OCCT documentation system.

@section intro_acknowledgment Acknowledgments 

The following parties are acknowledged for producing tools which are used within 
Open CASCADE Technology libraries or for release preparation.

You are hereby informed that all rights to the software listed below belong to its respective 
authors and such software may not be freely available and/or be free of charge for any kind 
of use or purpose. We strongly recommend that you carefully read the license of these products 
and, in case you need any further information, directly contact their authors.

**Qt** is a cross-platform application framework that is widely used for developing application software 
with graphical user interface (GUI). Qt is free and open source software distributed under 
the terms of the GNU Lesser General Public License. In OCCT Qt is used for programming samples. 
If you need further information on Qt, please, refer to Qt Homepage (http://www.qt.io/)

**Tcl** is a high-level programming language. Tk is a graphical user interface (GUI) toolkit, 
with buttons, menus, listboxes, scrollbars, and so on. Taken together Tcl and Tk provide a solution 
to develop cross-platform graphical user interfaces with a native look and feel. Tcl/Tk is under copyright by 
Scriptics Corp., Sun Microsystems, and other companies. However, Tcl/Tk is an open source, and 
the copyright allows you to use, modify, and redistribute Tcl/Tk for any purpose, without an 
explicit license agreement and without paying any license fees or royalties. 
To use Tcl/Tk, please refer to the Licensing Terms (http://www.tcl.tk/software/tcltk/license.html).

**GL2PS** is developed by Christophe Geuzaine and others. It is optionally used by OCCT to
export content of OpenGL scene to vector graphics formats (PS, PDF, EMF, SVG). 
The library is licensed under GL2PS LICENSE http://www.geuz.org/gl2ps/COPYING.GL2PS Version 2, November 2003.

**FreeType 2** is developed by Antoine Leca, David Turner, Werner Lemberg and others. 
It is a software font engine that is designed to be small, efficient, highly customizable and 
portable while capable of producing high-quality output (glyph images). This product 
can be used in graphic libraries, display servers, font conversion tools, 
text image generation tools, and many other products.

FreeType 2 is released under two open-source licenses: BSD-like FreeType License and the GPL.

**Intel(R) Threading Building Blocks (TBB)** offers a rich and complete approach to expressing parallelism in a C++ 
program. 
It is a library that helps you to take advantage of multi-core processor performance without having to be a threading 
expert. 
Threading Building Blocks is not just a threads-replacement library. It represents a higher-level, task-based 
parallelism that 
abstracts platform details and threading mechanisms for scalability and performance. 
TBB is available under GPLv2 license with the runtime exception.

**OpenGL** is an industry standard API for 3D graphics used by OCCT for 
implementation of 3D viewer. OpenGL specification is developed by the
Khronos group, http://www.khronos.org/opengl/. OCCT code includes header 
file *glext.h* obtained from Khronos web site.

**VTK** -- The **Visualization Toolkit (VTK)** is an open-source, freely available software system for 3D computer 
graphics, image processing and visualization. OCCT VIS component provides adaptation functionality for visualization of
 OCCT topological shapes by means of VTK library. If you need further information on VTK, please, refer to
<a href="http://www.vtk.org/">VTK Homepage</a>.

**Doxygen** developed by Dimitri van Heesch is open source documentation system for 
C++, C, Java, Objective-C, Python, IDL, PHP and C#. This product is used in Open CASCADE Technology 
for automatic creation of Technical Documentation from C++ header files. 
If you need further information on Doxygen, please refer to http://www.stack.nl/~dimitri/doxygen/index.html.

**Graphviz** is open source graph visualization software developed by John Ellson, Emden Gansner, Yifan Hu and Arif Bilgin. 
Graph visualization is representiation of structured information as diagrams of abstract graphs and networks. 
This product is used together with Doxygen in Open CASCADE Technology for automatic creation of Technical Documentation 
(generation of dependency graphs). Current versions of Graphviz are licensed on an open source 
basis under The Eclipse Public License (EPL) (http://www.graphviz.org/License.php).

**Inno Setup** is a free script-driven installation system created in CodeGear Delphi by Jordan Russell. 
In OCCT Inno Setup is used to create Installation Wizard on Windows. 
It is licensed under Inno Setup License (http://www.jrsoftware.org/files/is/license.txt).

**FreeImage** is an Open Source library supporting popular graphics image formats, such as PNG, BMP, JPEG, TIFF, 
and others used by multimedia applications. This library is developed by Hervé Drolon and Floris van den Berg. 
FreeImage is easy to use, fast, multithreading safe, compatible with all 32-bit or 64-bit versions of Windows, 
and cross-platform (works both with Linux and Mac OS X). FreeImage is optionally used by OCCT to work
with images, on conditions of the FreeImage Public License (FIPL) (http://freeimage.sourceforge.net/freeimage-license.txt).

**MikTEX** is up-to-date implementation of TeX/LaTeX and related programs for Windows. It is used 
for generation of User and Developer Guides in PDF format. See http://miktex.org for information
on this tool.

Adobe Systems, Inc. provides **Adobe Reader**, which can be used to view files in Portable Document Format (PDF). 


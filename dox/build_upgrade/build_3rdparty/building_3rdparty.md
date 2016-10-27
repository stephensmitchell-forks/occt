Build 3rd-parties {#build_upgrade__building_3rdparty}
====================
@tableofcontents

On Windows, the easiest way to install third-party libraries is to download archive with pre-built binaries from http://www.opencascade.com/content/3rd-party-components.
On Linux and OS X, it is recommended to use the version installed in the system natively.

@section build_3rdparty_windows Windows
This document presents guidelines for building third-party products used by Open CASCADE Technology (OCCT) and samples on Windows platform. It is assumed that you are already familiar with MS Visual Studio / Visual C++. 

You need to use the same version of MS Visual Studio for building all third-party products and OCCT itself, in order to receive a consistent set of run-time binaries. 

The links for downloading the third-party products are available on the web site of OPEN CASCADE SAS at http://www.opencascade.com/content/3rd-party-components.

There are two types of third-party products used  by OCCT: 

It is recommended to create a separate new folder on your workstation, where you will unpack the downloaded archives of the third-party  products, and where you will build these products (for example, *c:\\occ3rdparty*). 

Further in this document, this folder is referred to as *3rdparty*. 

@subsection build_thirdparty_win_tcltk Tcl/Tk

##Installation Tcl
  
Download the necessary archive from http://www.tcl.tk/software/tcltk/download.html and unpack it. 
  
1. In the *win* sub-directory, edit file *buildall.vc.bat*:

   * Edit the line "call ... vcvars32.bat" to have correct path to the version of Visual Studio to be used for building, for instance:

         call "%VS80COMNTOOLS%\vsvars32.bat"

     If you are building 64-bit version, set environment accordingly, e.g.:

         call "%VS80COMNTOOLS%\..\..\VC\vcvarsall.bat" amd64
     
   * Define variable *INSTALLDIR* pointing to directory where Tcl/Tk will be installed, e.g.:

         set INSTALLDIR=D:\OCCT\3rdparty\tcltk-86-32

   * Add option *install* to the first command line calling *nmake*:

         nmake -nologo -f makefile.vc release htmlhelp install %1

   * Remove second call to *nmake* (building statically linked executable)

2. Edit file *rules.vc* replacing line 

       SUFX	    = tsgx		->		SUFX	    = sgx

   This is to avoid extra prefix 't' in the library name, which is not recognized by default by OCCT build tools.

3. By default, Tcl uses dynamic version of run-time library (MSVCRT), which must be installed on the system where Tcl will be used.
   You may wish to link Tcl library with static version of run-time to avoid this dependency.
   For that:

   * Edit file *makefile.vc* replacing strings "crt = -MD" by "crt = -MT"

   * Edit source file *tclMain.c* (located in folder *generic*) commenting out forward declaration of function *isatty()*.

4. In the command prompt, run *buildall.vc.bat*

   You might need to run this script twice to have *tclsh* executable installed; check subfolder *bin* of specified installation path to verify this.

5. For convenience of use, we recommend making a copy of *tclsh* executable created in subfolder *bin* of *INSTALLDIR* and named with Tcl version number suffix, as *tclsh.exe* (with no suffix)

       > cd D:\OCCT\3rdparty\tcltk-86-32\bin
       > cp tclsh86.exe tclsh.exe

## Installation Tk
  
Download the necessary archive from http://www.tcl.tk/software/tcltk/download.html and unpack it. 

Apply the same steps as described for building Tcl above, with the same INSTALLDIR.
Note that Tk produces its own executable, called *wish*. 

You might need to edit default value of *TCLDIR* variable defined in *buildall.vc.bat* (should be not necessary if you unpack both Tcl and Tk sources in the same folder).

@subsection build_thirdparty_win_freetype FreeType

FreeType is required for text display in a 3D viewer. You can download its sources from http://sourceforge.net/projects/freetype/files/   

1. Unpack the downloaded archive of FreeType product into the *3rdparty*  folder. As a result, you will get a folder named, for example, *3rdparty\\freetype-2.4.10*.  Further in this document, this folder is referred to as *freetype*. 
   
2. Open the solution file *freetype\\builds\\win32\\vc20xx\\freetype.sln*  in Visual Studio. Here *vc20xx* stands for your version of Visual Studio. 

3. Select the configuration to build: either Debug or Release. 

4. Build  the *freetype* project. 

   As a result, you will get a  freetype import library (.lib) in the *freetype\\obj\\win32\\vc20xx*  folder. 

5. If you build FreeType for a 64 bit platform, select in the main menu **Build - Configuration Manager**  and add *x64* platform to the solution configuration by copying the settings from Win32 platform: 

   @image html /build_upgrade/build_3rdparty/images/3rdparty_image001.png 
   @image latex /build_upgrade/build_3rdparty/images/3rdparty_image001.png 

   Update the value of the Output File for  x64 configuration: 

   @image html /build_upgrade/build_3rdparty/images/3rdparty_image003.png 
   @image latex /build_upgrade/build_3rdparty/images/3rdparty_image003.png 

   Build the *freetype* project. 

   As a result, you will obtain a 64 bit import library (.lib) file in the *freetype\\x64\\vc20xx*  folder. 

   To build FreeType as a dynamic library (.dll) follow steps 6, 7 and 8 of this procedure. 

6. Open menu Project-> Properties-> Configuration  Properties-> General and change option **Configuration Type** to *Dynamic  Library (.dll)*. 
7. Edit file *freetype\\include\\freetype\\config\\ftoption.h*:  
  
   in line 255, uncomment the definition of macro *FT_EXPORT* and change it as follows: 

       #define FT_EXPORT(x)   __declspec(dllexport) x 

8. Build  the *freetype* project. 

   As a result, you will obtain the files of the import  library (.lib) and the dynamic library (.dll)   in folders <i>freetype  \\objs\\release</i> or <i>\\objs\\debug </i>. 
   
   If you build for a 64 bit platform, follow step 5 of the procedure. 

   To facilitate the use of FreeType libraries in OCCT with minimal adjustment of build procedures, it is recommended to copy the include files and libraries of FreeType into a separate folder, named according to the pattern: *freetype-compiler-bitness-building mode*, where: 
   * **compiler** is *vc8* or *vc9* or *vc10* or *vc11*; 
   * **bitness**  is *32* or *64*;
   * **building mode** is *opt* (for Release) or *deb* (for Debug). 
    
   The *include* subfolder should be copied as is, while libraries should be renamed to *freetype.lib* and *freetype.dll* (suffixes removed) and placed to subdirectories *lib *and  *bin*, respectively. If the Debug configuration is built, the Debug libraries should be put into subdirectories *libd* and *bind*. 

@subsection build_thirdparty_win_tbb TBB

This third-party product is installed with binaries 
from the archive that can be downloaded from http://threadingbuildingblocks.org/. 
Go to the **Download** page, find the  release version you need  (e.g. *tbb30_018oss*) and pick the archive for Windows  platform. 

Unpack the downloaded  archive of TBB product into the *3rdparty* folder. 

Further in this document,  this folder is referred to as *tbb*. 

@subsection build_thirdparty_win_gl2ps gl2ps

This third-party product should be built as a dynamically loadable library (dll file). 
You can download its sources from  http://geuz.org/gl2ps/src/. 

1. Unpack the downloaded  archive of gl2ps product (e.g. *gl2ps-1.3.5.tgz*) into the *3rdparty*  folder. 

   As a result, you will get a folder named, for example, *3rdparty\\gl2ps-1.3.5-source*. 

   Rename it into <i>gl2ps-platform-compiler-building mode</i>, where 
   * **platform** -- *win32*  or *win64*; 
   * **compiler** -- *vc8*, *vc9* or *vc10*; 
   * **building mode** -- *opt*  (for release) or *deb* (for debug). 
   
   For example, <i>gl2ps-win64-vc10-deb</i>

   Further in this document,  this folder is referred to as *gl2ps*. 

2. Download (from http://www.cmake.org/cmake/resources/software.html) 
   and install the *CMake* build system.  

3. Edit the file *gl2ps\\CMakeLists.txt*. 

   After line 113 in *CMakeLists.txt*: 

       set_target_properties(shared PROPERTIES  COMPILE_FLAGS \"-DGL2PSDLL -DGL2PSDLL_EXPORTS\")
    
   add the following line:   
    
       add_definitions(-D_USE_MATH_DEFINES)  

   Attention: If Cygwin was installed on  your computer, make sure that there is no path to it in the *PATH* variable to avoid possible conflicts during the configuration. 

4. Launch CMake <i>(cmake-gui.exe)</i> using  the Program menu. 

   In CMake: 
  
   * Define where the source code is. 
     This path must point to *gl2ps*  folder. 

   * Define where to build the  binaries. 
     This path must point to the folder where generated gl2ps project binaries will be placed 
     (for example, *gl2ps\\bin*). 
     Further in this document, this folder is referred to as *gl2ps_bin*.

   * Press  **Configure** button. 
   
     @image html /build_upgrade/build_3rdparty/images/3rdparty_image004.png 
     @image latex /build_upgrade/build_3rdparty/images/3rdparty_image004.png 

   * Select the generator (the compiler  and the target platform -- 32 or 64 bit) in the pop-up window. 
   
     @image html /build_upgrade/build_3rdparty/images/3rdparty_image005.png 
     @image latex /build_upgrade/build_3rdparty/images/3rdparty_image005.png 

   * Press **Finish** button to  return to the main CMake window. 
     Expand the ENABLE group and uncheck  ENABLE_PNG and ENABLE_ZLIB check boxes. 
	 
     @image html /build_upgrade/build_3rdparty/images/3rdparty_image006.png 
     @image latex /build_upgrade/build_3rdparty/images/3rdparty_image006.png 

   * Expand the CMAKE group and define *CMAKE_INSTALL_PREFIX* which is the path where you want to install the build results, for example, *c:\\occ3rdparty\\gl2ps-1.3.5*. 
   
     @image html /build_upgrade/build_3rdparty/images/3rdparty_image007.png 
     @image latex /build_upgrade/build_3rdparty/images/3rdparty_image007.png 

   * Press **Configure** button again, then press **Generate** button to generate Visual Studio projects. After completion, close CMake application. 

5. Open the solution file *gl2ps_bin\\gl2ps.sln* in Visual Studio. 

   * Select a configuration to build 
     * Choose **Release** to build Release binaries. 
     * Choose **Debug** to build Debug binaries. 
   * Select a platform to build. 
     * Choose **Win32** to build for a 32 bit platform. 
     * Choose **x64** to build for a 64 bit platform. 
   * Build the solution. 
   * Build the *INSTALL* project. 
    
As a  result, you should have the installed gl2ps product in the *CMAKE_INSTALL_PREFIX* path. 

@subsection build_thirdparty_win_freeimage FreeImage

This third-party product should be built as a dynamically loadable library (.dll file). 
You can download its sources from 
http://sourceforge.net/projects/freeimage/files/Source%20Distribution/

1. Unpack the  downloaded archive of FreeImage product into *3rdparty* folder. 
  
   As a result, you should have a folder named *3rdparty\\FreeImage*. 
  
   Rename it according to the rule: *freeimage-platform-compiler-building  mode*, where 
   
   * **platform**  is *win32* or *win64*; 
   * **compiler**  is *vc8* or *vc9* or *vc10* or *vc11*; 
   * **building  mode** is *opt* (for release) or *deb* (for debug) 

   Further in this  document, this folder is referred to as *freeimage*. 

2. Open the solution file *freeimage\\FreeImage.*.sln* in your Visual Studio. 
  
   If you use a Visual Studio version higher than VC++ 2008, apply conversion of the workspace. 
   Such conversion should be suggested automatically by Visual  Studio. 
    
3. Select a  configuration to build. 

   - Choose **Release**  if you are building Release binaries. 
   - Choose **Debug** if you are building  Debug binaries. 

   *Note:* 

   If you want to  build a debug version of FreeImage binaries then you need to rename the following files in FreeImage and FreeimagePlus projects:
   
   Project -> Properties -> Configuration Properties -> Linker -> General -> Output File

       FreeImage*d*.dll  to FreeImage.dll 
       FreeImagePlus*d*.dll to FreeImagePlus.dll 

   Project -> Properties -> Configuration Properties -> Linker -> Debugging-> Generate Program Database File

       FreeImage*d*.pdb  to FreeImage.pdb 
       FreeImagePlus*d*.pdb to FreeImagePlus.pdb 

   Project -> Properties -> Configuration Properties -> Linker -> Advanced-Import Library

       FreeImage*d*.lib  to FreeImage.lib 
       FreeImagePlus*d*.lib  to FreeImagePlus.lib 

   Project -> Properties -> Configuration Properties -> Build Events -> Post -> Build Event -> Command Line 

       FreeImage*d*.dll     to FreeImage.dll 
       FreeImage*d*.lib     to FreeImage.lib 
       FreeImagePlus*d*.dll to FreeImagePlus.dll 
       FreeImagePlus*d*.lib to FreeImagePlus.lib 

   Additionally, rename in project FreeImagePlus 
   
   Project -> Properties -> Configuration  Properties -> Linker -> Input -> Additional Dependencies 

       from FreeImage*d*.lib to FreeImage.lib 

4. Select a platform to build. 

   - Choose *Win32* if you are building for a 32 bit platform. 
   - Choose *x64* if you are building for a 64 bit platform. 

5. Start the building process. 

   As a result, you should have the  library files of FreeImage product  in *freeimage\\Dist* folder (*FreeImage.dll* and *FreeImage.lib*) and in *freeimage\\Wrapper\\FreeImagePlus\\dist* folder (*FreeImagePlus.dll* and *FreeImagePlus.lib*).

@subsection build_thirdparty_win_vtk VTK

VTK is an open-source, freely available software system for 3D computer graphics, image processing and visualization. VTK Integration Services component provides adaptation functionality for visualization of OCCT topological shapes by means of VTK library.

1. Download the necessary archive from http://www.vtk.org/VTK/resources/software.html and unpack it into *3rdparty* folder.

   As a result, you will get a folder named, for example, <i>3rdparty\VTK-6.1.0.</i>

   Further in this document, this folder is referred to as *VTK*.

2. Use CMake to generate VS projects for building the library:
   - Start CMake-GUI and select VTK folder as source path, and the folder of your choice for VS project and intermediate build data.
   - Click **Configure**.
   - Select the VS version to be used from the ones you have installed (we recommend using VS 2010) and the architecture (32 or 64-bit).
   - Generate VS projects with default CMake options. The open solution *VTK.sln* will be generated in the build folder.

3. Build project VTK in Release mode.


@section build_3rdparty_linux Linux
@subsection build_3rdparty_linux_tcltk Tcl/Tk 

## Installation Tcl
  
Download the necessary archive from http://www.tcl.tk/software/tcltk/download.html and unpack it. 
  
1. Enter the unix sub-directory of the directory where the Tcl source  files are located <i>(TCL_SRC_DIR)</i>. 

       cd TCL_SRC_DIR/unix   

2. Run the *configure* command:

       configure --enable-gcc  --enable-shared --enable-threads --prefix=TCL_INSTALL_DIR   

   For a 64 bit platform also add <i>--enable-64bit</i> option to the command line. 
  
3. If the configure command has finished successfully, start  the building process:
  
       make   

4. If building is finished successfully, start the installation of Tcl. 
   All binary and service files of the product will be copied to the directory defined by *TCL_INSTALL_DIR* 

       make install   

## Installation Tk
  
Download the necessary archive from http://www.tcl.tk/software/tcltk/download.html and unpack it. 
  
1. Enter the unix sub-directory of the directory where the Tk source files are located <i>(TK_SRC_DIR)</i> 

       cd TK_SRC_DIR/unix   

2. Run the configure command, where <i>TCL_LIB_DIR</i> is *TCL_INSTALL_DIR/lib*.

       configure --enable-gcc  --enable-shared --enable-threads --with-tcl=TCL_LIB_DIR  --prefix=TK_INSTALL_DIR   

   For a 64 bit platform also add <i>--enable-64bit</i> option to the  command line. 

3. If the configure command has finished successfully, start the building process: 

       make   

4. If the building has finished successfully, start the installation of Tk. 
   All binary and service files of the product will be copied 
   to the directory defined by *TK_INSTALL_DIR* (usually it is *TCL_INSTALL_DIR*) 

       make install   

@subsection build_3rdparty_linux_freetype FreeType

FreeType is required for text display in the 3D viewer. 
Download the necessary archive from http://sourceforge.net/projects/freetype/files/ and unpack it. 
  
1. Enter the directory where the source files of FreeType  are located <i>(FREETYPE_SRC_DIR)</i>. 

       cd FREETYPE_SRC_DIR   

2. Run the *configure* command: 
  
       configure  --prefix=FREETYPE_INSTALL_DIR   

   For a 64 bit platform also add <i>CFLAGS='-m64 -fPIC'  CPPFLAGS='-m64 -fPIC'</i> option to the command line. 
    
3. If the *configure* command has finished successfully, start  the building process: 

       make   

4. If the building has finished successfully, start the installation of FreeType. 
   All binary and service files of the product will be copied to the directory defined by *FREETYPE_INSTALL_DIR* 
    
       make install   
   
@subsection build_3rdparty_linux_tbb TBB

This third-party product is  installed with binaries from the archive that can be downloaded from http://threadingbuildingblocks.org. 
Go to the **Download** page, find the  release version you need and pick the archive for Linux platform.
To install, unpack the downloaded archive of TBB product.

@subsection build_3rdparty_linux_gl2ps gl2ps

Download the necessary archive from http://geuz.org/gl2ps/ and unpack it.
  
1. Install or build *cmake* product from the source file. 
2. Start *cmake* in GUI mode with the directory where the source files of gl2ps are located:

       ccmake GL2PS_SRC_DIR   

   * Press <i>[c]</i> to make the  initial configuration; 
   * Define the necessary options in *CMAKE_INSTALL_PREFIX* 
   * Press <i>[c]</i> to make the final  configuration 
   * Press <i>[g]</i> to generate  Makefile and exit 

   or just run the following command: 

       cmake  –DCMAKE_INSTALL_PREFIX=GL2PS_INSTALL_DIR –DCMAKE_BUILD_TYPE=Release   

3. Start the building of gl2ps: 

       make   

4. Start the installation of gl2ps. Binaries will be  installed according to the *CMAKE_INSTALL_PREFIX* option. 

       make install   

@subsection build_3rdparty_linux_freeimage FreeImage

Download the necessary archive from http://sourceforge.net/projects/freeimage/files/Source%20Distribution/
and unpack it. The directory with unpacked sources is  further referred to as *FREEIMAGE_SRC_DIR*. 
  
1. Modify *FREEIMAGE_SRC_DIR/Source/OpenEXR/Imath/ImathMatrix.h*: 
   In line 60 insert the following: 

       #include string.h   

2. Enter the directory where the source files of FreeImage are located <i>(FREEIMAGE_SRC_DIR)</i>. 
  
       cd FREEIMAGE_SRC_DIR   

3. Run the building process 
  
       make   

4. Run the installation process 

   a. If you have the permission to write into directories <i>/usr/include</i> and <i>/usr/lib</i>, run the following command: 

          make install   

   b. If you do not have this permission, you need to modify file *FREEIMAGE_SRC_DIR/Makefile.gnu*: 
  
      Change lines 7-9:
  
          DESTDIR ?= /							->		DESTDIR  ?= $(DESTDIR) 
          INCDIR  ?= $(DESTDIR)/usr/include		->		INCDIR  ?= $(DESTDIR)/include 
          INSTALLDIR  ?= $(DESTDIR)/usr/lib		->		INSTALLDIR  ?= $(DESTDIR)/lib 

      Change lines 65-67: 

          install  -m 644 -o root -g root $(HEADER) $(INCDIR)			->		install  -m 755 $(HEADER) $(INCDIR)  
          install  -m 644 -o root -g root $(STATICLIB) $(INSTALLDIR)	->		install  -m 755 $(STATICLIB) $(INSTALLDIR)  
          install  -m 755 -o root -g root $(SHAREDLIB) $(INSTALLDIR)	->		install  -m 755 $(SHAREDLIB) $(INSTALLDIR)

      Change line 70: 

          ldconfig		->		\#ldconfig
    
   Then run the installation process by the following command: 

        make DESTDIR=FREEIMAGE_INSTALL_DIR  install   

5. Clean temporary files

        make clean

@subsection build_3rdparty_linux_vtk VTK

Download the necessary archive from http://www.vtk.org/VTK/resources/software.html and unpack it.

1. Install or build *cmake* product from the source file.
2. Start *cmake* in GUI mode with the directory where the source files of *VTK* are located:

       ccmake VTK_SRC_DIR

   * Press <i>[c]</i> to make the  initial configuration
   * Define the necessary options in *VTK_INSTALL_PREFIX*
   * Press <i>[c]</i> to make the final  configuration
   * Press <i>[g]</i> to generate  Makefile and exit

3. Start the building of VTK:

       make

4. Start the installation of gl2ps. Binaries will be  installed according to the *VTK_INSTALL_PREFIX* option.

       make install

@subsection build_3rdparty_linux_install Installation From Official Repositories (Debian-based)

All 3rd-party products required for building of  OCCT could be installed
from official repositories. You may install them from  console using apt-get utility:

    sudo apt-get install tcllib tklib tcl-dev tk-dev libfreetype-dev libxt-dev libxmu-dev libxi-dev libgl1-mesa-dev libglu1-mesa-dev libfreeimage-dev libtbb-dev libgl2ps-dev

Building is possible with C++ compliant compiler:

    sudo apt-get install g++

@section build_3rdparty_macos Mac OS X
This document presents additional guidelines for building third-party products 
used by Open CASCADE Technology and samples on Mac OS X platform (10.6.4 and later). 

@subsection build_3rdparty_mac_tcltk Tcl/Tk

## Installation Tcl

Download the necessary archive from http://www.tcl.tk/software/tcltk/download.html and unpack it. 

1. Enter the *macosx* sub-directory of the directory where the Tcl source files are located <i>(TCL_SRC_DIR)</i>. 

       cd TCL_SRC_DIR/macosx   

2. Run the *configure* command 

       configure --enable-gcc  --enable-shared --enable-threads --prefix=TCL_INSTALL_DIR   

   For a 64 bit platform also add <i>--enable-64bit</i> option to the command line. 

3. If the *configure* command has finished successfully, start the building process 

       make   
  
4. If building is finished successfully, start the installation of Tcl. 
   All binary and service files of the product will be copied to the directory defined by *TCL_INSTALL_DIR*. 

       make install   

## Installation Tk

Download the necessary archive from http://www.tcl.tk/software/tcltk/download.html and unpack it. 

1. Enter the *macosx* sub-directory of the directory where the  source files of Tk are located <i>(TK_SRC_DIR)</i>. 

       cd TK_SRC_DIR/macosx   

2. Run the *configure* command, where TCL_LIB_DIR is  TCL_INSTALL_DIR/lib 

       configure --enable-gcc --enable-shared --enable-threads --with-tcl=TCL_LIB_DIR --prefix=TK_INSTALL_DIR   

	For a 64 bit platform also add <i>--enable-64bit</i> option to the  command line. 

3. If the *configure* command has finished successfully, start the building process:

       make   

4. If the building has finished successfully, start the  installation of Tk. All binary and service files of the product will be copied to the directory defined by *TK_INSTALL_DIR* (usually it is TCL_INSTALL_DIR) 

       make install   

@subsection build_3rdparty_mac_freetype FreeType

Download the necessary archive from http://sourceforge.net/projects/freetype/files/ and unpack it. 

1. Enter the directory where the source files of FreeType  are located <i>(FREETYPE_SRC_DIR)</i>. 

       cd FREETYPE_SRC_DIR   

2. Run the *configure* command 
  
       configure  --prefix=FREETYPE_INSTALL_DIR   

   For a 64 bit platform also add <i>CFLAGS='-m64 -fPIC'  CPPFLAGS='-m64 -fPIC'</i> option to the command line. 

3. If the *configure* command has finished successfully, start  the building process 

       make   

4. If building has finished successfully, start the installation of FreeType. 
   All binary and service files of the product will be copied to the directory defined by *FREETYPE_INSTALL_DIR*.

       make install   
	       
@subsection build_3rdparty_mac_tbb TBB

This third-party product is installed with binaries from the archive 
that can be downloaded from http://threadingbuildingblocks.org/. 
Go to the **Download** page, find the release version you need (e.g. *tbb30_018oss*) 
and  pick the archive for Mac OS X platform. 
To install, unpack the downloaded archive of TBB 3.0 product (*tbb30_018oss_osx.tgz*).

@subsection build_3rdparty_mac_gl2ps gl2ps

Download the necessary archive from http://geuz.org/gl2ps/ and unpack it. 

1. Install or build cmake product from the source file. 

2. Start cmake in GUI mode with the directory, where the source  files of *fl2ps* are located: 

       ccmake GL2PS_SRC_DIR   

   * Press <i>[c]</i> to make the  initial configuration; 
   * Define the necessary options in *CMAKE_INSTALL_PREFIX*; 
   * Press <i>[c]</i> to make the final  configuration; 
   * Press <i>[g]</i> to generate  Makefile and exit. 

   or just run the following command: 

       cmake  –DCMAKE_INSTALL_PREFIX=GL2PS_INSTALL_DIR –DCMAKE_BUILD_TYPE=Release   

3. Start the building of gl2ps 

       make   

4. Start the installation of gl2ps. Binaries will be  installed according to the *CMAKE_INSTALL_PREFIX* option 

       make install   

@subsection build_3rdparty_mac_freeimage FreeImage

Download the necessary archive from 
http://sourceforge.net/projects/freeimage/files/Source%20Distribution/   
and unpack it. The directory with unpacked sources is  further referred to as *FREEIMAGE_SRC_DIR*.  

Note that for building FreeImage on Mac OS X 10.7 you should replace *Makefile.osx* 
in *FREEIMAGE_SRC_DIR* by the corrected file, which you can find in attachment to issue #22811 in OCCT Mantis bug tracker 
(http://tracker.dev.opencascade.org/file_download.php?file_id=6937&type=bug). 

1. If you build FreeImage 3.15.x you can skip this step. 
   Modify <i>FREEIMAGE_SRC_DIR/Source/OpenEXR/Imath/ImathMatrix.h:</i> 
  
   In line 60 insert the following: 

       #include string.h 

   Modify <i>FREEIMAGE_SRC_DIR/Source/FreeImage/PluginTARGA.cpp:</i> 
  
   In line 320 replace: 

       SwapShort(value);		->		SwapShort(&value); 

2. Enter the directory where the source files of FreeImage  are located <i>(FREEIMAGE_SRC_DIR)</i>. 

       cd FREEIMAGE_SRC_DIR 

3. Run the building process 

       make   

4. Run the installation process 
   
   1. If you have the permission to write into <i>/usr/local/include</i>  and <i>/usr/local/lib</i> directories, run the following command: 

          make install   

   2. If you do not have this permission, you need to modify file *FREEIMAGE_SRC_DIR/Makefile.osx*: 

      Change line 49:     

          PREFIX ?= /usr/local		->		PREFIX  ?= $(PREFIX) 

      Change lines 65-69 from: 

          install -d -m 755 -o  root -g wheel $(INCDIR) $(INSTALLDIR) 
          install  -m 644 -o root -g wheel $(HEADER) $(INCDIR) 
          install  -m 644 -o root -g wheel $(SHAREDLIB) $(STATICLIB) $(INSTALLDIR) 
          ranlib  -sf $(INSTALLDIR)/$(STATICLIB) 
          ln  -sf $(SHAREDLIB) $(INSTALLDIR)/$(LIBNAME) 
    
      to: 

          install  -d $(INCDIR) $(INSTALLDIR) 
          install  -m 755 $(HEADER) $(INCDIR) 
          install  -m 755 $(STATICLIB) $(INSTALLDIR) 
          install  -m 755 $(SHAREDLIB) $(INSTALLDIR) 
          ln  -sf $(SHAREDLIB) $(INSTALLDIR)/$(VERLIBNAME)  
          ln  -sf $(VERLIBNAME) $(INSTALLDIR)/$(LIBNAME)

      Then run the installation process by the following command: 
  
          make PREFIX=FREEIMAGE_INSTALL_DIR  install 
  
5. Clean temporary files 
  
       make clean 

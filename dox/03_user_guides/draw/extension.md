Draw Extension {#user_guides__draw_extension}
==============

@tableofcontents

The following chapters explain how to extend Draw with custom commands and how to activate them using a plug-in mechanism. 

@section occt_draw_11_1 Custom command implementation

Custom command implementation has not undergone any changes since the introduction of the plug-in mechanism. The syntax 
of every command should still be like in the following example. 

**Example:** 
~~~~~
static Standard_Integer myadvcurve(Draw_Interpretor& di, Standard_Integer n, char** a) 
{ 
... 
} 
~~~~~

For examples of existing commands refer to Open CASCADE Technology (e.g. GeomliteTest.cxx). 


@section occt_draw_11_2 Registration of commands in Draw

To become available in the Test Harness the custom command must be registered in it. This should be done as follows. 

**Example:** 
~~~~~
void MyPack::CurveCommands(Draw_Interpretor& theCommands) 
{ 
... 
char* g = "Advanced curves creation"; 

theCommands.Add ( "myadvcurve", "myadvcurve name p1 p2 p3 - Creates my advanced curve from points", 
                  __FILE__, myadvcurve, g ); 
... 
} 
~~~~~

@section occt_draw_11_3 Creating a toolkit (library) as a plug-in

All custom commands are compiled and linked into a dynamic library (.dll on Windows, or .so on Unix/Linux). To make Draw
 recognize it as a plug-in it must respect certain conventions. Namely, it must export function *PLUGINFACTORY()*
 accepting the Test Harness interpreter object (*Draw_Interpretor*). This function will be called when the library is 
dynamically loaded during the Test Harness session. 

This exported function *PLUGINFACTORY()* must be implemented only once per library. 

For convenience the *DPLUGIN* macro (defined in the *Draw_PluginMacro.hxx* file) has been provided. It implements the
 *PLUGINFACTORY()* function as a call to the *Package::Factory()* method and accepts *Package* as an argument. 
Respectively, this *Package::Factory()* method must be implemented in the library and activate all implemented commands. 

**Example:** 
~~~~~
#include <Draw_PluginMacro.hxx>

void MyPack::Factory(Draw_Interpretor& theDI)
{
...
// 
MyPack::CurveCommands(theDI);
...
}

// Declare entry point PLUGINFACTORY
DPLUGIN(MyPack)
~~~~~

@section occt_draw_11_4 Creation of the plug-in resource file

Open CASCADE Technology is shipped with the <i>DrawPlugin</i> resource file located in the
 <i>$CASROOT/src/DrawResources</i> directory. 
The format of the file is compliant with standard OCCT resource files (see the *Resource_Manager.hxx* file for details). 
In particular, it should contain keys separated from their values by a colon (;:;). 

For every created plug-in there must be a key. For better readability and comprehension it is recommended to have some 
meaningful name. 
Thus, the resource file must contain a line mapping this name (key) to the library name. The latter should be without 
file extension (.dll on Windows, .so on Unix/Linux) and without the ;lib; prefix on Unix/Linux. 
For several plug-ins one resource file can be created. In such case, keys denoting plug-ins can be combined into groups,
 these groups -- into their groups and so on (thereby creating some hierarchy). Any new parent key must have its value
 as a sequence of child keys separated by spaces, tabs or commas. Keys should form a tree without cyclic dependencies. 

**Examples** (file MyDrawPlugin): 
~~~~~
! Hierarchy of plug-ins 
ALL                : ADVMODELING, MESHING 
DEFAULT            : MESHING 
ADVMODELING        : ADVSURF, ADVCURV 

! Mapping from naming to toolkits (libraries) 
ADVSURF            : TKMyAdvSurf 
ADVCURV            : TKMyAdvCurv 
MESHING            : TKMyMesh 
~~~~~

@section occt_draw_11_5 Dynamic loading and activation

To load a plug-in declared in the resource file and to activate the commands the following command must be used in Draw: 

~~~~~
pload [-PluginFileName] [[Key1] [Key2]...]
~~~~~

where: 

* <i>-PluginFileName</i> -- defines the name of a plug-in resource file (prefix "-" is mandatory) described above. If 
this parameter is omitted then the default name *DrawPlugin* is used. 
* *Key* -- defines the key(s) enumerating plug-ins to be loaded. If no keys are specified then the key named *DEFAULT* 
is used (if there is no such key in the file then no plug-ins are loaded). 

According to the OCCT resource file management rules, to access the resource file the environment variable 
*CSF_PluginFileNameDefaults* (and optionally *CSF_PluginFileNameUserDefaults*) must be set and point to the directory 
storing the resource file. If it is omitted then the plug-in resource file will be searched in the 
<i>$CASROOT/src/DrawResources</i> directory. 

~~~~~
Draw[]        pload -DrawPlugin OCAF 
~~~~~
This command will search the resource file *DrawPlugin* using variable *CSF_DrawPluginDefaults* (and 
*CSF_DrawPluginUserDefaults*) and will start with the OCAF key. Since the *DrawPlugin* is the file shipped with 
Open CASCADE Technology it will be found in the <i>$CASROOT/src/DrawResources</i> directory (unless this location is 
redefined by user's variables). The OCAF key will be recursively extracted into two toolkits/plug-ins: *TKDCAF* and 
*TKViewerTest* (e.g. on Windows they correspond to *TKDCAF.dll* and *TKViewerTest.dll*). Thus, commands implemented 
for Visualization and OCAF will be loaded and activated in Draw. 

~~~~~
Draw[]        pload (equivalent to pload -DrawPlugin DEFAULT). 
~~~~~
This command will find the default DrawPlugin file and the DEFAULT key. The latter finally maps to the TKTopTest toolkit
 which implements basic modeling commands. 

~~~~
Draw[]> set env(CSF_MyDrawPluginDefaults) /users/test
Draw[]> pload -MyDrawPlugin ALL
~~~~


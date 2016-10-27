Boolean Operations {#user_guides__modeling_algos_boolean_op}
==================

@tableofcontents

Boolean operations are  used to create new shapes from the combinations of two shapes. 

| Operation | Result |
| :---- | :------ |
| Fuse   |  all points in S1 or S2  |
| Common |  all points in S1 and S2 |
| Cut S1 by S2| all points in S1 and not in S2 | 

@image html /03_user_guides/modeling_algos/images/modeling_algos_image036.png  "Boolean Operations"
@image latex /03_user_guides/modeling_algos/images/modeling_algos_image036.png  "Boolean Operations"

From the viewpoint of Topology these are topological operations followed by blending (putting fillets onto edges created after the topological operation).

Topological operations are the most convenient way to create real industrial parts. As most industrial parts consist of several simple elements such as gear wheels, arms, holes, ribs, tubes and pipes. It is usually easy to create those elements separately and then to combine them by Boolean operations in the whole final part.

See @ref debug__boolean_operations "Boolean Operations" for detailed documentation.

Boolean Operations have the following types of the arguments and produce the following results:
* For arguments having the same shape type (e.g. SOLID / SOLID) the type of the resulting shape will be a COMPOUND, containing shapes of this type;
* For arguments having different shape types (e.g. SHELL / SOLID) the type of the resulting shape will be a COMPOUND, containing shapes of the type that is the same as that of the low type of the argument. Example: For SHELL/SOLID the result is a COMPOUND of SHELLs. 
* For arguments with different shape types some of Boolean Operations can not be done using the default implementation, because of a non-manifold type of the result. Example: the FUSE operation for SHELL and SOLID can not be done, but the CUT operation can be done, where SHELL is the object and SOLID is the tool.
* It is possible to perform Boolean Operations on arguments of the COMPOUND shape type. In this case each compound must not be heterogeneous, i.e. it must contain equidimensional shapes (EDGEs or/and WIREs, FACEs or/and SHELLs, SOLIDs). SOLIDs inside the COMPOUND must not contact (intersect or touch) each other. The same condition should be respected for SHELLs or FACEs, WIREs or EDGEs.
* Boolean Operations for COMPSOLID type of shape are not supported.

*BRepAlgoAPI_BooleanOperation* class is the deferred root class for Boolean  operations.

@section bop_fuse Fuse

*BRepAlgoAPI_Fuse* performs the Fuse operation. 

~~~~~
TopoDS_Shape A = ..., B = ...; 
TopoDS_Shape S = BRepAlgoAPI_Fuse(A,B); 
~~~~~

\ref anchor_bop_fuse_code_snippet "...more code snippet"

@section bop_common Common

*BRepAlgoAPI_Common*  performs the Common operation. 

~~~~~
TopoDS_Shape A = ..., B = ...; 
TopoDS_Shape S = BRepAlgoAPI_Common(A,B); 
~~~~~

\ref anchor_bop_common_code_snippet "...more code snippet"

@section bop_cut Cut
*BRepAlgoAPI_Cut* performs the Cut operation. 

~~~~~
TopoDS_Shape A = ..., B = ...; 
TopoDS_Shape S = BRepAlgoAPI_Cut(A,B); 
~~~~~

\ref anchor_bop_cut_code_snippet "...more code snippet"

@section bop_section Section

*BRepAlgoAPI_Section* performs the section, described as a *TopoDS_Compound* made of *TopoDS_Edge*. 

@image html /03_user_guides/modeling_algos/images/modeling_algos_image037.png "Section  operation"
@image latex /03_user_guides/modeling_algos/images/modeling_algos_image037.png "Section  operation"

~~~~~
TopoDS_Shape A = ...,  TopoDS_ShapeB = ...; 
TopoDS_Shape S =  BRepAlgoAPI_Section(A,B); 
~~~~~

\ref anchor_bop_section_code_snippet "...more code snippet"


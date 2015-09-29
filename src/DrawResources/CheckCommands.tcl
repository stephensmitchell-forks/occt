# Copyright (c) 2013-2014 OPEN CASCADE SAS
#
# This file is part of Open CASCADE Technology software library.
#
# This library is free software; you can redistribute it and/or modify it under
# the terms of the GNU Lesser General Public License version 2.1 as published
# by the Free Software Foundation, with special exception defined in the file
# OCCT_LGPL_EXCEPTION.txt. Consult the file LICENSE_LGPL_21.txt included in OCCT
# distribution for complete text of the license and disclaimer of any warranty.
#
# Alternatively, this file may be used under the terms of Open CASCADE
# commercial license or contractual agreement.

############################################################################
# This file defines scripts for verification of OCCT tests.
# It provides top-level commands starting with 'check'.
# Type 'help check*' to get their synopsys.
# See OCCT Tests User Guide for description of the test system.
#
# Note: procedures with names starting with underscore are for internal use
# inside the test system.
############################################################################

help checkcolor {
  Check pixel color.
  Use: checkcolor x y red green blue
  x y - pixel coordinates
  red green blue - expected pixel color (values from 0 to 1)
  Function check color with tolerance (5x5 area)
}
# Procedure to check color using command vreadpixel with tolerance
proc checkcolor { coord_x coord_y rd_get gr_get bl_get } {
    puts "Coordinate x = $coord_x"
    puts "Coordinate y = $coord_y"
    puts "RED color of RGB is $rd_get"
    puts "GREEN color of RGB is $gr_get"
    puts "BLUE color of RGB is $bl_get"

    if { $coord_x <= 1 || $coord_y <= 1 } {
      puts "Error : minimal coordinate is x = 2, y = 2. But we have x = $coord_x y = $coord_y"
      return -1
    }

    set color ""
    catch { [set color "[vreadpixel ${coord_x} ${coord_y} rgb]"] }
    if {"$color" == ""} {
      puts "Error : Pixel coordinates (${position_x}; ${position_y}) are out of view"
    }
    set rd [lindex $color 0]
    set gr [lindex $color 1]
    set bl [lindex $color 2]
    set rd_int [expr int($rd * 1.e+05)]
    set gr_int [expr int($gr * 1.e+05)]
    set bl_int [expr int($bl * 1.e+05)]
    set rd_ch [expr int($rd_get * 1.e+05)]
    set gr_ch [expr int($gr_get * 1.e+05)]
    set bl_ch [expr int($bl_get * 1.e+05)]

    if { $rd_ch != 0 } {
      set tol_rd [expr abs($rd_ch - $rd_int)/$rd_ch]
    } else {
      set tol_rd $rd_int
    }
    if { $gr_ch != 0 } {
      set tol_gr [expr abs($gr_ch - $gr_int)/$gr_ch]
    } else {
      set tol_gr $gr_int
    }
    if { $bl_ch != 0 } {
      set tol_bl [expr abs($bl_ch - $bl_int)/$bl_ch]
    } else {
      set tol_bl $bl_int
    }

    set status 0
    if { $tol_rd > 0.2 } {
      puts "Warning : RED light of additive color model RGB is invalid"
      set status 1
    }
    if { $tol_gr > 0.2 } {
      puts "Warning : GREEN light of additive color model RGB is invalid"
      set status 1
    }
    if { $tol_bl > 0.2 } {
      puts "Warning : BLUE light of additive color model RGB is invalid"
      set status 1
    }

    if { $status != 0 } {
      puts "Warning : Colors of default coordinate are not equal"
    }

    global stat
    if { $tol_rd > 0.2 || $tol_gr > 0.2 || $tol_bl > 0.2 } {
      set info [_checkpoint $coord_x $coord_y $rd_ch $gr_ch $bl_ch]
      set stat [lindex $info end]
      if { ${stat} != 1 } {
          puts "Error : Colors are not equal in default coordinate and in the near coordinates too"
          return $stat
      } else {
          puts "Point with valid color was found"
          return $stat
      }
    } else {
      set stat 1
    }
}

# Procedure to check color in the point near default coordinate
proc _checkpoint {coord_x coord_y rd_ch gr_ch bl_ch} {
    set x_start [expr ${coord_x} - 2]
    set y_start [expr ${coord_y} - 2]
    set mistake 0
    set i 0
    while { $mistake != 1 && $i <= 5 } {
      set j 0
      while { $mistake != 1 && $j <= 5 } {
          set position_x [expr ${x_start} + $j]
          set position_y [expr ${y_start} + $i]
          puts $position_x
          puts $position_y

          set color ""
          catch { [set color "[vreadpixel ${position_x} ${position_y} rgb]"] }
          if {"$color" == ""} {
            puts "Warning : Pixel coordinates (${position_x}; ${position_y}) are out of view"
            incr j
            continue
          }
          set rd [lindex $color 0]
          set gr [lindex $color 1]
          set bl [lindex $color 2]
          set rd_int [expr int($rd * 1.e+05)]
          set gr_int [expr int($gr * 1.e+05)]
          set bl_int [expr int($bl * 1.e+05)]

          if { $rd_ch != 0 } {
            set tol_rd [expr abs($rd_ch - $rd_int)/$rd_ch]
          } else {
            set tol_rd $rd_int
          }
          if { $gr_ch != 0 } {
            set tol_gr [expr abs($gr_ch - $gr_int)/$gr_ch]
          } else {
            set tol_gr $gr_int
          }
          if { $bl_ch != 0 } {
            set tol_bl [expr abs($bl_ch - $bl_int)/$bl_ch]
          } else {
            set tol_bl $bl_int
          }

          if { $tol_rd > 0.2 || $tol_gr > 0.2 || $tol_bl > 0.2 } {
            puts "Warning : Point with true color was not found near default coordinates"
            set mistake 0
          } else {
            set mistake 1
          }
          incr j
      }
      incr i
    }
    return $mistake
}

# auxiliary: check argument
proc _check_arg {check_name check_result {get_value 0}} {
  upvar ${check_result} ${check_result}
  upvar arg arg
  upvar narg narg
  upvar args args
  if { ${arg} == ${check_name} } {
    if {${get_value}} {
      incr narg
      if { ${narg} < [llength ${args}] && ! [regexp {^-} [lindex ${args} ${narg}]] } {
        set ${check_result} "[lindex ${args} ${narg}]"
      } else {
        error "Option ${check_result} requires argument"
      }
    } else {
      set ${check_result} 1
    }
    return 1
  }
  return 0
}

help checknbshapes {
  Compare number of sub-shapes in "shape" with given reference data

  Use: checknbshapes shape [options...]
  Allowed options are:
    -vertex N
    -edge N
    -wire N
    -face N
    -shell N
    -solid N
    -compsolid N
    -compound N
    -shape N
    -t: compare the number of sub-shapes in "shape" counting
        the same sub-shapes with different location as different sub-shapes.
    -m msg: print "msg" in case of error
    -ref [nbshapes a]: compare the number of sub-shapes in "shape" and in "a".
                       -vertex N, -edge N and other options are still working.
    -max_vertex N:    compare number of vertexes with max possible number of vertexes N
    -max_edge N:      compare number of edges with max possible number of edges N
    -max_wire N:      compare number of wires with max possible number of wires N
    -max_face N:      compare number of faces with max possible number of faces N
    -max_shell N:     compare number of shells with max possible number of shells N
    -max_solid N:     compare number of solids with max possible number of solids N
    -max_compsolid N: compare number of compsolids with max possible number of compsolids N
    -max_compound N:  compare number of compounds with max possible number of compounds N
    -max_shape N:     compare number of shapes with max possible number of shapes N
}
proc checknbshapes {shape args} {
  puts "checknbshapes ${shape} ${args}"
  upvar ${shape} ${shape}

  set nbVERTEX -1
  set nbEDGE -1
  set nbWIRE -1
  set nbFACE -1
  set nbSHELL -1
  set nbSOLID -1
  set nbCOMPSOLID -1
  set nbCOMPOUND -1
  set nbSHAPE -1

  set message ""
  set count_locations 0
  set ref_info ""

  set options {{"-vertex" nbVERTEX 1}
               {"-edge" nbEDGE 1}
               {"-wire" nbWIRE 1}
               {"-face" nbFACE 1}
               {"-shell" nbSHELL 1}
               {"-solid" nbSOLID 1}
               {"-compsolid" nbCOMPSOLID 1}
               {"-compound" nbCOMPOUND 1}
               {"-shape" nbSHAPE 1}
               {"-max_vertex" max_nbVERTEX 1}
               {"-max_edge" max_nbEDGE 1}
               {"-max_wire" max_nbWIRE 1}
               {"-max_face" max_nbFACE 1}
               {"-max_shell" max_nbSHELL 1}
               {"-max_solid" max_nbSOLID 1}
               {"-max_compsolid" max_nbCOMPSOLID 1}
               {"-max_compound" max_nbCOMPOUND 1}
               {"-max_shape" max_nbSHAPE 1}
               {"-t" count_locations}
               {"-de_iges" isDeIges 1}
               {"-not_de_iges" isNotDeIges 1}
               {"-m" message 1}
               {"-ref" ref_info 1}}

  _check_args ${args} ${options} "checknbshapes"

  if { ${count_locations} == 0 } {
    set nb_info [nbshapes ${shape}]
  } else {
    set nb_info [nbshapes ${shape} -t]
  }

  set EntityList {VERTEX EDGE WIRE FACE SHELL SOLID COMPSOLID COMPOUND SHAPE}

  set nbshapes_res [dict create]
  foreach Entity ${EntityList} {
    set expr_string "${Entity} +: +(\[-0-9.+eE\]+)"
    set to_compare {}
    # get number of elements from ${shape}
    if { [regexp "${expr_string}" ${nb_info} full nb_entity2] } {
      dict append nbshapes_res ${Entity} ${nb_entity2}
      lappend to_compare ${nb_entity2}
    } else {
      error "Error : command \"nbshapes ${shape}\" gives an empty result"
    }
    # get number of elements from options -vertex -edge and so on
    set nb_entity1 [set nb${Entity}]
    if { ${nb_entity1} != -1 } {
      lappend to_compare ${nb_entity1}
    }
    # get number of elements from option -ref
    if { [regexp "${expr_string}" ${ref_info} full nb_entity_ref] } {
      lappend to_compare ${nb_entity_ref}
    }
    # compare current number of elements with max possible number of elements
    # received from -max_vertex, -max_edge, ... options
    _comparewithmax nb_entity2 max_nb${Entity} "NBSHAPES" 0 "Compare number of ${Entity} in command nbshapes with max possible number"
    # skip comparing if no reference data was given
    if {[llength $to_compare] == 1} {
      continue
    }
    # compare all values, if they are equal, length of sorted list "to_compare"
    # (with key -unique) should be equal to 1
    set to_compare [lsort -dictionary -unique $to_compare]
    if { [llength $to_compare] != 1 } {
      puts "Error : ${message} is WRONG because number of ${Entity} entities in shape \"${shape}\" is ${nb_entity2}"
    } else {
      puts "OK : ${message} is GOOD because number of ${Entity} entities is equal to number of expected ${Entity} entities"
    }
  }

  if {[info exists isNotDeIges]} {
    set shapes_cur [dict get ${nbshapes_res} "SHAPE"]
    _comparewithmax isNotDeIges shapes_cur "NBSHAPES" 0 "Compare number of ${Entity} in command nbshapes with max possible number for DE test without iges shape"
  } elseif {[info exists isDeIges]} {
    set shapes_cur [dict get ${nbshapes_res} "SHAPE"]
    set comp_cur [dict get ${nbshapes_res} "COMPOUND"]
    set shape_comp_cur [expr ${shapes_cur} - ${comp_cur}]
    _comparewithmax isDeIges shape_comp_cur "NBSHAPES" 0 "Compare number of ${Entity} in command nbshapes with max possible number for DE test with iges shape"
  }
}

# Procedure to check equality of two reals with tolerance (relative and absolute)
help checkreal {
  Compare value with expected
  Use: checkreal name value expected tol_abs tol_rel
}
proc checkreal {name value expected tol_abs tol_rel} {
    if { abs ($value - $expected) > $tol_abs + $tol_rel * abs ($expected) } {
        puts "Error: $name = $value is not equal to expected $expected"
    } else {
        puts "Check of $name OK: value = $value, expected = $expected"
    }
    return
}

help checkfreebounds {
  Compare number of free edges with ref_value

  Use: checkfreebounds shape ref_value [options...]
  Allowed options are:
    -tol N: used tolerance (default -0.01)
    -type N: used type, possible values are "closed" and "opened" (default "closed")
}
proc checkfreebounds {shape ref_value args} {
  puts "checkfreebounds ${shape} ${ref_value} ${args}"
  upvar ${shape} ${shape}

  set tol -0.01
  set type "closed"

  set options {{"-tol" tol 1}
               {"-type" type 1}}

  _check_args ${args} ${options} "checkfreebounds"

  if {"$type" != "closed" && "$type" != "opened"} {
    error "Error : wrong -type key \"${type}\""
  }

  freebounds ${shape} ${tol}
  set free_edges [llength [explode ${shape}_[string range $type 0 0] e]]

  if { ${ref_value} == -1 } {
    puts "Error : Number of free edges is UNSTABLE"
    return
  }

  if { ${free_edges} != ${ref_value} } {
    puts "Error : Number of free edges is not equal to reference data"
  } else {
    puts "OK : Number of free edges is ${free_edges}"
  }
}

help checkmaxtol {
  Compare max and/or average tolerances of shape with reference values.
  Command returns max tolerance of the shape.

  Use: checkmaxtol shape [options...]
  Allowed options are:
    -tol: reference value of maximum tolerance.
    -avg: reference value of average tolerance.
    -source: list of shapes to compare with, e.g.: -source {shape1 shape2 shape3} (is used only for max tolerance).
    -max_tol: max possible value of max tolerance.
    -max_avg: max possible value of average tolerance.
    -multi_tol: tolerance multiplier (is used only for max tolerance).
}
proc checkmaxtol {shape args} {
  puts "checkmaxtol ${shape} ${args}"
  upvar ${shape} ${shape}

  set ref_tol 0
  set ref_avg 0
  set source_shapes {}
  set check_max_tol -1
  set check_max_avg -1
  set tol_multiplier 0

  set options {{"-max_tol" check_max_tol 1}
               {"-max_avg" check_max_avg 1}
               {"-multi_tol" tol_multiplier 1}
               {"-source" source_shapes 1}
               {"-tol" ref_tol 1}
               {"-avg" ref_avg 1}}

  _check_args ${args} ${options} "checkmaxtol"

  # get max tol of shape
  set max_tol 0
  set avg_tol 0
  if { ![regexp {Tolerance +MAX=([-0-9.+eE]+) +AVG=([-0-9.+eE]+)} [tolerance ${shape}] full max_tol avg_tol] } {
    error "Error: cannot get tolerances of shape \"${shape}\""
  }

  # find max tol of source shapes
  foreach source_shape ${source_shapes} {
    upvar ${source_shape} ${source_shape}
    set _src_max_tol [checkmaxtol ${source_shape}]
    if { [expr ${_src_max_tol} > ${check_max_tol} ] } {
      set check_max_tol ${_src_max_tol}
    }
  }
  # apply -multi_tol option
  if {${tol_multiplier}} {
    set check_max_tol [expr ${tol_multiplier} * ${check_max_tol}]
  }
  # compare max tol of source shapes with checking tolerance
  if { ${ref_tol} && [expr ${max_tol} > ${ref_tol}] } {
    if {([format %.7f [expr 1.*abs($max_tol - $ref_tol)]] > [format %.7f [expr 0.01 * $ref_tol]] && [format %.7f [expr 1. * ($max_tol - $ref_tol)]] > 0.0001) || ($max_tol == 0 && $ref_tol > 0)} {
      puts "Error: Max tolerance of ${shape} (${max_tol}) is greater than reference tolerance (${ref_tol})"
    }
  }
  # compare avg tol with checking tolerance
  if { ${ref_avg} && [expr ${avg_tol} > ${ref_avg}] } {
    if {([format %.7f [expr 1.*abs($avg_tol - $ref_avg)]] > [format %.7f [expr 0.01 * $ref_avg]] && [format %.7f [expr 1. * ($avg_tol - $ref_avg)]] > 0.0001) || ($avg_tol == 0 && $ref_avg > 0)} {
      puts "Error: Avg tolerance of ${shape} (${avg_tol}) is greater than reference tolerance (${ref_avg})"
    }
  }
  # compare max tol with checking max tolerance
  if { ${check_max_tol} != -1 } {
    if {([format %.7f [expr 1.*($max_tol - $check_max_tol)]] > [format %.7f [expr 0.0001 * $check_max_tol]] && [format %.7f [expr 1. * abs($check_max_tol-$max_tol)]] > 0.0000001) || ($max_tol == 0 && $check_max_tol > 0)} {
      puts "TOLERANCE : Faulty. Max tolerance of ${shape} (${max_tol}) is greater than max possible checking tolerance (${check_max_tol})"
    }
  }
  # compare avg tol with checking avg tolerance
  if { ${check_max_avg} != -1 } {
    if {([format %.7f [expr 1.*($avg_tol - $check_max_avg)]] > [format %.7f [expr 0.0001 * $check_max_avg]] && [format %.7f [expr 1. * abs($check_max_avg-$avg_tol)]] > 0.0000001) || ($avg_tol == 0 && $check_max_avg > 0)} {
      puts "TOLERANCE : Faulty. Avg tolerance of ${shape} (${max_tol}) is greater than max possible checking tolerance (${check_max_avg})"
    }
  }
  return ${max_tol}
}

help checkfaults {
  Compare faults number of given shapes.

  Use: checkfaults shape source_shape [ref_value=0]
}
proc checkfaults {shape source_shape {ref_value 0}} {
  puts "checkfaults ${shape} ${source_shape} ${ref_value}"
  upvar $shape $shape
  upvar $source_shape $source_shape
  set cs_a [checkshape $source_shape]
  set nb_a 0
  if {[regexp {Faulty shapes in variables faulty_([0-9]*) to faulty_([0-9]*)} $cs_a full nb_a_begin nb_a_end]} {
    set nb_a [expr $nb_a_end - $nb_a_begin +1]
  }
  set cs_r [checkshape $shape]
  set nb_r 0
  if {[regexp {Faulty shapes in variables faulty_([0-9]*) to faulty_([0-9]*)} $cs_r full nb_r_begin nb_r_end]} {
    set nb_r [expr $nb_r_end - $nb_r_begin +1]
  }
  puts "Number of faults for the initial shape is $nb_a."
  puts "Number of faults for the resulting shape is $nb_r."

  if { ${ref_value} == -1 } {
    puts "Error : Number of faults is UNSTABLE"
    return
  }

  if { $nb_r > $nb_a } {
    puts "Error : Number of faults is $nb_r"
  }
}

help ReadFileToDoc {
  Procedure to open data file as document.
  Uses 'locate_data_file' procedure to search the data file.
  Supported formats:
    - *.iges (*.igs)
    - *.stp (*.step)
  The search order is:
    - subdirectory "data" of the test script (grid) folder
    - subdirectories in environment variable CSF_TestDataPath
    - subdirectory set by datadir command
  If file is not found, raises Tcl error.

  Use: ReadFileToDoc filename [options...]
  Allowed options are:
    -doc D_First: variable to save opened as document data file (default document name - 'D_First').
    -productmode [0/1]: turn OFF/ON read.step.product.mode
}
proc ReadFileToDoc { filename args } {
  puts "ReadFileToDoc ${filename} ${args}"

  set DocName "D_First"
  set ProductMode ""

  set options {{"-doc" DocName 1}
               {"-productmode" ProductMode 1}}

  _check_args ${args} ${options} "ReadFileToDoc"

  upvar ${DocName} ${DocName}
  switch -exact [file extension ${filename}] {
    ".igs" -
    ".iges" { set ReadCommand ReadIges }
    ".step" -
    ".stp"  { set ReadCommand ReadStep }
  }
  switch -exact ${ProductMode} {
    "0" { param read.step.product.mode OFF; puts "param read.step.product.mode OFF" }
    "1" { param read.step.product.mode ON; puts "param read.step.product.mode ON" }
  }
  set path_file [locate_data_file ${filename}]
  if { [catch { ${ReadCommand} ${DocName} ${path_file} }] } {
    puts "Error: file ${filename} was not read (exception)"
  } else {
    puts "File '${filename}' was read into '${DocName}' document"
  }
}

help ReOpenDoc {
  Procedure to save document to the data file, close the document and reopen it.

  Supported formats:
    - *.iges (*.igs)
    - *.stp (*.step)

  Use: ReOpenDoc filename [options...]
  Allowed options are:
    -doc D_First: original document to reopen (default document name - 'D_First').
    -newdoc D_Second: name of new document (default document name - 'D_Second').
}
proc ReOpenDoc { filename args } {
  puts "ReOpenDoc ${filename} ${args}"

  upvar imagedir imagedir
  upvar casename casename

  set DocName "D_First"
  set NewDocName "D_Second"

  set options {{"-doc" DocName 1}
               {"-newdoc" NewDocName 1}}

  upvar ${DocName} ${DocName}
  upvar ${NewDocName} ${NewDocName}

  _check_args ${args} ${options} "ReOpenDoc"

  set ext [file extension ${filename}]
  switch -exact ${ext} {
    ".igs" -
    ".iges" { set ReadCommand ReadIges; set WriteCommand WriteIges }
    ".step" -
    ".stp"  { set ReadCommand ReadStep; set WriteCommand WriteStep }
  }
  file delete ${imagedir}/${casename}_D_First${ext}
  if {[catch { ${WriteCommand} ${DocName} ${imagedir}/${casename}_${DocName}${ext}}]} {
    puts "Error: First - file was not written - exception"
  } else {
    if {[catch { ${ReadCommand} ${NewDocName} ${imagedir}/${casename}_${DocName}${ext}}]} {
      puts "Error: Second - file was not read - exception"
    }
  }
  catch {[file delete ${imagedir}/${casename}_${DocName}${ext}]}

  # Close the document
  if { [catch { Close ${DocName} }] } {
    puts "Error : cannot close a document ${DocName} - exception"
  }
}

# auxiliary: check all arguments
proc _check_args { args {options {}} {command_name ""}} {
  # check arguments
  for {set narg 0} {${narg} < [llength ${args}]} {incr narg} {
    set arg [lindex ${args} ${narg}]
    set toContinue 0
    foreach option ${options} {
      set option_name            [lindex ${option} 0]
      set variable_to_save_value [lindex ${option} 1]
      set get_value              [lindex ${option} 2]
      if {${get_value} == ""} { set get_value 0 }
      set local_value ""
      if { [_check_arg ${option_name} local_value ${get_value}] } {
        upvar ${variable_to_save_value} ${variable_to_save_value}
        set ${variable_to_save_value} ${local_value}
        set toContinue 1
      }
    }
    if {${toContinue}} { continue }
    # unsupported option
    if { [regexp {^-} ${arg}] } {
      error "Error: unsupported option \"${arg}\""
    }
    error "Error: cannot interpret argument ${narg} (${arg})"
  }
  foreach option ${options} {
    set option_name            [lindex ${option} 0]
    set variable_to_save_value [lindex ${option} 1]
    set should_exist           [lindex ${option} 3]
    if {![info exists ${variable_to_save_value}] && ${should_exist} == 1} {
      error "Error: wrong using of command '${command_name}', '${option_name}' option is required"
    }
  }
}

help checkdoc {
  Compare number of faults and/or warnings in output of commands 'data c' and 'tpstat c'
  with reference values and/or with max possible values.
  Should be used after opening of some document.
  Use: checkdoc "data c" [options...]
    or
       checkdoc "tpstat c" [options...]
  Allowed options are:
    -warns N: reference number of warnings
    -faults N: reference number of faults
    -max_warns N: max possible number of warnings
    -max_faults N: max possible number of faults
}
proc checkdoc {command args} {
  puts "checkdoc ${command} ${args}"

  set options {{"-warns" ref_warns 1}
               {"-faults" ref_faults 1}
               {"-max_warns" max_warns 1}
               {"-max_faults" max_faults 1}}

  _check_args ${args} ${options} "checkdoc"

  switch -exact ${command} {
    "data c"   {set info [data c]; set errorMsg "DATA"}
    "tpstat c" {set info [tpstat c]; set errorMsg "TPSTAT"}
  }

  if { [llength ${info}] } {
    set warns 0
    set faults 0
    foreach line [split ${info} "\n"] {
      if {[regexp {[\t ]+([-0-9.+eE]+)[\t ]+([FW]):} $line full number type]} {
        switch -exact ${type} {
          "F" {incr faults ${number}}
          "W" {incr warns ${number}}
        }
      }
    }

    _comparewithmax warns ref_warns ${errorMsg} 1 "ref number of warnings in command ${command}"
    _comparewithmax faults ref_faults ${errorMsg} 1 "ref number of faults in command ${command}"

    _comparewithmax warns max_warns ${errorMsg} 0 "comparison ref number of warns in command ${command} with max possible"
    _comparewithmax faults max_faults ${errorMsg} 0 "comparison ref number of faults in command ${command} with max possible"
  } else {
    puts "Warning: ${command} checking was NOT provided"
  }
}

# auxiliary: puts Error status if value is bigger that maxvalue (if isEqual == 0)
#            puts Error status if values are not equal          (if isEqual == 1)
proc _comparewithmax {value maxvalue msg {isEqual 0} {description ""}} {
  upvar ${value} ${value}
  upvar ${maxvalue} ${maxvalue}

  set sign ">"
  if { ${isEqual} == 1 } {
    set sign "!="
  }

  if {[info exists ${value}] &&[info exists ${maxvalue}]} {
    if { [expr [set ${value}] ${sign} [set ${maxvalue}]]} {
      puts "${msg} : Faulty ${description}"
    } else {
      puts "${msg} : OK ${description}"
    }
  }
}

# auxiliary: puts Error status if value is bigger that maxvalue
proc _comparelists {value expected {msg "compare lists"} {description ""}} {
  upvar ${value} ${value}
  upvar ${expected} ${expected}
  if {[info exists ${value}] && [info exists ${expected}]} {
    if {[lsort [set ${value}]] != [lsort [set ${expected}]]} {
      puts "${msg} : Faulty ${description}"
    } else {
      puts "${msg} : OK ${description}"
    }
  }
}

help checkshapeff {
  Compare number of problem elements obtained by command 'checkshape aShape ff'
  with reference one or/and with max possible number of problem elements.
  Use: checkshapeff aShape [options...]
  Allowed options are:
    -wires N: reference number of problem wires
    -faces N: reference number of problem faces
    -shells N: reference number of problem shells
    -solids N: reference number of problem solids
    -max_wires N: max number of possible problem wires
    -max_faces N: max number of possible problem faces
    -max_shells N: max number of possible problem shells
    -max_solids N: max number of possible problem solids
}
proc checkshapeff { shape args} {
  puts "checkshapeff ${shape} ${args}"
  upvar ${shape} ${shape}

  set options {{"-wires" ref_wires 1}
               {"-faces" ref_faces 1}
               {"-shells" ref_shells 1}
               {"-solids" ref_solids 1}
               {"-max_wires" max_wires 1}
               {"-max_faces" max_faces 1}
               {"-max_shells" max_shells 1}
               {"-max_solids" max_solids 1}}

  _check_args ${args} ${options} "checkshapeff"

  set info [checkshape ${shape} ff]
  if { [llength ${info}] } {
    set cur_wires 0
    set cur_faces 0
    set cur_shells 0
    set cur_solids 0

    regexp {WIRE\t+: +([-0-9.+eE]+).+} $info full cur_wires
    regexp {FACE\t+: +([-0-9.+eE]+).+} $info full cur_faces
    regexp {SHELL\t+: +([-0-9.+eE]+).+} $info full cur_shells
    regexp {SOLID\t+: +([-0-9.+eE]+).+} $info full cur_solids

    set errorMsg "CHECKSHAPE"
    _comparewithmax cur_wires ref_wires ${errorMsg} 1 "ref number of wires in command checkshape ff"
    _comparewithmax cur_faces ref_faces ${errorMsg} 1 "ref number of faces in command checkshape ff"
    _comparewithmax cur_shells ref_shells ${errorMsg} 1 "ref number of shells in command checkshape ff"
    _comparewithmax cur_solids ref_solids ${errorMsg} 1 "ref number of solids in command checkshape ff"

    _comparewithmax cur_wires max_wires ${errorMsg} 0 "ref number of wires in command checkshape ff with max possible number"
    _comparewithmax cur_faces max_faces ${errorMsg} 0 "ref number of faces in command checkshape ff with max possible number"
    _comparewithmax cur_shells max_shells ${errorMsg} 0 "ref number of shells in command checkshape ff with max possible number"
    _comparewithmax cur_solids max_solids ${errorMsg} 0 "ref number of solids in command checkshape ff with max possible number"
  } else {
    puts "Warning: checkshape ff checking was NOT provided"
  }
}

help checkstatshape {
  Compare output of command statshape with given reference data

  Use: checkstatshape shape [options...]
  Allowed options are:
    -freeedge N: compare number of free edges with reference N
    -sharededge N: compare number of shared edges with reference N
    -freewire N: compare number of free wires with reference N
    -face N: compare number of faces with reference N
    -shell N: compare number of shells with reference N
    -solid N: compare number of solids with reference N

    -max_freeedge N: compare number of free edges with max possible number
    -max_sharededge N: compare number of shared edges with max possible number
    -max_freewire N: compare number of free wires with max possible number
    -max_face N: compare number of faces with max possible number
    -max_shell N: compare number of shells with max possible number
    -max_solid N: compare number of solids with max possible number
}
proc checkstatshape {shape args} {
  puts "checkstatshape ${shape} ${args}"
  upvar ${shape} ${shape}

  set ref_FreeEDGE -1
  set ref_SharedEDGE -1
  set ref_FreeWIRE -1
  set ref_FACE -1
  set ref_SHELL -1
  set ref_SOLID -1

  set options {{"-freeedge" ref_FreeEDGE 1}
               {"-sharededge" ref_SharedEDGE 1}
               {"-freewire" ref_FreeWIRE 1}
               {"-face" ref_FACE 1}
               {"-shell" ref_SHELL 1}
               {"-solid" ref_SOLID 1}
               {"-max_freeedge" max_FreeEDGE 1}
               {"-max_sharededge" max_SharedEDGE 1}
               {"-max_freewire" max_FreeWIRE 1}
               {"-max_face" max_FACE 1}
               {"-max_shell" max_SHELL 1}
               {"-max_solid" max_SOLID 1}}

  _check_args ${args} ${options} "checkstatshape"

  set info [statshape ${shape}]

            # Solid Shell Face FreeWire FreeEdge SharedEdge
  if { [llength ${info}] } {
    set cur_FreeEDGE 0
    set cur_SharedEDGE 0
    set cur_FreeWIRE 0
    set cur_FACE 0
    set cur_SHELL 0
    set cur_SOLID 0

    regexp {([-0-9.+eE]+)\t+ +EDGE \(Free\)} $info full cur_FreeEDGE
    regexp {([-0-9.+eE]+)\t+ +EDGE \(Shared\)} $info full cur_SharedEDGE
    regexp {([-0-9.+eE]+)\t+ +WIRE \(Free\)} $info full cur_FreeWIRE
    regexp {([-0-9.+eE]+)\t+ +FACE} $info full cur_FACE
    regexp {([-0-9.+eE]+)\t+ +SHELL} $info full cur_SHELL
    regexp {([-0-9.+eE]+)\t+ +SOLID} $info full cur_SOLID

    set errorMsg "STATSHAPE"
    _comparewithmax cur_FreeEDGE ref_FreeEDGE ${errorMsg} 1 "ref number of free edges in command statshape"
    _comparewithmax cur_SharedEDGE ref_SharedEDGE ${errorMsg} 1 "ref number of shared edges in command statshape"
    _comparewithmax cur_FreeWIRE ref_FreeWIRE ${errorMsg} 1 "ref number of free wires in command statshape"
    _comparewithmax cur_FACE ref_FACE ${errorMsg} 1 "ref number of faces in command statshape"
    _comparewithmax cur_SHELL ref_SHELL ${errorMsg} 1 "ref number of shells in command statshape"
    _comparewithmax cur_SOLID ref_SOLID ${errorMsg} 1 "ref number of solids in command statshape"

    _comparewithmax cur_FreeEDGE max_FreeEDGE ${errorMsg} 0 "ref number of free edges in command statshape with max possible number"
    _comparewithmax cur_SharedEDGE max_SharedEDGE ${errorMsg} 0 "ref number of shared edges in command statshape with max possible number"
    _comparewithmax cur_FreeWIRE max_FreeWIRE ${errorMsg} 0 "ref number of free wires in command statshape with max possible number"
    _comparewithmax cur_FACE max_FACE ${errorMsg} 0 "ref number of faces in command statshape with max possible number"
    _comparewithmax cur_SHELL max_SHELL ${errorMsg} 0 "ref number of shells in command statshape with max possible number"
    _comparewithmax cur_SOLID max_SOLID ${errorMsg} 0 "ref number of solids in command statshape with max possible number"
  } else {
    puts "Warning: statshape checking was NOT provided"
  }
}

help checkxstat {
  Compare output of command 'XStat docname' with given reference data

  Use: checkxstat docname [options...]
  Allowed options are:
    -n0 N: compare number of level N0 layers in command 'XStat ${docname}' with reference value
    -n1 N: compare number of level N1 layers in command 'XStat ${docname}' with reference value
    -n2 N: compare number of level N2 layers in command 'XStat ${docname}' with reference value
    -total N: compare number of layers in command 'XStat ${docname}' with reference value
    -withname N: compare number of layers with name in command 'XStat ${docname}' with reference value
    -withcolor N: compare number of layers with color link in command 'XStat ${docname}' with reference value
    -withlayer N: compare number of layers with layer link in command 'XStat ${docname}' with reference value
    -centroid N: compare number of Centroid Props in command 'XStat ${docname}' with reference value
    -volume N: compare number of Volume Props in command 'XStat ${docname}' with reference value
    -area N: compare number of Area Props in command 'XStat ${docname}' with reference value
    -nbcolors N: compare number of colors in command 'XStat ${docname}' with reference value
    -colors N: compare colors of layers in command 'XStat ${docname}' with reference value
    -nblayers N: compare number of layers in command 'XStat ${docname}' with reference value
    -layers N: compare number of layers in command 'XStat ${docname}' with reference value
  All options are independent and can be used in any order.
}
proc checkxstat { docname args } {
  puts "checkstatshape ${docname} ${args}"

  upvar ${docname} ${docname}

  set options {{"-n0" ref_n0 1}
               {"-n1" ref_n1 1}
               {"-n2" ref_n2 1}
               {"-total" ref_total 1}
               {"-withname" ref_withname 1}
               {"-withcolor" ref_withcolor 1}
               {"-withlayer" ref_withlayer 1}
               {"-centroid" ref_centroid 1}
               {"-volume" ref_volume 1}
               {"-area" ref_area 1}
               {"-nbcolors" ref_nbcolors 1}
               {"-colors" ref_colors 1}
               {"-nblayers" ref_nblayers 1}
               {"-layers" ref_layers 1}}

  _check_args ${args} ${options} "checkxstat"

  set info [ XStat ${docname} ]

  if { [llength ${info}] > 0 } {
    regexp {level +N +0 +: +([-0-9.+eE]+)} ${info} full n0
    regexp {level +N +1 +: +([-0-9.+eE]+)} ${info} full n1
    regexp {level +N +2 +: +([-0-9.+eE]+)} ${info} full n2
    regexp {Total +number +of +labels +for +shapes +in +the +document += +([-0-9.+eE]+)} ${info} full total
    regexp {Number +of +labels +with +name += +([-0-9.+eE]+)} ${info} full withname
    regexp {Number +of +labels +with +color +link += +([-0-9.+eE]+)} ${info} full withcolor
    regexp {Number +of +labels +with +layer +link += +([-0-9.+eE]+)} ${info} full withlayer

    regexp {Number +of +Centroid Props += +([-0-9.+eE]+)} ${info} full centroid
    regexp {Number +of +Volume Props += +([-0-9.+eE]+)} ${info} full volume
    regexp {Number +of +Area Props += +([-0-9.+eE]+)} ${info} full area

    regexp {Number +of +colors += +([-0-9.+eE]+)} ${info} full nbcolors
    regexp {Number +of +colors += +[-0-9.+eE]+\n([^\n]*)} ${info} full colors

    regexp {Number +of +layers += +([-0-9.+eE]+)} ${info} full nblayers
    regexp {Number +of +layers += +[-0-9.+eE]+\n([^\n]*)} ${info} full layers

    # compare with ref values
    _comparewithmax n0 ref_n0 "LABELS" 1 "ref number of level N0 layers in command 'XStat ${docname}'"
    _comparewithmax n1 ref_n1 "LABELS" 1 "ref number of level N1 layers in command 'XStat ${docname}'"
    _comparewithmax n2 ref_n2 "LABELS" 1 "ref number of level N2 layers in command 'XStat ${docname}'"
    _comparewithmax total ref_total "LABELS" 1 "ref number of layers in command 'XStat ${docname}'"
    _comparewithmax withname ref_withname "LABELS" 1 "ref number of layers with name in command 'XStat ${docname}'"
    _comparewithmax withcolor ref_withcolor "LABELS" 1 "ref number of layers with color link in command 'XStat ${docname}'"
    _comparewithmax withlayer ref_withlayer "LABELS" 1 "ref number of layers with layer link in command 'XStat ${docname}'"

    _comparewithmax centroid ref_centroid "PROPS" 1 "ref number of Centroid Props in command 'XStat ${docname}'"
    _comparewithmax volume ref_volume "PROPS" 1 "ref number of Volume Props in command 'XStat ${docname}'"
    _comparewithmax area ref_area "PROPS" 1 "ref number of Area Props in command 'XStat ${docname}'"

    _comparewithmax nbcolors ref_nbcolors "NCOLORS" 1 "ref number of colors in command 'XStat ${docname}'"
    _comparelists colors ref_colors "COLORS" "ref colors of layers in command 'XStat ${docname}'"

    _comparewithmax nblayers ref_nblayers "NLAYERS" 1 "ref number of layers in command 'XStat ${docname}'"
    _comparelists layers ref_layers "LAYERS" "ref number of layers in command 'XStat ${docname}'"
  } else {
    puts " DECAF information was NOT provided by command 'XStat ${docname}'"
  }
}

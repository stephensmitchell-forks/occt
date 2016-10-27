TCL Command Language {#user_guides__draw_tcl}
====================

@tableofcontents

The command language used in Draw is Tcl. Tcl documentation such as "TCL and the TK Toolkit" by John K. Ousterhout (Addison-Wesley) will prove useful if you intend to use Draw extensively. 

This chapter is designed to give you a short outline of both the TCL language and some extensions included in Draw. The following topics are covered: 

  * Syntax of the TCL language.
  * Accessing variables in TCL and Draw.
  * Control structures.
  * Procedures.

@section occt_draw_2_2 Syntax of TCL

TCL is an interpreted command language, not a structured language like C, Pascal, LISP or Basic. It uses a shell similar to that of csh. TCL is, however, easier to use than csh because control structures and procedures are easier to define. As well, because TCL does not assign a process to each command, it is faster than csh. 

The basic program for TCL is a script. A script consists of one or more commands. Commands are separated by new lines or semicolons. 

~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~{.cpp}
set a 24 
set b 15 
set a 25; set b 15 
~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

Each command consists of one or more *words*; the first word is the name of a command and additional words are arguments to that command. 

Words are separated by spaces or tabs. In the preceding example each of the four commands has three words. A command may contain any number of words and each word is a string of arbitrary length. 

The evaluation of a command by TCL is done in two steps. In the first step, the command is parsed and broken into words. Some substitutions are also performed. In the second step, the command procedure corresponding to the first word is called and the other words are interpreted as arguments. In the first step, there is only string manipulation, The words only acquire *meaning* in the second step by the command procedure. 

The following substitutions are performed by TCL: 

Variable substitution is triggered by the $ character (as with csh), the content of the variable is substitued; { } may be used as in csh to enclose the name of the variable. 

**Example:** 
~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~{.cpp}
# set a variable value 
set file documentation 
puts $file #to display file contents on the screen 

# a simple substitution, set psfile to documentation.ps 
set psfile $file.ps 
puts $psfile 

# another substitution, set pfile to documentationPS 
set pfile ${file}PS 

# a last one, 
# delete files NEWdocumentation and OLDdocumentation 
foreach prefix {NEW OLD} {rm $prefix$file} 
~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

Command substitution is triggered by the [ ] characters. The brackets must enclose a valid script. The script is evaluated and the result is substituted. 

Compare command construction in csh. 

**Example:** 
~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~{.cpp}
set degree 30 
set pi 3.14159265 
# expr is a command evaluating a numeric expression 
set radian [expr $pi*$degree/180] 
~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

Backslash substitution is triggered by the backslash character. It is used to insert special characters like $, [ , ] , etc. It is also useful to insert a new line, a backslash terminated line is continued on the following line. 

TCL uses two forms of *quoting* to prevent substitution and word breaking. 

Double quote *quoting* enables the definition of a string with space and tabs as a single word. Substitutions are still performed inside the inverted commas " ". 

**Example:** 
~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~{.cpp}
# set msg to ;the price is 12.00; 
set price 12.00 
set msg ;the price is $price; 
~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

Braces *quoting* prevents all substitutions. Braces are also nested. The main use of braces is to defer evaluation when defining procedures and control structures. Braces are used for a clearer presentation of TCL scripts on several lines. 

**Example:** 
~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~{.cpp}
set x 0 
# this will loop for ever 
# because while argument is ;0  3; 
while ;$x  3; {set x [expr $x+1]} 
# this will terminate as expected because 
# while argument is {$x  3} 
while {$x  3} {set x [expr $x+1]} 
# this can be written also 
while {$x  3} { 
set x [expr $x+1] 
} 
# the following cannot be written 
# because while requires two arguments 
while {$x  3} 
{ 
set x [expr $x+1] 
} 
~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

Comments start with a \# character as the first non-blank character in a command. To add a comment at the end of the line, the comment must be preceded by a semi-colon to end the preceding command. 

**Example:** 
~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~{.cpp}
# This is a comment 
set a 1 # this is not a comment 
set b 1; # this is a comment 
~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

The number of words is never changed by substitution when parsing in TCL. For example, the result of a substitution is always a single word. This is different from csh but convenient as the behavior of the parser is more predictable. It may sometimes be necessary to force a second round of parsing. **eval** accomplishes this: it accepts several arguments, concatenates them and executes the resulting script. 


**Example:** 
~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
# I want to delete two files 

set files ;foo bar; 

# this will fail because rm will receive only one argument 
# and complain that ;foo bar; does not exit 

exec rm $files 

# a second evaluation will do it 
~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

@section occt_draw_2_3 Accessing variables in TCL and Draw

TCL variables have only string values. Note that even numeric values are stored as string literals, and computations using the **expr** command start by parsing the strings. Draw, however, requires variables with other kinds of values such as curves, surfaces or topological shapes. 

TCL provides a mechanism to link user data to variables. Using this functionality, Draw defines its variables as TCL variables with associated data. 

The string value of a Draw variable is meaningless. It is usually set to the name of the variable itself. Consequently, preceding a Draw variable with a <i>$</i> does not change the result of a command. The content of a Draw variable is accessed using appropriate commands. 

There are many kinds of Draw variables, and new ones may be added with C++. Geometric and topological variables are described below. 

Draw numeric variables can be used within an expression anywhere a Draw command requires a numeric value. The *expr* command is useless in this case as the variables are stored not as strings but as floating point values. 

**Example:** 
~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~{.cpp}
# dset is used for numeric variables 
# pi is a predefined Draw variable 
dset angle pi/3 radius 10 
point p radius*cos(angle) radius*sin(angle) 0 
~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
It is recommended that you use TCL variables only for strings and Draw for numerals. That way, you will avoid the *expr* command. As a rule, Geometry and Topology require numbers but no strings. 

@subsection occt_draw_2_3_1 set, unset

Syntax:                  

~~~~~
set varname [value] 
unset varname [varname varname ...] 
~~~~~

*set* assigns a string value to a variable. If the variable does not already exist, it is created. 

Without a value, *set* returns the content of the variable. 

*unset* deletes variables. It is is also used to delete Draw variables. 

**Example:** 
~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~{.cpp}
set a "Hello world"
set b "Goodbye" 
set a 
== "Hello world" 
unset a b 
set a 
~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

**Note**, that the *set* command can set only one variable, unlike the *dset* command. 


@subsection occt_draw_2_3_2 dset, dval

Syntax

~~~~~
dset var1 value1 vr2 value2 ... 
dval name 
~~~~~

*dset* assigns values to Draw numeric variables. The argument can be any numeric expression including Draw numeric variables. Since all Draw commands expect a numeric expression, there is no need to use $ or *expr*. The *dset* command can assign several variables. If there is an odd number of arguments, the last variable will be assigned a value of 0. If the variable does not exist, it will be created. 

*dval* evaluates an expression containing Draw numeric variables and returns the result as a string, even in the case of a single variable. This is not used in Draw commands as these usually interpret the expression. It is used for basic TCL commands expecting strings. 


**Example:** 
~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~{.cpp}
# z is set to 0 
dset x 10 y 15 z 
== 0 

# no $ required for Draw commands 
point p x y z 

# "puts" prints a string 
puts ;x = [dval x], cos(x/pi) = [dval cos(x/pi)]; 
== x = 10, cos(x/pi) = -0.99913874099467914 
~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

**Note,** that in TCL, parentheses are not considered to be special characters. Do not forget to quote an expression if it contains spaces in order to avoid parsing different words. <i>(a + b)</i> is parsed as three words: <i>"(a + b)"</i> or <i>(a+b)</i> are correct.


@section occt_draw_2_4 lists

TCL uses lists. A list is a string containing elements separated by spaces or tabs. If the string contains braces, the braced part accounts as one element. 

This allows you to insert lists within lists. 

**Example:** 
~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~{.cpp}
# a list of 3 strings 
;a b c; 

# a list of two strings the first is a list of 2 
;{a b} c; 
~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

Many TCL commands return lists and **foreach** is a useful way to create loops on list elements. 

@subsection occt_draw_2_5 Control Structures

TCL allows looping using control structures. The control structures are implemented by commands and their syntax is very similar to that of their C counterparts (**if**, **while**, **switch**, etc.). In this case, there are two main differences between TCL and C: 

* You use braces instead of parentheses to enclose conditions. 
* You do not start the script on the next line of your command. 


@subsection occt_draw_2_5_1 if

Syntax       

~~~~~
if condition script [elseif script .... else script] 
~~~~~

**If** evaluates the condition and the script to see whether the condition is true. 



**Example:** 
~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~{.cpp}
if {$x  0} { 
puts ;positive; 
} elseif {$x == 0} { 
puts ;null; 
} else { 
puts ;negative; 
} 
~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

@subsection occt_draw_2_5_2 while, for, foreach

Syntax:                  


~~~~~~
while condition script 
for init condition reinit script 
foreach varname list script 
~~~~~

The three loop structures are similar to their C or csh equivalent. It is important to use braces to delay evaluation. **foreach** will assign the elements of the list to the variable before evaluating the script. \

**Example:** 
~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~{.cpp}
# while example 
dset x 1.1 
while {[dval x]  100} { 
  circle c 0 0 x 
  dset x x*x 
} 
# for example 
# incr var d, increments a variable of d (default 1) 
for {set i 0} {$i  10} {incr i} { 
  dset angle $i*pi/10 
  point p$i cos(angle0 sin(angle) 0 
} 
# foreach example 
foreach object {crapo tomson lucas} {display $object} 
~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

@subsection occt_draw_2_5_3 break, continue

Syntax:                  

~~~~~
break 
continue 
~~~~~

Within loops, the **break** and **continue** commands have the same effect as in C. 

**break** interrupts the innermost loop and **continue** jumps to the next iteration. 

**Example:** 
~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~{.cpp}
# search the index for which t$i has value ;secret; 
for {set i 1} {$i = 100} {incr i} { 
  if {[set t$i] == ;secret;} break; 
} 
~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

@section occt_draw_2_6 Procedures

TCL can be extended by defining procedures using the **proc** command, which sets up a context of local variables, binds arguments and executes a TCL script. 

The only problematic aspect of procedures is that variables are strictly local, and as they are implicitly created when used, it may be difficult to detect errors. 

There are two means of accessing a variable outside the scope of the current procedures: **global** declares a global variable (a variable outside all procedures); **upvar** accesses a variable in the scope of the caller. Since arguments in TCL are always string values, the only way to pass Draw variables is by reference, i.e. passing the name of the variable and using the **upvar** command as in the following examples. 

As TCL is not a strongly typed language it is very difficult to detect programming errors and debugging can be tedious. TCL procedures are, of course, not designed for large scale software development but for testing and simple command or interactive writing. 


@subsection occt_draw_2_6_1 proc

Syntax:

~~~~~
proc argumentlist script 
~~~~~

**proc** defines a procedure. An argument may have a default value. It is then a list of the form {argument value}. The script is the body of the procedure. 

**return** gives a return value to the procedure. 

**Example:** 
~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~{.cpp}
# simple procedure 
proc hello {} { 
  puts ;hello world; 
} 
# procedure with arguments and default values 
proc distance {x1 y1 {x2 0} {y2 0}} { 
  set d [expr (x2-x1)*(x2-x1) + (y2-y1)*(y2-y1)] 
  return [expr sqrt(d)] 
} 
proc fact n { 
  if {$n == 0} {return 1} else { 
    return [expr n*[fact [expr n -1]]] 
  } 
} 
~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~


@subsection occt_draw_2_6_2 global, upvar

Syntax:                 

~~~~~
global varname [varname ...] 
upvar varname localname [varname localname ...] 
~~~~~


**global** accesses high level variables. Unlike C, global variables are not visible in procedures. 

**upvar** gives a local name to a variable in the caller scope. This is useful when an argument is the name of a variable instead of a value. This is a call by reference and is the only way to use Draw variables as arguments. 

**Note** that in the following examples the \$ character is always necessarily used to access the arguments.
 
**Example:** 
~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~{.cpp}
# convert degree to radian 
# pi is a global variable 
proc deg2rad (degree} { 
  return [dval pi*$degree/2.] 
} 
# create line with a point and an angle 
proc linang {linename x y angle} { 
  upvar linename l 
  line l $x $y cos($angle) sin($angle) 
}
~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~


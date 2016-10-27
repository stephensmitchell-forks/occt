Documentation System {#contribution__documentation}
====================

@tableofcontents

This document provides practical guidelines for generation and editing of OCCT user documentation.

@section docsys_prerequisites Prerequisites

You need to have the following software installed to generate the documentation.

**Tcl/Tk**
Version 8.5 or 8.6: http://www.tcl.tk/software/tcltk/download.html

**Doxygen**
Version 1.8.4 or above: http://www.stack.nl/~dimitri/doxygen/download.html

**Dot**
Part of Graphviz software, used by Doxygen for generation of class diagrams in Reference Manual: http://www.graphviz.org/Download..php

**MiKTeX** or other package providing **pdflatex** command (only needed for generation of PDF documents): http://miktex.org/download

**Inkscape** (only needed for generation of PDF documents containing SVG images): http://www.inkscape.org/download

@note When generating PDF documentation, **pdflatex** and **inkscape** executables should be accessible by PATH variable.
You can use *custom.bat* file to add necessary paths to the *PATH* variable. 

In the process of PDF generation MiKTeX may need some packages not installed by default.
We recommend setting option "Install missing packages on-the-fly" to "Ask me first" (default) during MiKTeX installation:

@figure{/contribution/documentation/images/documentation_miktex.png}

On the first run of **pdflatex** it will open a dialog window prompting for installation of missing packages.
Follow the instructions to proceed (define proxy settings if needed, select a mirror site to download from, etc.).

**MathJax** is used for rendering math formulas in browser (HTML and CHM outputs): http://www.mathjax.org.

By default MathJAX scripts and fonts work on-line and no installation of MathJAX is necessary if Internet is accessible.
If you need to use OCCT documentation while off-line, you can install a local copy of MatJAX, see
 https://docs.mathjax.org/en/v2.6-latest/start.html#installing-your-own-copy-of-mathjax.

@section docsys_generation Generation

Run command *gendoc* from command prompt (with OCCT directory as current one) to generate OCCT documentation.
The synopsis is:

    gendoc \[-h\] {-refman|-overview} \[-html|-pdf|-chm\] \[-m=<list of modules>|-ug=<list of docs>\] \[-v\] \[-s=<search_mode>\] \[-mathjax=<path>\]
	
Here the options are:

* Choice of documentation to be generated:
  * <i>-overview</i>: To generate Overview and User Guides (cannot be used with -refman)
  * <i>-refman</i>: To generate class Reference Manual (cannot be used with -overview)

* Choice of output format:
  * <i>-html</i>: To generate HTML files (default, cannot be used with -pdf or -chm)
  * <i>-pdf</i>: To generate PDF files (cannot be used with -refman, -html, or -chm)
  * <i>-chm</i>: To generate CHM files (cannot be used with -html or -pdf)

* Additional options:
  * <i>-m=\<modules_list\></i>: List of OCCT modules (separated with comma), for generation of Reference Manual
  * <i>-ug=\<docs_list\></i>: List of MarkDown documents (separated with comma), to use for generation of
 Overview / User Guides
  * <i>-mathjax=\<path\></i>: To use local or alternative copy of MathJax
  * <i>-s=\<search_mode\></i>: Specifies the Search mode of HTML documents; can be: none | local | server | external
  * <i>-h</i>: Prints this help message
  * <i>-v</i>: Enables more verbose output

@note In case of PDF output the utility generates a separate PDF file for each document;
@note In case of HTML output the utility generates a common Table of contents containing references to all documents.
@note In case of CHM output single CHM file is generated

**Examples**

To generate the output for a specific document specify the path to the corresponding MarkDown file (paths relative to
 *dox* sub-folder can be given), for instance:

~~~~
    > gendoc -overview -ug=dev_guides/documentation/documentation.md
~~~~

To generate Reference Manual for the whole Open CASCADE Technology library, run: 
~~~~
    > gendoc -refman
~~~~

To generate Reference Manual for Foundation Classes and Modeling Data modules only, with search option, run:
~~~~
    > gendoc -refman -m=FoundationClasses,ModelingData,ModelingAlgorithms -s=local
~~~~

@section docsys_conventions Conventions

This section contains information about file format conventions, directories structure, etc.

The format used for documentation is MarkDown with Doxygen extensions. 
The MarkDown files have a <i>*.md</i> extension and are based on syntax provided by \ref anchor_docsys_conventions_syntax 
Markdown, Doxygen and Latex.

@figure{/contribution/documentation/images/documentation_folders.png}

Each document has its own folder if there are any images used in it. These images are stored in *images* subfolder.

If you want to use the same image for several documents, you can place it in *dox/resources* folder.

@note To avoid incorrect image display, use a relative path to the image (starting from *dox* folder). For instance:
@verbatim
@figure{/contribution/documentation/images/documentation_test_image.svg}
@endverbatim

The documentation is generated in subfolder *doc* :
* *html* -- a directory for generated HTML pages;
* *pdf* -- a directory for generated PDF files.

##New Document

Place a new document in the folder taking into account its logical position in the documentation hierarchy. If there are
 images in the document, it should be placed in its own folder containing a subfolder for images

Add a relative path to newly created file in <i>dox/FILES_*.txt</i> file(s). For instance

@note The order of paths to documents in *FILES_*.txt* is reproduced in the <i>Table of Contents</i> in the HTML output.
 Please, place them logically.

\anchor anchor_docsys_conventions_syntax 
##Documentation Syntax

* Information about Doxygen syntax can be found at: http://www.stack.nl/~dimitri/doxygen/manual

* Information on MarkDown syntax can be found at: https://daringfireball.net/projects/markdown/syntax#list

* Formulas within MarkDown documents can be defined using LaTeX syntax, see the following resources:

    https://latex-project.org/guides/

    http://en.wikipedia.org/wiki/Help:Displaying_a_formula

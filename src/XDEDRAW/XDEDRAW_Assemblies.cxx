// Created on: 2017-08-22
// Created by: Sergey SLYADNEV
// Copyright (c) 2017 OPEN CASCADE SAS
//
// This file is part of Open CASCADE Technology software library.
//
// This library is free software; you can redistribute it and/or modify it under
// the terms of the GNU Lesser General Public License version 2.1 as published
// by the Free Software Foundation, with special exception defined in the file
// OCCT_LGPL_EXCEPTION.txt. Consult the file LICENSE_LGPL_21.txt included in OCCT
// distribution for complete text of the license and disclaimer of any warranty.
//
// Alternatively, this file may be used under the terms of Open CASCADE
// commercial license or contractual agreement.

// Own include
#include <XDEDRAW_Assemblies.hxx>

// Other OCCT includes
#include <DDocStd.hxx>
#include <Draw.hxx>
#include <NCollection_Vector.hxx>
#include <XDEDRAW_DrawableAssemblyGraph.hxx>

// Standard includes
#include <vector>

//-----------------------------------------------------------------------------

std::vector< std::pair<TCollection_AsciiString, XCAFDoc_AssemblyGraph::NodeType> >
  XAssembly_NodeTypes = {
    {"-roots",           XCAFDoc_AssemblyGraph::NodeType_Root},
    {"-subassemblies",   XCAFDoc_AssemblyGraph::NodeType_Subassembly},
    {"-partoccurrences", XCAFDoc_AssemblyGraph::NodeType_PartOccurrence},
    {"-parts",           XCAFDoc_AssemblyGraph::NodeType_Part}
  };

//-----------------------------------------------------------------------------

//! Returns true if the passed command line option specifies node type.
//! \param[in] opt option to check.
//! \return true/false.
bool XAssembly_IsKeywordNodeType(const TCollection_AsciiString& opt)
{
  for ( size_t s = 0; s < XAssembly_NodeTypes.size(); ++s )
    if ( opt == XAssembly_NodeTypes[s].first )
      return true;

  return false;
}

//-----------------------------------------------------------------------------

//! Checks if the given graph node is of expected type.
//! \param[in] what     node to check.
//! \param[in] opt      expected type.
//! \param[in] asmGraph assembly graph.
//! \return true/false.
bool XAssembly_IsOfExpectedType(const int                            n,
                                const TCollection_AsciiString&       opt,
                                const Handle(XCAFDoc_AssemblyGraph)& asmGraph)
{
  const XCAFDoc_AssemblyGraph::NodeType actualType = asmGraph->GetNodeType(n);

  // Check against registered types
  TCollection_AsciiString expectedOpt;
  for ( size_t t = 0; t < XAssembly_NodeTypes.size(); ++t )
  {
    if ( actualType == XAssembly_NodeTypes[t].second )
    {
      expectedOpt = XAssembly_NodeTypes[t].first;
      break;
    }
  }
  if ( expectedOpt.IsEmpty() )
    return false;

  return (expectedOpt == opt);
}

//-----------------------------------------------------------------------------

//! Builds assembly graph for the given model.
//! \param[in] di   Draw interpreter instance.
//! \param[in] argc number of arguments.
//! \param[in] argv list of arguments.
//! \return execution result.
static int XAssemblyGraph(Draw_Interpretor& di, int argc, const char** argv)
{
  // Preliminary checks
  if ( argc != 4 )
  {
    std::cout << "Incorrect number of arguments. Check help for details..." << std::endl;
    return 1; // Failure
  }

  // Get document
  Handle(TDocStd_Document) doc;
  DDocStd::GetDocument(argv[1], doc);
  if ( doc.IsNull() )
  {
    di << argv[1] << " is not a document\n"; return 1;
  }

  // Whether to include parts to assembly graph or not
  const bool withParts = ( atoi(argv[2]) > 0 );

  // Construct assembly graph
  Handle(XCAFDoc_AssemblyGraph) asmGraph = new XCAFDoc_AssemblyGraph(doc, withParts);

  // Register assembly graph in Draw
  Draw::Set( argv[3], new XDEDRAW_DrawableAssemblyGraph(asmGraph) );

  return 0; // Success
}

//-----------------------------------------------------------------------------

//! Checks assembly graph.
//! \param[in] di   Draw interpreter instance.
//! \param[in] argc number of arguments.
//! \param[in] argv list of arguments.
//! \return execution result.
static int XAssemblyGraphCheck(Draw_Interpretor& di, int argc, const char** argv)
{
  // Preliminary checks
  if ( argc < 3 )
  {
    std::cout << "Incorrect number of arguments. Check help for details..." << std::endl;
    return 1; // Failure
  }

  // Get assembly graph
  Handle(XDEDRAW_DrawableAssemblyGraph)
    DAG = Handle(XDEDRAW_DrawableAssemblyGraph)::DownCast( Draw::Get(argv[1]) );
  //
  if ( DAG.IsNull() )
  {
    std::cout << "Error: Drawable Assembly Graph is NULL" << std::endl;
    return 1; // Failure
  }
  //
  Handle(XCAFDoc_AssemblyGraph) asmGraph = DAG->GetGraph();

  // Get summary
  int numRoots         = 0;
  int numSubassemblies = 0;
  int numPartInstances = 0;
  int numParts         = 0;
  //
  asmGraph->CalculateSummary(numRoots, numSubassemblies, numPartInstances, numParts);

  // Check according to the argument keys
  for ( int i = 2; i < argc; ++i )
  {
    TCollection_AsciiString opt(argv[i]);
    opt.LowerCase();

    // Check the number of graph nodes
    if ( opt == "-numnodes" )
    {
      const int num = atoi( argv[++i] );
      //
      if ( num != asmGraph->GetNumberOfNodes() )
      {
        di << 0; // FALSE
        std::cout << "Error: unexpected number of nodes" << std::endl;
        return 0;
      }
    }

    // Check the number of graph arcs
    else if ( opt == "-numarcs" )
    {
      const int num = atoi( argv[++i] );
      //
      if ( num != asmGraph->GetNumberOfArcs() )
      {
        di << 0; // FALSE
        std::cout << "Error: unexpected number of arcs" << std::endl;
        return 0;
      }
    }

    // Check the number of roots
    else if ( opt == "-numroots" )
    {
      const int num = atoi( argv[++i] );
      //
      if ( num != numRoots )
      {
        di << 0; // FALSE
        std::cout << "Error: unexpected number of roots" << std::endl;
        return 0;
      }
    }

    // Check the number of subassemblies
    else if ( opt == "-numsubassemblies" )
    {
      const int num = atoi( argv[++i] );
      //
      if ( num != numSubassemblies )
      {
        di << 0; // FALSE
        std::cout << "Error: unexpected number of subassemblies" << std::endl;
        return 0;
      }
    }

    // Check the number of part occurrences
    else if ( opt == "-numpartoccurrences" )
    {
      const int num = atoi( argv[++i] );
      //
      if ( num != numPartInstances )
      {
        di << 0; // FALSE
        std::cout << "Error: unexpected number of part occurrences" << std::endl;
        return 0;
      }
    }

    // Check the number of parts
    else if ( opt == "-numparts" )
    {
      const int num = atoi( argv[++i] );
      //
      if ( num != numParts )
      {
        di << 0; // FALSE
        std::cout << "Error: unexpected number of parts" << std::endl;
        return 0;
      }
    }

    // Check individual arc
    else if ( opt == "-arc" )
    {
      const int n1 = atoi( argv[++i] );
      const int n2 = atoi( argv[++i] );
      //
      if ( !asmGraph->HasArc(n1, n2) )
      {
        di << 0; // FALSE
        std::cout << "Error: arc (" << n1 << ", " << n2 << ") does not exist" << std::endl;
        return 0;
      }
    }

    // Check individual node types
    else if ( XAssembly_IsKeywordNodeType(opt) )
    {
      do
      {
        // Get node index
        const int n = atoi( argv[++i] );

        // Check type
        if ( !XAssembly_IsOfExpectedType(n, opt, asmGraph) )
        {
          di << 0; // FALSE
          std::cout << "Error: unexpected type for node " << n << std::endl;
          return 0;
        }
      }
      while ( i < (argc - 1) && !XAssembly_IsKeywordNodeType(argv[i + 1]) );
    }
  }
  di << 1; // TRUE

  return 0; // Success
}

//=======================================================================
//function : InitCommands
//purpose  : 
//=======================================================================

void XDEDRAW_Assemblies::InitCommands(Draw_Interpretor& di)
{
  static Standard_Boolean initactor = Standard_False;
  if (initactor)
  {
    return;
  }
  initactor = Standard_True;

  Standard_CString grp = "XDE assembly commands";

  di.Add("XAssemblyGraph", "XAssemblyGraph doc withParts graph",
    __FILE__, XAssemblyGraph, grp);

  di.Add("XAssemblyGraphCheck", "XAssemblyGraphCheck [-numnodes           num] "
                                                    "[-numarcs            num] "
                                                    "[-numroots           num] "
                                                    "[-numsubassemblies   num] "
                                                    "[-numpartoccurrences num] "
                                                    "[-numparts           num] "
                                                    "[-arc                n1 n2] "
                                                    "[-arc                n2 n3] "
                                                    " ... "
                                                    "[-roots              n1 n2 ...] "
                                                    "[-subassemblies      n3 n4 ...] "
                                                    "[-partoccurrences    n5 n6 ...] "
                                                    "[-parts              n7 n8 ...] ",
    __FILE__, XAssemblyGraphCheck, grp);
}

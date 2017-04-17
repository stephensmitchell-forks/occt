
#include <DDocStd.ixx>
#include <BRep_Builder.hxx>
#include <DBRep.hxx>
#include <Draw_Interpretor.hxx>
#include <FSD_File.hxx>
#include <FSD_CmpFile.hxx>
#include <FSD_BinaryFile.hxx>
#include <MgtBRep.hxx>
#include <NCollection_Handle.hxx>
#include <NCollection_DataMap.hxx>
#include <PCDM_ReadWriter.hxx>
#include <PTColStd_TransientPersistentMap.hxx>
#include <PTColStd_PersistentTransientMap.hxx>
#include <PTopoDS_HShape.hxx>
#include <ShapeSchema.hxx>
#include <Storage_Data.hxx>
#include <Storage_HSeqOfRoot.hxx>
#include <Storage_Root.hxx>
#include <TCollection_ExtendedString.hxx>
#include <TopTools_SequenceOfShape.hxx>
#include <TopoDS_Shape.hxx>

//=======================================================================
//function : DDocStd_ShapeSchema_Write 
//=======================================================================

static Standard_Integer DDocStd_ShapeSchema_Write(Draw_Interpretor& di, 
                                                  Standard_Integer n, 
                                                  const char** a)
{
  if (n < 3)
  {
    di << "Usage : ShapeSchema_Write shapes filename [gen | cmp | bin]\n";
    di << "        Arguments:\n";
    di << "        shapes   : list os shape names\n";
    di << "        filename : output file name\n";
    di << "        Storage driver:\n";
    di << "          gen : FSD_File driver (default)\n";
    di << "          cmp : FSD_CmpFile driver\n";
    di << "          bin : FSD_BinaryFile driver\n";
    return 1;
  }

  NCollection_Handle<Storage_BaseDriver> aFileDriver(new FSD_File);
  
  Standard_Boolean hasStorageDriver = Standard_False;
  Standard_Integer iArgN = n - 1;

  if (strncmp(a[iArgN], "gen", 3) == 0)
  {
    aFileDriver = new FSD_File;
    hasStorageDriver = Standard_True;
  }
  else if (strncmp(a[iArgN], "cmp", 3) == 0)
  {
    aFileDriver = new FSD_CmpFile;
    hasStorageDriver = Standard_True;
  }
  else if (strncmp(a[iArgN], "bin", 3) == 0)
  {
    aFileDriver = new FSD_BinaryFile;
    hasStorageDriver = Standard_True;
  }

  if (hasStorageDriver) --iArgN;

  Storage_Error aStatus = aFileDriver->Open(a[iArgN], Storage_VSWrite);
  if (aStatus != Storage_VSOk) {
    di << "Error : couldn't open file '" << "' for writing (" << aStatus << ")\n";
    return 1;
  }

  TopTools_SequenceOfShape aShapes;
  NCollection_DataMap<TCollection_AsciiString, Standard_Integer> aShapeNames;
  for (Standard_Integer i = 1; i < iArgN; ++i)
  {
    TopoDS_Shape aShape = DBRep::Get(a[i]);
    if (aShape.IsNull())
    {
      di << "Error : null shape " << a[i] << "\n";
      return 1;
    }
    aShapes.Append(aShape);
    if (aShapeNames.IsBound(a[i]))
      aShapeNames.ChangeFind(a[i]) += 1;
    else
      aShapeNames.Bind(a[i], 1);
  }

  Handle(ShapeSchema) aSchema = new ShapeSchema;
  Handle(Storage_Data) aData = new Storage_Data;

  aData->SetApplicationName(TCollection_ExtendedString("DDocStd_ShapeSchema_Write"));

  PTColStd_TransientPersistentMap aMap;
  for (Standard_Integer i = 1; i <= aShapes.Length(); ++i)
  {
    TopoDS_Shape aShape = aShapes.Value(i);
    Handle(PTopoDS_HShape) aPShape =
      MgtBRep::Translate(aShape, aMap, MgtBRep_WithTriangle);
    if (aPShape.IsNull())
    {
      di << "Error : couldn't translate shape " << a[i] << "\n";
      return 1;
    }
    TCollection_AsciiString aName = a[i];
    if (aShapeNames.IsBound(aName))
    {
      Standard_Integer n = aShapeNames.Find(a[i]);
      if (n > 1)
      {
        aName += "_";
        aName += n;
      }
    }
    aData->AddRoot(aName, aPShape);
  }

  aSchema->Write(*aFileDriver, aData);

  aFileDriver->Close();

  return 0;
}

//=======================================================================
//function : DDocStd_ShapeSchema_Read 
//=======================================================================

static Standard_Integer DDocStd_ShapeSchema_Read(Draw_Interpretor& di, 
                                                 Standard_Integer n, 
                                                 const char** a)
{
  if (n < 3)
  {
    di << "Usage : ShapeSchema_Read filename shape\n";
    di << "        Arguments:\n";
    di << "        filename : input file name\n";
    di << "        shape    : name of an output shape,\n";
    di << "                   root shapes will be put into a compound\n";
    di << "                   in case of multiple roots in the file\n";
    return 1;
  }

  // Create a driver appropriate for the given file
  NCollection_Handle<Storage_BaseDriver> aFileDriver;
  if (FSD_CmpFile::IsGoodFileType(TCollection_AsciiString(a[1])) == Storage_VSOk)
    aFileDriver = new FSD_CmpFile;
  else if (FSD_File::IsGoodFileType(TCollection_AsciiString(a[1])) == Storage_VSOk)
    aFileDriver = new FSD_File;
  else if (FSD_BinaryFile::IsGoodFileType(TCollection_AsciiString(a[1])) == Storage_VSOk)
    aFileDriver = new FSD_BinaryFile;
  else 
  {
    di << "Error : unknown storage driver\n";
    return 1;
  }

  PCDM_ReadWriter::Open(*aFileDriver, a[1], Storage_VSRead);

  Handle(ShapeSchema) aSchema = new ShapeSchema;
  Handle(Storage_Data) aData = aSchema->Read(*aFileDriver);
  Handle(Storage_HSeqOfRoot) aRoots = aData->Roots();

  di << "Info : " << aRoots->Length() << " root(s)\n";

  TopTools_SequenceOfShape aShapes;

  PTColStd_PersistentTransientMap aMap;
  for (Standard_Integer i = 1; i <= aRoots->Length(); ++i)
  {
    Handle(Storage_Root) aRoot = aRoots->Value(i);
    Handle(Standard_Persistent) aPObject = aRoot->Object();

    Handle(PTopoDS_HShape) aPShape = Handle(PTopoDS_HShape)::DownCast(aPObject);
    if (aPShape.IsNull())
    {
      di << "Warning : not a shape at [" << i << "] root\n";
      continue;
    }

    TopoDS_Shape aShape;
    MgtBRep::Translate(aPShape, aMap, aShape, MgtBRep_WithTriangle);

    if (aShape.IsNull())
    {
      di << "Error : persistent shape translation at [" << i << "] root\n";
      return 1;
    }

    aShapes.Append(aShape);
  }

  di << "Info : " << aShapes.Length() << " shape(s) translated\n";

  if (aShapes.Length() > 1)
  {
    BRep_Builder aB;
    TopoDS_Compound aC;
    aB.MakeCompound(aC);
    for (Standard_Integer i = 1; i <= aShapes.Length(); ++i)
      aB.Add(aC, aShapes.Value(i));
    DBRep::Set(a[2], aC);
  }
  else
    DBRep::Set(a[2], aShapes.First());

  return 0;
}

//=======================================================================
//function : ShapeSchemaCommands
//purpose  : registers shape schema related commands in Draw interpreter
//=======================================================================

void DDocStd::ShapeSchemaCommands(Draw_Interpretor& theCommands)
{
  static Standard_Boolean done = Standard_False;
  if (done) return;
  done = Standard_True;

  const char* g = "DDocStd commands";

  theCommands.Add("ShapeSchema_Write", 
    "ShapeSchema_Write shape filename [driver]",
    __FILE__, DDocStd_ShapeSchema_Write, g);

  theCommands.Add("ShapeSchema_Read",
    "ShapeSchema_Read filename shape",
    __FILE__, DDocStd_ShapeSchema_Read, g);

}

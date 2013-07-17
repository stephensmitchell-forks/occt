// Copyright (c) 2013 OPEN CASCADE SAS
//
// The content of this file is subject to the Open CASCADE Technology Public
// License Version 6.5 (the "License"). You may not use the content of this file
// except in compliance with the License. Please obtain a copy of the License
// at http://www.opencascade.org and read it completely before using this file.
//
// The Initial Developer of the Original Code is Open CASCADE S.A.S., having its
// main offices at: 1, place des Freres Montgolfier, 78280 Guyancourt, France.
//
// The Original Code and all software distributed under the License is
// distributed on an "AS IS" basis, without warranty of any kind, and the
// Initial Developer hereby disclaims all such warranties, including without
// limitation, any warranties of merchantability, fitness for a particular
// purpose or non-infringement. Please see the License for the specific terms
// and conditions governing the rights and limitations under the License.

#ifndef _IGESData_IGESReaderTool_HeaderFile
#define _IGESData_IGESReaderTool_HeaderFile

#include <Standard.hxx>
#include <Standard_DefineAlloc.hxx>
#include <Standard_Macro.hxx>
#include <Handle_Interface_ParamList.hxx>
#include <IGESData_FileRecognizer.hxx>
#include <Interface_GeneralLib.hxx>
#include <Interface_ReaderLib.hxx>
#include <Standard_Integer.hxx>
#include <IGESData_IGESType.hxx>
#include <IGESData_ReadStage.hxx>
#include <Handle_Interface_Check.hxx>
#include <Standard_Real.hxx>
#include <Interface_FileReaderTool.hxx>
#include <Handle_IGESData_IGESReaderData.hxx>
#include <Handle_IGESData_Protocol.hxx>
#include <Standard_Boolean.hxx>
#include <Handle_Standard_Transient.hxx>
#include <Handle_Interface_InterfaceModel.hxx>
#include <Handle_IGESData_IGESEntity.hxx>

class Interface_ParamList;
class IGESData_FileRecognizer;
class Interface_Check;
class IGESData_IGESReaderData;
class IGESData_Protocol;
class Standard_Transient;
class Interface_InterfaceModel;
class IGESData_IGESEntity;
class IGESData_DirPart;
class IGESData_ParamReader;


//! specific FileReaderTool for IGES <br>
//!           Parameters are accessed through specific objects, ParamReaders <br>
class IGESData_IGESReaderTool  : public Interface_FileReaderTool
{
public:

  DEFINE_STANDARD_ALLOC

  //! creates IGESReaderTool to work with an IGESReaderData and an <br>
//!           IGES Protocol. <br>
//!           Actually, no Lib is used <br>
  Standard_EXPORT   IGESData_IGESReaderTool(const Handle(IGESData_IGESReaderData)& reader,const Handle(IGESData_Protocol)& protocol);
  //! binds empty entities to records, works with the Protocol <br>
//!           (from IGESData) stored and later used <br>
//!           RQ : Actually, sets DNum into IGES Entities <br>
//!           Also loads the list of parameters for ParamReader <br>
  Standard_EXPORT     void Prepare(const Handle(IGESData_FileRecognizer)& reco) ;
  //! recognizes records by asking Protocol (on data of DirType) <br>
  Standard_EXPORT     Standard_Boolean Recognize(const Standard_Integer num,Handle(Interface_Check)& ach,Handle(Standard_Transient)& ent) ;
  //! fills model's header, that is, its GlobalSection <br>
  Standard_EXPORT     void BeginRead(const Handle(Interface_InterfaceModel)& amodel) ;
  //! fills an entity, given record no; works by calling ReadDirPart <br>
//!           then ReadParams (with help of a ParamReader), then if required <br>
//!           ReadProps and ReadAssocs, from IGESEntity <br>
//!           Returns True if no fail has been recorded <br>
  Standard_EXPORT     Standard_Boolean AnalyseRecord(const Standard_Integer num,const Handle(Standard_Transient)& anent,Handle(Interface_Check)& acheck) ;
  //! after reading entities, true line weights can be computed <br>
  Standard_EXPORT   virtual  void EndRead(const Handle(Interface_InterfaceModel)& amodel) ;
  //! Reads directory part componants from file; DP is the litteral <br>
//!           directory part, IR detains entities referenced by DP <br>
  Standard_EXPORT     void ReadDir(const Handle(IGESData_IGESEntity)& ent,const Handle(IGESData_IGESReaderData)& IR,const IGESData_DirPart& DP,Handle(Interface_Check)& ach) const;
  //! Performs Reading of own Parameters for each IGESEntity <br>
//!           Works with the ReaderLib loaded with ReadWriteModules for IGES <br>
//!           In case of failure, tries UndefinedEntity from IGES <br>
  Standard_EXPORT     void ReadOwnParams(const Handle(IGESData_IGESEntity)& ent,const Handle(IGESData_IGESReaderData)& IR,IGESData_ParamReader& PR) const;
  //! Reads Property List, if there is (if not, does nothing) <br>
//!           criterium is : current parameter of PR remains inside params <br>
//!           list, and Stage is "Own" <br>
//!           Current parameter must be a positive integer, which value <br>
//!           gives the length of the list; else, a Fail is produced (into <br>
//!           Check of PR) and reading process is stopped <br>
  Standard_EXPORT     void ReadProps(const Handle(IGESData_IGESEntity)& ent,const Handle(IGESData_IGESReaderData)& IR,IGESData_ParamReader& PR) const;
  //! Reads Associativity List, if there is (if not, does nothing) <br>
//!           criterium is : current parameter of PR remains inside params <br>
//!           list, and Stage is "Own" <br>
//!           Same conditions as above; in addition, no parameter must be <br>
//!           let after the list once read <br>
//!           Note that "Associated" entities are not declared "Shared" <br>
  Standard_EXPORT     void ReadAssocs(const Handle(IGESData_IGESEntity)& ent,const Handle(IGESData_IGESReaderData)& IR,IGESData_ParamReader& PR) const;





protected:





private:



Handle_Interface_ParamList thelist;
Handle_IGESData_FileRecognizer thereco;
Interface_GeneralLib theglib;
Interface_ReaderLib therlib;
Standard_Integer thecnum;
IGESData_IGESType thectyp;
IGESData_ReadStage thestep;
Handle_Interface_Check thechk;
Standard_Integer thegradweight;
Standard_Real themaxweight;
Standard_Real thedefweight;


};





// other Inline functions and methods (like "C++: function call" methods)


#endif

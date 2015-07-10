#include <Voxel_DicomReader.hxx>

#pragma comment(lib, "gdcmCommon.lib")
#pragma comment(lib, "gdcmDICT.lib")
#pragma comment(lib, "gdcmcharls.lib")
#pragma comment(lib, "gdcmDSED.lib")
#pragma comment(lib, "gdcmexpat.lib")
#pragma comment(lib, "gdcmgetopt.lib")
#pragma comment(lib, "gdcmIOD.lib")
#pragma comment(lib, "gdcmjpeg8.lib")
#pragma comment(lib, "gdcmjpeg12.lib")
#pragma comment(lib, "gdcmjpeg16.lib")
#pragma comment(lib, "gdcmMEXD.lib")
#pragma comment(lib, "gdcmMSFF.lib")
#pragma comment(lib, "gdcmopenjpeg.lib")
#pragma comment(lib, "gdcmzlib.lib")
#pragma comment(lib, "socketxx.lib")


//=========================================
//function : Voxel_DicomReader
//purpose  :
//=========================================
Voxel_DicomReader::Voxel_DicomReader()
{
}

#include <NCollection_Array1.hxx>
#include <NCollection_Handle.hxx>

namespace
{
  template<class T, bool IsCorrect>
  struct DicomTypeCheker
  {
    /* None */
  };

  template<class T>
  struct DicomTypeCheker<T, true>
  {
    typedef T Type;
  };

  template<int Format>
  struct DicomTypeSelector
  {
    /* None */
  };

  template<>
  struct DicomTypeSelector<gdcm::PixelFormat::UINT8>
  {
    typedef DicomTypeCheker<unsigned char, sizeof (unsigned char) == 1>::Type Type;
  };

  template<>
  struct DicomTypeSelector<gdcm::PixelFormat::UINT16>
  {
    typedef DicomTypeCheker<unsigned short, sizeof (unsigned short) == 2>::Type Type;
  };

  template<>
  struct DicomTypeSelector<gdcm::PixelFormat::UINT32>
  {
    typedef DicomTypeCheker<unsigned int, sizeof (unsigned int) == 4>::Type Type;
  };

  template<>
  struct DicomTypeSelector<gdcm::PixelFormat::FLOAT32>
  {
    typedef DicomTypeCheker<float, sizeof (float) == 4>::Type Type;
  };

  class DenseVolumeFiller
  {
  public:

    virtual void Push (const Standard_Address theBuffer,
                       const Standard_Integer theIndexZ) = 0;

    virtual Graphic3d_VolumeData* Data() const = 0;
  };

  template<int Format>
  class SpecificDenseVolumeFiller : public DenseVolumeFiller
  {
  public:

    typedef typename DicomTypeSelector<Format>::Type DataType;

  public:

    SpecificDenseVolumeFiller (const BVH_Box3d&       theBounds,
                               const Standard_Integer theNbVoxX,
                               const Standard_Integer theNbVoxY,
                               const Standard_Integer theNbVoxZ)
    {
      myData = new Graphic3d_DenseVolumeData<DataType> (theBounds,
                                                        theNbVoxX,
                                                        theNbVoxY,
                                                        theNbVoxZ);
    }

    virtual void Push (const Standard_Address theBuffer,
                       const Standard_Integer theIndexZ)
    {
      const Standard_Integer aNbSliceElems = myData->NbVoxelX() *
                                             myData->NbVoxelY();

      memcpy (&myData->Data()->ChangeFirst() +
        aNbSliceElems * theIndexZ, theBuffer, aNbSliceElems * sizeof (DataType));
    }

    virtual Graphic3d_VolumeData* Data() const
    {
      return myData;
    }

  private:

    //! Dense volumetric data of specific type.
    Graphic3d_DenseVolumeData<DataType>* myData;

  };
}

//========================================================================
//function : ReadDenseVolumeData
//purpose  :
//========================================================================
Standard_Boolean Voxel_DicomReader::ReadDenseVolumeData (Handle(Graphic3d_VolumeData)&   theVolumeData,
                                                         const TCollection_AsciiString&  thePath) const
{
  gdcm::ImageReader               aDicomReader;
  gdcm::Directory                 aFolder;

  aFolder.Load (thePath.ToCString());

  Standard_ASSERT_RETURN (!aFolder.GetFilenames().empty(),
    "Error: There are no any files in the directory", Standard_False);

  // read first file to get dataset parameters
  aDicomReader.SetFileName (aFolder.GetFilenames().front().c_str());

  Standard_ASSERT_RETURN (aDicomReader.Read(),
    "Error: Failed to read DICOM image", Standard_False);

  gdcm::Image& anImage = aDicomReader.GetImage();

  Standard_ASSERT_RETURN (!anImage.IsEmpty(),
    "Error: DICOM image is empty", Standard_False);

  const Standard_Integer  aDimX = anImage.GetDimension (0);
  const Standard_Integer  aDimY = anImage.GetDimension (1);
  const gdcm::PixelFormat aType = anImage.GetPixelFormat();

  BVH_Box3d aBounds (BVH_Vec3d (anImage.GetOrigin (0), anImage.GetOrigin (1), anImage.GetOrigin (2)),
                     BVH_Vec3d (anImage.GetOrigin (0), anImage.GetOrigin (1), anImage.GetOrigin (2)));

  aBounds.CornerMax().x() += aDimX * anImage.GetSpacing (0);
  aBounds.CornerMax().y() += aDimY * anImage.GetSpacing (1);

  NCollection_Handle<DenseVolumeFiller> aVolumeFiller = NULL;

  switch (aType)
  {
    case gdcm::PixelFormat::UINT8:
    {
      aVolumeFiller = new SpecificDenseVolumeFiller<gdcm::PixelFormat::UINT8> (
        aBounds, aDimX, aDimY, static_cast<Standard_Integer> (aFolder.GetFilenames().size()));
    } break;

    case gdcm::PixelFormat::UINT16:
    {
      aVolumeFiller = new SpecificDenseVolumeFiller<gdcm::PixelFormat::UINT16> (
        aBounds, aDimX, aDimY, static_cast<Standard_Integer> (aFolder.GetFilenames().size()));
    } break;

    case gdcm::PixelFormat::UINT32:
    {
      aVolumeFiller = new SpecificDenseVolumeFiller<gdcm::PixelFormat::UINT32> (
        aBounds, aDimX, aDimY, static_cast<Standard_Integer> (aFolder.GetFilenames().size()));
    } break;

    case gdcm::PixelFormat::FLOAT32:
    {
      aVolumeFiller = new SpecificDenseVolumeFiller<gdcm::PixelFormat::FLOAT32> (
        aBounds, aDimX, aDimY, static_cast<Standard_Integer> (aFolder.GetFilenames().size()));
    } break;
  }

  if (aVolumeFiller.IsNull())
  {
    return Standard_False; // unsupported pixel format
  }

  double aSizeZ = 0.0;
  for (std::vector<std::string>::const_iterator anIt = aFolder.GetFilenames().begin(); anIt != aFolder.GetFilenames().end(); ++anIt)
  {
    if (anIt != aFolder.GetFilenames().begin())
    {
      // Sometimes gdcm::ImageReader::GetImage() throw exception.
      // This magic solves this trouble
      gdcm::File* aFile = new gdcm::File;
      aDicomReader.SetFile (*aFile);
      aDicomReader.SetFileName (anIt->c_str());
      if (!aDicomReader.CanRead())
      {
        continue;
      }

      std::cout << anIt->c_str() << std::endl;

      Standard_ASSERT_RETURN (aDicomReader.Read(),
        "Error: Failed to read DICOM image", Standard_False);

      anImage = aDicomReader.GetImage();

      Standard_ASSERT_RETURN (!anImage.IsEmpty(),
        "Error: DICOM image is empty", Standard_False);

      if (aDimX != static_cast<Standard_Integer>(anImage.GetDimension (0))
       || aDimY != static_cast<Standard_Integer>(anImage.GetDimension (1))
       || aType != anImage.GetPixelFormat())
      {
        return Standard_False;
      }
    }

    aSizeZ += anImage.GetSpacing (2);

    if (anIt == aFolder.GetFilenames().end() - 1)
    {
      //aBounds.CornerMax().z() = anImage.GetOrigin (2); // TODO: ?
      aBounds.CornerMax().z() = Max (anImage.GetOrigin (2), aBounds.CornerMax().z() + aSizeZ);
    }

    const gdcm::PhotometricInterpretation& aPI = anImage.GetPhotometricInterpretation();

    if (aPI.GetType() == gdcm::PhotometricInterpretation::MONOCHROME1
     || aPI.GetType() == gdcm::PhotometricInterpretation::MONOCHROME2)
    {
      Standard_Character* aBuffer = new Standard_Character[anImage.GetBufferLength()];

      if (!anImage.GetBuffer (aBuffer))
      {
        return Standard_False;
      }

      aVolumeFiller->Push (aBuffer, anIt - aFolder.GetFilenames().begin());

      delete [] aBuffer;
    }
    else
    {
      return Standard_False;
    }
  }

  theVolumeData = aVolumeFiller->Data();

  theVolumeData->SetBounds (aBounds);

  return Standard_True;
}

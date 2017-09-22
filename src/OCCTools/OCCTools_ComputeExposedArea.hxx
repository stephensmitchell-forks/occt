// File: OCCTools_ComputeExposedArea.hxx
// Created: 20.02.2017
// Author: Mikhail Sazonov

#ifndef OCCTools_ComputeExposedArea_hxx
#define OCCTools_ComputeExposedArea_hxx

#include <Standard_TypeDef.hxx>
#include <TopTools_ListOfShape.hxx>

class Message_ProgressIndicator;

//! This class computes fusion of inputs shapes
//! and calculates the area of the result (exposed area).
//! It looks only for solid shapes in input arguments. If there are no solids
//! it looks for faces and tries to create closed shells and then solids of them.
//! The class has the following options:
//! - *Gluing mode* - allows speeding up the calculation on the touching arguments;
//!                   In this mode the area computation will be performed on the faces,
//!                   forming the closed shells, not on the solids. It will help to avoid
//!                   extra computations and, most importantly, possible errors while
//!                   constructing the solids;
//! - *Fuzzy mode* - additional tolerance for the operation to make the arguments
//!                  interfere when its appropriate.
//!
class OCCTools_ComputeExposedArea
{
public:

  //! Enumerates error statuses
  enum ErrorStatus
  {
    Status_OK = 0,           //!< No error
    Status_BadInput = 0x01,  //!< Bad input, no solids are found
    Status_FuseError = 0x02  //!< Failure of computation of fused shape
  };

  //! Empty constructor
  Standard_EXPORT OCCTools_ComputeExposedArea();

  //! Constructor with input shapes
  Standard_EXPORT OCCTools_ComputeExposedArea(const TopTools_ListOfShape& theShapes);

  //! Adds an input shape. If there are no solids it tries to make solids from faces.
  //! Returns false if the input does not contain solids or faces constituting closed shells.
  Standard_EXPORT Standard_Boolean AddShape(const TopoDS_Shape& theShape);

  //! Returns the number of recognized input solids
  Standard_Integer GetNbOfSolids() const
  {
    return myShapes.Extent();
  }

  //! Returns the list of input solid shapes.
  const TopTools_ListOfShape& GetShapes() const
  {
    return myShapes;
  }

  //! Sets the mode of gluing, which tells that no face-face intersection
  //! needs to be performed
  void SetIsGluing(const Standard_Boolean theVal)
  {
    myIsGluing = theVal;
  }

  //! Sets fuzzy value. It allows ignoring gaps of the given size between shapes.
  void SetFuzzyValue(const Standard_Real theVal)
  {
    myFuzzyValue = theVal;
  }

  //! Performs the computations.
  //! Returns the combination of error statuses (see ErrorStatus)
  Standard_EXPORT Standard_Integer Perform(const Handle(Message_ProgressIndicator)& theProgress =
                                           Handle(Message_ProgressIndicator)());

  //! Returns the fused shape
  const TopoDS_Shape& GetFusedShape() const
  {
    return myFusedShape;
  }

  //! Returns the sum of areas of inputs shapes
  Standard_Real GetSumArea() const
  {
    return mySumArea;
  }

  //! Returns the exposed area
  Standard_Real GetArea() const
  {
    return myArea;
  }

private:

  TopTools_ListOfShape myShapes;
  TopoDS_Shape myFusedShape;
  Standard_Real myFuzzyValue;
  Standard_Real mySumArea;
  Standard_Real myArea;
  Standard_Boolean myIsGluing;
};

#endif

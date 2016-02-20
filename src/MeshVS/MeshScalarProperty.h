// Copyright (c) 2015 OPEN CASCADE SAS
//
// This file is part of commercial software by OPEN CASCADE SAS.
//
// This software is furnished in accordance with the terms and conditions
// of the contract and with the inclusion of this copyright notice.
// This software or any other copy thereof may not be provided or otherwise
// be made available to any third party.
// No ownership title to the software is transferred hereby.
//
// OPEN CASCADE SAS makes no representation or warranties with respect to the
// performance of this software, and specifically disclaims any responsibility
// for any damages, special or consequential, connected with its use.

#ifndef MeshScalarProperty_H
#define MeshScalarProperty_H

#include <gp_XYZ.hxx>
#include <Graphic3d_Vec.hxx>
#include <NCollection_Vector.hxx>
#include <NCollection_Sequence.hxx>
#include <Precision.hxx>
#include <TCollection_AsciiString.hxx>

//! Return true for NaN.
inline bool IsNaN (const double theValue)
{
#if defined(_MSC_VER)
  return ::_isnan (theValue) != 0;
#else
  return std::isnan (theValue);
#endif
}

//! Property assigned to Mesh elements or nodes
class MeshScalarProperty : public Standard_Transient
{

public:

  //! Return property name.
  const TCollection_AsciiString& Name() const { return myName; }

  //! Return lower index.
  Standard_Integer LowerIndex() const { return myValues.Lower(); }

  //! Return upper index.
  Standard_Integer UpperIndex() const { return myValues.Upper(); }

  //! Return value for specified index.
  Standard_Real Value (const Standard_Integer theIndex) const { return myValues.Value (theIndex); }

  //! Return lower value of the property.
  Standard_Real LowerValue() const { return myValueLower; }

  //! Return lower value of the property.
  Standard_Real UpperValue() const { return myValueUpper; }

  //! Return true if values has opposite meaning (greater value is better).
  bool IsInversed() const { return myIsInversed; }

  //! Setup flag inficating that values has opposite meaning (greater value is better) or normal meaning (greater value is badder).
  void SetInversed(const bool theIsInversed) { myIsInversed = theIsInversed; }

  //! Return colorscale range (from).
  Standard_Real ColorscaleRangeFrom() const { return myCSRangeFrom; }

  //! Return colorscale range (to).
  Standard_Real ColorscaleRangeTo()   const { return myCSRangeTo; }

  //! Setup colorscale range.
  void SetColorscaleRange (const Standard_Real theFrom,
                           const Standard_Real theTo)
  {
    myCSRangeFrom = theFrom;
    myCSRangeTo   = theTo;
  }

public:

  //! Default constructor.
  MeshScalarProperty (const TCollection_AsciiString& theName)
  : myName       (theName),
    myValueLower ( Precision::Infinite()),
    myValueUpper (-Precision::Infinite()),
    myCSRangeFrom(0.0),
    myCSRangeTo  (1.0),
    myIsInversed (false)
  {}

  //! Append new value
  void AppendValue (const Standard_Real theValue)
  {
    myValues.Append ((float )theValue);

    if (!IsNaN (theValue))
    {
      myValueLower = Min (myValueLower, theValue);
      myValueUpper = Max (myValueUpper, theValue);
    }
  }

protected:

  TCollection_AsciiString           myName;       //!< property name
  //NCollection_Vector<Standard_Real> myValues;     //!< property values
  NCollection_Vector<float>         myValues;     //!< property values
  Standard_Real                     myValueLower; //!< values range - lower value
  Standard_Real                     myValueUpper; //!< values range - upper value
  Standard_Real                     myCSRangeFrom;//!< colorscale range - from
  Standard_Real                     myCSRangeTo;  //!< colorscale range - to
  bool                              myIsInversed; //!< values meaning

public:

  DEFINE_STANDARD_RTTIEXT(MeshScalarProperty, Standard_Transient)

};

DEFINE_STANDARD_HANDLE(MeshScalarProperty, Standard_Transient)

typedef NCollection_Vector<Handle(MeshScalarProperty)> MeshScalarProperties;

#endif // MeshScalarProperty_H

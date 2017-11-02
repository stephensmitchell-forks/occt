// Copyright (c) 1999-2014 OPEN CASCADE SAS
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

#ifndef _Graphic3d_CLight_HeaderFile
#define _Graphic3d_CLight_HeaderFile

#include <Graphic3d_TypeOfLightSource.hxx>
#include <Graphic3d_Vec.hxx>
#include <NCollection_List.hxx>
#include <NCollection_Shared.hxx>
#include <Quantity_ColorRGBA.hxx>

//! Generic light source definition.
//! This class defines arbitrary light source - see Graphic3d_TypeOfLightSource enumeration.
//! Some parameters are applicable only to particular light type;
//! calling methods unrelated to current type will throw an exception.
class Graphic3d_CLight : public Standard_Transient
{
  DEFINE_STANDARD_RTTIEXT(Graphic3d_CLight, Standard_Transient)
public:

  //! Empty constructor, which should be followed by light source properties configuration.
  Standard_EXPORT Graphic3d_CLight (Graphic3d_TypeOfLightSource theType);

  //! Returns the Type of the Light, cannot be changed after object construction.
  Graphic3d_TypeOfLightSource Type() const { return myType; }

  //! Returns the color of the light source.
  const Quantity_Color& Color() const { return myColor.GetRGB(); }

  //! Defines the color of a light source by giving the basic color.
  void SetColor (const Quantity_Color& theColor) { myColor.SetRGB (theColor); }

  //! Check that the light source is turned on; TRUE by default.
  //! This flag affects all occurrences of light sources, where it was registered and activated;
  //! so that it is possible defining an active light in View which is actually in disabled state.
  Standard_Boolean IsEnabled() const { return myIsEnabled; }

  //! Change enabled state of the light state.
  //! This call does not remove or deactivate light source in Views/Viewers;
  //! instead it turns it OFF so that it just have no effect.
  void SetEnabled (Standard_Boolean theIsOn) { myIsEnabled = theIsOn; }

  //! Returns true if the light is a headlight; FALSE by default.
  //! Headlight flag means that light position/direction are defined not in a World coordinate system, but relative to the camera orientation.
  Standard_Boolean IsHeadlight() const { return myIsHeadlight; }

  //! Alias for IsHeadlight().
  Standard_Boolean Headlight() const { return myIsHeadlight; }

  //! Setup headlight flag.
  void SetHeadlight (Standard_Boolean theValue) { myIsHeadlight = theValue; }

//! @name positional/spot light properties
public:

  //! Returns location of positional/spot light.
  const Graphic3d_Vec3d& Position() const { return myPosition; }

  //! Setup location of positional/spot light.
  Standard_EXPORT void SetPosition (const Graphic3d_Vec3d& thePosition);

  //! Returns constant attenuation factor of positional/spot light source; 0.0f by default.
  //! Distance attenuation factors of reducing positional/spot light intensity depending on the distance from its position:
  //! @code
  //!   float anAttenuation = 1.0 / (ConstAttenuation() + LinearAttenuation() * theDistance + QuadraticAttenuation() * theDistance * theDistance);
  //! @endcode
  Standard_ShortReal ConstAttenuation()  const { return myParams.x(); }

  //! Defines coefficient of constant attenuation; value should be >= 0.0.
  Standard_EXPORT void SetConstAttenuation (Standard_ShortReal theConstAttenuation);

  //! Returns linear attenuation factor of positional/spot light source; 0.0 by default.
  //! Distance attenuation factors of reducing positional/spot light intensity depending on the distance from its position:
  //! @code
  //!   float anAttenuation = 1.0 / (ConstAttenuation() + LinearAttenuation() * theDistance + QuadraticAttenuation() * theDistance * theDistance);
  //! @endcode
  Standard_ShortReal LinearAttenuation() const { return myParams.y(); }

  //! Defines coefficient of linear attenuation; value should be >= 0.0.
  Standard_EXPORT void SetLinearAttenuation (Standard_ShortReal theLinearAttenuation);

  //! Returns the attenuation factors.
  void Attenuation (Standard_Real& theConstAttenuation,
                    Standard_Real& theLinearAttenuation) const
  {
    theConstAttenuation  = ConstAttenuation();
    theLinearAttenuation = LinearAttenuation();
  }

  //! Defines the coefficients of attenuation; values should be >= 0.0.
  void SetAttenuation (Standard_Real theConstAttenuation,
                       Standard_Real theLinearAttenuation)
  {
    SetConstAttenuation ((float )theConstAttenuation);
    SetLinearAttenuation((float )theLinearAttenuation);
  }

//! @name directional/spot light additional properties
public:

  //! Returns direction of directional/spot light.
  Graphic3d_Vec3 Direction() const { return myDirection.xyz(); }

  //! Returns direction of directional/spot light.
  Standard_EXPORT void SetDirection (const Graphic3d_Vec3& theDirection);

//! @name spotlight additional definition parameters
public:

  //! Returns an angle in radians of the cone created by the spot.
  Standard_ShortReal Angle() const { return myParams.z(); }

  //! Angle in radians of the cone created by the spot, should be within range (0.0, M_PI).
  Standard_EXPORT void SetAngle (Standard_ShortReal theAngle);

  //! Returns intensity distribution of the spot light, within [0.0, 1.0] range; 0.0 by default.
  //! This coefficient should be converted into spotlight exponent within [0.0, 128.0] range:
  //! @code
  //!   float aSpotExponent = Concentration() * 128.0;
  //!   anAttenuation *= pow (aCosA, aSpotExponent);"
  //! @endcode
  Standard_ShortReal Concentration() const { return myParams.w(); }

  //! Defines the coefficient of concentration; value should be within range [0.0, 1.0].
  Standard_EXPORT void SetConcentration (Standard_ShortReal theConcentration);

//! @name Ray-Tracing / Path-Tracing light properties
public:

  //! Returns the intensity of light source; 1.0 by default.
  Standard_ShortReal Intensity() const { return myIntensity; }

  //! Modifies the intensity of light source, which should be > 0.0.
  Standard_EXPORT void SetIntensity (Standard_ShortReal theValue);

  //! Returns the smoothness of light source (either smoothing angle for directional light or smoothing radius in case of positional light); 0.0 by default.
  Standard_ShortReal Smoothness() const { return mySmoothness; }

  //! Modifies the smoothing radius of positional/spot light; should be >= 0.0.
  Standard_EXPORT void SetSmoothRadius (Standard_ShortReal theValue);

  //! Modifies the smoothing angle (in radians) of directional light source; should be within range [0.0, M_PI/2].
  Standard_EXPORT void SetSmoothAngle (Standard_ShortReal theValue);

//! @name low-level access methods
public:

  //! Packed light parameters.
  const Graphic3d_Vec4& PackedParams() const { return myParams; }

  //! Returns the color of the light source with dummy Alpha component, which should be ignored.
  const Graphic3d_Vec4& PackedColor() const { return myColor; }

  //! Returns direction of directional/spot light.
  const Graphic3d_Vec4& PackedDirection() const { return myDirection; }

protected:

  //! Access positional/spot light constant attenuation coefficient from packed vector.
  Standard_ShortReal& changeConstAttenuation()  { return myParams.x();  }

  //! Access positional/spot light linear attenuation coefficient from packed vector.
  Standard_ShortReal& changeLinearAttenuation() { return myParams.y();  }

  //! Access spotlight angle parameter from packed vector.
  Standard_ShortReal& changeAngle()             { return myParams.z();  }

  //! Access spotlight concentration parameter from packed vector.
  Standard_ShortReal& changeConcentration()     { return myParams.w();  }

protected:

  Graphic3d_Vec3d             myPosition;    //!< light position
  Quantity_ColorRGBA          myColor;       //!< light color
  Graphic3d_Vec4              myDirection;   //!< direction of directional/spot light
  Graphic3d_Vec4              myParams;      //!< packed light parameters
  Standard_ShortReal          mySmoothness;  //!< radius for point light or cone angle for directional light
  Standard_ShortReal          myIntensity;   //!< intensity multiplier for light
  Graphic3d_TypeOfLightSource myType;        //!< Graphic3d_TypeOfLightSource enumeration
  Standard_Boolean            myIsHeadlight; //!< flag to mark head light
  Standard_Boolean            myIsEnabled;   //!< enabled state

};

DEFINE_STANDARD_HANDLE(Graphic3d_CLight, Standard_Transient)

typedef NCollection_Shared< NCollection_List<Handle(Graphic3d_CLight)> > Graphic3d_ListOfCLight;

#endif // Graphic3d_CLight_HeaderFile

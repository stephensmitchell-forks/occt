// Created on: 2012-01-26
// Created by: Kirill GAVRILOV
// Copyright (c) 2012-2014 OPEN CASCADE SAS
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

#ifndef OpenGl_Context_HeaderFile
#define OpenGl_Context_HeaderFile

#include <Aspect_Handle.hxx>
#include <Aspect_Drawable.hxx>
#include <Aspect_Display.hxx>
#include <Aspect_RenderingContext.hxx>
#include <Handle_OpenGl_Context.hxx>
#include <Handle_OpenGl_ShaderManager.hxx>
#include <NCollection_DataMap.hxx>
#include <NCollection_Map.hxx>
#include <NCollection_Handle.hxx>
#include <NCollection_List.hxx>
#include <Message.hxx>
#include <OpenGl_Caps.hxx>
#include <OpenGl_Resource.hxx>
#include <Standard_Transient.hxx>
#include <TCollection_AsciiString.hxx>
#include <Handle_OpenGl_Context.hxx>
#include <OpenGl_Clipping.hxx>
#include <OpenGl_GlCore11.hxx>

//! Forward declarations
struct OpenGl_GlFunctions;
struct OpenGl_ArbTBO;
struct OpenGl_ArbIns;
struct OpenGl_ArbDbg;
struct OpenGl_ArbFBO;
struct OpenGl_ExtGS;

template<typename theBaseClass_t> struct OpenGl_TmplCore12;
typedef OpenGl_TmplCore12<OpenGl_GlCore11>     OpenGl_GlCore12;
typedef OpenGl_TmplCore12<OpenGl_GlCore11Fwd>  OpenGl_GlCore12Fwd;

struct OpenGl_GlCore13;
struct OpenGl_GlCore13Fwd;

template<typename theBaseClass_t> struct OpenGl_TmplCore14;
typedef OpenGl_TmplCore14<OpenGl_GlCore13>     OpenGl_GlCore14;
typedef OpenGl_TmplCore14<OpenGl_GlCore13Fwd>  OpenGl_GlCore14Fwd;

template<typename theBaseClass_t> struct OpenGl_TmplCore15;
typedef OpenGl_TmplCore15<OpenGl_GlCore14>     OpenGl_GlCore15;
typedef OpenGl_TmplCore15<OpenGl_GlCore14Fwd>  OpenGl_GlCore15Fwd;

template<typename theBaseClass_t> struct OpenGl_TmplCore20;
typedef OpenGl_TmplCore20<OpenGl_GlCore15>     OpenGl_GlCore20;
typedef OpenGl_TmplCore20<OpenGl_GlCore15Fwd>  OpenGl_GlCore20Fwd;

template<typename theBaseClass_t> struct OpenGl_TmplCore21;
typedef OpenGl_TmplCore21<OpenGl_GlCore20>     OpenGl_GlCore21;
typedef OpenGl_TmplCore21<OpenGl_GlCore20Fwd>  OpenGl_GlCore21Fwd;

template<typename theBaseClass_t> struct OpenGl_TmplCore30;
typedef OpenGl_TmplCore30<OpenGl_GlCore21>     OpenGl_GlCore30;
typedef OpenGl_TmplCore30<OpenGl_GlCore21Fwd>  OpenGl_GlCore30Fwd;

template<typename theBaseClass_t> struct OpenGl_TmplCore31;
typedef OpenGl_TmplCore31<OpenGl_GlCore30>     OpenGl_GlCore31Back;
typedef OpenGl_TmplCore31<OpenGl_GlCore30Fwd>  OpenGl_GlCore31;

template<typename theBaseClass_t> struct OpenGl_TmplCore32;
typedef OpenGl_TmplCore32<OpenGl_GlCore31Back> OpenGl_GlCore32Back;
typedef OpenGl_TmplCore32<OpenGl_GlCore31>     OpenGl_GlCore32;

template<typename theBaseClass_t> struct OpenGl_TmplCore33;
typedef OpenGl_TmplCore33<OpenGl_GlCore32Back> OpenGl_GlCore33Back;
typedef OpenGl_TmplCore33<OpenGl_GlCore32>     OpenGl_GlCore33;

template<typename theBaseClass_t> struct OpenGl_TmplCore40;
typedef OpenGl_TmplCore40<OpenGl_GlCore33Back> OpenGl_GlCore40Back;
typedef OpenGl_TmplCore40<OpenGl_GlCore33>     OpenGl_GlCore40;

template<typename theBaseClass_t> struct OpenGl_TmplCore41;
typedef OpenGl_TmplCore41<OpenGl_GlCore40Back> OpenGl_GlCore41Back;
typedef OpenGl_TmplCore41<OpenGl_GlCore40>     OpenGl_GlCore41;

template<typename theBaseClass_t> struct OpenGl_TmplCore42;
typedef OpenGl_TmplCore42<OpenGl_GlCore41Back> OpenGl_GlCore42Back;
typedef OpenGl_TmplCore42<OpenGl_GlCore41>     OpenGl_GlCore42;

template<typename theBaseClass_t> struct OpenGl_TmplCore43;
typedef OpenGl_TmplCore43<OpenGl_GlCore42Back> OpenGl_GlCore43Back;
typedef OpenGl_TmplCore43<OpenGl_GlCore42>     OpenGl_GlCore43;

template<typename theBaseClass_t> struct OpenGl_TmplCore44;
typedef OpenGl_TmplCore44<OpenGl_GlCore43Back> OpenGl_GlCore44Back;
typedef OpenGl_TmplCore44<OpenGl_GlCore43>     OpenGl_GlCore44;

//! This class generalize access to the GL context and available extensions.
//!
//! Functions are grouped into structures and accessed as fields.
//! You should check the group for NULL before usage (if group is not NULL
//! then all functions are available):
//! @code
//!   if (myContext->core20 != NULL)
//!   {
//!     myGlProgram = myContext->core20->glCreateProgram();
//!     .. do more stuff ..
//!   }
//!   else
//!   {
//!     .. compatibility with outdated configurations ..
//!   }
//! @endcode
//!
//! Current implementation provide access to OpenGL core functionality up to 2.0 version
//! (core12, core13, core14, core15, fields core20).
//! within several extensions (arbTBO, arbFBO, etc.).
//!
//! Simplified extensions classification:
//!  - prefixed with NV, AMD, ATI are vendor-specific (however may be provided by other vendors in some cases);
//!  - prefixed with EXT are accepted by 2+ vendors;
//!  - prefixed with ARB are accepted by Architecture Review Board and are candidates
//!    for inclusion into GL core functionality.
//! Some functionality can be represented in several extensions simultaneously.
//! In this case developer should be careful because different specification may differ
//! in aspects (like enumeration values and error-handling).
//!
//! Notice that some systems provide mechanisms to simultaneously incorporate with GL contexts
//! with different capabilities. Thats why OpenGl_Context should be initialized and used
//! for each GL context individually.
class OpenGl_Context : public Standard_Transient
{
public:

  //! Function for getting power of to number larger or equal to input number.
  //! @param theNumber    number to 'power of two'
  //! @param theThreshold upper threshold
  //! @return power of two number
  inline static Standard_Integer GetPowerOfTwo (const Standard_Integer theNumber,
                                                const Standard_Integer theThreshold)
  {
    for (Standard_Integer p2 = 2; p2 <= theThreshold; p2 <<= 1)
    {
      if (theNumber <= p2)
      {
        return p2;
      }
    }
    return theThreshold;
  }

public:

  //! Empty constructor. You should call Init() to perform initialization with bound GL context.
  Standard_EXPORT OpenGl_Context (const Handle(OpenGl_Caps)& theCaps = NULL);

  //! Destructor.
  Standard_EXPORT virtual ~OpenGl_Context();

  //! Share GL context resources.
  //! theShareCtx - handle to context to retrieve handles to shared resources.
  Standard_EXPORT void Share (const Handle(OpenGl_Context)& theShareCtx);

  //! Initialize class from currently bound OpenGL context. Method should be called only once.
  //! @return false if no GL context is bound to the current thread
  Standard_EXPORT Standard_Boolean Init();

  //! @return true if this context is valid (has been initialized)
  inline Standard_Boolean IsValid() const
  {
    return myIsInitialized;
  }

#if defined(_WIN32)
  //! Initialize class from specified window and rendering context. Method should be called only once.
  //! @return false if OpenGL context can not be bound to specified window
  Standard_EXPORT Standard_Boolean Init (const Aspect_Handle           theWindow,
                                         const Aspect_Handle           theWindowDC,
                                         const Aspect_RenderingContext theGContext);

  //! @return the window handle (HWND) currently bound to this OpenGL context
  inline Aspect_Handle Window() const
  {
    return myWindow;
  }

#elif defined(__APPLE__) && !defined(MACOSX_USE_GLX)
  //! Initialize class from specified OpenGL context (NSOpenGLContext). Method should be called only once.
  Standard_EXPORT Standard_Boolean Init (const void*                   theGContext);
#else
  //! Initialize class from specified window and rendering context. Method should be called only once.
  //! @return false if OpenGL context can not be bound to specified window
  Standard_EXPORT Standard_Boolean Init (const Aspect_Drawable         theWindow,
                                         const Aspect_Display          theDisplay,
                                         const Aspect_RenderingContext theGContext);

  //! @return the window handle (GLXDrawable) currently bound to this OpenGL context
  inline Aspect_Drawable Window() const
  {
    return myWindow;
  }
#endif

  //! Check if theExtName extension is supported by active GL context.
  Standard_EXPORT Standard_Boolean CheckExtension (const char* theExtName) const;

  //! Check if theExtName extension is in extensions string.
  Standard_EXPORT static Standard_Boolean CheckExtension (const char* theExtString,
                                                          const char* theExtName);

  //! Auxiliary template to retrieve GL function pointer.
  //! Pointer to function retrieved from library is statically casted
  //! to requested type - there no way to check real signature of exported function.
  //! The context should be bound before call.
  template <typename FuncType_t>
  Standard_Boolean FindProc (const char* theFuncName,
                             FuncType_t& theFuncPtr)
  {
    theFuncPtr = (FuncType_t )findProc (theFuncName);
    return (theFuncPtr != NULL);
  }

  //! @return true if detected GL version is greater or equal to requested one.
  inline Standard_Boolean IsGlGreaterEqual (const Standard_Integer theVerMajor,
                                            const Standard_Integer theVerMinor) const
  {
    return (myGlVerMajor >  theVerMajor)
        || (myGlVerMajor == theVerMajor && myGlVerMinor >= theVerMinor);
  }

  //! Clean up errors stack for this GL context (glGetError() in loop).
  Standard_EXPORT void ResetErrors();

  //! This method uses system-dependent API to retrieve information
  //! about GL context bound to the current thread.
  //! @return true if current thread is bound to this GL context
  Standard_EXPORT Standard_Boolean IsCurrent() const;

  //! Activates current context.
  //! Class should be initialized with appropriate info.
  Standard_EXPORT Standard_Boolean MakeCurrent();

  //! Swap front/back buffers for this GL context (should be activated before!).
  Standard_EXPORT void SwapBuffers();

  //! Return true if active mode is GL_RENDER (cached state)
  Standard_Boolean IsRender() const
  {
    return myRenderMode == GL_RENDER;
  }

  //! Return true if active mode is GL_FEEDBACK (cached state)
  Standard_Boolean IsFeedback() const
  {
    return myRenderMode == GL_FEEDBACK;
  }

  //! This function retrieves information from GL about free GPU memory that is:
  //!  - OS-dependent. On some OS it is per-process and on others - for entire system.
  //!  - Vendor-dependent. Currently available only on NVIDIA and AMD/ATi drivers only.
  //!  - Numbers meaning may vary.
  //! You should use this info only for diagnostics purposes.
  //! @return free GPU dedicated memory in bytes.
  Standard_EXPORT Standard_Size AvailableMemory() const;

  //! This function retrieves information from GL about GPU memory
  //! and contains more vendor-specific values than AvailableMemory().
  Standard_EXPORT TCollection_AsciiString MemoryInfo() const;

  //! Access shared resource by its name.
  //! @param  theKey - unique identifier;
  //! @return handle to shared resource or NULL.
  Standard_EXPORT const Handle(OpenGl_Resource)& GetResource (const TCollection_AsciiString& theKey) const;

  //! Access shared resource by its name.
  //! @param  theKey   - unique identifier;
  //! @param  theValue - handle to fill;
  //! @return true if resource was shared.
  template<typename TheHandleType>
  Standard_Boolean GetResource (const TCollection_AsciiString& theKey,
                                TheHandleType&                 theValue) const
  {
    const Handle(OpenGl_Resource)& aResource = GetResource (theKey);
    if (aResource.IsNull())
    {
      return Standard_False;
    }

    theValue = TheHandleType::DownCast (aResource);
    return !theValue.IsNull();
  }

  //! Register shared resource.
  //! Notice that after registration caller shouldn't release it by himself -
  //! it will be automatically released on context destruction.
  //! @param theKey      - unique identifier, shouldn't be empty;
  //! @param theResource - new resource to register, shouldn't be NULL.
  Standard_EXPORT Standard_Boolean ShareResource (const TCollection_AsciiString& theKey,
                                                  const Handle(OpenGl_Resource)& theResource);

  //! Release shared resource.
  //! If there are more than one reference to this resource
  //! (also used by some other existing object) then call will be ignored.
  //! This means that current object itself should nullify handle before this call.
  //! Notice that this is unrecommended operation at all and should be used
  //! only in case of fat resources to release memory for other needs.
  //! @param theKey     unique identifier
  //! @param theToDelay postpone release until next redraw call
  Standard_EXPORT void ReleaseResource (const TCollection_AsciiString& theKey,
                                        const Standard_Boolean         theToDelay = Standard_False);

  //! Append resource to queue for delayed clean up.
  //! Resources in this queue will be released at next redraw call.
  Standard_EXPORT void DelayedRelease (Handle(OpenGl_Resource)& theResource);

  //! Clean up the delayed release queue.
  Standard_EXPORT void ReleaseDelayed();

  //! @return tool for management of clippings within this context.
  inline OpenGl_Clipping& ChangeClipping() { return myClippingState; }

  //! @return tool for management of clippings within this context.
  inline const OpenGl_Clipping& Clipping() const { return myClippingState; }

  //! @return tool for management of shader programs within this context.
  inline const Handle(OpenGl_ShaderManager)& ShaderManager() const { return myShaderManager; }

public:

  //! @return maximum degree of anisotropy texture filter
  Standard_EXPORT Standard_Integer MaxDegreeOfAnisotropy() const;

  //! @return value for GL_MAX_TEXTURE_SIZE
  Standard_EXPORT Standard_Integer MaxTextureSize() const;

  //! Get maximum number of clip planes supported by OpenGl.
  //! This value is implementation dependent. At least 6
  //! planes should be supported by OpenGl (see specs).
  //! @return value for GL_MAX_CLIP_PLANES
  Standard_EXPORT Standard_Integer MaxClipPlanes() const;

public:

  //! @return messenger instance
  inline const Handle(Message_Messenger)& Messenger() const
  {
    return ::Message::DefaultMessenger();
  }

  //! Callback for GL_ARB_debug_output extension
  //! @param theSource   message source   within GL_DEBUG_SOURCE_   enumeration
  //! @param theType     message type     within GL_DEBUG_TYPE_     enumeration
  //! @param theId       message ID       within source
  //! @param theSeverity message severity within GL_DEBUG_SEVERITY_ enumeration
  //! @param theMessage  the message itself
  Standard_EXPORT void PushMessage (const unsigned int theSource,
                                    const unsigned int theType,
                                    const unsigned int theId,
                                    const unsigned int theSeverity,
                                    const TCollection_ExtendedString& theMessage);



  //! @return true if OpenGl context supports left and
  //! right rendering buffers.
  Standard_Boolean HasStereoBuffers() const
  {
    return myIsStereoBuffers;
  }

  //! Switch to left stereographic rendering buffer.
  //! This method can be used to keep unchanged choise
  //! of front/back/both buffer rendering.
  Standard_EXPORT void SetDrawBufferLeft();

  //! Switch to right stereographic rendering buffer.
  //! This method can be used to keep unchanged choise
  //! of front/back/both buffer rendering.
  Standard_EXPORT void SetDrawBufferRight();

  //! Switch to non-stereographic rendering buffer.
  //! This method can be used to keep unchanged choise
  //! of front/back/both buffer rendering.
  Standard_EXPORT void SetDrawBufferMono();

  //! Fetch OpenGl context state. This class tracks value of several OpenGl
  //! state variables. Consulting the cached values is quicker than
  //! doing the same via OpenGl API. Call this method if any of the controlled
  //! OpenGl state variables has a possibility of being out-of-date.
  Standard_EXPORT void FetchState();

private:

  //! Wrapper to system function to retrieve GL function pointer by name.
  Standard_EXPORT void* findProc (const char* theFuncName);

  //! Read OpenGL version information from active context.
  Standard_EXPORT void readGlVersion();

  //! Private initialization function that should be called only once.
  Standard_EXPORT void init();

public: //! @name core profiles

  OpenGl_GlCore11*     core11;     //!< OpenGL 1.1 core functionality
  OpenGl_GlCore11Fwd*  core11fwd;  //!< OpenGL 1.1 without deprecated entry points
  OpenGl_GlCore15*     core15;     //!< OpenGL 1.5 core functionality
  OpenGl_GlCore15Fwd*  core15fwd;  //!< OpenGL 1.5 without deprecated entry points
  OpenGl_GlCore20*     core20;     //!< OpenGL 2.0 core functionality (includes 1.5)
  OpenGl_GlCore20Fwd*  core20fwd;  //!< OpenGL 2.0 without deprecated entry points
  OpenGl_GlCore32*     core32;     //!< OpenGL 3.2 core profile
  OpenGl_GlCore32Back* core32back; //!< OpenGL 3.2 backward compatibility profile
  OpenGl_GlCore41*     core41;     //!< OpenGL 4.1 core profile
  OpenGl_GlCore41Back* core41back; //!< OpenGL 4.1 backward compatibility profile
  OpenGl_GlCore42*     core42;     //!< OpenGL 4.2 core profile
  OpenGl_GlCore42Back* core42back; //!< OpenGL 4.2 backward compatibility profile
  OpenGl_GlCore43*     core43;     //!< OpenGL 4.3 core profile
  OpenGl_GlCore43Back* core43back; //!< OpenGL 4.3 backward compatibility profile
  OpenGl_GlCore44*     core44;     //!< OpenGL 4.4 core profile
  OpenGl_GlCore44Back* core44back; //!< OpenGL 4.4 backward compatibility profile

  Handle(OpenGl_Caps) caps; //!< context options

public: //! @name extensions

  Standard_Boolean arbNPTW; //!< GL_ARB_texture_non_power_of_two
  Standard_Boolean arbTexRG;//!< GL_ARB_texture_rg
  OpenGl_ArbTBO*   arbTBO;  //!< GL_ARB_texture_buffer_object
  OpenGl_ArbIns*   arbIns;  //!< GL_ARB_draw_instanced
  OpenGl_ArbDbg*   arbDbg;  //!< GL_ARB_debug_output
  OpenGl_ArbFBO*   arbFBO;  //!< GL_ARB_framebuffer_object
  OpenGl_ExtGS*    extGS;   //!< GL_EXT_geometry_shader4
  Standard_Boolean extBgra; //!< GL_EXT_bgra
  Standard_Boolean extAnis; //!< GL_EXT_texture_filter_anisotropic
  Standard_Boolean extPDS;  //!< GL_EXT_packed_depth_stencil
  Standard_Boolean atiMem;  //!< GL_ATI_meminfo
  Standard_Boolean nvxMem;  //!< GL_NVX_gpu_memory_info

private: // system-dependent fields

#if defined(_WIN32)
  Aspect_Handle           myWindow;   //!< window handle (owner of GL context) : HWND
  Aspect_Handle           myWindowDC; //!< Device Descriptor handle : HDC
  Aspect_RenderingContext myGContext; //!< Rendering Context handle : HGLRC
#elif defined(__APPLE__) && !defined(MACOSX_USE_GLX)
  void*                   myGContext; //!< Rendering Context handle : NSOpenGLContext
#else
  Aspect_Drawable         myWindow;   //!< window handle (owner of GL context) : GLXDrawable
  Aspect_Display          myDisplay;  //!< connection to the X-server : Display*
  Aspect_RenderingContext myGContext; //!< X-GLX rendering context : GLXContext
#endif

private: // context info

  typedef NCollection_DataMap<TCollection_AsciiString, Standard_Integer> OpenGl_DelayReleaseMap;
  typedef NCollection_Handle<OpenGl_DelayReleaseMap> Handle(OpenGl_DelayReleaseMap);
  typedef NCollection_DataMap<TCollection_AsciiString, Handle(OpenGl_Resource)> OpenGl_ResourcesMap;
  typedef NCollection_Handle<OpenGl_ResourcesMap> Handle(OpenGl_ResourcesMap);
  typedef NCollection_List<Handle(OpenGl_Resource)> OpenGl_ResourcesStack;
  typedef NCollection_Handle<OpenGl_ResourcesStack> Handle(OpenGl_ResourcesStack);

  Handle(OpenGl_ResourcesMap)    mySharedResources; //!< shared resources with unique identification key
  Handle(OpenGl_DelayReleaseMap) myDelayed;         //!< shared resources for delayed release
  Handle(OpenGl_ResourcesStack)  myUnusedResources; //!< stack of resources for delayed clean up

  OpenGl_Clipping myClippingState; //!< state of clip planes

  void*            myGlLibHandle;     //!< optional handle to GL library
  NCollection_Handle<OpenGl_GlFunctions>
                   myFuncs;           //!< mega structure for all GL functions
  Standard_Integer myAnisoMax;        //!< maximum level of anisotropy texture filter
  Standard_Integer myMaxTexDim;       //!< value for GL_MAX_TEXTURE_SIZE
  Standard_Integer myMaxClipPlanes;   //!< value for GL_MAX_CLIP_PLANES
  Standard_Integer myGlVerMajor;      //!< cached GL version major number
  Standard_Integer myGlVerMinor;      //!< cached GL version minor number
  Standard_Integer myRenderMode;      //!< value for active rendering mode
  Standard_Boolean myIsInitialized;   //!< flag indicates initialization state
  Standard_Boolean myIsStereoBuffers; //!< context supports stereo buffering
  Standard_Integer myDrawBuffer;      //!< current draw buffer.

  Handle(OpenGl_ShaderManager) myShaderManager; //! support object for managing shader programs

private:

  //! Copying allowed only within Handles
  OpenGl_Context            (const OpenGl_Context& );
  OpenGl_Context& operator= (const OpenGl_Context& );

public:

  DEFINE_STANDARD_RTTI(OpenGl_Context) // Type definition

  friend class OpenGl_Window;

};

#endif // _OpenGl_Context_H__

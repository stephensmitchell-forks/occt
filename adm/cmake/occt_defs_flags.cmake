##

if(FLAGS_ALREADY_INCLUDED)
  return()
endif()
set(FLAGS_ALREADY_INCLUDED 1)

# this macro adds new flag to a cache variable is being connected with compiler
macro (ADD_CONTENT_IF_IT_ABSENT USER_VARIABLE USER_CONTENT)

  separate_arguments (USER_CONTENT)
  
  foreach (CURRENT_USER_CONTENT ${USER_CONTENT})
    if (NOT "${CURRENT_USER_CONTENT}" STREQUAL "")
      set (HAS_CONTENT OFF)
      foreach (COMPILER_OPTION_SPECIFICATOR "/" "-") # there are two compiler option specificators / (msvc) and - (msvc and others)
        set (FULL_USER_CONTENT "${COMPILER_OPTION_SPECIFICATOR}${CURRENT_USER_CONTENT}")
        string (REGEX MATCH "${FULL_USER_CONTENT}" HAS_CURRENT_CONTENT "${${USER_VARIABLE}}")
        if (HAS_CURRENT_CONTENT)
          set (HAS_CONTENT ON)
        endif()
      endforeach()
      
      if (NOT HAS_CONTENT)
        if (MSVC)
          set (FULL_USER_CONTENT "/${CURRENT_USER_CONTENT}")
        else()
          set (FULL_USER_CONTENT "-${CURRENT_USER_CONTENT}")
        endif()
        
        message (STATUS "Info: ${FULL_USER_CONTENT} added to ${USER_VARIABLE}")
        get_property(USER_VARIABLE_DOCSTRING CACHE ${USER_VARIABLE} PROPERTY HELPSTRING)
        set (${USER_VARIABLE} "${${USER_VARIABLE}} ${FULL_USER_CONTENT}" CACHE STRING "${USER_VARIABLE_DOCSTRING}" FORCE)
      endif()
    endforeach()
    endif()
endmacro()

# this macro replaces a specific flag with new one in a cache variable is being connected with compiler
macro (REPLACE_CONTENT_IF_IT_IS USER_VARIABLE USER_CONTENT NEW_USER_CONTENT)
  foreach (COMPILER_OPTION_SPECIFICATOR "/" "-") # there are two compiler option specificators / (msvc) and - (msvc and others)
    set (FULL_USER_CONTENT "${COMPILER_OPTION_SPECIFICATOR}${USER_CONTENT}")   
    string (REGEX MATCH "${FULL_USER_CONTENT}" HAS_CONTENT "${${USER_VARIABLE}}")
    if (HAS_CONTENT)
      if ("${NEW_USER_CONTENT}" STREQUAL "")
        message (STATUS "Info: ${FULL_USER_CONTENT} removed from ${USER_VARIABLE}")
        set (FULL_NEW_USER_CONTENT "${NEW_USER_CONTENT}")
      else()
        set (FULL_NEW_USER_CONTENT "${COMPILER_OPTION_SPECIFICATOR}${NEW_USER_CONTENT}")
        message (STATUS "Info: ${FULL_USER_CONTENT} replaced with ${FULL_NEW_USER_CONTENT} in ${USER_VARIABLE}")
      endif()
      
      string (REGEX REPLACE "${FULL_USER_CONTENT}" "${FULL_NEW_USER_CONTENT}" ${USER_VARIABLE} "${${USER_VARIABLE}}")
      
      get_property(USER_VARIABLE_DOCSTRING CACHE ${USER_VARIABLE} PROPERTY HELPSTRING)
      set (${USER_VARIABLE} "${${USER_VARIABLE}}" CACHE STRING "${USER_VARIABLE_DOCSTRING}" FORCE)
    endif()
  endforeach()
endmacro()

# force option -fp:precise for Visual Studio projects.
#
# Note that while this option is default for MSVC compiler, Visual Studio
# project can be switched later to use Intel Compiler (ICC).
# Enforcing -fp:precise ensures that in such case ICC will use correct
# option instead of its default -fp:fast which is harmful for OCCT.
if (MSVC)
  ADD_CONTENT_IF_IT_ABSENT (CMAKE_CXX_FLAGS "fp:precise")
  ADD_CONTENT_IF_IT_ABSENT (CMAKE_C_FLAGS   "fp:precise")
endif()

# set compiler short name and choose SSE2 option for appropriate MSVC compilers
# ONLY for 32-bit
if (NOT CMAKE_SIZEOF_VOID_P EQUAL 8)
  if (MSVC80 OR MSVC90 OR MSVC10)
    ADD_CONTENT_IF_IT_ABSENT (CMAKE_CXX_FLAGS "arch:SSE2")
    ADD_CONTENT_IF_IT_ABSENT (CMAKE_C_FLAGS "arch:SSE2")
  endif()
endif()

if (WIN32)
  add_definitions (-D_CRT_SECURE_NO_WARNINGS -D_CRT_NONSTDC_NO_DEPRECATE)
else()
  ADD_CONTENT_IF_IT_ABSENT (CMAKE_CXX_FLAGS "fPIC fexceptions")
  ADD_CONTENT_IF_IT_ABSENT (CMAKE_C_FLAGS   "fPIC fexceptions")
  
  add_definitions(-DOCC_CONVERT_SIGNALS)
endif()

# enable structured exceptions for MSVC
REPLACE_CONTENT_IF_IT_IS (CMAKE_CXX_FLAGS "EHsc" "EHa")
if (MSVC)
  ADD_CONTENT_IF_IT_ABSENT (CMAKE_CXX_FLAGS "EHa")
endif()

# remove _WINDOWS, WIN32 flag if they are
REPLACE_CONTENT_IF_IT_IS (CMAKE_CXX_FLAGS "D_WINDOWS" "")
REPLACE_CONTENT_IF_IT_IS (CMAKE_CXX_FLAGS "DWIN32" "")

REPLACE_CONTENT_IF_IT_IS (CMAKE_C_FLAGS "D_WINDOWS" "")
REPLACE_CONTENT_IF_IT_IS (CMAKE_C_FLAGS "DWIN32" "")

# remove DEBUG flag if it exists
REPLACE_CONTENT_IF_IT_IS (CMAKE_CXX_FLAGS_DEBUG "DDEBUG" "")
REPLACE_CONTENT_IF_IT_IS (CMAKE_C_FLAGS_DEBUG "DDEBUG" "")

# enable parallel compilation on MSVC 9 and above

if (MSVC AND NOT MSVC70 AND NOT MSVC80)
  ADD_CONTENT_IF_IT_ABSENT (CMAKE_CXX_FLAGS "MP")
endif()

# generate a single response file which enlist all of the object files
SET(CMAKE_C_USE_RESPONSE_FILE_FOR_OBJECTS 1)
SET(CMAKE_CXX_USE_RESPONSE_FILE_FOR_OBJECTS 1)

# increase compiler warnings level (-W4 for MSVC, -Wextra for GCC)
if (MSVC)
  REPLACE_CONTENT_IF_IT_IS (CMAKE_CXX_FLAGS "W[0-3]" "")
  ADD_CONTENT_IF_IT_ABSENT (CMAKE_CXX_FLAGS "W4")
elseif (CMAKE_COMPILER_IS_GNUCC OR CMAKE_COMPILER_IS_GNUCXX OR "${CMAKE_CXX_COMPILER_ID}" STREQUAL "Clang")
  ADD_CONTENT_IF_IT_ABSENT (CMAKE_CXX_FLAGS "Wextra Wall")
  if (BUILD_SHARED_LIBS)
    if(NOT WIN32)
      ADD_CONTENT_IF_IT_ABSENT (CMAKE_SHARED_LINKER_FLAGS "lm")
    endif()
  endif()
endif()

if(MINGW)
  # Set default release optimization option to O2 instead of O3, since in
  # some OCCT related examples, this gives significantly smaller binaries
  # at comparable performace with MinGW-w64.
  REPLACE_CONTENT_IF_IT_IS (CMAKE_CXX_FLAGS_RELEASE "O3" "O2")
  ADD_CONTENT_IF_IT_ABSENT (CMAKE_CXX_FLAGS_RELEASE "O2")

  ADD_CONTENT_IF_IT_ABSENT (CMAKE_CXX_FLAGS "std=gnu++0x")

  add_definitions(-D_WIN32_WINNT=0x0501)

  # workaround bugs in mingw with vtable export
  ADD_CONTENT_IF_IT_ABSENT (CMAKE_SHARED_LINKER_FLAGS "Wl,--export-all-symbols")

elseif (DEFINED CMAKE_COMPILER_IS_GNUCXX OR "x${CMAKE_CXX_COMPILER_ID}" STREQUAL "xClang")
  ADD_CONTENT_IF_IT_ABSENT (CMAKE_CXX_FLAGS "std=c++0x")
endif()

# Optimize size of binaries
if (CMAKE_COMPILER_IS_GNUCC OR CMAKE_COMPILER_IS_GNUCXX OR MINGW)
  ADD_CONTENT_IF_IT_ABSENT (CMAKE_CXX_FLAGS_RELEASE "s")
  ADD_CONTENT_IF_IT_ABSENT (CMAKE_C_FLAGS_RELEASE "s")
endif()

ADD_CONTENT_IF_IT_ABSENT (CMAKE_CXX_FLAGS_RELEASE "DNo_Exception")
ADD_CONTENT_IF_IT_ABSENT (CMAKE_C_FLAGS_RELEASE   "DNo_Exception")

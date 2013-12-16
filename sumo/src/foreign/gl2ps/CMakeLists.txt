#  GL2PS, an OpenGL to PostScript Printing Library
#  Copyright (C) 1999-2012 C. Geuzaine
# 
#  This program is free software; you can redistribute it and/or
#  modify it under the terms of either:
# 
#  a) the GNU Library General Public License as published by the Free
#  Software Foundation, either version 2 of the License, or (at your
#  option) any later version; or
# 
#  b) the GL2PS License as published by Christophe Geuzaine, either
#  version 2 of the License, or (at your option) any later version.
# 
#  This program is distributed in the hope that it will be useful, but
#  WITHOUT ANY WARRANTY; without even the implied warranty of
#  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See either
#  the GNU Library General Public License or the GL2PS License for
#  more details.
# 
#  You should have received a copy of the GNU Library General Public
#  License along with this library in the file named "COPYING.LGPL";
#  if not, write to the Free Software Foundation, Inc., 675 Mass Ave,
#  Cambridge, MA 02139, USA.
# 
#  You should have received a copy of the GL2PS License with this
#  library in the file named "COPYING.GL2PS"; if not, I will be glad
#  to provide one.
# 
#  For the latest info about gl2ps and a full list of contributors,
#  see http://www.geuz.org/gl2ps/.
# 
#  Please report all bugs and problems to <gl2ps@geuz.org>.

cmake_minimum_required(VERSION 2.4 FATAL_ERROR)

# if CMAKE_BUILD_TYPE is specified use it; otherwise set the default
# build type to "RelWithDebInfo" ("-O2 -g" with gcc) prior to calling
# project()
if(DEFINED CMAKE_BUILD_TYPE)
  set(CMAKE_BUILD_TYPE ${CMAKE_BUILD_TYPE} CACHE STRING "Choose build type")
else(DEFINED CMAKE_BUILD_TYPE)
  set(CMAKE_BUILD_TYPE RelWithDebInfo CACHE STRING "Choose build type")
endif(DEFINED CMAKE_BUILD_TYPE)

project(gl2ps C)

option(ENABLE_ZLIB "Enable compression using ZLIB" ON)
option(ENABLE_PNG "Enable PNG support" ON)

set(GL2PS_MAJOR_VERSION 1)
set(GL2PS_MINOR_VERSION 3)
set(GL2PS_PATCH_VERSION 8)
set(GL2PS_EXTRA_VERSION "" CACHE STRING "GL2PS extra version string")

set(GL2PS_VERSION "${GL2PS_MAJOR_VERSION}.${GL2PS_MINOR_VERSION}")
set(GL2PS_VERSION "${GL2PS_VERSION}.${GL2PS_PATCH_VERSION}${GL2PS_EXTRA_VERSION}")

execute_process(COMMAND date "+%Y%m%d" OUTPUT_VARIABLE DATE 
                OUTPUT_STRIP_TRAILING_WHITESPACE)
if(NOT DATE)
  set(DATE "unknown")
endif(NOT DATE)
set(GL2PS_DATE "${DATE}")

if(APPLE)
  set(GL2PS_OS "MacOSX")
elseif(CYGWIN)
  set(GL2PS_OS "Windows")
else(APPLE)
  set(GL2PS_OS "${CMAKE_SYSTEM_NAME}")
endif(APPLE)

include(CheckFunctionExists)
check_function_exists(vsnprintf HAVE_VSNPRINTF)
if(NOT HAVE_VSNPRINTF)
  add_definitions(-DHAVE_NO_VSNPRINTF)
endif(NOT HAVE_VSNPRINTF)

find_package(OpenGL)
if(OPENGL_FOUND)
  list(APPEND EXTERNAL_INCLUDES ${OPENGL_INCLUDE_DIR})
  list(APPEND EXTERNAL_LIBRARIES ${OPENGL_LIBRARIES})
endif(OPENGL_FOUND)

find_package(GLUT)
if(GLUT_FOUND)
  list(APPEND EXTERNAL_INCLUDES ${GLUT_INCLUDE_DIR})
  list(APPEND EXTERNAL_LIBRARIES ${GLUT_LIBRARIES})
endif(GLUT_FOUND)

if(ENABLE_ZLIB)
  find_package(ZLIB)
  if(ZLIB_FOUND)
    add_definitions(-DHAVE_ZLIB)
    list(APPEND EXTERNAL_INCLUDES ${ZLIB_INCLUDE_DIR})
    list(APPEND EXTERNAL_LIBRARIES ${ZLIB_LIBRARIES})
  endif(ZLIB_FOUND)
endif(ENABLE_ZLIB)

if(ENABLE_PNG)
  find_package(PNG)
  if(PNG_FOUND)
    add_definitions(-DHAVE_PNG)
    list(APPEND EXTERNAL_LIBRARIES ${PNG_LIBRARIES})
    list(APPEND EXTERNAL_INCLUDES ${PNG_INCLUDE_DIR})
  endif(PNG_FOUND)
endif(ENABLE_PNG)

include_directories(${EXTERNAL_INCLUDES})

if(OPENGL_FOUND)
  add_library(lib STATIC gl2ps.c gl2ps.h)
  set_target_properties(lib PROPERTIES OUTPUT_NAME gl2ps)

  add_library(shared SHARED gl2ps.c gl2ps.h)
  target_link_libraries(shared ${EXTERNAL_LIBRARIES})
  set_target_properties(shared PROPERTIES OUTPUT_NAME gl2ps)
  set_target_properties(shared PROPERTIES
                        VERSION ${GL2PS_MAJOR_VERSION}.${GL2PS_MINOR_VERSION}.${GL2PS_PATCH_VERSION}
                        SOVERSION ${GL2PS_MAJOR_VERSION})
  if(MSVC)
    set_target_properties(shared PROPERTIES COMPILE_FLAGS "-DGL2PSDLL -DGL2PSDLL_EXPORTS")
  endif(MSVC)

  install(TARGETS lib shared DESTINATION lib${LIB_SUFFIX})
endif(OPENGL_FOUND)

if(WIN32)
  set(GL2PS_DOC .)
else(WIN32)
  set(GL2PS_DOC share/doc/gl2ps)
endif(WIN32)

install(FILES gl2ps.h DESTINATION include)
install(FILES ${CMAKE_SOURCE_DIR}/README.txt DESTINATION ${GL2PS_DOC})
install(FILES ${CMAKE_SOURCE_DIR}/COPYING.LGPL DESTINATION ${GL2PS_DOC})
install(FILES ${CMAKE_SOURCE_DIR}/COPYING.GL2PS DESTINATION ${GL2PS_DOC})
install(FILES ${CMAKE_SOURCE_DIR}/gl2psTest.c DESTINATION ${GL2PS_DOC})
install(FILES ${CMAKE_SOURCE_DIR}/gl2psTestSimple.c DESTINATION ${GL2PS_DOC})

if(GLUT_FOUND)
  add_executable(gl2psTest WIN32 gl2psTest.c)
  target_link_libraries(gl2psTest lib ${EXTERNAL_LIBRARIES})
  add_executable(gl2psTestSimple WIN32 gl2psTestSimple.c)
  target_link_libraries(gl2psTestSimple lib ${EXTERNAL_LIBRARIES})
endif(GLUT_FOUND)

find_package(LATEX)
if(PDFLATEX_COMPILER)
  add_custom_command(OUTPUT gl2ps.pdf DEPENDS gl2ps.tex 
                     COMMAND ${PDFLATEX_COMPILER} ARGS ${CMAKE_SOURCE_DIR}/gl2ps.tex
                     COMMAND ${PDFLATEX_COMPILER} ARGS ${CMAKE_SOURCE_DIR}/gl2ps.tex
                     COMMAND ${PDFLATEX_COMPILER} ARGS ${CMAKE_SOURCE_DIR}/gl2ps.tex
                     COMMAND ${CMAKE_COMMAND} -E copy gl2ps.pdf ${CMAKE_SOURCE_DIR})
  add_custom_target(pdf ALL DEPENDS gl2ps.pdf)
  install(FILES gl2ps.pdf DESTINATION ${GL2PS_DOC})
  find_program(TTH tth)
  if(TTH)
    add_custom_command(OUTPUT gl2ps.html DEPENDS gl2ps.tex gl2ps.pdf
                       COMMAND ${CMAKE_COMMAND} -E copy_if_different 
                       ${CMAKE_SOURCE_DIR}/gl2ps.tex ${CMAKE_BINARY_DIR}/gl2ps.tex
                       COMMAND ${TTH} ARGS -w1 gl2ps.tex)
    add_custom_target(html DEPENDS gl2ps.html)
  endif(TTH)
endif(PDFLATEX_COMPILER)

set(CPACK_PACKAGE_VENDOR "Christophe Geuzaine")
set(CPACK_PACKAGE_VERSION_MAJOR ${GL2PS_MAJOR_VERSION})
set(CPACK_PACKAGE_VERSION_MINOR ${GL2PS_MINOR_VERSION})
set(CPACK_PACKAGE_VERSION_PATCH ${GL2PS_PATCH_VERSION})
set(CPACK_PACKAGE_DESCRIPTION_FILE ${CMAKE_SOURCE_DIR}/README.txt)
set(CPACK_PACKAGE_DESCRIPTION_SUMMARY 
    "An OpenGL to PostScript (and PDF, and SVG...) printing library")
set(CPACK_PACKAGE_FILE_NAME gl2ps-${GL2PS_VERSION}-${GL2PS_OS})
set(CPACK_PACKAGE_INSTALL_DIRECTORY "gl2ps")
set(CPACK_RESOURCE_FILE_LICENSE ${CMAKE_SOURCE_DIR}/COPYING.LGPL)
set(CPACK_RESOURCE_FILE_README ${CMAKE_SOURCE_DIR}/README.txt)
set(CPACK_RESOURCE_FILE_WELCOME ${CMAKE_SOURCE_DIR}/README.txt)
set(CPACK_PACKAGE_EXECUTABLE "gl2ps;gl2ps")
set(CPACK_STRIP_FILES TRUE)
set(CPACK_SOURCE_PACKAGE_FILE_NAME gl2ps-${GL2PS_VERSION}-source)
set(CPACK_SOURCE_GENERATOR TGZ)
set(CPACK_SOURCE_IGNORE_FILES "${CMAKE_BINARY_DIR}" "/CVS/" "/.svn" "~$" 
    "DS_Store$" "/tmp/" "/bin/" "/lib/")
if(WIN32)
  set(CPACK_GENERATOR ZIP)
else(WIN32)
  set(CPACK_GENERATOR TGZ)
endif(WIN32)

include(CPack)

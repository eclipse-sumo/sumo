# Source: https://github.com/qgis/QGIS/blob/3b3f6748f1e5e8f77fa87477add360ec0203fb26/cmake/FindProj.cmake
# Changes: included BSD license text

# Find Proj
# ~~~~~~~~~
# Copyright (c) 2007, Martin Dobias <wonder.sk at gmail.com>
# Redistribution and use is allowed according to the terms of the BSD license.
#
#Redistribution and use in source and binary forms, with or without
#modification, are permitted provided that the following conditions
#are met:
#
#1. Redistributions of source code must retain the copyright
#   notice, this list of conditions and the following disclaimer.
#2. Redistributions in binary form must reproduce the copyright
#   notice, this list of conditions and the following disclaimer in the
#   documentation and/or other materials provided with the distribution.
#3. The name of the author may not be used to endorse or promote products
#   derived from this software without specific prior written permission.
#
#THIS SOFTWARE IS PROVIDED BY THE AUTHOR ``AS IS'' AND ANY EXPRESS OR
#IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES
#OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE DISCLAIMED.
#IN NO EVENT SHALL THE AUTHOR BE LIABLE FOR ANY DIRECT, INDIRECT,
#INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT
#NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE,
#DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY
#THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
#(INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF
#THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.

# CMake module to search for Proj library
#
# If it's found it sets PROJ_FOUND to TRUE
# and following variables are set:
#    PROJ_INCLUDE_DIR
#    PROJ_API_FILE
#    PROJ_LIBRARY
#    PROJ_DATA_DIR (only if proj.db was found)

FIND_PATH(PROJ_INCLUDE_DIR NAMES proj.h proj_api.h PATHS
  "$ENV{INCLUDE}"
  "$ENV{LIB_DIR}/include"
  "$ENV{GDAL_DIR}/include"
  )

FIND_LIBRARY(PROJ_LIBRARY NAMES proj_i proj proj_6_1 PATHS
  "$ENV{LIB}"
  "$ENV{LIB_DIR}/lib"
  "$ENV{GDAL_DIR}/lib"
  )

IF (PROJ_INCLUDE_DIR AND PROJ_LIBRARY)
  SET(PROJ_FOUND TRUE)
  IF (EXISTS "${PROJ_INCLUDE_DIR}/proj.h")
    SET(PROJ_API_FILE "proj.h")
  ELSE ()
    SET(PROJ_API_FILE "proj_api.h")
  ENDIF ()
  FIND_PATH(PROJ_DATA_DIR NAMES proj.db PATHS "${PROJ_INCLUDE_DIR}/../share/proj")
  IF (NOT PROJ_DATA_DIR)
    # Ask projinfo for search path
    FIND_PROGRAM(PROJINFO_EXEC projinfo PATHS "${PROJ_INCLUDE_DIR}/../bin")
    IF (PROJINFO_EXEC)
      execute_process(COMMAND ${PROJINFO_EXEC} --searchpaths OUTPUT_VARIABLE PROJ_SEARCH_PATHS_RAW OUTPUT_STRIP_TRAILING_WHITESPACE)
      string(REPLACE "\n" ";" PROJ_SEARCH_PATHS_LIST "${PROJ_SEARCH_PATHS_RAW}")
      FIND_PATH(PROJ_DATA_DIR NAMES proj.db PATHS ${PROJ_SEARCH_PATHS_LIST})
    ENDIF ()
  ENDIF ()
  IF (PROJ_DATA_DIR)
    get_filename_component(PROJ_DATA_DIR "${PROJ_DATA_DIR}" REALPATH)
  ENDIF ()
  IF (NOT PROJ_FIND_QUIETLY)
    MESSAGE(STATUS "Found Proj: ${PROJ_LIBRARY} (data dir: ${PROJ_DATA_DIR})")
  ENDIF (NOT PROJ_FIND_QUIETLY)
ELSE ()
  SET(PROJ_FOUND FALSE)
  SET(PROJ_LIBRARY "")
  IF (PROJ_FIND_REQUIRED)
    MESSAGE(FATAL_ERROR "Could not find Proj")
  ELSE ()
    IF (NOT PROJ_FIND_QUIETLY)
      MESSAGE(STATUS "Could NOT find Proj")
    ENDIF (NOT PROJ_FIND_QUIETLY)
  ENDIF (PROJ_FIND_REQUIRED)
ENDIF ()

# find fox-config program
find_program(FOX_CONFIG fox-config)
# if was found, execute it and obtain the variables FOX_CXX_FLAGS and FOX_LIBRARY, needed for compilations
if(FOX_CONFIG)
    find_program(BASH bash)
    execute_process(COMMAND ${BASH} ${FOX_CONFIG} --cflags
        OUTPUT_VARIABLE FOX_CXX_FLAGS OUTPUT_STRIP_TRAILING_WHITESPACE)
    execute_process(COMMAND ${BASH} ${FOX_CONFIG} --libs
        OUTPUT_VARIABLE FOX_LIBRARY OUTPUT_STRIP_TRAILING_WHITESPACE)
    if(FOX_CXX_FLAGS MATCHES mingw)
        get_filename_component(root_dir "${FOX_CONFIG}" DIRECTORY)
        get_filename_component(root_dir "${root_dir}" DIRECTORY)
        string(REGEX REPLACE "/mingw../" "${root_dir}/" FOX_CXX_FLAGS "${FOX_CXX_FLAGS}")
        string(REGEX REPLACE "/mingw../" "${root_dir}/" FOX_LIBRARY "${FOX_LIBRARY}")
    endif()
endif(FOX_CONFIG)
    
# Declare ab boolean flag to note if Fox library was found
set(FOX_FOUND FALSE)

# if fox-config was executed successfully, fox was found
if(FOX_LIBRARY AND FOX_CXX_FLAGS)
    SET(FOX_FOUND TRUE)
else()
    # In Windows system, find the fox directory using as hint the environment variable "FOX_INCLUDE_DIR"
    FIND_PATH(FOX_INCLUDE_DIR NAMES fx.h HINTS $ENV{FOX_INCLUDE_DIR})
    # In Windows system, find the fox dll using as hint the environment variable "FOX_LIBRARY"
    FIND_LIBRARY(FOX_LIBRARY NAMES FOXDLL-1.6 HINTS $ENV{FOX_LIBRARY})
    # if both were found, set flag FOX_FOUND to true
    IF (FOX_INCLUDE_DIR AND FOX_LIBRARY)
        SET(FOX_FOUND TRUE)
    ELSE ()
        if (FOX_FIND_REQUIRED)
            message(FATAL_ERROR "Could NOT find Fox. GUI and threading will not be available. If it is installed, try to set the environment variables FOX_INCLUDE_DIR and FOX_LIBRARY.")
        endif()
        SET(FOX_LIBRARY "")
        message(STATUS "Could NOT find Fox. GUI and threading will not be available. If it is installed, try to set the environment variables FOX_INCLUDE_DIR and FOX_LIBRARY.")
    ENDIF (FOX_INCLUDE_DIR AND FOX_LIBRARY)
endif()
IF (NOT FOX_FIND_QUIETLY)
    message(STATUS "Found Fox: ${FOX_CXX_FLAGS} ${FOX_LIBRARY}")
endif()


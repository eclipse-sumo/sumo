# find fox-config program (usually in Linux)
find_program(FOX_CONFIG fox-config)
# if was found, execute it and obtain the variables FOX_CXX_FLAGS and FOX_LIBRARY_RELEASE, needed for compilations
if(FOX_CONFIG)
    find_program(BASH bash)
    execute_process(COMMAND ${BASH} ${FOX_CONFIG} --cflags
        OUTPUT_VARIABLE FOX_CXX_FLAGS OUTPUT_STRIP_TRAILING_WHITESPACE)
    execute_process(COMMAND ${BASH} ${FOX_CONFIG} --libs
        OUTPUT_VARIABLE FOX_LIBRARY_RELEASE OUTPUT_STRIP_TRAILING_WHITESPACE)
    if(FOX_CXX_FLAGS MATCHES mingw)
        get_filename_component(root_dir "${FOX_CONFIG}" DIRECTORY)
        get_filename_component(root_dir "${root_dir}" DIRECTORY)
        string(REGEX REPLACE "/mingw../" "${root_dir}/" FOX_CXX_FLAGS "${FOX_CXX_FLAGS}")
        string(REGEX REPLACE "/mingw../" "${root_dir}/" FOX_LIBRARY_RELEASE "${FOX_LIBRARY_RELEASE}")
    endif()
endif(FOX_CONFIG)

# Declare ab boolean flag to note if Fox library was found
set(FOX_FOUND FALSE)

# if fox-config was executed successfully, fox was found
if(FOX_LIBRARY_RELEASE AND FOX_CXX_FLAGS)
    SET(FOX_FOUND TRUE)
else()
    # In Windows system, find the fox directory using as hint the environment variable "FOX_INCLUDE_DIR" (usually SUMOLibraries)
    find_path(FOX_INCLUDE_DIR NAMES fx.h HINTS $ENV{FOX_INCLUDE_DIR})
    # In Windows system, find the fox dll using as hint the environment variable "FOX_LIBRARY_RELEASE"
    find_library(FOX_LIBRARY_DEBUG NAMES FOXDLL-1.6d FOX-16d HINTS $ENV{FOX_LIBRARY_DEBUG})
	find_library(FOX_LIBRARY_RELEASE NAMES FOXDLL-1.6 FOX-16 HINTS $ENV{FOX_LIBRARY_RELEASE})
    # if both were found, set flag FOX_FOUND to true
    if (FOX_INCLUDE_DIR AND FOX_LIBRARY_RELEASE)
        set(FOX_FOUND TRUE)
    else (FOX_INCLUDE_DIR AND FOX_LIBRARY_RELEASE)
        if (FOX_FIND_REQUIRED)
            message(FATAL_ERROR "Could NOT find Fox. GUI and threading will not be available. If it is installed, try to set the environment variables FOX_INCLUDE_DIR and FOX_LIBRARY_RELEASE.")
        endif(FOX_FIND_REQUIRED)
        set(FOX_LIBRARY_RELEASE "")
        message(STATUS "Could NOT find Fox. GUI and threading will not be available. If it is installed, try to set the environment variables FOX_INCLUDE_DIR and FOX_LIBRARY_RELEASE.")
    endif (FOX_INCLUDE_DIR AND FOX_LIBRARY_RELEASE)
endif(FOX_LIBRARY_RELEASE AND FOX_CXX_FLAGS)

# continue if fox was found
if(FOX_FOUND)
	# show found info
	if (NOT FOX_FIND_QUIETLY)
		message(STATUS "Found Fox: ${FOX_CXX_FLAGS} ${FOX_LIBRARY_RELEASE}")
	endif()
	# if FOX_LIBRARY was not set previously in FOX_CONFIG, set it manually
    if(NOT FOX_LIBRARY)
		# only use debug if it was previously found
		if (FOX_LIBRARY_DEBUG)
			set(FOX_LIBRARY "debug;${FOX_LIBRARY_DEBUG};optimized;${FOX_LIBRARY_RELEASE}")
		else (FOX_LIBRARY_DEBUG)
			set(FOX_LIBRARY "general;${FOX_LIBRARY_RELEASE}")
		endif (FOX_LIBRARY_DEBUG)
    endif(NOT FOX_LIBRARY)
endif()

# Start searching the fox-config.in file (used in linux distributions)
find_program(FOX_CONFIG fox-config)
# if was found, execute it and obtain the variables FOX_CXX_FLAGS and FOX_LIBRARY, needed for compilations
IF(FOX_CONFIG)
    exec_program(
        ${FOX_CONFIG}
        ARGS --cflags
        OUTPUT_VARIABLE FOX_CXX_FLAGS)
    exec_program(
        ${FOX_CONFIG}
        ARGS --libs
        OUTPUT_VARIABLE FOX_LIBRARY)
endif(FOX_CONFIG)

# Declare ab boolean flag to note if Fox library was found
set(FOX_FOUND 0)
# if Fox-config was executed sucesfully, fox was found
if(FOX_LIBRARY AND FOX_CXX_FLAGS)
    SET(FOX_FOUND TRUE)
else()
	# In Windows system, find the fox directory using as hint the enviroment variable "FOX_INCLUDE_DIR"
    FIND_PATH(FOX_INCLUDE_DIR 
			  NAMES fx.h
			  HINTS $ENV{FOX_INCLUDE_DIR}
			  )
	# In Windows system, find the fox dll using as hint the enviroment variable "FOX_LIBRARY"
    FIND_LIBRARY(FOX_LIBRARY NAMES 
				NAMES FOXDLL-1.6
				HINTS $ENV{FOX_LIBRARY}
				)
	# if both were found, set flag FOX_FOUND to true
    IF (FOX_INCLUDE_DIR AND FOX_LIBRARY)
        SET(FOX_FOUND TRUE)
    ENDIF (FOX_INCLUDE_DIR AND FOX_LIBRARY)
endif()

# if fox library wasn't found and it's requiered, stop cmake compilation and show error message
if(FOX_FIND_REQUIRED AND NOT FOX_FOUND)
	message(FATAL_ERROR "no fox. make sure that env variables FOX_INCLUDE_DIR and FOX_LIBRARY are set")
endif()


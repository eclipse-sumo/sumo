find_program(FOX_CONFIG fox-config)
   IF(FOX_CONFIG)
      exec_program(
         ${FOX_CONFIG}
         ARGS --cflags
         OUTPUT_VARIABLE FOX_CXX_FLAGS)
      exec_program(
         ${FOX_CONFIG}
         ARGS --libs
         OUTPUT_VARIABLE FOX_LIBRARIES)
endif(FOX_CONFIG)


set(FOX_FOUND 0)
if(FOX_LIBRARIES AND FOX_CXX_FLAGS)
    set(FOX_FOUND 1)
else()
	if(FOX_FIND_REQUIRED)
		message(FATAL_ERROR "no fox :-(")
	endif(FOX_FIND_REQUIRED)
endif()


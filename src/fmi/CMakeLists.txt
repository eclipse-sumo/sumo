
#########################################################
# Building of the libsumo-FMI2 library 
set (libsumofmi2_SRCS
  fmi2Functions.c 
  fmi2main.c 
  libsumocpp2c.cpp 
)

add_library(libsumofmi2 STATIC ${libsumofmi2_SRCS})
set_target_properties(libsumofmi2 PROPERTIES PREFIX "")
target_link_libraries(libsumofmi2 libsumocpp)

##########################################################
# Building of a test binary for the libsumo-FMI2 library
add_executable(testlibsumofmi2 testlibsumofmi2.c)
set_target_properties(testlibsumofmi2 PROPERTIES OUTPUT_NAME testlibsumofmi2${BINARY_SUFFIX})
set_target_properties(testlibsumofmi2 PROPERTIES OUTPUT_NAME_DEBUG testlibsumofmi2${BINARY_SUFFIX}D)
target_link_libraries(testlibsumofmi2 libsumofmi2)

#########################################################
# Building of the ZIP Archive with the model description
# and the static library for the FMU
set(libsumofmi2_model_description modelDescription.xml)
set(libsumofmi2_lib_name sumo-fmi2.fmu)

add_custom_target(create_fmi2_zip 
     COMMAND ${CMAKE_COMMAND} -E copy ${CMAKE_CURRENT_SOURCE_DIR}/${libsumofmi2_model_description} .
     COMMAND ${CMAKE_COMMAND} -E tar "cf" "${CMAKE_SOURCE_DIR}/bin/${libsumofmi2_lib_name}" --format=zip 
          "${libsumofmi2_model_description}"
          "libsumofmi2${CMAKE_STATIC_LIBRARY_SUFFIX}"
)

add_dependencies(create_fmi2_zip libsumofmi2)

###########################################################
# Define the custom target FMI to have all of these files 
# built (also as part of ALL - but of course the happens
# only when FMI is enabled)
add_custom_target(fmi ALL)
add_dependencies(fmi libsumofmi2 testlibsumofmi2 create_fmi2_zip)

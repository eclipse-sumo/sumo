set(sumofmi2_SRCS
  main.c
  fmi2Functions.c
)

set(sumofmi2_model_description
   modelDescription.xml
)

set(sumofmi2_lib_name sumo-fmi2.fmu)

add_library(sumofmi2 SHARED ${sumofmi2_SRCS})

add_custom_target(create_fmi2_zip 
    COMMAND ${CMAKE_COMMAND} -E copy ${CMAKE_CURRENT_SOURCE_DIR}/${sumofmi2_model_description} .
    COMMAND ${CMAKE_COMMAND} -E tar "cfv" "${CMAKE_SOURCE_DIR}/bin/${sumofmi2_lib_name}" --format=zip "${sumofmi2_model_description}"
)
add_dependencies(create_fmi2_zip sumofmi2)

add_custom_target(fmi)
add_dependencies(fmi create_fmi2_zip)

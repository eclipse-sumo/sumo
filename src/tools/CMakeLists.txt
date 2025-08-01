set(emissionsDrivingCycle_SRCS
   emissionsDrivingCycle_main.cpp
   TrajectoriesHandler.cpp
   TrajectoriesHandler.h
   VTypesHandler.cpp
   VTypesHandler.h
)

add_executable(emissionsDrivingCycle ${emissionsDrivingCycle_SRCS})
set_target_properties(emissionsDrivingCycle PROPERTIES OUTPUT_NAME emissionsDrivingCycle${BINARY_SUFFIX})
set_target_properties(emissionsDrivingCycle PROPERTIES OUTPUT_NAME_DEBUG emissionsDrivingCycle${BINARY_SUFFIX}D)
target_link_libraries(emissionsDrivingCycle utils_vehicle utils_emissions foreign_phemlight foreign_phemlight_V5 ${commonlibs} ${TCMALLOC_LIBRARY})
add_dependencies(emissionsDrivingCycle generate-version-h install_dll)

add_executable(emissionsMap VTypesHandler.cpp VTypesHandler.h emissionsMap_main.cpp)
set_target_properties(emissionsMap PROPERTIES OUTPUT_NAME emissionsMap${BINARY_SUFFIX})
set_target_properties(emissionsMap PROPERTIES OUTPUT_NAME_DEBUG emissionsMap${BINARY_SUFFIX}D)
target_link_libraries(emissionsMap utils_emissions utils_vehicle foreign_phemlight foreign_phemlight_V5 ${commonlibs} ${TCMALLOC_LIBRARY})
if (GNU_COMPILER AND ${CMAKE_CXX_COMPILER_VERSION} VERSION_LESS "9.0" AND "cxx_std_17" IN_LIST CMAKE_CXX_COMPILE_FEATURES)
    target_link_libraries(emissionsMap stdc++fs)
endif()
add_dependencies(emissionsMap generate-version-h install_dll)

install(TARGETS emissionsDrivingCycle emissionsMap RUNTIME DESTINATION bin)

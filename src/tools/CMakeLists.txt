set(emissionsDrivingCycle_SRCS
   emissionsDrivingCycle_main.cpp
   TrajectoriesHandler.cpp
   TrajectoriesHandler.h
)

add_executable(emissionsDrivingCycle ${emissionsDrivingCycle_SRCS})
set_target_properties(emissionsDrivingCycle PROPERTIES OUTPUT_NAME_DEBUG emissionsDrivingCycleD)
target_link_libraries(emissionsDrivingCycle utils_emissions foreign_phemlight ${commonlibs})

add_executable(emissionsMap emissionsMap_main.cpp)
set_target_properties(emissionsMap PROPERTIES OUTPUT_NAME_DEBUG emissionsMapD)
target_link_libraries(emissionsMap utils_emissions foreign_phemlight ${commonlibs})

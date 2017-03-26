set(emissionsDrivingCycle_SRCS
   emissionsDrivingCycle_main.cpp
   TrajectoriesHandler.cpp
   TrajectoriesHandler.h
)

add_executable(emissionsDrivingCycle ${emissionsDrivingCycle_SRCS})

target_link_libraries(emissionsDrivingCycle utils_emissions foreign_phemlight ${commonlibs})

install(TARGETS emissionsDrivingCycle DESTINATION bin)


add_executable(emissionsMap emissionsMap_main.cpp)

target_link_libraries(emissionsMap utils_emissions foreign_phemlight ${commonlibs})

install(TARGETS emissionsMap DESTINATION bin)

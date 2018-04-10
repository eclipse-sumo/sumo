add_subdirectory(activities)
add_subdirectory(city)

set(activitygen_SRCS
   activitygen_main.cpp
   AGActivityGen.cpp
   AGActivityGen.h
   AGActivityGenHandler.cpp
   AGActivityGenHandler.h
   AGActivityTripWriter.cpp
   AGActivityTripWriter.h
   AGFrame.cpp
   AGFrame.h
)

add_executable(activitygen ${activitygen_SRCS})
set_target_properties(activitygen PROPERTIES OUTPUT_NAME_DEBUG activitygenD)
target_link_libraries(activitygen activitygen_activities activitygen_city router ${commonvehiclelibs})

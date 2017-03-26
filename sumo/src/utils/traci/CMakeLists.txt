set(utils_traci_STAT_SRCS
   TraCIAPI.cpp
   TraCIAPI.h
)

add_library(utils_traci STATIC ${utils_traci_STAT_SRCS})

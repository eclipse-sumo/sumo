SET(testmicrosim_STAT_SRCS
   MSEventControlTest.cpp
   MSCFModelTest.cpp
)

add_library(testmicrosim STATIC ${testmicrosim_STAT_SRCS})

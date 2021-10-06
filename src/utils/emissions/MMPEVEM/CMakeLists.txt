set(mmpevem_STAT_SRCS
  CharacteristicMap.h
  CharacteristicMap.cpp
  MMPEVEM.h
  MMPEVEM.cpp
)

add_library(mmpevem STATIC ${mmpevem_STAT_SRCS})
set_property(TARGET mmpevem PROPERTY PROJECT_LABEL "z_mmpevem")


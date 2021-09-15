set(vka_STAT_SRCS
  CharacteristicMap.h
  CharacteristicMap.cpp
  SimpleEvPowertrain.h
  SimpleEvPowertrain.cpp
)

add_library(vka STATIC ${vka_STAT_SRCS})
set_property(TARGET vka PROPERTY PROJECT_LABEL "z_vka")


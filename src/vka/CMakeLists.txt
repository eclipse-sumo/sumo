set(vka_STAT_SRCS
  Constants.hh
  Helper.hh
  Helper.cc
  CharacteristicMap.hh
  CharacteristicMap.cc
  SimpleEvPowertrain.hh
  SimpleEvPowertrain.cc
)

add_library(vka STATIC ${vka_STAT_SRCS})
set_property(TARGET vka PROPERTY PROJECT_LABEL "z_vka")


set(foreign_polyfonts_STAT_SRCS
   polyfonttypes.h
   polyfonts.c
   polyfonts.h
   pfPSansBold16.c
   pfPSansBold16.h
)

add_library(foreign_polyfonts STATIC ${foreign_polyfonts_STAT_SRCS})
set_property(TARGET foreign_polyfonts PROPERTY PROJECT_LABEL "z_foreign_polyfonts")

set(foreign_gl2ps_STAT_SRCS
   gl2ps.h
   gl2ps.c
)

add_library(foreign_gl2ps STATIC ${foreign_gl2ps_STAT_SRCS})
set_property(TARGET foreign_gl2ps PROPERTY PROJECT_LABEL "z_foreign_gl2ps")

message("entering gl2ps")

set(gl2ps_files
	gl2ps.h gl2ps.c)

add_library(gl2ps STATIC ${gl2ps_files})

message("entering polyfonts")

set(foreignpolyfonts_files
	polyfonttypes.h polyfonts.c polyfonts.h
	pfPSansBold16.c pfPSansBold16.h)

add_library(foreignpolyfonts STATIC ${foreignpolyfonts_files})

message("entering geom")

set(geom_files
	AbstractPoly.h  Boundary.cpp Boundary.h
	Bresenham.cpp Bresenham.h GeomConvHelper.cpp GeomConvHelper.h
	GeoConvHelper.cpp GeoConvHelper.h
	GeomHelper.cpp GeomHelper.h
	Helper_ConvexHull.cpp Helper_ConvexHull.h
	Position.cpp Position.h
	PositionVector.cpp PositionVector.h bezier.cpp bezier.h)

add_library(utgeom STATIC ${geom_files})

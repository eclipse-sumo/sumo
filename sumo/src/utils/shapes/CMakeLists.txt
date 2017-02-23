message("entering shapes")

set(shapes_files
	PointOfInterest.h
	Shape.cpp Shape.h
	Polygon.cpp Polygon.h
	ShapeHandler.cpp ShapeHandler.h
	ShapeContainer.cpp ShapeContainer.h)

add_library(utshapes STATIC ${shapes_files})

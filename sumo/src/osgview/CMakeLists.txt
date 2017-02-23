message("entering osgview")

set(osgview_files
	GUIOSGBoundingBoxCalculator.h
	GUIOSGBuilder.cpp GUIOSGBuilder.h
	GUIOSGView.cpp GUIOSGView.h)

add_library(osgview STATIC ${osgview_files})

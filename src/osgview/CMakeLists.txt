
########### next target ###############

SET(osgview_STAT_SRCS
   GUIOSGBoundingBoxCalculator.h
   GUIOSGBuilder.cpp
   GUIOSGBuilder.h
   GUIOSGView.cpp
   GUIOSGView.h
)

add_library(osgview STATIC ${osgview_STAT_SRCS})
set_property(TARGET osgview PROPERTY PROJECT_LABEL "z_osgview")


########### install files ###############




#original Makefile.am contents follow:

#noinst_LIBRARIES = libosgview.a
#
#libosgview_a_SOURCES = GUIOSGBoundingBoxCalculator.h \
#GUIOSGBuilder.cpp GUIOSGBuilder.h \
#GUIOSGView.cpp GUIOSGView.h

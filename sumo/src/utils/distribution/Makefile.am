noinst_LIBRARIES = libdistribution.a

EXTRA_DIST = Distribution.h

libdistribution_a_SOURCES = Distribution_MeanDev.cpp Distribution_MeanDev.h \
Distribution_Points.cpp Distribution_Points.h \
DistributionCont.cpp DistributionCont.h

AM_CPPFLAGS = -I$(top_srcdir) -I$(top_srcdir)/src \
-I$(XERCESINCLUDE) -I$(XERCESINCLUDE_PRIOR_1_7)

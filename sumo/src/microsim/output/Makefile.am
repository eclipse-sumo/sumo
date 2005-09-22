noinst_LIBRARIES = libmicrosimoutput.a


EXTRA_DIST =

libmicrosimoutput_a_SOURCES = \
MSApproachingVehiclesStates.h MSCrossSection.h \
MSCurrentHaltingDurationSumPerVehicle.h \
MSDensity.cpp MSDensity.h MSDetector2File.cpp MSDetector2File.h \
MSDetectorContainerWrapper.h MSDetectorContainerWrapperBase.h \
MSDetectorControl.cpp MSDetectorControl.h \
MSDetectorCounterContainerWrapper.h \
MSDetectorFileOutput.h \
MSDetectorHaltingContainerWrapper.h \
MSDetectorInterfaceCommon.h \
MSDetectorOccupancyCorrection.h \
MSDetectorTypedefs.h \
MSEDDetector.h MSEDDetectorInterface.h \
MSHaltDuration.h \
MSHaltingDurationSum.h \
MSInductLoop.cpp MSInductLoop.h \
MSJamLengthAheadOfTrafficLights.h \
MSJamLengthSum.h MSLDDetector.h \
MSLDDetectorInterface.h \
MSMaxJamLength.h MSMeanDetector.h \
MSNStartedHalts.h MSNVehicles.cpp MSNVehicles.h \
MSObserver.h MSOccupancyDegree.h MSPredicates.h \
MSQueueLengthAheadOfTrafficLights.h \
MSSpaceMeanSpeed.h MSSubject.h MSSumDetector.h \
MSTDDetector.h MSTDDetectorInterface.h \
MSXMLRawOut.cpp MSXMLRawOut.h

SUBDIRS = e1_detectors e2_detectors e3_detectors meandata

AM_CPPFLAGS = -I$(top_srcdir)  -I$(top_srcdir)/src  \
-I$(top_srcdir)/src/helpers \
-I$(XERCESINCLUDE) -I$(XERCESINCLUDE_PRIOR_1_7)






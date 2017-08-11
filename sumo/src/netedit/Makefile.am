EXTRA_DIST = netedit64.ico netedit.rc

XERCES_LIBS = -l$(LIB_XERCES)

if WITH_GUI
GUI_APPS = netedit
endif

if TRACI
TRACI_LIBS = ../traci-server/libtraciserver.a
endif

bin_PROGRAMS = $(GUI_APPS)

if WITH_GUI
netedit_SOURCES = netedit_main.cpp \
GNEApplicationWindow.h GNEApplicationWindow.cpp \
GNELoadThread.h GNELoadThread.cpp \
GNENet.h GNENet.cpp \
GNENetElement.h GNENetElement.cpp \
GNEEdge.h GNEEdge.cpp \
GNELane.h GNELane.cpp \
GNEInternalLane.h GNEInternalLane.cpp \
GNEConnection.h GNEConnection.cpp \
GNECrossing.h GNECrossing.cpp \
GNEJunction.h GNEJunction.cpp \
GNEPOI.h GNEPOI.cpp \
GNEPoly.h GNEPoly.cpp \
GNEProhibition.h GNEProhibition.cpp \
GNEViewNet.h GNEViewNet.cpp \
GNEViewParent.h GNEViewParent.cpp \
GNEDialog_About.h GNEDialog_About.cpp \
GNEDialog_Wizard.h GNEDialog_Wizard.cpp \
GNEDialog_AllowDisallow.h GNEDialog_AllowDisallow.cpp \
GNEDialog_FixAdditionalPositions.h GNEDialog_FixAdditionalPositions.cpp \
GNEAdditionalDialog.h GNEAdditionalDialog.cpp \
GNEChange.h GNEChange.cpp \
GNEChange_Junction.h GNEChange_Junction.cpp \
GNEChange_Edge.h GNEChange_Edge.cpp \
GNEChange_Crossing.h GNEChange_Crossing.cpp \
GNEChange_Lane.h GNEChange_Lane.cpp \
GNEChange_Connection.h GNEChange_Connection.cpp \
GNEChange_Attribute.h GNEChange_Attribute.cpp \
GNEChange_TLS.h GNEChange_TLS.cpp \
GNEChange_Selection.h GNEChange_Selection.cpp \
GNEChange_Additional.h GNEChange_Additional.cpp \
GNEChange_RerouterInterval.h GNEChange_RerouterInterval.cpp \
GNEChange_CalibratorItem.h GNEChange_CalibratorItem.cpp \
GNEChange_POI.h GNEChange_POI.cpp \
GNEChange_Poly.h GNEChange_Poly.cpp \
GNEUndoList.h GNEUndoList.cpp \
GNEAttributeCarrier.h GNEAttributeCarrier.cpp \
GNEFrame.h GNEFrame.cpp \
GNEInspectorFrame.h GNEInspectorFrame.cpp \
GNESelectorFrame.h GNESelectorFrame.cpp \
GNEConnectorFrame.h GNEConnectorFrame.cpp \
GNETLSEditorFrame.h GNETLSEditorFrame.cpp \
GNEAdditionalFrame.h GNEAdditionalFrame.cpp \
GNECrossingFrame.h GNECrossingFrame.cpp \
GNEDeleteFrame.h GNEDeleteFrame.cpp \
GNEPolygonFrame.h GNEPolygonFrame.cpp \
GNEAdditionalHandler.h GNEAdditionalHandler.cpp \
GNEAdditional.h GNEAdditional.cpp \
GNEShape.h GNEShape.cpp \
GNEChargingStation.h GNEChargingStation.cpp \
GNEStoppingPlace.h GNEStoppingPlace.cpp \
GNEBusStop.h GNEBusStop.cpp \
GNEContainerStop.h GNEContainerStop.cpp \
GNEDetector.h GNEDetector.cpp \
GNEDetectorE1.h GNEDetectorE1.cpp \
GNEDetectorE2.h GNEDetectorE2.cpp \
GNEDetectorE3.h GNEDetectorE3.cpp \
GNEDetectorEntry.h GNEDetectorEntry.cpp \
GNEDetectorExit.h GNEDetectorExit.cpp \
GNERerouter.h GNERerouter.cpp \
GNERerouterDialog.h GNERerouterDialog.cpp \
GNERerouterIntervalDialog.h GNERerouterIntervalDialog.cpp \
GNERouteProbe.h GNERouteProbe.cpp \
GNEVaporizer.h GNEVaporizer.cpp \
GNECalibrator.h GNECalibrator.cpp \
GNECalibratorFlow.h GNECalibratorFlow.cpp \
GNECalibratorVehicleType.h GNECalibratorVehicleType.cpp \
GNECalibratorRoute.h GNECalibratorRoute.cpp \
GNECalibratorDialog.h GNECalibratorDialog.cpp \
GNECalibratorFlowDialog.h GNECalibratorFlowDialog.cpp \
GNECalibratorRouteDialog.h GNECalibratorRouteDialog.cpp \
GNECalibratorVehicleTypeDialog.h GNECalibratorVehicleTypeDialog.cpp \
GNEVariableSpeedSign.h GNEVariableSpeedSign.cpp \
GNEVariableSpeedSignStep.h GNEVariableSpeedSignStep.cpp \
GNEVariableSpeedSignDialog.h GNEVariableSpeedSignDialog.cpp \
GNEClosingLaneReroute.h GNEClosingLaneReroute.cpp \
GNEClosingReroute.h GNEClosingReroute.cpp \
GNEDestProbReroute.h GNEDestProbReroute.cpp \
GNERouteProbReroute.h GNERouteProbReroute.cpp \
GNERerouterInterval.h GNERerouterInterval.cpp \
GNEReferenceCounter.h GNEEvent_NetworkLoaded.h

netedit_LDADD = ../netimport/libnetimport.a \
../netimport/vissim/libvissimimport.a \
../netimport/vissim/typeloader/libvissimtypeload.a \
../netimport/vissim/tempstructs/libvissimtmpstr.a \
../netbuild/libnetbuild.a \
../netwrite/libnetwrite.a \
../utils/distribution/libdistribution.a \
../foreign/eulerspiral/libeulerspiral.a \
../utils/gui/windows/libguiutilsWindows.a \
../utils/gui/globjects/libguiutilsglobjects.a \
../utils/gui/div/libguiutilsdiv.a \
../utils/gui/settings/libguiutilssettings.a \
../utils/gui/images/libguiutilsimages.a \
../utils/gui/tracker/libguiutilstracker.a \
../utils/foxtools/libfoxtools.a \
../utils/distribution/libdistribution.a \
../foreign/gl2ps/libgl2ps.a \
../foreign/polyfonts/libpolyfonts.a \
../foreign/eulerspiral/libeulerspiral.a \
../utils/geom/libgeom.a \
../utils/shapes/libshapes.a \
../utils/options/liboptions.a \
../utils/common/libcommon.a \
../utils/emissions/libemissions.a \
../utils/xml/libxml.a \
../utils/importio/libimportio.a \
../utils/iodevices/libiodevices.a \
../foreign/tcpip/libtcpip.a \
$(XERCES_LIBS) $(FFMPEG_LIBS) \
$(FOX_LDFLAGS) $(XERCES_LDFLAGS) $(PROJ_LDFLAGS) $(GDAL_LDFLAGS) $(X_LDFLAGS)
endif

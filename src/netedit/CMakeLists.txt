add_subdirectory(changes)

set(netedit_SRCS
   netedit_main.cpp
   GNEApplicationWindow.h
   GNEApplicationWindow.cpp
   GNELoadThread.h
   GNELoadThread.cpp
   GNENet.h
   GNENet.cpp
   GNEPOI.h
   GNEPOI.cpp
   GNEPoly.h
   GNEPoly.cpp
   GNEViewNet.h
   GNEViewNet.cpp
   GNEViewParent.h
   GNEViewParent.cpp
   GNEUndoList.h
   GNEUndoList.cpp
   GNEAttributeCarrier.h
   GNEAttributeCarrier.cpp
   GNEAdditionalHandler.h
   GNEAdditionalHandler.cpp
   GNEAdditional.h
   GNEAdditional.cpp
   GNEShape.h
   GNEShape.cpp
   GNEChargingStation.h
   GNEChargingStation.cpp
   GNEStoppingPlace.h
   GNEStoppingPlace.cpp
   GNEBusStop.h
   GNEBusStop.cpp
   GNEContainerStop.h
   GNEContainerStop.cpp
   GNEParkingArea.h
   GNEParkingArea.cpp
   GNEParkingSpace.h
   GNEParkingSpace.cpp
   GNEDetector.h
   GNEDetector.cpp
   GNEDetectorE1.h
   GNEDetectorE1.cpp
   GNEDetectorE2.h
   GNEDetectorE2.cpp
   GNEDetectorE3.h
   GNEDetectorE3.cpp
   GNEDetectorEntry.h
   GNEDetectorEntry.cpp
   GNEDetectorExit.h
   GNEDetectorExit.cpp
   GNERerouter.h
   GNERerouter.cpp
   GNERouteProbe.h
   GNERouteProbe.cpp
   GNEVaporizer.h
   GNEVaporizer.cpp
   GNECalibrator.h
   GNECalibrator.cpp
   GNECalibratorFlow.h
   GNECalibratorFlow.cpp
   GNECalibratorVehicleType.h
   GNECalibratorVehicleType.cpp
   GNECalibratorRoute.h
   GNECalibratorRoute.cpp
   GNEVariableSpeedSign.h
   GNEVariableSpeedSign.cpp
   GNEVariableSpeedSignStep.h
   GNEVariableSpeedSignStep.cpp
   GNEClosingLaneReroute.h
   GNEClosingLaneReroute.cpp
   GNEClosingReroute.h
   GNEClosingReroute.cpp
   GNEDestProbReroute.h
   GNEDestProbReroute.cpp
   GNERouteProbReroute.h
   GNERouteProbReroute.cpp
   GNERerouterInterval.h
   GNERerouterInterval.cpp
   GNEReferenceCounter.h
   GNEEvent_NetworkLoaded.h
)

add_executable(netedit ${netedit_SRCS} netedit.rc)
set_target_properties(netedit PROPERTIES OUTPUT_NAME_DEBUG neteditD)
target_link_libraries(netedit utils_gui_windows utils_gui_globjects utils_gui_div utils_gui_settings utils_gui_images utils_gui_tracker
        utils_foxtools foreign_eulerspiral netedit_changes netedit_frames netedit_dialogs netedit_netElements ${netconvertlibs} ${FOX_LIBRARY} ${GL2PS_LIBRARIES})
if (MSVC)
    set_target_properties(netedit PROPERTIES LINK_FLAGS_RELEASE "/SUBSYSTEM:WINDOWS /ENTRY:mainCRTStartup")
    target_link_libraries(netedit opengl32 glu32)
endif ()

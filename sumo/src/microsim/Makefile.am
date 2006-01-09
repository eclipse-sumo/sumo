noinst_LIBRARIES = libmicrosim.a

EXTRA_DIST =

libmicrosim_a_SOURCES = MSAbstractLaneChangeModel.h MSBitSetLogic.h \
MSDebugHelper.cpp MSDebugHelper.h \
MSCORN.cpp MSCORN.h MSEdge.cpp MSEdge.h \
MSEdgeContinuations.cpp MSEdgeContinuations.h \
MSEdgeControl.cpp MSEdgeControl.h \
MSEmitControl.cpp MSEmitControl.h MSEventControl.cpp MSEventControl.h \
MSGlobals.cpp MSGlobals.h MSInterface_NetRun.h \
MSInternalLane.cpp MSInternalLane.h MSJunction.cpp MSJunction.h \
MSJunctionControl.cpp MSJunctionControl.h \
MSJunctionLogic.cpp MSJunctionLogic.h MSLane.cpp MSLane.h MSLane.icc \
MSLaneChanger.cpp MSLaneChanger.h MSLaneChanger.icc \
MSLink.cpp MSLink.h MSLinkCont.cpp MSLinkCont.h \
MSLogicJunction.cpp MSLogicJunction.h MSMoveReminder.h \
MSNet.cpp MSNet.h MSNet.icc MSNoLogicJunction.cpp MSNoLogicJunction.h \
MSRightOfWayJunction.cpp MSRightOfWayJunction.h \
MSRoute.cpp MSRoute.h MSRouteHandler.cpp MSRouteHandler.h \
MSRouteLoader.cpp MSRouteLoader.h \
MSRouteLoaderControl.cpp MSRouteLoaderControl.h \
MSSaveState.h \
MSSlowLaneChanger.cpp MSSlowLaneChanger.h MSSlowLaneChanger.icc \
MSSourceLane.cpp MSSourceLane.h MSTravelcostDetector.h \
MSUnit.cpp MSUnit.h MSUpdateEachTimestep.h MSUpdateEachTimestepContainer.h \
MSVehicle.cpp MSVehicle.h MSVehicle.icc \
MSVehicleContainer.cpp MSVehicleContainer.h \
MSVehicleControl.cpp MSVehicleControl.h \
MSVehicleQuitReminded.h \
MSVehicleTransfer.cpp MSVehicleTransfer.h \
MSVehicleType.cpp MSVehicleType.h MSVehicleType.icc

SUBDIRS = actions devices lanechanging logging output traffic_lights trigger

AM_CPPFLAGS = -I$(top_srcdir)  -I$(top_srcdir)/src  \
-I$(top_srcdir)/src/helpers \
-I$(XERCES_INCLUDES) -I$(XERCES_INCLUDES_PRIOR_1_7)






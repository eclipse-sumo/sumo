noinst_LIBRARIES = libmicrosim.a


if CFRONT
TMP_CFRONT = MSBitSetLogic.cpp
else !CFRONT
EXTRA_DIST = MSBitSetLogic.cpp
endif !CFRONT



libmicrosim_a_SOURCES = MSLaneChanger.cpp MSRightOfWayJunction.cpp MSLogicJunction.cpp MSNoLogicJunction.cpp MSJunctionLogic.cpp MSJunction.cpp MSVehicleType.cpp MSVehicle.cpp MSPersonControl.cpp MSPerson.cpp MSNet.cpp MSLane.cpp MSEdge.cpp MSJunctionControl.cpp MSEventControl.cpp MSEmitControl.cpp MSEdgeControl.cpp MSBitSetLogic.h MSJunctionLogic.h MSPerson.h MSEdge.h MSLane.h MSPersonControl.h MSEdgeControl.h MSLaneChanger.h MSRightOfWayJunction.h MSEmitControl.h MSLogicJunction.h MSVehicle.h MSEventControl.h MSVehicleType.h MSJunction.h MSNet.h MSJunctionControl.h MSNoLogicJunction.h MSDetector.h MSInductLoop.cpp MSInductLoop.h MSLaneChanger.icc MSLane.icc MSNet.icc MSVehicle.icc MSVehicleType.icc $(TMP_CFRONT)


AM_CPPFLAGS = -I../helpers





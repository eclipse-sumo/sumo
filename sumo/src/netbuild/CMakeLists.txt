message("entering netbuild")

set(netbuild_files
        NBAlgorithms.cpp NBAlgorithms.h
        NBAlgorithms_Ramps.cpp NBAlgorithms_Ramps.h
        NBCapacity2Lanes.h
        NBConnection.cpp NBConnection.h
        NBConnectionDefs.h
        NBCont.h
        NBContHelper.cpp NBContHelper.h
        NBDistrict.cpp NBDistrict.h
        NBDistrictCont.cpp NBDistrictCont.h
        NBEdge.cpp NBEdge.h
        NBEdgeCont.cpp NBEdgeCont.h
        NBFrame.cpp NBFrame.h
        NBHeightMapper.cpp NBHeightMapper.h
        NBHelpers.cpp NBHelpers.h
        NBLinkPossibilityMatrix.h
        NBLoadedSUMOTLDef.cpp NBLoadedSUMOTLDef.h
        NBLoadedTLDef.cpp NBLoadedTLDef.h
        NBNetBuilder.cpp NBNetBuilder.h
        NBNode.cpp NBNode.cpp
        NBNodeCont.cpp NBNodeCont.h
        NBNodeShapeComputer.cpp NBNodeShapeComputer.h
        NBOwnTLDef.cpp NBOwnTLDef.h
        NBRequest.cpp NBRequest.h
        NBSign.cpp NBSign.h
        NBTrafficLightDefinition.cpp NBTrafficLightDefinition.h
        NBTrafficLightLogic.cpp NBTrafficLightLogic.h
        NBTrafficLightLogicCont.cpp NBTrafficLightLogicCont.h
        NBTypeCont.cpp NBTypeCont.h
        NBPTStopCont.cpp NBPTStopCont.h NBPTStop.cpp NBPTStop.h)

add_library(nb STATIC ${netbuild_files})